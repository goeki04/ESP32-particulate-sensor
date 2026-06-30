#include "a_HomeAssistant.hpp"
#include "a_logger.hpp"
#include <nlohmann/json.hpp>
#include "a_network_info.hpp"
#include "a_FileSystem.hpp"
#include <httplib.h>
namespace Andromeda {

    void HomeAssistantService::init()
    {
        const std::string host = "pi.andromeda-cloud.net";
        const std::string path = "/api/states/sensor.bmv080_bmv080";

        const std::string token = readHomeAssistantTokenFromSecretsJson();
        if (token.empty()) {
            A_ERROR("Home Assistant Token is empty!");
            return;
        }

        // Use a real TLS/HTTPS client. The endpoint sits behind Cloudflare, which 301-redirects
        // any plaintext http:// request to https:// - hence the previous hand-rolled HTTP request
        // only ever saw a redirect. SSLClient + follow_location mirrors the working WeatherService.
        httplib::SSLClient client(host.c_str());
        client.set_follow_location(true);

        if (auto proxy = NetworkInfo::getProxySettings(); proxy.has_value()) {
            client.set_proxy(proxy->host.c_str(), proxy->port);
            A_INFO("Using proxy {}:{}", proxy->host, proxy->port);
        }

        const httplib::Headers headers = {
            { "Authorization", "Bearer " + token },
            { "User-Agent",    "Andromeda-Client" },
            { "Content-Type",  "application/json" }
        };

        auto res = client.Get(path.c_str(), headers);

        if (res && res->status == 200) {
            A_INFO("HTTP Status Code: {}", res->status);
            A_INFO("Response Body:\n{}", res->body);

            try {
                auto jsonPayload = nlohmann::json::parse(res->body);
                A_INFO("Home Assistant Service successfully initialized and authenticated!");
            }
            catch (const std::exception& e) {
                A_ERROR("Error parsing Home Assistant response: {}", e.what());
            }
        }
        else {
            A_ERROR("Home Assistant request failed. Status: {}", res ? res->status : -1);
        }
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
}