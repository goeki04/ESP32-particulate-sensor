#pragma once
#include "a_KeyCodes.hpp"
namespace Andromeda {
	enum class KeyEvents {
		KeyPressed,
		KeyRelease,
		KeyHeld
	};

    struct KeyData {
        Keycode Code;
        KeyEvents Action;
    };

    struct MouseData {
        u16 x, y;
        u8 button;
    };

    struct Event {
        enum class Type : u8 {
            None = 0,
            Keyboard,
            Mouse,
            Window,
            Count
        };

        Type type;
        bool handled = false;
        union {
            KeyData key;
            MouseData mouse;
        };
    };
}