#pragma once
#include "ISubsystem.h"
#include "Shader.h"
#include "Mesh.h"
#include "camera.h"
#include "SceneObject.h"
#include <unordered_map>
struct MeshRecord {
	uint32_t id;
	std::string name;
	Mesh mesh;
};


class ResourceManager : public ISubsystem{
public:
	void updateEvent(SDL_Event* event) override;
	void start() override;
	void update() override;
	unsigned int getMeshVaoByID(uint32_t meshID);
	unsigned int getMeshIndexSizeByID(uint32_t meshID);
	Mesh& getMeshByID(uint32_t meshID);
	SDL_GLContext m_GlContext = NULL;

	std::vector<std::unique_ptr<Shader>> m_Shaders;
	std::vector<SceneObject> m_SceneObjects;
	std::unordered_map<uint32_t, MeshRecord> m_MeshRecords;          // id -> record
	std::unordered_map<std::string, uint32_t> m_MeshIDbyName;     // name -> id
	Camera m_Cam;
private:
	unsigned int m_NextMeshID = 0;
	template<typename T> requires std::derived_from<T,Shader>
	void addShader(const char* vertexShader,const char* fragmentShader) {
		m_Shaders.emplace_back(std::make_unique<T>(m_Cam,vertexShader,fragmentShader));
		m_Shaders.back()->compileShader();
	}
	void setupMeshes();
	void processNode(const uint32_t meshId,const aiScene* scene, aiNode* node);
	void loadScene(const std::string& path);
};
