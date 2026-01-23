#include "Renderer.h"
#include "pch.h"
#include "SubsystemManager.h"
#include "WindowManager.h"
#include "ResourceManager.h"
#include "SceneObject.h"
#include "camera.h"

void Renderer::start()
{
    m_ResourceManager = SystemManager::getInstance().getSubsystem<ResourceManager>();
    m_GuiManager.init(Window::g_Window, m_ResourceManager);
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
void Renderer::geometryPass() {
    for (auto& sceneObject : m_ResourceManager->getEntitys()) {
        sceneObject.drawMesh();
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
    SDL_GL_SwapWindow(Window::g_Window);
}
void Renderer::selectionPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_SelectionFramebuffer);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& sceneObject : m_ResourceManager->getEntitys()) {
        if (sceneObject.m_IsSelected) {
            sceneObject.drawMesh(MaterialShaderType::white);
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

    glClearColor(0.518f, 0.506f, 0.478f, 1.0f);
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
        bool anyHit = false;
        for (auto& sceneObject : m_ResourceManager->getEntitys()) {
            const glm::mat4 modelMatrix = sceneObject.m_Transform.modelMatrix();
            bool hit = sceneObject.m_BoundingBox.RayIntersectAABB(cam, modelMatrix);
            sceneObject.m_IsSelected = hit;

            if (hit) {
                m_GuiManager.m_CurrentSelectedID = sceneObject.m_ID;
                anyHit = true;
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
    glClearColor(0.10f, 0.12f, 0.16f, 1.0f);
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