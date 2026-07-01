#include "a_network_manager.hpp"
#include "a_HomeAssistant.hpp"
#include "a_logger.hpp"
#include "a_BoostWebsocketClient.hpp"
namespace Andromeda {
	void NetworkManager::start()
	{
		m_ProxySettings = NetworkInfo::getProxySettings();
		m_SslContext.set_verify_mode(boost::asio::ssl::verify_peer);
		std::string certFile = std::string(SOURCE_DIRECTORY) + "/certs/cacert.pem";
		m_SslContext.load_verify_file(certFile);
		m_SslContext.set_default_verify_paths();
		m_WebsocketClient = std::make_unique<BoostWebsocketClient>(m_IoContext, m_SslContext, m_ProxySettings);
		m_WebsocketClient->connect();
	}
	void NetworkManager::update()
	{
	}
}