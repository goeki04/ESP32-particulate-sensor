#pragma once
#include "a_graphics_base.hpp"
#include "a_ISubsystem.hpp"
#include "a_components.hpp"
#include "OpenGL/a_opengl_framebuffer.hpp"
#include "a_primitives.hpp"
#include "OpenGL/a_opengl_handles.hpp"
#include "a_IGraphicsContext.hpp"
#include "OpenGL/a_OpenGLContext.hpp"
#include <a_CubemapData.hpp>
#include "OpenGL/a_opengl_constant_buffer.hpp"
#include "a_texture.hpp"
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
		void irradianceCubemapBaking();
		void brdfLUTBaking();
		std::shared_ptr<IFramebuffer> helperCreateFBO(ivec2 size, std::vector<FramebufferTextureFormat> formats, u32 samples);
		void destroy() override;
		void setActiveCamera(amath::CameraData* camData);
		void geometryPass() const;
		Texture getFinalSceneViewportTexture() const;
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
		ResourceManager* m_ResourceManager = nullptr;
		SceneManager* m_SceneManager = nullptr;
		std::unique_ptr<OpenGLContext> m_GLContext = nullptr;
		IGraphicsContext* m_RenderContext = nullptr;
		amath::CameraData* m_Cam = nullptr;

		ECS::Entity m_SelectedForHighlighting = ECS::INVALID_ENTITY_ID;
		vec2 m_TexelSize = vec2(0.0, 0.0);
		u32 m_CubeVao = 0;

		bool m_WireframeActive = false;
		MeshGPUHandle cubemapgpuHandle;
		float m_ResizeTimer = 0.0f;
		bool m_ResizePending = false;
		ivec2 m_TargetSize = ivec2(0.0f);

		CubemapData m_EnvironmentCubemap;
		CubemapData m_IrradianceCubemap;
		CubemapData m_PrefilterMap;
		Texture m_BrdfLUTTexture;
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
		GLConstantBuffer m_LightUBO;
		GLConstantBuffer m_pbrMaterialUBO;
		void initRenderer();
        void prefilterCubemapBaking();
        void registerEvents();
        void createFramebuffers();
		void createMaterials();
	};
}