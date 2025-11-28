#include "Renderer.h"
#include "pch.h"
#include "SubsystemManager.h"
#include "WindowManager.h"

void Renderer::start()
{
	m_WindowManager = SystemManager::getInstance().getSubsystem<WindowManager>();
    SDL_GLContext glContext = SDL_GL_CreateContext(m_WindowManager->m_Window);
    if (!glContext) {
        throw std::exception("Failed to create SDL_GL context!");
    }
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw std::exception("Failed to call glewInit!");
    }
    m_GuiManager.init(m_WindowManager->m_Window);
    ImVec2 viewportWindowSize = m_GuiManager.getViewportWindowSize();
    m_framebufferSize = glm::ivec2(viewportWindowSize.x, viewportWindowSize.y);

    ImGui_ImplSDL3_InitForOpenGL(m_WindowManager->m_Window, glContext);
    ImGui_ImplOpenGL3_Init(Renderer::glsl_version);
    createFramebuffer();

}

/// <summary>
/// Call this function before ImGui::Render()
/// </summary>
void Renderer::drawOpenGLViewport()
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::SetNextWindowPos(m_GuiManager.getViewportWindowPos());
    ImGui::SetNextWindowSize(m_GuiManager.getViewportWindowSize());
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0.0f,0.0f));
    ImGui::Begin("Viewport",0,windowFlags);
    ImGui::Image((void*)(intptr_t)m_FramebufferTexture,ImVec2(m_framebufferSize.x,m_framebufferSize.y), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::End();
    ImGui::PopStyleVar();
}

void Renderer::update()
{
    m_GuiManager.update();
    drawOpenGLViewport();
    ImGui::Render();
    glViewport(0, 0, m_WindowManager->m_WindowWidth, m_WindowManager->m_WindowHeight);
    glClearColor(0.172f, 0.172f, 0.329f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_WindowManager->m_Window);
}
/// <summary>
/// Call this function after initializing guimanager to create an offscreen framebuffer.
/// </summary>
void Renderer::createFramebuffer()
{
    size_t numPixels = static_cast<size_t>(m_framebufferSize.x) * m_framebufferSize.y;
    std::vector<unsigned char> pixels(numPixels * 4, 0); // alles auf 0 setzen
    for (size_t i = 0; i < numPixels; ++i) {
        pixels[i * 4 + 2] = 255; // Blau setzen
        pixels[i * 4 + 3] = 255;
    }
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    //Color attachment
    glGenTextures(1, &m_FramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_framebufferSize.x, m_framebufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
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
