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
    m_framebufferSize = glm::ivec2(viewportWindowSize.x, viewportWindowSize.y);
    m_ResourceManager->m_Cam.m_framebufferSize = m_framebufferSize;
    ImGui_ImplSDL3_InitForOpenGL(Window::g_Window, m_ResourceManager->m_GlContext);
    ImGui_ImplOpenGL3_Init(Renderer::glsl_version);
    createFramebuffer();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void Renderer::update()
{
    Camera& cam = m_ResourceManager->m_Cam;
    m_GuiManager.update();
    m_GuiManager.drawViewportGUI(m_FramebufferTexture, ImVec2(m_framebufferSize.x,m_framebufferSize.y),&cam.m_ImGuiMouseX,&cam.m_ImGuiMouseY);
    ImGui::Render();
    glViewport(0, 0, Window::g_WindowWidth, Window::g_WindowHeight);
    glClearColor(0.10f, 0.12f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_MsaaFramebuffer);
    ImVec2 viewportSize = m_GuiManager.getViewportWindowSize();
    glViewport(0,0,(GLsizei)viewportSize.x, (GLsizei)viewportSize.y);
    glClearColor(0.518f, 0.506f, 0.478f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto& sceneObject : m_ResourceManager->m_SceneObjects) {
        sceneObject.drawMesh();
        if (m_ResourceManager->m_Cam.m_CursorToWorldRayEnabled) {
            const glm::mat4 modelMatrix = sceneObject.m_Transform.modelMatrix();
            bool hit = sceneObject.m_BoundingBox.RayIntersectAABB(cam,modelMatrix);
        }
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MsaaFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer);
    glBlitFramebuffer(0, 0, m_framebufferSize.x, m_framebufferSize.y, 0, 0, m_framebufferSize.x, m_framebufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(Window::g_Window);
}
/// <summary>
/// Call this function after initializing guimanager to create offscreen framebuffers.
/// </summary>
void Renderer::createFramebuffer()
{
    //MSAA Framebuffer;
    glGenFramebuffers(1, &m_MsaaFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,m_MsaaFramebuffer);
    glGenTextures(1, &m_MsaaFramebufferTexture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MsaaFramebufferTexture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_MSAAsamples, GL_RGB, m_framebufferSize.x, m_framebufferSize.y, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D_MULTISAMPLE,m_MsaaFramebufferTexture,0);
    
    //Depth and stencil information
    glGenRenderbuffers(1, &m_Rendererbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_Rendererbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_MSAAsamples, GL_DEPTH24_STENCIL8, m_framebufferSize.x, m_framebufferSize.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rendererbuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("MSAA FBO incomplete!");
    
    //Color attachment & main fbo
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glGenTextures(1, &m_FramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_framebufferSize.x, m_framebufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferTexture, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}   

void Renderer::destroy() {
    if (m_FramebufferTexture)        
        glDeleteTextures(1, &m_FramebufferTexture);
    if (m_MsaaFramebufferTexture)    
        glDeleteTextures(1, &m_MsaaFramebufferTexture);
    if (m_Rendererbuffer)           
        glDeleteRenderbuffers(1, &m_Rendererbuffer);

    if (m_Framebuffer)              
        glDeleteFramebuffers(1, &m_Framebuffer);
    if (m_MsaaFramebuffer)          
        glDeleteFramebuffers(1, &m_MsaaFramebuffer);

    m_FramebufferTexture = 0;
    m_MsaaFramebufferTexture = 0;
    m_Rendererbuffer = 0;
    m_Framebuffer = 0;
    m_MsaaFramebuffer = 0;

    m_GuiManager.destroy();
}