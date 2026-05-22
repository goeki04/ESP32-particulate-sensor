
#include "a_opengl_constant_buffer.hpp"

namespace Andromeda {
	void GLConstantBuffer::setData(const void* data, i32 size) {
		glBindBuffer(GL_UNIFORM_BUFFER, m_id);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
	}

	void GLConstantBuffer::initialize(i32 size) {
		if (m_id == 0) {
			glGenBuffers(1, &m_id);
			glBindBuffer(GL_UNIFORM_BUFFER, m_id);
			glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		}
	}
	void GLConstantBuffer::bind(u32 bindingPoint) {
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_id);
	}
}