#include "pch.h"
#include "SceneObject.h"

void SceneObject::drawMesh()
{
        glUseProgram(m_Shader->m_Program);
        glm::mat4 localMatrix = m_Transform.modelMatrix();
        m_Shader->setUniforms(localMatrix);
        glBindVertexArray(m_Mesh.m_Vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Mesh.m_IndexBuffer.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
}

void SceneObject::initialize()
{
    m_BoundingBox.setAABB(m_Mesh);
}

const AABB& BoundingBox::getAABB() const
{
    return m_AABB;
}

bool BoundingBox::RayIntersectAABB(const Camera& cam,const glm::mat4& modelMatrix)
{
    {
        const Ray& rayW = cam.m_CursorToWorldRay;

        // World -> Local (Objektspace)
        glm::mat4 invModel = glm::inverse(modelMatrix);

        glm::vec3 o = glm::vec3(invModel * glm::vec4(rayW.origin, 1.0f));          // Punkt -> w=1
        glm::vec3 d = glm::normalize(glm::vec3(invModel * glm::vec4(rayW.direction, 0.0f))); // Richtung -> w=0

        float tMin = 0.001f;
        float tMax = 1e30f;

        for (int i = 0; i < 3; ++i)
        {
            float oi = o[i];
            float di = d[i];

            if (std::fabs(di) < 1e-8f) {
                if (oi < m_AABB.min[i] || oi > m_AABB.max[i]) return false;
                continue;
            }

            float invD = 1.0f / di;
            float t0 = (m_AABB.min[i] - oi) * invD;
            float t1 = (m_AABB.max[i] - oi) * invD;
            if (t0 > t1) std::swap(t0, t1);

            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);
            if (tMin > tMax) return false;
        }

        return true;
    }
}

void BoundingBox::setAABB(const Mesh& mesh)
{
    if (mesh.m_VertexBuffer.empty())
        throw std::runtime_error("Mesh has no vertices");

    const auto& vb = mesh.m_VertexBuffer;
    glm::vec3 min = vb[0].pos;
    glm::vec3 max = min;

    for (const auto& v : vb) {
        const glm::vec3 p = v.pos;
        min = glm::min(min, p);
        max = glm::max(max, p);
    }

    glm::vec3 center = (min + max) * 0.5f;
    m_AABB.min = min;
    m_AABB.max = max;
    m_AABB.center = center;
}
