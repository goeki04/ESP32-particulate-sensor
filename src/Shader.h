#pragma once
#include "camera.h"
class Shader {
	public:
	const char* vertexShaderPath;
	const char* fragmentShaderPath;
	unsigned int m_Program = 0;
	Camera& m_Camera;
	Shader(Camera& cam,const char* vertexPath, const char* fragmentPath) :
		 m_Camera(cam),vertexShaderPath(vertexPath), fragmentShaderPath(fragmentPath) {
	}
	virtual ~Shader() = default;
	virtual void setUniforms() = 0;
	virtual void compileShader() = 0;
	std::string readShaderSource(const char* shaderPath);
	void use() {
		glUseProgram(m_Program);
	}
	void setMat4x4(const char* uniformName, const glm::mat4& matrix);
};

enum class shaderType{
	unlit
};

class UnlitShader : public Shader {
	public:
	UnlitShader(Camera& cam, const char* vertexPath, const char* fragmentPath) : Shader(cam,vertexPath, fragmentPath) {}
	void setUniforms() override;
	void compileShader() override;
};