#pragma once
#include "a_Keycodes.hpp"
#include <variant>

namespace Andromeda {
	enum class EventType {
		OnKeyPressed, OnKeyRelease,
		OnMouseBtnPressed, OnMouseWheelScroll
	};
	struct IEvent {
		virtual ~IEvent() = default;
		EventType type;
	};
	struct MouseEvent : public IEvent{
		float x, y;
		MouseEvent(float _x, float _y) : x(_x), y(_y){}
	};
}



