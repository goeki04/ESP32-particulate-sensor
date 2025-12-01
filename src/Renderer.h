#pragma once
#include "ISubsystem.h"
#include "GuiManager.h"
class WindowManager;
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
struct Mesh{
	std::vector<Vertex> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
public:
	Mesh(std::vector<Vertex>&& vertexPositions, std::vector<unsigned int>&& vertexIndices)
		: vertexBuffer(std::move(vertexPositions)),
		indexBuffer(std::move(vertexIndices)) {};
	Mesh() {};
};

class Renderer : public ISubsystem{
public:
	void start() override;
	void update() override;
	void destroy() override;
	static constexpr const char* glsl_version = "#version 150";
private:
	std::vector<Mesh> m_Meshes;
	glm::ivec2 m_framebufferSize;
	unsigned int m_Framebuffer = 0;
	unsigned int m_FramebufferTexture = 0;
	unsigned int m_Rendererbuffer = 0;
	unsigned int m_DefaultShader = 0;
	const char* readShaderSource(const char* shaderPath);
	void compileDefaultShader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void createFramebuffer();

	void loadObjModel(const std::string& path);
	WindowManager* m_WindowManager;
	GuiManager m_GuiManager;
#ifdef DEBUG_RENDERING_OPENGL
	bool m_DebugMode = false;
#endif
};