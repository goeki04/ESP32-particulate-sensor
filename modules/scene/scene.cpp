#include "scene.hpp"
#include "components.hpp"
namespace Andromeda{
    using Entity = uint32_t;
    void Scene::addEntity(unsigned int meshID, const std::string& name, ECS::Component::Transform transform)
    {
        auto handle = m_Registry->createHandle();
        handle.add<ECS::Component::Transform>(transform);
        handle.add<ECS::Component::Mesh>({ meshID, MaterialShaderType::unlit });
        ECS::Component::AABB aabb;
        setAABB(getMeshByID(meshID), aabb);
        handle.add<ECS::Component::AABB>(aabb);
        handle.add<ECS::Component::Tag>({ name });
    }

    void Scene::deleteEntity(Entity id)
    {
        m_Registry->destroyEntity(id);
    }
}