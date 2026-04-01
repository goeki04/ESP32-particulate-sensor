#include "subsystem_manager.h"
#include "a_ISubsystem.hpp"
#include "a_primitives.hpp"

namespace Andromeda {
	Uint64 SystemManager::lastCounter = SDL_GetPerformanceCounter();
	float SystemManager::s_deltaTime = 0.0f;
	SystemManager& SystemManager::getInstance() {
		static SystemManager instance;
		return instance;
	}

	void SystemManager::addSubsystem(ISubsystem* s) {
		m_Subsystems.emplace_back(s);
	}

	void SystemManager::startSubsystems() const {

		for (auto& v : m_Subsystems) {
			v->start();
		}
	}

	void SystemManager::updateEvent(SDL_Event* event) const {
		for (const auto& v : m_Subsystems) {
			v->updateEvent(event);
		}
	}

	void SystemManager::updateSubsystems() const {
		const u32 currentCounter = SDL_GetPerformanceCounter();
		s_deltaTime = static_cast<float>((currentCounter - lastCounter)) / static_cast<float>(SDL_GetPerformanceFrequency());
		lastCounter = currentCounter;
		for (auto& v : m_Subsystems) {
			v->update();
		}
	}

	void SystemManager::destroy() const {
		for (auto& v : m_Subsystems) {
			v->destroy();
		}
	}
}
