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

// #include <SDOM/SDOM_Handle.hpp>
#include <SDOM/SDOM_DomHandle.hpp>
#include <SDOM/SDOM_ResHandle.hpp>

#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_SDL_Utils.hpp>


namespace SDOM
{
    SDOM::Event::Event(EventType type, DomHandle target, float fElapsedTime)
        : type(type), target(target), currentPhase(Phase::Capture), fElapsedTime(fElapsedTime)
    {
        // Constructor implementation
        useCapture = true; // Default to using capture phase
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

    DomHandle Event::getTarget() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return target;
    }
    Event& Event::setTarget(const DomHandle newTarget) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        target = newTarget;
        return *this;
    }

    DomHandle Event::getCurrentTarget() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return currentTarget;
    }
    Event& Event::setCurrentTarget(const DomHandle newCurrentTarget) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        currentTarget = newCurrentTarget;
        return *this;
    }

    DomHandle Event::getRelatedTarget() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return relatedTarget;
    }
    Event& Event::setRelatedTarget(const DomHandle newRelatedTarget) 
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

// Lua registration implementation
void SDOM::Event::registerLua(sol::state_view lua)
{
    try {
        // Ensure EventType usertype exists in the state
        if (!lua["EventType"].valid()) {
            lua.new_usertype<EventType>("EventType", sol::no_constructor);
        }

        if (!lua["Event"].valid()) {
            lua.new_usertype<Event>("Event", sol::no_constructor,
                "dt", sol::property([](const Event& e) { return e.getElapsedTime(); }),
                "type", sol::property([](const Event& e) { return e.getTypeName(); }),
                "target", sol::property([](const Event& e) { return e.getTarget(); }),
                // sdl: returns a Lua table representation of the underlying SDL_Event (if any)
                "sdl", sol::property([](const Event& e, sol::this_state s) -> sol::object {
                    sol::state_view lua(s);
                    try {
                        // Convert the stored SDL_Event into a Lua table using SDL_Utils
                        sol::table tbl = SDL_Utils::eventToLuaTable(e.getSDL_Event(), lua);
                        return sol::object(lua, tbl);
                    } catch (...) {
                        // Return nil on any failure
                        return sol::object(lua, sol::lua_nil);
                    }
                }),
                "name", sol::property([](const Event& e) {
                    try {
                        DomHandle dh = e.getTarget();
                        if (dh) {
                            if (auto* obj = dh.get()) return obj->getName();
                            std::string hn = dh.getName();
                            if (!hn.empty()) return hn;
                        }
                    } catch (...) {}
                    return e.getTypeName();
                }),
                "getName", [](const Event& e) -> std::string {
                    DomHandle dh = e.getTarget();
                    if (dh) {
                        if (auto* obj = dh.get()) return obj->getName();
                        std::string hn = dh.getName();
                        if (!hn.empty()) return hn;
                    }
                    return e.getTypeName();
                },
                "getElapsedTime", &Event::getElapsedTime
                , sol::meta_function::pairs, [](const Event& e, sol::this_state ts) {
                    sol::state_view lua(ts);
                    sol::table t = lua.create_table();
                    try {
                        t["type"] = e.getTypeName();
                    } catch (...) {}
                    try { t["dt"] = e.getElapsedTime(); } catch (...) {}
                    try {
                        DomHandle dh = e.getTarget();
                        if (dh) {
                            if (auto* obj = dh.get()) {
                                t["target"] = obj;
                            } else {
                                t["target"] = dh.getName();
                            }
                        }
                    } catch (...) {}
                    // return the standard pairs iterator over the temp table
                    return std::make_tuple(lua["pairs"], t, sol::nil);
                }
            );
        }
    } catch (...) {
        // non-fatal
        WARNING("Failed to register Event with Lua");
    }
}