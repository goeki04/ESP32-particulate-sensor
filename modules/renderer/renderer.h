#pragma once
#include "a_graphics_base.hpp"
#include "a_ISubsystem.hpp"
#include "a_components.hpp"
#include "framebuffer.hpp"
#include "a_primitives.hpp"
namespace Andromeda {
	namespace amath {
		struct CameraData;
	}
	class CubemapData;
	class SceneManager;
	enum class MaterialShaderType : int;
	namespace Window { class WindowManager; }
	class ResourceManager;
	class Mesh;
	enum class MsaaSamples {
		x2 = 2, x4 = 4, x8 = 8
	};

	/// <summary>
	/// TODO: abstract OpenGL specific code from the renderer into the rhi. Also if theres enough time, implement
	/// a Vulkan renderer. MacOS should also be supported via MoltenVK
	/// </summary>
	enum class AndromedaGXAPI {
		OpenGL,
		Directx12,
		Vulkan,
		Metal
	};

	class Renderer : public ISubsystem {
	public:
		framebufferManager fboManager;
		ivec2 m_FramebufferSize = glm::ivec2(0, 0);
		static constexpr const char* glsl_version = "#version 460";

		static constexpr std::string_view GetStaticName() { return "Renderer"; }
		const char* getSubsystemName() const override {
			return GetStaticName().data();
		}
		void start() override;
		void update() override;
		void drawMesh(const ECS::Component::MeshRenderer& mesh, const ECS::Component::Transform& transform) const;
		void drawMesh(const ECS::Component::MeshRenderer& mesh, const ECS::Component::Transform& transform, MaterialShaderType type) const;
		void destroy() override;
		void setActiveCamera(amath::CameraData* camData);
		void geometryPass() const;
		u32 getFinalSceneViewportTexture() const;
		void onViewportResize(ivec2 newSize);
		void processResizeTimer();
		void selectionPass(ECS::Entity selectedEntity) const;
		void postprocessingPass() const;

		void scenePassBegin() const;

		void proceduralPass() const;
		void scenePassEndResolve() const;

		static void windowClearPass();

		//--- Resource Management ---
		void createCubemapTexture(CubemapData& data);
	private:
		AndromedaGXAPI m_GXAPI = AndromedaGXAPI::OpenGL;
		ECS::Entity m_SelectedForHighlighting = ECS::INVALID_ENTITY_ID;
		ResourceManager* m_ResourceManager = nullptr;
		SceneManager* m_SceneManager = nullptr;
		MsaaSamples m_MSAAsamples = MsaaSamples::x4;
		vec2 m_TexelSize = vec2(0.0, 0.0);
		u32 m_Vao = 0;
		amath::CameraData* m_Cam = nullptr;
		bool m_WireframeActive = false;
		float m_ResizeTimer = 0.0f;
		bool m_ResizePending = false;
		ivec2 m_TargetSize = ivec2(0.0f);
	};
}