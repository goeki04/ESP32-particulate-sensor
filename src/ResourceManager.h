#pragma once
#include "ISubsystem.h"
#include "Shader.h"
#include "Mesh.h"
#include "camera.h"
#include <unordered_map>
#include "SceneObject.h"
struct MeshRecord {
	uint32_t id;
	std::string name;
	Mesh mesh;
};

class ResourceManager : public ISubsystem{
public:
	void updateEvent(SDL_Event* event) override;
	static SDL_Surface* CreateSDLSurface(const char* path);
	void start() override;
	void update() override;
	static std::vector<std::string> getAllFilesInDirectory(const std::string& directory);
	std::vector<std::string> getAllFilesInDirectory(const std::string& directory, std::span<const std::string> filter);
	void addSceneObject(const std::string& name, unsigned int meshID);
	void deleteSceneObject(SceneObject& sceneObject);
	GLsizei getMeshVaoByID(uint32_t meshID);
	GLsizei getMeshIndexSizeByID(uint32_t meshID);
	Shader* getShaderByID(shaderType type);
	Mesh& getMeshByID(uint32_t meshID);
	SDL_GLContext m_GlContext = NULL;
	std::vector<std::unique_ptr<Shader>> m_Shaders;
	std::vector<SceneObject> m_SceneObjects;
	std::vector<SDL_Surface> m_DeviceIcons;
	Camera m_Cam;
private:
	std::unordered_map<uint32_t, MeshRecord> m_MeshRecords;
	std::unordered_map<std::string, uint32_t> m_MeshIDbyName;
	unsigned int m_NextMeshID = 0;
	unsigned int m_NextSceneObjectID = 0;
	template<typename T> requires std::derived_from<T,Shader>
	void addShader(const char* vertexShader,const char* fragmentShader) {
		m_Shaders.emplace_back(std::make_unique<T>(m_Cam,vertexShader,fragmentShader));
		m_Shaders.back()->compileShader();
	}
	void setupMeshes();
	void processNode(const uint32_t meshId,const aiScene* scene, aiNode* node);
	void loadScene(const std::string& path);
};
