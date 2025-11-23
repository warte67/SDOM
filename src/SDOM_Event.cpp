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
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_SDL_Utils.hpp>
#include <SDOM/SDOM_EventManager.hpp>
// #include <SDOM/SDOM_CAPI_Events_runtime.h>


namespace SDOM
{
    SDOM::Event::Event(EventType type, DisplayHandle target, float fElapsedTime)
        : type(type),
          target(target),
          current_phase(Phase::Capture),
          elapsed_time(fElapsedTime),
          payload_(nlohmann::json::object())
    {
        // Constructor implementation
        use_capture = true; // Default to using capture phase

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


    std::string Event::getPhaseString() const 
    {
        std::lock_guard<std::mutex> lock(event_mutex_);
        switch (current_phase) 
        {
            case Phase::Capture:    return "Capture";
            case Phase::Target:     return "Target";
            case Phase::Bubbling:   return "Bubbling";
            default:                return "Unknown phase";
        }
    }    

    EventType Event::getType() const                { std::lock_guard<std::mutex> lock(event_mutex_); return type; }
    std::string Event::getTypeName() const          { std::lock_guard<std::mutex> lock(event_mutex_); return type.getName(); }

    Event::Phase Event::getPhase() const            { std::lock_guard<std::mutex> lock(event_mutex_); return current_phase; }
    Event& Event::setPhase(Event::Phase phase)      { std::lock_guard<std::mutex> lock(event_mutex_); current_phase = phase; return *this; }

    DisplayHandle Event::getTarget() const          { std::lock_guard<std::mutex> lock(event_mutex_); return target; }
    Event& Event::setTarget(DisplayHandle newTarget) 
        { std::lock_guard<std::mutex> lock(event_mutex_); target = newTarget; return *this; }

    DisplayHandle Event::getCurrentTarget() const   { std::lock_guard<std::mutex> lock(event_mutex_); return current_target; }
    Event& Event::setCurrentTarget(DisplayHandle newCurrentTarget) 
        { std::lock_guard<std::mutex> lock(event_mutex_); current_target = newCurrentTarget; return *this; }

    DisplayHandle Event::getRelatedTarget() const   { std::lock_guard<std::mutex> lock(event_mutex_); return related_target; }
    Event& Event::setRelatedTarget(DisplayHandle newRelatedTarget) 
        { std::lock_guard<std::mutex> lock(event_mutex_); related_target = newRelatedTarget; return *this; }

    bool Event::isPropagationStopped() const        { std::lock_guard<std::mutex> lock(event_mutex_); return propagation_stopped; }
    Event& Event::stopPropagation()                 { std::lock_guard<std::mutex> lock(event_mutex_); propagation_stopped = true; return *this; }

    bool Event::isDefaultBehaviorDisabled() const         { std::lock_guard<std::mutex> lock(event_mutex_); return disable_default_behavior; }
    Event& Event::setDisableDefaultBehavior(bool disable) 
        { std::lock_guard<std::mutex> lock(event_mutex_); disable_default_behavior = disable; return *this; }

    bool Event::getUseCapture() const               { std::lock_guard<std::mutex> lock(event_mutex_); return use_capture; }
    Event& Event::setUseCapture(bool useCapture)    { std::lock_guard<std::mutex> lock(event_mutex_); this->use_capture = useCapture; return *this; } 

    float Event::getElapsedTime() const             { std::lock_guard<std::mutex> lock(event_mutex_); return elapsed_time; }
    Event& Event::setElapsedTime(float elapsedTime) { std::lock_guard<std::mutex> lock(event_mutex_); elapsed_time = elapsedTime; return *this; }

    SDL_Event Event::getSDL_Event() const           { std::lock_guard<std::mutex> lock(event_mutex_); return sdl_event; }
    Event& Event::setSDL_Event(const SDL_Event& e)  { std::lock_guard<std::mutex> lock(event_mutex_); sdl_event = e; return *this; }


    // ------------------------------ //
    // --- JSON Payload Accessors --- //
    // ------------------------------ //

    const nlohmann::json& Event::getPayload() const {
        std::lock_guard<std::mutex> lock(event_mutex_);
        return payload_;
    }

    Event& Event::setPayload(const nlohmann::json& j) {
        std::lock_guard<std::mutex> lock(event_mutex_);
        payload_ = j;
        return *this;
    }

    Event& Event::setPayloadString(const std::string& jsonStr) {
        std::lock_guard<std::mutex> lock(event_mutex_);
        payload_ = nlohmann::json::parse(jsonStr, nullptr, false);
        if (payload_.is_discarded()) {
            payload_ = nlohmann::json::object(); // fail-safe empty value
        }
        return *this;
    }

    std::string Event::getPayloadString() const {
        std::lock_guard<std::mutex> lock(event_mutex_);
        return payload_.dump();
    }


    // ------------------------------ //
    // --- Mouse Event Properties --- //
    // ------------------------------ //

    float Event::getMouseX() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return 0.0f;
        return payload_.value("mouse_x", 0.0f);
    }

    Event& Event::setMouseX(float x)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["mouse_x"] = x;
        return *this;
    }

    float Event::getMouseY() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return 0.0f;
        return payload_.value("mouse_y", 0.0f);
    }

    Event& Event::setMouseY(float y)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["mouse_y"] = y;
        return *this;
    }

    float Event::getWheelX() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return 0.0f;
        return payload_.value("wheel_x", 0.0f);
    }

    Event& Event::setWheelX(float x)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["wheel_x"] = x;
        return *this;
    }

    float Event::getWheelY() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return 0.0f;
        return payload_.value("wheel_y", 0.0f);
    }

    Event& Event::setWheelY(float y)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["wheel_y"] = y;
        return *this;
    }

    float Event::getDragOffsetX() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return 0.0f;
        return payload_.value("drag_offset_x", 0.0f);
    }

    Event& Event::setDragOffsetX(float v)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["drag_offset_x"] = v;
        return *this;
    }

    float Event::getDragOffsetY() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return 0.0f;
        return payload_.value("drag_offset_y", 0.0f);
    }

    Event& Event::setDragOffsetY(float v)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["drag_offset_y"] = v;
        return *this;
    }


    // -------------------------- //
    // --- Clicks and Buttons --- //
    // -------------------------- //

    int Event::getClickCount() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return 0;
        return payload_.value("click_count", 0);
    }

    Event& Event::setClickCount(int c)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["click_count"] = c;
        return *this;
    }

    uint8_t Event::getButton() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return 0u;
        return static_cast<uint8_t>(payload_.value("button", 0));
    }

    Event& Event::setButton(uint8_t b)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["button"] = b;
        return *this;
    }

    // -- Keyboard Event Properties -- //
    SDL_Scancode Event::getScanCode() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return SDL_SCANCODE_UNKNOWN;
        return static_cast<SDL_Scancode>(payload_.value("scancode", static_cast<int>(SDL_SCANCODE_UNKNOWN)));
    }

    Event& Event::setScanCode(SDL_Scancode sc)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["scancode"] = static_cast<int>(sc);
        return *this;
    }

    SDL_Keycode Event::getKeycode() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return SDLK_UNKNOWN;
        return static_cast<SDL_Keycode>(payload_.value("keycode", static_cast<int>(SDLK_UNKNOWN)));
    }

    Event& Event::setKeycode(SDL_Keycode kc)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["keycode"] = static_cast<int>(kc);
        return *this;
    }

    Uint16 Event::getKeymod() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return 0;
        return static_cast<Uint16>(payload_.value("keymod", 0));
    }

    Event& Event::setKeymod(Uint16 km)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["keymod"] = km;
        return *this;
    }

    int Event::getAsciiCode() const
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) return 0;
        return payload_.value("ascii_code", 0);
    }

    Event& Event::setAsciiCode(int a)
    {
        std::lock_guard lock(event_mutex_);
        if (!payload_.is_object()) payload_ = nlohmann::json::object();
        payload_["ascii_code"] = a;
        return *this;
    }


    void Event::registerBindingsImpl(const std::string& typeName)
    {
        BIND_INFO(typeName, "Event");


        // -----------------------------
        // Register Functions
        // -----------------------------
        auto* activeRegistry = IDataObject::activeRegistry();
        auto lookupTypeInfo = [&](const std::string& name) -> const SDOM::TypeInfo* {
            if (activeRegistry) {
                return activeRegistry->lookupType(name);
            }
            return SDOM::DataRegistry::instance().lookupType(name);
        };

        if (!lookupTypeInfo(typeName))
        {
            SDOM::TypeInfo eventInfo;
            eventInfo.name        = typeName;
            eventInfo.cpp_type_id = "SDOM::Event";
            eventInfo.file_stem   = "Event";
            eventInfo.export_name = "SDOM_Event";
            eventInfo.kind        = SDOM::EntryKind::Object;
            eventInfo.category    = "Events";
            eventInfo.doc         = "SDOM Event object bindings";
            addDataType(typeName, eventInfo);
        }


        SDOM::FunctionInfo getTypeInfo;
        getTypeInfo.name          = "getType";
        getTypeInfo.cpp_signature = "EventType Event::getType() const";
        getTypeInfo.return_type   = "EventType";
        getTypeInfo.doc           = "Retrieves the numeric SDOM_EventType id for this event.";
        getTypeInfo.c_name        = "SDOM_GetEventType";
        getTypeInfo.c_signature   = "bool SDOM_GetEventType(const SDOM_Event* evt, SDOM_EventType* out_type)";

        auto getTypeBinding = [](const Event* evt, EventType::IdType* outType) -> bool {
            if (!evt || !outType) {
                return false;
            }

            std::lock_guard<std::mutex> lock(evt->event_mutex_);
            Event* mutableEvent = const_cast<Event*>(evt);
            EventType& typeRef = mutableEvent->type;
            *outType = typeRef.getOrAssignId();
            return true;
        };

        addFunction(typeName, std::move(getTypeInfo), getTypeBinding);

        // -----------------------------
        // Register Event Handle Struct
        // -----------------------------
        const std::string eventHandleName = "SDOM_Event";
        if (!lookupTypeInfo(eventHandleName))
        {
            SDOM::TypeInfo eventHandle;
            eventHandle.name        = eventHandleName;
            eventHandle.cpp_type_id = eventHandleName;
            eventHandle.file_stem   = "Event";
            eventHandle.export_name = eventHandleName;
            eventHandle.kind        = SDOM::EntryKind::Struct;
            eventHandle.doc         = "C API opaque handle that wraps an SDOM::Event pointer.";

            SDOM::PropertyInfo implField;
            implField.name      = "impl";
            implField.cpp_type  = "SDOM::Event*";
            implField.read_only = true;
            implField.doc       = "Pointer to the underlying C++ SDOM::Event instance.";
            eventHandle.properties.push_back(std::move(implField));

            addDataType(eventHandleName, eventHandle);
        }


        // -----------------------------
        // Register EventType Enumeration
        // -----------------------------
        auto makeEventTypeInfo = [](const EventType& et) {
            SDOM::TypeInfo ti;
            ti.name        = std::string("EventType::") + et.getName();
            ti.cpp_type_id = et.getName();
            ti.file_stem   = "Event";  // EventType enumeration belongs in the SDOM_CAPI_Event.h header
            ti.export_name = "SDOM_EventType";
            ti.kind        = SDOM::EntryKind::Enum;
            ti.enum_value  = const_cast<EventType&>(et).getOrAssignId();
            ti.doc         = et.getDoc();
            ti.category    = et.getCategory();

            auto addBoolProp = [&](std::string_view prop, std::string_view doc) {
                SDOM::PropertyInfo p;
                p.name      = std::string(prop);
                p.cpp_type  = "bool";
                p.read_only = false;
                p.doc       = std::string(doc);
                ti.properties.push_back(std::move(p));
            };
            addBoolProp("captures",    "Whether this event type participates in capture.");
            addBoolProp("bubbles",     "Whether this event type bubbles.");
            addBoolProp("target_only", "If true, event is target-only.");
            addBoolProp("global",      "Whether this event type is global.");

            return ti;
        };

        for (EventType* et : EventType::getAll())
        {
            if (!et) continue;
            addDataType(et->getName(), makeEventTypeInfo(*et));
        }
    } // END Event::registerBindingsImpl(const std::string& typeName)


} // namespace SDOM