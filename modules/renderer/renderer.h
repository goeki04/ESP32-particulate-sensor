#pragma once
#include "a_graphics_base.hpp"
#include "a_ISubsystem.hpp"
#include "components.hpp"
#include "framebuffer.hpp"
#include "a_primitives.hpp"
namespace Andromeda {
	namespace amath {
		class CameraData;
	}
	class SceneManager;
	enum class MaterialShaderType : int;
	namespace Window { class WindowManager; }
	class ResourceManager;
	class Mesh;
	enum class MsaaSamples {
		x2 = 2, x4 = 4, x8 = 8
	};

	class Renderer : public ISubsystem {
	public:
		ivec2 m_FramebufferSize = glm::ivec2(0, 0);
		static constexpr const char* glsl_version = "#version 460";

		static constexpr std::string_view GetStaticName() { return "Renderer"; }
		const char* getSubsystemName() const override {
			return GetStaticName().data();
		}
		void start() override;
		void update() override;
		void drawMesh(ResourceManager* rm, const ECS::Component::Mesh& mesh, const ECS::Component::Transform& transform);
		void drawMesh(ResourceManager* rm, const ECS::Component::Mesh& mesh, const ECS::Component::Transform& transform, MaterialShaderType type);
		void destroy() override;
		void SetActiveCamera(amath::CameraData* camData);
		void geometryPass();
		void guiPass(amath::CameraData* cam);
		void imGuiPass();
		u32 getFinalSceneViewportTexture() const;
		void onViewportResize(ivec2 newSize);
		void processResizeTimer();
		void selectionPass();
		void postprocessingPass();
		void scenePassBegin();
		void proceduralPass();
		void pickingPass(const amath::CameraData* cam);
		void scenePassEndResolve();
		void windowClearPass();
	private:
		ResourceManager* m_ResourceManager = nullptr;
		SceneManager* m_SceneManager = nullptr;
		framebufferManager fboManager;
		MsaaSamples m_MSAAsamples = MsaaSamples::x4;
		vec2 m_TexelSize = vec2(0.0, 0.0);
		u32 m_Vao = 0;
		amath::CameraData* m_Cam = nullptr;
		float m_ResizeTimer = 0.0f;
		bool m_ResizePending = false;
		ivec2 m_TargetSize = ivec2(0.0f);
#ifdef DEBUG_RENDERING_OPENGL
		bool m_DebugMode = false;
#endif
	};
}