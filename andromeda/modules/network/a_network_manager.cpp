#include "a_network_manager.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include "a_HomeAssistant.hpp"
#include "a_logger.hpp"
namespace Andromeda {
	void NetworkManager::start()
	{
		m_SslContext.set_verify_mode(boost::asio::ssl::verify_peer);

		std::string verifyFilePath(CERTIFICATE_DIRECTORY + std::string("cacert.pem"));
		m_SslContext.load_verify_file(verifyFilePath);
		A_INFO("Loading SSL certificate from: {}", verifyFilePath);
		boost::asio::co_spawn(m_IoContext, Andromeda::HomeAssistantService::init(m_SslContext), boost::asio::detached);
	}
	void NetworkManager::update()
	{
		m_IoContext.poll();
	}
}