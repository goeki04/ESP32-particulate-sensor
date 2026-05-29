#pragma once
#include "a_samplerState.hpp"
#include "a_primitives.hpp"
namespace Andromeda {
    enum class TextureFormat : u8
    {
        None = 0,

        RGBA8_UNORM,
        RGBA8_SRGB,
        RG8_UNORM,
        R8_UNORM,

        RGBA16_FLOAT,       
        RG16_FLOAT,
        RGBA32_FLOAT,

        D24_UNORM_S8_UINT,
        D32_FLOAT           
    };

	class Texture {
	public:
		u32 width = 0, height = 0, channels = 0;
        TextureFormat internalFormat = TextureFormat::None;
		SamplerState sampler;   
		u32 textureID = 0;
	};
}