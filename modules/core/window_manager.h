#pragma once
#include "a_ISubsystem.hpp"
#include <SDL3/SDL.h>
#include <string_view>
namespace Andromeda::Window {
	inline SDL_Window* g_Window = nullptr;
	inline int g_WindowWidth = 0, g_WindowHeight = 0;
	inline SDL_GLContext m_GlContext = nullptr;
	class WindowManager : public ISubsystem {
	public:
		void start() override;
		void destroy() override;
		static constexpr std::string_view GetStaticName() { return "WindowManager"; }
		const char* getSubsystemName() const override {
			return GetStaticName().data();
		}
	private:
		SDL_Surface* CreateSDLSurface(const char* path);
	};
}