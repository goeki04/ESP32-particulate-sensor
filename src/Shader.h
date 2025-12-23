#pragma once
#include "camera.h"
struct DirLight{
	glm::vec3 direction;
	glm::vec3 color;
};

class Shader {
	public:
	const char* m_VertexShaderPath;
	const char* m_FragmentShaderPath;
	unsigned int m_Program = 0;
	Camera& m_Camera;
	DirLight m_DirLight;
	Shader(Camera& cam,const char* vertexPath, const char* fragmentPath) :
		 m_Camera(cam),m_VertexShaderPath(vertexPath), m_FragmentShaderPath(fragmentPath){
		m_DirLight.color = glm::vec3(1.0f,1.0f,1.0f);
		m_DirLight.direction = glm::vec3(1.0f,1.0f,0.5f);
	}
	virtual ~Shader() = default;
	virtual void setUniforms(GLuint textureID, glm::mat4& modelMatrix) = 0;
	virtual void compileShader() = 0;
	std::string readShaderSource(const char* shaderPath);
	void use() {
		glUseProgram(m_Program);
	}
	void setMat4x4(const char* uniformName, const glm::mat4& matrix);
	void setVec3(const char* uniformName, const glm::vec3& vector);
	void setTexture(const char* uniformName, const GLuint textureID);
};

enum class shaderType{
	unlit
};

class UnlitShader : public Shader {
	public:
	UnlitShader(Camera& cam, const char* vertexPath, const char* fragmentPath) : Shader(cam,vertexPath, fragmentPath) {}
	void setUniforms(GLuint textureID, glm::mat4& modelMatrix) override;
	void compileShader() override;
};