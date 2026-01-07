#include "pch.h"
#include "SceneObject.h"

void SceneObject::drawMesh()
{
        glUseProgram(m_Shader->m_Program);
        glm::mat4 localMatrix = m_Transform.localMatrix();
        m_Shader->setUniforms(localMatrix);
        glBindVertexArray(m_Mesh.m_Vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Mesh.m_IndexBuffer.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
}