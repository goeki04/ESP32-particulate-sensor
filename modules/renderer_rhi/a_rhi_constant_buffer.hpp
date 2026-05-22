#pragma once
#include "a_Primitives.hpp"
namespace Andromeda {

	class IConstantBuffer {
	public:
		virtual ~IConstantBuffer() = default;

		virtual void setData(const void* data, i32 size) = 0;
		virtual void bind(u32 bindingPoint) = 0;
	};
}