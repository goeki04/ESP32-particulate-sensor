#pragma once
#include "Mesh.h"
class SceneObject {
public:
	std::string name = "unnamed";
	std::vector<Mesh> m_Submeshes;
	Shader* m_Shader = nullptr;
	void setShader(Shader* shader) {
		m_Shader = shader;
	}
	void drawMesh();
	SceneObject() { }
};