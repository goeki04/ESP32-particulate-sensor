#pragma once
#include "a_Keycodes.hpp"
#include <variant>

namespace Andromeda {
	struct IEvent {
		virtual ~IEvent() = default;
		EventType type;
	};
	struct MouseEvent : public IEvent{
		float x, y;
		MouseEvent(float _x, float _y) : x(_x), y(_y){}
	};
	enum class EventType {
		OnKeyPressed, OnKeyRelease,
		OnMouseBtnPressed, OnMouseWheelScroll
	};
}



