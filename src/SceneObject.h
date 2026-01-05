#pragma once
#include "Mesh.h"

struct Transform {
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	//Rotation in radians
	glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

	const glm::mat4 localMatrix()
	{
		glm::mat4 m(1.0f);
		m = glm::translate(m, position);
		m = glm::rotate(m, rotation.x, glm::vec3(1, 0, 0));
		m = glm::rotate(m, rotation.y, glm::vec3(0, 1, 0));
		m = glm::rotate(m, rotation.z, glm::vec3(0, 0, 1));
		m = glm::scale(m, scale);
		return m;
	}
};

class SceneObject {
public:
	std::string m_Name = "unnamed";
	std::vector<Mesh> m_Submeshes;
	Shader* m_Shader = nullptr;
	Transform m_Transform;
	BoundingBox m_BoundingBox;
	void setShader(Shader* shader) {
		m_Shader = shader;
	}
	void drawMesh();
	SceneObject(std::string& name) : m_Name(name) { }
	SceneObject(){}
};