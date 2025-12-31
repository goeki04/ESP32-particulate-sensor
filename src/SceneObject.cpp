#include "pch.h"
#include "SceneObject.h"

void SceneObject::drawMesh()
{
    for (auto& mesh : m_Submeshes) {
        glUseProgram(m_Shader->m_Program);
        glm::mat4 localMatrix = m_Transform.localMatrix();
        m_Shader->setUniforms(mesh.getTextureID(),localMatrix);
        glBindVertexArray(mesh.m_Vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.m_IndexBuffer.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}