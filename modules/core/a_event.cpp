#include "a_event.hpp"
#include <unordered_map>
#include <string_view>
namespace Andromeda {
	static constexpr std::string_view GetStaticName() { return "Renderer"; }
	const char* EventManager::getSubsystemName() const
	{
		return GetStaticName().data();
	}
	void EventManager::start()
	{
		m_EventQueue.reserve((u32)Event::Type::Count);
	}
	void EventManager::update()
	{
	}
	void EventManager::addEvent()
	{
	}
}