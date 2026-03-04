#pragma once
/// <summary>
/// Interface which declares essential functions for all kind of Subsystems
/// </summary>
namespace Andromeda {
	class ISubsystem {
	public:
		virtual void start() {};
		virtual void updateEvent(SDL_Event* event) {}
		virtual void update() {}
		virtual void destroy() {}
		virtual ~ISubsystem() = default;
	};
}