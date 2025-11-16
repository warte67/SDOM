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
#include <SDOM/CAPI/SDOM_CAPI_Events.h>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_SDL_Utils.hpp>
#include <SDOM/SDOM_EventManager.hpp>
// #include <SDOM/SDOM_CAPI_Events_runtime.h>


namespace SDOM
{
    SDOM::Event::Event(EventType type, DisplayHandle target, float fElapsedTime)
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


    // IDataObject binding registration for Event (DataRegistry + Lua helpers)
    void Event::registerBindingsImpl(const std::string& typeName)
    {
        // Base info
        BIND_INFO(typeName, "Event");

        // Properties
        addProperty(typeName, "name",
            [](const Event& e) { return e.getTypeName(); });

        addProperty(typeName, "captures",
            [](const Event& e) { return e.getUseCapture(); },
            [](Event& e, bool v) { e.setUseCapture(v); });

        addProperty(typeName, "dt",
            [](const Event& e) { return e.getElapsedTime(); },
            [](Event& e, float v) { e.setElapsedTime(v); });

        // coerce enum properties to integers for binding
        addProperty(typeName, "phase",
            [](const Event& e) { return static_cast<int>(e.getPhase()); },
            [](Event& e, int v) { e.setPhase(static_cast<Event::Phase>(v)); });

        // Register a function to declare/register EventTypes from Lua/registry table
        addFunction(typeName, "registerEventType", [](const sol::table& desc) {
            try {
                if (!desc["name"].valid()) return;
                std::string name = desc["name"].get<std::string>();
                EventType et(name);
                // if (desc["captures"].valid()) et.setCaptures(desc["captures"].get<bool>());
                // if (desc["bubbles"].valid()) et.setBubbles(desc["bubbles"].get<bool>());
                // if (desc["targetOnly"].valid()) et.setTargetOnly(desc["targetOnly"].get<bool>());
                // if (desc["global"].valid()) et.setGlobal(desc["global"].get<bool>());
                // if (desc["critical"].valid()) et.setCritical(desc["critical"].get<bool>());
                // if (desc["meter_enabled"].valid()) et.setMeterEnabled(desc["meter_enabled"].get<bool>());
                // if (desc["meter_interval_ms"].valid()) et.setMeterIntervalMs(static_cast<uint16_t>(desc["meter_interval_ms"].get<int>()));
                // coalesce enums may be passed as integer
                if (desc["coalesce_strategy"].valid()) et.setCoalesceStrategy(static_cast<EventType::CoalesceStrategy>(desc["coalesce_strategy"].get<int>()));
                if (desc["coalesce_key"].valid()) et.setCoalesceKey(static_cast<EventType::CoalesceKey>(desc["coalesce_key"].get<int>()));
            } catch(...) {}
        });

        addFunction(typeName, "getEventTypeByName", [](const std::string& name) -> std::string {
            EventType* et = EventType::fromName(name);
            if (!et) return std::string();
            return et->getName();
        });

        // --- Populate DataRegistry with EventType metadata ---
        // --- Register canonical DataTypes for EventType and enums ---
        try {
            // 1) EventType DataType: properties + functions that operate on an EventType id
            SDOM::TypeInfo etype;
            etype.name = "EventType";
            etype.cpp_type_id = "SDOM::EventType";
    
            SDOM::PropertyInfo pp;
            pp.name = "captures";
            pp.cpp_type = "bool";
            pp.read_only = false;
            pp.doc = "Whether this event type participates in the capture phase.";
            etype.properties.push_back(pp);
    
            pp.name = "bubbles";
            pp.cpp_type = "bool";
            pp.read_only = false;
            pp.doc = "Whether this event type bubbles.";
            etype.properties.push_back(pp);
    
            pp.name = "targetOnly";
            pp.cpp_type = "bool";
            pp.read_only = false;
            pp.doc = "If true the event is target-only (no capture/bubble).";
            etype.properties.push_back(pp);
    
            pp.name = "global";
            pp.cpp_type = "bool";
            pp.read_only = false;
            pp.doc = "Whether this event type is considered global.";
            etype.properties.push_back(pp);
    
            // instance-style functions that accept EventType id as first arg
            SDOM::FunctionInfo fi;
            fi.name = "get_name";
            fi.cpp_signature = "std::string(SDOM::EventType::IdType)";
            fi.return_type = "const char*";
            fi.c_signature = "const char* SDOM_EventType_name_for_id(SDOM_EventTypeId id);";
            fi.doc = "Return the EventType name for the given id.";
            etype.functions.push_back(fi);
    
            fi.name = "get_id_for_name";
            fi.cpp_signature = "SDOM::EventType::IdType(const std::string&)";
            fi.return_type = "uint32_t";
            fi.c_signature = "SDOM_EventTypeId SDOM_EventType_id_for_name(const char* name);";
            fi.doc = "Return numeric id for the given EventType name (assigns if missing).";
            etype.functions.push_back(fi);
    
            // getters/setters for boolean properties (take id as first param)
            SDOM::FunctionInfo g;
            g.name = "get_captures";
            g.cpp_signature = "bool(SDOM::EventType::IdType)";
            g.return_type = "bool";
            g.c_signature = "bool SDOM_EventType_get_captures(SDOM_EventTypeId id);";
            g.doc = "Get captures flag for EventType id.";
            etype.functions.push_back(g);
    
            g.name = "set_captures";
            g.cpp_signature = "void(SDOM::EventType::IdType, bool)";
            g.return_type = "void";
            g.c_signature = "void SDOM_EventType_set_captures(SDOM_EventTypeId id, bool v);";
            g.doc = "Set captures flag for EventType id.";
            etype.functions.push_back(g);
    
            g.name = "get_bubbles";
            g.cpp_signature = "bool(SDOM::EventType::IdType)";
            g.return_type = "bool";
            g.c_signature = "bool SDOM_EventType_get_bubbles(SDOM_EventTypeId id);";
            g.doc = "Get bubbles flag for EventType id.";
            etype.functions.push_back(g);
    
            g.name = "set_bubbles";
            g.cpp_signature = "void(SDOM::EventType::IdType, bool)";
            g.return_type = "void";
            g.c_signature = "void SDOM_EventType_set_bubbles(SDOM_EventTypeId id, bool v);";
            g.doc = "Set bubbles flag for EventType id.";
            etype.functions.push_back(g);
    
            g.name = "get_target_only";
            g.cpp_signature = "bool(SDOM::EventType::IdType)";
            g.return_type = "bool";
            g.c_signature = "bool SDOM_EventType_get_target_only(SDOM_EventTypeId id);";
            g.doc = "Get targetOnly flag for EventType id.";
            etype.functions.push_back(g);
    
            g.name = "set_target_only";
            g.cpp_signature = "void(SDOM::EventType::IdType, bool)";
            g.return_type = "void";
            g.c_signature = "void SDOM_EventType_set_target_only(SDOM_EventTypeId id, bool v);";
            g.doc = "Set targetOnly flag for EventType id.";
            etype.functions.push_back(g);
    
            g.name = "get_global";
            g.cpp_signature = "bool(SDOM::EventType::IdType)";
            g.return_type = "bool";
            g.c_signature = "bool SDOM_EventType_get_global(SDOM_EventTypeId id);";
            g.doc = "Get global flag for EventType id.";
            etype.functions.push_back(g);
    
            g.name = "set_global";
            g.cpp_signature = "void(SDOM::EventType::IdType, bool)";
            g.return_type = "void";
            g.c_signature = "void SDOM_EventType_set_global(SDOM_EventTypeId id, bool v);";
            g.doc = "Set global flag for EventType id.";
            etype.functions.push_back(g);
    
            // Register the canonical EventType data type in the global registry
            addDataType("EventType", etype);
    
            // 2) CoalesceStrategy enum description
            SDOM::TypeInfo csinfo;
            csinfo.name = "EventType::CoalesceStrategy";
            csinfo.cpp_type_id = "SDOM::EventType::CoalesceStrategy";
            csinfo.doc = "Enum: None=0, Last=1, Sum=2, Count=3";
            addDataType(csinfo.name, csinfo);
    
            // 3) CoalesceKey enum description
            SDOM::TypeInfo ckinfo;
            ckinfo.name = "EventType::CoalesceKey";
            ckinfo.cpp_type_id = "SDOM::EventType::CoalesceKey";
            ckinfo.doc = "Enum: Global=0, ByTarget=1";
            addDataType(ckinfo.name, ckinfo);
    
        } catch(...) {
            BIND_WARN("Event::registerBindingsImpl: failed to register EventType DataTypes");
        }
        // Re-register any already-constructed EventType instances so the
        // generator sees per-instance metadata (EventType::<name>) for
        // each built-in event. Use addDataType to forward metadata.
        try {
            auto all = EventType::getAll();
            for (EventType* et : all) {
                if (!et) continue;
                SDOM::TypeInfo ti;
                ti.name = std::string("EventType::") + et->getName();
                ti.cpp_type_id = et->getName();
                // forward per-event documentation from the runtime EventType
                try { ti.doc = et->getDoc(); } catch(...) { ti.doc.clear(); }
                // forward per-event category (used by generators for grouping)
                try { ti.category = et->getCategory(); } catch(...) { ti.category.clear(); }

                SDOM::PropertyInfo p;
                p.name = "captures";
                p.cpp_type = "bool";
                p.read_only = false;
                p.doc = "Whether this event type participates in the capture phase.";
                ti.properties.push_back(p);

                p.name = "bubbles";
                p.cpp_type = "bool";
                p.read_only = false;
                p.doc = "Whether this event type bubbles.";
                ti.properties.push_back(p);

                p.name = "target_only";
                p.cpp_type = "bool";
                p.read_only = false;
                p.doc = "If true the event is target-only (no capture/bubble).";
                ti.properties.push_back(p);

                p.name = "global";
                p.cpp_type = "bool";
                p.read_only = false;
                p.doc = "Whether this event type is considered global.";
                ti.properties.push_back(p);

                SDOM::FunctionInfo fi;
                fi.name = "getName";
                fi.cpp_signature = "std::string()";
                fi.return_type = "std::string";
                fi.doc = "Return the canonical name of the EventType.";
                fi.is_static = false;
                ti.functions.push_back(fi);

                fi.name = "getId";
                fi.cpp_signature = "uint32_t()";
                fi.return_type = "uint32_t";
                fi.doc = "Return the numeric id for this EventType (assigning one if necessary).";
                fi.is_static = false;
                ti.functions.push_back(fi);

                addDataType(ti.name, ti);
            }
        } catch(...) {
            BIND_WARN("Event::registerBindingsImpl: failed to register existing EventType instances");
        }
        // Register class-level helpers under the "EventType" type so generators
        // can emit runtime helpers. Use IDataObject registration helpers so
        // the DataRegistry gets metadata + callables when available.
        addFunction("EventType", "registerEventType", [](const std::string& name, bool captures, bool bubbles, bool targetOnly, bool global) {
            new EventType(name, captures, bubbles, targetOnly, global);
        });

        addFunction("EventType", "isRegistered", [](const std::string& name) -> bool {
            return EventType::isRegistered(name);
        });

        addFunction("EventType", "idForName", [](const std::string& name) -> uint32_t {
            if (auto *et = EventType::fromName(name)) return static_cast<uint32_t>(et->getOrAssignId());
            auto *et = new EventType(name);
            return static_cast<uint32_t>(et->getOrAssignId());
        });

        addFunction("EventType", "nameForId", [](uint32_t id) -> std::string {
            if (auto *et = EventType::fromId(static_cast<EventType::IdType>(id))) return et->getName();
            return std::string();
        });
        // Register dispatcher callables used by generated C API wrappers.
        using namespace SDOM::CAPI;
        registerCallable("Event.CreateEventType", [](const std::vector<CallArg>& args) -> CallResult {
            try {
                const SDOM_EventTypeDesc* desc = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) desc = reinterpret_cast<const SDOM_EventTypeDesc*>(args[0].v.p);
                if (!desc || !desc->name) return CallResult::Void();
                std::string name = desc->name ? desc->name : std::string();
                std::string category = desc->category ? desc->category : std::string();
                std::string doc = desc->doc ? desc->doc : std::string();
                SDOM::EventType* et = nullptr;
                if (!category.empty()) et = new SDOM::EventType(name, category, doc);
                else { et = new SDOM::EventType(name); if (!doc.empty()) et->setDoc(doc); }
                return CallResult::FromPtr(reinterpret_cast<void*>(et));
            } catch(...) { return CallResult::Void(); }
        });

        registerCallable("Event.FindEventTypeByName", [](const std::vector<CallArg>& args) -> CallResult {
            try {
                std::string name;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) name = args[0].s;
                    else if (args[0].kind == CallArg::Kind::Ptr && args[0].v.p) name = reinterpret_cast<const char*>(args[0].v.p);
                }
                if (name.empty()) return CallResult::Void();
                SDOM::EventType* et = SDOM::EventType::fromName(name);
                if (!et) return CallResult::Void();
                return CallResult::FromPtr(reinterpret_cast<void*>(et));
            } catch(...) { return CallResult::Void(); }
        });

        registerCallable("Event.EnumEventTypes", [](const std::vector<CallArg>& args) -> CallResult {
            try {
                size_t index = 0;
                if (!args.empty() && args[0].kind == CallArg::Kind::UInt) index = static_cast<size_t>(args[0].v.u);
                auto all = SDOM::EventType::getAll();
                if (index >= all.size()) return CallResult::Void();
                SDOM::EventType* et = all[index];
                return CallResult::FromPtr(reinterpret_cast<void*>(et));
            } catch(...) { return CallResult::Void(); }
        });

        registerCallable("Event.CreateEvent", [](const std::vector<CallArg>& args) -> CallResult {
            try {
                const SDOM_EventDesc* desc = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) desc = reinterpret_cast<const SDOM_EventDesc*>(args[0].v.p);
                SDOM::EventType* et = nullptr;
                if (desc) {
                    if (desc->type_id != 0) et = SDOM::EventType::fromId(static_cast<SDOM::EventType::IdType>(desc->type_id));
                    if (!et && desc->name) et = SDOM::EventType::fromName(std::string(desc->name));
                }
                SDOM::Event* ev = nullptr;
                if (et) ev = new SDOM::Event(*et);
                else ev = new SDOM::Event(SDOM::EventType("None"));
                return CallResult::FromPtr(reinterpret_cast<void*>(ev));
            } catch(...) { return CallResult::Void(); }
        });

        registerCallable("Event.SendEvent", [](const std::vector<CallArg>& args) -> CallResult {
            try {
                if (args.empty() || args[0].kind != CallArg::Kind::Ptr) return CallResult::FromInt(SDOM_CAPI_ERR_INVALID_ARG);
                SDOM::Event* ev = reinterpret_cast<SDOM::Event*>(args[0].v.p);
                if (!ev) return CallResult::FromInt(SDOM_CAPI_ERR_INVALID_ARG);
                auto evCopy = std::make_unique<SDOM::Event>(*ev);
                SDOM::Core::getInstance().getEventManager().addEvent(std::move(evCopy));
                return CallResult::FromInt(SDOM_CAPI_OK);
            } catch(...) { return CallResult::FromInt(SDOM_CAPI_ERR_INTERNAL); }
        });

        registerCallable("Event.GetEventQueueSize", [](const std::vector<CallArg>&) -> CallResult {
            try { int n = SDOM::Core::getInstance().getEventManager().getEventQueueSize(); return CallResult::FromInt(n); } catch(...) { return CallResult::FromInt(-1); }
        });
        
    } // END Event::registerBindingsImpl(const std::string& typeName)


} // namespace SDOM