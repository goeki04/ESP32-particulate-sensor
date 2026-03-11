#pragma once
#include "registry.h"
#include <string>
#include "subsystem.h"
namespace Andromeda {
	using Entity = uint32_t;
	class Scene : public ISubsystem{
		
		Andromeda::ECS::ComponentRegistry* m_Registry;
		void addEntity(unsigned int meshID, const std::string& name, Andromeda::ECS::Component::Transform transform);
		void deleteEntity(Entity id);

		void start() override;
	};
}