#pragma once

/**
 * @file a_rhi_constant_buffer.hpp
 * @brief Backend-agnostic interface for a constant/uniform buffer (UBO).
 */

#include "a_Primitives.hpp"
namespace Andromeda {

	/**
	 * @class IConstantBuffer
	 * @brief Abstract GPU constant buffer used to upload uniform block data to shaders.
	 * @details Implemented per backend (e.g. @c GLConstantBuffer for OpenGL).
	 */
	class IConstantBuffer {
	public:
		virtual ~IConstantBuffer() = default;

		/**
		 * @brief Uploads (replaces) the buffer's contents.
		 * @param data Pointer to the source data.
		 * @param size Number of bytes to upload.
		 */
		virtual void setData(const void* data, i32 size) = 0;

		/**
		 * @brief Binds the buffer to a uniform block binding point so shaders can read it.
		 * @param bindingPoint The binding index referenced by the shader's layout(binding = N).
		 */
		virtual void bind(u32 bindingPoint) = 0;
	};
}