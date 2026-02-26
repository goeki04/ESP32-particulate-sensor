#pragma once
#include "ISubsystem.h"
#include "GuiRenderer.h"
namespace Window {
	inline SDL_Window* g_Window = nullptr;
	inline int g_WindowWidth = 0, g_WindowHeight = 0;
	class WindowManager : public ISubsystem {
	public:
		void start() override;
		void destroy() override;
		void updateEvent(SDL_Event* event) override;
	};
}