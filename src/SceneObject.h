#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <bitset>
class ResourceManager;
class Camera;

struct ParticleEmitter {
	glm::vec3 pos, speed;
	glm::vec4 color;
	float size, angle, weight;
	float life;
};

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
		m = glm::translate(m,glm::vec3(0.0,0.5f,0.0f));
		return m;
	}
};


class BoundingBox {
public:
	BoundingBox() {};
	void setAABB(const Mesh& mesh);
	const AABB& getAABB() const;
	bool RayIntersectAABB(const Camera& cam,const glm::mat4& model);
};