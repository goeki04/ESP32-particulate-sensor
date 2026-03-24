#pragma once
#include "a_EventTypes.hpp"
#include <unordered_map>
#include <vector>
#include <functional>
namespace Andromeda {
	using EventHandle = std::function<void(const IEvent&)>;
	class EventManager {
	public:
		void AddEventListener(const EventType& event,EventHandle handle) {
			m_Events[event].push_back(std::move(handle));
		}

		void Dispatch(const EventType& type, const IEvent& event) {
			auto it = m_Events.find(type);
			if (it == m_Events.end()) {
				return;
			}
			for (auto& handle : it->second) {
				handle(event);
			}
		}
	private:
		std::unordered_map<EventType, std::vector<EventHandle>> m_Events;
	};
}