#include "a_IGraphicsContext.hpp"
#include <string>
#include "a_clearFlags.hpp"
#include "a_rhi_constant_buffer.hpp"
namespace Andromeda {
    class SamplerState;

    /**
     * @class OpenGLContext
     * @brief Handles low-level rendering operations and state management for the OpenGL backend.
     * * This class implements the graphics interface for OpenGL, managing shader programs,
     * uniform uploads, texture binding, framebuffer operations, and render pipeline states
     * (e.g., culling, depth testing, blending). It serves as the primary bridge between
     * the engine's abstract rendering commands and OpenGL's API calls.
     */
    class OpenGLContext : public IGraphicsContext {
    public:
    public:
        /**
         * @brief Creates a shader program from vertex and fragment shader source files.
         * @param vertSrc Path to the vertex shader source file.
         * @param fragSrc Path to the fragment shader source file.
         * @return A ShaderProgramHandle identifying the created program.
         */
        ShaderProgramHandle createShaderProgram(const std::string& vertSrc, const std::string& fragSrc) override;

        /**
         * @brief Compiles vertex and fragment shaders and links them into a program.
         * @param vertSrc Path to the vertex shader.
         * @param fragSrc Path to the fragment shader.
         * @return The OpenGL program ID (apiID).
         */
        u32 compileOpenGLShader(const std::string& vertSrc, const std::string& fragSrc);

        /**
         * @brief Destroys an existing shader program and frees GPU resources.
         * @param handle The handle to the shader program to delete.
         */
        void destroyShaderProgram(ShaderProgramHandle handle) override;

        /**
         * @brief Reads a shader source file from disk into a string.
         * @param shaderPath The filesystem path to the shader source.
         * @return The shader source code as a string.
         */
        std::string readShaderSource(const char* shaderPath);

        /**
         * @brief Sets the active shader program for subsequent draw calls.
         * @param handle The handle of the shader program to use.
         */
        void bindShaderProgram(ShaderProgramHandle handle) override;

        /**
         * @brief Submits a collection of uniforms to the currently active shader.
         * @param uniforms A span containing uniform data.
         */
        void submitUniforms(std::span<const UniformData> uniforms) override;

        /**
         * @brief Binds multiple textures to their designated slots.
         * @param textures A span of texture binding structures.
         */
        void bindTextures(std::span<const TextureBinding> textures) override;

        /**
         * @brief Performs shader reflection to retrieve uniform metadata.
         * @param handle The shader program handle to reflect.
         * @return A vector of ReflectedUniform structures.
         */
        std::vector<ReflectedUniform> getProgramUniforms(ShaderProgramHandle handle) override;

        /**
         * @brief Configures render pipeline states like culling, depth testing, and blending.
         * @param specs The RenderPassSpecs defining the desired pipeline state.
         */
        void setRenderPassSpecs(const RenderPassSpecs& specs) override;

        /**
         * @brief Executes an indexed draw command using the specified vertex array.
         * @param vao The ID of the vertex array object.
         * @param indexCount The number of indices to render.
         */
        void drawIndexed(u32 vao, u32 indexCount) override;

        /**
         * @brief Executes a non-indexed draw command using the specified vertex array.
         * @param vao The ID of the vertex array object.
         * @param vertexCount The number of vertices to render.
         */
        void drawArrays(u32 vao, u32 vertexCount) override;

        /**
         * @brief Binds a framebuffer for rendering.
         * @param framebuffer A shared pointer to the framebuffer object.
         */
        void bindFramebuffer(std::shared_ptr<IFramebuffer> framebuffer) override;

        /**
         * @brief Unbinds the currently bound framebuffer, reverting to the default framebuffer.
         */
        void unbindFramebuffer() override;

        /**
         * @brief Copies a region of the framebuffer from source to target.
         * @param source The source framebuffer.
         * @param target The target framebuffer (pass nullptr to target the default framebuffer).
         * @param copyDepth Whether to include the depth buffer in the blit operation.
         */
        void blitFramebuffer(std::shared_ptr<IFramebuffer> source, std::shared_ptr<IFramebuffer> target, bool copyDepth = false) override;

        /**
         * @brief Generates an empty Vertex Array Object (VAO).
         * @return The VAO handle.
         */
        u32 createEmptyVAO() override;

        /**
         * @brief Clears the specified buffers with a given color.
         * @param flags The buffers to clear (Color, Depth, Stencil).
         * @param color The clear color (vec4).
         */
        void clear(ClearFlags flags, const vec4& color) override;

        /**
         * @brief Clears the specified buffers.
         * @param flags The buffers to clear (Color, Depth, Stencil).
         */
        void clear(ClearFlags flags) override;

        /**
         * @brief Initializes the rendering context and sets global OpenGL defaults.
         */
        void initRenderContext() override;

        /**
         * @brief Creates a constant buffer for uniform data.
         * @param size Size in bytes.
         * @return A shared pointer to the created IConstantBuffer.
         */
        std::shared_ptr<IConstantBuffer> createConstantBuffer(u32 size) override;

        /**
         * @brief Sets the active viewport dimensions.
         * @param vpPosX X-offset of the viewport.
         * @param vpPosY Y-offset of the viewport.
         * @param vpWidth Width of the viewport.
         * @param vpHeight Height of the viewport.
         */
        void setViewport(i32 vpPosX, i32 vpPosY, u32 vpWidth, u32 vpHeight) override;

        /**
         * @brief Creates a framebuffer object based on the provided specifications.
         * @param specs The framebuffer requirements.
         * @return A shared pointer to the IFramebuffer object.
         */
        std::shared_ptr<IFramebuffer> createFramebuffer(const FramebufferSpecification& specs) override;

        /**
         * @brief Deletes a vertex array object from the GPU.
         * @param vao The VAO ID to delete.
         */
        void deleteVertexArrays(u32 vao) override;

        /**
         * @brief Retrieves the location of a uniform variable within a shader.
         * @param shader The shader program handle.
         * @param name The name of the uniform.
         * @return The uniform location integer.
         */
        i32 getUniformLocation(ShaderProgramHandle shader, const std::string& name);

        /**
         * @brief Binds a 2D texture to a specific texture unit.
         * @param slot The texture unit index.
         * @param textureID The OpenGL texture ID.
         */
        void bindTexture(u32 slot, u32 textureID);

        /**
         * @brief Attaches a cubemap face to the currently bound framebuffer.
         * @param faceIndex The index of the cubemap face (0-5).
         * @param cubemapTexID The OpenGL texture ID of the cubemap.
         */
        void attachCubemapFace(u32 faceIndex, u32 cubemapTexID);

        /**
         * @brief Sets a matrix uniform parameter.
         */
        void setParameter(ShaderProgramHandle shader, const std::string& name, const mat4& matrix) override;

        /**
         * @brief Sets a vector uniform parameter.
         */
        void setParameter(ShaderProgramHandle shader, const std::string& name, const vec3& vector) override;

        /**
         * @brief Sets an integer uniform parameter.
         */
        void setParameter(ShaderProgramHandle shader, const std::string& name, i32 value) override;

        /**
         * @brief Applies sampler state parameters to the currently bound texture.
         * @param state The SamplerState configuration.
         */
        void bindSamplerState(u32 textureID, const SamplerState& state) override;

        /**
         * @brief Binds a cubemap texture to a specific texture unit.
         */
        void bindTextureCube(u32 slot, u32 textureID) override;

        /**
         * @brief Attaches a 2D texture level to the currently bound framebuffer.
         */
        void framebufferTexture2D(u32 faceIndex, u32 textureID, u32 mip) override;

        void generateMipmap(TextureType type) override;
    private:
        RenderPassSpecs m_CurrentSpecs;
        bool m_IsFirstContextInit = true;
    };
}