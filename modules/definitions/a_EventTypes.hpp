#pragma once
#include "a_Keycodes.hpp"
#include <variant>

namespace Andromeda {
	enum class EventType {
		OnKeyPressed, OnKeyRelease,
		OnMouseBtnPressed, OnMouseBtnReleased,OnMouseWheelScroll,
		OnMouseMoved
	};
	struct IEvent {
		virtual ~IEvent() = default;
	};

	struct KeyPressed : public IEvent {
		Keycode code;
		static constexpr EventType GetStaticType() { return EventType::OnKeyPressed; }
		KeyPressed(Keycode _code) : code(_code) {};
	};

	struct KeyRelease : public IEvent {
		Keycode code;
		static constexpr EventType GetStaticType() { return EventType::OnKeyRelease; }
		KeyRelease(Keycode _code) : code(_code) {};
	};

	struct MouseMoved : public IEvent{
		float x, y;
		MouseMoved(float _x, float _y) : x(_x), y(_y){}

		static constexpr EventType GetStaticType() { return EventType::OnMouseMoved; }
	};

	struct MouseWheelScroll : public IEvent {
		float mouseWheelX;
		float mouseWheelY;
		static constexpr EventType GetStaticType() { return EventType::OnMouseWheelScroll; }
		MouseWheelScroll(float _mouseWheelX, float _mouseWheelY) : mouseWheelX(_mouseWheelX), mouseWheelY(_mouseWheelY) {};
	};

	struct MouseBtnPressed : public IEvent {
		MouseCode code;
		static constexpr EventType GetStaticType() { return EventType::OnMouseBtnPressed; }
		MouseBtnPressed(MouseCode _code) : code(_code) {};
	};

	struct MouseBtnReleased : public IEvent {
		MouseCode code;
		static constexpr EventType GetStaticType() { return EventType::OnMouseBtnReleased; }
		MouseBtnReleased(MouseCode _code) : code(_code) {};
	};
}



