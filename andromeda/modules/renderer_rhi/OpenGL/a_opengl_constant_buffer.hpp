#pragma once

/**
 * @file a_opengl_constant_buffer.hpp
 * @brief OpenGL implementation of the constant/uniform buffer (UBO) interface.
 */

#include <GL/Glew.h>
#include "a_rhi_constant_buffer.hpp"
#include "a_Primitives.hpp"
namespace Andromeda {
	/**
	 * @class GLConstantBuffer
	 * @brief OpenGL-backed uniform buffer object (UBO) implementing @c IConstantBuffer.
	 */
	class GLConstantBuffer : public IConstantBuffer{
	public:
		GLConstantBuffer() {
		}
		/** @brief Deletes the underlying OpenGL buffer object. */
		~GLConstantBuffer() override {
			glDeleteBuffers(1, &m_id);
		};

		/** @copydoc IConstantBuffer::setData */
		void setData(const void* data, i32 size) override;

		/**
		 * @brief Allocates the GPU buffer storage for a given size.
		 * @param size Buffer size in bytes.
		 */
		void initialize(i32 size);

		/** @copydoc IConstantBuffer::bind */
		void bind(u32 bindingPoint) override;
	private:
		GLuint m_id = 0; ///< OpenGL buffer object ID.
	};
}