#pragma once

/**
 * @file a_opengl_storage_buffer.hpp
 * @brief OpenGL shader storage buffer (SSBO), e.g. for GPU compute particle data.
 */

#include "a_Primitives.hpp"
#include "GL/glew.h"
#include "a_rhi_types.hpp"
namespace Andromeda {
	/**
	 * @class GLStorageBuffer
	 * @brief OpenGL-backed shader storage buffer (SSBO). Satisfies the @c IsStorageBuffer concept.
	 *
	 * @details Move-only RAII wrapper around a GL buffer object bound to @c GL_SHADER_STORAGE_BUFFER.
	 *          Used for read/write GPU data shared between compute and render passes (e.g. particles).
	 */
	class GLStorageBuffer {
	public:
		GLStorageBuffer() = default;
		~GLStorageBuffer() {
			if (m_BufferID != 0) {
				glDeleteBuffers(1, &m_BufferID);
			}
		}

		GLStorageBuffer(const GLStorageBuffer&) = delete;
		GLStorageBuffer& operator=(const GLStorageBuffer&) = delete;

		GLStorageBuffer(GLStorageBuffer&& other) noexcept
			: m_BufferID(other.m_BufferID), m_Size(other.m_Size) {
			other.m_BufferID = 0;
			other.m_Size = 0;
		}

		GLStorageBuffer& operator=(GLStorageBuffer&& other) noexcept {
			if (this != &other) {
				if (m_BufferID != 0) glDeleteBuffers(1, &m_BufferID);
				m_BufferID = other.m_BufferID;
				m_Size = other.m_Size;
				other.m_BufferID = 0;
				other.m_Size = 0;
			}
			return *this;
		}
		/**
		 * @brief Binds the buffer to an SSBO binding point so shaders can access it.
		 * @param bindingPoint The binding index referenced by the shader's layout(std430, binding = N).
		 */
		void bind(u32 bindingPoint);

		/**
		 * @brief Updates a sub-range of the buffer with new data.
		 * @param data Pointer to the source bytes.
		 * @param size Number of bytes to write.
		 * @param offset Byte offset into the buffer where writing begins.
		 */
		void setData(const void* data, u32 size, u32 offset);

		/**
		 * @brief Allocates the buffer storage with a usage hint and optional initial data.
		 * @param usage Access/update-frequency hint.
		 * @param size Total buffer size in bytes.
		 * @param initialData Optional initial contents (nullptr leaves the storage uninitialized).
		 */
		void create(BufferUsage usage,u32 size, const void* initialData = nullptr);

		/** @brief Returns the buffer's allocated size in bytes. */
		u32 getSize() const;
	private:
		u32 m_BufferID = 0; ///< OpenGL buffer object ID.
		u32 m_Size = 0;     ///< Allocated size in bytes.
	};
}