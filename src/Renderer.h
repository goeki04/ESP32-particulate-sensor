#pragma once
#include "ISubsystem.h"
#include "GuiManager.h"
namespace Window { class WindowManager; }
class ResourceManager;

enum class MsaaSamples {
	x2 = 2,x4 = 4,x8 = 8
};

class Renderer : public ISubsystem{
public:
	void start() override;
	void update() override;
	void destroy() override;
	static constexpr const char* glsl_version = "#version 460";
private:
	glm::ivec2 m_framebufferSize = glm::ivec2(0,0);
	unsigned int m_Framebuffer, m_MsaaFramebuffer;
	unsigned int m_FramebufferTexture,m_MsaaFramebufferTexture;
	unsigned int m_Rendererbuffer = 0;
	const unsigned int m_MSAAsamples = (int)MsaaSamples::x4;
	void createFramebuffer();
	ResourceManager* m_ResourceManager = nullptr;
	GuiManager m_GuiManager;
#ifdef DEBUG_RENDERING_OPENGL
	bool m_DebugMode = false;
#endif
};