#include "pch.h"
#include "SceneObject.h"

void SceneObject::drawMesh()
{
    for (auto& mesh : m_Submeshes) {
        glUseProgram(m_Shader->m_Program);
        glm::mat4 modelMatrix = mesh.m_ModelMatrix.localMatrix();
        m_Shader->setUniforms(mesh.getTextureID(), modelMatrix);
        glBindVertexArray(mesh.m_Vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.m_IndexBuffer.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}