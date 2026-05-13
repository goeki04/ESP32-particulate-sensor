#pragma once
#include "a_primitives.hpp"
#include <array>
namespace Andromeda{
    struct CubemapData{
        std::array<const char*, 6> faceTextures;
        i32 width;
        i32 height;
        i32 channels;
    };
}