#include "input_manager.hpp"
#include <SDL3/SDL.h>
#include "a_event_manager.hpp"
namespace Andromeda {

	void InputSystem::updateEvent(SDL_Event* event)
	{
		switch (event->type) {
		case SDL_EVENT_KEY_DOWN:
			if (event->key.repeat) return;
			EventManager::getInstance().Dispatch(EventType::OnKeyDown, KeyDown(sdlKeyToAndromeda(*event)));
			break;
		case SDL_EVENT_KEY_UP:
			EventManager::getInstance().Dispatch(EventType::OnKeyUp, KeyUp(sdlKeyToAndromeda(*event)));
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			EventManager::getInstance().Dispatch(EventType::OnMouseBtnDown, MouseBtnDown(sdlMouseBtnToAndromeda(*event)));
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			EventManager::getInstance().Dispatch(EventType::OnMouseBtnUp, MouseBtnUp(sdlMouseBtnToAndromeda(*event)));
			break;
		}
	}
		/**
	 * @brief Translates a raw SDL event into an Andromeda-specific Keycode.
	 * * This function utilizes a "fast-path" by casting the SDL scancode directly
	 * to an Andromeda Keycode. This is possible because the Andromeda Keycode
	 * enum values are intentionally aligned with the HID (Human Interface Device)
	 * usage standard used by SDL3.
	 * * @param e The raw SDL_Event received from the OS/Windowing system.
	 * @return Keycode The corresponding Andromeda Keycode if valid and mapped;
	 * otherwise, Keycode::Unknown.
	 */
    Keycode InputSystem::sdlKeyToAndromeda(const SDL_Event& e) {
        if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
            Keycode andromedaCode = static_cast<Keycode>(e.key.scancode);
            if (andromedaCode < Keycode::Count) {
				return andromedaCode;
            }
        }
		return Keycode::Unknown;
    }
	/**
	* @brief Translates a raw SDL event into an Andromeda-specific MouseCode.
	* * @param e The raw SDL_Event received from the OS/Windowing system.
	* @return Keycode The corresponding Andromeda MouseCode if valid and mapped;
	* otherwise, MouseCode::Unknown.
	*/
	MouseCode Andromeda::InputSystem::sdlMouseBtnToAndromeda(const SDL_Event& e)
	{
		MouseCode mouseCode = MouseCode::Unknown;
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
			switch (e.button.button) {
			case SDL_BUTTON_LEFT: mouseCode = MouseCode::MouseBtnLeft; break;
			case SDL_BUTTON_RIGHT: mouseCode = MouseCode::MouseBtnRight; break;
			case SDL_BUTTON_MIDDLE: mouseCode = MouseCode::MouseBtnWheel; break;
			}
		}
		return mouseCode;
	}
}