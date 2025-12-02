#pragma once
#include "ISubsystem.h"
#include "Shader.h"
#include "Mesh.h"
class ResourceManager : public ISubsystem{
	void start() override;
	void loadImages();
	void loadAssimpScene(const char* path);
private:
	//std::vector<Shader> m_Shaders;
	std::vector<Mesh> m_Meshes;
	void setupShaders();
	void setupMeshes();
};