/***  SDOM_EventType.cpp  ****************************
 *    ___ ___   ___  __  __   ___             _  _____                              
 *   / __|   \ / _ \|  \/  | | __|_ _____ _ _| ||_   _|  _ _ __  ___   __ _ __ _ __ 
 *   \__ \ |) | (_) | |\/| | | _|\ V / -_) ' \  _|| || || | '_ \/ -_)_/ _| '_ \ '_ \
 *   |___/___/ \___/|_|  |_|_|___|\_/\___|_||_\__||_| \_, | .__/\___(_)__| .__/ .__/
 *                        |___|                       |__/|_|            |_|  |_|    
 *  
 * The SDOM_EventType class defines and manages the various types of events that can 
 * be dispatched and handled within the SDOM framework. It provides a flexible system 
 * for registering both predefined and custom event types, each with configurable 
 * propagation properties such as capturing, bubbling, target-only, and global scope. 
 * EventType instances are used to categorize and identify events throughout the event 
 * system, enabling robust event-driven programming and fine-grained control over event 
 * flow. By supporting both standard UI events (mouse, keyboard, window, etc.) and 
 * user-defined events, the EventType class forms the foundation for extensible and 
 * scalable event handling in SDOM-based applications.
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
#include <SDOM/SDOM_EventType.hpp>
#include <mutex>

namespace SDOM
{
    static std::mutex s_eventtype_id_mutex;




    // Numeric ID helpers
    EventType::IdType EventType::getOrAssignId()
    {
        // Fast path
        if (id_ != 0) return id_;

        // assign a new id atomically
        EventType::IdType newId = next_event_type_id.fetch_add(1, std::memory_order_relaxed);
        if (newId == 0) { // wrap-around safety: skip 0
            newId = next_event_type_id.fetch_add(1, std::memory_order_relaxed);
        }

        // store mapping under mutex to protect idRegistry
        {
            std::lock_guard<std::mutex> lk(s_eventtype_id_mutex);
            id_ = newId;
            idRegistry[id_] = this;
        }
        return id_;
    }

    void EventType::setId(IdType id)
    {
        if (id == 0) return;
        std::lock_guard<std::mutex> lk(s_eventtype_id_mutex);
        id_ = id;
        idRegistry[id_] = this;
    }

    EventType* EventType::fromId(IdType id)
    {
        if (id == 0) return nullptr;
        std::lock_guard<std::mutex> lk(s_eventtype_id_mutex);
        auto it = idRegistry.find(id);
        if (it != idRegistry.end()) return it->second;
        return nullptr;
    }
    // Metering Policy Notes
    // ---------------------
    // Each EventType may define optional metering behavior used by the
    // EventManager to coalesce high-frequency events prior to enqueue:
    //   - critical:        never metered; flush coalesced queue before enqueue
    //   - meter_enabled:   enable coalescing for this type
    //   - meter_interval_ms: min interval between enqueues (0 = use default)
    //   - coalesce_strategy:
    //       * None: do not coalesce
    //       * Last: keep only the last event (e.g., MouseMove, Resize)
    //       * Sum:  accumulate deltas (e.g., MouseWheel)
    //       * Count:increment only (unused currently)
    //   - coalesce_key:
    //       * Global: single slot for the type (last-wins/sum across all targets)
    //       * ByTarget: one slot per target (for target-sensitive metering)
    //
    //              // captures, bubbles, target, global
    // 🚫 Untestable ---------------------------------------------------------------
    EventType EventType::None("None", false, false, false, false);
    EventType EventType::Quit("Quit", false, false, false, true);

    // 💻 Application Lifecycle ----------------------------------------------------
    EventType EventType::Added("Added", true, true, false, false);
    EventType EventType::Removed("Removed", true, true, false, false);
    EventType EventType::AddedToStage("AddedToStage", true, true, false, false);
    EventType EventType::RemovedFromStage("RemovedFromStage", true, true, false, false);
    EventType EventType::StageOpened("StageOpened", false, false, false, true);
    EventType EventType::StageClosed("StageClosed", false, false, false, true);

    // 💻 Input --------------------------------------------------------------------
    // Keyboard
    EventType EventType::KeyDown("KeyDown", true, true, false, false);
    EventType EventType::KeyUp("KeyUp", true, true, false, false);
    EventType EventType::TextInput("TextInput", true, true, false, false);

    // Mouse
    EventType EventType::MouseButtonDown("MouseButtonDown", true, true, false, false);
    EventType EventType::MouseButtonUp("MouseButtonUp", true, true, false, false);
    EventType EventType::MouseWheel("MouseWheel", true, true, false, false);
    EventType EventType::MouseMove("MouseMove", false, false, true, false);
    EventType EventType::MouseClick("MouseClick", true, true, false, false);
    EventType EventType::MouseDoubleClick("MouseDoubleClick", true, true, false, false);
    // Make MouseEnter/MouseLeave observable by ancestors (capture/bubble enabled,
    // not target-only) so parent containers can react to hover transitions.
    EventType EventType::MouseEnter("MouseEnter", true, true, false, false);
    EventType EventType::MouseLeave("MouseLeave", true, true, false, false);

    // 💻 Window / Focus -----------------------------------------------------------
    // ⚠️ Focus events are compositor-controlled on Wayland and may not be delivered
    EventType EventType::FocusGained("FocusGained", false, false, true, false);  // ⚠️ Wayland‑gated
    EventType EventType::FocusLost("FocusLost",   false, false, true, false);    // ⚠️ Wayland‑gated
    // 🔄 Resize/Show/Hide usually work normally across backends
    EventType EventType::Resize("Resize",         true,  true,  false, false);   // 🔄 Wayland‑normal
    // ⚠️ Move events are often suppressed on Wayland for programmatic reposition
    EventType EventType::Move("Move",             true,  true,  false, false);   // ⚠️ Wayland‑gated
    EventType EventType::Show("Show",             false, false, true,  false);   // 🔄 Wayland‑normal
    EventType EventType::Hide("Hide",             false, false, true,  false);   // 🔄 Wayland‑normal
    // ✅ Fullscreen enter/leave verified to work without gating
    EventType EventType::EnterFullscreen("EnterFullscreen", true, true,  false, false); // ✅
    EventType EventType::LeaveFullscreen("LeaveFullscreen", true, true,  false, false); // ✅

    // 💻 UI / State ---------------------------------------------------------------
    EventType EventType::ValueChanged("ValueChanged", true, true, false, false);
    EventType EventType::StateChanged("StateChanged", true, true, false, false);
    EventType EventType::SelectionChanged("SelectionChanged", true, true, false, false);
    EventType EventType::Enabled("Enabled", false, false, true, false);
    EventType EventType::Disabled("Disabled", false, false, true, false);
    EventType EventType::Visible("Visible", true, true, false, false);
    EventType EventType::Hidden("Hidden", true, true, false, false);

    // 💻 Drag & Drop --------------------------------------------------------------
    EventType EventType::Drag("Drag", true, true, false, false);
    EventType EventType::Dragging("Dragging", true, true, false, false);
    EventType EventType::Drop("Drop", true, true, false, false);

    // ❓ Timer (no Timer system yet) ----------------------------------------------
    EventType EventType::TimerStart("TimerStart", false, false, false, false);
    EventType EventType::TimerStop("TimerStop", false, false, false, false);
    EventType EventType::TimerPause("TimerPause", false, false, false, false);
    EventType EventType::TimerTick("TimerTick", false, false, false, false);
    EventType EventType::TimerCycleComplete("TimerCycleComplete", false, false, false, false);
    EventType EventType::TimerComplete("TimerComplete", false, false, false, false);

    // ❓ Clipboard (clipboard sub-system not yet implemented) ---------------------
    EventType EventType::ClipboardCopy("ClipboardCopy", true, true, false, false);
    EventType EventType::ClipboardPaste("ClipboardPaste", true, true, false, false);

    // 💻 Listener-Only ------------------------------------------------------------
    EventType EventType::OnInit("OnInit", true, true, false, false);
    EventType EventType::OnQuit("OnQuit", false, false, false, true); // global only
    EventType EventType::OnEvent("OnEvent", true, true, false, false);
    EventType EventType::OnUpdate("OnUpdate", true, true, false, false);
    EventType EventType::OnRender("OnRender", true, true, false, false);
    EventType EventType::OnPreRender("OnPreRender", false, false, false, false); // listeners-only, non-capturing/non-bubbling

    // 💻 Frame / Misc -------------------------------------------------------------
    // EnterFrame exists for legacy compatibility but is not emitted by Core.
    // Prefer OnUpdate for per-frame logic. Keeping the type allows tests and
    // Lua to reference it without generating event spam.
    EventType EventType::EnterFrame("EnterFrame", false, false, false, false);
    EventType EventType::SDL_Event("SDL_Event", true, true, false, false);

    // --- Default metering policy assignments --- //
    namespace {
    bool init_eventtype_policies() {
        // Critical (never coalesced; flush coalesced queue before enqueue):
        EventType::MouseButtonDown.setCritical(true);
        EventType::MouseButtonUp.setCritical(true);
        EventType::Drag.setCritical(true);
        EventType::Dragging.setCritical(true);
        EventType::Drop.setCritical(true);
        EventType::KeyDown.setCritical(true);
        EventType::KeyUp.setCritical(true);
        EventType::TextInput.setCritical(true);
        EventType::StageOpened.setCritical(true);
        EventType::StageClosed.setCritical(true);

        // Metered (10 ms default via SDOM_EVENT_METER_MS_DEFAULT):
        EventType::MouseMove
            .setMeterEnabled(true)
            .setCoalesceStrategy(EventType::CoalesceStrategy::Last)
            .setCoalesceKey(EventType::CoalesceKey::Global);

        EventType::MouseWheel
            .setMeterEnabled(true)
            .setCoalesceStrategy(EventType::CoalesceStrategy::Sum)
            .setCoalesceKey(EventType::CoalesceKey::Global);

        EventType::Resize
            .setMeterEnabled(true)
            .setCoalesceStrategy(EventType::CoalesceStrategy::Last)
            .setCoalesceKey(EventType::CoalesceKey::Global);

        EventType::Move
            .setMeterEnabled(true)
            .setCoalesceStrategy(EventType::CoalesceStrategy::Last)
            .setCoalesceKey(EventType::CoalesceKey::Global);

        return true;
    }
    const bool s_eventtype_policies_initialized = init_eventtype_policies();
    } // anonymous namespace
    EventType EventType::User("User", true, true, false, false);


    void EventType::registerLua(sol::state_view lua)
    {
        try {
            // Create a userdata type for EventType values so Lua sees instances
            // as first-class objects. Don't rely on the global `EventType` slot
            // to indicate whether the usertype was registered because we later
            // export a table into that same global name and that would cause
            // a race/ordering bug where the usertype registration is skipped.
            // Use a static guard to register the usertype exactly once.
            static bool s_eventtype_usertype_registered = false;
            if (!s_eventtype_usertype_registered) {
                lua.new_usertype<EventType>("EventType", sol::no_constructor,
                    // expose readable properties (idiomatic from Lua use)
                    "name", sol::property(&EventType::getName),
                    "captures", sol::property(&EventType::getCaptures),
                    "bubbles", sol::property(&EventType::getBubbles),
                    "targetOnly", sol::property(&EventType::getTargetOnly),
                    "global", sol::property(&EventType::getGlobal),
                    // tostring for pretty printing
                    sol::meta_function::to_string, [](const EventType& et) { return et.getName(); }
                );
                s_eventtype_usertype_registered = true;
            }

            // Populate a table with references to the static EventType instances
            sol::table etbl = lua.create_table();
            const auto& reg = EventType::getRegistry();
            for (const auto& kv : reg)
            {
                const std::string& name = kv.first;
                EventType* ptr = kv.second;
                if (ptr)
                {
                    // create a Lua userdata that references the C++ instance so
                    // Lua sees the registered properties (uses the EventType usertype)
                    etbl[name] = sol::make_object(lua, std::ref(*ptr));
                }
            }

            // Make the table callable: EventType("MouseClick") -> returns entry or nil
            sol::table meta = lua.create_table();
            meta[sol::meta_function::call] = [](sol::table t, const std::string& name) -> sol::object {
                // use table:get to return a sol::object (may be nil) safely
                return t.get<sol::object>(name);
            };
            etbl[sol::metatable_key] = meta;

            // Allow runtime registration from Lua: EventType.register(name, captures, bubbles, targetOnly, global)
            // Capture the table so we can insert the created EventType into it immediately.
            etbl.set_function("register", [etbl](sol::this_state ts, const std::string& name, bool captures, bool bubbles, bool targetOnly, bool global) mutable -> sol::object {
                sol::state_view lua(ts);
                // allocate a new EventType and let its constructor register it
                EventType* et = new EventType(name, captures, bubbles, targetOnly, global);
                // insert into the lua-side table as a userdata referencing the C++ instance
                try {
                    sol::object obj = sol::make_object(lua, std::ref(*et));
                    etbl[name] = obj;
                    return obj;
                } catch (...) {
                    // ignore table set failures; the C++ registry still contains the type
                    return sol::nil;
                }
            });

            // Export the table as the global `EventType` module
            lua["EventType"] = etbl;
        } catch (...) {
            // non-fatal
            WARNING("Failed to register EventType with Lua");
        }
        
    } // END EventType::registerLua(sol::state_view lua)



    // getters
    bool EventType::getCaptures() const 
    { 
        return captures_; 
    }
    bool EventType::getBubbles() const     
    { 
        return bubbles_; 
    }
    bool EventType::getTargetOnly() const 
    { 
        return targetOnly_; 
    }
    bool EventType::getGlobal() const 
    { 
        return global_; 
    }



    // setters
    EventType& EventType::setCaptures(bool captures) 
    { 
        captures_ = captures; 
        return *this; 
    }
    EventType& EventType::setBubbles(bool bubbles) 
    { 
        bubbles_ = bubbles; 
        return *this; 
    }
    EventType& EventType::setTargetOnly(bool targetOnly) 
    { 
        targetOnly_ = targetOnly; 
        return *this; 
    }
    EventType& EventType::setGlobal(bool global) 
    { 
        global_ = global; 
        return *this; 
    }

    // --- Metering policy getters/setters --- //
    bool EventType::isCritical() const { return critical_; }
    bool EventType::isMeterEnabled() const { return meter_enabled_; }
    uint16_t EventType::getMeterIntervalMs() const { return meter_interval_ms_; }
    EventType::CoalesceStrategy EventType::getCoalesceStrategy() const { return coalesce_strategy_; }
    EventType::CoalesceKey EventType::getCoalesceKey() const { return coalesce_key_; }

    EventType& EventType::setCritical(bool critical) { critical_ = critical; return *this; }
    EventType& EventType::setMeterEnabled(bool enabled) { meter_enabled_ = enabled; return *this; }
    EventType& EventType::setMeterIntervalMs(uint16_t ms) { meter_interval_ms_ = ms; return *this; }
    EventType& EventType::setCoalesceStrategy(CoalesceStrategy s) { coalesce_strategy_ = s; return *this; }
    EventType& EventType::setCoalesceKey(CoalesceKey k) { coalesce_key_ = k; return *this; }

    // -- lookup helpers --
    std::vector<EventType*> EventType::getAll()
    {
        std::vector<EventType*> out;
        out.reserve(registry.size());
        for (const auto &kv : registry) out.push_back(kv.second);
        return out;
    }

    EventType* EventType::fromName(const std::string& name)
    {
        auto it = registry.find(name);
        if (it != registry.end()) return it->second;
        return nullptr;
    }

    std::string EventType::toString(const EventType& et)
    {
        return et.getName();
    }

    size_t EventType::count()
    {
        return registry.size();
    }

} // namespace SDOM
