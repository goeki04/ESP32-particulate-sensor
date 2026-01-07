#pragma once
#include "ISubsystem.h"
#include "Shader.h"
#include "Mesh.h"
#include "camera.h"
#include "SceneObject.h"
#include <unordered_map>
class ResourceManager : public ISubsystem{
public:
	void updateEvent(SDL_Event* event) override;
	void start() override;
	void update() override;
	SDL_GLContext m_GlContext = NULL;
	std::vector<std::unique_ptr<Shader>> m_Shaders;
	std::vector<SceneObject> m_SceneObjects;
	std::unordered_map<std::string, Mesh> m_Meshes;
	Camera m_Cam;
private:
	template<typename T> requires std::derived_from<T,Shader>
	void addShader(const char* vertexShader,const char* fragmentShader) {
		m_Shaders.emplace_back(std::make_unique<T>(m_Cam,vertexShader,fragmentShader));
		m_Shaders.back()->compileShader();
	}
	void setupMeshes();
	void processNode(const std::string& path,const aiScene* scene, aiNode* node, SceneObject* sceneObject);
	void loadScene(const std::string& path);
};
