#pragma once
#include "ISubsystem.h"
#include "GuiManager.h"
class WindowManager;
class ResourceManager;
class Renderer : public ISubsystem{
public:
	void start() override;
	void update() override;
	void destroy() override;
	static constexpr const char* glsl_version = "#version 460";
private:
	glm::ivec2 m_framebufferSize = glm::ivec2(0,0);
	unsigned int m_Framebuffer = 0;
	unsigned int m_FramebufferTexture = 0;
	unsigned int m_Rendererbuffer = 0;

	void createFramebuffer();
	ResourceManager* m_ResourceManager = nullptr;
	WindowManager* m_WindowManager = nullptr;
	GuiManager m_GuiManager;
#ifdef DEBUG_RENDERING_OPENGL
	bool m_DebugMode = false;
#endif
};