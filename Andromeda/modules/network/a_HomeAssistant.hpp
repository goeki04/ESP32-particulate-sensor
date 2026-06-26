#pragma once
#include "a_Primitives.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>
namespace Andromeda {
	class HomeAssistantService {
	public:
		void init();
	private:
		boost::asio::io_context ioContext;
		std::string readHomeAssistantTokenFromSecretsJson();
		std::string dechunkBody(const std::string& chunkedBody);
		bool validateStatusCode(i32 statusCode);
	};
}