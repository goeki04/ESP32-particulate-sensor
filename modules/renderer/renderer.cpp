#include "Renderer.h"
#include "a_subsystem_manager.hpp"
#include "window_manager.hpp"
#include "resource_manager.h"
#include "a_math.hpp"
#include "scene.hpp"
#include "a_event_manager.hpp"
#include "a_CubemapData.hpp"
#include "a_GLcubemap.hpp"
using namespace Andromeda::ECS;
namespace Andromeda {
    void Renderer::start()
    {
        m_ResourceManager = SystemManager::getInstance().getSubsystem<ResourceManager>();
        m_SceneManager = SystemManager::getInstance().getSubsystem<SceneManager>();
        assert(m_ResourceManager && "ResourceManager is nullptr in Renderer::Start()");
        assert(m_SceneManager && "SceneManager is nullptr in Renderer::Start()");
        m_Cam = &m_SceneManager->m_EditorCamData;
        m_FramebufferSize = ivec2(Window::g_WindowWidth, Window::g_WindowHeight);

        if (m_FramebufferSize.x == 0 || m_FramebufferSize.y == 0) {
            throw std::runtime_error("Framebuffer has an start value of 0!!!");
        }
        m_Cam->framebufferSize = m_FramebufferSize;
        m_TexelSize = 1.0f / vec2(m_FramebufferSize.x, m_FramebufferSize.y);

        fboManager.createFramebuffers(m_FramebufferSize,static_cast<int>(m_MSAAsamples));
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glGenVertexArrays(1, &m_Vao);
        EventManager::getInstance().AddEventListener<SceneObjectSelected>([this](const SceneObjectSelected& event) {
            this->m_SelectedForHighlighting = event.entity;
            });

        EventManager::getInstance().AddEventListener<OnEnableWireFrame>([this](const OnEnableWireFrame& event) {
            this->m_WireframeActive = event.state;
            });
    }

    void Renderer::update()
    {
        processResizeTimer();
        windowClearPass();
        scenePassBegin();
        geometryPass();
        proceduralPass();
        scenePassEndResolve();
        selectionPass(m_SelectedForHighlighting);
        postprocessingPass();
    }

    void Renderer::drawMesh(const Component::Mesh& mesh, const Component::Transform& transform) const {
        auto* sh = m_ResourceManager->getMaterialShaderByID(MaterialShaderType::white);
        sh->use();
        const mat4 localMatrix = transform.modelMatrix();
        sh->setUniforms(m_Cam,localMatrix);
        glBindVertexArray(m_ResourceManager->getMeshVaoByID(mesh.meshID));
        glDrawElements(GL_TRIANGLES, m_ResourceManager->getMeshIndexSizeByID(mesh.meshID), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Renderer::drawMesh(const Component::Mesh& mesh, const Component::Transform& transform, MaterialShaderType type) const {
        auto* sh = m_ResourceManager->getMaterialShaderByID(type);
        sh->use();
        const mat4 localMatrix = transform.modelMatrix();

        sh->setUniforms(m_Cam,localMatrix);
        glBindVertexArray(m_ResourceManager->getMeshVaoByID(mesh.meshID));
        glDrawElements(GL_TRIANGLES, m_ResourceManager->getMeshIndexSizeByID(mesh.meshID), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
     
    void Renderer::destroy() {
        fboManager.destroyFramebuffers();
        if (m_Vao) {
            glDeleteVertexArrays(1, &m_Vao);
        }
        m_Vao = 0;
    }

    void Renderer::setActiveCamera(amath::CameraData* camData) {
        assert(camData && "CameraData is nullptr in Renderer::setActiveCamera()");
        m_Cam = camData;
    }

    void Renderer::geometryPass() const {
        if (m_WireframeActive) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);
        }
        auto& meshPool = m_SceneManager->m_Registry.getPool<Component::Mesh>();
        auto& transformPool = m_SceneManager->m_Registry.getPool<Component::Transform>();
        const auto& entitiesWithMesh = meshPool.getEntities();
        const auto& meshData = meshPool.data();

        for (size_t i = 0; i < entitiesWithMesh.size(); ++i) {
            Entity e = entitiesWithMesh[i];
            if (transformPool.has(e)) {
                drawMesh(meshData[i], transformPool.get(e), MaterialShaderType::unlit);
            }
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
    }

    void Renderer::createCubemapTexture(const CubemapData& data) {
         CubemapGL::CreateCubemapTextureGL(data);
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
            m_TexelSize = 1.0f / vec2(static_cast<float>(m_FramebufferSize.x), static_cast<float>(m_FramebufferSize.y));
            m_Cam->framebufferSize = m_FramebufferSize;

            fboManager.destroyFramebuffers();
            fboManager.createFramebuffers(m_FramebufferSize, static_cast<int>(m_MSAAsamples));

            m_ResizePending = false;
        }
    }
    void Renderer::selectionPass(Entity selectedEntity) const {
        glBindFramebuffer(GL_FRAMEBUFFER, fboManager.m_SelectionFramebuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (selectedEntity == ECS::INVALID_ENTITY_ID) { return; }
        EntityHandle handle = { selectedEntity, &m_SceneManager->m_Registry };
        if (handle.has<Component::Mesh>() && handle.has<Component::Transform>()) {
            drawMesh(handle.get<Component::Mesh>(), handle.get<Component::Transform>(),
                MaterialShaderType::white);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void Renderer::postprocessingPass() const {
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
    void Renderer::scenePassBegin() const {
        glBindFramebuffer(GL_FRAMEBUFFER, fboManager.m_MsaaFramebuffer);
        glViewport(0, 0, m_FramebufferSize.x, m_FramebufferSize.y);
        glClearColor(0.2f, 0.2f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    }
    void Renderer::proceduralPass() const {
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

    void Renderer::scenePassEndResolve() const {
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
}