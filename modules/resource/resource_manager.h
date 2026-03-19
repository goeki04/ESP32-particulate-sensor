#pragma once
#include "shader.hpp"
#include <unordered_map>
#include "components.hpp"
#include "a_texture.hpp"
#include <memory>
#include "a_device.hpp"

#include <assimp/scene.h>
#include "a_opengl_handles.hpp"
#include "a_primitives.hpp"
#include "a_ISubsystem.hpp"
namespace Andromeda {

	class ResourceManager : public ISubsystem, public IDeviceProvider {
	public:

		std::vector<std::unique_ptr<MaterialShader>> m_MaterialShaders;
		std::vector<std::unique_ptr<ProceduralShader>> m_ProceduralShaders;
		std::vector<std::unique_ptr<PostProcessShader>> m_PostProcessShaders;
		std::unordered_map<deviceType, GLtexture> m_DeviceIcons;

		static constexpr std::string_view GetStaticName() { return "ResourceManager"; }
		const char* getSubsystemName() const override {
			return GetStaticName().data();
		}

		void updateEvent(SDL_Event* event) override;

		void start() override;

		void update() override;

		void loadModels();

		void loadIcons();

		deviceType findDeviceIcon(std::string iconName);

		u32 getMeshVaoByID(uint32_t meshID) const;

		u32 getMeshIndexSizeByID(uint32_t meshID) const;

		MaterialShader* getMaterialShaderByID(MaterialShaderType t)   const;

		ProceduralShader* getProceduralShaderByID(ProceduralShaderType t) const;

		PostProcessShader* getPostprocessShaderByID(PostProcessShaderType t) const;

		const Mesh& getMeshByID(uint32_t meshID) const;

		u32 getDeviceRecordsSize() const;

		const std::unordered_map<uint32_t, Device>& getDeviceRecords() const;

		GLtexture CreateOpenGLTexture(const char* path);
		
		/// <summary>
		/// Interface implementations of DeviceProvider which are being used from the guirenderer.
		/// </summary>
		/// <returns></returns>
		u32 getDeviceCount() const override;
		const Device& getDeviceData(u32 index) const override;
		virtual u32 getDeviceIconID(deviceType type) const;

	private:

		std::unordered_map<uint32_t, Device> m_DeviceRecords;
		std::unordered_map<i32,MeshGPUHandle> m_GPUMeshes;
		/// <summary>
		/// Stores the device index order. Used by the guimanager to draw the icons in the device browser
		/// </summary>
		std::vector<u32> m_DeviceIndexList;
		std::unordered_map<std::string, uint32_t> m_MeshIDbyName;
		unsigned int m_NextMeshID = 0;

		template<typename T> requires std::derived_from<T, MaterialShader>
		void addMaterialShader(const char* vertexShader, const char* fragmentShader) {
			m_MaterialShaders.emplace_back(std::make_unique<T>(vertexShader, fragmentShader));
			m_MaterialShaders.back()->compileShader();
		}

		template<typename T> requires std::derived_from<T, ProceduralShader>
		void addProceduralShader(const char* vertexShader, const char* fragmentShader) {
			m_ProceduralShaders.emplace_back(std::make_unique<T>(vertexShader, fragmentShader));
			m_ProceduralShaders.back()->compileShader();
		}

		template<typename T> requires std::derived_from<T, PostProcessShader>
		void addPostProcessShader(const char* vertexShader, const char* fragmentShader) {
			m_PostProcessShaders.emplace_back(std::make_unique<T>(vertexShader, fragmentShader));
			m_PostProcessShaders.back()->compileShader();
		}

		void setupMeshes();
		void processNode(const uint32_t meshId, const aiScene* scene, aiNode* node);

		void loadScene(const std::string& path);
	};
}