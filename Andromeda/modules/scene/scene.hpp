#pragma once
#include "a_registry.hpp"
#include <string>
#include "a_ISubsystem.hpp"
#include "a_math.hpp"
#include "resource_manager.h"
namespace Andromeda {
	using Entity = uint32_t;
	class SceneManager : public ISubsystem{
	public:
		static constexpr const char* subsystemName = "Scene";
		ECS::ComponentRegistry m_Registry;
		amath::CameraData m_EditorCamData;

		void addEntity(u32 meshID, const std::string& name, ECS::Component::Transform transform);
		void deleteEntity(Entity id);
		void start() override;

		static constexpr std::string_view GetStaticName() { return "SceneManager"; }
		const char* getSubsystemName() const override {
			return GetStaticName().data();
		}
	private:
		ResourceManager* m_ResourceManager = nullptr;
	};
}