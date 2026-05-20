#pragma once
#include <nlohmann/json.hpp>
#include "a_glm_json_parser.hpp"
#include "a_components.hpp"
#include "a_model_record.hpp"
 /**
  * @namespace Andromeda::ECS::Component
  * @brief Contains all ECS component structures and their associated serialization logic.
  */
namespace Andromeda::ECS::Component {

    inline void to_json(nlohmann::json& json, const deviceType& e) {
        json = e;
    }

    inline void from_json(const nlohmann::json& json, deviceType& e) {
        e = static_cast<deviceType>(json.get<int>());
    }

    /** @brief Serializes the Transform component (Position, Rotation, Scale). */
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform, position, rotation, scale)

    /** @brief Serializes the Axis-Aligned Bounding Box (AABB) component. */
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AABB, min, max, center)

    /** @brief Serializes the Tag component used for entity identification. */
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Tag, name)

    /** @brief Serializes the Device component. */
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Device, type)

    /** @brief Serializes the MeshFilter component. */
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MeshRenderer, meshID, materialName)
}