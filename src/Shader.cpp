#include "pch.h"
#include "Shader.h"
#include "WindowManager.h"
#include "camera.h"
std::string MaterialShader::readShaderSource(const char* shaderPath)
{
    std::ifstream fileStream(shaderPath);
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    std::string shaderSource = buffer.str();
    return shaderSource;
}

void MaterialShader::setMat4x4(const char* uniformName, const glm::mat4& matrix)
{
    GLuint matrixLocation = glGetUniformLocation(m_Program, uniformName);
    if (matrixLocation == -1) {
        throw std::runtime_error("Uniform location for matrix4x4 not found");
    }
    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(matrix)); //value ptr gets a pointer of the internal float data.
}

void MaterialShader::setVec3(const char* uniformName,const glm::vec3& vector) {
    GLuint vec3Location = glGetUniformLocation(m_Program,uniformName);
    if (vec3Location == -1) {
        throw std::runtime_error("Uniform location for vec3 not found");
    }
    glUniform3fv(vec3Location,1,glm::value_ptr(vector));
}

void MaterialShader::setTexture(const char* uniformName, const GLuint textureID)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(m_Program, uniformName), 0);
}

void UnlitShader::setUniforms(glm::mat4& modelMatrix)
{
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    use();
    setMat4x4("model", modelMatrix);
    setMat4x4("projection", Camera::getProjectionMatrix());
    setMat4x4("view", m_Camera.m_ViewMatrix);
    setVec3("sunLight.color", m_DirLight.color);
    setVec3("sunLight.direction", m_DirLight.direction);
    setVec3("ambientLight",m_AmbientLight);
}

void MaterialShader::compileShader()
{
    GLint success;
    std::string vertexShaderStr = readShaderSource(m_VertexShaderPath);
    const char* vertexShaderSource = vertexShaderStr.c_str();
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        throw std::runtime_error("failed compiling vertex shader!");
    }
    std::string fragmentShaderStr = readShaderSource(m_FragmentShaderPath);
    const char* fragmentShaderSource = fragmentShaderStr.c_str();
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        throw std::runtime_error("failed compiling fragment shader!");
    }
    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertexShader);
    glAttachShader(m_Program, fragmentShader);
    glLinkProgram(m_Program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
    if (!success) {
        throw std::runtime_error("failed to create a shader program!");
    }
}

void GridShader::setUniforms()
{
    setCameraUniforms();
}

void GridShader::setUniforms()
{
}
