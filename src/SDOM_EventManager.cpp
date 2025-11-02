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
        // std::cout << "Event added: " << event->getTypeName() << std::endl;
        eventQueue.push(std::move(event));
    }

    void EventManager::DispatchQueuedEvents() 
    {
        DisplayHandle rootNode = getFactory().getStageHandle();
        if (!rootNode) return;
        while (!eventQueue.empty()) 
        {
            auto event = std::move(eventQueue.front());
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
                }
            }
            if (deliver)
            {
                dispatchEvent(std::move(event), rootNode);
            }
            eventQueue.pop();
        }
    }

    void EventManager::dispatchEvent(std::unique_ptr<Event> event, DisplayHandle rootHandle)    
    {
        IDisplayObject* rootNode = rootHandle.get();
        if (!rootNode) return;

        Stage* stage = getStage(); //Core::getInstance().getStage();
        if (!stage) return;

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
            case SDL_EVENT_WINDOW_FOCUS_LOST:
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_EXPOSED:
            case SDL_EVENT_WINDOW_OCCLUDED:
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
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
        if (!activeWindow)
            return;

        DisplayHandle stageHandle = getFactory().getStageHandle();
        if (!stageHandle)
            return;

        const auto sdlType = static_cast<SDL_EventType>(e.type);

        auto makeAndQueue = [&](EventType& type)
        {
            auto evt = std::make_unique<Event>(type, stageHandle);
            evt->setSDL_Event(e);
            addEvent(std::move(evt));
        };

        switch (sdlType)
        {
            case SDL_EVENT_WINDOW_FOCUS_LOST:         makeAndQueue(EventType::FocusLost); break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:       makeAndQueue(EventType::FocusGained); break;
            case SDL_EVENT_WINDOW_RESIZED:            makeAndQueue(EventType::Resize); break;
            case SDL_EVENT_WINDOW_EXPOSED:            makeAndQueue(EventType::Show); break;
            case SDL_EVENT_WINDOW_OCCLUDED:           makeAndQueue(EventType::Hide); break;
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:   makeAndQueue(EventType::EnterFullscreen); break;
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:   makeAndQueue(EventType::LeaveFullscreen); break;
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
        DisplayHandle focused = core.getKeyboardFocusedObject();
        const float elapsed = core.getElapsedTime();

        // If no focused object, broadcast to all event listeners on the stage
        // to avoid per-object copies (stage traversal reuses a single Event).
        if (!focused || !focused.isValid())
        {
            if (e.type == SDL_EVENT_KEY_DOWN)
            {
                auto keyDown = std::make_unique<Event>(EventType::KeyDown, getFactory().getStageHandle(), elapsed);
                keyDown->setSDL_Event(e);
                keyDown->setScanCode(e.key.scancode);
                keyDown->setKeycode(e.key.key);
                keyDown->setKeymod(e.key.mod);
                keyDown->setAsciiCode(SDL_Utils::keyToAscii(e.key.key, e.key.mod));
                dispatchEventToAllEventListenersOnStage(std::move(keyDown));
            }
            else if (e.type == SDL_EVENT_KEY_UP)
            {
                auto keyUp = std::make_unique<Event>(EventType::KeyUp, getFactory().getStageHandle(), elapsed);
                keyUp->setSDL_Event(e);
                keyUp->setScanCode(e.key.scancode);
                keyUp->setKeycode(e.key.key);
                keyUp->setKeymod(e.key.mod);
                keyUp->setAsciiCode(SDL_Utils::keyToAscii(e.key.key, e.key.mod));
                dispatchEventToAllEventListenersOnStage(std::move(keyUp));
            }
            return;
        }

        // --- KeyDown -------------------------------------------------------------
        if (e.type == SDL_EVENT_KEY_DOWN)
        {
            auto keyDown = std::make_unique<Event>(EventType::KeyDown, focused, elapsed);
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
            auto keyUp = std::make_unique<Event>(EventType::KeyUp, focused, elapsed);
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
                evt->mouse_x = mX;
                evt->mouse_y = mY;
                evt->button = e.button.button;
                evt->clickCount = e.button.clicks;
                evt->setTarget(topObject);
                addEvent(std::move(evt));
                s_lastMouseDownObject = topObject;
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                auto evt = std::make_unique<Event>(EventType::MouseButtonUp, node, elapsed);
                evt->setSDL_Event(e);
                evt->mouse_x = mX;
                evt->mouse_y = mY;
                evt->button = e.button.button;
                evt->clickCount = e.button.clicks;
                evt->setTarget(topObject);
                addEvent(std::move(evt));

                // Fire click / double click events if same object
                if (s_lastMouseDownObject == topObject)
                {
                    const int clicks = e.button.clicks;

                    // 🔹 Always dispatch a MouseClick
                    {
                        auto clickEvent = std::make_unique<Event>(EventType::MouseClick, node, elapsed);
                        clickEvent->setSDL_Event(e);
                        clickEvent->mouse_x = mX;
                        clickEvent->mouse_y = mY;
                        clickEvent->button = e.button.button;
                        clickEvent->clickCount = clicks;
                        clickEvent->setTarget(topObject);
                        addEvent(std::move(clickEvent));
                    }

                    // 🔹 Add specialized multi-click events if needed
                    if (clicks >= 2) {
                        auto dblEvent = std::make_unique<Event>(EventType::MouseDoubleClick, node, elapsed);
                        dblEvent->setSDL_Event(e);
                        dblEvent->mouse_x = mX;
                        dblEvent->mouse_y = mY;
                        dblEvent->button = e.button.button;
                        dblEvent->clickCount = clicks;
                        dblEvent->setTarget(topObject);
                        addEvent(std::move(dblEvent));
                    }

                    // 🔹 Maintain your existing keyboard focus rule
                    if (clicks == 1) {
                        if (auto* topNode = dynamic_cast<IDisplayObject*>(topObject.get()))
                            if (topNode->isTabEnabled())
                                topNode->setKeyboardFocus();
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
                evt->wheelX = e.wheel.x;
                evt->wheelY = e.wheel.y;
                evt->mouse_x = mX;
                evt->mouse_y = mY;
                evt->button = e.wheel.direction;
                evt->clickCount = 0;
                evt->setTarget(wheelTarget);
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
    void EventManager::updateHoverState(const SDL_Event& e, DisplayHandle node)
    {
        static DisplayHandle lastHoveredObject = nullptr;
        const SDL_EventType et = static_cast<SDL_EventType>(e.type);

        // Prefer clickable object under mouse; if none, fall back to any visible under mouse
        DisplayHandle clickable = findTopObjectUnderMouse(node, draggedObject);
        DisplayHandle currentHovered = (clickable && clickable != node) ? clickable
            : findTopObjectUnderMouseForHover(node, draggedObject);
        getCore().setMouseHoveredObject(currentHovered);

        // Only synthesize move/enter/leave during motion
        if (et != SDL_EVENT_MOUSE_MOTION)
        {
            lastHoveredObject = currentHovered;
            return;
        }

        const float mX = static_cast<float>(e.motion.x);
        const float mY = static_cast<float>(e.motion.y);
        const float elapsed = getCore().getElapsedTime();

        // --- Dispatch MouseMove ------------------------------------------------------------
        auto moveEvt = std::make_unique<Event>(EventType::MouseMove, currentHovered, elapsed);
        moveEvt->setSDL_Event(e);
        moveEvt->mouse_x = mX;
        moveEvt->mouse_y = mY;
        moveEvt->setTarget(currentHovered);
        moveEvt->setCurrentTarget(currentHovered);
        addEvent(std::move(moveEvt));

        // --- Handle MouseLeave -------------------------------------------------------------
        if (lastHoveredObject && lastHoveredObject != currentHovered)
        {
            auto leaveEvt = std::make_unique<Event>(EventType::MouseLeave, lastHoveredObject, elapsed);
            leaveEvt->setSDL_Event(e);
            leaveEvt->mouse_x = mX;
            leaveEvt->mouse_y = mY;
            leaveEvt->setTarget(lastHoveredObject);
            leaveEvt->setCurrentTarget(lastHoveredObject);
            leaveEvt->setRelatedTarget(currentHovered);
            addEvent(std::move(leaveEvt));
        }

        // --- Handle MouseEnter -------------------------------------------------------------
        if (currentHovered != lastHoveredObject)
        {
            auto enterEvt = std::make_unique<Event>(EventType::MouseEnter, currentHovered, elapsed);
            enterEvt->setSDL_Event(e);
            enterEvt->mouse_x = mX;
            enterEvt->mouse_y = mY;
            enterEvt->setTarget(currentHovered);
            enterEvt->setCurrentTarget(currentHovered);
            enterEvt->setRelatedTarget(lastHoveredObject);
            addEvent(std::move(enterEvt));
        }

        lastHoveredObject = currentHovered;
    } // END -- updateHoverState()


    // --- dispatchWindowEnterLeave: Detect when the mouse enters or leaves the window --------
    // ⚙️ System Behavior:
    // Uses SDL’s window focus API to trigger stage-level MouseEnter/MouseLeave events when
    // the mouse crosses window boundaries.
    void EventManager::dispatchWindowEnterLeave(const SDL_Event& e, DisplayHandle node)
    {
        SDL_CaptureMouse(true); // Ensure consistent focus tracking

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
            enterEvt->mouse_x = mX;
            enterEvt->mouse_y = mY;
            enterEvt->setTarget(stageObject);
            enterEvt->setCurrentTarget(stageObject);
            addEvent(std::move(enterEvt));
        }
        // --- Mouse Leave -------------------------------------------------------------------
        else
        {
            auto leaveEvt = std::make_unique<Event>(EventType::MouseLeave, stageObject, elapsed);
            leaveEvt->setSDL_Event(e);
            leaveEvt->mouse_x = mX;
            leaveEvt->mouse_y = mY;
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
                dragEvt->mouse_x = mX;
                dragEvt->mouse_y = mY;
                dragEvt->button  = drag_button;
                dragEvt->dragOffsetX = drag_offset_x;
                dragEvt->dragOffsetY = drag_offset_y;
                dragEvt->setTarget(draggedObject);
                dragEvt->setCurrentTarget(draggedObject);
                dragEvt->setRelatedTarget(draggedObject->getParent());
                addEvent(std::move(dragEvt));
            }
        }

        // --- Active dragging ---------------------------------------------------------------
        if (isDragging && e.type == SDL_EVENT_MOUSE_MOTION)
        {
            if (!draggedObject)
                return;

            auto draggingEvt = std::make_unique<Event>(EventType::Dragging, node, elapsed);
            draggingEvt->setSDL_Event(e);
            draggingEvt->mouse_x = mX;
            draggingEvt->mouse_y = mY;
            draggingEvt->button  = drag_button;
            draggingEvt->dragOffsetX = drag_offset_x;
            draggingEvt->dragOffsetY = drag_offset_y;
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
            dropEvt->mouse_x = mX;
            dropEvt->mouse_y = mY;
            dropEvt->dragOffsetX = drag_offset_x;
            dropEvt->dragOffsetY = drag_offset_y;
            dropEvt->button  = drag_button ? drag_button : e.button.button;
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
        if (!node || !node->isEnabled() || node->isHidden())
            return;

        Stage* stage = getStage();
        if (!stage)
            return;

        preprocessSDLEvent(sdlEvent, stage);

        SDL_EventType sdlType = static_cast<SDL_EventType>(sdlEvent.type);

        // Compute point under cursor for mouse events and resolve targets by point
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

            DisplayHandle ptClickable = findTopObjectAt(node, mX, mY, draggedObject, /*clickableOnly*/true);
            DisplayHandle ptHover     = findTopObjectAt(node, mX, mY, draggedObject, /*clickableOnly*/false);
            getCore().setMouseHoveredObject(ptHover);

            // DisplayHandle buttonTarget = (ptClickable && ptClickable.isValid() && ptClickable != node)
            //     ? ptClickable
            //     : ptHover;
            // dispatchMouseEvents(sdlEvent, node, buttonTarget);
            dispatchMouseEvents(sdlEvent, node, ptClickable);

        }
        else if (isWindowEvent(sdlType))          dispatchWindowEvents(sdlEvent);
        else if (isKeyboardEvent(sdlType))         dispatchKeyboardEvents(sdlEvent);
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
                DisplayHandle ptClickable = findTopObjectAt(node, mX, mY, draggedObject, /*clickableOnly*/true);
                DisplayHandle ptHover     = findTopObjectAt(node, mX, mY, draggedObject, /*clickableOnly*/false);
                getCore().setMouseHoveredObject(ptHover);
                // DisplayHandle buttonTarget = (ptClickable && ptClickable.isValid() && ptClickable != node)
                //     ? ptClickable
                //     : ptHover;
                // dispatchMouseEvents(sdlEvent, node, buttonTarget);
                dispatchMouseEvents(sdlEvent, node, ptClickable);

            }
        }

        // Specialized systems
        // Only synthesize hover move/enter/leave on actual motion; avoid window
        // notifications (e.g., WINDOW_MOUSE_LEAVE) from clobbering hovered state.
        if (sdlType == SDL_EVENT_MOUSE_MOTION)
            updateHoverState(sdlEvent, node);
        dispatchWindowEnterLeave(sdlEvent, node);
        if (isMouseEvent(sdlType))
        {
            // Recompute clickable target for drag in case events mutated state
            float mX = 0.0f, mY = 0.0f;
            switch (sdlType)
            {
                case SDL_EVENT_MOUSE_MOTION:       mX = static_cast<float>(sdlEvent.motion.x); mY = static_cast<float>(sdlEvent.motion.y); break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:    mX = static_cast<float>(sdlEvent.button.x); mY = static_cast<float>(sdlEvent.button.y); break;
                case SDL_EVENT_MOUSE_WHEEL:        mX = static_cast<float>(sdlEvent.wheel.mouse_x); mY = static_cast<float>(sdlEvent.wheel.mouse_y); break;
                default: break;
            }
            DisplayHandle ptClickable = findTopObjectAt(node, mX, mY, draggedObject, /*clickableOnly*/true);
            dispatchDragEvents(sdlEvent, node, ptClickable); // ToDo: This should not happen unless there is a button down event and not already dragging
        }
    } // END -- Queue_SDL_Event()


} // namespace SDOM
