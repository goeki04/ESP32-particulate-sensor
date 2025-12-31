#include "pch.h"
#include "SceneObject.h"

void SceneObject::drawMesh()
{
    for (auto& mesh : m_Submeshes) {
        glUseProgram(m_Shader->m_Program);
        glm::mat4 localMatrix = m_Transform.localMatrix();
        static bool debug = false;
        if (debug == false) {
            std::cout << "\ndrawMesh" << std::endl;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    std::cout << localMatrix[i][j] << " ";
                }
                std::cout << " " << std::endl;
            }
            debug = true;
        }

        m_Shader->setUniforms(mesh.getTextureID(),localMatrix);
        glBindVertexArray(mesh.m_Vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.m_IndexBuffer.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}