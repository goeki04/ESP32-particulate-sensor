#pragma once
#include "ISubsystem.h"
#include "Shader.h"
#include "Mesh.h"
#include "camera.h"
#include <unordered_map>
#include "SceneObject.h"

enum class deviceType {
	DEFAULT = -1,
	SENSOR,
	CONTROLLER,
	CABLE,
	BREADBOARD,
};

struct GLtexture
{
	GLuint id = 0;
	int w = 0;
	int h = 0;

	GLtexture() = default;
	GLtexture(const GLtexture&) = delete;
	GLtexture& operator=(const GLtexture&) = delete;
	GLtexture(GLtexture&& other) noexcept
	{
		*this = std::move(other);
	}

	GLtexture& operator=(GLtexture&& other) noexcept
	{
		if (this != &other) {
			destroy();
			id = other.id;
			w = other.w;
			h = other.h;
			other.id = 0;
			other.w = other.h = 0;
		}
		return *this;
	}

	~GLtexture() { destroy(); }

	void destroy()
	{
		if (id != 0) {
			if (SDL_GL_GetCurrentContext()) {
				glDeleteTextures(1, &id);
			}
			id = 0;
		}
	}
};

struct Device {
	uint32_t id;
	std::string name;
	Mesh mesh;
	deviceType type;
};

class ResourceManager : public ISubsystem{
public:
	SDL_GLContext m_GlContext = NULL;
	std::vector<std::unique_ptr<MaterialShader>> m_MaterialShaders;
	std::vector<std::unique_ptr<ProceduralShader>> m_ProceduralShaders;
	std::unordered_map<deviceType, GLtexture> m_DeviceIcons;
	Camera m_Cam;
	static constexpr std::array<std::pair<std::string_view, deviceType>, 4> m_DirectoryNames{ {
		{"dsensor",     deviceType::SENSOR},
		{"dcontroller", deviceType::CONTROLLER},
		{"dcable",      deviceType::CABLE},
		{"dbreadboard", deviceType::BREADBOARD},
		 } };
	std::vector<std::string> getAllFilesInDirectoryRecursive(const std::string& directory, std::span<const std::string> filter);
	void updateEvent(SDL_Event* event) override;
	static SDL_Surface* CreateSDLSurface(const char* path);
	void start() override;
	void update() override;
	void loadModels();
	void loadIcons();
	deviceType findDeviceIcon(std::string iconName);
	std::string getFileName(const std::string& path) const;
	static std::vector<std::string> getAllFilesInDirectory(const std::string& directory);
	static std::vector<std::string> getAllFilesInDirectory(const std::string& directory, std::span<std::string> filter);
	void addEntity(unsigned int meshID, const std::string& name, Transform& transform);
	void deleteEntityObject(Entity& sceneObject);
	GLsizei getMeshVaoByID(uint32_t meshID) const;
	GLsizei getMeshIndexSizeByID(uint32_t meshID) const;
	MaterialShader* getMaterialShaderByID(MaterialShaderType t)   const;
	ProceduralShader* getProceduralShaderByID(ProceduralShaderType t) const;
	const Mesh& getMeshByID(uint32_t meshID) const;
    size_t getDeviceRecordsSize() const;
	const std::unordered_map<uint32_t, Device>& getDeviceRecords() const;
	GLtexture CreateOpenGLTexture(const char* path);
	std::vector<Entity>& getEntitys();
private:
	std::vector<Entity> m_Entitys;
	std::unordered_map<uint32_t, Device> m_DeviceRecords;
	std::unordered_map<std::string,uint32_t> m_MeshIDbyName;
	unsigned int m_NextMeshID = 0;
	unsigned int m_NextSceneObjectID = 0;
	template<typename T> requires std::derived_from<T,MaterialShader>
	void addMaterialShader(const char* vertexShader,const char* fragmentShader) {
		m_MaterialShaders.emplace_back(std::make_unique<T>(m_Cam,vertexShader,fragmentShader));
		m_MaterialShaders.back()->compileShader();
	}
	template<typename T> requires std::derived_from<T, ProceduralShader>
	void addProceduralShader(const char* vertexShader, const char* fragmentShader) {
		m_ProceduralShaders.emplace_back(std::make_unique<T>(m_Cam, vertexShader, fragmentShader));
		m_ProceduralShaders.back()->compileShader();
	}
	void setupMeshes();
	void processNode(const uint32_t meshId,const aiScene* scene, aiNode* node);
	deviceType findDeviceTypeByPath(const std::string& path);
	void loadScene(const std::string& path);
};
