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
	glm::ivec2 m_framebufferSize;
	unsigned int m_Framebuffer;
	unsigned int m_FramebufferTexture;
	unsigned int m_Rendererbuffer;
	void createFramebuffer();
	void drawOpenGLViewport();
	WindowManager* m_WindowManager;
	GuiManager m_GuiManager;
#ifdef DEBUG_RENDERING_OPENGL
	bool m_DebugMode = false;
#endif
};