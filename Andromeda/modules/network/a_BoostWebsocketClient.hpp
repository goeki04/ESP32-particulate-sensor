#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <string>
#include "a_network_info.hpp"
#include "a_IWebsocketClient.hpp"
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace Andromeda {

	class BoostWebsocketClient : public IWebsocketClient {
	public:
		BoostWebsocketClient(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context, WebsocketReq req,const std::optional<ProxySettings>& proxy_settings)
			: m_IoContext(io_context), m_SslContext(ssl_context), m_ProxySettings(proxy_settings), m_Req(req), m_Stream(net::make_strand(m_IoContext), m_SslContext) {}
		void connect() override;
		void disconnect() override;
		void send(const std::string& payload) override;

		void onMessageReceived(std::function<void(const std::string&)> callback) override;
	private:
		boost::asio::io_context& m_IoContext;
		boost::asio::ssl::context& m_SslContext;
		std::optional<ProxySettings> m_ProxySettings;
		WebsocketReq m_Req;
		websocket::stream<beast::ssl_stream<beast::tcp_stream>> m_Stream;
	};
}