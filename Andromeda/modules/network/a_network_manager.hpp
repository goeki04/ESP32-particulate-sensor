#pragma once
#include "a_subsystem_manager.hpp"
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/beast/core.hpp>

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
		boost::asio::io_context m_IoContext;
		boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_WorkGuard{ boost::asio::make_work_guard(m_IoContext) };
		boost::asio::ssl::context m_SslContext{boost::asio::ssl::context::tls_client};
	};
}