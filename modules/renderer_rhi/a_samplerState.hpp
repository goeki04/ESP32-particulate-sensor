#pragma once

namespace Andromeda {

	enum class FilterModeMin {
		Nearest,
		Billinear,
		Trillinear
	};

	enum class FilterModeMag {
		Nearest,
		Billinear
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

        /**
         * @brief Constructs a SamplerState with sensible defaults.
         * * Default settings:
         * - type: Texture2D
         * - minFilter: Billinear
         * - magFilter: Billinear
         * - wrap modes: ClampToEdge
         */
        SamplerState() :
            type(TextureType::Texture2D),
            minFilter(FilterModeMin::Billinear),
            magFilter(FilterModeMag::Billinear),
            wrapS(WrapMode::ClampToEdge),
            wrapT(WrapMode::ClampToEdge),
            wrapR(WrapMode::ClampToEdge) {
        }

        /**
         * @brief Constructs a SamplerState with custom values.
         */
        SamplerState(TextureType type, FilterModeMin minF, FilterModeMag magF, WrapMode s, WrapMode t, WrapMode r) :
            type(type), minFilter(minF), magFilter(magF), wrapS(s), wrapT(t), wrapR(r) {
        }
    };
}