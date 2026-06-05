#pragma once

/**
 * @file a_ISubsystem.hpp
 * @brief Base interface for all Andromeda engine modules.
 */

union SDL_Event;

namespace Andromeda {

    /**
     * @class ISubsystem
     * @brief Interface declaring essential lifecycle functions for all engine subsystems.
     * * Subsystems are managed by the SystemManager and follow a specific lifecycle:
     * start -> update -> updateEvent -> destroy.
     */
    class ISubsystem {
    public:
        /**
         * @brief Initializes the subsystem.
         * Called once when the subsystem is registered or the engine starts.
         */
        virtual void start() {}

        /**
         * @brief Processes OS-level events (e.g., Input, Window-Resizing).
         * @param event Pointer to the current SDL_Event.
         */
        virtual void updateEvent(SDL_Event* event) {}

        /**
         * @brief Core update loop for the subsystem.
         * Called once per frame to handle logic or rendering.
         */
        virtual void update() {}

        /**
         * @brief Clean up resources before the subsystem is removed or the engine shuts down.
         */
        virtual void destroy() {}

        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         */
        virtual ~ISubsystem() = default;

        /**
         * @brief Returns the internal identifier of the subsystem.
         * * Used for exception handling, logging, and debugging to identify
         * which specific module is causing an issue.
         * * @return A pointer to a static string representing the subsystem's name.
         */
        virtual const char* getSubsystemName() const = 0;
    };
}