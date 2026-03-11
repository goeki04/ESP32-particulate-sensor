
#include "editor.hpp"
#include "renderer.h"
#include "window_manager.h"
#include "subsystem_manager.h"
namespace Andromeda {
	void Editor::syncLibraryWithRM() {

	}
	void Editor::start()
	{
		m_Renderer = SystemManager::getInstance().getSubsystem<Renderer>();
		if (m_Renderer == nullptr) {
			std::runtime_error("Renderer is nullptr");
			return;
		}
		ImGui_ImplSDL3_InitForOpenGL(Window::g_Window, m_Renderer->m_GlContext);
		ImGui_ImplOpenGL3_Init(Renderer::glsl_version);
	}
	void Editor::update()
	{
	}
}