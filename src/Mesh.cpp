#include "pch.h"
#include "Mesh.h"

void Mesh::createMesh()
{
    glGenVertexArrays(1, &m_Vao);
    glGenBuffers(1, &m_Vbo);
    glGenBuffers(1, &m_Ebo);
    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, m_VertexBuffer.size() * sizeof(Vertex), &m_VertexBuffer[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer.size() * sizeof(unsigned int),
        &m_IndexBuffer[0], GL_STATIC_DRAW);
    //positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    //normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    //texCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glBindVertexArray(0);
}


void Mesh::setTextureID(GLuint id)
{
    m_TextureID = id;
}

GLuint Mesh::getTextureID()
{
    return m_TextureID;
}

void BoundingBox::recalculateBoundingBox(Mesh& mesh)
{
    auto& vertexBuffer = mesh.m_VertexBuffer;
    xMin = vertexBuffer[0].pos.x;
    yMin = vertexBuffer[0].pos.y;
    zMin = vertexBuffer[0].pos.z;

    xMax = vertexBuffer[0].pos.x;
    yMax = vertexBuffer[0].pos.y;
    zMax = vertexBuffer[0].pos.z;
    for (int i = 0; i < vertexBuffer.size(); i++) {
        xMin = std::min(xMin, vertexBuffer[i].pos.x);
        yMin = std::min(yMin, vertexBuffer[i].pos.y);
        zMin = std::min(zMin, vertexBuffer[i].pos.z);

        xMax = std::max(xMax, vertexBuffer[i].pos.x);
        yMax = std::max(yMax, vertexBuffer[i].pos.y);
        zMax = std::max(zMax, vertexBuffer[i].pos.z);
    }
}

glm::vec3 BoundingBox::getBoxCenter()
{ 
    return glm::vec3((xMin + xMax) / 2.0f, (yMin + yMax) / 2.0f, (zMin + zMax) / 2.0f);
}
