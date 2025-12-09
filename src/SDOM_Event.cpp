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
#include <SDOM/CAPI/SDOM_CAPI_Handles.h>
#include <SDOM/CAPI/SDOM_CAPI_Variant.h>
#include <SDOM/SDOM_CoreAPI.hpp>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
// #include <SDOM/SDOM_CAPI_Events_runtime.h>


namespace SDOM
{
    namespace {
        struct DisplayHandleScratch {
            std::string name;
            std::string type;
        };

        thread_local DisplayHandleScratch s_eventTargetScratch;
        thread_local DisplayHandleScratch s_eventCurrentTargetScratch;
        thread_local DisplayHandleScratch s_eventRelatedTargetScratch;

        inline double bitsToDouble(std::uint64_t bits)
        {
            double out{};
            std::memcpy(&out, &bits, sizeof(double));
            return out;
        }

        inline std::uint64_t doubleToBits(double d)
        {
            std::uint64_t out{};
            std::memcpy(&out, &d, sizeof(double));
            return out;
        }

        bool exportDisplayHandleToC(const DisplayHandle& handle,
                                    SDOM_DisplayHandle* out,
                                    DisplayHandleScratch& scratch)
        {
            if (!out) {
                return false;
            }

            scratch.name = handle.getName();
            scratch.type = handle.getType();

            out->object_id = handle.getId();
            out->name = scratch.name.c_str();
            out->type = scratch.type.c_str();
            return true;
        }

        DisplayHandle importDisplayHandleFromC(const SDOM_DisplayHandle* in)
        {
            if (!in) {
                return DisplayHandle{};
            }

            DisplayHandle result;
            result.setName(in->name ? in->name : "");
            result.setType(in->type ? in->type : "");
            result.setId(in->object_id);
            return result;
        }

        // Variant marshaling helpers for the provisional C ABI view. Only scalar
        // kinds are supported here; complex containers are deferred until their
        // ownership semantics are defined.
        thread_local std::string s_variantStringScratch;

        bool importVariantFromC(const SDOM_Variant* in, Variant& out)
        {
            out = Variant{};
            if (!in) {
                return false;
            }

            switch (static_cast<SDOM_VariantType>(in->type)) {
                case SDOM_VARIANT_TYPE_NULL:
                    out = Variant{};
                    return true;
                case SDOM_VARIANT_TYPE_BOOL:
                    out = Variant(static_cast<bool>(in->data != 0));
                    return true;
                case SDOM_VARIANT_TYPE_INT:
                    out = Variant(static_cast<std::int64_t>(in->data));
                    return true;
                case SDOM_VARIANT_TYPE_FLOAT: {
                    const double f = bitsToDouble(static_cast<std::uint64_t>(in->data));
                    out = Variant(f);
                    return true;
                }
                case SDOM_VARIANT_TYPE_STRING: {
                    const char* s = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(in->data));
                    out = Variant(s ? std::string{s} : std::string{});
                    return true;
                }
                default:
                    SDOM::CoreAPI::setErrorMessage("Unsupported Variant type for ABI import");
                    return false;
            }
        }

        SDOM_Variant exportVariantToC(const Variant& v)
        {
            SDOM_Variant out{};
            switch (v.type()) {
                case VariantType::Null:
                    out.type = SDOM_VARIANT_TYPE_NULL;
                    break;
                case VariantType::Bool:
                    out.type = SDOM_VARIANT_TYPE_BOOL;
                    out.data = static_cast<std::uint64_t>(v.toBool());
                    break;
                case VariantType::Int:
                    out.type = SDOM_VARIANT_TYPE_INT;
                    out.data = static_cast<std::uint64_t>(v.toInt64());
                    break;
                case VariantType::Real:
                    out.type = SDOM_VARIANT_TYPE_FLOAT;
                    out.data = doubleToBits(static_cast<double>(v.toDouble()));
                    break;
                case VariantType::String:
                    out.type = SDOM_VARIANT_TYPE_STRING;
                    s_variantStringScratch = v.toString();
                    out.data = reinterpret_cast<std::uint64_t>(s_variantStringScratch.c_str());
                    break;
                default:
                    // Unsupported container/dynamic types signal an error for now.
                    SDOM::CoreAPI::setErrorMessage("Unsupported Variant type for ABI export");
                    return out; // defaults to null/zeroed
            }
            return out;
        }
    } // namespace

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

    std::string Event::getSDL_EventJson() const
    {
        std::lock_guard<std::mutex> lock(event_mutex_);
        return SDL_Utils::eventToJsonString(sdl_event);
    }

    Event& Event::setSDL_EventJson(const std::string& json)
    {
        std::lock_guard<std::mutex> lock(event_mutex_);
        SDL_Event decoded{};
        if (SDL_Utils::eventFromJsonString(json, decoded)) {
            sdl_event = decoded;
        }
        return *this;
    }


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

    bool Event::payloadKeyExists(const std::string& key) const {
        if (!payload_.is_object()) {
            return false;
        }
        return payload_.contains(key);
    }

    Variant Event::getPayloadValueVariant(const std::string& key) const {
        std::lock_guard<std::mutex> lock(event_mutex_);
        if (!payload_.is_object()) {
            SDOM::CoreAPI::setErrorMessage("Payload key not found");
            return Variant{};
        }

        auto it = payload_.find(key);
        if (it == payload_.end()) {
            SDOM::CoreAPI::setErrorMessage("Payload key not found");
            return Variant{};
        }

        return Variant::fromJson(*it);
    }

    Event& Event::setPayloadValueVariant(const std::string& key, const Variant& value) {
        std::lock_guard<std::mutex> lock(event_mutex_);
        if (!payload_.is_object()) {
            payload_ = nlohmann::json::object();
        }

        payload_[key] = value.toJson();
        return *this;
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
        // -------------------------------------------------------------------------
        // Diagnostic / Binding Context
        // -------------------------------------------------------------------------
        // Records metadata about the binding operation (source file, typeName, etc.)
        BIND_INFO(typeName, "Event");


        // -------------------------------------------------------------------------
        // Ensure EventType TypeInfo Exists
        // -------------------------------------------------------------------------
        // EventType is used by Event but does not expose callables; register its
        // descriptor here so binding generators and tests can rely on the metadata.
        if (!lookup("EventType")) {
            SDOM::TypeInfo ti;
            ti.name        = "EventType";
            ti.kind        = SDOM::EntryKind::Object;
            ti.cpp_type_id = "SDOM::EventType";
            ti.file_stem   = "EventType";
            ti.export_name = "SDOM_EventType";
            ti.subject_kind = "EventType";
            ti.subject_uses_handle = false;
            ti.has_handle_override = true;
            ti.dispatch_family_override = "event_router";
            ti.doc = "Logical subject descriptor for SDOM::EventType dispatch metadata.";
            registry().registerType(ti);
        }


        // -------------------------------------------------------------------------
        // Ensure Primary TypeInfo Exists
        // -------------------------------------------------------------------------
        // Creates (or retrieves) the canonical TypeInfo entry for SDOM::Event.
        // This guarantees the registry is ready before we attach methods, properties,
        // enum values, or opaque handle metadata.
        SDOM::TypeInfo& typeInfo = ensureType(typeName,
                SDOM::EntryKind::Object,
                "SDOM::Event",
                "Event",
                "SDOM_Event",
                "Events",
                "SDOM Event object bindings");

        typeInfo.subject_kind = "Event";
        typeInfo.subject_uses_handle = false;
        typeInfo.has_handle_override = true;
        typeInfo.dispatch_family_override = "event_router";


        // -------------------------------------------------------------------------
        // Register Opaque C Handle Type (SDOM_Event)
        // -------------------------------------------------------------------------
        // Exposes `struct SDOM_Event { void* impl; };` to the C API.  This is the
        // stable ABI-safe representation of Event instances outside C++.
        registerOpaqueHandle("SDOM::Event",
                            "SDOM_Event",
                            "Event",
                            "Opaque C handle for SDOM::Event.");


        // -------------------------------------------------------------------------
        // Register Methods (C/C++ Callable Functions)
        // -------------------------------------------------------------------------
        // Adds the C-facing function SDOM_GetEventType() and metadata about its
        // C++ signature, return type, documentation, call wrapper, etc.
        registerMethod(
            typeName,
            "getType",
            "EventType Event::getType() const",
            "EventType",
            "SDOM_GetEventType",
            "bool SDOM_GetEventType(const SDOM_Event* evt, SDOM_EventType* out_type)",
            "Retrieves the numeric SDOM_EventType id for this event.",
            [](const Event* evt, EventType::IdType* outType) -> bool {
                if (!evt || !outType) {
                    return false;
                }

                std::lock_guard<std::mutex> lock(evt->event_mutex_);
                Event* mutableEvent = const_cast<Event*>(evt);
                EventType& typeRef = mutableEvent->type;
                *outType = typeRef.getOrAssignId();
                return true;
            });

        // Adds the C-facing function SDOM_GetEventTypeName() and metadata about its
        // C++ signature, return type, documentation, call wrapper, etc.
        registerMethod(
            typeName,
            "getTypeName",
            "std::string Event::getTypeName() const",
            "const char*",
            "SDOM_GetEventTypeName",
            "const char* SDOM_GetEventTypeName(const SDOM_Event* evt)",
            "Retrieves the EventType name associated with this event.",
            [](const Event* evt) -> std::string {
                if (!evt) {
                    return {};
                }

                std::lock_guard<std::mutex> lock(evt->event_mutex_);
                return evt->type.getName();
            });

        // Adds the C-facing function SDOM_GetEventPhase() and metadata about its
        // C++ signature, return type, documentation, call wrapper, etc.
        registerMethod(
            typeName,
            "getPhase",
            "Event::Phase Event::getPhase() const",
            "SDOM_EventPhase",
            "SDOM_GetEventPhase",
            "SDOM_EventPhase SDOM_GetEventPhase(const SDOM_Event* evt)",
            "Returns the current propagation phase for this event.",
            [](const Event* evt) -> Event::Phase {
                if (!evt) {
                    return Event::Phase::Capture;
                }

                return evt->getPhase();
            });

        // Adds the C-facing function SDOM_SetEventPhase() and metadata about its
        // C++ signature, return type, documentation, call wrapper, etc.
        registerMethod(
            typeName,
            "setPhase",
            "Event& Event::setPhase(Event::Phase phase)",
            "bool",
            "SDOM_SetEventPhase",
            "bool SDOM_SetEventPhase(SDOM_Event* evt, SDOM_EventPhase phase)",
            "Sets the propagation phase for this event.",
            [](Event* evt, Event::Phase phase) -> bool {
                if (!evt) {
                    return false;
                }

                evt->setPhase(phase);
                return true;
            });

        registerMethod(
            typeName,
            "getPhaseString",
            "std::string Event::getPhaseString() const",
            "const char*",
            "SDOM_GetEventPhaseString",
            "const char* SDOM_GetEventPhaseString(const SDOM_Event* evt)",
            "Retrieves the textual description of the current dispatch phase.",
            [](const Event* evt) -> std::string {
                if (!evt) {
                    return {};
                }

                return evt->getPhaseString();
            });

        registerMethod(
            typeName,
            "getTargetHandle",
            "DisplayHandle Event::getTarget() const",
            "bool",
            "SDOM_GetEventTarget",
            "bool SDOM_GetEventTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target)",
            "Copies the event's target DisplayHandle into an SDOM_DisplayHandle snapshot.",
            [](const Event* evt, SDOM_DisplayHandle* outTarget) -> bool {
                if (!evt || !outTarget) {
                    return false;
                }

                std::lock_guard<std::mutex> lock(evt->event_mutex_);
                return exportDisplayHandleToC(evt->target, outTarget, s_eventTargetScratch);
            });

        registerMethod(
            typeName,
            "setTargetHandle",
            "Event& Event::setTarget(DisplayHandle newTarget)",
            "bool",
            "SDOM_SetEventTarget",
            "bool SDOM_SetEventTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target)",
            "Assigns a new DisplayHandle target snapshot to the event.",
            [](Event* evt, const SDOM_DisplayHandle* newTarget) -> bool {
                if (!evt) {
                    return false;
                }

                DisplayHandle targetHandle = importDisplayHandleFromC(newTarget);
                std::lock_guard<std::mutex> lock(evt->event_mutex_);
                evt->target = targetHandle;
                return true;
            });

        registerMethod(
            typeName,
            "getCurrentTargetHandle",
            "DisplayHandle Event::getCurrentTarget() const",
            "bool",
            "SDOM_GetEventCurrentTarget",
            "bool SDOM_GetEventCurrentTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target)",
            "Copies the current dispatch target DisplayHandle into an SDOM_DisplayHandle snapshot.",
            [](const Event* evt, SDOM_DisplayHandle* outTarget) -> bool {
                if (!evt || !outTarget) {
                    return false;
                }

                std::lock_guard<std::mutex> lock(evt->event_mutex_);
                return exportDisplayHandleToC(evt->current_target, outTarget, s_eventCurrentTargetScratch);
            });

        registerMethod(
            typeName,
            "setCurrentTargetHandle",
            "Event& Event::setCurrentTarget(DisplayHandle newCurrentTarget)",
            "bool",
            "SDOM_SetEventCurrentTarget",
            "bool SDOM_SetEventCurrentTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target)",
            "Assigns a new current dispatch target via an SDOM_DisplayHandle snapshot.",
            [](Event* evt, const SDOM_DisplayHandle* newTarget) -> bool {
                if (!evt) {
                    return false;
                }

                DisplayHandle targetHandle = importDisplayHandleFromC(newTarget);
                std::lock_guard<std::mutex> lock(evt->event_mutex_);
                evt->current_target = targetHandle;
                return true;
            });

        registerMethod(
            typeName,
            "getRelatedTargetHandle",
            "DisplayHandle Event::getRelatedTarget() const",
            "bool",
            "SDOM_GetEventRelatedTarget",
            "bool SDOM_GetEventRelatedTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target)",
            "Copies the related target DisplayHandle (if any) into an SDOM_DisplayHandle snapshot.",
            [](const Event* evt, SDOM_DisplayHandle* outTarget) -> bool {
                if (!evt || !outTarget) {
                    return false;
                }

                std::lock_guard<std::mutex> lock(evt->event_mutex_);
                return exportDisplayHandleToC(evt->related_target, outTarget, s_eventRelatedTargetScratch);
            });

        registerMethod(
            typeName,
            "setRelatedTargetHandle",
            "Event& Event::setRelatedTarget(DisplayHandle newRelatedTarget)",
            "bool",
            "SDOM_SetEventRelatedTarget",
            "bool SDOM_SetEventRelatedTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target)",
            "Assigns a new related target DisplayHandle snapshot on the event.",
            [](Event* evt, const SDOM_DisplayHandle* newTarget) -> bool {
                if (!evt) {
                    return false;
                }

                DisplayHandle targetHandle = importDisplayHandleFromC(newTarget);
                std::lock_guard<std::mutex> lock(evt->event_mutex_);
                evt->related_target = targetHandle;
                return true;
            });

        registerMethod(
            typeName,
            "getSDL_EventJson",
            "std::string Event::getSDL_EventJson() const",
            "const char*",
            "SDOM_GetEventSDLJson",
            "const char* SDOM_GetEventSDLJson(const SDOM_Event* evt)",
            "Serializes the embedded SDL_Event into a JSON snapshot for interop.",
            [](const Event* evt) -> std::string {
                if (!evt) {
                    return {};
                }

                return evt->getSDL_EventJson();
            });

        registerMethod(
            typeName,
            "setSDL_EventJson",
            "Event& Event::setSDL_EventJson(const std::string& json)",
            "bool",
            "SDOM_SetEventSDLJson",
            "bool SDOM_SetEventSDLJson(SDOM_Event* evt, const char* json)",
            "Reconstructs the embedded SDL_Event from a JSON snapshot.",
            [](Event* evt, const char* json) -> bool {
                if (!evt || !json) {
                    return false;
                }

                evt->setSDL_EventJson(json);
                return true;
            });

        // Variant-backed payload access (scalar-only for now)
        registerMethod(
            typeName,
            "getPayloadValueVariant",
            "Variant Event::getPayloadValueVariant(const std::string& key) const",
            "bool",
            "SDOM_GetEventPayloadValue",
            "bool SDOM_GetEventPayloadValue(const SDOM_Event* evt, const char* key, SDOM_Variant* out_value)",
            "Retrieves a payload entry as an SDOM_Variant (null/bool/int/float/string).",
            [](const Event* evt, const char* key, SDOM_Variant* outValue) -> bool {
                if (!evt || !key || !outValue) {
                    return false;
                }

                const Variant v = evt->getPayloadValueVariant(key);
                if (!evt->payloadKeyExists(key)) {
                    SDOM::CoreAPI::setErrorMessage("Payload key not found");
                    *outValue = exportVariantToC(Variant{});
                    return false;
                }

                switch (v.type()) {
                    case VariantType::Null:
                    case VariantType::Bool:
                    case VariantType::Int:
                    case VariantType::Real:
                    case VariantType::String:
                        break;
                    default:
                        SDOM::CoreAPI::setErrorMessage("Unsupported payload type for ABI access");
                        return false;
                }

                *outValue = exportVariantToC(v);
                return true;
            });

        registerMethod(
            typeName,
            "setPayloadValueVariant",
            "Event& Event::setPayloadValueVariant(const std::string& key, const Variant& value)",
            "bool",
            "SDOM_SetEventPayloadValue",
            "bool SDOM_SetEventPayloadValue(SDOM_Event* evt, const char* key, const SDOM_Variant* value)",
            "Sets a payload entry from an SDOM_Variant (null/bool/int/float/string).",
            [](Event* evt, const char* key, const SDOM_Variant* value) -> bool {
                if (!evt || !key || !value) {
                    return false;
                }

                Variant v;
                if (!importVariantFromC(value, v)) {
                    return false;
                }

                evt->setPayloadValueVariant(key, v);
                return true;
            });

        registerMethod(
            typeName,
            "isPropagationStopped",
            "bool Event::isPropagationStopped() const",
            "bool",
            "SDOM_IsEventPropagationStopped",
            "bool SDOM_IsEventPropagationStopped(const SDOM_Event* evt)",
            "Returns true if propagation has been stopped for this event.",
            [](const Event* evt) -> bool {
                if (!evt) {
                    return false;
                }

                return evt->isPropagationStopped();
            });

        registerMethod(
            typeName,
            "stopPropagation",
            "Event& Event::stopPropagation()",
            "bool",
            "SDOM_StopEventPropagation",
            "bool SDOM_StopEventPropagation(SDOM_Event* evt)",
            "Stops event propagation (capture/bubble) for this event instance.",
            [](Event* evt) -> bool {
                if (!evt) {
                    return false;
                }

                evt->stopPropagation();
                return true;
            });

        registerMethod(
            typeName,
            "isDefaultBehaviorDisabled",
            "bool Event::isDefaultBehaviorDisabled() const",
            "bool",
            "SDOM_IsEventDefaultBehaviorDisabled",
            "bool SDOM_IsEventDefaultBehaviorDisabled(const SDOM_Event* evt)",
            "Returns true if the event's default behavior has been disabled.",
            [](const Event* evt) -> bool {
                if (!evt) {
                    return false;
                }

                return evt->isDefaultBehaviorDisabled();
            });

        registerMethod(
            typeName,
            "setDisableDefaultBehavior",
            "Event& Event::setDisableDefaultBehavior(bool disable)",
            "bool",
            "SDOM_SetEventDisableDefaultBehavior",
            "bool SDOM_SetEventDisableDefaultBehavior(SDOM_Event* evt, bool disable)",
            "Enables or disables the event's default behavior.",
            [](Event* evt, bool disable) -> bool {
                if (!evt) {
                    return false;
                }

                evt->setDisableDefaultBehavior(disable);
                return true;
            });

        registerMethod(
            typeName,
            "getUseCapture",
            "bool Event::getUseCapture() const",
            "bool",
            "SDOM_GetEventUseCapture",
            "bool SDOM_GetEventUseCapture(const SDOM_Event* evt)",
            "Returns whether this event is currently using capture semantics.",
            [](const Event* evt) -> bool {
                if (!evt) {
                    return false;
                }

                return evt->getUseCapture();
            });

        registerMethod(
            typeName,
            "setUseCapture",
            "Event& Event::setUseCapture(bool useCapture)",
            "bool",
            "SDOM_SetEventUseCapture",
            "bool SDOM_SetEventUseCapture(SDOM_Event* evt, bool use_capture)",
            "Sets whether this event should use capture semantics during dispatch.",
            [](Event* evt, bool useCapture) -> bool {
                if (!evt) {
                    return false;
                }

                evt->setUseCapture(useCapture);
                return true;
            });

        registerMethod(
            typeName,
            "getElapsedTime",
            "float Event::getElapsedTime() const",
            "float",
            "SDOM_GetEventElapsedTime",
            "float SDOM_GetEventElapsedTime(const SDOM_Event* evt)",
            "Retrieves the elapsed time associated with this event in seconds.",
            [](const Event* evt) -> float {
                if (!evt) {
                    return 0.0f;
                }

                return evt->getElapsedTime();
            });

        registerMethod(
            typeName,
            "setElapsedTime",
            "Event& Event::setElapsedTime(float elapsedTime)",
            "bool",
            "SDOM_SetEventElapsedTime",
            "bool SDOM_SetEventElapsedTime(SDOM_Event* evt, float elapsed_time)",
            "Sets the elapsed time (in seconds) for this event instance.",
            [](Event* evt, float elapsedTime) -> bool {
                if (!evt) {
                    return false;
                }

                evt->setElapsedTime(elapsedTime);
                return true;
            });


        // ---------------------------------------------------------------------
        // Register Mouse Accessors
        // ---------------------------------------------------------------------
        auto registerMouseGetter = [&](const char* methodName,
                                       const char* cppSig,
                                       const char* cReturn,
                                       const char* cName,
                                       const char* cSig,
                                       const char* doc,
                                       auto getter) {
            registerMethod(
                typeName,
                methodName,
                cppSig,
                cReturn,
                cName,
                cSig,
                doc,
                getter);
        };

        auto registerMouseSetter = [&](const char* methodName,
                                       const char* cppSig,
                                       const char* cReturn,
                                       const char* cName,
                                       const char* cSig,
                                       const char* doc,
                                       auto setter) {
            registerMethod(
                typeName,
                methodName,
                cppSig,
                cReturn,
                cName,
                cSig,
                doc,
                setter);
        };

        registerMouseGetter(
            "getMouseX",
            "float Event::getMouseX() const",
            "float",
            "SDOM_GetEventMouseX",
            "float SDOM_GetEventMouseX(const SDOM_Event* evt)",
            "Retrieves the cached mouse X coordinate for the event.",
            [](const Event* evt) -> float {
                return evt ? evt->getMouseX() : 0.0f;
            });

        registerMouseSetter(
            "setMouseX",
            "Event& Event::setMouseX(float x)",
            "bool",
            "SDOM_SetEventMouseX",
            "bool SDOM_SetEventMouseX(SDOM_Event* evt, float x)",
            "Sets the mouse X coordinate for the event payload.",
            [](Event* evt, float x) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setMouseX(x);
                return true;
            });

        registerMouseGetter(
            "getMouseY",
            "float Event::getMouseY() const",
            "float",
            "SDOM_GetEventMouseY",
            "float SDOM_GetEventMouseY(const SDOM_Event* evt)",
            "Retrieves the cached mouse Y coordinate for the event.",
            [](const Event* evt) -> float {
                return evt ? evt->getMouseY() : 0.0f;
            });

        registerMouseSetter(
            "setMouseY",
            "Event& Event::setMouseY(float y)",
            "bool",
            "SDOM_SetEventMouseY",
            "bool SDOM_SetEventMouseY(SDOM_Event* evt, float y)",
            "Sets the mouse Y coordinate for the event payload.",
            [](Event* evt, float y) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setMouseY(y);
                return true;
            });

        registerMouseGetter(
            "getWheelX",
            "float Event::getWheelX() const",
            "float",
            "SDOM_GetEventWheelX",
            "float SDOM_GetEventWheelX(const SDOM_Event* evt)",
            "Returns the horizontal wheel delta collected for the event.",
            [](const Event* evt) -> float {
                return evt ? evt->getWheelX() : 0.0f;
            });

        registerMouseSetter(
            "setWheelX",
            "Event& Event::setWheelX(float x)",
            "bool",
            "SDOM_SetEventWheelX",
            "bool SDOM_SetEventWheelX(SDOM_Event* evt, float x)",
            "Sets the horizontal wheel delta in the payload.",
            [](Event* evt, float x) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setWheelX(x);
                return true;
            });

        registerMouseGetter(
            "getWheelY",
            "float Event::getWheelY() const",
            "float",
            "SDOM_GetEventWheelY",
            "float SDOM_GetEventWheelY(const SDOM_Event* evt)",
            "Returns the vertical wheel delta collected for the event.",
            [](const Event* evt) -> float {
                return evt ? evt->getWheelY() : 0.0f;
            });

        registerMouseSetter(
            "setWheelY",
            "Event& Event::setWheelY(float y)",
            "bool",
            "SDOM_SetEventWheelY",
            "bool SDOM_SetEventWheelY(SDOM_Event* evt, float y)",
            "Sets the vertical wheel delta in the payload.",
            [](Event* evt, float y) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setWheelY(y);
                return true;
            });

        registerMouseGetter(
            "getDragOffsetX",
            "float Event::getDragOffsetX() const",
            "float",
            "SDOM_GetEventDragOffsetX",
            "float SDOM_GetEventDragOffsetX(const SDOM_Event* evt)",
            "Provides the drag offset (X axis) for drag events.",
            [](const Event* evt) -> float {
                return evt ? evt->getDragOffsetX() : 0.0f;
            });

        registerMouseSetter(
            "setDragOffsetX",
            "Event& Event::setDragOffsetX(float offsetX)",
            "bool",
            "SDOM_SetEventDragOffsetX",
            "bool SDOM_SetEventDragOffsetX(SDOM_Event* evt, float offset_x)",
            "Stores the drag offset (X axis) into the event payload.",
            [](Event* evt, float offsetX) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setDragOffsetX(offsetX);
                return true;
            });

        registerMouseGetter(
            "getDragOffsetY",
            "float Event::getDragOffsetY() const",
            "float",
            "SDOM_GetEventDragOffsetY",
            "float SDOM_GetEventDragOffsetY(const SDOM_Event* evt)",
            "Provides the drag offset (Y axis) for drag events.",
            [](const Event* evt) -> float {
                return evt ? evt->getDragOffsetY() : 0.0f;
            });

        registerMouseSetter(
            "setDragOffsetY",
            "Event& Event::setDragOffsetY(float offsetY)",
            "bool",
            "SDOM_SetEventDragOffsetY",
            "bool SDOM_SetEventDragOffsetY(SDOM_Event* evt, float offset_y)",
            "Stores the drag offset (Y axis) into the event payload.",
            [](Event* evt, float offsetY) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setDragOffsetY(offsetY);
                return true;
            });

        registerMouseGetter(
            "getClickCount",
            "int Event::getClickCount() const",
            "int",
            "SDOM_GetEventClickCount",
            "int SDOM_GetEventClickCount(const SDOM_Event* evt)",
            "Returns the click count (single/double/etc.) stored in the event.",
            [](const Event* evt) -> int {
                return evt ? evt->getClickCount() : 0;
            });

        registerMouseSetter(
            "setClickCount",
            "Event& Event::setClickCount(int count)",
            "bool",
            "SDOM_SetEventClickCount",
            "bool SDOM_SetEventClickCount(SDOM_Event* evt, int count)",
            "Sets the click count metadata for the event payload.",
            [](Event* evt, int count) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setClickCount(count);
                return true;
            });

        registerMouseGetter(
            "getButton",
            "uint8_t Event::getButton() const",
            "uint8_t",
            "SDOM_GetEventButton",
            "uint8_t SDOM_GetEventButton(const SDOM_Event* evt)",
            "Returns the mouse button identifier that triggered the event.",
            [](const Event* evt) -> std::uint8_t {
                return evt ? evt->getButton() : 0u;
            });

        registerMouseSetter(
            "setButton",
            "Event& Event::setButton(uint8_t btn)",
            "bool",
            "SDOM_SetEventButton",
            "bool SDOM_SetEventButton(SDOM_Event* evt, uint8_t button)",
            "Sets the mouse button identifier for this event payload.",
            [](Event* evt, std::uint8_t button) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setButton(button);
                return true;
            });


        // ---------------------------------------------------------------------
        // Register Keyboard Accessors
        // ---------------------------------------------------------------------
        registerMethod(
            typeName,
            "getScanCode",
            "SDL_Scancode Event::getScanCode() const",
            "int",
            "SDOM_GetEventScanCode",
            "int SDOM_GetEventScanCode(const SDOM_Event* evt)",
            "Retrieves the SDL scancode associated with the keyboard event.",
            [](const Event* evt) -> int {
                return evt ? static_cast<int>(evt->getScanCode()) : 0;
            });

        registerMethod(
            typeName,
            "setScanCode",
            "Event& Event::setScanCode(SDL_Scancode scancode)",
            "bool",
            "SDOM_SetEventScanCode",
            "bool SDOM_SetEventScanCode(SDOM_Event* evt, int scancode)",
            "Sets the SDL scancode metadata for the keyboard event.",
            [](Event* evt, int scancode) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setScanCode(static_cast<SDL_Scancode>(scancode));
                return true;
            });

        registerMethod(
            typeName,
            "getKeycode",
            "SDL_Keycode Event::getKeycode() const",
            "int",
            "SDOM_GetEventKeycode",
            "int SDOM_GetEventKeycode(const SDOM_Event* evt)",
            "Retrieves the SDL keycode value stored on the event.",
            [](const Event* evt) -> int {
                return evt ? static_cast<int>(evt->getKeycode()) : 0;
            });

        registerMethod(
            typeName,
            "setKeycode",
            "Event& Event::setKeycode(SDL_Keycode keycode)",
            "bool",
            "SDOM_SetEventKeycode",
            "bool SDOM_SetEventKeycode(SDOM_Event* evt, int keycode)",
            "Sets the SDL keycode value for the event payload.",
            [](Event* evt, int keycode) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setKeycode(static_cast<SDL_Keycode>(keycode));
                return true;
            });

        registerMethod(
            typeName,
            "getKeymod",
            "Uint16 Event::getKeymod() const",
            "int",
            "SDOM_GetEventKeymod",
            "int SDOM_GetEventKeymod(const SDOM_Event* evt)",
            "Returns the SDL key modifier mask stored on the event.",
            [](const Event* evt) -> int {
                return evt ? static_cast<int>(evt->getKeymod()) : 0;
            });

        registerMethod(
            typeName,
            "setKeymod",
            "Event& Event::setKeymod(Uint16 keymod)",
            "bool",
            "SDOM_SetEventKeymod",
            "bool SDOM_SetEventKeymod(SDOM_Event* evt, int keymod)",
            "Sets the key modifier mask for the keyboard event.",
            [](Event* evt, int keymod) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setKeymod(static_cast<Uint16>(keymod));
                return true;
            });

        registerMethod(
            typeName,
            "getAsciiCode",
            "int Event::getAsciiCode() const",
            "int",
            "SDOM_GetEventAsciiCode",
            "int SDOM_GetEventAsciiCode(const SDOM_Event* evt)",
            "Returns the translated ASCII code (if available) for the keypress.",
            [](const Event* evt) -> int {
                return evt ? evt->getAsciiCode() : 0;
            });

        registerMethod(
            typeName,
            "setAsciiCode",
            "Event& Event::setAsciiCode(int asciiCode)",
            "bool",
            "SDOM_SetEventAsciiCode",
            "bool SDOM_SetEventAsciiCode(SDOM_Event* evt, int ascii_code)",
            "Sets the translated ASCII code for the keyboard event payload.",
            [](Event* evt, int asciiCode) -> bool {
                if (!evt) {
                    return false;
                }
                evt->setAsciiCode(asciiCode);
                return true;
            });


        // -------------------------------------------------------------------------
        // Register Enum Values for SDOM_EventPhase
        // -------------------------------------------------------------------------
        struct PhaseEnumDesc {
            Phase value;
            const char* name;
            const char* doc;
        };

        static constexpr PhaseEnumDesc kPhaseEnumValues[] = {
            { Phase::Capture, "Capture", "Event is traveling from the root down toward the target (capture phase)." },
            { Phase::Target, "Target", "Event has reached the target object; handlers on the target run here." },
            { Phase::Bubbling, "Bubbling", "Event is traveling upward from the target back toward the root (bubble phase)." }
        };

        for (const auto& entry : kPhaseEnumValues)
        {
            registerEnumValue(
                "SDOM_EventPhase",
                "Event",
                entry.name,
                static_cast<std::uint32_t>(entry.value),
                entry.doc,
                "Event Phase");
        }

        // -------------------------------------------------------------------------
        // Register Enum Values for SDOM_EventType
        // -------------------------------------------------------------------------
        // Iterates over every EventType instance known to the system, assigns IDs,
        // registers enum metadata for C bindings and scripting, and attaches
        // additional boolean properties to describe event behavior (bubbles, etc.)
        for (EventType* et : EventType::getAll())
        {
            if (!et) {
                continue;
            }

            EventType& typeRef = *et;
            const auto id = typeRef.getOrAssignId();

            registerEnumValue(
                "SDOM_EventType",
                "Event",
                typeRef.getName(),
                id,
                typeRef.getDoc(),
                typeRef.getCategory(),
                [](SDOM::TypeInfo& ti) {
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
                });
        }

    } // END Event::registerBindingsImpl(const std::string& typeName)


} // namespace SDOM