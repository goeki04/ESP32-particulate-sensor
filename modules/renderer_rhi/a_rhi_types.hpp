#pragma once
#include "a_Primitives.hpp"
#include <string>
namespace Andromeda {
	enum class ShaderDataType {
		Mat4, Mat3, Vec4, Vec3, Vec2, Float, Int, Texture2D, TextureCube
	};

	enum class CullMode {
		None, Back, Front
	};

	enum class DepthFunc { Less, Equal, LEqual};

	enum class BlendMode { None, AlphaBlend };

	enum class RasterizerMode { Fill, Wireframe };

	struct RenderPassSpecs {
		RasterizerMode rasterizerMode = RasterizerMode::Fill;
		CullMode cullMode = CullMode::Back;
		bool depthTest = true;
		DepthFunc depthFunction = DepthFunc::Less;
		BlendMode blendMode = BlendMode::None;
	};

	struct ShaderProgramHandle {
		u32 apiID = 0;
	};

	struct TextureBinding {
		u32 apiID = 0;
	};

	struct ReflectedUniform {
		std::string name;
		u32 location;
		ShaderDataType type;
	};

	struct UniformData {
		u32 location;
		u32 size;
		const void* dataPtr;
		ShaderDataType type;
	};
}