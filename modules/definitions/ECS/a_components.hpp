#pragma once
#include "a_math.hpp"
#include "a_model_record.hpp"
#include <string>

namespace Andromeda::ECS::Component {

    template<typename T>
    void resetComponent(T& component) {
        component = T();
    }

    /**
    * @brief Marker attribute for the automated Undo/Redo system.
    *
    * @details Components marked with this attribute are parsed by the
    *          Python metadata pre-processor (gen_undo.py). The build
    *          system uses this information to automatically generate:
    *          - Command-specific POD data structures.
    *          - Dispatcher logic for the Data-Oriented Undo buffer.
    *          - Memory-efficient snapshot logic for editor state management.
    *
    * @note Only components intended for manual user editing in the
    *       Inspector should be marked. Volatile or calculated data
    *       (like AABB or Physics state) should be excluded to save memory.
    */

    /**
     * @struct [[Andromeda::Undo]]
     * @brief Spatial representation of an entity.
     * @details Marked with [[Andromeda::Undo]] to track position, scale,
     *          and rotation changes in the editor.
     */
    struct [[Andromeda::Undo]] Transform {
        vec3 position{ 0.0f, 0.0f, 0.0f };
        vec3 scale{ 1.0f, 1.0f, 1.0f };
        quat rotation = quat(1.0f,0.0f,0.0f,0.0f);
        [[nodiscard]] mat4 modelMatrix() const {
            mat4 m(1.0f);
            m = amath::translate(m, position);
            m *= glm::mat4_cast(rotation);
            m = amath::scale(m, scale);
            return m;
        }
    };

    struct Device
    {
        deviceType type;
    };

    struct MeshRenderer
    {
        u32 meshID;
        std::string materialName = "PBRMaterial";
    };

    struct AABB {
        vec3 min = { 0.0f,0.0f,0.0f };
        vec3 max = { 0.0f,0.0f,0.0f };
        vec3 center = { 0.0f,0.0f,0.0f };
    };

    struct [[Andromeda::Undo]] Tag{
        std::string name = "Unnamed";
    };
}
