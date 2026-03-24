#pragma once
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <functional>
#include <concepts>
#include "a_EventTypes.hpp"

namespace Andromeda {

    /**
     * @brief Unique identifier for a registered event listener.
     * Used to specifically remove a listener at a later time.
     */
    using eventHandleID = size_t;

    /**
     * @class EventManager
     * @brief Centralized system for registering and dispatching specialized events.
     * * The EventManager enables a decoupled design where listeners can respond to
     * specific events (e.g., Input, Collision) without knowing the source.
     * It utilizes type erasure and C++20 concepts for maximum type safety and performance.
     */
    class EventManager {
    private:
        /**
         * @struct EventListener
         * @brief Internal representation of an active listener.
         * Pairs a unique ID with the type-erased callback function.
         */
        struct EventListener {
            eventHandleID id;
            std::function<void(const IEvent&)> callback;
        };

        /** @brief Internal storage mapping EventTypes to a list of registered listeners. */
        std::unordered_map<EventType, std::vector<EventListener>> m_Events;

        /** @brief Counter for assigning consecutive, unique listener IDs. */
        eventHandleID m_NextID = 0;

    public:
        /**
         * @brief Subscribes a callback to a specific event type.
         * * @tparam T The concrete event struct (must derive from IEvent).
         * @tparam F The callable type (lambda, function pointer, etc.).
         * * @param type The EventType enum key to listen for.
         * @param handle The callback function. Expected signature: void(const T&).
         * * @return eventHandleID The ID of the listener (required for RemoveEventListener).
         */
        template<typename T, typename F>
            requires std::derived_from<T, IEvent>
        eventHandleID AddEventListener(F&& handle) {
            static_assert(requires { T::GetStaticType(); },
                "Event struct must implement static constexpr EventType GetStaticType()");
            eventHandleID id = m_NextID++;

            // Create a wrapper that casts the base IEvent to the target type T.
            auto wrapper = [handle = std::forward<F>(handle)](const IEvent& e) {
                handle(static_cast<const T&>(e));
                };

            m_Events[T::GetStaticType()].push_back({id, std::move(wrapper)});
            return id;
        }

        /**
         * @brief Dispatches an event to all registered listeners of the given type.
         * * @param type The type of the event to trigger.
         * @param event The actual event data object.
         * * @warning Internally creates a temporary copy of the listener list.
         * This prevents crashes (iterator invalidation) if a listener is removed
         * while the dispatch loop is still running.
         */
        void Dispatch(const EventType& type, const IEvent& event) {
            auto it = m_Events.find(type);
            if (it == m_Events.end()) {
                return;
            }

            // Create a copy for reentrancy safety.
            auto listenerCopy = it->second;
            for (auto& listener : listenerCopy) {
                listener.callback(event);
            }
        }

        /**
         * @brief Removes a listener by its IDa and EventType.
         * * @param type The EventType under which the listener was registered.
         * @param id The ID returned by AddEventListener.
         * * @details Uses C++20 std::erase_if for efficient removal from the vector.
         */
        void RemoveEventListener(const EventType& type, eventHandleID id) {
            auto it = m_Events.find(type);
            if (it == m_Events.end()) {
                return;
            }

            std::erase_if(it->second, [id](const EventListener& l) {
                return l.id == id;
                });
        }
    };
}