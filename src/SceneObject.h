#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <bitset>
class ResourceManager;
class Camera;


struct Transform {
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	//Rotation in radians
	glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

	const glm::mat4 modelMatrix() const
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

struct AABB {
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 center;
	AABB() : min(0.0f), max(0.0f), center(0.0f) {};
};
class BoundingBox {
private:
	AABB m_AABB;
public:
	BoundingBox() {};
	void setAABB(const Mesh& mesh);
	const AABB& getAABB() const;
	bool RayIntersectAABB(const Camera& cam,const glm::mat4& model);
};
class Entity {
public:
	unsigned int m_ID;
	std::string m_Name = "unnamed";
	unsigned int m_MeshID = 0;
	bool m_IsSelected = false;
	Transform m_Transform;
	BoundingBox m_BoundingBox;
	ResourceManager* m_ResourceManager = nullptr;
	MaterialShaderType m_Shadertype = MaterialShaderType::unlit;
	Entity(ResourceManager* rm, unsigned int meshID, unsigned int objectID, const std::string& name,Transform& transform);
	void drawMesh();

	void drawMeshOutline();

	bool operator==(const Entity& other) const{
		return other.m_ID == m_ID;
	}
};