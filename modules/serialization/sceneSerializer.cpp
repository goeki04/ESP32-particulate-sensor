#include "sceneSerializer.hpp"
#include "a_components.hpp"
#include <fstream>
#include <iostream>
namespace Andromeda {

	bool SceneSerializer::save(const std::string& filepath, ECS::ComponentRegistry& registry)
	{
		nlohmann::json root;
		for (const auto &poolPtr: registry.m_Pools | std::views::values) {
			const std::string typeName = poolPtr->getTypeName();
			root[typeName] = poolPtr->serializePool();
		}
		std::ofstream file(filepath);
		if (file.is_open()) {
			file << std::setw(4) << root << std::endl;
			return true;
		}
		return false;
	}
    bool SceneSerializer::load(const std::string& filepath, ECS::ComponentRegistry& registry)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;

        nlohmann::json root;
        file >> root;
        ECS::Entity maxID = 0;

        const std::string transformName = typeid(ECS::Component::Transform).name();
        if (root.contains(transformName)) {
            auto& pool = registry.getPool<ECS::Component::Transform>();
            pool.deserializePool(root[transformName]);
            for (const auto id : pool.getEntities()) if (id > maxID) maxID = id;
        }
        const std::string tagName = typeid(ECS::Component::Tag).name();
        if (root.contains(tagName)) {
            auto& pool = registry.getPool<ECS::Component::Tag>();
            pool.deserializePool(root[tagName]);
            for (const auto id : pool.getEntities()) if (id > maxID) maxID = id;
        }
        const std::string meshName = typeid(ECS::Component::MeshRenderer).name();
        if (root.contains(meshName)) {
            auto& pool = registry.getPool<ECS::Component::MeshRenderer>();
            pool.deserializePool(root[meshName]);
            for (auto id : pool.getEntities()) if (id > maxID) maxID = id;
        }
        if (maxID > 0 || !root.empty()) {
            registry.m_NextID = maxID + 1;
        }
        return true;
    }
}