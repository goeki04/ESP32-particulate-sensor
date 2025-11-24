#pragma once
#include "ISubsystem.h"
#include "Renderer.h"
class WindowManager : public ISubsystem{
public:
	void start() override;
	void update() override;
	void destroy() override;
	SDL_Renderer* getRenderer();
	SDL_Window* getWindow();
private:
	SDL_Window* m_Window = NULL;
	Renderer m_Renderer;
};