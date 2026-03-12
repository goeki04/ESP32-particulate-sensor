#include "scene.hpp"
#include "components.hpp"
#include "subsystem_manager.h"
namespace Andromeda{

    void Scene::addEntity(unsigned int meshID, const std::string& name, ECS::Component::Transform transform)
    {
        m_ResourceManager = SystemManager::getInstance().getSubsystem<ResourceManager>();
        auto handle = m_Registry.createHandle();
        
        handle.add<ECS::Component::Transform>(transform);
        handle.add<ECS::Component::Mesh>({ meshID, MaterialShaderType::unlit });
        handle.add<ECS::Component::Tag>({ name });
        auto aabb = m_ResourceManager->getMeshByID(meshID).getAABB();
        handle.add<ECS::Component::AABB>(aabb);
    }

    void Scene::deleteEntity(Entity id)
    {
        m_Registry.destroyEntity(id);
    }
    void Scene::start()
    {
        
    }
}