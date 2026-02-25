#include "Renderer.h"
#include "pch.h"
#include "SubsystemManager.h"
#include "WindowManager.h"
#include "ResourceManager.h"
#include "camera.h"
#include "Shader.h"
#include "Collision.h"
using namespace ECS;
void Renderer::start()
{
    m_ResourceManager = SystemManager::getInstance().getSubsystem<ResourceManager>();
    m_Registry = SystemManager::getInstance().getSubsystem<ComponentRegistry>();

    m_GuiManager.init(Window::g_Window, m_ResourceManager,m_Registry);
    ImVec2 viewportWindowSize = m_GuiManager.getViewportWindowSize();
    m_FramebufferSize = glm::ivec2(viewportWindowSize.x, viewportWindowSize.y);
    m_TexelSize = 1.0f / glm::vec2(m_FramebufferSize.x, m_FramebufferSize.y);
    m_ResourceManager->m_Cam.m_framebufferSize = m_FramebufferSize;
    ImGui_ImplSDL3_InitForOpenGL(Window::g_Window, m_ResourceManager->m_GlContext);
    ImGui_ImplOpenGL3_Init(Renderer::glsl_version);
    createFramebuffers();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glGenVertexArrays(1, &m_Vao);
}
void Renderer::update()
{
    Camera& cam = m_ResourceManager->m_Cam;
    windowClearPass();
    scenePassBegin();
    geometryPass();
    proceduralPass();
    scenePassEndResolve();
    selectionPass();
    postprocessingPass();
    guiPass(cam);
    pickingPass(cam);
    imGuiPass();
}

void Renderer::drawMesh(ResourceManager* rm,const component::Mesh& mesh,const component::Transform& transform)
{
    auto* sh = rm->getMaterialShaderByID(MaterialShaderType::white);
    sh->use();
    glm::mat4 localMatrix = transform.modelMatrix();
    sh->setUniforms(localMatrix);
    glBindVertexArray(rm->getMeshVaoByID(mesh.meshID));
    glDrawElements(GL_TRIANGLES, rm->getMeshIndexSizeByID(mesh.meshID), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::drawMesh(ResourceManager* rm, const component::Mesh& mesh, const component::Transform& transform,MaterialShaderType type)
{
    auto* sh = rm->getMaterialShaderByID(type);
    sh->use();
    glm::mat4 localMatrix = transform.modelMatrix();
    sh->setUniforms(localMatrix);
    glBindVertexArray(rm->getMeshVaoByID(mesh.meshID));
    glDrawElements(GL_TRIANGLES, rm->getMeshIndexSizeByID(mesh.meshID), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::geometryPass() {
    auto& meshPool = m_Registry->getPool<component::Mesh>();
    auto& transformPool = m_Registry->getPool<component::Transform>();
    const auto& entitiesWithMesh = meshPool.getEntities();
    const auto& meshData = meshPool.data();

    for (size_t i = 0; i < entitiesWithMesh.size(); ++i) {
        Entity e = entitiesWithMesh[i];
        if (transformPool.has(e)) {
            drawMesh(m_ResourceManager, meshData[i], transformPool.get(e),MaterialShaderType::unlit);
        }
    }
}
void Renderer::guiPass(Camera& cam)
{
    m_GuiManager.update();

    m_GuiManager.drawViewportGUI(
        m_PostprocessTexture,
        ImVec2(m_FramebufferSize.x, m_FramebufferSize.y),
        &cam.m_ImGuiMouseX,
        &cam.m_ImGuiMouseY
    );
    
    ImGui::Render();
}
void Renderer::imGuiPass()
{
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
    SDL_GL_SwapWindow(Window::g_Window);
}
void Renderer::selectionPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_SelectionFramebuffer);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto& selectedPool = m_Registry->getPool<component::Selected>();
    for (Entity e : selectedPool.getEntities()) {
        EntityHandle handle = { e, m_Registry };
        if (handle.has<component::Mesh>() && handle.has<component::Transform>()) {
            drawMesh(
                m_ResourceManager, 
                handle.get<component::Mesh>(), 
                handle.get<component::Transform>(), MaterialShaderType::white
            );
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::postprocessingPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_PostprocessFramebuffer);
    glViewport(0, 0, m_FramebufferSize.x, m_FramebufferSize.y);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto* outlineShader = m_ResourceManager->getPostprocessShaderByID(PostProcessShaderType::outline);
    outlineShader->use();
    outlineShader->setTexture("fboSampler",  m_FramebufferTexture, 0);
    outlineShader->setTexture("maskSampler", m_SelectionTexture, 1);
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
    glBindFramebuffer(GL_FRAMEBUFFER, m_MsaaFramebuffer);
    ImVec2 viewportSize = m_GuiManager.getViewportWindowSize();
    glViewport(0, 0, (GLsizei)viewportSize.x, (GLsizei)viewportSize.y);

    glClearColor(0.2f, 0.2f, 0.35f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
}
void Renderer::proceduralPass()
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (auto& pSh : m_ResourceManager->m_ProceduralShaders) {
        pSh->use();
        glBindVertexArray(m_Vao);
        pSh->setUniforms();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
}
void Renderer::pickingPass(const Camera& cam)
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        auto& aabbPool = m_Registry->getPool<component::AABB>();
        auto& selectedPool = m_Registry->getPool<component::Selected>();
        std::vector<Entity> currentlySelected = selectedPool.getEntities();
        for (Entity e : currentlySelected) {
            m_Registry->getPool<component::Selected>().removeEntity(e);
        }
        bool anyHit = false;


        for (Entity e : aabbPool.getEntities()) {
            EntityHandle handle = { e,m_Registry };
            if (!handle.has<component::Transform>()) {
                continue;
            }
            const glm::mat4 modelMatrix = handle.get<component::Transform>().modelMatrix();
            const auto& aabb = handle.get<component::AABB>();

            if (collision::RayIntersectAABB(cam, aabb, modelMatrix)) {
                handle.add<component::Selected>({});
                m_GuiManager.m_CurrentSelectedID = e;
                anyHit = true;
                break;
            }
        }

        if (!anyHit) {
            m_GuiManager.m_CurrentSelectedID = -1;
        }
    }
}
void Renderer::scenePassEndResolve()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MsaaFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer);

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
void Renderer::createSceneFbo()
{
    //Color attachment & main fbo
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glGenTextures(1, &m_FramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_FramebufferSize.x, m_FramebufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::createMSAAFbo()
{
    glGenFramebuffers(1, &m_MsaaFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_MsaaFramebuffer);
    glGenTextures(1, &m_MsaaFramebufferTexture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MsaaFramebufferTexture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_MSAAsamples, GL_RGB, m_FramebufferSize.x, m_FramebufferSize.y, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_MsaaFramebufferTexture, 0);

    glGenRenderbuffers(1, &m_Rendererbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_Rendererbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_MSAAsamples, GL_DEPTH24_STENCIL8, m_FramebufferSize.x, m_FramebufferSize.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rendererbuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("MSAA FBO incomplete!");
}
void Renderer::createSelectionFBO()
{
    //create selection framebuffer
    glGenFramebuffers(1, &m_SelectionFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_SelectionFramebuffer);

    glGenTextures(1, &m_SelectionTexture);
    glBindTexture(GL_TEXTURE_2D, m_SelectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_FramebufferSize.x, m_FramebufferSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SelectionTexture, 0);

    glGenRenderbuffers(1, &m_SelectionDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_SelectionDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_FramebufferSize.x, m_FramebufferSize.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_SelectionDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Selection Framebuffer incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::createPostprocessFBO() {
    glGenFramebuffers(1, &m_PostprocessFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_PostprocessFramebuffer);

    glGenTextures(1, &m_PostprocessTexture);
    glBindTexture(GL_TEXTURE_2D, m_PostprocessTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_FramebufferSize.x, m_FramebufferSize.y,
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PostprocessTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Postprocess FBO incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
/// <summary>
/// Call this function after initializing guimanager to create offscreen framebuffers.
/// </summary>
void Renderer::createFramebuffers()
{
    createMSAAFbo();
    createSceneFbo();
    createSelectionFBO();
    createPostprocessFBO();
}   
void Renderer::destroy() {
    if (m_FramebufferTexture)        
        glDeleteTextures(1, &m_FramebufferTexture);
    if (m_PostprocessTexture) {
        glDeleteTextures(1, &m_PostprocessTexture);
    }
    if (m_MsaaFramebufferTexture)    
        glDeleteTextures(1, &m_MsaaFramebufferTexture);
    if (m_SelectionTexture) {
        glDeleteTextures(1, &m_SelectionTexture);
    }
    if (m_Rendererbuffer)           
        glDeleteRenderbuffers(1, &m_Rendererbuffer);
    if (m_SelectionDepth) {
        glDeleteRenderbuffers(1, &m_SelectionDepth);
    }
    if (m_Framebuffer)              
        glDeleteFramebuffers(1, &m_Framebuffer);
    if (m_MsaaFramebuffer)          
        glDeleteFramebuffers(1, &m_MsaaFramebuffer);
    if (m_PostprocessFramebuffer) {
        glDeleteFramebuffers(1, &m_PostprocessFramebuffer);
    }
    if (m_SelectionFramebuffer) {
        glDeleteFramebuffers(1,&m_SelectionFramebuffer);
    }
    if (m_Vao) {
        glDeleteVertexArrays(1,&m_Vao);
    }
    m_Vao = 0;
    m_SelectionDepth = 0;
    m_SelectionFramebuffer = 0;
    m_PostprocessFramebuffer = 0;
    m_PostprocessTexture = 0;
    m_SelectionTexture = 0;
    m_FramebufferTexture = 0;
    m_MsaaFramebufferTexture = 0;
    m_Rendererbuffer = 0;
    m_Framebuffer = 0;
    m_MsaaFramebuffer = 0;

    m_GuiManager.destroy();
}