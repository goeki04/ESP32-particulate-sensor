#pragma once

/**
 * @file a_clearFlags.hpp
 * @brief Bitmask flags selecting which framebuffer attachments to clear before rendering.
 */

#include "a_primitives.hpp"
namespace Andromeda {
    /**
     * @enum ClearFlags
     * @brief Bitmask describing which buffer(s) the RHI should clear (e.g. before a render pass).
     * @note Combine values with operator| (e.g. @c ClearFlags::Color @c | @c ClearFlags::Depth)
     *       and test for a flag with operator& .
     */
    enum class ClearFlags : u8 {
        None = 0,               ///< Clear nothing.
        Color = 1 << 0,         ///< Clear the color attachment.
        Depth = 1 << 1,         ///< Clear the depth buffer.
        Stencil = 1 << 2,       ///< Clear the stencil buffer.
        All = Color | Depth | Stencil ///< Clear color, depth and stencil in one pass.
    };

    /** @brief Combines two clear flag sets (bitwise OR). */
    inline ClearFlags operator|(ClearFlags a, ClearFlags b) {
        return static_cast<ClearFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    /** @brief Intersects two clear flag sets (bitwise AND); useful for checking if a flag is set. */
    inline ClearFlags operator&(ClearFlags a, ClearFlags b) {
        return static_cast<ClearFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }
}