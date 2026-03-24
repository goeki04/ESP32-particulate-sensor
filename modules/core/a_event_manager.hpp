#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <functional>
#include <concepts>
#include "a_EventTypes.hpp"

namespace Andromeda {

    /**
     * @brief A smart handle that uniquely identifies a registered listener.
     * * This structure bundles the EventType and the internal ID. It allows the
     * EventManager to locate and remove a listener efficiently without the
     * user needing to remember which type the listener was registered for.
     */
    struct EventListenerID {
        EventType type; ///< The category of the event this listener is attached to.
        size_t id;      ///< The unique numerical ID assigned by the manager.
    };

    /**
     * @class EventManager
     * @brief High-performance, type-safe event distribution system.
     * * The EventManager facilitates decoupled communication between engine components.
     * Features:
     * - **Type Safety**: Uses C++20 concepts to enforce correct event inheritance.
     * - **Performance**: Optimized dispatch loop using deferred removal to avoid
     * expensive heap allocations (vector copies) during event firing.
     * - **Reentrancy**: Safe to add or remove listeners even while a dispatch is in progress.
     */
    class EventManager {
    private:
        EventManager() = default;
        /**
         * @struct EventListener
         * @brief Internal representation of a subscriber.
         */
        struct EventListener {
            size_t id;                                   ///< Unique ID for this specific listener.
            std::function<void(const IEvent&)> callback; ///< The type-erased callback wrapper.
            bool pendingRemoval = false;                 ///< Flag set if removed during a dispatch.
        };

        /** @brief Maps event types to their respective list of subscribers. */
        std::unordered_map<EventType, std::vector<EventListener>> m_Events;

        /** @brief Monotonically increasing counter for listener IDs. */
        size_t m_NextID = 0;

        /** @brief Tracks the current recursion depth of Dispatch calls to ensure thread/reentrancy safety. */
        int m_DispatchLevel = 0;

    public:
        static EventManager& getInstance() {
            static EventManager instance;
            return instance;
        }
        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;
        /**
         * @brief Registers a listener for a specific event type deduced from T.
         * * @tparam T The concrete event structure (must derive from IEvent).
         * @tparam F The callable type (lambda, function pointer, etc.).
         * * @param handle The callback function. Expected signature: void(const T&).
         * @return EventListenerID A handle required to unregister the listener later.
         * * @note Requires T to implement 'static constexpr EventType GetStaticType()'.
         */
        template<typename T, typename F>
            requires std::derived_from<T, IEvent>
        EventListenerID AddEventListener(F&& handle) {
            static_assert(requires { T::GetStaticType(); },
                "Event struct must implement 'static constexpr EventType GetStaticType()'");

            size_t id = m_NextID++;
            EventType type = T::GetStaticType();

            auto wrapper = [handle = std::forward<F>(handle)](const IEvent& e) {
                handle(static_cast<const T&>(e));
                };

            m_Events[type].push_back({ id, std::move(wrapper), false });
            return { type, id };
        }

        /**
         * @brief Broadcasts an event to all interested listeners.
         * If listeners are removed during this call, they are marked as 'pending'
         * and cleaned up once the outermost dispatch scope is exited.
         * * @param type The type of event to trigger.
         * @param event The event data object.
         */
        void Dispatch(const EventType& type, const IEvent& event) {
            auto it = m_Events.find(type);
            if (it == m_Events.end()) return;

            m_DispatchLevel++;

            for (auto& listener : it->second) {
                if (!listener.pendingRemoval) {
                    listener.callback(event);
                }
            }

            m_DispatchLevel--; 

            // Perform cleanup only after the last nested dispatch finished
            if (m_DispatchLevel == 0) {
                ClearPendingListeners();
            }
        }

        /**
         * @brief Unregisters a listener using its handle.
         * * If called during a Dispatch, the listener is marked for removal.
         * If called outside, the listener is erased from memory immediately.
         * * @param handle The ID object returned by AddEventListener.
         */
        void RemoveEventListener(EventListenerID handle) {
            auto it = m_Events.find(handle.type);
            if (it == m_Events.end()) return;

            if (m_DispatchLevel > 0) {
                // We are currently dispatching; mark for later to keep iterators valid
                for (auto& listener : it->second) {
                    if (listener.id == handle.id) {
                        listener.pendingRemoval = true;
                        break;
                    }
                }
            }
            else {
                // Safe to remove immediately
                std::erase_if(it->second, [id = handle.id](const EventListener& l) {
                    return l.id == id;
                    });
            }
        }

    private:
        /**
         * @brief Performs the actual memory cleanup for listeners marked as pendingRemoval.
         */
        void ClearPendingListeners() {
            for (auto& [type, listeners] : m_Events) {
                std::erase_if(listeners, [](const EventListener& l) {
                    return l.pendingRemoval;
                    });
            }
        }
    };
}