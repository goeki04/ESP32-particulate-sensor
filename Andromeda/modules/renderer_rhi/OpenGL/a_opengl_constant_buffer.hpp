#pragma once
#include <GL/Glew.h>
#include "a_rhi_constant_buffer.hpp"
#include "a_Primitives.hpp"
namespace Andromeda {
	class GLConstantBuffer : public IConstantBuffer{
	public:
		GLConstantBuffer() {
		}
		~GLConstantBuffer() override {
			glDeleteBuffers(1, &m_id);
		};

		void setData(const void* data, i32 size) override;
		void initialize(i32 size);
		void bind(u32 bindingPoint) override;
	private:
		GLuint m_id = 0;
	};
}