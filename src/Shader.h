#pragma once

class Shader {
	public:
	const char* vertexShaderPath;
	const char* fragmentShaderPath;
	unsigned int m_Program = 0;

	Shader(const char* vertexPath, const char* fragmentPath) :
			vertexShaderPath(vertexPath), fragmentShaderPath(fragmentPath) {
	}
	virtual ~Shader() {};

	virtual void setProperties() = 0;
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
	UnlitShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath) {}
	void setProperties() override;
	void compileShader() override;
};