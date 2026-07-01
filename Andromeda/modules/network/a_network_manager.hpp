#pragma once
#include "a_subsystem_manager.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include "a_BoostWebsocketClient.hpp"
namespace Andromeda {
	class NetworkManager : public ISubsystem {
		public:
			void start() override;
			void update() override;
			static constexpr std::string_view GetStaticName() { return "NetworkManager"; }
			/**
			 * @brief Gets the runtime string identifier of the subsystem.
			 * @return A C-string containing the subsystem's name.
			 */
			const char* getSubsystemName() const override {
				return GetStaticName().data();
			}
	private:
		std::unique_ptr<BoostWebsocketClient> m_WebsocketClient;
		boost::asio::io_context io_context;
		boost::asio::ssl::context ssl_context{boost::asio::ssl::context::tls_client};
	};
}