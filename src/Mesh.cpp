#include "pch.h"
#include "Mesh.h"

void Mesh::createMesh()
{
    glGenVertexArrays(1, &m_Vao);
    glGenBuffers(1, &m_Vbo);
    glGenBuffers(1, &m_Ebo);
    glBindVertexArray(m_Vao);

    //Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, m_VertexBuffer.size() * sizeof(Vertex), &m_VertexBuffer[0], GL_STATIC_DRAW);

    //Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer.size() * sizeof(unsigned int),
        &m_IndexBuffer[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glBindVertexArray(0);
}

void Mesh::drawMesh()
{
    glUseProgram(m_Shader->m_Program);
    m_Shader->setProperties();
    glBindVertexArray(m_Vao);
    glDrawElements(GL_TRIANGLES, m_IndexBuffer.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}