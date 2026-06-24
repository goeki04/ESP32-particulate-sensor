#pragma once

/**
 * @file a_ImGuiOperators.hpp
 * @brief Convenience arithmetic/comparison operators for ImGui's @c ImVec2 type.
 *
 * @details ImGui does not define these operators by default (unless
 *          @c IMGUI_DEFINE_MATH_OPERATORS is set). They are provided here in the
 *          @c Andromeda::Gui namespace so editor code can do vector math on screen
 *          coordinates and sizes directly.
 */

namespace Andromeda::Gui{
    /** @brief Component-wise addition of two 2D vectors. */
    inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
        return { a.x + b.x, a.y + b.y };
    }
    /** @brief Component-wise subtraction of two 2D vectors. */
    inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
        return { a.x - b.x, a.y - b.y };
    }
    /** @brief Exact equality comparison of two 2D vectors. */
    inline bool operator==(const ImVec2& a, const ImVec2& b) {
        if (a.x == b.x && a.y == b.y) return true;
        return false;
    }
    /** @brief Scales a 2D vector by a scalar factor. */
    inline ImVec2 operator*(const ImVec2& a, const float b) {
        return { a.x * b, a.y * b };
    }
}