#include "a_HomeAssistant.hpp"
#include "a_logger.hpp"
#include <nlohmann/json.hpp>
#include "a_network_info.hpp"
#include "a_FileSystem.hpp"
namespace Andromeda {
	void HomeAssistantService::init()
	{
#ifdef _WIN32
		ix::initNetSystem();
#endif
		std::string url = "wss://pi.andromeda-cloud.net/api/websocket";
		std::string token = readHomeAssistantTokenFromSecretsJson();

		m_WebSocket.setUrl(url);
		m_WebSocket.setOnMessageCallback([this,token](const ix::WebSocketMessagePtr& msg) {
			if (msg->type == ix::WebSocketMessageType::Open) {
				A_TRACE("WebSocket connected");
			}
			else if (msg->type == ix::WebSocketMessageType::Message) {
				A_TRACE("Received message: {}", msg->str);
				try {
					nlohmann::json message = nlohmann::json::parse(msg->str);
					std::string type = message.value("type", "");
					if (type == "auth_required") {
						nlohmann::json authMessage = {
							{"type", "auth"},
							{"access_token", token}
						};
						m_WebSocket.send(authMessage.dump());
					}
					else if (type == "auth_ok") {
						A_TRACE("Authentication successful");
						subscribeToEvents();
					}
					else if (type == "event") {
						A_TRACE("Received event: {}", message.dump());
					}
				}
				catch(const std::exception& e){
				    A_ERROR("Exception while parsing message: {}", e.what());
				}
			}
			else if (msg->type == ix::WebSocketMessageType::Error) {
				A_CRITICAL("WebSocket error: {}", msg->errorInfo.reason);
			}
		});

		m_WebSocket.start();

	}

	std::string HomeAssistantService::readHomeAssistantTokenFromSecretsJson()
	{
		std::string source = SOURCE_DIRECTORY;
		std::string rawTokenFile = Filesystem::readFile(source + "/secrets.json");
		nlohmann::json tokenJson = nlohmann::json::parse(rawTokenFile);

		std::string ha_token = "";

		if (tokenJson.contains("home_assistant") && tokenJson["home_assistant"].is_object()) {
			ha_token = tokenJson["home_assistant"].value("token", "");
		}

		if (ha_token.empty()) {
			A_ERROR("Home Assistant Token konnte nicht aus der secrets.json gelesen werden!");
			return "";
		}
		return ha_token;
	}
	void HomeAssistantService::subscribeToEvents()
	{
		nlohmann::json subscribeMessage = {
			{"id", m_CurrentID++},
			{"type", "subscribe_events"},
			{"event_type", "state_changed"}
		};

		std::string messageStr = subscribeMessage.dump();
		m_WebSocket.send(messageStr);
	}
}