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

void Renderer::update()
{
    m_GuiManager.update();
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
