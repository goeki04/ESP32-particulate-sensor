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
};
class Mesh {
public:
	//std::unique_ptr<Shader> m_Shader;
	std::vector<Vertex> m_VertexBuffer;
	std::vector<unsigned int> m_IndexBuffer;
	Mesh(std::vector<Vertex>&& vertexPositions, std::vector<unsigned int>&& vertexIndices)
		: m_VertexBuffer(std::move(vertexPositions)),
		m_IndexBuffer(std::move(vertexIndices)) {
	};
	Mesh() {
		//m_Shader = std::make_unique<UnlitShader>("../src/shader/vertexShader.glsl", "../src/shader/fragmentShader.glsl");
	}

	~Mesh() {
		glDeleteVertexArrays(1, &m_Vao);
		glDeleteBuffers(1, &m_Vbo);
	}
	void createMesh();
	void drawMesh();
private:
	unsigned int m_Vbo = 0, m_Ebo = 0, m_Vao = 0;
};