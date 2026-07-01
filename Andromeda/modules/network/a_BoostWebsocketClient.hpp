#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <cstdlib>
#include <string>
#include "a_network_info.hpp"
#include "a_Primitives.hpp"
#include "a_IWebsocketClient.hpp"
#include <unordered_map>
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace Andromeda {
	struct WebsocketReq {
		i32 Host;
		std::string target;
		std::vector<std::string> headers;
	};

	class BoostWebsocketClient : public IWebsocketClient {
	public:
		BoostWebsocketClient(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context)
			: m_IoContext(io_context), m_SslContext(ssl_context) {}
		void connect() override;
		void disconnect() override;
		void send(const std::string& payload) override;

		void onMessageReceived(std::function<void(const std::string&)> callback) override;
	private:
		boost::asio::io_context& m_IoContext;
		boost::asio::ssl::context& m_SslContext;
	};
}