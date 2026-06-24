#include "a_WeatherService.hpp"
#include <string>
#include <cstdlib>
#include "a_logger.hpp"
#include <nlohmann/json.hpp>

namespace Andromeda {
	WeatherData WeatherService::getLiveWeatherData() {
		std::string location = "Reutlingen";
		
		auto coordsOpt = fetchGeoCoordinates(location);
		if (!coordsOpt) {
			A_ERROR("Failed to fetch geo coordinates for location: {}", location);
			return WeatherData{};
		}
		GeoCoordinates coords = *coordsOpt;

		auto weatherDataOpt = fetchWeatherData(coords);
		if (!weatherDataOpt) {
			A_ERROR("Failed to fetch weather data for coordinates: ({}, {})", coords.latitude, coords.longitude);
			return WeatherData{};
		}
		WeatherData weatherData = *weatherDataOpt;
		return weatherData;
	}
	std::optional<GeoCoordinates> WeatherService::fetchGeoCoordinates(const std::string& location)
	{
		std::string geoDomain = "geocoding-api.open-meteo.com";
		httplib::SSLClient geoClient(geoDomain.c_str());
		geoClient.set_follow_location(true);
		applySystemProxy(geoClient);
		std::string geoURL = "/v1/search?name=" + location + "&count=1&language=de&format=json";
		auto geoResponse = geoClient.Get(geoURL.c_str());
		float latitude = 0.0f;
		float longitude = 0.0f;

		if (geoResponse && geoResponse->status == 200) {
			try {
				nlohmann::json geoJson = nlohmann::json::parse(geoResponse->body);

				if (geoJson.contains("results") && !geoJson["results"].empty()) {
					auto firstResult = geoJson["results"][0];

					latitude = firstResult["latitude"].get<float>();
					longitude = firstResult["longitude"].get<float>();
				}
				else {
					A_ERROR("Location '{}' couldn't be found by Geocoding-API!", location);
					return std::nullopt;
				}
			}
			catch (const nlohmann::json::exception& e) {
				A_ERROR("Geocoding JSON Error: {}", e.what());
				return std::nullopt;
			}
		}
		else {
			A_ERROR("Geocoding request failed. Status: {}", geoResponse ? geoResponse->status : -1);
			return std::nullopt;
		}

		return GeoCoordinates{ latitude, longitude };
	}
	std::optional<WeatherData> WeatherService::fetchWeatherData(const GeoCoordinates& coords)
	{
		std::string domain = "api.open-meteo.com";
		std::string url = "/v1/forecast?latitude=" + std::to_string(coords.latitude) + "&longitude=" + std::to_string(coords.longitude) + "&current_weather=true";

		httplib::SSLClient client(domain.c_str());
		client.set_follow_location(true);
		applySystemProxy(client);

		auto res = client.Get(url.c_str());
		float windSpeed = 0.0f;
		i32 windDirection = 0;
		if (res && res->status == 200) {
			std::string responseBody = res->body;
			try {
				nlohmann::json jsonResponse = nlohmann::json::parse(responseBody);
				if (jsonResponse.contains("current_weather")) {
					auto current = jsonResponse["current_weather"];

					windSpeed = current["windspeed"].get<float>();
					windDirection = static_cast<i32>(current["winddirection"].get<i32>());
					A_INFO("Current Weather: Wind Speed: {} km/h, Wind Direction: {} degrees", windSpeed, windDirection);
				}
			}
			catch (const nlohmann::json::parse_error& e) {
				A_ERROR("JSON Parse Error: " + std::string(e.what()));
				return std::nullopt;
			}
			catch (const nlohmann::json::type_error& e) {
				A_ERROR("JSON Type Error: " + std::string(e.what()));
				return std::nullopt;
			}
		}
		else {
			A_ERROR("Weather request failed. Status: {}", res ? res->status : -1);
			return std::nullopt;
		}
		return WeatherData{ windSpeed, windDirection }; 
	}
	void WeatherService::applySystemProxy(httplib::SSLClient& client)
	{
		const char* proxyEnv = std::getenv("HTTPS_PROXY");
		if (!proxyEnv || proxyEnv[0] == '\0') {
			proxyEnv = std::getenv("HTTP_PROXY");
		}
		if (!proxyEnv || proxyEnv[0] == '\0') {
			return;
		}

		std::string proxyUrl = proxyEnv;
		auto schemeEnd = proxyUrl.find("://");
		std::string hostPort = (schemeEnd != std::string::npos) ? proxyUrl.substr(schemeEnd + 3) : proxyUrl;
		auto colonPos = hostPort.find(':');
		if (colonPos == std::string::npos) {
			return;
		}

		std::string host = hostPort.substr(0, colonPos);
		i32 port = std::atoi(hostPort.c_str() + colonPos + 1);
		if (host.empty() || port <= 0) {
			return;
		}

		client.set_proxy(host.c_str(), port);
	}
}