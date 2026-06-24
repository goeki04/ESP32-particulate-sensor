#pragma once

/**
 * @file a_rhi_framebuffer.hpp
 * @brief Backend-agnostic framebuffer (render target) abstraction and its specification types.
 */

#include "a_Primitives.hpp"
#include "a_texture.hpp"
#include <vector>
namespace Andromeda {
	/**
	 * @enum FramebufferTextureFormat
	 * @brief Formats available for framebuffer color and depth/stencil attachments.
	 */
	enum class FramebufferTextureFormat {
		None = 0,        ///< Unspecified / invalid.
		RGBA8,           ///< 8-bit per channel color attachment.
		RGBA16F,         ///< 16-bit float per channel color attachment (HDR).

		DEPTH24Stencil8, ///< Combined 24-bit depth + 8-bit stencil attachment.
		DEPTH32F         ///< 32-bit float depth attachment.
	};

	/**
	 * @struct FramebufferTextureSpecification
	 * @brief Describes a single attachment requested for a framebuffer.
	 */
	struct FramebufferTextureSpecification {
		FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None; ///< Format of this attachment.
		bool generateMipmaps = false; ///< Whether to allocate/generate a mip chain for this attachment.
	};

	/**
	 * @struct FramebufferSpecification
	 * @brief Full description of a framebuffer to create: size, MSAA samples and its attachments.
	 */
	struct FramebufferSpecification {
		u32 width = 0;    ///< Width in pixels.
		u32 height = 0;   ///< Height in pixels.
		u32 samples = 1;  ///< MSAA sample count (1 = no multisampling).
		std::vector<FramebufferTextureSpecification> attachments; ///< Ordered list of color/depth attachments to create.
	};

	/**
	 * @class IFramebuffer
	 * @brief Abstract render target wrapping one or more attachments; implemented per backend.
	 */
	class IFramebuffer {
	public:
		virtual ~IFramebuffer() = default;

		/**
		 * @brief Recreates the framebuffer attachments at a new size.
		 * @param newSize The new dimensions in pixels.
		 */
		virtual void resize(const ivec2& newSize) = 0;

		/** @brief Returns the specification this framebuffer was created from. */
		virtual const FramebufferSpecification& getSpecification() const = 0;

		/**
		 * @brief Returns the texture backing a given color attachment.
		 * @param index Zero-based color attachment index.
		 * @return The color attachment texture.
		 */
		virtual const Texture& getColorAttachmentTexture(i32 index = 0) const = 0;

		std::vector<Texture> m_ColorAttachments; ///< Color attachment textures owned by this framebuffer.
	};
}