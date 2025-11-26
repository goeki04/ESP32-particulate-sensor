#pragma once
#include "ISubsystem.h"
#include "GuiManager.h"
class WindowManager : public ISubsystem{
public:
	SDL_Window* m_Window = NULL;
	int m_WindowWidth, m_WindowHeight;
	void start() override;
	void destroy() override;
	SDL_Surface* CreateSDLSurface(const char* path);
	void updateEvent(SDL_Event* event) override;
};