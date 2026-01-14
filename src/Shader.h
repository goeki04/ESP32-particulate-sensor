#pragma once
#include "camera.h"
struct DirLight{
	glm::vec3 direction;
	glm::vec3 color;
};

class Shader {
public:
	Camera& m_Camera;
	GLuint m_Program = 0;
	std::string m_VertexShaderPath;
	std::string m_FragmentShaderPath;
	Shader(Camera& cam, const char* vertexPath, const char* fragmentPath) :
		m_Camera(cam), m_VertexShaderPath(vertexPath), m_FragmentShaderPath(fragmentPath) {
	}
	virtual ~Shader() = default;
	void setMat4x4(const char* uniformName, const glm::mat4& matrix);
	void setVec3(const char* uniformName, const glm::vec3& vector);
	void setTexture(const char* uniformName, const GLuint textureID);
	std::string readShaderSource(const char* shaderPath);
	void compileShader();
	void use() {
		glUseProgram(m_Program);
	}

	void setCameraUniforms() {
		setMat4x4("view", m_Camera.getViewMatrix());
		setMat4x4("proj", m_Camera.getProjectionMatrix());
		setVec3("camPos", m_Camera.getCameraPos());
	}
};

class MaterialShader  : public Shader{
	public:
	MaterialShader(Camera& cam,const char* vertexPath, const char* fragmentPath) : Shader(cam,vertexPath,fragmentPath){
	}
	virtual ~MaterialShader() = default;
	virtual void setUniforms(const glm::mat4& modelMatrix) = 0;
};

class ProceduralShader : public Shader{
public:
	ProceduralShader(Camera& cam,const char* vertexPath, const char* fragmentPath) : Shader(cam,vertexPath,fragmentPath){
	}
	virtual ~ProceduralShader() = default;
	virtual void setUniforms() = 0;

};

class GridShader : public ProceduralShader {
public:
	
	GridShader(Camera& cam,const char* vertexPath, const char* fragmentPath) : ProceduralShader(cam,vertexPath, fragmentPath)  {}
	void setUniforms() override;
};

class UnlitShader : public MaterialShader {
public:
	DirLight m_DirLight;
	glm::vec3 m_AmbientLight;
	UnlitShader(Camera& cam, const char* vertexPath, const char* fragmentPath) : MaterialShader(cam, vertexPath, fragmentPath) {
		m_DirLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
		m_DirLight.direction = glm::vec3(1.0f, 1.0f, 0.5f);
		m_AmbientLight = glm::vec3(0.4f);
	}
	void setUniforms(const glm::mat4& modelMatrix) override;
};
