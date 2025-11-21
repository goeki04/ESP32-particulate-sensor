#pragma once
#include <vector>
#include "ISubsystem.h"
/// <summary>
/// Singleton that handles subsystem management
/// </summary>
union SDL_Event;
class SystemManager {
public:
	std::vector<ISubsystem*> m_subsystems;
	static SystemManager& getInstance();
	void addSubsystem(ISubsystem* s);
	/*Get the Subsystem by Name*/
	template<typename T> requires std::derived_from<T, ISubsystem>
	T* getSubsystem() {	
		for(auto& i : m_subsystems){
			if (T* casted = dynamic_cast<T*>(i)) {
				return casted;
			}
		}
		return nullptr;
	}
	void startSubsystems();
	void updateSubsystems();
	void updateEvent(SDL_Event* event);
	void destroy();
private:
	SystemManager() {};
	~SystemManager() {
		m_subsystems.clear();
	};

	SystemManager(const SystemManager&) = delete;
	SystemManager& operator=(const SystemManager&) = delete;
};
