#pragma once
#include "Shader.h"
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
	glm::vec3 operator=(aiVector3D vec) {
		pos.x = vec.x;
		pos.y = vec.y;
		pos.z = vec.z;
	}
};
class Mesh {
public:
	Shader* m_Shader = nullptr;
	std::vector<Vertex> m_VertexBuffer;
	std::vector<unsigned int> m_IndexBuffer;
	GLuint textureID = 0;
	Mesh(std::vector<Vertex>&& vertexPositions, std::vector<unsigned int>&& vertexIndices)
		: m_VertexBuffer(std::move(vertexPositions)),
		m_IndexBuffer(std::move(vertexIndices)) {
	};
	Mesh() {
	}
	void setShader(Shader* shader) {
		m_Shader = shader;
	}
	~Mesh() {
		glDeleteVertexArrays(1, &m_Vao);
		glDeleteBuffers(1, &m_Vbo);
	}

	void createMesh();
	void drawMesh();
	void setTextureID(GLuint id);
private:
	GLuint m_TextureID = 0;
	unsigned int m_Vbo = 0, m_Ebo = 0, m_Vao = 0;
	void applyBoundingBox();
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

