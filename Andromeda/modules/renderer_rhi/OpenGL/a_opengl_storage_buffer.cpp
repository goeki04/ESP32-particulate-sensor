#include "a_opengl_storage_buffer.hpp"
#include "a_Primitives.hpp"

namespace Andromeda {
	void GLStorageBuffer::bind(u32 bindingPoint)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_BufferID);
	}

	void GLStorageBuffer::setData(const void* data, u32 size, u32 offset)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferID);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void GLStorageBuffer::create(BufferUsage usage, u32 size, const void* initialData)
	{
		if (m_BufferID != 0) {
			glDeleteBuffers(1, &m_BufferID);
			m_BufferID = 0;
		}
		GLenum glUsage = BufferUsageToGL(usage);
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, initialData, glUsage);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		m_Size = size;
	}

	u32 GLStorageBuffer::getSize() const
	{
		return m_Size;
	}
}
