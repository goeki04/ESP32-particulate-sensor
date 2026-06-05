#include "input_manager.hpp"
#include <SDL3/SDL.h>
#include "a_event_manager.hpp"
namespace Andromeda {
	void InputSystem::updateEvent(SDL_Event* event)
	{
		switch (event->type) {
			case SDL_EVENT_KEY_DOWN: {
				Keycode keyDown = sdlKeyToAndromeda(*event);
				s_keyStates[static_cast<size_t>(keyDown)] = true;
				if (event->key.repeat) return;
				EventManager::getInstance().Dispatch(EventType::OnKeyDown, KeyDown(keyDown));
				break;
			}
			case SDL_EVENT_KEY_UP: {
				Keycode keyUp = sdlKeyToAndromeda(*event);
				s_keyStates[static_cast<size_t>(keyUp)] = false;
				EventManager::getInstance().Dispatch(EventType::OnKeyUp, KeyUp(keyUp));
				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_DOWN: {
				EventManager::getInstance().Dispatch(EventType::OnMouseBtnDown, MouseBtnDown(sdlMouseBtnToAndromeda(*event)));
				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_UP: {
				EventManager::getInstance().Dispatch(EventType::OnMouseBtnUp, MouseBtnUp(sdlMouseBtnToAndromeda(*event)));
				break;
			}
			case SDL_EVENT_MOUSE_MOTION: {
				EventManager::getInstance().Dispatch(
					EventType::OnMouseMoved,
					MouseMoved(event->motion.x, event->motion.y, event->motion.xrel, event->motion.yrel)
				);
				break;
			}
			default: ;
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

			auto andromedaCode = static_cast<Keycode>(e.key.scancode);

			SDL_Keycode virtualKey = e.key.key;

			// OVERRIDE: If it's a letter (a-z), map it mathematically to our enum.
			// ASCII 'a' is 97. Keycode::A is 4.
			if (virtualKey >= 'a' && virtualKey <= 'z') {
				andromedaCode = static_cast<Keycode>((virtualKey - 'a') + static_cast<u8>(Keycode::A));
			}
			// Fallback just in case SDL sends uppercase via caps lock
			else if (virtualKey >= 'A' && virtualKey <= 'Z') {
				andromedaCode = static_cast<Keycode>((virtualKey - 'A') + static_cast<u8>(Keycode::A));
			}

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
			default: mouseCode = MouseCode::Unknown;
			}
		}
		return mouseCode;
	}
}