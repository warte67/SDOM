/***  SDOM_Event.cpp  ****************************
 *    ___ ___   ___  __  __   ___             _                  
 *   / __|   \ / _ \|  \/  | | __|_ _____ _ _| |_   __ _ __ _ __ 
 *   \__ \ |) | (_) | |\/| | | _|\ V / -_) ' \  _|_/ _| '_ \ '_ \
 *   |___/___/ \___/|_|  |_|_|___|\_/\___|_||_\__(_)__| .__/ .__/
 *                        |___|                       |_|  |_|     
 *  
 * The SDOM_Event class encapsulates the event system within the SDOM framework, 
 * providing a unified interface for handling user input and system events such 
 * as keyboard, mouse, and custom application events. It supports event propagation 
 * phases (capture, target, bubbling), payload data for custom event information, 
 * and thread-safe access to event properties. The class integrates closely with 
 * SDL3’s event system, allowing seamless translation between SDL events and SDOM’s 
 * object-oriented event model. SDOM_Event is designed for flexibility and 
 * extensibility, enabling robust event-driven programming and interactive GUI 
 * development within SDOM-based applications.
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
#include <SDOM/SDOM_Event.hpp>

namespace SDOM
{
    Event::Event(EventType type, IDisplayObject* target, float fElapsedTime)
            : type(type), target(target), currentPhase(Phase::Capture), fElapsedTime(fElapsedTime)
    {
        // Constructor implementation
        useCapture = true; // Default to using capture phase

        // Registration of properties and commands

        // Register properties (getter/setter pairs)
        registerProperty("type",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getTypeName(); },
            nullptr); // read-only
        registerProperty("phase",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getPhaseString(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                auto& e = static_cast<Event&>(obj);
                std::string s = val.get<std::string>();
                if (s == "Capture") e.setPhase(Phase::Capture);
                else if (s == "Target") e.setPhase(Phase::Target);
                else if (s == "Bubbling") e.setPhase(Phase::Bubbling);
                return obj;
            });
        registerProperty("target",
            [](const IDataObject& obj) { return reinterpret_cast<uintptr_t>(static_cast<const Event&>(obj).getTarget()); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                auto& e = static_cast<Event&>(obj);
                e.setTarget(reinterpret_cast<IDisplayObject*>(val.get<uintptr_t>()));
                return obj;
            });
        registerProperty("currentTarget",
            [](const IDataObject& obj) { return reinterpret_cast<uintptr_t>(static_cast<const Event&>(obj).getCurrentTarget()); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                auto& e = static_cast<Event&>(obj);
                e.setCurrentTarget(reinterpret_cast<IDisplayObject*>(val.get<uintptr_t>()));
                return obj;
            });
        registerProperty("relatedTarget",
            [](const IDataObject& obj) { return reinterpret_cast<uintptr_t>(static_cast<const Event&>(obj).getRelatedTarget()); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                auto& e = static_cast<Event&>(obj);
                e.setRelatedTarget(reinterpret_cast<IDisplayObject*>(val.get<uintptr_t>()));
                return obj;
            });
        registerProperty("propagationStopped",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).isPropagationStopped(); },
            nullptr); // read-only
        registerProperty("disableDefaultBehavior",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).isDefaultBehaviorDisabled(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setDisableDefaultBehavior(val.get<bool>());
                return obj;
            });
        registerProperty("useCapture",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getUseCapture(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setUseCapture(val.get<bool>());
                return obj;
            });
        registerProperty("elapsedTime",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getElapsedTime(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setElapsedTime(val.get<float>());
                return obj;
            });
        registerProperty("payload",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getPayload(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setPayload(val);
                return obj;
            });
        registerProperty("mouse_x",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getMouseX(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setMouseX(val.get<float>());
                return obj;
            });

        registerProperty("mouse_y",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getMouseY(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setMouseY(val.get<float>());
                return obj;
            });

        registerProperty("wheelX",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getWheelX(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setWheelX(val.get<float>());
                return obj;
            });

        registerProperty("wheelY",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getWheelY(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setWheelY(val.get<float>());
                return obj;
            });

        registerProperty("dragOffsetX",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getDragOffsetX(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setDragOffsetX(val.get<float>());
                return obj;
            });

        registerProperty("dragOffsetY",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getDragOffsetY(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setDragOffsetY(val.get<float>());
                return obj;
            });

        registerProperty("clickCount",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getClickCount(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setClickCount(val.get<int>());
                return obj;
            });

        registerProperty("button",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getButton(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setButton(val.get<uint8_t>());
                return obj;
            });

        // Register Keyboard Event Properties

        registerProperty("scancode",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getScanCode(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setScanCode(val.get<SDL_Scancode>());
                return obj;
            });

        registerProperty("keycode",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getKeycode(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setKeycode(val.get<SDL_Keycode>());
                return obj;
            });

        registerProperty("keymod",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getKeymod(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setKeymod(val.get<Uint16>());
                return obj;
            });

        registerProperty("asciiCode",
            [](const IDataObject& obj) { return static_cast<const Event&>(obj).getAsciiCode(); },
            [](IDataObject& obj, const SDOM::Json& val) -> IDataObject& {
                static_cast<Event&>(obj).setAsciiCode(val.get<int>());
                return obj;
            });

        // Register commands (mutating actions)
        registerCommand("stopPropagation",
            [](IDataObject& obj, const Json&) { static_cast<Event&>(obj).stopPropagation(); });
        registerCommand("setPhase",
            [](IDataObject& obj, const Json& val) {
                auto& e = static_cast<Event&>(obj);
                std::string s = val.get<std::string>();
                if (s == "Capture") e.setPhase(Phase::Capture);
                else if (s == "Target") e.setPhase(Phase::Target);
                else if (s == "Bubbling") e.setPhase(Phase::Bubbling);
            });
        registerCommand("setTarget",
            [](IDataObject& obj, const Json& val) {
                static_cast<Event&>(obj).setTarget(reinterpret_cast<IDisplayObject*>(val.get<uintptr_t>()));
            });
        registerCommand("setCurrentTarget",
            [](IDataObject& obj, const Json& val) {
                static_cast<Event&>(obj).setCurrentTarget(reinterpret_cast<IDisplayObject*>(val.get<uintptr_t>()));
            });
        registerCommand("setRelatedTarget",
            [](IDataObject& obj, const Json& val) {
                static_cast<Event&>(obj).setRelatedTarget(reinterpret_cast<IDisplayObject*>(val.get<uintptr_t>()));
            });
        registerCommand("setDisableDefaultBehavior",
            [](IDataObject& obj, const Json& val) {
                static_cast<Event&>(obj).setDisableDefaultBehavior(val.get<bool>());
            });
        registerCommand("setUseCapture",
            [](IDataObject& obj, const Json& val) {
                static_cast<Event&>(obj).setUseCapture(val.get<bool>());
            });
        registerCommand("setElapsedTime",
            [](IDataObject& obj, const Json& val) {
                static_cast<Event&>(obj).setElapsedTime(val.get<float>());
            });
        registerCommand("setPayload",
            [](IDataObject& obj, const Json& val) {
                static_cast<Event&>(obj).setPayload(val);
            });
    }

    // virtual methods
    bool Event::onInit()
    {
        // Perform any initialization logic specific to Event
        return true; // Return true if initialization succeeds
    }

    void Event::onQuit()
    {
        // Perform any cleanup logic specific to Event
    }


    EventType Event::getType() const { std::lock_guard<std::mutex> lock(eventMutex_); return type; }
    std::string Event::getTypeName() const { std::lock_guard<std::mutex> lock(eventMutex_); return type.getName(); }

    std::string Event::getPhaseString() const 
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        switch (currentPhase) 
        {
            case Phase::Capture:    return "Capture";
            case Phase::Target:     return "Target";
            case Phase::Bubbling:   return "Bubbling";
            default:                return "Unknown phase";
        }
    }    

    Event::Phase Event::getPhase() const 
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        return currentPhase;
    }

    Event& Event::setPhase(SDOM::Event::Phase phase) 
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        currentPhase = phase;
        return *this;
    }

    IDisplayObject* Event::getTarget() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return target;
    }
    Event& Event::setTarget(IDisplayObject* newTarget) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        target = newTarget;
        return *this;
    }

    IDisplayObject* Event::getCurrentTarget() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return currentTarget;
    }
    Event& Event::setCurrentTarget(IDisplayObject* newCurrentTarget) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        currentTarget = newCurrentTarget;
        return *this;
    }

    IDisplayObject* Event::getRelatedTarget() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return relatedTarget;
    }
    Event& Event::setRelatedTarget(IDisplayObject* newRelatedTarget) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        relatedTarget = newRelatedTarget;
        return *this;
    }

    bool Event::isPropagationStopped() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return propagationStopped;
    }
    Event& Event::stopPropagation() 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        propagationStopped = true; 
        return *this; 
    }

    bool Event::isDefaultBehaviorDisabled() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return disableDefaultBehavior; 
    }
    Event& Event::setDisableDefaultBehavior(bool disable) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        disableDefaultBehavior = disable; 
        return *this; 
    }

    bool Event::getUseCapture() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return useCapture; 
    }
    Event& Event::setUseCapture(bool useCapture) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        this->useCapture = useCapture; 
        return *this; 
    }

    float Event::getElapsedTime() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return fElapsedTime; 
    }
    Event& Event::setElapsedTime(float elapsedTime) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        fElapsedTime = elapsedTime; 
        return *this; 
    }

    SDL_Event Event::getSDL_Event() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return sdlEvent; 
    }
    Event& Event::setSDL_Event(const SDL_Event& sdlEvent) {
        this->sdlEvent = sdlEvent;
        return *this;
    }

    // Mouse event accessors
    float Event::getMouseX() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return mouse_x; 
    }
    Event& Event::setMouseX(float x) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        mouse_x = x; 
        return *this; 
    }

    float Event::getMouseY() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return mouse_y; 
    }
    Event& Event::setMouseY(float y) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        mouse_y = y; 
        return *this; 
    }

    float Event::getWheelX() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return wheelX; 
    }
    Event& Event::setWheelX(float x) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        wheelX = x; 
        return *this; 
    }

    float Event::getWheelY() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return wheelY; 
    }
    Event& Event::setWheelY(float y) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        wheelY = y; 
        return *this; 
    }

    float Event::getDragOffsetX() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return dragOffsetX; 
    }
    Event& Event::setDragOffsetX(float offsetX) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        dragOffsetX = offsetX; 
        return *this; 
    }

    float Event::getDragOffsetY() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return dragOffsetY; 
    }
    Event& Event::setDragOffsetY(float offsetY) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        dragOffsetY = offsetY; 
        return *this; 
    }

    int Event::getClickCount() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return clickCount; 
    }
    Event& Event::setClickCount(int count) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        clickCount = count; 
        return *this; 
    }

    uint8_t Event::getButton() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return button; 
    }
    Event& Event::setButton(uint8_t btn) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        button = btn; 
        return *this; 
    }


    // keyboard event accessors

    SDL_Scancode Event::getScanCode() const
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        return scancode_;
    }

    Event& Event::setScanCode(SDL_Scancode scancode)
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        scancode_ = scancode;
        return *this;
    }

    SDL_Keycode Event::getKeycode() const
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        return keycode_;
    }
    Event& Event::setKeycode(SDL_Keycode keycode)
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        keycode_ = keycode;
        return *this;
    }

    Uint16 Event::getKeymod() const
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        return keymod_;
    }   
    
    Event& Event::setKeymod(Uint16 keymod)
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        keymod_ = keymod;
        return *this;
    }


    int Event::getAsciiCode() const
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        return asciiCode_;
    }

    Event& Event::setAsciiCode(int asciiCode)
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        asciiCode_ = asciiCode;
        return *this;
    }

} // namespace SDOM