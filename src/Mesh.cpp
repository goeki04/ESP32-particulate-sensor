#include "pch.h"
#include "Mesh.h"
#include "camera.h"
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
    glBindVertexArray(0);
}

void BoundingBox::setAABB(const Mesh& mesh)
{
    if (mesh.m_VertexBuffer.empty())
        throw std::runtime_error("Mesh has no vertices");

    const auto& vb = mesh.m_VertexBuffer;
    glm::vec3 min = vb[0].getPosition();
    glm::vec3 max = min;

    for (const auto& v : vb) {
        const glm::vec3 p = v.getPosition();
        min = glm::min(min, p);
        max = glm::max(max, p);
    }

    glm::vec3 center = (min + max) * 0.5f;
    m_AABB.min = min;
    m_AABB.max = max;
    m_AABB.center = center;
}

const AABB& BoundingBox::getAABB() const
{
    return m_AABB;
}

bool BoundingBox::RayIntersectAABB(Camera& cam)
{
    Ray cursorToRay = cam.m_CursorToWorldRay;

    return false;
}
