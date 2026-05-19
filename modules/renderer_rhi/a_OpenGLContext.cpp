#include "a_OpenGLContext.hpp"
#include 
namespace Andromeda {
    ShaderProgramHandle OpenGLContext::createShaderProgram(const std::string& vertSrc, const std::string& fragSrc) {
        uint32_t id = compileOpenGLShader(vertSrc, fragSrc);
        return ShaderProgramHandle{ id };
    }

    void OpenGLContext::destroyShaderProgram(ShaderProgramHandle handle) {
        if (handle.apiID != 0) {
            glDeleteProgram(handle.apiID);
        }
    }

    void OpenGLContext::bindShaderProgram(ShaderProgramHandle handle) {
        glUseProgram(handle.apiID);
    }
}