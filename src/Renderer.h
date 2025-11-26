#pragma once
#include "ISubsystem.h"
#include "GuiManager.h"
class WindowManager;
class Renderer : public ISubsystem{
public:
	void start() override;
	void update() override;
	void destroy() override;
	static constexpr const char* glsl_version = "#version 150";
private:
	WindowManager* m_WindowManager;
	GuiManager m_GuiManager;
};