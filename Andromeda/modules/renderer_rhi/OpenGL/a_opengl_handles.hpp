#pragma once

/**
 * @file a_opengl_handles.hpp
 * @brief OpenGL object handles for an uploaded mesh.
 */

#include "a_primitives.hpp"
namespace Andromeda {
	/**
	 * @struct MeshGPUHandle
	 * @brief Holds the OpenGL buffer/array object IDs for a mesh that has been uploaded to the GPU.
	 */
	struct MeshGPUHandle {
        u32 vao = 0;        ///< Vertex Array Object ID.
        u32 vbo = 0;        ///< Vertex Buffer Object ID (vertex data).
        u32 ebo = 0;        ///< Element Buffer Object ID (index data).
        u32 indexCount = 0; ///< Number of indices, for the draw call.
	};
}