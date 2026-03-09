#pragma once
namespace Andromeda {
	struct DirLight {
		glm::vec3 direction;
		glm::vec3 color;
	};

	enum class MaterialShaderType { unlit, white };
	enum class ProceduralShaderType { grid };
	enum class PostProcessShaderType { outline, maskVertex };
}