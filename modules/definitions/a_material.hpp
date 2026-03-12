#pragma once
#include "a_primitives.hpp"
namespace Andromeda {
	struct DirLight {
		vec3 direction;
		vec3 color;
	};

	enum class MaterialShaderType { unlit, white };
	enum class ProceduralShaderType { grid };
	enum class PostProcessShaderType { outline, maskVertex };
}