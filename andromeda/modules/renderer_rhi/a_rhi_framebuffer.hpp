#pragma once
#include "a_Primitives.hpp"
#include "a_texture.hpp"
#include <vector>
namespace Andromeda {
	enum class FramebufferTextureFormat {
		None = 0,
		RGBA8,
		RGBA16F,

		DEPTH24Stencil8,
		DEPTH32F
	};

	struct FramebufferTextureSpecification {
		FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
		bool generateMipmaps = false;
	};

	struct FramebufferSpecification {
		u32 width = 0;
		u32 height = 0;
		u32 samples = 1;
		std::vector<FramebufferTextureSpecification> attachments;
	};

	class IFramebuffer {
	public:
		virtual ~IFramebuffer() = default;
		virtual void resize(const ivec2& newSize) = 0;
		virtual const FramebufferSpecification& getSpecification() const = 0;
		virtual const Texture& getColorAttachmentTexture(i32 index = 0) const = 0;
		std::vector<Texture> m_ColorAttachments;
	};
}