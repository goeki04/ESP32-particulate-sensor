#pragma once
#include "a_primitives.hpp"
namespace Andromeda {
	struct MeshGPUHandle {
        u32 vao = 0;
        u32 vbo = 0;
        u32 ebo = 0;
        u32 indexCount = 0;
	};
}