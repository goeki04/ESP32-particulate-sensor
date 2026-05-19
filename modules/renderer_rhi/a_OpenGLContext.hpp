#include "a_IGraphicsContext.hpp"
namespace Andromeda {
    class OpenGLContext : public IGraphicsContext {
    public:
        ShaderProgramHandle createShaderProgram(const std::string& vertSrc, const std::string& fragSrc) override;
        void destroyShaderProgram(ShaderProgramHandle handle) override;
        void bindShaderProgram(ShaderProgramHandle handle) override;
    };
}