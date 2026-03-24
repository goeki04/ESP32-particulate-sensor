#pragma once

#include "a_EventTypes.hpp"
#include <unordered_map>
#include <vector>
#include <functional>
namespace Andromeda {

    /**
     * @class EventManager
     * @brief Centralized system for registering and dispatching specialized events.
     * * This manager uses a type-safe wrapper pattern to allow users to subscribe
     * to specific event types using their concrete structs, while storing them
     * internally in a unified map.
     */
    class EventManager {
    public:
        /**
         * @brief Subscribes a callback to a specific event type.
         * * @tparam T The concrete event struct (must derive from IEvent).
         * @tparam F The callable type (lambda, function pointer, etc.).
         * * @param type The EventType enum key to listen for.
         * @param handle The callback function. Expected signature: void(const T&).
         * * @note Uses C++20 constraints to ensure T is a valid event type.
         * The callback is wrapped in a lambda to handle internal type erasure.
         */
        template<typename T, typename F>
            requires std::derived_from<T, IEvent>
        void AddEventListener(const EventType& type, F&& handle) {
            // Capture the specific handler and wrap it to accept the base IEvent interface
            auto wrapper = [handle = std::forward<F>(handle)](const IEvent& e) {
                // Perform a static_cast since the requirement guarantees T derives from IEvent
                handle(static_cast<const T&>(e));
                };

            m_Events[type].push_back(std::move(wrapper));
        }

        /**
         * @brief Dispatches an event to all registered listeners.
         * * @param type The EventType enum key identifying which listeners to trigger.
         * @param event The actual event data struct.
         * * @details If no listeners are registered for the given type, the function
         * returns silently. Otherwise, it iterates through all handles and executes them.
         */
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
        /**
         * @brief Internal storage mapping EventTypes to a list of type-erased callbacks.
         */
        std::unordered_map<EventType, std::vector<std::function<void(const IEvent&)>>> m_Events;
    };
}