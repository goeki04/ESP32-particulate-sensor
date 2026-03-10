#pragma once
#include "registry.h"
#include <string>
namespace Andromeda {
	class Scene {
		
		Andromeda::ECS::ComponentRegistry* m_Registry;
		void addEntity(unsigned int meshID, const std::string& name, Andromeda::ECS::Component::Transform transform);
		void deleteEntity(Entity id);
	};
}