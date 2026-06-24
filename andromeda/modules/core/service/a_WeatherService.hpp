#pragma once
#include <httplib.h>
#include "a_Primitives.hpp"
#include <optional>
namespace Andromeda {
	struct GeoCoordinates {
		float latitude;
		float longitude;
	};

	struct WeatherData {
		float windSpeed;
		i32 windDirection;
	};

	class WeatherService {
	public:
		WeatherService() {};
		void applySystemProxy(httplib::SSLClient& client);
		WeatherData getLiveWeatherData();
		std::optional<GeoCoordinates> fetchGeoCoordinates(const std::string& location);
		std::optional<WeatherData> fetchWeatherData(const GeoCoordinates& coords);
	};
}