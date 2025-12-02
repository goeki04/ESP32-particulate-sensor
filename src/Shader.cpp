#include "pch.h"
#include "Shader.h"
#include "WindowManager.h"
std::string Shader::readShaderSource(const char* shaderPath)
{
    std::ifstream fileStream(shaderPath);
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    std::string shaderSource = buffer.str();
    return shaderSource;
}

void Shader::setMat4x4(const char* uniformName, const glm::mat4& matrix)
{
    GLuint matrixLocation = glGetUniformLocation(m_Program, uniformName);
    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(matrix)); //value ptr gets a pointer of the internal float data.
}

void UnlitShader::setProperties()
{
    use();
    glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)WindowManager::m_WindowWidth / (float)WindowManager::m_WindowHeight, 0.1f, 100.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    setMat4x4("model", model);
    setMat4x4("projection", projection);
    setMat4x4("view", view);
}

void UnlitShader::compileShader()
{
    GLint success;
    std::string vertexShaderStr = readShaderSource(vertexShaderPath);
    const char* vertexShaderSource = vertexShaderStr.c_str();
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        throw std::runtime_error("failed compiling vertex shader!");
    }
    std::string fragmentShaderStr = readShaderSource(fragmentShaderPath);
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