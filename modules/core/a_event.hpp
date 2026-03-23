#pragma once
#include "a_ISubsystem.hpp"
#include "a_EventTypes.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
namespace Andromeda {
	class EventManager : public ISubsystem {
		std::unordered_map<Event::Type,std::vector<std::function<void(const Event&)>>> m_EventQueue;
		const char* getSubsystemName() const override;
		void start() override;
		void update() override;
		void addEvent();
	};
}