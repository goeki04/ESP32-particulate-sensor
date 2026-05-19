#include "a_IGraphicsContext.hpp"
namespace Andromeda {
    class OpenGLContext : public IGraphicsContext {
    public:
        ShaderProgramHandle createShaderProgram(const std::string& vertSrc, const std::string& fragSrc) override {
            // Hier kommt dein bisheriger glCreateShader, glCompileShader, glLinkProgram Code rein
            uint32_t id = compileOpenGLShader(vertSrc, fragSrc);
            return ShaderProgramHandle{ id };
        }

        void destroyShaderProgram(ShaderProgramHandle handle) override {
            if (handle.apiID != 0) {
                glDeleteProgram(handle.apiID);
            }
        }

        void bindShaderProgram(ShaderProgramHandle handle) override {
            glUseProgram(handle.apiID);
        }

        void drawIndexed(uint32_t indexCount) override {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        }
    };
}