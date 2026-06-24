#pragma once

/**
 * @file input_manager.hpp
 * @brief Subsystem that tracks keyboard state and translates SDL input into engine events.
 */

#include "a_ISubsystem.hpp"
#include <string_view>
#include <array>
#include "a_Keycodes.hpp"
namespace Andromeda {
	/**
	 * @class InputSystem
	 * @brief Maintains the current keyboard state and dispatches input events from raw SDL events.
	 *
	 * @details On each SDL event it updates a held-key bitmap and (for key/mouse button changes)
	 *          translates the platform code into the engine's @c Keycode / @c MouseCode and fires
	 *          the corresponding event via the @c EventManager. Polled state is exposed through the
	 *          static @c isKeyHeld() so any system can query keys without subscribing to events.
	 */
	class InputSystem : public ISubsystem {
	private:
		/** @brief Held/released state for every key, indexed by @c Keycode value. */
		inline static std::array<bool, static_cast<size_t>(Keycode::Count)> s_keyStates;

		/**
		 * @brief Gets the static compile-time string identifier of the subsystem.
		 * @return A string_view containing "InputSystem".
		 */
		static constexpr std::string_view GetStaticName() { return "InputSystem"; }

		/**
		 * @brief Gets the runtime string identifier of the subsystem.
		 * @return A C-string containing the subsystem's name.
		 */
		[[nodiscard]] const char* getSubsystemName() const override {
			return GetStaticName().data();
		}

		/**
		 * @brief Processes a raw SDL event: updates key state and dispatches input events.
		 * @param event Pointer to the SDL event to handle.
		 */
		void updateEvent(SDL_Event* event) override;

		/**
		 * @brief Translates an SDL keyboard event into the engine's @c Keycode.
		 * @param e The SDL event carrying the key.
		 * @return The mapped @c Keycode (@c Keycode::Unknown if unmapped).
		 */
		static Keycode sdlKeyToAndromeda(const SDL_Event& e);

		/**
		 * @brief Translates an SDL mouse-button event into the engine's @c MouseCode.
		 * @param e The SDL event carrying the mouse button.
		 * @return The mapped @c MouseCode (@c MouseCode::Unknown if unmapped).
		 */
		static MouseCode sdlMouseBtnToAndromeda(const SDL_Event& e);

	public:
		/**
		 * @brief Queries whether a key is currently held down.
		 * @param key The key to test.
		 * @return true if the key is currently pressed, false otherwise.
		 */
		static bool isKeyHeld(Keycode key) { return s_keyStates[static_cast<size_t>(key)]; }
	};

}