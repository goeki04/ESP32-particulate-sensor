#pragma once

/**
 * @file window_manager.hpp
 * @brief Subsystem that owns the application's SDL window and OpenGL context.
 */

#include "a_ISubsystem.hpp"
#include <SDL3/SDL.h>
#include <string_view>

/**
 * @namespace Andromeda::Window
 * @brief Global window/GL-context state and the subsystem that manages its lifetime.
 */
namespace Andromeda::Window {
	inline SDL_Window* g_Window = nullptr;        ///< The application's main SDL window (global handle).
	inline int g_WindowWidth = 0, g_WindowHeight = 0; ///< Current window dimensions in pixels.
	inline SDL_GLContext m_GlContext = nullptr;   ///< The OpenGL rendering context bound to @c g_Window.

	/**
	 * @class WindowManager
	 * @brief Creates and tears down the SDL window and OpenGL context as an engine subsystem.
	 */
	class WindowManager : public ISubsystem {
	public:
		/** @brief Creates the SDL window, OpenGL context and sets the window icon. */
		void start() override;

		/** @brief Destroys the OpenGL context and SDL window. */
		void destroy() override;

		/**
		 * @brief Gets the static compile-time string identifier of the subsystem.
		 * @return A string_view containing "WindowManager".
		 */
		static constexpr std::string_view GetStaticName() { return "WindowManager"; }

		/**
		 * @brief Gets the runtime string identifier of the subsystem.
		 * @return A C-string containing the subsystem's name.
		 */
		const char* getSubsystemName() const override {
			return GetStaticName().data();
		}
	private:
		/**
		 * @brief Loads an image from disk into an SDL surface (e.g. for the window icon).
		 * @param path Filesystem path to the image.
		 * @return A newly created SDL_Surface, or nullptr on failure.
		 */
		static SDL_Surface* CreateSDLSurface(const char* path);
	};
}