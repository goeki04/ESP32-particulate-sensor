#pragma once
#include "ISubsystem.h"
#include "Registry.h"
#include "GuiRenderer.h"
#include "components.h"
namespace Andromeda {
	enum class MaterialShaderType : int;
	namespace Window { class WindowManager; }
	class ResourceManager;
	class Camera;
	enum class MsaaSamples {
		x2 = 2, x4 = 4, x8 = 8
	};

	class Renderer : public ISubsystem {
	public:
		void start() override;
		void update() override;
		void drawMesh(Andromeda::ResourceManager* rm, const Andromeda::ECS::Component::Mesh& mesh, const Andromeda::ECS::Component::Transform& transform);
		void drawMesh(Andromeda::ResourceManager* rm, const Andromeda::ECS::Component::Mesh& mesh, const Andromeda::ECS::Component::Transform& transform, MaterialShaderType type);
		void destroyFramebuffers();
		void destroy() override;
		void geometryPass();
		void guiPass(Andromeda::Camera& cam);
		void imGuiPass();
		void selectionPass();
		void postprocessingPass();
		void scenePassBegin();
		void proceduralPass();
		void pickingPass(const Andromeda::Camera& cam);
		void scenePassEndResolve();
		void windowClearPass();
		void createSceneFbo();
		void createMSAAFbo();
		void createSelectionFBO();
		void createPostprocessFBO();
		static constexpr const char* glsl_version = "#version 460";
	private:
		void handleResize();
		glm::ivec2 m_FramebufferSize = glm::ivec2(0, 0);
		glm::vec2 m_TexelSize = glm::vec2(0.0, 0.0);
		GLuint m_Framebuffer = 0, m_MsaaFramebuffer = 0;
		GLuint m_FramebufferTexture = 0, m_MsaaFramebufferTexture = 0;
		GLuint m_SelectionFramebuffer = 0, m_SelectionTexture = 0;
		GLuint m_PostprocessFramebuffer = 0, m_PostprocessTexture = 0;
		unsigned int m_Rendererbuffer = 0;
		float m_ResizeTimer = 0.0f;
		bool m_ResizePending = false;
		glm::ivec2 m_TargetSize;
		GLuint m_Vao = 0;
		GLuint m_SelectionDepth = 0;
		const unsigned int m_MSAAsamples = (int)MsaaSamples::x4;
		void createFramebuffers();
		Andromeda::ResourceManager* m_ResourceManager = nullptr;
		Andromeda::ECS::ComponentRegistry* m_Registry = nullptr;
		Andromeda::Gui::GuiRenderer m_GuiManager;
#ifdef DEBUG_RENDERING_OPENGL
		bool m_DebugMode = false;
#endif
	};
}