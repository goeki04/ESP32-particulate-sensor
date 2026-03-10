#pragma once
#include "subsystem.h"
#include "shader.h"
#include "mesh.h"
#include <unordered_map>
#include "components.hpp"
#include "a_texture.hpp"
#include "a_device.hpp"
namespace Andromeda {


	class ResourceManager : public Andromeda::ISubsystem {
	public:
		SDL_GLContext m_GlContext = NULL;
		std::vector<std::unique_ptr<MaterialShader>> m_MaterialShaders;
		std::vector<std::unique_ptr<ProceduralShader>> m_ProceduralShaders;
		std::vector<std::unique_ptr<PostProcessShader>> m_PostProcessShaders;
		std::unordered_map<deviceType, GLtexture> m_DeviceIcons;

		void updateEvent(SDL_Event* event) override;

		void start() override;

		void update() override;

		void loadModels();

		void loadIcons();

		deviceType findDeviceIcon(std::string iconName);
		void setAABB(const Mesh& mesh, Andromeda::ECS::Component::AABB& aabb);

		GLsizei getMeshVaoByID(uint32_t meshID) const;

		GLsizei getMeshIndexSizeByID(uint32_t meshID) const;

		MaterialShader* getMaterialShaderByID(MaterialShaderType t)   const;

		ProceduralShader* getProceduralShaderByID(ProceduralShaderType t) const;

		PostProcessShader* getPostprocessShaderByID(PostProcessShaderType t) const;

		const Mesh& getMeshByID(uint32_t meshID) const;

		size_t getDeviceRecordsSize() const;

		const std::unordered_map<uint32_t, Device>& getDeviceRecords() const;

		GLtexture CreateOpenGLTexture(const char* path);

	private:

		std::unordered_map<uint32_t, Device> m_DeviceRecords;
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