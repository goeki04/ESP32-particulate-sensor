#include "Renderer.h"
#include "pch.h"
#include "SubsystemManager.h"
#include "WindowManager.h"
#include "ResourceManager.h"
const constexpr char* modelPath = "../assets/models/cube.obj";

void Renderer::start()
{
	m_WindowManager = SystemManager::getInstance().getSubsystem<WindowManager>();
    m_ResourceManager = SystemManager::getInstance().getSubsystem<ResourceManager>();
    m_GuiManager.init(m_WindowManager->m_Window);
    ImVec2 viewportWindowSize = m_GuiManager.getViewportWindowSize();
    m_framebufferSize = glm::ivec2(viewportWindowSize.x, viewportWindowSize.y);
    ImGui_ImplSDL3_InitForOpenGL(m_WindowManager->m_Window, m_ResourceManager->m_GlContext);
    ImGui_ImplOpenGL3_Init(Renderer::glsl_version);
    createFramebuffer();
}

void Renderer::update()
{
    m_GuiManager.update();
    m_GuiManager.drawViewportGUI(m_FramebufferTexture, ImVec2(m_framebufferSize.x,m_framebufferSize.y));
    ImGui::Render();
    glViewport(0, 0, m_WindowManager->m_WindowWidth, m_WindowManager->m_WindowHeight);
    glClearColor(0.172f, 0.172f, 0.329f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER,m_Framebuffer);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (auto& mesh : m_ResourceManager->m_Meshes) {
        mesh.drawMesh();
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_WindowManager->m_Window);
}


/// <summary>
/// Call this function after initializing guimanager to create an offscreen framebuffer.
/// </summary>
void Renderer::createFramebuffer()
{
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    //Color attachment
    glGenTextures(1, &m_FramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_framebufferSize.x, m_framebufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferTexture, 0);
    //Depth and stencil information
    glGenRenderbuffers(1, &m_Rendererbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_Rendererbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_framebufferSize.x, m_framebufferSize.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rendererbuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::destroy() {
    glDeleteTextures(1,&m_FramebufferTexture);
    glDeleteRenderbuffers(1,&m_Rendererbuffer);
    glDeleteFramebuffers(1,&m_Framebuffer);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}




