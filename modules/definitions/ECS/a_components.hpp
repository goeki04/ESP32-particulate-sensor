#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "a_material.hpp"
#include "a_math.hpp"
#include <string>
namespace Andromeda::ECS::Component {
    struct Transform {
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

    struct AABB {
        vec3 min = { 0.0f,0.0f,0.0f };
        vec3 max = { 0.0f,0.0f,0.0f };
        vec3 center = { 0.0f,0.0f,0.0f };
    };

    struct Tag{
        std::string name = "Unnamed";
    };

    struct Mesh {
        u32 meshID;
        MaterialShaderType shaderType;
    };
}