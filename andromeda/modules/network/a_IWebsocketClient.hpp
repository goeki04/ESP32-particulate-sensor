#pragma once
#include <string>
#include <functional>
namespace Andromeda {
	class IWebsocketClient {
	public:
		virtual ~IWebsocketClient() = default;

		virtual void connect() = 0;
		virtual void disconnect() = 0;
		virtual void send(const std::string& payload) = 0;

		virtual void onMessageReceived(std::function<void(const std::string&)> callback) = 0;
	};
}