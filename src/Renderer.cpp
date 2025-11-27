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
    ImGui_ImplSDL3_InitForOpenGL(m_WindowManager->m_Window, glContext);
    ImGui_ImplOpenGL3_Init(Renderer::glsl_version);
}
/// <summary>
/// Call this function before ImGui::Render()
/// </summary>
void Renderer::drawOpenGLViewport()
{
    ImVec4 viewportPosScale = m_GuiManager.getViewportPosScale();
    ImGui::SetNextWindowPos(ImVec2(viewportPosScale.x,viewportPosScale.y));
    ImGui::SetNextWindowSize(ImVec2(viewportPosScale.z, viewportPosScale.w));

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //Color attachment
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportPosScale.z, viewportPosScale.w, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texture,0);

    //Depth and stencil information
    unsigned int renderBuffer;
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,viewportPosScale.z,viewportPosScale.w);
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,renderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete";
    }
#ifdef DEBUG_RENDERING_OPENGL
    else if (m_DebugMode == false){
        std::cout << "Framebuffer is complete" << std::endl;
        m_DebugMode = true;
    }
#endif
    glBindFramebuffer(GL_FRAMEBUFFER,0);



    ImGui::Begin("Viewport");
    ImGui::End();
}

void Renderer::update()
{
    m_GuiManager.update();
    drawOpenGLViewport();
    ImGui::Render();
    glViewport(0, 0, m_WindowManager->m_WindowWidth, m_WindowManager->m_WindowHeight);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_WindowManager->m_Window);
}

void Renderer::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
