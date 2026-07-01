#pragma once
#include "a_Primitives.hpp"

#include <string>
namespace Andromeda {
	class HomeAssistantService {
	public:
		void init();
		std::string readHomeAssistantTokenFromSecretsJson();
	};
}