#include "a_HomeAssistant.hpp"
#include "a_logger.hpp"
#include <nlohmann/json.hpp>
#include "a_network_info.hpp"
#include "a_FileSystem.hpp"
#include <boost/asio/streambuf.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/buffers_iterator.hpp>
namespace Andromeda {

    void HomeAssistantService::init()
    {
        std::string url = "pi.andromeda-cloud.net";
        std::string token = readHomeAssistantTokenFromSecretsJson();
        if (token.empty()) {
            A_ERROR("Home Assistant Token is empty!");
            return;
        }

        boost::asio::ip::tcp::resolver resolver(ioContext);
        boost::asio::ip::tcp::socket socket(ioContext);
        auto proxySettings = NetworkInfo::getProxySettings();

        try {
            auto endpoints = resolver.resolve(proxySettings->host, std::to_string(proxySettings->port));
            boost::asio::connect(socket, endpoints);
            A_INFO("Successfully connected to Proxy {}:{}", proxySettings->host, proxySettings->port);

            std::string req =
                "GET http://" + url + "/api/ HTTP/1.1\r\n"
                "Host: " + url + "\r\n"
                "User-Agent: Andromeda-Client\r\n"
                "Authorization: Bearer " + token + "\r\n"
                "Content-Type: application/json\r\n"
                "Connection: close\r\n\r\n";

            boost::asio::write(socket, boost::asio::buffer(req));

            boost::asio::streambuf response;
            boost::system::error_code ec;

            std::string response_string;
            while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), ec)) {
                response_string.append(
                    boost::asio::buffers_begin(response.data()),
                    boost::asio::buffers_end(response.data())
                );
                response.consume(response.size());
            }

            if (ec && ec != boost::asio::error::eof) {
                throw boost::system::system_error(ec);
            }

            if (response_string.empty()) {
                A_WARN("Received empty response from server.");
                return;
            }

            size_t firstLineEnd = response_string.find("\r\n");
            if (firstLineEnd == std::string::npos) {
                A_ERROR("Invalid HTTP response format.");
                return;
            }

            std::string statusLine = response_string.substr(0, firstLineEnd);
            size_t firstSpace = statusLine.find(" ");
            if (firstSpace == std::string::npos) {
                A_ERROR("Invalid HTTP status line format.");
                return;
            }

            std::string statusCodeStr = statusLine.substr(firstSpace + 1, 3);
            int statusCode = std::stoi(statusCodeStr);
            A_INFO("HTTP Status Code: {}", statusCode);

            if (!validateStatusCode(statusCode)) {
                return;
            }

            size_t bodyPos = response_string.find("\r\n\r\n");
            if (bodyPos == std::string::npos) {
                A_ERROR("Could not find HTTP response body separator.");
                return;
            }

            std::string rawBody = response_string.substr(bodyPos + 4);
            std::string body = dechunkBody(rawBody);
            A_INFO("Response Body:\n{}", body);

            try {
                auto jsonPayload = nlohmann::json::parse(body);
            }
            catch (const std::exception& e) {
                A_INFO("{}", body);
                A_ERROR("Error parsing Home Assistant response: {}", e.what());
            }
        }
        catch (std::exception& e) {
            A_ERROR("Network error in init(): {}", e.what());
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

    std::string HomeAssistantService::dechunkBody(const std::string& chunkedBody)
    {
        u32 pos = 0;
		std::string dechunkedBody = "";
        while (pos < chunkedBody.size()) {
			u32 lineEnd = chunkedBody.find("\r\n", pos);
            if(lineEnd == std::string::npos) {
                break;
            }
			std::string hexStr = chunkedBody.substr(pos, lineEnd - pos);

            u32 chunkSize = 0;

            try {
                chunkSize = std::stoul(hexStr, nullptr, 16);
            }
            catch (...) {
                break;
            }

			if (chunkSize == 0) {
				break;
			}
            pos = lineEnd + 2;

            if (pos + chunkSize <= chunkedBody.size()) {
                dechunkedBody.append(chunkedBody.substr(pos, chunkSize));
            }
            pos += chunkSize + 2;
        }
        return dechunkedBody;
    }
    bool HomeAssistantService::validateStatusCode(i32 statusCode)
    {
        if (statusCode >= 200 && statusCode < 300) return true;

        if (statusCode >= 300 && statusCode < 400)       A_WARN("Redirect response: {}", statusCode);
        else if (statusCode >= 400 && statusCode < 500)  A_ERROR("Client error: {}", statusCode);
        else if (statusCode >= 500 && statusCode < 600)  A_ERROR("Server error: {}", statusCode);

        return false;
    }
}