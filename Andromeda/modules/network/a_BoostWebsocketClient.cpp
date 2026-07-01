#include <boost/asio/co_spawn.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/asio/detached.hpp>
#include <chrono>
#include "a_logger.hpp"
#include "a_BoostWebsocketClient.hpp"
#include "a_network_info.hpp"
#include "a_Primitives.hpp"
namespace Andromeda
{
	void BoostWebsocketClient::connect() {
		net::co_spawn(m_IoContext, [this]() -> net::awaitable<void> {
			try {
				auto executor = co_await net::this_coro::executor;
				auto resolver = net::ip::tcp::resolver(executor);
				bool hasProxy = NetworkInfo::hasProxysettings(m_ProxySettings);
				beast::get_lowest_layer(m_Stream).expires_after(std::chrono::seconds(30));

				std::string host = hasProxy ? m_ProxySettings->host : m_Req.host;
				std::string port = std::to_string(hasProxy ? m_ProxySettings->port : m_Req.port);
				
				auto const results = co_await resolver.async_resolve(host, port, net::use_awaitable);
				co_await beast::get_lowest_layer(m_Stream).async_connect(results, net::use_awaitable); //implicit TCP handshake

				if (hasProxy) {
					http::request<http::empty_body> req{ http::verb::connect, m_Req.host + ":" + std::to_string(m_Req.port), 11 };
					req.set(http::field::host, m_Req.host);
					beast::get_lowest_layer(m_Stream).expires_after(std::chrono::seconds(30));
					co_await http::async_write(beast::get_lowest_layer(m_Stream), req, net::use_awaitable);

					beast::flat_buffer buffer;
					http::response<http::empty_body> res;
					beast::get_lowest_layer(m_Stream).expires_after(std::chrono::seconds(30));
					co_await http::async_read(beast::get_lowest_layer(m_Stream), buffer, res, net::use_awaitable);

					if (res.result() != http::status::ok) {
						throw std::runtime_error("Proxy tunnel failed: " + std::string(res.reason()));
					}
				}

				if (!SSL_set_tlsext_host_name(m_Stream.next_layer().native_handle(), m_Req.host.c_str())) {
					throw beast::system_error{ beast::error_code{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()} };
				}
				beast::get_lowest_layer(m_Stream).expires_after(std::chrono::seconds(30));
				co_await m_Stream.next_layer().async_handshake(net::ssl::stream_base::client, net::use_awaitable);	//SSL handshake
				std::string hostHeader = m_Req.host + ":" + std::to_string(m_Req.port);
				std::string path = m_Req.path.empty() ? "/" : m_Req.path;
				beast::get_lowest_layer(m_Stream).expires_never();
				m_Stream.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
				co_await m_Stream.async_handshake(hostHeader, path, net::use_awaitable); //WebSocket handshake

				A_INFO("Successfully connected to WebSocket: {}", m_Req.host);
			}
			catch (const std::exception& e) {
				A_ERROR("Connection failed: {}", e.what());
			}
		}, net::detached);
	}
	void BoostWebsocketClient::disconnect() {

	}
	void BoostWebsocketClient::send(const std::string& payload) {

	}

	void BoostWebsocketClient::onMessageReceived(std::function<void(const std::string&)> callback) {
	
	}
}