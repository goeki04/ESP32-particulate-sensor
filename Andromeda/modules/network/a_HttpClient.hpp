#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/connect.hpp>
#include <vector>
#include <string>
#include "a_network_info.hpp"

struct HttpsRequest {
    std::string host;
    std::string path;
    std::string method = "GET";
    std::string body = "";
    std::vector<std::pair<std::string, std::string>> headers;
};

namespace Andromeda {
    class HttpsClient {
    public:
        HttpsClient(boost::asio::ssl::context& ssl_context)
            : m_SslContext(ssl_context) {}

        boost::asio::awaitable<std::string> sendRequest(const HttpsRequest& req) {
            auto proxySettings = NetworkInfo::getProxySettings();
            auto executor = co_await boost::asio::this_coro::executor;

            try {
                boost::asio::ip::tcp::resolver resolver(executor);

                boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(executor, m_SslContext);

                if (!SSL_set_tlsext_host_name(stream.native_handle(), req.host.c_str())) {
                    boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
                    A_ERROR("HttpClient: Failed to set SNI: {}", ec.message());
                    co_return "";
                }

                boost::asio::ip::tcp::resolver::results_type endpoints;
                bool useProxy = proxySettings.has_value() && !proxySettings->host.empty() && proxySettings->port > 0;

                if (useProxy) {
                    endpoints = co_await resolver.async_resolve(proxySettings->host, std::to_string(proxySettings->port), boost::asio::use_awaitable);
                }
                else {
                    endpoints = co_await resolver.async_resolve(req.host, "443", boost::asio::use_awaitable);
                }

                co_await boost::asio::async_connect(stream.lowest_layer(), endpoints, boost::asio::use_awaitable);

                if (useProxy) {
                    boost::beast::http::request<boost::beast::http::empty_body> tunnelReq{ boost::beast::http::verb::connect, req.host + ":443", 11 };
                    tunnelReq.set(boost::beast::http::field::host, req.host + ":443");

                    co_await boost::beast::http::async_write(stream.next_layer(), tunnelReq, boost::asio::use_awaitable);

                    boost::beast::flat_buffer tunnelBuffer;
                    boost::beast::http::response_parser<boost::beast::http::empty_body> tunnelParser;
                    tunnelParser.skip(true); // CONNECT-Antwort hat keinen Body -> nicht auf EOF warten
                    co_await boost::beast::http::async_read(stream.next_layer(), tunnelBuffer, tunnelParser, boost::asio::use_awaitable);

                    if (tunnelParser.get().result() != boost::beast::http::status::ok) {
                        A_ERROR("HttpClient: Failed to establish proxy tunnel. Status: {}", tunnelParser.get().result_int());
                        co_return "";
                    }
                }

                co_await stream.async_handshake(boost::asio::ssl::stream_base::client, boost::asio::use_awaitable);

                boost::beast::http::verb verb = boost::beast::http::string_to_verb(req.method);
                if (verb == boost::beast::http::verb::unknown) {
                    verb = boost::beast::http::verb::get;
                }

                boost::beast::http::request<boost::beast::http::string_body> httpReq{ verb, req.path, 11 };
                httpReq.set(boost::beast::http::field::host, req.host);
                httpReq.set(boost::beast::http::field::connection, "close");

                for (const auto& header : req.headers) {
                    httpReq.set(header.first, header.second);
                }

                if (!req.body.empty()) {
                    httpReq.body() = req.body;
                    httpReq.prepare_payload();
                }

                std::size_t bytesWritten = co_await boost::beast::http::async_write(stream, httpReq, boost::asio::use_awaitable);
                if (bytesWritten == 0) {
                    A_ERROR("HttpClient: Request not sent (0 Bytes written).");
                    co_return "";
                }

                boost::beast::flat_buffer buffer;
                boost::beast::http::response<boost::beast::http::string_body> httpRes;

                std::size_t bytesRead = co_await boost::beast::http::async_read(stream, buffer, httpRes, boost::asio::use_awaitable);
                if (bytesRead == 0) {
                    A_ERROR("HttpClient: No response read (0 Bytes) - read operation failed.");
                    co_return "";
                }
                try {
                    co_await stream.async_shutdown(boost::asio::use_awaitable);
                }
                catch (const boost::system::system_error& e) {
                    // Gegenseite schliesst bei "Connection: close" oft hart -> kein sauberer close_notify.
                    // Diese Codes sind harmlos, der Body wurde bereits vollstaendig gelesen.
                    if (e.code() != boost::asio::error::eof
                        && e.code() != boost::asio::ssl::error::stream_truncated
                        && e.code() != boost::asio::error::connection_aborted   // 10053 (WSAECONNABORTED)
                        && e.code() != boost::asio::error::connection_reset) {  // 10054 (WSAECONNRESET)
                        A_WARN("HttpClient shutdown warning: {}", e.what());
                    }
                }

                int statusCode = httpRes.result_int();
                A_INFO("HTTP Status Code: {}", statusCode);

                if (statusCode < 200 || statusCode >= 300) {
                    A_WARN("Server returned error code: {}", statusCode);
                    co_return "";
                }

                co_return httpRes.body();
            }
            catch (const std::exception& e) {
                A_ERROR("Universal Beast HttpClient failed: {}", e.what());
                co_return "";
            }
        }
    private:
        boost::asio::ssl::context& m_SslContext;
    };
}