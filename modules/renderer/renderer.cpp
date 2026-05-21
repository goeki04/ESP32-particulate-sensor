#include "Renderer.h"
#include "a_subsystem_manager.hpp"
#include "window_manager.hpp"
#include "resource_manager.h"
#include "a_math.hpp"
#include "scene.hpp"
#include "a_event_manager.hpp"
#include <a_CubemapData.hpp>
#include "OpenGL/a_GLcubemap.hpp"
#include "a_PrimitiveGenerator.hpp"
#include "OpenGL/a_opengl_upload.hpp"
using namespace Andromeda::ECS;
namespace Andromeda {

    void Renderer::initRenderer()
    {
        m_GLContext = std::make_unique<OpenGLContext>();
        m_RenderContext = m_GLContext.get();
        m_ResourceManager = SystemManager::getInstance().getSubsystem<ResourceManager>();
        m_SceneManager = SystemManager::getInstance().getSubsystem<SceneManager>();
        assert(m_ResourceManager && "ResourceManager is nullptr in Renderer::Start()");
        assert(m_SceneManager && "SceneManager is nullptr in Renderer::Start()");
        m_Cam = &m_SceneManager->m_EditorCamData;
    }

    void Renderer::start()
    {
        initRenderer();
        m_FramebufferSize = ivec2(Window::g_WindowWidth, Window::g_WindowHeight);

        if (m_FramebufferSize.x == 0 || m_FramebufferSize.y == 0) {
            throw std::runtime_error("Framebuffer has an start value of 0!!!");
        }
        m_Cam->framebufferSize = m_FramebufferSize;
        m_TexelSize = 1.0f / vec2(m_FramebufferSize.x, m_FramebufferSize.y);

        createFramebuffers();

        m_EnvironmentCubemapID = CubemapGL::AllocateEnvironmentMapTexture();
        auto& bakeShader = m_ResourceManager->m_BakingShaders[0];
        
        u32 hdrMapID = m_ResourceManager->m_CubemapData["citrus_orchard_road_puresky_4k"].textureID;
        Mesh cubeMesh;
        PrimitiveGenerator::generateCube(cubeMesh);

        createMesh(cubemapgpuHandle,cubeMesh);
        auto glBakingFBO = std::static_pointer_cast<GLFramebuffer>(m_BakingBuffer);
        u32 id = glBakingFBO->getFramebufferID();
        CubemapGL::ConvertEquiretangularToCubemap(
            *bakeShader,
            hdrMapID,
            id,
            m_EnvironmentCubemapID,
            [&]() { 
                m_RenderContext->drawIndexed(cubemapgpuHandle.vao,36);
            }
        );
        m_PostProcessScreenTriangle = m_RenderContext->createEmptyVAO();

        createMaterials();
        RenderPassSpecs initSpecs;
        m_RenderContext->setRenderPassSpecs(initSpecs);
        registerEvents();
    }

    void Renderer::createFramebuffers()
    {
        FramebufferSpecification msaaSpecs;
        msaaSpecs.width = m_FramebufferSize.x;
        msaaSpecs.height = m_FramebufferSize.y;
        msaaSpecs.samples = 4;
        msaaSpecs.attachments = {
            { FramebufferTextureFormat::RGBA8 },
            { FramebufferTextureFormat::DEPTH24Stencil8 }
        };
        m_MsaaBuffer = m_RenderContext->createFramebuffer(msaaSpecs);

        FramebufferSpecification sceneSpecs;
        sceneSpecs.width = m_FramebufferSize.x;
        sceneSpecs.height = m_FramebufferSize.y;
        sceneSpecs.samples = 1;
        sceneSpecs.attachments = {
            { FramebufferTextureFormat::RGBA8 }
        };
        m_SceneBuffer = m_RenderContext->createFramebuffer(sceneSpecs);

        FramebufferSpecification selectionSpecs;
        selectionSpecs.width = m_FramebufferSize.x;
        selectionSpecs.height = m_FramebufferSize.y;
        selectionSpecs.samples = 1;
        selectionSpecs.attachments = {
            { FramebufferTextureFormat::None },
            { FramebufferTextureFormat::DEPTH24Stencil8 }
        };
        m_SelectionBuffer = m_RenderContext->createFramebuffer(selectionSpecs);

        FramebufferSpecification postSpecs;
        postSpecs.width = m_FramebufferSize.x;
        postSpecs.height = m_FramebufferSize.y;
        postSpecs.samples = 1;
        postSpecs.attachments = {
            { FramebufferTextureFormat::RGBA8 }
        };
        m_PostprocessBuffer = m_RenderContext->createFramebuffer(postSpecs);

        FramebufferSpecification bakingSpecs;
        bakingSpecs.width = 512;
        bakingSpecs.height = 512;
        bakingSpecs.samples = 1;
        bakingSpecs.attachments = {
            { FramebufferTextureFormat::DEPTH32F },
            { FramebufferTextureFormat::RGBA16F}
        };
        m_BakingBuffer = m_RenderContext->createFramebuffer(bakingSpecs);
    }

    void Renderer::createMaterials()
    {
        ShaderProgramHandle testShaderHandle = m_ResourceManager->loadShaderRHI(m_RenderContext, "Test_Shader", SHADER_PATH "unlitVertex.glsl", SHADER_PATH "unlitFragment.glsl");
        auto testMaterial = m_ResourceManager->createMaterial("Standard", testShaderHandle, m_RenderContext);
        if (testMaterial) {
            testMaterial->setParameter("model", mat4(1.0f), ShaderDataType::Mat4);
            testMaterial->setParameter("sunLight.direction", vec3(0.5f, -1.0f, -0.5f), ShaderDataType::Vec3);
            testMaterial->setParameter("sunLight.color", vec3(1.0f, 1.0f, 1.0f), ShaderDataType::Vec3);
            testMaterial->setParameter("ambientLight", vec3(0.4f, 0.4f, 0.4f), ShaderDataType::Vec3);
        }

        ShaderProgramHandle outlineShaderHandle = m_ResourceManager->loadShaderRHI(m_RenderContext, "Outline_Shader", SHADER_PATH "outlineVertex.glsl", SHADER_PATH "outlineFragment.glsl");
        auto outlineMaterial = m_ResourceManager->createMaterial("OutlineMaterial", outlineShaderHandle, m_RenderContext);
        if (outlineMaterial) {
            outlineMaterial->setParameter("fboSampler", 0, ShaderDataType::Int);
            outlineMaterial->setParameter("maskSampler", 1, ShaderDataType::Int);
        }
        ShaderProgramHandle maskShaderHandle = m_ResourceManager->loadShaderRHI(m_RenderContext, "Mask_Shader", SHADER_PATH "maskVertex.glsl", SHADER_PATH "maskFrag.glsl");
        auto selectionMaterial = m_ResourceManager->createMaterial("SelectionMaterial", maskShaderHandle, m_RenderContext);
        if (selectionMaterial) {
            selectionMaterial->setParameter("model", mat4(1.0f), ShaderDataType::Mat4);
        }

        ShaderProgramHandle skyboxShaderHandle = m_ResourceManager->loadShaderRHI(m_RenderContext, "Skybox_Shader", SHADER_PATH "skyboxVertex.glsl", SHADER_PATH "skyboxFragment.glsl");
        auto skyboxMaterial = m_ResourceManager->createMaterial("SkyboxMaterial", skyboxShaderHandle, m_RenderContext);
        if (skyboxMaterial) {
            skyboxMaterial->setParameter("environmentMap", 0, ShaderDataType::Int);
        }
    }

    void Renderer::registerEvents()
    {
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
     
    void Renderer::destroy() {
        m_RenderContext->deleteVertexArrays(m_PostProcessScreenTriangle);
    }

    void Renderer::setActiveCamera(amath::CameraData* camData) {
        assert(camData && "CameraData is nullptr in Renderer::setActiveCamera()");
        m_Cam = camData;
    }

    void Renderer::geometryPass() const {

        RenderPassSpecs specs;
        if (m_WireframeActive) {
            specs.rasterizerMode = RasterizerMode::Wireframe;
            specs.cullMode = CullMode::None;
        }
        else {
            specs.rasterizerMode = RasterizerMode::Fill;
            specs.cullMode = CullMode::Back;
        }
        specs.depthTest = true;
        m_RenderContext->setRenderPassSpecs(specs);


        auto& meshPool = m_SceneManager->m_Registry.getPool<Component::MeshRenderer>();
        auto& transformPool = m_SceneManager->m_Registry.getPool<Component::Transform>();
        const auto& entitiesWithMesh = meshPool.getEntities();
        const auto& meshData = meshPool.data();

        for (size_t i = 0; i < entitiesWithMesh.size(); ++i) {
            Entity e = entitiesWithMesh[i];

            if (transformPool.has(e)) [[likely]] {
                const auto& meshComp = meshData[i];
                const auto& transform = transformPool.get(e);

                auto material = m_ResourceManager->getMaterial(meshComp.materialName);

                u32 vao = m_ResourceManager->getMeshVaoByID(meshComp.meshID);
                u32 indexCount = m_ResourceManager->getMeshIndexSizeByID(meshComp.meshID);
                if (material && vao != 0) [[likely]] {
                    material->setParameter("viewMatrix", m_Cam->viewMatrix, ShaderDataType::Mat4);
                    material->setParameter("projMatrix", m_Cam->projection, ShaderDataType::Mat4);
                    material->setParameter("model", transform.modelMatrix(), ShaderDataType::Mat4);

                    material->bind(m_RenderContext);
                    m_RenderContext->drawIndexed(vao, indexCount);
                }
            }
        }
        RenderPassSpecs resetSpecs;
        m_RenderContext->setRenderPassSpecs(resetSpecs);
    }

    void Renderer::createCubemapTexture(CubemapData& data) {
        if (m_GXAPI == AndromedaGXAPI::OpenGL) {
            CubemapGL::CubemapTextureUploadGL(data);
        }
    }

    u32 Renderer::getFinalSceneViewportTexture() const
    {
        return m_PostprocessBuffer->getColorAttachmentRendererID(0);
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

            m_MsaaBuffer->resize(m_FramebufferSize);
            m_SceneBuffer->resize(m_FramebufferSize);
            m_SelectionBuffer->resize(m_FramebufferSize);
            m_PostprocessBuffer->resize(m_FramebufferSize);

            m_ResizePending = false;
        }
    }
    void Renderer::selectionPass(Entity selectedEntity) const {
        m_RenderContext->bindFramebuffer(m_SelectionBuffer);
        m_RenderContext->clear(vec4(0.0f, 0.0f, 0.0f, 0.0f));

        if (selectedEntity == ECS::INVALID_ENTITY_ID) {
            m_RenderContext->unbindFramebuffer();
            return;
        }

        EntityHandle handle = { selectedEntity, &m_SceneManager->m_Registry };
        if (handle.has<Component::MeshRenderer>() && handle.has<Component::Transform>()) {

            const auto& meshComp = handle.get<Component::MeshRenderer>();
            const auto& transform = handle.get<Component::Transform>();

            auto material = m_ResourceManager->getMaterial("SelectionMaterial");
            u32 vao = m_ResourceManager->getMeshVaoByID(meshComp.meshID);
            u32 indexCount = m_ResourceManager->getMeshIndexSizeByID(meshComp.meshID);

            if (material && vao != 0) {

                RenderPassSpecs specs;
                specs.rasterizerMode = RasterizerMode::Fill;
                specs.cullMode = CullMode::None;
                specs.depthTest = true;
                specs.depthFunction = DepthFunc::LEqual;
                m_RenderContext->setRenderPassSpecs(specs);

                material->setParameter("viewMatrix", m_Cam->viewMatrix, ShaderDataType::Mat4);
                material->setParameter("projMatrix", m_Cam->projection, ShaderDataType::Mat4);
                material->setParameter("model", transform.modelMatrix(), ShaderDataType::Mat4);

                material->bind(m_RenderContext);
                m_RenderContext->drawIndexed(vao, indexCount);

                RenderPassSpecs resetSpecs;
                m_RenderContext->setRenderPassSpecs(resetSpecs);
            }
        }
        m_RenderContext->unbindFramebuffer();
    }
    void Renderer::postprocessingPass() const {
        m_RenderContext->bindFramebuffer(m_PostprocessBuffer);

        RenderPassSpecs specs;
        specs.depthTest = false;
        specs.blendMode = BlendMode::AlphaBlend;
        specs.cullMode = CullMode::None;
        m_RenderContext->setRenderPassSpecs(specs);

        auto outlineMat = m_ResourceManager->getMaterial("OutlineMaterial");

        if (outlineMat) [[likely]] {
            outlineMat->setParameter("texelSize", m_TexelSize, ShaderDataType::Vec2);
            outlineMat->setParameter("fboSize", vec2(m_FramebufferSize), ShaderDataType::Vec2);

            outlineMat->bind(m_RenderContext);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_SceneBuffer->getColorAttachmentRendererID(0));

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_SelectionBuffer->getColorAttachmentRendererID(0));

            m_RenderContext->drawArrays(m_PostProcessScreenTriangle, 3);
        }

        RenderPassSpecs resetSpecs;
        m_RenderContext->setRenderPassSpecs(resetSpecs);
        m_RenderContext->unbindFramebuffer();
    }
    void Renderer::scenePassBegin() const {
        m_RenderContext->bindFramebuffer(m_MsaaBuffer);
        m_RenderContext->clear(vec4(0.2f, 0.2f, 0.35f, 1.0f));

    }
    void Renderer::proceduralPass() const {
        RenderPassSpecs specs;
        specs.cullMode = CullMode::None;
        specs.depthTest = true;
        specs.depthFunction = DepthFunc::LEqual;
        specs.blendMode = BlendMode::AlphaBlend;
        m_RenderContext->setRenderPassSpecs(specs);

        auto skyboxMat = m_ResourceManager->getMaterial("SkyboxMaterial");
        if (skyboxMat) [[likely]] {
            skyboxMat->setParameter("view", m_Cam->viewMatrix, ShaderDataType::Mat4);
            skyboxMat->setParameter("proj", m_Cam->projection, ShaderDataType::Mat4);

            skyboxMat->bind(m_RenderContext);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvironmentCubemapID);

            m_RenderContext->drawIndexed(cubemapgpuHandle.vao, 36);
        }
        RenderPassSpecs resetSpecs;
        m_RenderContext->setRenderPassSpecs(resetSpecs);
    }

    void Renderer::scenePassEndResolve() const {
        m_RenderContext->blitFramebuffer(m_MsaaBuffer, m_SceneBuffer, false);
        m_RenderContext->unbindFramebuffer();
    }

    void Renderer::windowClearPass()
    {
        m_RenderContext->setViewport(0,0,Window::g_WindowWidth, Window::g_WindowHeight);
        m_RenderContext->clear(vec4(0.10f, 0.10f, 0.10f, 1.0f));
    }
}