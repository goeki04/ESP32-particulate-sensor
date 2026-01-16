#include "pch.h"
#include "SceneObject.h"
#include "ResourceManager.h"

Entity::Entity(ResourceManager* rm,unsigned int meshID, unsigned int objectID,const std::string& name,Transform& transform) : m_ResourceManager(rm), m_MeshID(meshID), m_ID(objectID), m_Name(name), m_Transform(transform){
    m_BoundingBox.setAABB(m_ResourceManager->getMeshByID(m_MeshID));
}
void Entity::drawMesh()
{
    if (!m_ResourceManager) {
        throw std::runtime_error("ResourceManager ptr is null");
    }
    auto* sh = m_ResourceManager->getMaterialShaderByID(m_Shadertype);
    sh->use();
    glm::mat4 localMatrix = m_Transform.modelMatrix();
    sh->setUniforms(localMatrix);
    glBindVertexArray(m_ResourceManager->getMeshVaoByID(m_MeshID));
    glDrawElements(GL_TRIANGLES, m_ResourceManager->getMeshIndexSizeByID(m_MeshID), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Entity::drawMeshOutline() {
    if (!m_ResourceManager) {
        throw std::runtime_error("ResourceManager ptr is null");
    }
    auto* sh = m_ResourceManager->getMaterialShaderByID(MaterialShaderType::outline);
    sh->use();
    glm::mat4 M = m_Transform.modelMatrix();
    glm::vec3 center = m_BoundingBox.getAABB().center;

    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.03f));
    glm::mat4 T1 = glm::translate(glm::mat4(1.0f), center);
    glm::mat4 T2 = glm::translate(glm::mat4(1.0f), -center);
    glm::mat4 outlineModel = M * T1 * S * T2;
    sh->setUniforms(outlineModel);
    glBindVertexArray(m_ResourceManager->getMeshVaoByID(m_MeshID));
    glDrawElements(GL_TRIANGLES, m_ResourceManager->getMeshIndexSizeByID(m_MeshID), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

const AABB& BoundingBox::getAABB() const
{
    return m_AABB;
}

bool BoundingBox::RayIntersectAABB(const Camera& cam, const glm::mat4& modelMatrix)
{
    const Ray& rayW = cam.m_CursorToWorldRay;

    glm::mat4 invModel = glm::inverse(modelMatrix);

    glm::vec3 o = glm::vec3(invModel * glm::vec4(rayW.origin, 1.0f));
    glm::vec3 d = glm::normalize(glm::vec3(invModel * glm::vec4(rayW.direction, 0.0f)));

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
