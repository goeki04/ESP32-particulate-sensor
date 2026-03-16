#pragma once
#include <vector>
#include "a_ISubsystem.hpp"
#include <SDL3/SDL.h>
#include <stdexcept>
/// <summary>
/// Singleton that handles subsystem management
/// </summary>
union SDL_Event;
namespace Andromeda {
	class SystemManager {
	public:
		std::vector<ISubsystem*> m_Subsystems;
		static Uint64 lastCounter;
		static float s_deltaTime;
		static SystemManager& getInstance();
		void addSubsystem(ISubsystem* s);
		void startSubsystems();
		void updateSubsystems();
		void updateEvent(SDL_Event* event);
		void destroy();

		template<typename T> requires std::derived_from<T, ISubsystem>
		T* getSubsystem() {
			for (auto& i : m_Subsystems) {
				if (T* casted = dynamic_cast<T*>(i)) {
					return casted;
				}
			}
			throw std::runtime_error("Subsystem not found!");
		}

	private:
		SystemManager() {};
		~SystemManager() {
			m_Subsystems.clear();
		};

		SystemManager(const SystemManager&) = delete;
		SystemManager& operator=(const SystemManager&) = delete;
	};
}