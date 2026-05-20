#include "shader.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <glm/gtc/type_ptr.hpp>

namespace Andromeda {

    std::string Shader::readShaderSource(const char* shaderPath)
    {
        std::ifstream fileStream(shaderPath);
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        return buffer.str();
    }

    void Shader::setCameraUniforms(const amath::CameraData* cam) {
        assert(cam && "CameraData is nullptr in Shader::setCameraUniforms");
        setMat4x4(c_viewMatrix, cam->viewMatrix);
        setMat4x4(c_projMatrix, cam->projection);
        setVec3(c_camPos, cam->cameraPos);
    }

    void Shader::setMat4x4(const char* uniformName, const mat4& matrix)
    {
        GLuint matrixLocation = getUniformLocation(uniformName);
        glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::setVec3(const char* uniformName, const vec3& vector) {
        GLuint vec3Location = getUniformLocation(uniformName);
        glUniform3fv(vec3Location, 1, glm::value_ptr(vector));
    }

    void Shader::setVec2(const char* uniformName, const vec2& vector)
    {
        GLuint vec2Location = getUniformLocation(uniformName);
        glUniform2fv(vec2Location, 1, glm::value_ptr(vector));
    }

    void Shader::setFloat(const char* uniformName, const float floatVal)
    {
        GLuint floatLocation = getUniformLocation(uniformName);
        glUniform1f(floatLocation, floatVal);
    }

    void Shader::setInt(const char* uniformName, const i32 intValue)
    {
        GLuint intLocation = getUniformLocation(uniformName);
        glUniform1i(intLocation, intValue);
    }

    void Shader::setTexture(const char* uniformName, u32 textureID, u32 slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, textureID);
        setInt(uniformName, slot);
    }

    void Shader::compileShader()
    {
        auto throwShaderLog = [](u32 sh, const char* stage) {
            i32 len = 0;
            glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
            std::string log;
            log.resize((len > 1) ? len : 1);

            GLsizei outLen = 0;
            glGetShaderInfoLog(sh, (GLsizei)log.size(), &outLen, log.data());
            log.resize(outLen);
            throw std::runtime_error(std::string(stage) + " compile error:\n" + log);
            };

        i32 success;
        std::string vertexShaderStr = readShaderSource(m_VertexShaderPath.c_str());
        const char* vertexShaderSource = vertexShaderStr.c_str();
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            throwShaderLog(vertexShader, "Vertex shader");
        }

        std::string fragmentShaderStr = readShaderSource(m_FragmentShaderPath.c_str());
        const char* fragmentShaderSource = fragmentShaderStr.c_str();
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            throwShaderLog(fragmentShader, "Fragment shader");
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
        m_UniformCache.clear();
    }
}