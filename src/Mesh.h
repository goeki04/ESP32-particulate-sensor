#pragma once
#include "Shader.h"

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

struct Vertex {
public:
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	Vertex() : pos(0.0f), normal(0.0f), uv(0.0f) {};
	void setPosition(const float x,const float y,const float z) {
		pos.x = x; pos.y = y; pos.z = z;
	}
	void setNormals(const float x, const float y, const float z) {
		normal.x = x; normal.y = y; normal.z = z;
	}
	void setUV(const float u,const float v) {
		uv.x = u; uv.y = v;
	}
};
class Mesh {
public:
	unsigned int m_Vao = 0;
	std::vector<Vertex> m_VertexBuffer;
	std::vector<unsigned int> m_IndexBuffer;
	Transform m_ModelMatrix;
	Mesh(std::vector<Vertex>&& vertexPositions, std::vector<unsigned int>&& vertexIndices)
		: m_VertexBuffer(std::move(vertexPositions)),
		m_IndexBuffer(std::move(vertexIndices)) {
	};
	Mesh() {
	}

	~Mesh() {
		glDeleteVertexArrays(1, &m_Vao);
		glDeleteBuffers(1, &m_Vbo);
	}

	void createMesh();
	void drawMesh();
	void setTextureID(GLuint id);
	GLuint getTextureID();
private:
	GLuint m_TextureID = 0;
	unsigned int m_Vbo = 0, m_Ebo = 0;
};

class BoundingBox {
private:
	float xMin; float xMax;
	float yMin; float yMax;
	float zMin; float zMax;
public:
	BoundingBox(Mesh& mesh) {
		recalculateBoundingBox(mesh);
	}
	BoundingBox() : xMin(0.0f), xMax(0.0f), yMin(0.0f), yMax(0.0f), zMin(0.0f), zMax(0.0f) {
		std::printf("Bounding box has no mesh data.");
	}
	void recalculateBoundingBox(Mesh& mesh);
	glm::vec3 getBoxCenter();
};

