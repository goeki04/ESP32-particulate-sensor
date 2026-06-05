#pragma once
#include "a_ISubsystem.hpp"
#include <string_view>
#include <array>
#include "a_Keycodes.hpp"
namespace Andromeda {
	class InputSystem : public ISubsystem {
	private:
		inline static std::array<bool, static_cast<size_t>(Keycode::Count)> s_keyStates;
		static constexpr std::string_view GetStaticName() { return "InputSystem"; }
		[[nodiscard]] const char* getSubsystemName() const override {
			return GetStaticName().data();
		}

		void updateEvent(SDL_Event* event) override;

		static Keycode sdlKeyToAndromeda(const SDL_Event& e);
		static MouseCode sdlMouseBtnToAndromeda(const SDL_Event& e);

	public:
		static bool isKeyHeld(Keycode key) { return s_keyStates[static_cast<size_t>(key)]; }
	};

}