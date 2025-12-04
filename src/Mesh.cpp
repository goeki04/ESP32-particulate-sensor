#include "pch.h"
#include "Mesh.h"

void Mesh::createMesh()
{
    applyBoundingBox();
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
    m_Shader->setUniforms();
    glBindVertexArray(m_Vao);
    glDrawElements(GL_TRIANGLES, m_IndexBuffer.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
/// <summary>
/// Call this function after initializing the vertexbuffer because it depends on it
/// </summary>
void Mesh::applyBoundingBox()
{
    BoundingBox boundingBox(*this);
    glm::vec3 center = boundingBox.getBoxCenter();
    for (auto& v : m_VertexBuffer) {
        v = v - center;
    }
}

void BoundingBox::recalculateBoundingBox(Mesh& mesh)
{
    auto& vertexBuffer = mesh.m_VertexBuffer;
    xMin = vertexBuffer[0].x;
    yMin = vertexBuffer[0].y;
    zMin = vertexBuffer[0].z;

    xMax = vertexBuffer[0].x;
    yMax = vertexBuffer[0].y;
    zMax = vertexBuffer[0].z;
    for (int i = 0; i < vertexBuffer.size(); i++) {
        xMin = std::min(xMin, vertexBuffer[i].x);
        yMin = std::min(yMin, vertexBuffer[i].y);
        zMin = std::min(zMin, vertexBuffer[i].z);

        xMax = std::max(xMax, vertexBuffer[i].x);
        yMax = std::max(yMax, vertexBuffer[i].y);
        zMax = std::max(zMax, vertexBuffer[i].z);
    }
}

glm::vec3 BoundingBox::getBoxCenter()
{ 
    return glm::vec3((xMin + xMax) / 2.0f, (yMin + yMax) / 2.0f, (zMin + zMax) / 2.0f);
}
