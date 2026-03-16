#pragma once
#include "registry.h"
#include <string>
#include "a_ISubsystem.hpp"
#include "resource_manager.h"
namespace Andromeda {
	using Entity = uint32_t;
	class Scene : public ISubsystem{
	public:
		ECS::ComponentRegistry m_Registry;
		ResourceManager* m_ResourceManager;
		void addEntity(unsigned int meshID, const std::string& name, ECS::Component::Transform transform);
		void deleteEntity(Entity id);
		void start() override;
		
	};
}