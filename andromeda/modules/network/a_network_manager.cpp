#include "a_network_manager.hpp"
#include "a_HomeAssistant.hpp"
#include "a_logger.hpp"
#include "a_BoostWebsocketClient.hpp"
namespace Andromeda {
	void NetworkManager::start()
	{
		ssl_context.set_verify_mode(boost::asio::ssl::verify_peer);
		std::string certFile = std::string(SOURCE_DIRECTORY) + "/certs/cacert.pem";
		ssl_context.load_verify_file(certFile);
		ssl_context.set_default_verify_paths();
		m_WebsocketClient = std::make_unique<BoostWebsocketClient>(io_context, ssl_context);
	}
	void NetworkManager::update()
	{
	}
}