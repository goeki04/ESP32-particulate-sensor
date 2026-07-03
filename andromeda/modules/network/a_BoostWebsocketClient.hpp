#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <atomic>
#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/awaitable.hpp>
#include <memory>
#include <string>
#include "a_network_info.hpp"
#include "a_IWebsocketClient.hpp"
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace Andromeda {

	class BoostWebsocketClient : public IWebsocketClient, public std::enable_shared_from_this<BoostWebsocketClient> {
	public:
		BoostWebsocketClient(std::shared_ptr<boost::asio::io_context> io_context, boost::asio::ssl::context& ssl_context, WebsocketReq req,const std::optional<ProxySettings>& proxy_settings)
			: m_IoContext(io_context), m_SslContext(ssl_context), m_ProxySettings(proxy_settings), m_Req(req), m_Stream(net::make_strand(*m_IoContext), m_SslContext) {}
		void connect() override;
		void disconnect() override;
		void send(const std::string& payload) override;
		bool isConnected() const override { return m_IsConnected.load(std::memory_order_acquire); }
		void onMessageReceived(std::function<void(const std::string&)> callback) override;
	private:
		std::shared_ptr<boost::asio::io_context> m_IoContext;
		boost::asio::ssl::context& m_SslContext;
		std::atomic<bool> m_IsConnected{ false };
		std::optional<ProxySettings> m_ProxySettings;
		WebsocketReq m_Req;
		websocket::stream<beast::ssl_stream<beast::tcp_stream>> m_Stream;

		std::function<void(const std::string&)> m_MessageCallback;
		void startReadLoop();
		static net::awaitable<void> doConnect(std::shared_ptr<BoostWebsocketClient> self);
		static net::awaitable<void> doDisconnect(std::shared_ptr<BoostWebsocketClient> self);
		static net::awaitable<void> doSend(std::shared_ptr<BoostWebsocketClient> self, std::string payload);
		static net::awaitable<void> doReadLoop(std::shared_ptr<BoostWebsocketClient> self);
	};
}