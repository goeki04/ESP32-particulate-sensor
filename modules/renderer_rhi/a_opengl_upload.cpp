#include "a_opengl_upload.hpp"
#include "GL/glew.h"
#include "a_opengl_handles.hpp"
#include "a_geometry.hpp"
namespace Andromeda {
    void createMesh(MeshGPUHandle& mesh)
    {
        glGenVertexArrays(1, &mesh.vao);
        glGenBuffers(1, &mesh.vbo);
        glGenBuffers(1, &mesh.ebo);
        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER,
            mesh.m_VertexBuffer.size() * sizeof(Vertex),
            mesh.m_VertexBuffer.empty() ? nullptr : mesh.m_VertexBuffer.data(),
            GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            mesh.m_IndexBuffer.size() * sizeof(unsigned int),
            mesh.m_IndexBuffer.empty() ? nullptr : mesh.m_IndexBuffer.data(),
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