#include "pch.h"
#include "SubsystemManager.h"
Uint64 SystemManager::lastCounter = SDL_GetPerformanceCounter();
float SystemManager::s_deltaTime = 0.0f;
SystemManager& SystemManager::getInstance() {
	static SystemManager instance;
	return instance;
}

void SystemManager::addSubsystem(ISubsystem* system) {
	m_Subsystems.emplace_back(system);
}

void SystemManager::startSubsystems() {
	for (auto& v : m_Subsystems) {
		v->start();
	}
}

void SystemManager::updateEvent(SDL_Event* event) {
	for (auto& v : m_Subsystems) {
		v->updateEvent(event);
	}
}

void SystemManager::updateSubsystems() {
	Uint64 currentCounter = SDL_GetPerformanceCounter();
	s_deltaTime = (float)(currentCounter - lastCounter) / SDL_GetPerformanceFrequency();
	lastCounter = currentCounter;
	for (auto& v : m_Subsystems) {
		v->update();
	}
}

void SystemManager::destroy() {
	for (auto& v : m_Subsystems) {
		v->destroy();
	}
}