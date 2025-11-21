#pragma once
/// <summary>
/// Interface which declares essential functions for all kind of Subsystems
/// </summary>
class ISubsystem {
public:
	virtual void start() = 0;
	virtual void updateEvent(SDL_Event* event) { }
	virtual void update() = 0;
	virtual void destroy() = 0;
	virtual ~ISubsystem() = default;
};