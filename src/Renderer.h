#pragma once
#include "ISubsystem.h"
#include "GuiManager.h"
namespace Window { class WindowManager; }
class ResourceManager;
class Camera;
enum class MsaaSamples {
	x2 = 2,x4 = 4,x8 = 8
};

class Renderer : public ISubsystem{
public:
	void start() override;
	void update() override;
	void geometryPass();
	void guiPass(Camera& cam);
	void imGuiPass();
	void selectionPass();
	void postprocessingPass();
	void scenePassBegin();
	void proceduralPass();
	void pickingPass(const Camera& cam);
	void scenePassEndResolve();
	void windowClearPass();
	void destroy() override;
	static constexpr const char* glsl_version = "#version 460";
private:
	glm::ivec2 m_framebufferSize = glm::ivec2(0,0);
	GLuint m_Framebuffer = 0, m_MsaaFramebuffer = 0;
	GLuint m_FramebufferTexture = 0, m_MsaaFramebufferTexture = 0;
	GLuint m_SelectionFramebuffer, m_SelectionTexture;
	unsigned int m_Rendererbuffer = 0;
	GLuint m_Vao;
	const unsigned int m_MSAAsamples = (int)MsaaSamples::x4;
	void createFramebuffer();
	ResourceManager* m_ResourceManager = nullptr;
	GuiManager m_GuiManager;
#ifdef DEBUG_RENDERING_OPENGL
	bool m_DebugMode = false;
#endif
};