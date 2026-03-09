#include "pch.h"
#include "mesh.h"
namespace Andromeda {
    void Mesh::createMesh()
    {
        glGenVertexArrays(1, &m_Vao);
        glGenBuffers(1, &m_Vbo);
        glGenBuffers(1, &m_Ebo);
        glBindVertexArray(m_Vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
        glBufferData(GL_ARRAY_BUFFER,
            m_VertexBuffer.size() * sizeof(Vertex),
            m_VertexBuffer.empty() ? nullptr : m_VertexBuffer.data(),
            GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            m_IndexBuffer.size() * sizeof(unsigned int),
            m_IndexBuffer.empty() ? nullptr : m_IndexBuffer.data(),
            GL_STATIC_DRAW);
        //positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        //normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        //texCoords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glBindVertexArray(0);
    }
}