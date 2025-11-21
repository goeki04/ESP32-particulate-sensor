#pragma once
#include "ISubsystem.h"
class WindowManager : public ISubsystem{
public:
	static SDL_Window* m_Window;
	static SDL_Renderer* m_Renderer;
	void start() override;
	void update() override;
	void destroy() override;
};