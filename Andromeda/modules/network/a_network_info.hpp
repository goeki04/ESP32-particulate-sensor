#pragma once 
#include "a_Primitives.hpp"
#include <string>
#include <cstdlib>
#include <optional>
#include "a_logger.hpp"
namespace Andromeda {
	struct ProxySettings {
		std::string host;
		i32 port;
	};

	class NetworkInfo {
	public:
        static std::optional<ProxySettings> getProxySettings() {
            const char* proxyEnv = std::getenv("HTTPS_PROXY");
            if (!proxyEnv || proxyEnv[0] == '\0') {
                proxyEnv = std::getenv("HTTP_PROXY");
            }
            if (!proxyEnv || proxyEnv[0] == '\0') {
                return {};
            }

            std::string proxyUrl = proxyEnv;

            auto schemeEnd = proxyUrl.find("://");
            std::string hostPort = (schemeEnd != std::string::npos) ? proxyUrl.substr(schemeEnd + 3) : proxyUrl;

            if (!hostPort.empty() && hostPort.back() == '/') {
                hostPort.pop_back();
            }

            auto colonPos = hostPort.find(':');
            if (colonPos == std::string::npos) {
                return {};
            }

            std::string host = hostPort.substr(0, colonPos);
            std::string portStr = hostPort.substr(colonPos + 1);

            if (host.empty() || portStr.empty()) {
                return {};
            }

            try {
                i32 port = std::stoi(portStr);
                if (port <= 0 || port > 65535) {
                    return {};
                }
                return ProxySettings{ host, port };
            }
            catch (const std::exception&) {
                return {};
            }
        }

        static bool hasProxysettings(const std::optional<ProxySettings> &proxySettings) {
            if (proxySettings.has_value()) {
                if (proxySettings->host != "" && proxySettings->port > 0) {
                    A_INFO("Proxy settings found: {}:{}", proxySettings->host, proxySettings->port);
                    return true;
                }
            }
            A_INFO("No proxy found or invalid configuration. Connecting directly.");
            return false;
        }
	};
}