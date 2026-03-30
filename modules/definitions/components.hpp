#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "a_material.hpp"
#include <string>
namespace Andromeda::ECS::Component {
    struct ComponentID {
        template <typename T>
        static uint32_t value() {
            static uint32_t id = nextID();
            return id;
        }

        static uint32_t nextID() {
            static uint32_t lastID = 0;
            return lastID++;
        }
    };

    struct Transform{
        vec3 position{ 0.0f, 0.0f, 0.0f };
        vec3 rotation{ 0.0f, 0.0f, 0.0f };
        vec3 scale{ 1.0f, 1.0f, 1.0f };

        const glm::mat4 modelMatrix() const
        {
            glm::mat4 m(1.0f);
            m = glm::translate(m, position);
            m = glm::rotate(m, rotation.x, glm::vec3(1, 0, 0));
            m = glm::rotate(m, rotation.y, glm::vec3(0, 1, 0));
            m = glm::rotate(m, rotation.z, glm::vec3(0, 0, 1));
            m = glm::scale(m, scale);
            m = glm::translate(m, glm::vec3(0.0, 0.5f, 0.0f));
            return m;
        }
    };

    struct Tag{
        std::string name;
    };

    struct Selected{};

    struct Mesh{
        unsigned int meshID;
        MaterialShaderType shaderType;
    };

    struct Particle{
        vec3 position;
        vec3 velocity;
        vec3 accel;
        vec4 color;
        float life;
    };

    struct AABB{
        vec3 min;
        vec3 max;
        vec3 center;
        AABB() : min(0.0f), max(0.0f), center(0.0f) {};
    };
}