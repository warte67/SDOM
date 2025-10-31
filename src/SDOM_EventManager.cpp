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
            if (object.isValid() && (rootNode->hasChild(object) || object->getType() == "Stage" || event->getType().getGlobal() ))
            {
                dispatchEvent(std::move(event), rootNode);
            }
            // dispatchEvent(std::move(event), rootNode);
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


/* 
    void EventManager::Queue_SDL_Event_helper(SDL_Event& sdlEvent) 
    {
        // Convert SDL_Event to SDOM::Event and add to the queue

        // Skip processing if the node is not enabled or is hidden
        DisplayHandle node = getStageHandle();
        // IDisplayObject* node = dynamic_cast<IDisplayObject*>(nodeHandle.get());
        if (!node) return;

        if (!node->isEnabled() || node->isHidden())
            return;
        // fetch the stage instance
        Stage* stage = getStage(); //Core::getInstance().getStage();
        if (!stage) return;


        // (previously there was an initial pre-conversion hit-test here)
        // Hit-testing must occur after converting window/render coordinates into
        // logical stage coordinates. The early call was moved below so mouse
        // coordinates are correct for hit-testing.

        // Scale the SDL event coordinates to the render coordinates
        SDL_Renderer* renderer = getRenderer();
        if (renderer)
            SDL_ConvertEventToRenderCoordinates(renderer, &sdlEvent);
        else
            ERROR("Unable to retrieve proper SDL Renderer.");

            // Note: The mouse coordinates are retrieved from the motion element of the 
            // SDL_Event structure. Depending on the SDL_Event type, this may not be valid. 
            // Special cases for offending event types will need to be handled appropriately 
            // in the future.

        // Set the mouse position in the stage (convert from window/render
        // coordinates back into logical stage coordinates using the Core
        // pixel scaling). This prevents mixing window pixels with stage
        // coordinates during hit-testing and event dispatch.  We must
        // consistently scale all event coordinates by the current pixel
        // width/height so hit-tests compare like-for-like values.
        // Use renderer coordinates directly for stage mouse positions.  The
        // SDL_ConvertEventToRenderCoordinates() call above converts window
        // coordinates into renderer/logical coordinates already, and the
        // display objects and hit-tests operate in that same coordinate space.
        stage->mouseX = static_cast<float>(sdlEvent.motion.x);
        stage->mouseY = static_cast<float>(sdlEvent.motion.y);
        // correct for the difference in the mouse wheel events
        if (sdlEvent.type == SDL_EVENT_MOUSE_WHEEL) 
        {
            stage->mouseX = static_cast<float>(sdlEvent.wheel.mouse_x);
            stage->mouseY = static_cast<float>(sdlEvent.wheel.mouse_y);
        }


        
        // Find the top object that is under the mouse cursor (after coordinate conversion)
        // Modify findTopObjectUnderMouse to exclude the dragged object
        DisplayHandle topObject = findTopObjectUnderMouse(node, draggedObject);
        // Update Core hovered object to the post-conversion topObject so Lua reads correct value
        getCore().setMouseHoveredObject(topObject);

        // alias the SDL_EventType for ease of use:
        SDL_EventType sdl_type = static_cast<SDL_EventType>(sdlEvent.type);

        // // TEST SDL EVENTS        
        // if (sdl_type != SDL_EVENT_MOUSE_MOTION)
        // {
        //     std::cout << CLR::YELLOW << "EVENT: " << SDL_Utils::eventTypeToString(sdl_type) << CLR::NORMAL << std::endl;
        // }


        // -----------------------------------------------------------------
        //     __      ___         _              ___             _      
        //     \ \    / (_)_ _  __| |_____ __ __ | __|_ _____ _ _| |_ ___
        //      \ \/\/ /| | ' \/ _` / _ \ V  V / | _|\ V / -_) ' \  _(_-<
        //       \_/\_/ |_|_||_\__,_\___/\_/\_/  |___|\_/\___|_||_\__/__/
        // 
        // -----------------------------------------------------------------

        // EventType::FocusLost == SDL_EVENT_WINDOW_FOCUS_LOST            
        if (sdl_type == SDL_EVENT_WINDOW_FOCUS_LOST)
        {
            SDL_Window* activeWindow = SDL_GetWindowFromEvent(&sdlEvent);
            if (activeWindow)
            {
                // Dispatch EventType::FocusLost
                // auto unfocusEvent = std::make_unique<Event>(EventType::FocusLost, getStage());
                auto unfocusEvent = std::make_unique<Event>(EventType::FocusLost, getFactory().getStageHandle());
                unfocusEvent->setSDL_Event(sdlEvent);
                addEvent(std::move(unfocusEvent));
            }
        }
        // EventType::FocusGained == SDL_EVENT_WINDOW_FOCUS_GAINED   
        if (sdl_type == SDL_EVENT_WINDOW_FOCUS_GAINED)
        {
            SDL_Window* activeWindow = SDL_GetWindowFromEvent(&sdlEvent);
            if (activeWindow)
            {
                // Dispatch EventType::FocusGained
                // auto focusEvent = std::make_unique<Event>(EventType::FocusGained, getStage());
                auto focusEvent = std::make_unique<Event>(EventType::FocusGained, getFactory().getStageHandle());
                focusEvent->setSDL_Event(sdlEvent);
                addEvent(std::move(focusEvent));
            }
        }

        // EventType::Resize == SDL_EVENT_WINDOW_RESIZED
        if (sdl_type == SDL_EVENT_WINDOW_RESIZED)
        {
            SDL_Window* activeWindow = SDL_GetWindowFromEvent(&sdlEvent);
            if (activeWindow)
            {
                // Dispatch EventType::Resize
                auto resizeEvent = std::make_unique<Event>(EventType::Resize, getFactory().getStageHandle());
                resizeEvent->setSDL_Event(sdlEvent);
                addEvent(std::move(resizeEvent));
            }
        }

        // EventType::Show == SDL_EVENT_WINDOW_EXPOSED
        if (sdl_type == SDL_EVENT_WINDOW_EXPOSED)
        {
            SDL_Window* activeWindow = SDL_GetWindowFromEvent(&sdlEvent);
            if (activeWindow)
            {
                // Dispatch EventType::Show
                auto showEvent = std::make_unique<Event>(EventType::Show, getFactory().getStageHandle());
                showEvent->setSDL_Event(sdlEvent);
                addEvent(std::move(showEvent));
            }
        }

        // EventType::Hide == SDL_EVENT_WINDOW_OCCLUDED
        if (sdl_type == SDL_EVENT_WINDOW_OCCLUDED)
        {
            SDL_Window* activeWindow = SDL_GetWindowFromEvent(&sdlEvent);
            if (activeWindow)
            {
                // Dispatch EventType::Hide
                auto hideEvent = std::make_unique<Event>(EventType::Hide, getFactory().getStageHandle());
                hideEvent->setSDL_Event(sdlEvent);
                addEvent(std::move(hideEvent));
            }
        }

        // EventType::EnterFullscreen == SDL_EVENT_WINDOW_ENTER_FULLSCREEN
        if (sdl_type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN)
        {
            SDL_Window* activeWindow = SDL_GetWindowFromEvent(&sdlEvent);
            if (activeWindow)
            {
                // Dispatch EventType::EnterFullscreen
                auto fullscreenEvent = std::make_unique<Event>(EventType::EnterFullscreen, getFactory().getStageHandle());
                fullscreenEvent->setSDL_Event(sdlEvent);
                addEvent(std::move(fullscreenEvent));
            }
        }

        // EventType::LeaveFullscreen == SDL_EVENT_WINDOW_LEAVE_FULLSCREEN
        if (sdl_type == SDL_EVENT_WINDOW_LEAVE_FULLSCREEN)
        {
            SDL_Window* activeWindow = SDL_GetWindowFromEvent(&sdlEvent);
            if (activeWindow)
            {
                // Dispatch EventType::LeaveFullscreen
                auto leaveFullscreenEvent = std::make_unique<Event>(EventType::LeaveFullscreen, getFactory().getStageHandle());
                leaveFullscreenEvent->setSDL_Event(sdlEvent);
                addEvent(std::move(leaveFullscreenEvent));
            }
        }


        // -----------------------------------------------------------------
        //   _  __         _                      _   ___             _      
        //  | |/ /___ _  _| |__  ___  __ _ _ _ __| | | __|_ _____ _ _| |_ ___
        //  | ' </ -_) || | '_ \/ _ \/ _` | '_/ _` | | _|\ V / -_) ' \  _(_-<
        //  |_|\_\___|\_, |_.__/\___/\__,_|_| \__,_| |___|\_/\___|_||_\__/__/
        //            |__/                                                           
        // -----------------------------------------------------------------      
        

        DisplayHandle keyFocused = getCore().getKeyboardFocusedObject();
        if (auto focusedObject = keyFocused)
        {
            if (sdlEvent.type == SDL_EVENT_KEY_DOWN)
            {
                // Create a KeyDown event
                auto keyDownEvent = std::make_unique<Event>(
                    EventType::KeyDown,
                    focusedObject,
                    getCore().getElapsedTime()
                );
                keyDownEvent->sdlEvent = sdlEvent;
                keyDownEvent->setScanCode(sdlEvent.key.scancode);
                keyDownEvent->setKeycode(sdlEvent.key.key);
                keyDownEvent->setKeymod(sdlEvent.key.mod);
                keyDownEvent->setAsciiCode(SDL_Utils::keyToAscii(sdlEvent.key.key, sdlEvent.key.mod));
                addEvent(std::move(keyDownEvent));
            }

            if (sdlEvent.type == SDL_EVENT_KEY_UP)
            {
                // Create a KeyUp event
                auto keyUpEvent = std::make_unique<Event>(
                    EventType::KeyUp,
                    focusedObject,
                    getCore().getElapsedTime()
                );                
                keyUpEvent->sdlEvent = sdlEvent;
                keyUpEvent->setScanCode(sdlEvent.key.scancode);
                keyUpEvent->setKeycode(sdlEvent.key.key);
                keyUpEvent->setKeymod(sdlEvent.key.mod);
                keyUpEvent->setAsciiCode(SDL_Utils::keyToAscii(sdlEvent.key.key, sdlEvent.key.mod));
                addEvent(std::move(keyUpEvent));
            }
        }


        // -----------------------------------------------------------------
        //         __  __                    ___             _      
        //        |  \/  |___ _  _ ___ ___  | __|_ _____ _ _| |_ ___
        //        | |\/| / _ \ || (_-</ -_) | _|\ V / -_) ' \  _(_-<
        //        |_|  |_\___/\_,_/__/\___| |___|\_/\___|_||_\__/__/
        //                  
        // -----------------------------------------------------------------   
        
        // Mouse Statics:
        static DisplayHandle s_lastMouseDownObject = nullptr;

        // Handle mouse button up events
        if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_UP) 
        {

// INFO("EventManager::Queue_SDL_Event: Mouse Button Up event received... at window X=" << sdlEvent.button.x << " Y=" << sdlEvent.button.y);

            // Convert to stage coordinates for event payloads and hit-testing
            // Use logical stage coordinates (scale by pixel dimensions)
            float mX = static_cast<float>(sdlEvent.motion.x);
            float mY = static_cast<float>(sdlEvent.motion.y);
            auto buttonEvent = std::make_unique<Event>(EventType::MouseButtonUp, node, getCore().getElapsedTime());
            buttonEvent->sdlEvent = sdlEvent;
            buttonEvent->mouse_x = mX;
            buttonEvent->mouse_y = mY;
            buttonEvent->button = sdlEvent.button.button;
            buttonEvent->clickCount = sdlEvent.button.clicks;
            buttonEvent->setTarget(topObject);
            buttonEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            addEvent(std::move(buttonEvent));
            // find the top object under the mouse
            DisplayHandle topObject = findTopObjectUnderMouse(node);

            // Only fire MouseClick if down and up on the same object
            if (s_lastMouseDownObject == topObject)
            {
                // Single Click Event
                if (sdlEvent.button.clicks == 1) 
                {
                    DisplayHandle top = topObject;
                    IDisplayObject* topNode = dynamic_cast<IDisplayObject*>(top.get());
                    if (topNode && topNode->isTabEnabled())
                        topNode->setKeyboardFocus();
                    auto clickEvent = std::make_unique<Event>(EventType::MouseClick, node, getCore().getElapsedTime());
                    clickEvent->sdlEvent = sdlEvent;
                    clickEvent->mouse_x = mX;
                    clickEvent->mouse_y = mY;
                    clickEvent->button = sdlEvent.button.button;
                    clickEvent->clickCount = sdlEvent.button.clicks;
                    clickEvent->setTarget(topObject);
                    clickEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
                    addEvent(std::move(clickEvent));
                // Double Click Events (Multiple Click Events)
                } 
                else if (sdlEvent.button.clicks > 1) 
                {
                    auto doubleClickEvent = std::make_unique<Event>(EventType::MouseDoubleClick, node, getCore().getElapsedTime());
                    doubleClickEvent->sdlEvent = sdlEvent;
                    doubleClickEvent->mouse_x = mX;
                    doubleClickEvent->mouse_y = mY;
                    doubleClickEvent->button = sdlEvent.button.button;
                    doubleClickEvent->clickCount = sdlEvent.button.clicks;
                    doubleClickEvent->setTarget(topObject);
                    doubleClickEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
                    addEvent(std::move(doubleClickEvent));
                }
            }
        }

        // Handle mouse button down events
        if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN) 
        {

// INFO("EventManager::Queue_SDL_Event: Mouse Button Down event received... at window X=" << sdlEvent.button.x << " Y=" << sdlEvent.button.y);

            // Convert to stage coordinates (logical coords)
            float mX = static_cast<float>(sdlEvent.motion.x);
            float mY = static_cast<float>(sdlEvent.motion.y);
            auto buttonDownEvent = std::make_unique<Event>(EventType::MouseButtonDown, node, getCore().getElapsedTime());
            buttonDownEvent->sdlEvent = sdlEvent;
            buttonDownEvent->mouse_x = mX;
            buttonDownEvent->mouse_y = mY;
            buttonDownEvent->button = sdlEvent.button.button;
            buttonDownEvent->clickCount = sdlEvent.button.clicks;
            buttonDownEvent->setTarget(topObject);
            buttonDownEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            s_lastMouseDownObject = topObject; // record the topObject under mouse
            addEvent(std::move(buttonDownEvent));
        }

        // Handle mouse wheel events
        if (sdlEvent.type == SDL_EVENT_MOUSE_WHEEL) 
        {
            auto wheelEvent = std::make_unique<Event>(EventType::MouseWheel, topObject, getCore().getElapsedTime());
            wheelEvent->wheelX = sdlEvent.wheel.x;
            wheelEvent->wheelY = sdlEvent.wheel.y;
            // Also include the mouse position in stage coords for listeners
            wheelEvent->mouse_x = static_cast<float>(sdlEvent.wheel.mouse_x) / (getCore().getPixelWidth() != 0.0f ? getCore().getPixelWidth() : 1.0f);
            wheelEvent->mouse_y = static_cast<float>(sdlEvent.wheel.mouse_y) / (getCore().getPixelHeight() != 0.0f ? getCore().getPixelHeight() : 1.0f);
            wheelEvent->setTarget(topObject);
            wheelEvent->setCurrentTarget(topObject); // Set currentTarget
            wheelEvent->setRelatedTarget(topObject); // Set relatedTarget
            wheelEvent->button = sdlEvent.wheel.direction; // Use direction as button for wheel events
            wheelEvent->clickCount = 0; // No click count for wheel events
            wheelEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            addEvent(std::move(wheelEvent));
        }

        // Handle mouse motion events
        static DisplayHandle lastHoveredObject = nullptr;
        if (sdlEvent.type == SDL_EVENT_MOUSE_MOTION) 
        {
            // Convert to stage coordinates for hit-testing and event payloads
            // Convert to stage coordinates (logical coords)
            float mX = static_cast<float>(sdlEvent.motion.x);
            float mY = static_cast<float>(sdlEvent.motion.y);
            DisplayHandle currentHoveredObject = findTopObjectUnderMouse(node, draggedObject);

            // Dispatch the motion event
            auto motionEvent = std::make_unique<Event>(EventType::MouseMove, currentHoveredObject, getCore().getElapsedTime());
            motionEvent->mouse_x = mX;
            motionEvent->mouse_y = mY;
            motionEvent->setCurrentTarget(currentHoveredObject);
            motionEvent->setTarget(topObject);
            motionEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            addEvent(std::move(motionEvent));

            // Dispatch "leave" event if the mouse left the last hovered object
            if (lastHoveredObject && lastHoveredObject != currentHoveredObject) 
            {
                auto leaveEvent = std::make_unique<Event>(EventType::MouseLeave, lastHoveredObject, getCore().getElapsedTime());
                leaveEvent->mouse_x = mX;
                leaveEvent->mouse_y = mY;
                leaveEvent->setCurrentTarget(lastHoveredObject);
                leaveEvent->setRelatedTarget(currentHoveredObject);
                leaveEvent->setTarget(lastHoveredObject);
                leaveEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
                addEvent(std::move(leaveEvent));
            }

            // Dispatch "enter" event if the mouse entered a new object
            if (currentHoveredObject != lastHoveredObject) 
            {
                auto enterEvent = std::make_unique<Event>(EventType::MouseEnter, currentHoveredObject, getCore().getElapsedTime());
                enterEvent->mouse_x = mX;
                enterEvent->mouse_y = mY;
                enterEvent->setCurrentTarget(currentHoveredObject);
                enterEvent->setRelatedTarget(lastHoveredObject);
                enterEvent->setTarget(currentHoveredObject);
                enterEvent->fElapsedTime =  getCore().getElapsedTime(); // Set elapsed time for the event
                addEvent(std::move(enterEvent));
            }

            // Update the last hovered object
            lastHoveredObject = currentHoveredObject;
        }

        // Special Case: Track window leave and enter events
        SDL_CaptureMouse(true);
        static SDL_Window* focusedWindow = SDL_GetMouseFocus();
        SDL_Window* currentWindow = SDL_GetMouseFocus();
        // Convert to stage coordinates (renderer/logical coords)
        float mX = static_cast<float>(sdlEvent.motion.x);
        float mY = static_cast<float>(sdlEvent.motion.y);
        DisplayHandle currentHoveredObject = findTopObjectUnderMouse(node, draggedObject);

        // Check if the focused window has changed
        if (focusedWindow != currentWindow || isDragging) 
        {
            if (currentWindow) 
            {
                // std::cout << CLR::GREEN << "Mouse entered window" << CLR::RESET << std::endl;  
                // Handle mouse enter logic here
                DisplayHandle stageObject = getFactory().getStageHandle();
                auto enterEvent = std::make_unique<Event>(EventType::MouseEnter, stageObject, getCore().getElapsedTime());
                enterEvent->mouse_x = mX;
                enterEvent->mouse_y = mY;
                enterEvent->setCurrentTarget(stageObject); // Current target is the stage
                enterEvent->setRelatedTarget(lastHoveredObject); // Related target is the last hovered object
                enterEvent->setTarget(stageObject); // Target is the stage
                enterEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
                addEvent(std::move(enterEvent));
            } 
            else 
            {
                // std::cout << CLR::GREEN << "Mouse left window" << CLR::RESET << std::endl;    
                // Handle mouse leave logic here
                DisplayHandle stageObject = getFactory().getStageHandle();
                auto leaveEvent = std::make_unique<Event>(EventType::MouseLeave, stageObject, getCore().getElapsedTime());
                leaveEvent->mouse_x = mX;
                leaveEvent->mouse_y = mY;
                leaveEvent->setCurrentTarget(stageObject); // Current target is the stage
                leaveEvent->setRelatedTarget(lastHoveredObject); // Related target is the last hovered object
                leaveEvent->setTarget(stageObject); // Target is the stage
                leaveEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
                addEvent(std::move(leaveEvent));
            }
            focusedWindow = currentWindow;
        }
        // Update the last hovered object
        lastHoveredObject = currentHoveredObject;

        // Static variable to track dragging state
        static float drag_offset_x = 0.0f;
        static float drag_offset_y = 0.0f;
        static float drag_start_x = -1.0f;  // negative coordinates are "non-seeded"
        static float drag_start_y = -1.0f;  // negative coordinates are "non-seeded"
        mX = sdlEvent.motion.x;
        mY = sdlEvent.motion.y;

        constexpr float DRAG_THRESHOLD = 5.0f;
        if (!isDragging)
        {
            if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_UP) 
            {
                // Reset drag start coordinates
                drag_start_x = -1.0f;
                drag_start_y = -1.0f;
            }
            // "seed" the drag operation
            if (drag_start_x < 0.0f || drag_start_y < 0.0f) 
            {
                if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                {
                    drag_start_x = mX;
                    drag_start_y = mY;
                }
            }
            else if (sdlEvent.type == SDL_EVENT_MOUSE_MOTION)
            {
                if (fabs(mX - drag_start_x) > DRAG_THRESHOLD || fabs(mY - drag_start_y) > DRAG_THRESHOLD) 
                {
                    // Start dragging if the mouse has moved significantly
                    isDragging = true;
                    draggedObject = topObject; // Set the dragged object to the top object under the mouse

                    if (!draggedObject.isValid()) 
                        ERROR("draggedObject is invalid.");

                    drag_offset_x = mX - draggedObject->getX();
                    drag_offset_y = mY - draggedObject->getY();

                    // Create Drag event
                    auto dragEvent = std::make_unique<Event>(EventType::Drag, node, getCore().getElapsedTime());
                    dragEvent->sdlEvent = sdlEvent;
                    dragEvent->mouse_x = mX;
                    dragEvent->mouse_y = mY;
                    dragEvent->button = sdlEvent.button.button;
                    dragEvent->dragOffsetX = drag_offset_x;
                    dragEvent->dragOffsetY = drag_offset_y;
                    dragEvent->setTarget(draggedObject);
                    dragEvent->setCurrentTarget(draggedObject);
                    DisplayHandle parent = draggedObject->getParent();
                    if (parent.isValid()) 
                        dragEvent->setRelatedTarget(parent);
                    dragEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event                    
                    addEvent(std::move(dragEvent)); // Dispatch the Drag event                    
                    isDragging = true; // Set dragging state to true
                }                    
            }
        }

        // Handle dragging events
        if (sdlEvent.type == SDL_EVENT_MOUSE_MOTION && isDragging) 
        {
            // Create Dragging event
            // IDisplayObject* draggedNode = dynamic_cast<IDisplayObject*>(draggedObject.get());
            DisplayHandle draggedNode = draggedObject;
            if (!draggedNode.isValid()) return;
            auto draggingEvent = std::make_unique<Event>(EventType::Dragging, node, getCore().getElapsedTime());
            draggingEvent->sdlEvent = sdlEvent;
            draggingEvent->mouse_x = mX;
            draggingEvent->mouse_y = mY;
            draggingEvent->button = sdlEvent.button.button;
            draggingEvent->dragOffsetX = drag_offset_x;
            draggingEvent->dragOffsetY = drag_offset_y;
            draggingEvent->setTarget(draggedObject);
            draggingEvent->setCurrentTarget(draggedObject);
            // IDisplayObject* parent = dynamic_cast<IDisplayObject*>(draggedNode->getParent().get());
            DisplayHandle parent = draggedNode->getParent();
            if (!parent.isValid()) return;
            draggingEvent->setRelatedTarget(parent->getParent());
            draggingEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            // Dispatch the Dragging event
            addEvent(std::move(draggingEvent));
        }

        // Handle drop events
        if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_UP && isDragging) 
        {
            // Create Drop event
            DisplayHandle topObject = findTopObjectUnderMouse(getFactory().getStageHandle(), draggedObject);
            auto dropEvent = std::make_unique<Event>(EventType::Drop, topObject, getCore().getElapsedTime());
            dropEvent->sdlEvent = sdlEvent;
            dropEvent->mouse_x = mX;
            dropEvent->mouse_y = mY;
            dropEvent->dragOffsetX = drag_offset_x;
            dropEvent->dragOffsetY = drag_offset_y;
            dropEvent->button = sdlEvent.button.button;
            dropEvent->setTarget(draggedObject);
            dropEvent->setCurrentTarget(draggedObject);
            dropEvent->setRelatedTarget(topObject);
            dropEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            // Dispatch the Drop event
            addEvent(std::move(dropEvent));            
            // Reset dragging state
            isDragging = false;
            draggedObject = nullptr;
            drag_offset_x = 0.0f;
            drag_offset_y = 0.0f;
            drag_start_x = -1.0f;
            drag_start_y = -1.0f;
        }
    }
*/



    // --- preprocessSDLEvent: Normalize coordinates and sync stage mouse position ------------
    // ⚙️ System Behavior:
    // Converts window coordinates into renderer logical coordinates, sets the stage’s
    // mouseX/mouseY values consistently for hit-testing and event dispatch.
    void EventManager::preprocessSDLEvent(SDL_Event& e, Stage* stage)
    {
        if (auto* renderer = getRenderer())
            SDL_ConvertEventToRenderCoordinates(renderer, &e);
        else
            ERROR("Unable to retrieve proper SDL Renderer.");

        stage->mouseX = static_cast<float>(e.motion.x);
        stage->mouseY = static_cast<float>(e.motion.y);

        if (e.type == SDL_EVENT_MOUSE_WHEEL)
        {
            stage->mouseX = static_cast<float>(e.wheel.mouse_x);
            stage->mouseY = static_cast<float>(e.wheel.mouse_y);
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
        return (type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                type == SDL_EVENT_MOUSE_BUTTON_UP   ||
                type == SDL_EVENT_MOUSE_MOTION      ||
                type == SDL_EVENT_MOUSE_WHEEL);
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

        // 🧩 Stage fallback if no focused object
        if (!focused || !focused.isValid())
            focused = getFactory().getStageHandle();

        if (!focused)
            return; // If still invalid (extremely rare), bail out safely.

        const float elapsed = core.getElapsedTime();

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
        if (e.type == SDL_EVENT_MOUSE_WHEEL) {
            mX = static_cast<float>(e.wheel.mouse_x);
            mY = static_cast<float>(e.wheel.mouse_y);
        } else {
            mX = static_cast<float>(e.motion.x);
            mY = static_cast<float>(e.motion.y);
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

                // Fire click/doubleclick if same object
                if (s_lastMouseDownObject == topObject)
                {
                    EventType& clickType = (e.button.clicks > 1)
                        ? EventType::MouseDoubleClick
                        : EventType::MouseClick;

                    auto clickEvent = std::make_unique<Event>(clickType, node, elapsed);
                    clickEvent->setSDL_Event(e);
                    clickEvent->mouse_x = mX;
                    clickEvent->mouse_y = mY;
                    clickEvent->button = e.button.button;
                    clickEvent->clickCount = e.button.clicks;
                    clickEvent->setTarget(topObject);
                    addEvent(std::move(clickEvent));
                }
                break;
            }

            case SDL_EVENT_MOUSE_WHEEL:
            {
                auto evt = std::make_unique<Event>(EventType::MouseWheel, topObject, elapsed);
                evt->setSDL_Event(e);
                evt->wheelX = e.wheel.x;
                evt->wheelY = e.wheel.y;
                evt->mouse_x = mX;
                evt->mouse_y = mY;
                evt->button = e.wheel.direction;
                evt->clickCount = 0;
                evt->setTarget(topObject);
                addEvent(std::move(evt));
                break;
            }

            case SDL_EVENT_MOUSE_MOTION:
            {
                auto evt = std::make_unique<Event>(EventType::MouseMove, topObject, elapsed);
                evt->setSDL_Event(e);
                evt->mouse_x = mX;
                evt->mouse_y = mY;
                evt->setTarget(topObject);
                addEvent(std::move(evt));
                break;
            }

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
        if (e.type != SDL_EVENT_MOUSE_MOTION)
            return;

        const float mX = static_cast<float>(e.motion.x);
        const float mY = static_cast<float>(e.motion.y);
        const float elapsed = getCore().getElapsedTime();

        DisplayHandle currentHovered = findTopObjectUnderMouse(node, draggedObject);

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
        static bool isDragging = false;
        static DisplayHandle draggedObject = nullptr;
        static float drag_offset_x = 0.0f;
        static float drag_offset_y = 0.0f;
        static float drag_start_x = -1.0f;
        static float drag_start_y = -1.0f;

        const float mX = static_cast<float>(e.motion.x);
        const float mY = static_cast<float>(e.motion.y);
        const float elapsed = getCore().getElapsedTime();
        constexpr float DRAG_THRESHOLD = 5.0f;

        // --- Reset on Mouse Up --------------------------------------------------------------
        if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            drag_start_x = -1.0f;
            drag_start_y = -1.0f;
        }

        // --- Seed drag start ---------------------------------------------------------------
        if (drag_start_x < 0.0f && drag_start_y < 0.0f && e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            drag_start_x = mX;
            drag_start_y = mY;
        }

        // --- Detect drag start --------------------------------------------------------------
        if (!isDragging && e.type == SDL_EVENT_MOUSE_MOTION)
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
            dropEvt->setTarget(draggedObject);
            dropEvt->setCurrentTarget(draggedObject);
            dropEvt->setRelatedTarget(dropTarget);
            addEvent(std::move(dropEvt));

            // Reset drag state
            isDragging = false;
            draggedObject = nullptr;
            drag_offset_x = drag_offset_y = 0.0f;
            drag_start_x = drag_start_y = -1.0f;
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

        DisplayHandle topObject = findTopObjectUnderMouse(node, draggedObject);
        getCore().setMouseHoveredObject(topObject);

        SDL_EventType sdlType = static_cast<SDL_EventType>(sdlEvent.type);

        // 🧭 Dispatch by category
        if (isWindowEvent(sdlType))          dispatchWindowEvents(sdlEvent);
        else if (isKeyboardEvent(sdlType))   dispatchKeyboardEvents(sdlEvent);
        else if (isMouseEvent(sdlType))      dispatchMouseEvents(sdlEvent, node, topObject);

        // Specialized systems
        updateHoverState(sdlEvent, node);
        dispatchWindowEnterLeave(sdlEvent, node);
        dispatchDragEvents(sdlEvent, node, topObject);
    } // END -- Queue_SDL_Event()


} // namespace SDOM