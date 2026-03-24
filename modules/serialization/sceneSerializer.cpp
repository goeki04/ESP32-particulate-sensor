#include "sceneSerializer.hpp"
#include "components.hpp"
#include <fstream>
namespace Andromeda {

	bool SceneSerializer::save(const std::string& filepath, ECS::ComponentRegistry& registry)
	{
		nlohmann::json root;
		root["scene_name"] = "Example Room";
		auto& entities = root["entities"];
		auto& tagPool = registry.getPool<ECS::Component::Tag>();
		for (ECS::Entity e : tagPool.getEntities()) {
			entities.push_back(serializeEntity(e, registry));
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

		if (root.contains("entities") && root["entities"].is_array()) {
			for (auto& eJson : root["entities"]) {

				auto handle = registry.createHandle();

				if (eJson.contains("tag")) {
					handle.add<ECS::Component::Tag>({ eJson["tag"].get<std::string>() });
				}

				if (eJson.contains("transform")) {
					auto& tJson = eJson["transform"];
					ECS::Component::Transform t;

					t.position = { tJson["pos"][0], tJson["pos"][1], tJson["pos"][2] };
					t.rotation = { tJson["rot"][0], tJson["rot"][1], tJson["rot"][2] };
					t.scale = { tJson["scale"][0], tJson["scale"][1], tJson["scale"][2] };

					handle.add<ECS::Component::Transform>(t);
				}
			}
		}

		return true;
	}

	nlohmann::json SceneSerializer::serializeEntity(ECS::Entity entity, ECS::ComponentRegistry& registry)
	{
		nlohmann::json eJson;
		ECS::EntityHandle handle = { entity, &registry };

		eJson["id"] = handle.id;

		if (handle.has<ECS::Component::Tag>()) {
			eJson["tag"] = handle.get<ECS::Component::Tag>().name;
		}

		if (handle.has<ECS::Component::Transform>()) {
			auto& t = handle.get<ECS::Component::Transform>();
			eJson["transform"] = {
				{"pos", {t.position.x, t.position.y, t.position.z}},
				{"rot", {t.rotation.x, t.rotation.y, t.rotation.z}},
				{"scale", {t.scale.x, t.scale.y, t.scale.z}}
			};
		}

		return eJson;
	}
}