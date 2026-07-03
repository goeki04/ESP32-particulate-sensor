#pragma once
#include "a_Primitives.hpp"
#include "a_IWebsocketClient.hpp"
#include <string>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include "a_network_info.hpp"
#include <optional>
#include <mutex>
#include <memory>
#include <vector>
namespace Andromeda {
	class HomeAssistantService {
	public:
		HomeAssistantService(std::shared_ptr<boost::asio::io_context> ioContext,
			boost::asio::ssl::context& sslContext,
			std::optional<ProxySettings> proxySettings)
			: m_IoContext(std::move(ioContext)), m_SslContext(sslContext), m_ProxySettings(std::move(proxySettings)) {}

		void init();
		void update();
		std::string readHomeAssistantTokenFromSecretsJson();
	private:
		std::shared_ptr<boost::asio::io_context> m_IoContext;
		boost::asio::ssl::context& m_SslContext;
		std::optional<ProxySettings> m_ProxySettings;
		i32 m_MessageID = 1;
		bool m_Authenticated = false;
		std::shared_ptr<IWebsocketClient> m_WebsocketClient;
		std::mutex m_QueueMutex;
		std::vector<std::string> m_MessageQueue;

		void handleMessage(const std::string& message);
	};
}