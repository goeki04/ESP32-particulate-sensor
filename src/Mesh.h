#pragma once
#include "Shader.h"
class Camera;
class Mesh;
namespace Andromeda {
    struct Vertex {
    public:
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 color; //this is not a vertex color, its the diffuse color
        Vertex() : pos(0.0f), normal(0.0f), uv(0.0f), color(1.0f) {};
    };
    class Mesh {
    public:
        unsigned int m_Vao = 0;
        std::vector<Vertex> m_VertexBuffer;
        std::vector<unsigned int> m_IndexBuffer;

        Mesh(std::vector<Vertex>&& vertexPositions, std::vector<unsigned int>&& vertexIndices)
            : m_VertexBuffer(std::move(vertexPositions)),
            m_IndexBuffer(std::move(vertexIndices)) {
        }

        Mesh() = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&& other) noexcept {
            moveFrom(std::move(other));
        }

        Mesh& operator=(Mesh&& other) noexcept {
            if (this != &other) {
                destroyGL();
                moveFrom(std::move(other));
            }
            return *this;
        }

        ~Mesh() {
            destroyGL();
        }

        void createMesh();
    private:
        unsigned int m_Vbo = 0, m_Ebo = 0;

    private:
        void destroyGL() noexcept {
            if (m_Ebo) glDeleteBuffers(1, &m_Ebo);
            if (m_Vbo) glDeleteBuffers(1, &m_Vbo);
            if (m_Vao) glDeleteVertexArrays(1, &m_Vao);
            m_Ebo = m_Vbo = m_Vao = 0;
        }

        void moveFrom(Mesh&& other) noexcept {
            m_Vao = other.m_Vao;
            other.m_Vao = 0;
            m_Vbo = other.m_Vbo;
            other.m_Vbo = 0;
            m_Ebo = other.m_Ebo;
            other.m_Ebo = 0;

            m_VertexBuffer = std::move(other.m_VertexBuffer);
            m_IndexBuffer = std::move(other.m_IndexBuffer);
        }
    };
}


