#pragma once

#include "a_Keycodes.hpp"
#include <variant>

namespace Andromeda {

    /**
     * @brief Defines all supported event types within the Andromeda Engine.
     */
    enum class EventType {
        OnKeyPressed, OnKeyRelease,
        OnMouseBtnPressed, OnMouseBtnReleased, OnMouseWheelScroll,
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
    struct KeyPressed : public IEvent {
        Keycode keycode; ///< The unique code of the pressed key.

        KeyPressed(Keycode _code) : keycode(_code) {}
        static constexpr EventType GetStaticType() { return EventType::OnKeyPressed; }
    };

    /**
     * @brief Triggered when a keyboard key is released.
     */
    struct KeyRelease : public IEvent {
        Keycode keycode; ///< The unique code of the released key.

        KeyRelease(Keycode _code) : keycode(_code) {}
        static constexpr EventType GetStaticType() { return EventType::OnKeyRelease; }
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
    struct MouseBtnPressed : public IEvent {
        MouseCode mousecode; ///< The specific mouse button (Left, Right, Middle, etc.).

        MouseBtnPressed(MouseCode _code) : mousecode(_code) {}
        static constexpr EventType GetStaticType() { return EventType::OnMouseBtnPressed; }
    };

    /**
     * @brief Triggered when a mouse button is released.
     */
    struct MouseBtnReleased : public IEvent {
        MouseCode mousecode; ///< The specific mouse button that was released.

        MouseBtnReleased(MouseCode _code) : mousecode(_code) {}
        static constexpr EventType GetStaticType() { return EventType::OnMouseBtnReleased; }
    };
}