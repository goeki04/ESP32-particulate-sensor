#pragma once
#include <string>
#include "a_Primitives.hpp"
#include <span>
#include <vector>
#include "a_rhi_types.hpp"
#include <memory>
#include "a_rhi_framebuffer.hpp"
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
		virtual void clear(const vec4& color) = 0;
		virtual void setViewport(i32 vpPosX, i32 vpPosY, u32 vpWidth, u32 vpHeight) = 0;
		virtual void deleteVertexArrays(u32 vao) = 0;
	};
}