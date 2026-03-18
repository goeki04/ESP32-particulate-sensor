#include "Renderer.h"
#include "subsystem_manager.h"
#include "window_manager.h"
#include "resource_manager.h"
#include "a_math.hpp"
#include "scene.hpp"
#include <iostream>
using namespace Andromeda::ECS;
namespace Andromeda {
    void Renderer::start()
    {
        m_ResourceManager = SystemManager::getInstance().getSubsystem<ResourceManager>();
        m_Scene = SystemManager::getInstance().getSubsystem<Scene>();
        
        //m_GuiManager.init(Window::g_Window, &m_Cam, m_Registry);
        m_FramebufferSize = ivec2(Window::g_WindowWidth, Window::g_WindowHeight);
        if (m_FramebufferSize.x == 0 || m_FramebufferSize.y == 0) {
            throw std::runtime_error("Framebuffer has an start value of 0!!!");
        }
        m_TexelSize = 1.0f / vec2(m_FramebufferSize.x, m_FramebufferSize.y);

        fboManager.createFramebuffers(m_FramebufferSize,(int)m_MSAAsamples);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glGenVertexArrays(1, &m_Vao);
    }

    u32 Renderer::getFinalSceneViewportTexture() const
    {
        return fboManager.m_PostprocessTexture;
    }

    void Renderer::onViewportResize(ivec2 newSize)
    {
        if (newSize.x <= 0 || newSize.y <= 0) return;
        if (newSize == m_FramebufferSize) return;

        m_TargetSize = newSize;
        m_ResizePending = true;
        m_ResizeTimer = 0.15f;
    }

    void Renderer::processResizeTimer()
    {
        if (!m_ResizePending) return;

        m_ResizeTimer -= SystemManager::s_deltaTime;
        if (m_ResizeTimer <= 0.0f)
        {
            m_FramebufferSize = m_TargetSize;
            m_TexelSize = 1.0f / vec2((float)m_FramebufferSize.x, (float)m_FramebufferSize.y);
            m_Cam->m_framebufferSize = m_FramebufferSize;

            fboManager.destroyFramebuffers();
            fboManager.createFramebuffers(m_FramebufferSize, (int)m_MSAAsamples);

            m_ResizePending = false;
        }
    }

    void Renderer::update()
    {

        /*if (Gui::GuiRenderer::s_ViewportFocused)
            m_Cam.cameraMovement();*/
        windowClearPass();
        scenePassBegin();
        geometryPass();
        proceduralPass();
        scenePassEndResolve();
        selectionPass();
        postprocessingPass();
        guiPass(m_Cam);
        pickingPass(m_Cam);
        imGuiPass();
    }



    void Renderer::drawMesh(ResourceManager* rm, const Component::Mesh& mesh, const Component::Transform& transform)
    {
        auto* sh = rm->getMaterialShaderByID(MaterialShaderType::white);
        sh->use();
        glm::mat4 localMatrix = transform.modelMatrix();
        sh->setUniforms(m_Cam,localMatrix);
        glBindVertexArray(rm->getMeshVaoByID(mesh.meshID));
        glDrawElements(GL_TRIANGLES, rm->getMeshIndexSizeByID(mesh.meshID), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Renderer::drawMesh(ResourceManager* rm, const Component::Mesh& mesh, const Component::Transform& transform, MaterialShaderType type)
    {
        auto* sh = rm->getMaterialShaderByID(type);
        sh->use();
        glm::mat4 localMatrix = transform.modelMatrix();
        sh->setUniforms(m_Cam,localMatrix);
        glBindVertexArray(rm->getMeshVaoByID(mesh.meshID));
        glDrawElements(GL_TRIANGLES, rm->getMeshIndexSizeByID(mesh.meshID), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Renderer::geometryPass() {
        auto& meshPool = m_Scene->m_Registry.getPool<Component::Mesh>();
        auto& transformPool = m_Scene->m_Registry.getPool<Component::Transform>();
        const auto& entitiesWithMesh = meshPool.getEntities();
        const auto& meshData = meshPool.data();

        for (size_t i = 0; i < entitiesWithMesh.size(); ++i) {
            Entity e = entitiesWithMesh[i];
            if (transformPool.has(e)) {
                drawMesh(m_ResourceManager, meshData[i], transformPool.get(e), MaterialShaderType::unlit);
            }
        }
    }
    void Renderer::guiPass(amath::CameraData* cam)
    {
        //m_GuiManager.update();

        /*Gui::Panels::drawViewportGUI(
            m_GuiManager,
            m_PostprocessTexture,
            ImVec2(m_FramebufferSize.x, m_FramebufferSize.y),
            &cam.m_ImGuiMouseX,
            &cam.m_ImGuiMouseY
        );
        

        ImGui::Render();
        */
    }
    void Renderer::imGuiPass()
    {
        /*
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();

            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();

            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
        SDL_GL_SwapWindow(Window::g_Window);*/
    }
    void Renderer::selectionPass()
    {
        /*
        glBindFramebuffer(GL_FRAMEBUFFER, fboManager.m_SelectionFramebuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto& selectedPool = m_Registry->getPool<Component::Selected>();
        for (Entity e : selectedPool.getEntities()) {
            EntityHandle handle = { e, m_Registry };
            if (handle.has<Component::Mesh>() && handle.has<Component::Transform>()) {
                drawMesh(
                    m_ResourceManager,
                    handle.get<Component::Mesh>(),
                    handle.get<Component::Transform>(), MaterialShaderType::white
                );
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        */
    }
    void Renderer::postprocessingPass()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fboManager.m_PostprocessFramebuffer);
        glViewport(0, 0, m_FramebufferSize.x, m_FramebufferSize.y);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        auto* outlineShader = m_ResourceManager->getPostprocessShaderByID(PostProcessShaderType::outline);
        outlineShader->use();
        outlineShader->setTexture("fboSampler", fboManager.m_FramebufferTexture, 0);
        outlineShader->setTexture("maskSampler", fboManager.m_SelectionTexture, 1);
        outlineShader->setVec2("texelSize", m_TexelSize);
        outlineShader->setVec2("fboSize", m_FramebufferSize);

        glBindVertexArray(m_Vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void Renderer::scenePassBegin()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fboManager.m_MsaaFramebuffer);
        glViewport(0, 0, m_FramebufferSize.x, m_FramebufferSize.y);

        glClearColor(0.2f, 0.2f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    }
    void Renderer::proceduralPass()
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (auto& pSh : m_ResourceManager->m_ProceduralShaders) {
            pSh->use();
            glBindVertexArray(m_Vao);
            pSh->setUniforms(m_Cam);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }
    void Renderer::pickingPass(const amath::CameraData* cam)
    {
        /*
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            auto& aabbPool = m_Registry->getPool<Component::AABB>();
            auto& selectedPool = m_Registry->getPool<Component::Selected>();
            std::vector<Entity> currentlySelected = selectedPool.getEntities();
            for (Entity e : currentlySelected) {
                m_Registry->getPool<Component::Selected>().removeEntity(e);
            }
            bool anyHit = false;


            for (Entity e : aabbPool.getEntities()) {
                EntityHandle handle = { e,m_Registry };
                if (!handle.has<Component::Transform>()) {
                    continue;
                }
                const glm::mat4 modelMatrix = handle.get<Component::Transform>().modelMatrix();
                const auto& aabb = handle.get<Component::AABB>();

                if (Collision::RayIntersectAABB(cam, aabb, modelMatrix)) {
                    handle.add<Component::Selected>({});
                    m_GuiManager.m_CurrentSelectedID = e;
                    anyHit = true;
                    break;
                }
            }

            if (!anyHit) {
                m_GuiManager.m_CurrentSelectedID = -1;
            }
        }
        */
    }
    void Renderer::scenePassEndResolve()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fboManager.m_MsaaFramebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboManager.m_Framebuffer);

        glBlitFramebuffer(
            0, 0, m_FramebufferSize.x, m_FramebufferSize.y,
            0, 0, m_FramebufferSize.x, m_FramebufferSize.y,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void Renderer::windowClearPass()
    {
        glViewport(0, 0, Window::g_WindowWidth, Window::g_WindowHeight);
        glClearColor(0.10f, 0.10f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }


    void Renderer::destroy() {
        fboManager.destroyFramebuffers();
        if (m_Vao) {
            glDeleteVertexArrays(1, &m_Vao);
        }
        m_Vao = 0;
    }
}