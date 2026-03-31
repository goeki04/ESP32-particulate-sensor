#pragma once

/**
 * @file a_component_parser.hpp
 * @brief Provides JSON serialization logic for Andromeda ECS components.
 */

#include <nlohmann/json.hpp>
#include "a_components.hpp"
#include "a_material.hpp"

 /**
  * @namespace Andromeda::ECS::Component
  * @brief Contains all ECS component structures and their associated serialization logic.
  */
    namespace Andromeda::ECS::Component {

    /** @brief Serializes the Transform component (Position, Rotation, Scale). */
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform, position, rotation, scale)

        /** @brief Serializes the Axis-Aligned Bounding Box (AABB) component. */
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AABB, min, max, center)

        /** @brief Serializes the Tag component used for entity identification. */
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Tag, name)

        /**
         * @brief Serializes the Selected marker component.
         * @note As a marker component with no data, it is represented as an empty JSON object.
         */
        inline void to_json(nlohmann::json& json, const Selected& s) {
        json = nlohmann::json::object();
    }

    /** @brief Deserializes the Selected marker component. */
    inline void from_json(const nlohmann::json& json, Selected& s) {}

    /**
     * @brief Converts MaterialShaderType enum to a JSON integer.
     * @param json The target JSON object.
     * @param e The enum value to serialize.
     */
    inline void to_json(nlohmann::json& json, const MaterialShaderType& e) {
        json = static_cast<u32>(e);
    }

    /**
     * @brief Converts a JSON integer back into a MaterialShaderType enum.
     * @param json The source JSON object.
     * @param e The enum reference to populate.
     */
    inline void from_json(const nlohmann::json& json, MaterialShaderType& e) {
        e = static_cast<MaterialShaderType>(json.get<u32>());
    }

    /** * @brief Serializes the Mesh component.
     * @details Depends on the MaterialShaderType serialization defined above.
     */
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Mesh, meshID, shaderType)
}