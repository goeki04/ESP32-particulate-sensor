#pragma once
#include "a_primitives.hpp"
namespace Andromeda {
    enum class ClearFlags : u8 {
        None = 0,
        Color = 1 << 0,
        Depth = 1 << 1,
        Stencil = 1 << 2,
        All = Color | Depth | Stencil
    };

    inline ClearFlags operator|(ClearFlags a, ClearFlags b) {
        return static_cast<ClearFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    inline ClearFlags operator&(ClearFlags a, ClearFlags b) {
        return static_cast<ClearFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }
}