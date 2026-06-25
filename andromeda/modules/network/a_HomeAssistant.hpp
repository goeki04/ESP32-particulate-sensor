#pragma once
#include "a_Primitives.hpp"
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <string>
namespace Andromeda {
	class HomeAssistantService {
	public:
		void init();
	private:
		i32 m_CurrentID = 0;
		ix::WebSocket m_WebSocket;
		std::string readHomeAssistantTokenFromSecretsJson();
		void subscribeToEvents();
	};
}