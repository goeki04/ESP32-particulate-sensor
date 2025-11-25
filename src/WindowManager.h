#pragma once
#include "ISubsystem.h"
#include "GuiManager.h"
class WindowManager : public ISubsystem{
public:
	void start() override;
	void update() override;
	void destroy() override;
	SDL_Surface* LoadPNG(const char* path);
	void updateEvent(SDL_Event* event) override;
private:
	SDL_Window* m_Window = NULL;
	SDL_Renderer* m_SDLRenderer = NULL;
	GuiManager m_GuiManager;
};