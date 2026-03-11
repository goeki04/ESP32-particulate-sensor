#pragma once
#include "subsystem.h"
#include "registry.h"
#include "components.hpp"
#include "camera.h"
#include "GL/glew.h"
namespace Andromeda {
	enum class MaterialShaderType : int;
	namespace Window { class WindowManager; }
	class ResourceManager;
	class Camera;
	class Mesh;
	enum class MsaaSamples {
		x2 = 2, x4 = 4, x8 = 8
	};

	class Renderer : public ISubsystem {
	public:
		void start() override;
		void update() override;
		void createMesh(Mesh& mesh);
		void drawMesh(ResourceManager* rm, const ECS::Component::Mesh& mesh, const ECS::Component::Transform& transform);
		void drawMesh(ResourceManager* rm, const ECS::Component::Mesh& mesh, const ECS::Component::Transform& transform, MaterialShaderType type);
		void destroyFramebuffers();
		void destroy() override;
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
		void createSceneFbo();
		void createMSAAFbo();
		void createSelectionFBO();
		void createPostprocessFBO();
		static constexpr const char* glsl_version = "#version 460";
	private:
		void handleResize();
		glm::ivec2 m_FramebufferSize = glm::ivec2(0, 0);
		glm::vec2 m_TexelSize = glm::vec2(0.0, 0.0);

		Andromeda::Camera m_Cam;

		float m_ResizeTimer = 0.0f;
		bool m_ResizePending = false;
		glm::ivec2 m_TargetSize;
		GLuint m_Vao = 0;

		const unsigned int m_MSAAsamples = (int)MsaaSamples::x4;
		void createFramebuffers();
		ResourceManager* m_ResourceManager = nullptr;
		ECS::ComponentRegistry* m_Registry = nullptr;
#ifdef DEBUG_RENDERING_OPENGL
		bool m_DebugMode = false;
#endif
	};
}