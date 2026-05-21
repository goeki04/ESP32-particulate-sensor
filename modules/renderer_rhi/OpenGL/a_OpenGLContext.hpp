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
        void drawArrays(u32 vao, u32 vertexCount) override;
        void bindFramebuffer(std::shared_ptr<IFramebuffer> framebuffer) override;
        void unbindFramebuffer() override;
        void blitFramebuffer(std::shared_ptr<IFramebuffer> source, std::shared_ptr<IFramebuffer> target, bool copyDepth = false) override;
        u32 createEmptyVAO() override;
        void clear(const vec4& color) override;
        void setViewport(i32 vpPosX, i32 vpPosY, u32 vpWidth, u32 vpHeight) override;
        std::shared_ptr<IFramebuffer> createFramebuffer(const FramebufferSpecification& specs) override;
        void deleteVertexArrays(u32 vao) override;
    private:
        RenderPassSpecs m_CurrentSpecs;
        bool m_IsFirstContextInit = true;
    };
}