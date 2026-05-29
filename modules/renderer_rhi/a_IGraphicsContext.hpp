#pragma once
#include <string>
#include "a_primitives.hpp"
#include <span>
#include <vector>
#include "a_rhi_types.hpp"
#include <memory>
#include "a_rhi_framebuffer.hpp"
#include "a_clearFlags.hpp"
#include "a_rhi_constant_buffer.hpp"
#include "a_samplerState.hpp"
namespace Andromeda {
	class IGraphicsContext {
	public:
		virtual ~IGraphicsContext() = default;

		virtual ShaderProgramHandle createShaderProgram(const std::string& vertSrc, const std::string& fragSrc) = 0;
		virtual void destroyShaderProgram(ShaderProgramHandle handle) = 0;
		virtual void bindShaderProgram(ShaderProgramHandle handle) = 0;
		virtual void setRenderPassSpecs(const RenderPassSpecs& specs) = 0;
		virtual void drawIndexed(u32 vaoID, u32 indexCount) = 0;
		virtual void drawArrays(u32 vao, u32 vertexCount) = 0;
		virtual void bindTextures(std::span<const TextureBinding> texture) = 0;
		virtual std::vector<ReflectedUniform> getProgramUniforms(ShaderProgramHandle handle) = 0;
		virtual void submitUniforms(std::span<const UniformData> uniforms) = 0;

		virtual void bindFramebuffer(std::shared_ptr<IFramebuffer> framebuffer) = 0;
		virtual void unbindFramebuffer() = 0;
		virtual void blitFramebuffer(std::shared_ptr<IFramebuffer> source, std::shared_ptr<IFramebuffer> target, bool copyDepth = false) = 0;
		virtual std::shared_ptr<IFramebuffer> createFramebuffer(const FramebufferSpecification& specs) = 0;
		virtual u32 createEmptyVAO() = 0;
		virtual void clear(ClearFlags flags, const vec4& color) = 0;
		virtual void clear(ClearFlags flags) = 0;
		virtual void setViewport(i32 vpPosX, i32 vpPosY, u32 vpWidth, u32 vpHeight) = 0;
		virtual void deleteVertexArrays(u32 vao) = 0;
		virtual i32 getUniformLocation(ShaderProgramHandle shader, const std::string& name) = 0;
		virtual std::shared_ptr<IConstantBuffer> createConstantBuffer(u32 size) = 0;
		virtual void bindTexture(u32 slot, u32 textureID) = 0;
		virtual void attachCubemapFace(u32 faceIndex, u32 cubemapTexID) = 0;
		virtual void bindTextureCube(u32 slot, u32 textureID) = 0;
		virtual void framebufferTexture2D(u32 faceIndex, u32 textureID, u32 mip) = 0;
		virtual void setSamplerState(const SamplerState& state) = 0;
		virtual void initRenderContext() = 0;
		/// <summary>
		/// This should be optimized in future and is DEFINITELY not the way to go. Instead of storing uniform locations we should use SPIRV-Reflect.
		/// </summary>
		virtual void setParameter(ShaderProgramHandle shader, const std::string& name, const mat4& matrix) = 0;
		virtual void setParameter(ShaderProgramHandle shader, const std::string& name, const vec3& vector) = 0;
		virtual void setParameter(ShaderProgramHandle shader, const std::string& name, i32 value) = 0;
	};
}