#pragma once
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace ECS {
    namespace component {
        struct Transform {
            glm::vec3 position{ 0.0f, 0.0f, 0.0f };
            glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
            glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

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

        struct Tag {
            std::string name;
        };

        struct Selected{};

        struct Mesh {
            unsigned int meshID;
            MaterialShaderType type;
        };

        struct AABB {
            glm::vec3 min;
            glm::vec3 max;
            glm::vec3 center;
            AABB() : min(0.0f), max(0.0f), center(0.0f) {};
        };
    }
}