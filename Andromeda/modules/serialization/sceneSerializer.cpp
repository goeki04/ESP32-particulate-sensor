#include "sceneSerializer.hpp"
#include "a_components.hpp"
#include <fstream>
#include <iostream>
#include <unordered_set>
#include "a_Primitives.hpp"
namespace Andromeda {

	bool SceneSerializer::save(const std::string& filepath, ECS::ComponentRegistry& registry, ResourceManager& rm)
	{
		nlohmann::json root;
		const std::string meshTypeName = typeid(ECS::Component::MeshRenderer).name();

		for (const auto &poolPtr: registry.m_Pools | std::views::values) {
			const std::string typeName = poolPtr->getTypeName();
			// The MeshRenderer pool is handled specially below: its volatile, session-local
			// meshID must be persisted as a stable mesh name instead of a raw number.
			if (typeName == meshTypeName) continue;
			root[typeName] = poolPtr->serializePool();
		}

		// MeshRenderer: store stable mesh names instead of volatile IDs.
		if (registry.m_Pools.contains(std::type_index(typeid(ECS::Component::MeshRenderer)))) {
			auto& meshPool = registry.getPool<ECS::Component::MeshRenderer>();
			nlohmann::json mr;
			mr["entities"] = meshPool.getEntities();
			nlohmann::json names = nlohmann::json::array();
			for (const auto& comp : meshPool.data()) {
				names.push_back(rm.getMeshNameByID(comp.meshID));
			}
			mr["meshNames"] = names;
			root[meshTypeName] = mr;
		}

		std::ofstream file(filepath);
		if (file.is_open()) {
			file << std::setw(4) << root << std::endl;
			return true;
		}
		return false;
	}
    bool SceneSerializer::load(const std::string& filepath, ECS::ComponentRegistry& registry, ResourceManager& rm)
    {
		registry.clearRegistry();
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
        const std::string meshMaterialName = typeid(ECS::Component::Material).name();
        if (root.contains(meshMaterialName)) {
            auto& pool = registry.getPool<ECS::Component::Material>();
            pool.deserializePool(root[meshMaterialName]);
            for (const auto id : pool.getEntities()) if (id > maxID) maxID = id;
        }
        const std::string meshName = typeid(ECS::Component::MeshRenderer).name();
        if (root.contains(meshName)) {
            const auto& mr = root[meshName];
            const auto entities = mr.at("entities").get<std::vector<ECS::Entity>>();
            const auto names = mr.at("meshNames").get<std::vector<std::string>>();

            // Translate stable mesh names back into the current session's mesh IDs,
            // then rebuild the pool from the resolved data.
            nlohmann::json rebuilt;
            rebuilt["entities"] = nlohmann::json::array();
            rebuilt["components"] = nlohmann::json::array();
            for (size_t i = 0; i < entities.size(); ++i) {
                u32 id;
                if (!rm.tryGetMeshIDByName(names[i], id)) {
                    std::cout << "Warning: mesh '" << names[i]
                              << "' not found; skipping entity " << entities[i] << std::endl;
                    continue;
                }
                rebuilt["entities"].push_back(entities[i]);
                rebuilt["components"].push_back(ECS::Component::MeshRenderer{ id });
            }

            auto& pool = registry.getPool<ECS::Component::MeshRenderer>();
            pool.deserializePool(rebuilt);
            for (auto id : pool.getEntities()) {
                if (id > maxID) maxID = id;
            }
        }
        const std::string aabb = typeid(ECS::Component::AABB).name();
        if (root.contains(aabb)) {
            auto& pool = registry.getPool<ECS::Component::AABB>();
            pool.deserializePool(root[aabb]);
            for (auto id : pool.getEntities()) {
                if (id > maxID) maxID = id;
            }
        }

        if (maxID > 0 || !root.empty()) {
            registry.m_NextID = maxID + 1;
			std::cout << "Scene loaded. Next entity ID set to: " << registry.m_NextID << std::endl;
           
        }
        registry.rebuildActiveEntities();
        return true;
    }
}