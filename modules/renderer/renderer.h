#pragma once
#include "a_graphics_base.hpp"
#include "a_ISubsystem.hpp"
#include "a_components.hpp"
#include "OpenGL/a_opengl_framebuffer.hpp"
#include "a_primitives.hpp"
#include "OpenGL/a_opengl_handles.hpp"
#include "a_IGraphicsContext.hpp"
#include "OpenGL/a_OpenGLContext.hpp"
#include "a_shader_cpu.hpp"
#include "OpenGL/a_opengl_constant_buffer.hpp"
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
		ivec2 m_FramebufferSize = glm::ivec2(0, 0);
		static constexpr const char* glsl_version = "#version 460";

		static constexpr std::string_view GetStaticName() { return "Renderer"; }
		const char* getSubsystemName() const override {
			return GetStaticName().data();
		}
		void start() override;
		void update() override;
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

		void windowClearPass();
		void createCubemapTexture(CubemapData& data);
	private:
		AndromedaGXAPI m_GXAPI = AndromedaGXAPI::OpenGL;
		ECS::Entity m_SelectedForHighlighting = ECS::INVALID_ENTITY_ID;
		ResourceManager* m_ResourceManager = nullptr;
		SceneManager* m_SceneManager = nullptr;
		vec2 m_TexelSize = vec2(0.0, 0.0);
		u32 m_CubeVao = 0;
		u32 m_EnvironmentCubemapID = 0;
		amath::CameraData* m_Cam = nullptr;
		bool m_WireframeActive = false;
		MeshGPUHandle cubemapgpuHandle;
		float m_ResizeTimer = 0.0f;
		bool m_ResizePending = false;
		ivec2 m_TargetSize = ivec2(0.0f);
		std::unique_ptr<OpenGLContext> m_GLContext = nullptr;
		IGraphicsContext* m_RenderContext = nullptr;


		std::shared_ptr<IFramebuffer> m_MsaaBuffer;
		std::shared_ptr<IFramebuffer> m_SceneBuffer;
		std::shared_ptr<IFramebuffer> m_SelectionBuffer;
		std::shared_ptr<IFramebuffer> m_PostprocessBuffer;
		std::shared_ptr<IFramebuffer> m_BakingBuffer;

		GLConstantBuffer m_CameraUBO;
		GLConstantBuffer m_ObjectUBO;
		GLConstantBuffer m_ColorUBO;
		GLConstantBuffer m_GridUBO;
		GLConstantBuffer m_GridParamsUBO;
		GLConstantBuffer m_OutlineUBO;

		void initRenderer();
		void registerEvents();
		void createFramebuffers();
		void createMaterials();
	};
}