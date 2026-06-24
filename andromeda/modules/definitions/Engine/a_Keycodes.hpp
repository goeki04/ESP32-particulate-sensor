#pragma once

/**
 * @file a_Keycodes.hpp
 * @brief Platform-agnostic key and mouse button codes used by the input system.
 *
 * @details The numeric values intentionally mirror the USB HID usage table /
 *          SDL3 scancode values, so that input backends (e.g. SDL3) can map
 *          their native codes onto @c Keycode with a direct numeric cast
 *          instead of a lookup table. Gaps between value groups (e.g. between
 *          @c Z and @c D1) correspond to scancodes not currently exposed by
 *          the engine.
 */

#include <a_primitives.hpp>
namespace Andromeda {
    /**
     * @enum Keycode
     * @brief Identifies a physical keyboard key, independent of the underlying windowing/input backend.
     * @note Values are based on the USB HID/SDL3 scancode layout, not ASCII.
     */
    enum class Keycode : u8 {
        Unknown = 0, ///< No key / unrecognized scancode.

        A = 4, B, C, D, E, F, G, H, I, J, K, L, M, ///< Letter keys A-M.
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,     ///< Letter keys N-Z.
        D1 = 30, D2, D3, D4, D5, D6, D7, D8, D9, D0, ///< Top-row digit keys 1-9, 0 (in physical left-to-right order).

        F1 = 58, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, ///< Function keys F1-F12.

        Return = 40,    ///< Enter / Return key.
        Escape = 41,    ///< Escape key.
        Backspace = 42, ///< Backspace key.
        Tab = 43,       ///< Tab key.
        Space = 44,     ///< Spacebar.

        Insert = 73,    ///< Insert key.
        Home = 74,      ///< Home key.
        PageUp = 75,    ///< Page Up key.
        Delete = 76,    ///< Delete (forward delete) key.
        End = 77,       ///< End key.
        PageDown = 78,  ///< Page Down key.

        Right = 79, ///< Right arrow key.
        Left = 80,  ///< Left arrow key.
        Down = 81,  ///< Down arrow key.
        Up = 82,    ///< Up arrow key.

        LeftControl = 224,  ///< Left Control modifier key.
        LeftShift = 225,    ///< Left Shift modifier key.
        LeftAlt = 226,      ///< Left Alt modifier key.
        RightControl = 228, ///< Right Control modifier key.
        RightShift = 229,   ///< Right Shift modifier key.
        RightAlt = 230,     ///< Right Alt modifier key.
        Count ///< Sentinel value marking the number of distinct codes; not a real key.
    };

    /**
     * @enum MouseCode
     * @brief Identifies a physical mouse button.
     */
    enum class MouseCode {
        Unknown = 0,    ///< No button / unrecognized code.
        MouseBtnLeft,   ///< Primary (left) mouse button.
        MouseBtnRight,  ///< Secondary (right) mouse button.
        MouseBtnWheel   ///< Middle mouse button / scroll wheel click.
    };
}