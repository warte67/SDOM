/***  SDOM_EventManager.cpp  ****************************
 *   ___ ___   ___  __  __   ___             _   __  __                                           
 *  / __|   \ / _ \|  \/  | | __|_ _____ _ _| |_|  \/  |__ _ _ _  __ _ __ _ ___ _ _  __ _ __ _ __ 
 *  \__ \ |) | (_) | |\/| | | _|\ V / -_) ' \  _| |\/| / _` | ' \/ _` / _` / -_) '_|/ _| '_ \ '_ \
 *  |___/___/ \___/|_|  |_|_|___|\_/\___|_||_\__|_|  |_\__,_|_||_\__,_\__, \___|_|(_)__| .__/ .__/
 *                       |___|                                        |___/            |_|  |_|     
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

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>

#include <SDOM/SDOM_DisplayHandle.hpp>

namespace SDOM 
{
    // Static variables to track dragging state and the dragged object
    // static bool isDragging = false;
    // static DisplayHandle draggedObject = nullptr;


    void EventManager::addEvent(std::unique_ptr<Event> event) 
    {
        // Apply metering/coalescing policy per EventType
        const EventType type = event->getType();
        const bool isCritical = type.isCritical();
        const bool canMeter = type.isMeterEnabled();

// std::cout << "[addEvent] type=" << event->getTypeName()
//           << " isCritical=" << isCritical
//           << " queueSize(before)=" << eventQueue.size()
//           << std::endl;

        if (isCritical)
        {

            // Flush metered events to preserve ordering, then enqueue immediately
            flushCoalesced_();
            eventQueue.push(std::move(event));
            return;
        }

        if (!canMeter)
        {
            eventQueue.push(std::move(event));
            return;
        }

        // Build coalesce key
        auto build_key = [&](const Event& ev) -> std::string {
            if (type.getCoalesceKey() == EventType::CoalesceKey::ByTarget)
            {
                std::string tgt = ev.getTarget().isValid() ? ev.getTarget().getName() : std::string("<null>");
                return ev.getTypeName() + std::string("|") + tgt;
            }
            return ev.getTypeName();
        };

        const std::string key = build_key(*event);
        Uint32 now_ms = SDL_GetTicks();
        auto& entry = coalesce_map_[key];
        if (!entry.ev)
        {
            entry.ev = std::move(event);
            entry.first_ms = entry.last_ms = now_ms;
        }
        else
        {
            // Merge according to strategy
            switch (type.getCoalesceStrategy())
            {
                case EventType::CoalesceStrategy::Last:
                    entry.ev = std::move(event);
                    entry.last_ms = now_ms;
                    break;
                case EventType::CoalesceStrategy::Sum:
                {
                    // Sum deltas for wheel; keep last position
                    float wx = entry.ev->getWheelX() + event->getWheelX();
                    float wy = entry.ev->getWheelY() + event->getWheelY();
                    entry.ev->setWheelX(wx);
                    entry.ev->setWheelY(wy);
                    entry.ev->setMouseX(event->getMouseX());
                    entry.ev->setMouseY(event->getMouseY());
                    entry.last_ms = now_ms;
                    break;
                }
                case EventType::CoalesceStrategy::Count:
                {
                    int cnt = entry.ev->getClickCount();
                    entry.ev->setClickCount(cnt + 1);
                    entry.last_ms = now_ms;
                    break;
                }
                case EventType::CoalesceStrategy::None:
                default:
                    // Should not be metered if strategy None; fall back to enqueue
                    eventQueue.push(std::move(event));
                    return;
            }
        }

        // Throttle flush by configured interval (uses global default if unset)
        Uint32 interval = type.getMeterIntervalMs() ? type.getMeterIntervalMs() : SDOM_EVENT_METER_MS_DEFAULT;
        if (coalesce_last_flush_ms_ == 0 || (now_ms - coalesce_last_flush_ms_) >= interval)
            flushCoalesced_();

// std::cout << "[addEvent] type=" << event->getTypeName()
//           << " isCritical=" << isCritical
//           << " queueSize(before)=" << eventQueue.size()
//           << std::endl;

    }

    bool EventManager::hasListeners(const EventType& type) const
    {
        // Fast path: if no objects exist, trivially false
        const auto names = getFactory().getDisplayObjectNames();
        for (const auto& name : names)
        {
            auto handle = getFactory().getDisplayObject(name);
            if (!handle) continue;
            auto* obj = dynamic_cast<IDisplayObject*>(handle.get());
            if (!obj) continue;
            if (obj->hasEventListener(type, /*useCapture*/true) || obj->hasEventListener(type, /*useCapture*/false))
                return true;
        }
        return false;
    }

    bool EventManager::should_emit_hover_events() const
    {
        // If anyone listens explicitly, emit hover events
        if (hasListeners(EventType::MouseEnter) || hasListeners(EventType::MouseLeave))
            return true;
        // Heuristic: if common hover-sensitive UI types are present, keep
        // emitting hover so default onEvent handlers can update state.
        static const std::unordered_set<std::string> hover_types = {
            // Common interactive widgets that rely on hover state
            "Button", "IconButton", "ArrowButton",
            "CheckButton", "RadioButton", "TristateButton",
            "Slider", "ScrollBar",
            // Test scaffolds / simple containers used in examples
            "Box", "Frame"
        };
        const auto names = getFactory().getDisplayObjectNames();
        for (const auto& name : names)
        {
            auto handle = getFactory().getDisplayObject(name);
            auto* obj = handle ? dynamic_cast<IDisplayObject*>(handle.get()) : nullptr;
            if (!obj) continue;
            if (obj->isHidden()) continue;
            if (!obj->isEnabled()) continue;
            if (hover_types.find(obj->getType()) != hover_types.end())
                return true;
        }
        return false;
    }

    void EventManager::DispatchQueuedEvents() 
    {
        // Ensure any coalesced events are emitted before dispatching
        flushCoalesced_();
        DisplayHandle rootNode = getFactory().getStageHandle();
        if (!rootNode) { return; }
        while (auto event = takeNextEvent()) 
        {
            DisplayHandle object = event->getTarget();
            bool deliver = false;
            if (object.isValid())
            {
                // Deliver if target is the stage itself, a descendant of the stage,
                // or if the event type is marked global.
                if (object->getType() == "Stage" || event->getType().getGlobal())
                {
                    deliver = true;
                }
                else if (auto* rootObj = dynamic_cast<IDisplayObject*>(rootNode.get()))
                {
                    deliver = rootObj->isAncestorOf(object);
                    // Fallback: if ancestry check fails (e.g., during synthetic tests
                    // or when handles are reconstructed), allow delivery when the
                    // object is known to be on the current stage.
                    if (!deliver) {
                        IDisplayObject* objPtr = dynamic_cast<IDisplayObject*>(object.get());
                        if (objPtr && objPtr->isOnStage()) { deliver = true; }
                    }

                    // // 🔍 Debug block: show ancestry relationships
                    // std::cout << "DEBUG: isAncestorOf? this=" << rootObj->getName()
                    //         << " checking descendant=" << (object ? object->getName() : "<null>")
                    //         << " parent=" << (object && object->getParent() ? object->getParent()->getName() : "<none>")
                    //         << " result=" << (deliver ? "TRUE" : "FALSE")
                    //         << " event=" << event->getTypeName()
                    //         << std::endl;
                }
            }
#if defined(SDOM_DEBUG_BEHAVIOR_TEST)
            std::cout << "[DispatchQueuedEvents] type=" << event->getTypeName()
                      << " target=" << (object ? object.getName() : std::string("<null>"))
                      << " deliver=" << (deliver ? "yes" : "no")
                      << std::endl;
#endif
            if (deliver)
            {
// std::cout << "[DispatchQueuedEvents] Dispatching "
//           << event->getTypeName()
//           << " target="
//           << (object.isValid() ? object.getName() : "<null>")
//           << std::endl;

                dispatchEvent(std::move(event), rootNode);
            }
            // else
            // {
            //     std::cout << "⚠️  Dropping event " << event->getTypeName()
            //             << " (target=" << (object ? object->getName() : "<null>") << ")"
            //             << std::endl;
            // }
        }
    }

    std::unique_ptr<Event> EventManager::takeNextEvent()
    {
        flushCoalesced_();
        if (eventQueue.empty())
            return nullptr;

        auto evt = std::move(eventQueue.front());
        eventQueue.pop();
        return evt;
    }

    // Emit all coalesced events into the real queue (FIFO order not guaranteed
    // across different types; map order is acceptable for metered events).
    void EventManager::flushCoalesced_()
    {
// std::cout << "[flushCoalesced_] queueSize(before)=" << eventQueue.size() << std::endl;

        if (coalesce_map_.empty()) return;
        for (auto& kv : coalesce_map_)
        {
            if (kv.second.ev)
                eventQueue.push(std::move(kv.second.ev));
        }
        coalesce_map_.clear();
        coalesce_last_flush_ms_ = SDL_GetTicks();
    }

    void EventManager::dispatchEvent(std::unique_ptr<Event> event, DisplayHandle rootHandle)    
    {
        IDisplayObject* rootNode = rootHandle.get();
        if (rootNode == nullptr) { return; }

        Stage* stage = getStage(); //Core::getInstance().getStage();
        if (stage == nullptr) { return; }

        // // Convert SDL_Event to Lua table and make available to Lua listeners
        // sol::state& lua = getCore().getLua();
        // sol::state_view luaView(lua);
        // sol::table luaEvent = SDL_Utils::eventToLuaTable(event->getSDL_Event(), luaView);        

        // dispatch global "only" events
        if (event->getType().getGlobal())
        {
            // std::cout << CLR::CYAN << "Dispatching global-only event: " << event->getTypeName() << CLR::NORMAL << std::endl;   

            // Dispatch to the stage
            event->setPhase(Event::Phase::Target);
            auto eventCopy = std::make_unique<Event>(*event);
            eventCopy->setPhase(Event::Phase::Target);
            dispatchEventToAllNodesOnStage(std::move(event));
            // dispatchEventToAllEventListenersOnStage(std::move(eventCopy));
            dispatchEventToAllEventListenersGlobally(std::move(eventCopy));
            return;
        }

        // Capture Phase (if this event is even allowed to capture)
        if (event->getType().getCaptures() && !event->getType().getTargetOnly())
        {
            event->setPhase(Event::Phase::Capture);
            auto capturePhase = [&](DisplayHandle rootHandle, DisplayHandle targetHandle) -> void 
            {
                std::vector<DisplayHandle> nodeStack;
                // Build the stack from target to root
                DisplayHandle currentHandle = targetHandle;
                while (currentHandle && currentHandle != rootHandle) 
                {
                    IDisplayObject* currentNode = currentHandle.get();
                    if (!currentNode) break;
                    currentHandle = currentNode->getParent();
                    if (currentHandle)
                        nodeStack.push_back(currentHandle);
                }
                // Process nodes in reverse order (from root to target)
                while (!nodeStack.empty()) 
                {
                    DisplayHandle nodeHandle = nodeStack.back();
                    nodeStack.pop_back();
                    IDisplayObject* node = nodeHandle.get();
                    if (!node || node->isHidden()) 
                        continue;

                    event->setPhase(Event::Phase::Capture);
                    event->setCurrentTarget(nodeHandle); // Set currentTarget
                    if (node->isEnabled()) 
                    {
                        node->triggerEventListeners(*event, true ); // Trigger capture listeners
                        if (!event->isPropagationStopped() && !event->isDefaultBehaviorDisabled()) 
                        {
                            // Call the default onEvent method
                            node->onEvent(*event); 
                            // Call the users registered event function if available
                            if (getCore()._fnGetOnEvent())
                                getCore()._fnGetOnEvent()(*event);        
                        }
                    }
                    if (event->isPropagationStopped()) 
                        break;
                }
            };       
            getCore().setIsTraversing(true); 
            capturePhase(rootHandle, event->getTarget());
            getCore().setIsTraversing(false);
        }
        // Target Phase: Dispatch to the target only 
        event->setPhase(Event::Phase::Target);
        DisplayHandle targetHandle = event->getTarget();
        IDisplayObject* targetNode = targetHandle.get();
        if (targetNode && targetNode->isEnabled()) 
        {
            event->setCurrentTarget(targetHandle); // Set currentTarget
            targetNode->triggerEventListeners(*event, false ); // Trigger target listeners
            if (!event->isPropagationStopped() && !event->isDefaultBehaviorDisabled()) 
            {
                // Call the default onEvent method
                targetNode->onEvent(*event); 
                // Call the users registered event function if available
                if (getCore()._fnGetOnEvent())
                    getCore()._fnGetOnEvent()(*event);        
            }
        }

        // Bubbling Phase: Dispatch to the parent nodes
        if (event->getType().getBubbles() && !event->getType().getTargetOnly())
        {
            auto bubbleTraverse = [&](DisplayHandle nodeHandle, auto& self) -> void 
            {
                IDisplayObject* node = nodeHandle.get();
                if (!node || node->isHidden()) 
                    return;

                event->setPhase(Event::Phase::Bubbling);
                event->setCurrentTarget(nodeHandle); // Set currentTarget
                if (node->isEnabled()) 
                {
                    node->triggerEventListeners(*event, false ); // Trigger bubbling listeners
                    if (!event->isPropagationStopped() && !event->isDefaultBehaviorDisabled()) 
                    {
                        // Call the default onEvent method
                        node->onEvent(*event); 
                        // Call the users registered event function if available
                        if (getCore()._fnGetOnEvent())
                            getCore()._fnGetOnEvent()(*event);   
                    }
                }
                if (node->getParent() && !event->isPropagationStopped()) 
                {
                    DisplayHandle parentHandle = node->getParent();
                    if (parentHandle)
                        self(parentHandle, self);
                }
            };

            // Start bubbling from the parent of the target node`
            DisplayHandle targetHandle = event->getTarget();
            IDisplayObject* targetNode = targetHandle.get();
            if (targetNode && targetNode->getParent() && !event->isPropagationStopped()) 
            {
                DisplayHandle parentHandle = targetNode->getParent();
                if (parentHandle)
                {
                    getCore().setIsTraversing(true);
                    bubbleTraverse(parentHandle, bubbleTraverse);
                    getCore().setIsTraversing(false);
                }
            }            
        } // end if is allowed to bubble
    }


    void EventManager::dispatchEventToAllNodesOnStage(std::unique_ptr<Event> event) 
    {
        std::function<void(DisplayHandle, Event*)> dispatchToChildren =
            [&](DisplayHandle nodeHandle, Event* evt)
        {
            IDisplayObject* node = nodeHandle.get();
            if (!node) return;
            evt->setPhase(Event::Phase::Target);
            evt->setTarget(nodeHandle);
            // Call the default onEvent method
            node->onEvent(*evt);
            // Call the users registered event function if available
            if (getCore()._fnGetOnEvent())
                getCore()._fnGetOnEvent()(*event);               
            for (const auto& child : node->getChildren())
                dispatchToChildren(child, evt);
        };
        getCore().setIsTraversing(true);
        DisplayHandle stageHandle = getStageHandle();
        dispatchToChildren(stageHandle, event.get());
        getCore().setIsTraversing(false);
    }

    void EventManager::dispatchEventToAllEventListenersOnStage(std::unique_ptr<Event> event) 
    {
        std::function<void(DisplayHandle, Event*)> dispatchToChildren =
            [&](DisplayHandle nodeHandle, Event* evt)
        {
            if (!nodeHandle) return;
            evt->setPhase(Event::Phase::Target);
            evt->setTarget(nodeHandle);
            nodeHandle->triggerEventListeners(*evt, false);
            for (const auto& child : nodeHandle->getChildren())
                dispatchToChildren(child, evt);
        };
        getCore().setIsTraversing(true);
        DisplayHandle stageHandle = getStageHandle();
        dispatchToChildren(stageHandle, event.get());
        getCore().setIsTraversing(false);
    }    

    void EventManager::dispatchEventToAllEventListenersGlobally(std::unique_ptr<Event> event) 
    {
        std::vector<std::string> names = getFactory().getDisplayObjectNames();
        for (const auto& name : names) 
        {
            auto obj = getFactory().getDisplayObject(name);
            if (obj) 
            {
                Event evnt = *event;
                obj->triggerEventListeners(evnt, false);
            }
        }
    }



    bool EventManager::isMouseWithinBounds(IDisplayObject& target) const 
    {
        // Skip bounds checking if the target is not visible
        if (target.isHidden()) 
            return false;

        // Get the stage and mouse position
        // Stage* stage = target.getStage();
        SDL_Point mousePoint = { static_cast<int>(Stage::mouseX), static_cast<int>(Stage::mouseY) };
        SDL_Rect targetRect = 
        {
            static_cast<int>(target.getX()),
            static_cast<int>(target.getY()),
            static_cast<int>(target.getWidth()),
            static_cast<int>(target.getHeight())
        };
        bool inside = SDL_PointInRect(&mousePoint, &targetRect);
        return inside;
    }

    DisplayHandle EventManager::findTopObjectUnderMouse(DisplayHandle rootNode, DisplayHandle excludeNode) const 
    {
        DisplayHandle targetHandle; // Reset target handle
        int maxDepth = -1;      // Track the maximum depth of the target node
        int maxZ = std::numeric_limits<int>::min();
        int maxSeq = -1;
        int seqCounter = 0;

        auto traverse = [&](DisplayHandle nodeHandle, int depth, auto& self) -> void 
        {
            if (!nodeHandle || nodeHandle == excludeNode) return;
            auto& node = *nodeHandle; // Dereference DisplayHandle to get IDisplayObject&
            ++seqCounter; // visitation sequence (monotonic)
            int seqNow = seqCounter;

            if (node.isHidden())
                return;

            // Only consider node as a candidate if it is clickable and under mouse
            if (node.isClickable() && isMouseWithinBounds(node)) 
            {
                int z = node.getZOrder();
                bool take = false;
                if (!targetHandle) {
                    take = true;
                } else if (z > maxZ) {
                    take = true;
                } else if (z == maxZ) {
                    if (depth > maxDepth) take = true;
                    else if (depth == maxDepth && seqNow > maxSeq) take = true;
                }

                if (take) {
                    targetHandle = nodeHandle;
                    maxDepth = depth;
                    maxZ = z;
                    maxSeq = seqNow;
                }
            }

            // Traverse children (depth-first)
            for (const auto& child : nodeHandle->getChildren()) 
                self(child, depth + 1, self);
        };

        getCore().setIsTraversing(true);
        traverse(rootNode, 0, traverse);
        getCore().setIsTraversing(false);

        // Default to rootNode if no other node is targeted
        if (!targetHandle)
            targetHandle = rootNode; 
        return targetHandle;
    }  // END: findTopObjectUnderMouse()

    // A hover-specific resolver which ignores clickability when determining
    // the top-most object under the mouse. Useful for hover/scroll targeting.
    DisplayHandle EventManager::findTopObjectUnderMouseForHover(DisplayHandle rootNode, DisplayHandle excludeNode) const 
    {
        DisplayHandle targetHandle; // Reset target handle
        int maxDepth = -1;      // Track the maximum depth of the target node
        int maxZ = std::numeric_limits<int>::min();
        int maxSeq = -1;
        int seqCounter = 0;

        auto traverse = [&](DisplayHandle nodeHandle, int depth, auto& self) -> void 
        {
            if (!nodeHandle || nodeHandle == excludeNode) return;
            auto& node = *nodeHandle; // Dereference DisplayHandle to get IDisplayObject&
            ++seqCounter; // visitation sequence (monotonic)
            int seqNow = seqCounter;

            if (node.isHidden())
                return;

            // For hover, consider any visible object that is under the mouse
            if (isMouseWithinBounds(node)) 
            {
                int z = node.getZOrder();
                bool take = false;
                if (!targetHandle) {
                    take = true;
                } else if (z > maxZ) {
                    take = true;
                } else if (z == maxZ) {
                    if (depth > maxDepth) take = true;
                    else if (depth == maxDepth && seqNow > maxSeq) take = true;
                }

                if (take) {
                    targetHandle = nodeHandle;
                    maxDepth = depth;
                    maxZ = z;
                    maxSeq = seqNow;
                }
            }

            // Traverse children (depth-first)
            for (const auto& child : nodeHandle->getChildren()) 
                self(child, depth + 1, self);
        };

        getCore().setIsTraversing(true);
        traverse(rootNode, 0, traverse);
        getCore().setIsTraversing(false);

        if (!targetHandle)
            targetHandle = rootNode; 
        return targetHandle;
    } // END: findTopObjectUnderMouseForHover()

    // Point-based resolver that uses the provided (px, py) in stage/render coordinates.
    // When clickableOnly is true, only consider clickable nodes; otherwise, any visible node.
    DisplayHandle EventManager::findTopObjectAt(DisplayHandle rootNode, float px, float py, DisplayHandle excludeNode, bool clickableOnly) const
    {
        DisplayHandle targetHandle;
        int maxDepth = -1;
        int maxZ = std::numeric_limits<int>::min();
        int maxSeq = -1;
        int seqCounter = 0;

        auto pointInNode = [&](IDisplayObject& node) -> bool
        {
            if (node.isHidden()) return false;
            SDL_Point p { static_cast<int>(px), static_cast<int>(py) };
            SDL_Rect r {
                static_cast<int>(node.getX()),
                static_cast<int>(node.getY()),
                static_cast<int>(node.getWidth()),
                static_cast<int>(node.getHeight())
            };
            return SDL_PointInRect(&p, &r);
        };

        auto traverse = [&](DisplayHandle nodeHandle, int depth, auto& self) -> void 
        {
            if (!nodeHandle || nodeHandle == excludeNode) return;
            IDisplayObject* obj = nodeHandle.get();
            if (!obj) return;
            ++seqCounter;
            int seqNow = seqCounter;

            if (!obj->isHidden())
            {
                bool okClickable = !clickableOnly || obj->isClickable();
                if (okClickable && pointInNode(*obj))
                {
                    int z = obj->getZOrder();
                    bool take = false;
                    if (!targetHandle) take = true;
                    else if (z > maxZ) take = true;
                    else if (z == maxZ) {
                        if (depth > maxDepth) take = true;
                        else if (depth == maxDepth && seqNow > maxSeq) take = true;
                    }
                    if (take) {
                        targetHandle = nodeHandle;
                        maxDepth = depth;
                        maxZ = z;
                        maxSeq = seqNow;
                    }
                }
            }

            for (const auto& child : nodeHandle->getChildren())
                self(child, depth + 1, self);
        };

        getCore().setIsTraversing(true);
        traverse(rootNode, 0, traverse);
        getCore().setIsTraversing(false);

        if (!targetHandle)
        {
            // Global fallback: scan all registered display objects in the factory.
            // This defends against cases where the object tree link is not present
            // in the current stage's children_ but the object is active and visible.
            const auto names = getFactory().getDisplayObjectNames();
            for (const auto& name : names)
            {
                DisplayHandle h = getFactory().getDisplayObject(name);
                if (!h || h == excludeNode) continue;
                IDisplayObject* obj = h.get();
                if (!obj || obj->isHidden()) continue;
                if (clickableOnly && !obj->isClickable()) continue;
                SDL_Point p { static_cast<int>(px), static_cast<int>(py) };
                SDL_Rect r {
                    static_cast<int>(obj->getX()),
                    static_cast<int>(obj->getY()),
                    static_cast<int>(obj->getWidth()),
                    static_cast<int>(obj->getHeight())
                };
                if (SDL_PointInRect(&p, &r))
                {
                    int z = obj->getZOrder();
                    if (!targetHandle || z > maxZ)
                    {
                        targetHandle = h;
                        maxZ = z;
                    }
                }
            }
        }

        if (!targetHandle)
            targetHandle = rootNode;
        return targetHandle;
    }


    // --- preprocessSDLEvent: Normalize coordinates and sync stage mouse position ------------
    // ⚙️ System Behavior:
    // Converts window coordinates into renderer logical coordinates, sets the stage’s
    // mouseX/mouseY values consistently for hit-testing and event dispatch.
    void EventManager::preprocessSDLEvent(SDL_Event& e, Stage* stage)
    {
        const SDL_EventType et = static_cast<SDL_EventType>(e.type);

        // Only convert window->render coordinates when the event is associated
        // with a real window (synthetic tests may omit windowID and already
        // provide logical/render coordinates).
        if (isMouseEvent(et))
        {
            if (auto* renderer = getRenderer())
            {
                SDL_Window* win = SDL_GetWindowFromEvent(&e);
                if (win)
                    SDL_ConvertEventToRenderCoordinates(renderer, &e);
            }
            else
            {
                ERROR("Unable to retrieve proper SDL Renderer.");
            }
        }

        // Sync stage mouse position using the best-available fields, even if the
        // type enum was not recognized (heuristic for synthetic events).
        if (et == SDL_EVENT_MOUSE_WHEEL)
        {
            stage->mouseX = static_cast<float>(e.wheel.mouse_x);
            stage->mouseY = static_cast<float>(e.wheel.mouse_y);
        }
        else if (et == SDL_EVENT_MOUSE_MOTION)
        {
            stage->mouseX = static_cast<float>(e.motion.x);
            stage->mouseY = static_cast<float>(e.motion.y);
        }
        else if (et == SDL_EVENT_MOUSE_BUTTON_DOWN || et == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            stage->mouseX = static_cast<float>(e.button.x);
            stage->mouseY = static_cast<float>(e.button.y);
        }
        else
        {
            // Heuristic fallback: pick first non-zero coordinate among mouse unions
            if (e.wheel.mouse_x != 0 || e.wheel.mouse_y != 0)
            {
                stage->mouseX = static_cast<float>(e.wheel.mouse_x);
                stage->mouseY = static_cast<float>(e.wheel.mouse_y);
            }
            else if (e.motion.x != 0 || e.motion.y != 0)
            {
                stage->mouseX = static_cast<float>(e.motion.x);
                stage->mouseY = static_cast<float>(e.motion.y);
            }
            else if (e.button.x != 0 || e.button.y != 0)
            {
                stage->mouseX = static_cast<float>(e.button.x);
                stage->mouseY = static_cast<float>(e.button.y);
            }
        }
    } // END -- preprocessSDLEvent()

    // --- isWindowEvent: Filter SDL window-related events -------------------------------------
    bool EventManager::isWindowEvent(SDL_EventType type)
    {
        switch (type)
        {
            case SDL_EVENT_WINDOW_SHOWN:
            case SDL_EVENT_WINDOW_HIDDEN:
            case SDL_EVENT_WINDOW_FOCUS_LOST:
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_EXPOSED:
            case SDL_EVENT_WINDOW_OCCLUDED:
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
            case SDL_EVENT_WINDOW_MOVED:
                return true;
            default:
                return false;
        }
    }

    // --- isKeyboardEvent: Filter SDL keyboard-related events ---------------------------------
    bool EventManager::isKeyboardEvent(SDL_EventType type)
    {
        return (type == SDL_EVENT_KEY_DOWN || type == SDL_EVENT_KEY_UP);
    }

    // --- isMouseEvent: Filter SDL mouse-related events ---------------------------------------
    bool EventManager::isMouseEvent(SDL_EventType type)
    {
        return (type == SDL_EVENT_MOUSE_BUTTON_DOWN  ||
                type == SDL_EVENT_MOUSE_BUTTON_UP    ||
                type == SDL_EVENT_MOUSE_MOTION       ||
                type == SDL_EVENT_MOUSE_WHEEL        ||
                type == SDL_EVENT_WINDOW_MOUSE_ENTER ||
                type == SDL_EVENT_WINDOW_MOUSE_LEAVE);
    }


    // --- dispatchWindowEvents: Handle SDL window-level events -------------------------------
    // 💡 Notes:
    // Converts SDL window notifications (focus, resize, show/hide, fullscreen) into
    // SDOM::EventType equivalents and queues them to the event system.
    void EventManager::dispatchWindowEvents(const SDL_Event& e)
    {
        SDL_Window* activeWindow = SDL_GetWindowFromEvent(&e);
        if (activeWindow == nullptr) { return; }

        DisplayHandle stageHandle = getFactory().getStageHandle();
        if (!stageHandle) { return; }

        const auto sdlType = static_cast<SDL_EventType>(e.type);

        auto makeAndQueue = [&](EventType& type)
        {
            auto evt = std::make_unique<Event>(type, stageHandle);
            evt->setSDL_Event(e);
            addEvent(std::move(evt));
        };

        switch (sdlType)
        {
            case SDL_EVENT_WINDOW_SHOWN:             makeAndQueue(EventType::Show); break;
            case SDL_EVENT_WINDOW_HIDDEN:            makeAndQueue(EventType::Hide); break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:        makeAndQueue(EventType::FocusLost); break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:      makeAndQueue(EventType::FocusGained); break;
            case SDL_EVENT_WINDOW_RESIZED:
            {
                // Include new size in payload for listeners that care
                auto evt = std::make_unique<Event>(EventType::Resize, stageHandle);
                evt->setSDL_Event(e);
                evt->setPayloadValue("width",  e.window.data1);
                evt->setPayloadValue("height", e.window.data2);
                addEvent(std::move(evt));
                break;
            }
            case SDL_EVENT_WINDOW_EXPOSED:           makeAndQueue(EventType::Show); break;
            case SDL_EVENT_WINDOW_OCCLUDED:          makeAndQueue(EventType::Hide); break;
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:  makeAndQueue(EventType::EnterFullscreen); break;
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:  makeAndQueue(EventType::LeaveFullscreen); break;
            case SDL_EVENT_WINDOW_MOVED:
            {
                // Only emit Move if anyone listens or metering will coalesce it
                if (hasListeners(EventType::Move) || EventType::Move.isMeterEnabled())
                {
                    auto evt = std::make_unique<Event>(EventType::Move, stageHandle);
                    evt->setSDL_Event(e);
                    // Provide compositor-reported position in payload
                    evt->setPayloadValue("x", e.window.data1);
                    evt->setPayloadValue("y", e.window.data2);
                    addEvent(std::move(evt));
                }
                break;
            }
            default: break;
        }
    } // END -- dispatchWindowEvents()


    // --- dispatchKeyboardEvents: Handle SDL keyboard input -----------------------------
    // 🧠 Behavior:
    // - Uses the currently focused DisplayHandle for KeyDown / KeyUp.
    // - If no focused object exists, the Stage acts as the default target.
    // - Converts SDL key info into Event payload fields for higher-level logic.
    void EventManager::dispatchKeyboardEvents(const SDL_Event& e)
    {
        if (e.type != SDL_EVENT_KEY_DOWN && e.type != SDL_EVENT_KEY_UP)
            return;

        Core& core = getCore();
        DisplayHandle target = core.getKeyboardFocusedObject();
        const float elapsed = core.getElapsedTime();

        // No focused object means the stage acts as the global target so
        // keyboard events still enter the queue and remain visible to the C API.
        if (!target || !target.isValid())
            target = getFactory().getStageHandle();

        if (!target || !target.isValid())
            return;

        // --- KeyDown -------------------------------------------------------------
        if (e.type == SDL_EVENT_KEY_DOWN)
        {
            auto keyDown = std::make_unique<Event>(EventType::KeyDown, target, elapsed);
            keyDown->setSDL_Event(e);
            keyDown->setScanCode(e.key.scancode);
            keyDown->setKeycode(e.key.key);
            keyDown->setKeymod(e.key.mod);
            keyDown->setAsciiCode(SDL_Utils::keyToAscii(e.key.key, e.key.mod));
            addEvent(std::move(keyDown));
        }

        // --- KeyUp ---------------------------------------------------------------
        if (e.type == SDL_EVENT_KEY_UP)
        {
            auto keyUp = std::make_unique<Event>(EventType::KeyUp, target, elapsed);
            keyUp->setSDL_Event(e);
            keyUp->setScanCode(e.key.scancode);
            keyUp->setKeycode(e.key.key);
            keyUp->setKeymod(e.key.mod);
            keyUp->setAsciiCode(SDL_Utils::keyToAscii(e.key.key, e.key.mod));
            addEvent(std::move(keyUp));
        }
    } // END -- dispatchKeyboardEvents()


    // --- dispatchMouseEvents: Handle SDL mouse button/move/wheel events ---------------------
    // ⚙️ System Behavior:
    // Translates SDL mouse events into SDOM equivalents, sets coordinates,
    // click count, and button data, then queues for dispatch.
    void EventManager::dispatchMouseEvents(const SDL_Event& e, DisplayHandle node, DisplayHandle topObject)
    {
        static DisplayHandle s_lastMouseDownObject = nullptr;
        const auto elapsed = getCore().getElapsedTime();

        float mX = 0.0f, mY = 0.0f;
        switch (static_cast<SDL_EventType>(e.type))
        {
            case SDL_EVENT_MOUSE_WHEEL:
                mX = static_cast<float>(e.wheel.mouse_x);
                mY = static_cast<float>(e.wheel.mouse_y);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                mX = static_cast<float>(e.button.x);
                mY = static_cast<float>(e.button.y);
                break;
            case SDL_EVENT_MOUSE_MOTION:
            default:
                mX = static_cast<float>(e.motion.x);
                mY = static_cast<float>(e.motion.y);
                break;
        }

        switch (e.type)
        {
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                auto evt = std::make_unique<Event>(EventType::MouseButtonDown, node, elapsed);
                evt->setSDL_Event(e);
                evt->setMouseX(mX);
                evt->setMouseY(mY);
                evt->setButton(e.button.button);
                evt->setClickCount(e.button.clicks);
                evt->setTarget(topObject);

// std::cout << "Dispatching " << evt->getTypeName()
//           << " EventType@" << static_cast<const void*>(&evt->getTypeRef())
//           << std::endl;

                addEvent(std::move(evt));
                s_lastMouseDownObject = topObject;
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                auto evt = std::make_unique<Event>(EventType::MouseButtonUp, node, elapsed);
                evt->setSDL_Event(e);
                evt->setMouseX(mX);
                evt->setMouseY(mY);
                evt->setButton(e.button.button);
                evt->setClickCount(e.button.clicks);
                evt->setTarget(topObject);
                
// std::cout << "Dispatching " << evt->getTypeName()
//           << " EventType@" << static_cast<const void*>(&evt->getTypeRef())
//           << std::endl;

                addEvent(std::move(evt));

                // Fire click / double click events if same object
                if (s_lastMouseDownObject == topObject)
                {
                    const int clicks = e.button.clicks;

                    // 🔹 Always dispatch a MouseClick
                    {
                        auto clickEvent = std::make_unique<Event>(EventType::MouseClick, node, elapsed);
                        clickEvent->setSDL_Event(e);
                        clickEvent->setMouseX(mX);
                        clickEvent->setMouseY(mY);
                        clickEvent->setButton(e.button.button);
                        clickEvent->setClickCount(clicks);
                        clickEvent->setTarget(topObject);
                
// std::cout << "Dispatching " << evt->getTypeName()
//           << " EventType@" << static_cast<const void*>(&evt->getTypeRef())
//           << std::endl;

                        addEvent(std::move(clickEvent));
                    }

                    // 🔹 Add specialized multi-click events if needed
                    if (clicks >= 2) {
                        auto dblEvent = std::make_unique<Event>(EventType::MouseDoubleClick, node, elapsed);
                        dblEvent->setSDL_Event(e);
                        dblEvent->setMouseX(mX);
                        dblEvent->setMouseY(mY);
                        dblEvent->setButton(e.button.button);
                        dblEvent->setClickCount(clicks);
                        dblEvent->setTarget(topObject);
                
// std::cout << "Dispatching " << evt->getTypeName()
//           << " EventType@" << static_cast<const void*>(&evt->getTypeRef())
//           << std::endl;

                        addEvent(std::move(dblEvent));
                    }

                    // 🔹 Maintain your existing keyboard focus rule
                    if (clicks == 1) {
                        if (auto* topNode = dynamic_cast<IDisplayObject*>(topObject.get())) {
                            if (topNode->isTabEnabled()) {
                                topNode->setKeyboardFocus();
                            }
                        }
                    }
                }
                break;
            }

            case SDL_EVENT_MOUSE_WHEEL:
            {
                // Prefer hovered object as target; if none set yet, resolve
                // using hover-specific resolver (ignores clickability).
                DisplayHandle hovered = getCore().getMouseHoveredObject();
                DisplayHandle wheelTarget = (hovered && hovered.isValid())
                    ? hovered
                    : findTopObjectUnderMouseForHover(node, draggedObject);
                // Keep Core hovered in sync on wheel
                getCore().setMouseHoveredObject(wheelTarget);
                auto evt = std::make_unique<Event>(EventType::MouseWheel, wheelTarget, elapsed);
                evt->setSDL_Event(e);
                evt->setWheelX(static_cast<float>(e.wheel.x));
                evt->setWheelY(static_cast<float>(e.wheel.y));
                evt->setMouseX(mX);
                evt->setMouseY(mY);
                evt->setButton(static_cast<uint8_t>(e.wheel.direction));
                evt->setClickCount(0);
                evt->setTarget(wheelTarget);
                
// std::cout << "Dispatching " << evt->getTypeName()
//           << " EventType@" << static_cast<const void*>(&evt->getTypeRef())
//           << std::endl;

                addEvent(std::move(evt));
                break;
            }

            // MouseMove events are enqueued by updateHoverState() to avoid duplicates

            default: break;
        }
    } // END -- dispatchMouseEvents()


    // --- updateHoverState: Track MouseEnter / MouseLeave / MouseMove transitions ------------
    // 💡 Notes:
    // Detects object-level hover transitions within the scene graph. Called every frame for
    // motion events, this function queues MouseMove, MouseEnter, and MouseLeave as needed.
    void EventManager::updateHoverState(const SDL_Event& e, [[maybe_unused]] DisplayHandle node)
    {
        static DisplayHandle lastHoveredObject = nullptr;
        const SDL_EventType et = static_cast<SDL_EventType>(e.type);

        // Only synthesize move/enter/leave during motion
        if (et != SDL_EVENT_MOUSE_MOTION)
        {
            // Keep lastHoveredObject in sync with core's notion, but avoid re-hit-testing here
            lastHoveredObject = getCore().getMouseHoveredObject();
            return;
        }

        // Reuse the hovered object computed earlier in Queue_SDL_Event to avoid
        // redundant hit-tests on every motion event.
        DisplayHandle currentHovered = getCore().getMouseHoveredObject();
        // Update watchdog: any motion resets the idle counter and tracks target
        hover_watch_target_ = currentHovered;
        hover_idle_frames_ = 0;

        const float mX = static_cast<float>(e.motion.x);
        const float mY = static_cast<float>(e.motion.y);
        const float elapsed = getCore().getElapsedTime();

        // --- Dispatch MouseMove (only if anyone listens) -----------------------------------
        // MouseMove is high-frequency; if no listener exists anywhere, skip queuing it.
        if (hasListeners(EventType::MouseMove))
        {
            auto moveEvt = std::make_unique<Event>(EventType::MouseMove, currentHovered, elapsed);
            moveEvt->setSDL_Event(e);
            moveEvt->setMouseX(mX);
            moveEvt->setMouseY(mY);
            moveEvt->setTarget(currentHovered);
            moveEvt->setCurrentTarget(currentHovered);
            addEvent(std::move(moveEvt));
        }

        // --- Handle MouseLeave / MouseEnter (leave always, enter gated) --------------------
        if (currentHovered != lastHoveredObject)
        {
            // Always emit MouseLeave for the previous hovered object so behavior tests
            // and default hover logic remain reliable, even when MouseEnter is gated off.
            if (lastHoveredObject)
            {
                auto leaveEvt = std::make_unique<Event>(EventType::MouseLeave, lastHoveredObject, elapsed);
                leaveEvt->setSDL_Event(e);
                leaveEvt->setMouseX(mX);
                leaveEvt->setMouseY(mY);
                leaveEvt->setTarget(lastHoveredObject);
                leaveEvt->setCurrentTarget(lastHoveredObject);
                leaveEvt->setRelatedTarget(currentHovered);
                addEvent(std::move(leaveEvt));
            }

            // Emit MouseEnter only when hover events are desired to avoid event spam.
            if (currentHovered && should_emit_hover_events())
            {
                auto enterEvt = std::make_unique<Event>(EventType::MouseEnter, currentHovered, elapsed);
                enterEvt->setSDL_Event(e);
                enterEvt->setMouseX(mX);
                enterEvt->setMouseY(mY);
                enterEvt->setTarget(currentHovered);
                enterEvt->setCurrentTarget(currentHovered);
                enterEvt->setRelatedTarget(lastHoveredObject);
                addEvent(std::move(enterEvt));
            }
        }

        lastHoveredObject = currentHovered;
    } // END -- updateHoverState()

    // --- onFrameTick: per-frame hover watchdog ---------------------------------------------
    // Forces a MouseLeave if no motion events have been processed for a number of frames
    // while an object remains hovered. This addresses cases where input pauses or
    // window focus changes without motion, preventing "stuck hover" states.
    void EventManager::onFrameTick()
    {
        if (hover_watch_target_) {
            ++hover_idle_frames_;
            constexpr int kHoverLeaveFrameLimit = 250;
            if (hover_idle_frames_ >= kHoverLeaveFrameLimit) {
                float elapsed = getCore().getElapsedTime();
                auto leaveEvt = std::make_unique<Event>(EventType::MouseLeave, hover_watch_target_, elapsed);
                // Use last known stage mouse coords if available (optional)
                leaveEvt->setTarget(hover_watch_target_);
                addEvent(std::move(leaveEvt));
                // Clear hovered state
                getCore().setMouseHoveredObject(DisplayHandle(nullptr));
                hover_watch_target_ = nullptr;
                hover_idle_frames_ = 0;
            }
        }
    }


    // --- dispatchWindowEnterLeave: Detect when the mouse enters or leaves the window --------
    // ⚙️ System Behavior:
    // Uses SDL’s window focus API to trigger stage-level MouseEnter/MouseLeave events when
    // the mouse crosses window boundaries.
    void EventManager::dispatchWindowEnterLeave(const SDL_Event& e, [[maybe_unused]] DisplayHandle node)
    {
        // Enable mouse capture once to ensure consistent focus tracking; avoid
        // calling this per-event to reduce overhead on high-frequency motion.
        static bool s_mouseCaptured = false;
        if (!s_mouseCaptured) {
            SDL_CaptureMouse(true);
            s_mouseCaptured = true;
        }

        static SDL_Window* focusedWindow = SDL_GetMouseFocus();
        SDL_Window* currentWindow = SDL_GetMouseFocus();

        const float mX = static_cast<float>(e.motion.x);
        const float mY = static_cast<float>(e.motion.y);
        const float elapsed = getCore().getElapsedTime();

        if (focusedWindow == currentWindow)
            return; // No change in focus state

        DisplayHandle stageObject = getFactory().getStageHandle();
        if (!stageObject)
            return;

        // --- Mouse Enter -------------------------------------------------------------------
        if (currentWindow)
        {
            auto enterEvt = std::make_unique<Event>(EventType::MouseEnter, stageObject, elapsed);
            enterEvt->setSDL_Event(e);
            enterEvt->setMouseX(mX);
            enterEvt->setMouseY(mY);
            enterEvt->setTarget(stageObject);
            enterEvt->setCurrentTarget(stageObject);
            addEvent(std::move(enterEvt));
        }
        // --- Mouse Leave -------------------------------------------------------------------
        else
        {
            auto leaveEvt = std::make_unique<Event>(EventType::MouseLeave, stageObject, elapsed);
            leaveEvt->setSDL_Event(e);
            leaveEvt->setMouseX(mX);
            leaveEvt->setMouseY(mY);
            leaveEvt->setTarget(stageObject);
            leaveEvt->setCurrentTarget(stageObject);
            addEvent(std::move(leaveEvt));
        }

        focusedWindow = currentWindow;
    } // END -- dispatchWindowEnterLeave()


    // --- dispatchDragEvents: Manage dragging lifecycle (start, dragging, drop) --------------
    // 🧠 Internal Mechanism:
    // Implements a threshold-based drag detection system. Once the mouse moves beyond
    // DRAG_THRESHOLD pixels while pressed, a drag begins. Subsequent motion triggers
    // Dragging events until button release, which generates a Drop event.
    void EventManager::dispatchDragEvents(const SDL_Event& e, DisplayHandle node, DisplayHandle topObject)
    {
        // Use EventManager member state so other helpers see consistent drag state
        static float drag_offset_x = 0.0f;
        static float drag_offset_y = 0.0f;
        static float drag_start_x = -1.0f;
        static float drag_start_y = -1.0f;
        static Uint8 drag_button = 0; // active button for current drag (SDL button id)

        float mX = 0.0f, mY = 0.0f;
        switch (static_cast<SDL_EventType>(e.type))
        {
            case SDL_EVENT_MOUSE_MOTION:
                mX = static_cast<float>(e.motion.x);
                mY = static_cast<float>(e.motion.y);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                mX = static_cast<float>(e.button.x);
                mY = static_cast<float>(e.button.y);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                mX = static_cast<float>(e.wheel.mouse_x);
                mY = static_cast<float>(e.wheel.mouse_y);
                break;
            default:
                break;
        }
        const float elapsed = getCore().getElapsedTime();
        constexpr float DRAG_THRESHOLD = 5.0f;

        // --- Reset on Mouse Up --------------------------------------------------------------
        if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            drag_start_x = -1.0f;
            drag_start_y = -1.0f;
            drag_button = 0;
        }

        // --- Seed drag start ---------------------------------------------------------------
        if (drag_start_x < 0.0f && drag_start_y < 0.0f && e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            drag_start_x = mX;
            drag_start_y = mY;
            drag_button = e.button.button;
        }

        // --- Detect drag start --------------------------------------------------------------
        // Only consider starting a drag if a prior button-down seeded the start coords
        if (!isDragging && e.type == SDL_EVENT_MOUSE_MOTION && drag_start_x >= 0.0f && drag_start_y >= 0.0f)
        {
            if (fabs(mX - drag_start_x) > DRAG_THRESHOLD || fabs(mY - drag_start_y) > DRAG_THRESHOLD)
            {
                isDragging = true;
                draggedObject = topObject;
                drag_offset_x = mX - draggedObject->getX();
                drag_offset_y = mY - draggedObject->getY();

                auto dragEvt = std::make_unique<Event>(EventType::Drag, node, elapsed);
                dragEvt->setSDL_Event(e);
                dragEvt->setMouseX(mX);
                dragEvt->setMouseY(mY);
                dragEvt->setButton(drag_button);
                dragEvt->setDragOffsetX(drag_offset_x);
                dragEvt->setDragOffsetY(drag_offset_y);
                dragEvt->setTarget(draggedObject);
                dragEvt->setCurrentTarget(draggedObject);
                dragEvt->setRelatedTarget(draggedObject->getParent());
                addEvent(std::move(dragEvt));
            }
        }

        // --- Active dragging ---------------------------------------------------------------
        if (isDragging && e.type == SDL_EVENT_MOUSE_MOTION)
        {
            if (!draggedObject) { return; }

            auto draggingEvt = std::make_unique<Event>(EventType::Dragging, node, elapsed);
            draggingEvt->setSDL_Event(e);
            draggingEvt->setMouseX(mX);
            draggingEvt->setMouseY(mY);
            draggingEvt->setButton(drag_button);
            draggingEvt->setDragOffsetX(drag_offset_x);
            draggingEvt->setDragOffsetY(drag_offset_y);
            draggingEvt->setTarget(draggedObject);
            draggingEvt->setCurrentTarget(draggedObject);
            draggingEvt->setRelatedTarget(draggedObject->getParent());
            addEvent(std::move(draggingEvt));
        }

        // --- Drop event on release ----------------------------------------------------------
        if (isDragging && e.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            DisplayHandle dropTarget = findTopObjectUnderMouse(getFactory().getStageHandle(), draggedObject);

            auto dropEvt = std::make_unique<Event>(EventType::Drop, dropTarget, elapsed);
            dropEvt->setSDL_Event(e);
            dropEvt->setMouseX(mX);
            dropEvt->setMouseY(mY);
            dropEvt->setDragOffsetX(drag_offset_x);
            dropEvt->setDragOffsetY(drag_offset_y);
            dropEvt->setButton(drag_button ? drag_button : e.button.button);
            dropEvt->setTarget(draggedObject);
            dropEvt->setCurrentTarget(draggedObject);
            dropEvt->setRelatedTarget(dropTarget);
            addEvent(std::move(dropEvt));

            // Reset drag state
            isDragging = false;
            draggedObject = nullptr;
            drag_offset_x = drag_offset_y = 0.0f;
            drag_start_x = drag_start_y = -1.0f;
            drag_button = 0;
        }
    } // END -- dispatchDragEvents()



    // --- Queue_SDL_Event: Dispatch SDL events into SDOM Event system -------------------------
    // 💡 Notes:
    // This function now serves only as the high-level dispatcher. It delegates all 
    // SDL event processing to specialized private helpers (window, keyboard, mouse, etc.)
    // ensuring readability and modularity.
    //
    // 🧠 Internal Flow:
    // 1. Validate stage and renderer availability
    // 2. Normalize coordinates (preprocessSDLEvent)
    // 3. Identify top object under cursor
    // 4. Route event by category
    // 5. Run system-level updates (hover, window enter/leave, dragging)
    void EventManager::Queue_SDL_Event(SDL_Event& sdlEvent)
    {
        DisplayHandle node = getStageHandle();
        if (!node || !node->isEnabled() || node->isHidden()) { return; }

        Stage* stage = getStage();
        if (stage == nullptr) { return; }

        preprocessSDLEvent(sdlEvent, stage);

        SDL_EventType sdlType = static_cast<SDL_EventType>(sdlEvent.type);

        if (sdlType == SDL_EVENT_QUIT)
        {
            DisplayHandle stageHandle = getStageHandle();
            if (stageHandle)
            {
                auto quitEvent = std::make_unique<Event>(EventType::Quit, stageHandle);
                quitEvent->setSDL_Event(sdlEvent);
                addEvent(std::move(quitEvent));
            }
            return;
        }

        // Compute point under cursor for mouse events and resolve targets by point,
        // throttled to reduce repeated subtree walks during high-frequency motion.
        DisplayHandle cachedPtClickable = nullptr;
        if (isMouseEvent(sdlType))
        {
            float mX = 0.0f, mY = 0.0f;
            switch (sdlType)
            {
                case SDL_EVENT_MOUSE_MOTION:
                    mX = static_cast<float>(sdlEvent.motion.x);
                    mY = static_cast<float>(sdlEvent.motion.y);
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    mX = static_cast<float>(sdlEvent.button.x);
                    mY = static_cast<float>(sdlEvent.button.y);
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    mX = static_cast<float>(sdlEvent.wheel.mouse_x);
                    mY = static_cast<float>(sdlEvent.wheel.mouse_y);
                    break;
                default: break;
            }

            // Throttle hit-testing to at most once per throttle interval during motion,
            // but always recompute for button down/up and while actively dragging.
            static Uint32 s_last_hover_ht_ms = 0;
            static DisplayHandle s_cachedPtClickable = nullptr;
            static DisplayHandle s_cachedPtHover = nullptr;
            constexpr Uint32 kHoverThrottleMs = SDOM_HOVER_HITTEST_MS; // meter hover hit-test
            Uint32 now_ms = SDL_GetTicks();

            bool forceHT = (sdlType == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                             sdlType == SDL_EVENT_MOUSE_BUTTON_UP ||
                             isDragging);
            bool doHT = forceHT || (sdlType != SDL_EVENT_MOUSE_MOTION) ||
                        (now_ms - s_last_hover_ht_ms >= kHoverThrottleMs);

            if (doHT)
            {
                s_cachedPtClickable = findTopObjectAt(node, mX, mY, draggedObject, /*clickableOnly*/true);
                s_cachedPtHover     = findTopObjectAt(node, mX, mY, draggedObject, /*clickableOnly*/false);
                s_last_hover_ht_ms = now_ms;
            }

            getCore().setMouseHoveredObject(s_cachedPtHover);
            cachedPtClickable = s_cachedPtClickable;
            // Prefer clickable target; fall back to hovered if none
            {
                DisplayHandle buttonTarget = (s_cachedPtClickable && s_cachedPtClickable.isValid() && s_cachedPtClickable != node)
                    ? s_cachedPtClickable
                    : s_cachedPtHover;
#if defined(SDOM_DEBUG_BEHAVIOR_TEST)
                auto nm = [&](DisplayHandle h){ return (h && h.isValid()) ? h.getName() : std::string("<null>"); };
                std::cout << "[Queue_SDL_Event] mouse@(" << mX << "," << mY << ")"
                          << " clickTarget=" << nm(s_cachedPtClickable)
                          << " hoverTarget=" << nm(s_cachedPtHover)
                          << " chosen=" << nm(buttonTarget)
                          << " type=" << sdlType
                          << std::endl;
#endif
                dispatchMouseEvents(sdlEvent, node, buttonTarget);
            }

        }
        else if (isWindowEvent(sdlType)) { dispatchWindowEvents(sdlEvent); }
        else if (isKeyboardEvent(sdlType)) { dispatchKeyboardEvents(sdlEvent); }
        else
        {
            // Heuristic: treat events with mouse-like coordinates as mouse events
            float mX = 0.0f, mY = 0.0f; bool looksMouse = false;
            if (sdlEvent.wheel.mouse_x != 0 || sdlEvent.wheel.mouse_y != 0) {
                mX = static_cast<float>(sdlEvent.wheel.mouse_x);
                mY = static_cast<float>(sdlEvent.wheel.mouse_y);
                looksMouse = true;
            } else if (sdlEvent.motion.x != 0 || sdlEvent.motion.y != 0) {
                mX = static_cast<float>(sdlEvent.motion.x);
                mY = static_cast<float>(sdlEvent.motion.y);
                looksMouse = true;
            } else if (sdlEvent.button.x != 0 || sdlEvent.button.y != 0) {
                mX = static_cast<float>(sdlEvent.button.x);
                mY = static_cast<float>(sdlEvent.button.y);
                looksMouse = true;
            }

            if (looksMouse)
            {
                static Uint32 s_last_hover_ht_ms2 = 0;
                static DisplayHandle s_cachedPtClickable2 = nullptr;
                static DisplayHandle s_cachedPtHover2 = nullptr;
                constexpr Uint32 kHoverThrottleMs = SDOM_HOVER_HITTEST_MS;
                Uint32 now_ms = SDL_GetTicks();

                bool forceHT = isDragging; // wheel/unknown mouse-like: only force if dragging
                bool doHT = forceHT || (now_ms - s_last_hover_ht_ms2 >= kHoverThrottleMs);
                if (doHT)
                {
                    s_cachedPtClickable2 = findTopObjectAt(node, mX, mY, draggedObject, /*clickableOnly*/true);
                    s_cachedPtHover2     = findTopObjectAt(node, mX, mY, draggedObject, /*clickableOnly*/false);
                    s_last_hover_ht_ms2  = now_ms;
                }
                getCore().setMouseHoveredObject(s_cachedPtHover2);
                cachedPtClickable = s_cachedPtClickable2;
                // Prefer clickable target; fall back to hovered if none
                DisplayHandle buttonTarget = (s_cachedPtClickable2 && s_cachedPtClickable2.isValid() && s_cachedPtClickable2 != node)
                    ? s_cachedPtClickable2
                    : s_cachedPtHover2;
#if defined(SDOM_DEBUG_BEHAVIOR_TEST)
                auto nm2 = [&](DisplayHandle h){ return (h && h.isValid()) ? h.getName() : std::string("<null>"); };
                std::cout << "[Queue_SDL_Event] (heuristic) mouse@(" << mX << "," << mY << ")"
                          << " clickTarget=" << nm2(s_cachedPtClickable2)
                          << " hoverTarget=" << nm2(s_cachedPtHover2)
                          << " chosen=" << nm2(buttonTarget)
                          << " type=" << sdlType
                          << std::endl;
#endif
                dispatchMouseEvents(sdlEvent, node, buttonTarget);

            }
        }

        // Specialized systems
        // Only synthesize hover move/enter/leave on actual motion; avoid window
        // notifications (e.g., WINDOW_MOUSE_LEAVE) from clobbering hovered state.
        if (sdlType == SDL_EVENT_MOUSE_MOTION) { updateHoverState(sdlEvent, node); }
        // Window enter/leave only needs to run on motion or explicit window events;
        // avoid calling it for every event to reduce overhead.
        if (sdlType == SDL_EVENT_MOUSE_MOTION || isWindowEvent(sdlType)) { dispatchWindowEnterLeave(sdlEvent, node); }
        if (isMouseEvent(sdlType))
        {
            // Use the clickable target we already resolved above to avoid another hit-test.
            dispatchDragEvents(sdlEvent, node, cachedPtClickable);
        }
    } // END -- Queue_SDL_Event()


    void SDOM::EventManager::clearEventQueue()
    {
        while (!eventQueue.empty())
            eventQueue.pop();
    } // END -- clearEventQueue()

    std::vector<const SDOM::Event*> SDOM::EventManager::getQueuedEvents()
    {
        std::vector<const Event*> snapshot;
        snapshot.reserve(eventQueue.size());

        // std::queue doesn't support const iteration and cannot be copied when it
        // holds unique_ptr. Rotate the queue in-place to take a snapshot of raw
        // pointers, preserving order and restoring the queue state.
        const size_t n = eventQueue.size();
        for (size_t i = 0; i < n; ++i)
        {
            std::unique_ptr<Event> evt = std::move(eventQueue.front());
            snapshot.push_back(evt.get());
            eventQueue.pop();
            eventQueue.push(std::move(evt));
        }
        return snapshot;
    } // END -- getQueuedEvents()

    // Debug: print the contents of the event queue
    void EventManager::debugPrintEventQueue()
    {
        const auto snapshot = getQueuedEvents();
        std::cout << "🧾 [Event Queue Dump] (" << snapshot.size() << " events)\n";
        for (const Event* evt : snapshot)
        {
            if (evt)
                std::cout << "   • " << evt->getType().getName() << "\n";
        }
    }


} // namespace SDOM
