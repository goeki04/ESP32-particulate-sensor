#pragma once

/**
 * @file a_opengl_texture.hpp
 * @brief OpenGL texture handle (RAII) and the mapping from engine texture formats to GL enums.
 */

#include "a_primitives.hpp"
#include "GL/Glew.h"
#include <SDL3/SDL.h>
#include "a_texture.hpp"
namespace Andromeda {
	/**
	 * @struct GLTextureFormat
	 * @brief The triple of OpenGL enums describing how a texture's pixels are stored and supplied.
	 */
	struct GLTextureFormat {
		GLenum internalFormat; ///< GPU-side storage format (e.g. GL_RGBA8).
		GLenum dataFormat;     ///< Layout of the source pixel data (e.g. GL_RGBA).
		GLenum dataType;       ///< Component type of the source data (e.g. GL_UNSIGNED_BYTE).
	};

	/**
	 * @struct GLtexture
	 * @brief Move-only RAII wrapper owning an OpenGL texture object and its dimensions.
	 *
	 * @details On destruction the texture is deleted (guarded by a current-GL-context check, so it is
	 *          safe even during shutdown). Copying is disabled to prevent double-frees of the GL object.
	 */
	struct GLtexture
	{
		u32 id = 0; ///< OpenGL texture object ID (0 if none).
		int w = 0;  ///< Texture width in pixels.
		int h = 0;  ///< Texture height in pixels.

		GLtexture() = default;
		GLtexture(const GLtexture&) = delete;
		GLtexture& operator=(const GLtexture&) = delete;

		/** @brief Move-constructs, taking ownership of the other texture's GL object. */
		GLtexture(GLtexture&& other) noexcept
		{
			*this = std::move(other);
		}

		/** @brief Move-assigns, deleting this texture first and taking ownership of the other's. */
		GLtexture& operator=(GLtexture&& other) noexcept
		{
			if (this != &other) {
				destroy();
				id = other.id;
				w = other.w;
				h = other.h;
				other.id = 0;
				other.w = other.h = 0;
			}
			return *this;
		}

		/** @brief Destroys the wrapper, freeing the owned GL texture. */
		~GLtexture() { destroy(); }

		/** @brief Deletes the owned OpenGL texture if a GL context is current, then resets the ID. */
		void destroy()
		{
			if (id != 0) {
				if (SDL_GL_GetCurrentContext()) {
					glDeleteTextures(1, &id);
				}
				id = 0;
			}
		}

		/**
		 * @brief Maps an engine @c TextureFormat to the corresponding OpenGL format triple.
		 * @param format The backend-agnostic texture format.
		 * @return The matching internal/data/type GL enums (all zero for an unsupported format).
		 */
		inline static GLTextureFormat ConvertFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::RGBA8_UNORM:
				return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
			case TextureFormat::RGBA8_SRGB:
				return { GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE };
			case TextureFormat::RG8_UNORM:
				return { GL_RG8, GL_RG, GL_UNSIGNED_BYTE };
			case TextureFormat::R8_UNORM:
				return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };

			case TextureFormat::RGBA16_FLOAT:
				return { GL_RGBA16F, GL_RGBA, GL_FLOAT };
			case TextureFormat::RG16_FLOAT:
				return { GL_RG16F, GL_RG, GL_FLOAT };
			case TextureFormat::RGBA32_FLOAT:
				return { GL_RGBA32F, GL_RGBA, GL_FLOAT };

			case TextureFormat::D24_UNORM_S8_UINT:
				return { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 };
			case TextureFormat::D32_FLOAT:
				return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT };

			default:
				return { 0, 0, 0 };
			}
		}
	};
}