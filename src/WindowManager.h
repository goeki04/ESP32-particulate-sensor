#pragma once
#include "ISubsystem.h"
#include "GuiManager.h"
class WindowManager : public ISubsystem{
public:
	SDL_Window* m_Window = NULL;
	int m_WindowWidth = 0, m_WindowHeight = 0;
	void start() override;
	void destroy() override;
	SDL_Surface* CreateSDLSurface(const char* path);
	void updateEvent(SDL_Event* event) override;
};