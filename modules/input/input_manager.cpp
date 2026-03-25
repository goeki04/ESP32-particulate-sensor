#include "input_manager.hpp"
#include <SDL3/SDL.h>
#include "a_event_manager.hpp"
#include <iostream>
#include <string>
namespace Andromeda {

	void InputSystem::updateEvent(SDL_Event* event)
	{
		if (event->type == SDL_EVENT_KEY_DOWN) {
			Keycode translatedCode = translateSDLToAndromeda(*event);
			KeyPressed keyPressedEvent(translatedCode);
			EventManager::getInstance().Dispatch(EventType::OnKeyPressed, keyPressedEvent);
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
	 * * @note This method is highly efficient as it avoids a heap-based lookup
	 * (unordered_map) in favor of a simple bounds-checked static_cast.
	 */
    Keycode InputSystem::translateSDLToAndromeda(const SDL_Event& e) {
        if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
            Keycode andromedaCode = static_cast<Keycode>(e.key.scancode);
            if (andromedaCode < Keycode::Count) {
				return andromedaCode;
            }
			else {
				return Keycode::Unknown;
			}
        }
    }
}