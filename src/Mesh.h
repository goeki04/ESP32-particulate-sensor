#pragma once
#include "Shader.h"

struct Vertex {
public:
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 color; //this is not a vertex color, its the diffuse color
	Vertex() : pos(0.0f), normal(0.0f), uv(0.0f), color(1.0f) {};
	void setPosition(const float x,const float y,const float z) {
		pos.x = x; pos.y = y; pos.z = z;
	}
	void setNormals(const float x, const float y, const float z) {
		normal.x = x; normal.y = y; normal.z = z;
	}
	void setUV(const float u,const float v) {
		uv.x = u; uv.y = v;
	}
	glm::vec3 getPosition() const{
		return pos;
	}
};
class Mesh {
public:
	unsigned int m_Vao = 0;
	std::vector<Vertex> m_VertexBuffer;
	std::vector<unsigned int> m_IndexBuffer;
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
struct AABB {
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 center;
};
class BoundingBox {
private:
	float xMin; float xMax;
	float yMin; float yMax;
	float zMin; float zMax;
public:
	BoundingBox(Mesh& mesh) {
		getAABB(mesh);
	}
	BoundingBox() : xMin(0.0f), xMax(0.0f), yMin(0.0f), yMax(0.0f), zMin(0.0f), zMax(0.0f) {
		std::printf("Bounding box has no mesh data.");
	}
	AABB getAABB(const Mesh& mesh);
};

