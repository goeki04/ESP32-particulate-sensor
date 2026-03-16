#pragma once
#include "a_ISubsystem.hpp"
#include <SDL3/SDL.h>
namespace Andromeda::Window {
	inline SDL_Window* g_Window = nullptr;
	inline int g_WindowWidth = 0, g_WindowHeight = 0;
	inline SDL_GLContext m_GlContext = nullptr;
	class WindowManager : public ISubsystem {
	public:
		void start() override;
		void destroy() override;
		void updateEvent(SDL_Event* event) override;
	private:
		SDL_Surface* CreateSDLSurface(const char* path);
	};
}