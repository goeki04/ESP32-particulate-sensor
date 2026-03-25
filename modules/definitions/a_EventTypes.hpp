#pragma once

#include "a_Keycodes.hpp"
#include <variant>

namespace Andromeda {

    /**
     * @brief Defines all supported event types within the Andromeda Engine.
     */
    enum class EventType {
        OnKeyDown, OnKeyUp,
        OnMouseBtnDown, OnMouseBtnUp, OnMouseWheelScroll,
        OnMouseMoved
    };

    /**
     * @brief Base interface for all events.
     * Includes a virtual destructor to ensure proper cleanup of derived event types.
     */
    struct IEvent {
        virtual ~IEvent() = default;
    };

    /**
     * @brief Triggered when a keyboard key is pressed.
     */
    struct KeyDown : public IEvent {
        Keycode keycode; ///< The unique code of the pressed key.

        KeyDown(Keycode _code) : keycode(_code) {}
        static constexpr EventType GetStaticType() { return EventType::OnKeyDown; }
    };

    /**
     * @brief Triggered when a keyboard key is released.
     */
    struct KeyUp : public IEvent {
        Keycode keycode; ///< The unique code of the released key.

        KeyUp(Keycode _code) : keycode(_code) {}
        static constexpr EventType GetStaticType() { return EventType::OnKeyUp; }
    };

    /**
     * @brief Triggered when the mouse is moved across the window.
     */
    struct MouseMoved : public IEvent {
        float x, y; ///< The new absolute window coordinates of the mouse.

        MouseMoved(float _x, float _y) : x(_x), y(_y) {}
        static constexpr EventType GetStaticType() { return EventType::OnMouseMoved; }
    };

    /**
     * @brief Triggered by the mouse wheel or touchpad scrolling.
     */
    struct MouseWheelScroll : public IEvent {
        float mouseWheelX; ///< Horizontal scroll delta.
        float mouseWheelY; ///< Vertical scroll delta (common scroll wheel).

        MouseWheelScroll(float _x, float _y) : mouseWheelX(_x), mouseWheelY(_y) {}
        static constexpr EventType GetStaticType() { return EventType::OnMouseWheelScroll; }
    };

    /**
     * @brief Triggered when a mouse button is pressed.
     */
    struct MouseBtnDown : public IEvent {
        MouseCode mousecode; ///< The specific mouse button (Left, Right, Middle, etc.).

        MouseBtnDown(MouseCode _code) : mousecode(_code) {}
        static constexpr EventType GetStaticType() { return EventType::OnMouseBtnDown; }
    };

    /**
     * @brief Triggered when a mouse button is released.
     */
    struct MouseBtnUp : public IEvent {
        MouseCode mousecode; ///< The specific mouse button that was released.

        MouseBtnUp(MouseCode _code) : mousecode(_code) {}
        static constexpr EventType GetStaticType() { return EventType::OnMouseBtnUp; }
    };
}