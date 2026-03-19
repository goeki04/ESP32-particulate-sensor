#pragma once
#include <vector>
#include <glm/glm.hpp>          
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "components.hpp"
#include "a_primitives.hpp"
#include <stdexcept>

namespace Andromeda {

    /**
     * @brief Represents a single point in 3D space with associated data.
     */
    struct Vertex {
    public:
        vec3 pos;    ///< 3D position of the vertex
        vec3 normal; ///< Normal vector for lighting calculations
        vec2 uv;     ///< Texture coordinates (u, v)
        vec3 color;  ///< Diffuse color of the vertex (not per-vertex-lighting color)

        /// Default constructor: Initializes all vectors to zero, color to white.
        Vertex() : pos(0.0f), normal(0.0f), uv(0.0f), color(1.0f) {};
    };

    /**
     * @brief Axis-Aligned Bounding Box for collision and culling.
     */
    struct AABB {
        vec3 min{ 1e34f };  ///< Initialized to infinity to ensure correct min-clamping
        vec3 max{ -1e34f }; ///< Initialized to negative infinity to ensure correct max-clamping
    };

    /**
     * @brief CPU-side representation of a 3D model's geometry.
     * Manages vertex and index data and provides utility for bounds calculation.
     */
    struct Mesh {
        std::vector<Vertex> m_Vertexbuffer; ///< Storage for all vertices in the mesh
        std::vector<u32> m_Indexbuffer;     ///< Storage for the rendering order (indices)

        /**
         * @brief Move constructor for efficient data transfer.
         * Prevents deep copying of large vertex/index arrays.
         */
        Mesh(std::vector<Vertex>&& vertexPositions, std::vector<u32>&& vertexIndices)
            : m_Vertexbuffer(std::move(vertexPositions)),
            m_Indexbuffer(std::move(vertexIndices)) {
        }

        Mesh() = default;

        /**
         * @brief Calculates the Axis-Aligned Bounding Box (AABB) for this mesh.
         * Iterates through all vertices to find the min/max bounds.
         * @return Calculated AABB component.
         * @throws std::runtime_error if the vertex buffer is empty.
         */
        ECS::Component::AABB getAABB() const
        {
            if (m_Vertexbuffer.empty())
                throw std::runtime_error("Mesh has no vertices");

            const auto& vb = m_Vertexbuffer;
            vec3 min = vb[0].pos;
            vec3 max = min;

            for (const auto& v : vb) {
                const vec3 p = v.pos;
                min = glm::min(min, p);
                max = glm::max(max, p);
            }

            ECS::Component::AABB aabb;
            aabb.min = min;
            aabb.max = max;
            aabb.center = (min + max) * 0.5f;

            return aabb;
        }

        // Disable copying to avoid accidental performance hits with large meshes
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        // Enable moving for efficient resource management
        Mesh(Mesh&& other) noexcept = default;
        Mesh& operator=(Mesh&& other) noexcept = default;

        ~Mesh() = default;
    };
}