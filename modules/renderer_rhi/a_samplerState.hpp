#pragma once

namespace Andromeda {
	enum class FilterModeMin {
		Nearest,
		Billinear,
		Trillinear
	};

	enum class FilterModeMag {
		Nearest,
		Linear
	};

	enum class WrapMode {
		ClampToEdge
	};

	enum class TextureType {
		Texture2D,
		Cubemap
	};

	struct SamplerState {
		TextureType type;
		FilterModeMin minFilter;
		FilterModeMag magFilter;
		WrapMode wrapS;
		WrapMode wrapT;
		WrapMode wrapR;
	};
}