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
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>

#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_SDL_Utils.hpp>


namespace SDOM
{
    SDOM::Event::Event(EventType type, DisplayHandle target, float fElapsedTime)
        : type(type), target(target), currentPhase(Phase::Capture), fElapsedTime(fElapsedTime)
    {
        // Constructor implementation
        useCapture = true; // Default to using capture phase

        // Ensure payload is a valid Lua table so callers can safely set payload values
        try {
            // Attempt to acquire the Core-managed Lua state and create an empty table
            sol::state_view sv = SDOM::getLua();
            payload = sv.create_table();
        } catch (...) {
            // Non-fatal: if Lua isn't available yet, leave payload in its default state
            // and warn so debugging is easier.
            WARNING("Event: unable to initialize Lua payload table (Lua state not available yet)");
        }
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

    DisplayHandle Event::getTarget() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return target;
    }
    Event& Event::setTarget(const DisplayHandle newTarget) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        target = newTarget;
        return *this;
    }

    DisplayHandle Event::getCurrentTarget() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return currentTarget;
    }
    Event& Event::setCurrentTarget(const DisplayHandle newCurrentTarget) 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        currentTarget = newCurrentTarget;
        return *this;
    }

    DisplayHandle Event::getRelatedTarget() const 
    { 
        std::lock_guard<std::mutex> lock(eventMutex_);
        return relatedTarget;
    }
    Event& Event::setRelatedTarget(const DisplayHandle newRelatedTarget) 
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
        // Ensure EventType usertype is fully registered. Delegate to
        // EventType::registerLua so the canonical usertype (with properties)
        // is created exactly once and avoids partial registrations when
        // Event::registerLua is called earlier than EventType::registerLua.
        EventType::registerLua(lua);

        if (!lua["Event"].valid()) {
            lua.new_usertype<Event>("Event", sol::no_constructor,
                // existing convenience properties
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
                // name helpers
                "name", sol::property([](const Event& e) {
                    try {
                        DisplayHandle dh = e.getTarget();
                        // If there is an underlying object prefer its live name
                        if (auto* obj = dh.get()) return obj->getName();
                        // If no underlying object, fall back to the handle's cached name
                        std::string hn = dh.getName();
                        if (!hn.empty()) return hn;
                    } catch (...) {}
                    return e.getTypeName();
                }),
                "getName", [](const Event& e) -> std::string {
                    DisplayHandle dh = e.getTarget();
                    if (auto* obj = dh.get()) return obj->getName();
                    std::string hn = dh.getName();
                    if (!hn.empty()) return hn;
                    return e.getTypeName();
                },

                // Payload accessors
                // getPayload requires access to the Lua state to return a sol::table safely
                "getPayload", [](const Event& e, sol::this_state ts) -> sol::object {
                    sol::state_view sv(ts);
                    try {
                        return sol::object(sv, e.getPayload());
                    } catch (...) {
                        return sol::object(sv, sv.create_table());
                    }
                },                
                "setPayload", [](Event& e, const sol::table& t) { e.setPayload(t); return e; },

                // Mouse event accessors (methods)
                "getMouseX", &Event::getMouseX,
                "setMouseX", &Event::setMouseX,
                "getMouseY", &Event::getMouseY,
                "setMouseY", &Event::setMouseY,
                "getWheelX", &Event::getWheelX,
                "setWheelX", &Event::setWheelX,
                "getWheelY", &Event::getWheelY,
                "setWheelY", &Event::setWheelY,
                "getDragOffsetX", &Event::getDragOffsetX,
                "setDragOffsetX", &Event::setDragOffsetX,
                "getDragOffsetY", &Event::getDragOffsetY,
                "setDragOffsetY", &Event::setDragOffsetY,
                "getClickCount", &Event::getClickCount,
                "setClickCount", &Event::setClickCount,
                "getButton", &Event::getButton,
                "setButton", &Event::setButton,

                // Keyboard accessors (methods)
                "getScanCode", &Event::getScanCode,
                "setScanCode", &Event::setScanCode,
                "getKeycode", &Event::getKeycode,
                "setKeycode", &Event::setKeycode,
                "getKeymod", &Event::getKeymod,
                "setKeymod", &Event::setKeymod,
                "getAsciiCode", &Event::getAsciiCode,
                "setAsciiCode", &Event::setAsciiCode,

                // Core/default accessors (methods)
                "getType", &Event::getType,
                "getTypeName", &Event::getTypeName,
                "getPhaseString", &Event::getPhaseString,
                "getPhase", &Event::getPhase,
                "setPhase", &Event::setPhase,
                "getTarget", &Event::getTarget,
                "setTarget", &Event::setTarget,
                "getCurrentTarget", &Event::getCurrentTarget,
                "setCurrentTarget", &Event::setCurrentTarget,
                "getRelatedTarget", &Event::getRelatedTarget,
                "setRelatedTarget", &Event::setRelatedTarget,
                "isPropagationStopped", &Event::isPropagationStopped,
                "stopPropagation", &Event::stopPropagation,
                "isDefaultBehaviorDisabled", &Event::isDefaultBehaviorDisabled,
                "setDisableDefaultBehavior", &Event::setDisableDefaultBehavior,
                "getUseCapture", &Event::getUseCapture,
                "setUseCapture", &Event::setUseCapture,
                "getElapsedTime", &Event::getElapsedTime,
                "setElapsedTime", &Event::setElapsedTime,
                "getSDLEvent", &Event::getSDL_Event,
                "setSDLEvent", &Event::setSDL_Event,

                // Lower-camelcase properties (convenience) - readable and writable where applicable
                "mouse_x", sol::property(&Event::getMouseX, &Event::setMouseX),
                "mouse_y", sol::property(&Event::getMouseY, &Event::setMouseY),
                "wheel_x", sol::property(&Event::getWheelX, &Event::setWheelX),
                "wheel_y", sol::property(&Event::getWheelY, &Event::setWheelY),
                "drag_offset_x", sol::property(&Event::getDragOffsetX, &Event::setDragOffsetX),
                "drag_offset_y", sol::property(&Event::getDragOffsetY, &Event::setDragOffsetY),
                "click_count", sol::property(&Event::getClickCount, &Event::setClickCount),
                "button", sol::property(&Event::getButton, &Event::setButton),
                "scan_code", sol::property(&Event::getScanCode, &Event::setScanCode),
                "keycode", sol::property(&Event::getKeycode, &Event::setKeycode),
                "keymod", sol::property(&Event::getKeymod, &Event::setKeymod),
                "ascii_code", sol::property(&Event::getAsciiCode, &Event::setAsciiCode),
                // Targets as lower-camelcase properties for Lua convenience
                "currentTarget", sol::property(&Event::getCurrentTarget, &Event::setCurrentTarget),
                "relatedTarget", sol::property(&Event::getRelatedTarget, &Event::setRelatedTarget),

                // maintain previous pairs meta-function
                sol::meta_function::pairs, [](const Event& e, sol::this_state ts) {
                    sol::state_view lua(ts);

                    // iterator state stored in Lua as a user value
                    struct event_iter_state {
                        struct item {
                            std::string key;
                            // kind: 1 = string, 2 = number, 3 = object pointer
                            int kind = 0;
                            std::string sval;
                            double dval = 0.0;
                            SDOM::IDataObject* obj = nullptr;
                        };
                        std::size_t idx = 0;
                        std::vector<item> items;
                    };

                    // populate iterator state with snapshot of keys/values we want to expose
                    auto st = event_iter_state();
                    try {
                        st.items.push_back({"type", 1, e.getTypeName(), 0.0, nullptr});
                    } catch (...) {}
                    try {
                        st.items.push_back({"dt", 2, std::string(), static_cast<double>(e.getElapsedTime()), nullptr});
                    } catch (...) {}
                    try {
                        DisplayHandle dh = e.getTarget();
                        if (dh) {
                            // Prefer live object's name when available, otherwise the handle's cached name
                            std::string targName;
                            if (auto* obj = dh.get()) {
                                try { targName = obj->getName(); } catch(...) { targName.clear(); }
                            } else {
                                try { targName = dh.getName(); } catch(...) { targName.clear(); }
                            }
                            if (!targName.empty()) {
                                st.items.push_back({"target", 1, targName, 0.0, nullptr});
                            }
                        }
                    } catch (...) {}

                    // the 'next' function for our iterator: it advances the index and returns key,value
                    auto event_next = +[] (sol::user<event_iter_state&> user_st, sol::this_state ts) -> std::tuple<sol::object, sol::object> {
                        sol::state_view lua(ts);
                        event_iter_state& s = user_st;
                        if (s.idx >= s.items.size()) {
                            return std::make_tuple(sol::object(lua, sol::lua_nil), sol::object(lua, sol::lua_nil));
                        }
                        auto &it = s.items[s.idx++];
                        sol::object k(lua, sol::in_place, it.key);
                        sol::object v;
                        if (it.kind == 1) {
                            v = sol::object(lua, sol::in_place, it.sval);
                        } else if (it.kind == 2) {
                            v = sol::object(lua, sol::in_place, it.dval);
                        } else {
                            v = sol::object(lua, sol::lua_nil);
                        }
                        return std::make_tuple(k, v);
                    };

                    // return (iterator function, user state, nil)
                    return std::make_tuple(event_next, sol::user<event_iter_state>(std::move(st)), sol::lua_nil);
                }
            );
        }
    } catch (...) {
        // non-fatal
        WARNING("Failed to register Event with Lua");
    }
}