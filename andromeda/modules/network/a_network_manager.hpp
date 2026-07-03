#pragma once
#include "a_subsystem_manager.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <memory>
#include "a_HomeAssistant.hpp"
#include "a_network_info.hpp"
#include <optional>
#include <thread>
#include <boost/asio/executor_work_guard.hpp>
namespace Andromeda {
	class NetworkManager : public ISubsystem {
		public:
			void start() override;
			void destroy() override;
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
		std::shared_ptr<boost::asio::io_context> m_IoContext;
		boost::asio::ssl::context m_SslContext{boost::asio::ssl::context::tls_client};
		std::unique_ptr<HomeAssistantService> m_HomeAssistantService;
		std::optional<ProxySettings> m_ProxySettings;
		std::optional<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_WorkGuard;
		std::jthread m_NetworkThread;
	};
}