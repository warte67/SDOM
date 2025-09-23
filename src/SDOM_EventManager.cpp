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

#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>

namespace SDOM 
{
    // Static variables to track dragging state and the dragged object
    static bool isDragging = false;
    static IDisplayObject* draggedObject = nullptr;

    // void EventManager::addEvent(std::unique_ptr<Event> event) 
    // {
    //     std::lock_guard<std::mutex> lock(queueMutex);
    //     eventQueue.push(std::move(event));
    // }

    void EventManager::addEvent(std::unique_ptr<Event> event) 
    {
        // std::cout << "Event added: " << event->getTypeName() << std::endl;
        eventQueue.push(std::move(event));
    }

    void EventManager::DispatchQueuedEvents(IDisplayObject& rootNode) 
    {
        while (!eventQueue.empty()) 
        {
            auto event = std::move(eventQueue.front());
            eventQueue.pop();
            dispatchEvent(std::move(event), rootNode);
        }
    }

    void EventManager::dispatchEvent(std::unique_ptr<Event> event, IDisplayObject& rootNode) 
    {
        Stage* stage = getStage(); //Core::getInstance().getStage();
        if (!stage) return;

        // dispatch global "only" events
        if (event->getType().getGlobal())
        {
            std::cout << CLR::CYAN << "Dispatching global-only event: " << event->getTypeName() << CLR::NORMAL << std::endl;   

            // Dispatch to the stage
            event->setPhase(Event::Phase::Target);
            auto eventCopy = std::make_unique<Event>(*event);
            eventCopy->setPhase(Event::Phase::Target);
            dispatchEventToAllNodesOnStage(std::move(event), *stage);
            dispatchEventToAllEventListenersOnStage(std::move(eventCopy), *stage);
            return;
        }

        // dispatch event listener only events
        if (!event->getType().getGlobal()   && !event->getType().getTargetOnly() &&
            !event->getType().getCaptures() && !event->getType().getBubbles())
        {
            // Dispatch to the active stage
            auto eventCopy = std::make_unique<Event>(*event);
            eventCopy->setPhase(Event::Phase::Target);
            dispatchEventToAllEventListenersOnStage(std::move(eventCopy), *stage);
            return;
        } 

        // Capture Phase (if this event is even allowed to capture)
        if (event->getType().getCaptures() && !event->getType().getTargetOnly())
        {
            event->setPhase(Event::Phase::Capture);
            auto capturePhase = [&](IDisplayObject& rootNode, IDisplayObject& targetNode) -> void 
            {
                std::vector<IDisplayObject*> nodeStack;
                // Build the stack from target to root
                IDisplayObject* currentNode = &targetNode;
                while (currentNode && currentNode != &rootNode) 
                {
                    currentNode = dynamic_cast<IDisplayObject*>(currentNode->getParent().get());
                    if (currentNode) 
                        nodeStack.push_back(currentNode);
               }
                // Process nodes in reverse order (from root to target)
                while (!nodeStack.empty()) 
                {
                    IDisplayObject* node = nodeStack.back();
                    nodeStack.pop_back();

                    // Skip nodes that are hidden
                    if (node->isHidden()) 
                        continue;

                    // Dispatch the event
                    event->setPhase(Event::Phase::Capture);
                    event->setCurrentTarget(node); // Set currentTarget
                    if (node->isEnabled()) 
                    {
                        node->triggerEventListeners(*event, true ); // Trigger capture listeners
                        if (!event->isPropagationStopped() && !event->isDefaultBehaviorDisabled()) 
                        {
                            node->onEvent(*event); // Call the default onEvent method
                        }
                    }
                    // Stop propagation if requested
                    if (event->isPropagationStopped()) 
                        break;
                }

            };        
            capturePhase(rootNode, *event->getTarget());

        } // end if is allowed to capture
      
        // Target Phase: Dispatch to the target only 
        event->setPhase(Event::Phase::Target);
        auto targetNode = event->getTarget();
        if (targetNode && targetNode->isEnabled()) 
        {
            event->setCurrentTarget(targetNode); // Set currentTarget
            targetNode->triggerEventListeners(*event, false ); // Trigger target listeners
            if (!event->isPropagationStopped() && !event->isDefaultBehaviorDisabled()) 
                targetNode->onEvent(*event); // Call the default onEvent method
        }

        // Bubbling Phase: Dispatch to the parent nodes
        if (event->getType().getBubbles() && !event->getType().getTargetOnly())
        {
            auto bubbleTraverse = [&](IDisplayObject& node, auto& self) -> void 
            {
                // Skip nodes that are hidden
                if (node.isHidden()) 
                    return;

                event->setPhase(Event::Phase::Bubbling);
                event->setCurrentTarget(&node); // Set currentTarget
                // Skip disabled nodes
                if (node.isEnabled()) 
                {
                    node.triggerEventListeners(*event, false ); // Trigger bubbling listeners
                    if (!event->isPropagationStopped() && !event->isDefaultBehaviorDisabled()) 
                        node.onEvent(*event); // Call the default onEvent method
                }
                if (node.getParent() && !event->isPropagationStopped()) 
                {
                    IDisplayObject* parent = dynamic_cast<IDisplayObject*>(node.getParent().get());
                    if (parent)
                        self(*parent, self);
                }
            };

            // Start bubbling from the parent of the target node
            if (event->getTarget() && event->getTarget()->getParent() && !event->isPropagationStopped()) 
            {
                IDisplayObject* parent = dynamic_cast<IDisplayObject*>(event->getTarget()->getParent().get());
                if (parent)
                    bubbleTraverse(*parent, bubbleTraverse);
            }            
        } // end if is allowed to bubble
    }


    void EventManager::dispatchEventToAllNodesOnStage(std::unique_ptr<Event> event, IDisplayObject& rootNode) 
    {
        std::function<void(IDisplayObject&, Event*)> dispatchToChildren =
            [&](IDisplayObject& node, Event* evt)
        {
            evt->setPhase(Event::Phase::Target);
            evt->setTarget(&node);
            node.onEvent(*evt);
            for (const auto& child : node.getChildren())
            {
                IDisplayObject* childPtr = dynamic_cast<IDisplayObject*>(child.get());
                if (childPtr)
                    dispatchToChildren(*childPtr, evt);
            }
        };
        dispatchToChildren(rootNode, event.get());
    }

    
    void EventManager::dispatchEventToAllEventListenersOnStage(std::unique_ptr<Event> event, IDisplayObject& rootNode) 
    {
        std::function<void(IDisplayObject&, Event*)> dispatchToChildren =
            [&](IDisplayObject& node, Event* evt)
        {
            evt->setPhase(Event::Phase::Target);
            evt->setTarget(&node);
            node.triggerEventListeners(*evt, false );
            for (const auto& child : node.getChildren()) 
            {
                IDisplayObject* childPtr = dynamic_cast<IDisplayObject*>(child.get());
                if (childPtr)
                    dispatchToChildren(*childPtr, evt);
            }
        };
        dispatchToChildren(rootNode, event.get());
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
        return SDL_PointInRect(&mousePoint, &targetRect);
    }

    IDisplayObject& EventManager::findTopObjectUnderMouse(IDisplayObject& rootNode, IDisplayObject* excludeNode) const 
    {
        IDisplayObject* targetNode = nullptr; // Reset targetNode
        int maxDepth = -1;    // Track the maximum depth of the target node

        auto traverse = [&](IDisplayObject& node, int depth, auto& self) -> void 
        {
            if (node.isHidden() || &node == excludeNode)
                return;

            // Only consider node as a candidate if it is clickable
            if (node.isClickable() && isMouseWithinBounds(node)) 
            {
                if (!targetNode || depth > maxDepth || (depth == maxDepth && node.getZOrder() > targetNode->getZOrder())) 
                {
                    targetNode = &node;
                    maxDepth = depth;
                }
            }
            // Always traverse children, even if node is not clickable
            for (const auto& child : node.getChildren()) 
            {
                IDisplayObject* childPtr = dynamic_cast<IDisplayObject*>(child.get());
                if (childPtr) 
                    self(*childPtr, depth + 1, self);
            }
        };
        traverse(rootNode, 0, traverse);
        // Default to rootNode if no other node is targeted
        if (!targetNode) 
            targetNode = &rootNode; 
        return *targetNode;
    }
    
    void EventManager::Queue_SDL_Event(IDisplayObject& node, SDL_Event& sdlEvent) 
    {
        // Convert SDL_Event to SDOM::Event and add to the queue

        // Skip processing if the node is not enabled or is hidden
        if (!node.isEnabled() || node.isHidden())
            return;
        // fetch the stage instance
        Stage* stage = getStage(); //Core::getInstance().getStage();
        if (!stage) return;


        // find the top object under the mouse
        IDisplayObject& mouseHoveredObject = findTopObjectUnderMouse(node);
        ResourceHandle hoveredResource = getFactory().getResourceHandle(mouseHoveredObject.getName());
        getCore().setMouseHoveredObject(hoveredResource);
        // SDOM::Core::getInstance().setMouseHoveredObject(&mouseHoveredObject);


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

        // Set the mouse position in the stage (conceptual code)
        // This code will need mutex to avoid data races between multiple
        // stage windows.
        stage->mouseX = static_cast<float>(sdlEvent.motion.x);
        stage->mouseY = static_cast<float>(sdlEvent.motion.y);
        // correct for the difference in the mouse wheel events
        if (sdlEvent.type == SDL_EVENT_MOUSE_WHEEL) 
        {
            stage->mouseX = sdlEvent.wheel.mouse_x;
            stage->mouseY = sdlEvent.wheel.mouse_y;
        }

        // Find the top object that is under the mouse cursor
        // Modify findTopObjectUnderMouse to exclude the dragged object
        IDisplayObject& topObject = findTopObjectUnderMouse(node, draggedObject);

        std::cout << CLR::GREEN << "Top Object under mouse: " << topObject.getName() 
                  << " (MouseX: " << stage->mouseX 
                  << ", MouseY: " << stage->mouseY << ")" << CLR::NORMAL << std::endl;


        // alias the SDL_EventType for ease of use:
        SDL_EventType sdl_type = static_cast<SDL_EventType>(sdlEvent.type);

        // TEST SDL EVENTS        
        if (sdl_type != SDL_EVENT_MOUSE_MOTION)
        {
            std::cout << CLR::YELLOW << "EVENT: " << SDL_Utils::eventTypeToString(sdl_type) << CLR::NORMAL << std::endl;
        }


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
                auto unfocusEvent = std::make_unique<Event>(EventType::FocusLost, getStage());
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
                auto focusEvent = std::make_unique<Event>(EventType::FocusGained, getStage());
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
                auto resizeEvent = std::make_unique<Event>(EventType::Resize, getStage());
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
                auto showEvent = std::make_unique<Event>(EventType::Show, getStage());
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
                auto hideEvent = std::make_unique<Event>(EventType::Hide, getStage());
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
                auto fullscreenEvent = std::make_unique<Event>(EventType::EnterFullscreen, getStage());
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
                auto leaveFullscreenEvent = std::make_unique<Event>(EventType::LeaveFullscreen, getStage());
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
        

        ResourceHandle keyFocused = getCore().getKeyboardFocusedObject();
        if (auto focusedObject = keyFocused)
        {
            if (sdlEvent.type == SDL_EVENT_KEY_DOWN)
            {
                // Create a KeyDown event
                auto keyDownEvent = std::make_unique<Event>(
                    EventType::KeyDown,
                    dynamic_cast<IDisplayObject*>(focusedObject.get()),
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
                // auto keyUpEvent = std::make_unique<Event>(EventType::KeyUp, focusedObject.get(), getCore().getElapsedTime());
                auto keyUpEvent = std::make_unique<Event>(
                    EventType::KeyUp,
                    dynamic_cast<IDisplayObject*>(focusedObject.get()),
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
        static IDisplayObject* s_lastMouseDownObject = nullptr;

        // Handle mouse button up events
        if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_UP) 
        {
            float mX = sdlEvent.motion.x;
            float mY = sdlEvent.motion.y;
            auto buttonEvent = std::make_unique<Event>(EventType::MouseButtonUp, &node, getCore().getElapsedTime());
            buttonEvent->sdlEvent = sdlEvent;
            buttonEvent->mouse_x = mX;
            buttonEvent->mouse_y = mY;
            buttonEvent->button = sdlEvent.button.button;
            buttonEvent->clickCount = sdlEvent.button.clicks;
            buttonEvent->setTarget(&topObject);
            buttonEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            addEvent(std::move(buttonEvent));
            // find the top object under the mouse
            IDisplayObject& topObject = findTopObjectUnderMouse(node);

            // Only fire MouseClick if down and up on the same object
            if (s_lastMouseDownObject == &topObject)
            {
                // Single Click Event
                if (sdlEvent.button.clicks == 1) 
                {
                    if (topObject.isTabEnabled())
                    {
                        topObject.setKeyboardFocus();
                    }
                    auto clickEvent = std::make_unique<Event>(EventType::MouseClick, &node, getCore().getElapsedTime());
                    clickEvent->sdlEvent = sdlEvent;
                    clickEvent->mouse_x = mX;
                    clickEvent->mouse_y = mY;
                    clickEvent->button = sdlEvent.button.button;
                    clickEvent->clickCount = sdlEvent.button.clicks;
                    clickEvent->setTarget(&topObject);
                    clickEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
                    addEvent(std::move(clickEvent));
                // Double Click Events (Multiple Click Events)
                } 
                else if (sdlEvent.button.clicks > 1) 
                {
                    auto doubleClickEvent = std::make_unique<Event>(EventType::MouseDoubleClick, &node, getCore().getElapsedTime());
                    doubleClickEvent->sdlEvent = sdlEvent;
                    doubleClickEvent->mouse_x = mX;
                    doubleClickEvent->mouse_y = mY;
                    doubleClickEvent->button = sdlEvent.button.button;
                    doubleClickEvent->clickCount = sdlEvent.button.clicks;
                    doubleClickEvent->setTarget(&topObject);
                    doubleClickEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
                    addEvent(std::move(doubleClickEvent));
                }
            }
        }

        // Handle mouse button down events
        if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN) 
        {
            float mX = sdlEvent.motion.x;
            float mY = sdlEvent.motion.y;
            auto buttonDownEvent = std::make_unique<Event>(EventType::MouseButtonDown, &node, getCore().getElapsedTime());
            buttonDownEvent->sdlEvent = sdlEvent;
            buttonDownEvent->mouse_x = mX;
            buttonDownEvent->mouse_y = mY;
            buttonDownEvent->button = sdlEvent.button.button;
            buttonDownEvent->clickCount = sdlEvent.button.clicks;
            buttonDownEvent->setTarget(&topObject);
            buttonDownEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            s_lastMouseDownObject = &topObject; // record the topObject under mouse
            addEvent(std::move(buttonDownEvent));
        }

        // Handle mouse wheel events
        if (sdlEvent.type == SDL_EVENT_MOUSE_WHEEL) 
        {
            auto wheelEvent = std::make_unique<Event>(EventType::MouseWheel, &topObject, getCore().getElapsedTime());
            wheelEvent->wheelX = sdlEvent.wheel.x;
            wheelEvent->wheelY = sdlEvent.wheel.y;
            wheelEvent->setTarget(&topObject);
            wheelEvent->setCurrentTarget(&topObject); // Set currentTarget
            wheelEvent->setRelatedTarget(&topObject); // Set relatedTarget
            wheelEvent->button = sdlEvent.wheel.direction; // Use direction as button for wheel events
            wheelEvent->clickCount = 0; // No click count for wheel events
            wheelEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            addEvent(std::move(wheelEvent));
        }

        // Handle mouse motion events
        static IDisplayObject* lastHoveredObject = nullptr;
        if (sdlEvent.type == SDL_EVENT_MOUSE_MOTION) 
        {
            float mX = sdlEvent.motion.x;
            float mY = sdlEvent.motion.y;
            IDisplayObject& currentHoveredObject = findTopObjectUnderMouse(node, draggedObject);

            // Dispatch the motion event
            auto motionEvent = std::make_unique<Event>(EventType::MouseMove, &currentHoveredObject, getCore().getElapsedTime());
            motionEvent->mouse_x = mX;
            motionEvent->mouse_y = mY;
            motionEvent->setCurrentTarget(&currentHoveredObject);
            motionEvent->setTarget(&topObject);
            motionEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            addEvent(std::move(motionEvent));

            // Dispatch "leave" event if the mouse left the last hovered object
            if (lastHoveredObject && lastHoveredObject != &currentHoveredObject) 
            {
                auto leaveEvent = std::make_unique<Event>(EventType::MouseLeave, lastHoveredObject, getCore().getElapsedTime());
                leaveEvent->mouse_x = mX;
                leaveEvent->mouse_y = mY;
                leaveEvent->setCurrentTarget(lastHoveredObject);
                leaveEvent->setRelatedTarget(&currentHoveredObject);
                leaveEvent->setTarget(lastHoveredObject);
                leaveEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
                addEvent(std::move(leaveEvent));
            }

            // Dispatch "enter" event if the mouse entered a new object
            if (&currentHoveredObject != lastHoveredObject) 
            {
                auto enterEvent = std::make_unique<Event>(EventType::MouseEnter, &currentHoveredObject, getCore().getElapsedTime());
                enterEvent->mouse_x = mX;
                enterEvent->mouse_y = mY;
                enterEvent->setCurrentTarget(&currentHoveredObject);
                enterEvent->setRelatedTarget(lastHoveredObject);
                enterEvent->setTarget(&currentHoveredObject);
                enterEvent->fElapsedTime =  getCore().getElapsedTime(); // Set elapsed time for the event
                addEvent(std::move(enterEvent));
            }

            // Update the last hovered object
            lastHoveredObject = &currentHoveredObject;
        }

        // Special Case: Track window leave and enter events
        SDL_CaptureMouse(true);
        static SDL_Window* focusedWindow = SDL_GetMouseFocus();
        SDL_Window* currentWindow = SDL_GetMouseFocus();
        float mX = sdlEvent.motion.x;
        float mY = sdlEvent.motion.y;
        IDisplayObject& currentHoveredObject = findTopObjectUnderMouse(node, draggedObject);

        // Check if the focused window has changed
        if (focusedWindow != currentWindow || isDragging) 
        {
            if (currentWindow) 
            {
                // std::cout << CLR::GREEN << "Mouse entered window" << CLR::RESET << std::endl;  
                // Handle mouse enter logic here
                IDisplayObject* stageObject = getStage();
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
                IDisplayObject* stageObject = getStage();
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
        lastHoveredObject = &currentHoveredObject;

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
                    draggedObject = &topObject; // Set the dragged object to the top object under the mouse
                    drag_offset_x = mX - draggedObject->getX();
                    drag_offset_y = mY - draggedObject->getY();
                    // Create Drag event
                    auto dragEvent = std::make_unique<Event>(EventType::Drag, &node, getCore().getElapsedTime());
                    dragEvent->sdlEvent = sdlEvent;
                    dragEvent->mouse_x = mX;
                    dragEvent->mouse_y = mY;
                    dragEvent->button = sdlEvent.button.button;
                    dragEvent->dragOffsetX = drag_offset_x;
                    dragEvent->dragOffsetY = drag_offset_y;
                    dragEvent->setTarget(draggedObject);
                    dragEvent->setCurrentTarget(draggedObject);
                    IDisplayObject* parent = dynamic_cast<IDisplayObject*>(draggedObject->getParent().get());
                    dragEvent->setRelatedTarget(parent);
                    dragEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
                    // Dispatch the Drag event
                    addEvent(std::move(dragEvent));
                    // Set dragging state to true
                    isDragging = true;
                }
            }
        }

        // Handle dragging events
        if (sdlEvent.type == SDL_EVENT_MOUSE_MOTION && isDragging) 
        {
            // Create Dragging event
            auto draggingEvent = std::make_unique<Event>(EventType::Dragging, &node, getCore().getElapsedTime());
            draggingEvent->sdlEvent = sdlEvent;
            draggingEvent->mouse_x = mX;
            draggingEvent->mouse_y = mY;
            draggingEvent->button = sdlEvent.button.button;
            draggingEvent->dragOffsetX = drag_offset_x;
            draggingEvent->dragOffsetY = drag_offset_y;
            draggingEvent->setTarget(draggedObject);
            draggingEvent->setCurrentTarget(draggedObject);
            IDisplayObject* parent = dynamic_cast<IDisplayObject*>(draggedObject->getParent().get());
            draggingEvent->setRelatedTarget(parent);
            draggingEvent->fElapsedTime = getCore().getElapsedTime(); // Set elapsed time for the event
            // Dispatch the Dragging event
            addEvent(std::move(draggingEvent));
        }

        // Handle drop events
        if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_UP && isDragging) 
        {
            // Create Drop event
            Stage* stage = getStage();
            IDisplayObject& topObject = findTopObjectUnderMouse(*stage, draggedObject);
            auto dropEvent = std::make_unique<Event>(EventType::Drop, &node, getCore().getElapsedTime());
            dropEvent->sdlEvent = sdlEvent;
            dropEvent->mouse_x = mX;
            dropEvent->mouse_y = mY;
            dropEvent->dragOffsetX = drag_offset_x;
            dropEvent->dragOffsetY = drag_offset_y;
            dropEvent->button = sdlEvent.button.button;
            dropEvent->setTarget(draggedObject);
            dropEvent->setCurrentTarget(draggedObject);
            dropEvent->setRelatedTarget(&topObject);
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

} // namespace SDOM