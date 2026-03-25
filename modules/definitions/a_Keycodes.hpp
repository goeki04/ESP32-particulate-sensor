#pragma once
#include <a_primitives.hpp>
namespace Andromeda {
    enum class Keycode : u8 {
        Unknown = 0,

        A = 4, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        D1 = 30, D2, D3, D4, D5, D6, D7, D8, D9, D0,

        F1 = 58, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        Return = 40,
        Escape = 41,
        Backspace = 42,
        Tab = 43,
        Space = 44,

        Insert = 73,
        Home = 74,
        PageUp = 75,
        Delete = 76,
        End = 77,
        PageDown = 78,

        Right = 79,
        Left = 80,
        Down = 81,
        Up = 82,

        LeftControl = 224,
        LeftShift = 225,
        LeftAlt = 226,
        RightControl = 228,
        RightShift = 229,
        RightAlt = 230,
        Count
    };

    enum class MouseCode {
        MouseBtnLeft,
        MouseBtnRight,
        MouseBtnWheel
    };
}