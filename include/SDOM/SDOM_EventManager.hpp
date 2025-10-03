/***  SDOM_EventManager.hpp  ****************************
 *  ___ ___   ___  __  __   ___             _   __  __                              _              
 * / __|   \ / _ \|  \/  | | __|_ _____ _ _| |_|  \/  |__ _ _ _  __ _ __ _ ___ _ _ | |_  _ __ _ __ 
 * \__ \ |) | (_) | |\/| | | _|\ V / -_) ' \  _| |\/| / _` | ' \/ _` / _` / -_) '_|| ' \| '_ \ '_ \
 * |___/___/ \___/|_|  |_|_|___|\_/\___|_||_\__|_|  |_\__,_|_||_\__,_\__, \___|_|(_)_||_| .__/ .__/
 *                      |___|                                        |___/              |_|  |_|   
 *  
 * The SDOM_EventManager class is responsible for managing the event queue and dispatching 
 * events throughout the SDOM display object hierarchy. It provides methods for queuing, 
 * translating, and dispatching both SDOM and SDL events, ensuring that user input and 
 * system events are delivered efficiently and in the correct order. The EventManager 
 * supports event propagation to individual nodes, all nodes on a stage, or all event 
 * listeners, and includes utility functions for hit-testing and mouse interaction. By 
 * centralizing event management, the EventManager enables robust, flexible, and scalable 
 * event-driven programming within SDOM-based applications.
 * 
 *
 *   This software is provided 'as-is', without any express or implied
 *   warranty.  In no event will the authors be held liable for any damages
 *   arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *   including commercial applications, and to alter it and redistribute it
 *   freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not
 *       claim that you wrote the original software. If you use this software
 *       in a product, an acknowledgment in the product documentation would be
 *       appreciated but is not required.
 *   2. Altered source versions must be plainly marked as such, and must not be
 *       misrepresented as being the original software.
 *   3. This notice may not be removed or altered from any source distribution.
 *
 * Released under the ZLIB License.
 * Original Author: Jay Faries (warte67)
 *
 ******************/
#pragma once
#ifndef __SDOM_EVENT_MANAGER_HPP__
#define __SDOM_EVENT_MANAGER_HPP__

#include <SDOM/SDOM.hpp>

// #include <queue>`
// #include <memory>
// #include "SDOM/SDOM_Event.hpp"
// #include "SDOM/SDOM_IDisplayObject.hpp"

namespace SDOM 
{
    class IDisplayObject;
    class Event;

    class EventManager 
    {
    public:
        EventManager() = default;
        ~EventManager() = default;

        // Add an event to the queue
        void addEvent(std::unique_ptr<Event> event);

        // Queue an SDL_Event as an SDOM::Event
        void Queue_SDL_Event(SDL_Event& sdlEvent);

        // Dispatch all queued events
        void DispatchQueuedEvents();

        // Dispatch a single event to the target node
        void dispatchEvent(std::unique_ptr<Event> event, DisplayObject rootNode);
        // Dispatch an event to all nodes on the same stage
        void dispatchEventToAllNodesOnStage(std::unique_ptr<Event> event);

        // Dispatch an event to all event listeners on the same stage
        void dispatchEventToAllEventListenersOnStage(std::unique_ptr<Event> event);

        // Returns true if there are any listeners (or node-default handlers)
        // registered that would receive events of the given type on the current stage.
        bool hasListeners(const EventType& type) const;

        // Utility methods
        bool isMouseWithinBounds(IDisplayObject& target) const;
        DisplayObject findTopObjectUnderMouse(DisplayObject rootNode, DisplayObject excludeNode = DisplayObject()) const;

        int getEventQueueSize() const { return static_cast<int>(eventQueue.size()); }

        // Event popEvent() { if (!eventQueue.empty()) return std::move(*eventQueue.front()); return Event(); }

    private:
        std::queue<std::unique_ptr<Event>> eventQueue; // Queue for storing events
    };

} // namespace SDOM

#endif // __EVENT_MANAGER_HPP__