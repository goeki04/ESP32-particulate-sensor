#pragma once
#include <vector>
#include <glm/glm.hpp>          
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "components.hpp"
#include "a_primitives.hpp"
namespace Andromeda {
    struct Vertex {
    public:
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 color; //this is not a vertex color, its the diffuse color
        Vertex() : pos(0.0f), normal(0.0f), uv(0.0f), color(1.0f) {};
    };
    struct AABB {
        glm::vec3 min{ 1e34f };
        glm::vec3 max{ -1e34f };
    };

    struct Mesh {
        std::vector<Vertex> m_Vertexbuffer;
        std::vector<u32> m_Indexbuffer;

        Mesh(std::vector<Vertex>&& vertexPositions, std::vector<u32>&& vertexIndices)
            : m_Vertexbuffer(std::move(vertexPositions)),
            m_Indexbuffer(std::move(vertexIndices)) {
        }

        Mesh() = default;

        ECS::Component::AABB getAABB() const
        {
            if (m_Vertexbuffer.empty())
                throw std::runtime_error("Mesh has no vertices");

            const auto& vb = m_Vertexbuffer;
            glm::vec3 min = vb[0].pos;
            glm::vec3 max = min;

            for (const auto& v : vb) {
                const glm::vec3 p = v.pos;
                min = glm::min(min, p);
                max = glm::max(max, p);
            }
            ECS::Component::AABB aabb;
            aabb.min = min;
            aabb.max = max;
            aabb.center = (min + max) * 0.5f;

            return aabb;
        }

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&& other) noexcept = default;
        Mesh& operator=(Mesh&& other) noexcept = default;

        ~Mesh() = default;
    };
}