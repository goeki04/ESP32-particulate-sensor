#pragma once
#include "a_ISubsystem.hpp"
#include <string_view>
#include <unordered_map>
#include "a_Keycodes.hpp"
namespace Andromeda {
	class InputSystem : public ISubsystem {
		//std::unordered_map<SDL_Event, Keycode> m_KeyMap;
		static constexpr std::string_view GetStaticName() { return "InputSystem"; }
		[[nodiscard]] const char* getSubsystemName() const override {
			return GetStaticName().data();
		}

		void updateEvent(SDL_Event* event) override;

		static Keycode sdlKeyToAndromeda(const SDL_Event& e);
		static MouseCode sdlMouseBtnToAndromeda(const SDL_Event& e);
	};
}