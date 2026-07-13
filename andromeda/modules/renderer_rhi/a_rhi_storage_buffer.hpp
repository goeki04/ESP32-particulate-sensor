#pragma once

/**
 * @file a_rhi_storage_buffer.hpp
 * @brief Compile-time contract (concept) describing a shader storage buffer (SSBO).
 */

#include "a_Primitives.hpp"
#include "a_rhi_types.hpp"
#include <concepts>
namespace Andromeda {

	/**
	 * @concept IsStorageBuffer
	 * @brief Constrains a type to the storage-buffer interface required by the engine.
	 *
	 * @details Storage buffers are used via static polymorphism: any backend type that provides
	 *          @c create / @c bind / @c unbind / @c setData / @c getSize satisfies this concept
	 *          and can be used directly (e.g. @c GLStorageBuffer). This keeps the hot path free
	 *          of virtual calls - the same goal @c RHIConstantBuffer and @c RHIFramebuffer pursue
	 *          via the Pimpl idiom instead (see a_rhi_constant_buffer.hpp / a_rhi_framebuffer.hpp).
	 *
	 * @tparam T The candidate storage-buffer type.
	 */
	template <typename T>
    concept IsStorageBuffer = requires(T buffer, u32 bindingPoint, const void* data, u32 size, u32 offset, BufferUsage usage) {
        { buffer.bind(bindingPoint) } -> std::same_as<void>;        ///< Bind to an SSBO binding point.
        { buffer.setData(data, size, offset) } -> std::same_as<void>; ///< Update a sub-range of the buffer.
        { buffer.getSize() } -> std::same_as<u32>;                  ///< Query the buffer's size in bytes.
        { buffer.create(usage, size, data) } -> std::same_as<void>; ///< Allocate the buffer with a usage hint and optional initial data.
    };
}