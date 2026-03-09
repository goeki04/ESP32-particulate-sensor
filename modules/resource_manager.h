#pragma once
#include "subsystem.h"
#include "shader.h"
#include "mesh.h"
#include "camera.h"
#include <unordered_map>
#include "registry.h"
#include "components.h"
#include "a_texture.hpp"
namespace Andromeda {
	using Entity = uint32_t;
	enum class deviceType {
		DEFAULT = -1,
		SENSOR,
		CONTROLLER,
		CABLE,
		BREADBOARD,
	};

	struct Device {
		uint32_t id;
		std::string name;
		Mesh mesh;
		deviceType type;
	};

	class ResourceManager : public Andromeda::ISubsystem {
	public:
		SDL_GLContext m_GlContext = NULL;
		std::vector<std::unique_ptr<MaterialShader>> m_MaterialShaders;
		std::vector<std::unique_ptr<ProceduralShader>> m_ProceduralShaders;
		std::vector<std::unique_ptr<PostProcessShader>> m_PostProcessShaders;
		std::unordered_map<deviceType, GLtexture> m_DeviceIcons;
		Andromeda::Camera m_Cam;

		static constexpr std::array<std::pair<std::string_view, deviceType>, 4> m_DirectoryNames{ {
			{"dsensor",     deviceType::SENSOR},
			{"dcontroller", deviceType::CONTROLLER},
			{"dcable",      deviceType::CABLE},
			{"dbreadboard", deviceType::BREADBOARD},
		} };

		std::vector<std::string> getAllFilesInDirectoryRecursive(const std::string& directory, std::span<const std::string> filter);

		void updateEvent(SDL_Event* event) override;

		void start() override;

		void update() override;

		void loadModels();

		void loadIcons();

		deviceType findDeviceIcon(std::string iconName);

		std::string getFileName(const std::string& path) const;

		static std::vector<std::string> getAllFilesInDirectory(const std::string& directory);

		static std::vector<std::string> getAllFilesInDirectory(const std::string& directory, std::span<std::string> filter);

		void addEntity(unsigned int meshID, const std::string& name, Andromeda::ECS::Component::Transform transform);
		void deleteEntity(Entity id);
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
		Andromeda::ECS::ComponentRegistry* m_Registry;
		std::unordered_map<uint32_t, Device> m_DeviceRecords;
		std::unordered_map<std::string, uint32_t> m_MeshIDbyName;
		unsigned int m_NextMeshID = 0;

		template<typename T> requires std::derived_from<T, MaterialShader>
		void addMaterialShader(const char* vertexShader, const char* fragmentShader) {
			m_MaterialShaders.emplace_back(std::make_unique<T>(m_Cam, vertexShader, fragmentShader));
			m_MaterialShaders.back()->compileShader();
		}

		template<typename T> requires std::derived_from<T, ProceduralShader>
		void addProceduralShader(const char* vertexShader, const char* fragmentShader) {
			m_ProceduralShaders.emplace_back(std::make_unique<T>(m_Cam, vertexShader, fragmentShader));
			m_ProceduralShaders.back()->compileShader();
		}

		template<typename T> requires std::derived_from<T, PostProcessShader>
		void addPostProcessShader(const char* vertexShader, const char* fragmentShader) {
			m_PostProcessShaders.emplace_back(std::make_unique<T>(m_Cam, vertexShader, fragmentShader));
			m_PostProcessShaders.back()->compileShader();
		}

		void setupMeshes();
		void processNode(const uint32_t meshId, const aiScene* scene, aiNode* node);
		deviceType findDeviceTypeByPath(const std::string& path);
		void loadScene(const std::string& path);
	};
}