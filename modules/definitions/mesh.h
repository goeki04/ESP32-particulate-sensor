#pragma once
#include "a_geometry.hpp"
#include "a_primitives.hpp"
namespace Andromeda {
    
    struct Mesh {
        std::vector<Vertex> m_VertexBuffer;
        std::vector<u32> m_IndexBuffer;

        Mesh(std::vector<Vertex>&& vertexPositions, std::vector<u32>&& vertexIndices)
            : m_VertexBuffer(std::move(vertexPositions)),
            m_IndexBuffer(std::move(vertexIndices)) {
        }

        Mesh() = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&& other) noexcept = default;
        Mesh& operator=(Mesh&& other) noexcept = default;

        ~Mesh() = default;
    };
}


