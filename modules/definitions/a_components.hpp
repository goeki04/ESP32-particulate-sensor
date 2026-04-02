#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "a_material.hpp"
#include "a_math.hpp"
#include <string>
namespace Andromeda::ECS::Component {   

    struct Transform {
        vec3 position{ 0.0f, 0.0f, 0.0f };
        vec3 rotation{ 0.0f, 0.0f, 0.0f };
        vec3 scale{ 1.0f, 1.0f, 1.0f };

        [[nodiscard]] mat4 modelMatrix() const {
            mat4 m(1.0f);
            m = amath::translate(m, position);
            m = amath::rotate(m, rotation.x, vec3(1, 0, 0));
            m = amath::rotate(m, rotation.y, vec3(0, 1, 0));
            m = amath::rotate(m, rotation.z, vec3(0, 0, 1));
            m = amath::scale(m, scale);
            m = amath::translate(m, vec3(0.0, 0.5f, 0.0f));
            return m;
        }
    };

    struct AABB {
        vec3 min = { 0.0f,0.0f,0.0f };
        vec3 max = { 0.0f,0.0f,0.0f };
        vec3 center = { 0.0f,0.0f,0.0f };
    };

    struct Tag{
        std::string name;
    };

    struct Selected{};

    struct Mesh {
        u32 meshID;
        MaterialShaderType shaderType;
    };
}