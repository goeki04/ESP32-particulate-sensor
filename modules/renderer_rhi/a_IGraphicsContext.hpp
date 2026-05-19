#pragma once
#include <string>
#include "a_Primitives.hpp"
namespace Andromeda {
	struct ShaderProgramHandle {
		u32 apiID = 0;
	};

	class IGraphicsContext {
	public:
		virtual ~IGraphicsContext() = default;

		virtual ShaderProgramHandle createShaderProgram(const std::string& vertSrc, const std::string& fragSrc) = 0;
		virtual void destroyShaderProgram(ShaderProgramHandle handle) = 0;
	};
}