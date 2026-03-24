#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "registry.h"
namespace Andromeda {
	class SceneSerializer{
	public:
		static bool save(const std::string& filepath, ECS::ComponentRegistry& registry);
		static bool load(const std::string& filepath, ECS::ComponentRegistry& registry);
	private:
		static nlohmann::json serializeEntity(ECS::Entity entity, ECS::ComponentRegistry& registry);
	};
}