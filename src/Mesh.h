#pragma once
#include "Shader.h"
struct Vertex {
public:
	float x;
	float y;
	float z;

	Vertex(float x, float y, float z) : x(x), y(y), z(z) {};
	Vertex() {
		x = 0;
		y = 0;
		z = 0;
	};
	Vertex operator-(glm::vec3 other) {
		return Vertex(x-other.x, y-other.y,z-other.z);
	}
};
class Mesh {
public:
	Shader* m_Shader = nullptr;
	std::vector<Vertex> m_VertexBuffer;
	std::vector<unsigned int> m_IndexBuffer;
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
private:
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

