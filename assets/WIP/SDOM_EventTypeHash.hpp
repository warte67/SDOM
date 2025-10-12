#ifndef SDOM_EVENT_TYPE_HASH_HPP
#define SDOM_EVENT_TYPE_HASH_HPP

#include <functional>
#include "SDOM_EventManager.hpp" // Include the full definition of EventType
#include "SDOM_EventType.hpp"

// Define a custom hash functor for EventType
struct EventTypeHash {
    size_t operator()(const SDOM::EventType& eventType) const {
        return std::hash<size_t>()(eventType.getId());
    }
};

#endif // SDOM_EVENT_TYPE_HASH_HPP
