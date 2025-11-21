#include "pch.h"
#include "SubsystemManager.h"
SystemManager& SystemManager::getInstance() {
	static SystemManager instance;
	return instance;
}

void SystemManager::addSubsystem(ISubsystem* system) {
	m_subsystems.emplace_back(system);
}

void SystemManager::startSubsystems() {
	for (auto& v : m_subsystems) {
		v->start();
	}
}

void SystemManager::destroy() {
	for (auto& v : m_subsystems) {
		v->destroy();
	}
}

void SystemManager::updateSubsystems() {
	for (auto& v : m_subsystems) {
		v->update();
	}
}