#pragma once

#include <vector>
#include <stdexcept>
#include <SDL3/SDL.h>
#include "a_ISubsystem.hpp"

union SDL_Event;

namespace Andromeda {

    /**
     * @brief Singleton class that manages the lifecycle and communication of all engine subsystems.
     * * The SystemManager acts as a central hub for registering, updating, and retrieving
     * various engine modules (e.g., Renderer, ResourceManager, SceneManager).
     */
    class SystemManager {
    public:
        /// Collection of all registered subsystems
        std::vector<ISubsystem*> m_Subsystems;

        /// Timing variables for frame-independent calculations
        static Uint64 lastCounter;
        static float s_deltaTime;

        /**
         * @brief Returns the global instance of the SystemManager.
         * @return Reference to the singleton instance.
         */
        static SystemManager& getInstance();

        /**
         * @brief Adds a new subsystem to the manager.
         * @param s Pointer to the subsystem instance.
         */
        void addSubsystem(ISubsystem* s);

        /**
         * @brief Initializes all registered subsystems. Should be called once at engine start.
         */
        void startSubsystems() const;

        /**
         * @brief Updates all registered subsystems. Should be called every frame.
         */
        void updateSubsystems() const;

        /**
         * @brief Passes SDL events to all subsystems that require event handling.
         * @param event Pointer to the current SDL_Event.
         */
        void updateEvent(SDL_Event* event) const;

        /**
         * @brief Cleans up and shuts down all subsystems.
         */
        void destroy() const;

        /**
         * @brief Retrieves a specific subsystem by its type.
         * * Uses C++20 concepts to ensure T is derived from ISubsystem.
         * Performs a dynamic_cast to find the correct module.
         * * @tparam T The class type of the subsystem to retrieve.
         * @return Pointer to the requested subsystem.
         * @throws std::runtime_error If the requested subsystem is not registered.
         */
        template<typename T> requires std::derived_from<T, ISubsystem>
        T* getSubsystem() {
            for (auto& i : m_Subsystems) {
                if (T* casted = dynamic_cast<T*>(i)) {
                    return casted;
                }
            }

            std::string errorMsg = "Subsystem not found: ";
            errorMsg += T::GetStaticName(); // Expects a static GetStaticName() method in T
            throw std::runtime_error(errorMsg);
        }
        /// Deleted copy constructor to prevent multiple instances
        SystemManager(const SystemManager&) = delete;

        /// Deleted assignment operator to prevent multiple instances
        SystemManager& operator=(const SystemManager&) = delete;
    private:
        /// Private constructor for singleton pattern
        SystemManager() = default;

        /// Destructor clearing the subsystem list
        ~SystemManager() {
            m_Subsystems.clear();
        };
    };
}