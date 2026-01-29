#include "pch.h"
#include "systems.h"
#include "ResourceManager.h"
namespace ECS::system {
    void MeshRenderingSystem::drawMesh(ResourceManager* rm,component::Mesh& mesh, component::Transform& transform)
    {
        auto* sh = rm->getMaterialShaderByID(mesh.type);
        sh->use();
        glm::mat4 localMatrix = transform.modelMatrix();
        sh->setUniforms(localMatrix);
        glBindVertexArray(rm->getMeshVaoByID(mesh.meshID));
        glDrawElements(GL_TRIANGLES, rm->getMeshIndexSizeByID(mesh.meshID), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
