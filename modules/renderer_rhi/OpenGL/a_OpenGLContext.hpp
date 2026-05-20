#include "a_IGraphicsContext.hpp"
#include <string>
namespace Andromeda {
    class OpenGLContext : public IGraphicsContext {
    public:
        ShaderProgramHandle createShaderProgram(const std::string& vertSrc, const std::string& fragSrc) override;
        u32 compileOpenGLShader(const std::string& vertSrc, const std::string& fragSrc);
        void destroyShaderProgram(ShaderProgramHandle handle) override;
        std::string readShaderSource(const char* shaderPath);
        void bindShaderProgram(ShaderProgramHandle handle) override;
        void submitUniforms(std::span<const UniformData> uniforms) override;
        void bindTextures(std::span<const TextureBinding> textures) override;
        std::vector<ReflectedUniform> getProgramUniforms(ShaderProgramHandle handle) override;
        void setRenderPassSpecs(const RenderPassSpecs& specs) override;
        void drawIndexed(u32 vao, u32 indexCount) override;

        virtual void bindFramebuffer(std::shared_ptr<IFramebuffer> framebuffer) override;
        virtual void unbindFramebuffer() override;
        virtual void clear(const vec4& color = vec4(0.1f, 0.1f, 0.1f, 1.0f)) override;
        virtual void blitFramebuffer(std::shared_ptr<IFramebuffer> source, std::shared_ptr<IFramebuffer> target, bool copyDepth = false) override;

        virtual std::shared_ptr<IFramebuffer> createFramebuffer(const FramebufferSpecification& specs) override;
    };
}