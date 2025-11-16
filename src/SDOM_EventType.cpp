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
// Force use of repository header to avoid picking up an installed copy.
#include "../include/SDOM/SDOM_EventType.hpp"
#include <mutex>
#include <memory>


#include <SDOM/SDOM_Utils.hpp>

namespace SDOM
{
    extern Core* g_core;  // temporarty global for getCore()



    static std::mutex s_eventtype_id_mutex;

    // Per-category block allocator configuration
    static constexpr unsigned SDOM_EVENT_ID_SHIFT = 8; // 256 ids per category
    static constexpr uint32_t SDOM_EVENT_BLOCK_SIZE = (1u << SDOM_EVENT_ID_SHIFT);
    static constexpr uint32_t SDOM_EVENT_LOCAL_MAX = SDOM_EVENT_BLOCK_SIZE - 1;

    // category -> index and per-category local id counters
    static std::unordered_map<std::string, unsigned> s_category_index;
    static std::vector<std::unique_ptr<std::atomic<uint32_t>>> s_next_local_id;




    // Numeric ID helpers using per-category blocks
    static unsigned getOrAssignCategoryIndex(const std::string &cat)
    {
        std::lock_guard<std::mutex> lk(s_eventtype_id_mutex);
        auto it = s_category_index.find(cat);
        if (it != s_category_index.end()) { return it->second; }
        unsigned idx = static_cast<unsigned>(s_category_index.size());
        s_category_index.emplace(cat, idx);
        s_next_local_id.emplace_back(std::make_unique<std::atomic<uint32_t>>(0)); // start local ids at 0 (allow 0 as sentinel in Core)
        return idx;
    }

    EventType::IdType EventType::getOrAssignId()
    {
        // Fast path
        if (id_ != 0) { return id_; }

        // Determine category index (stable per-run based on first-seen order)
        std::string cat = category_.empty() ? std::string("Uncategorized") : category_;
        unsigned cidx = getOrAssignCategoryIndex(cat);

        // allocate local id atomically for this category
        uint32_t local = s_next_local_id[cidx]->fetch_add(1, std::memory_order_relaxed);

        // handle overflow: if local exceeds local max, fall back to linear allocation
        uint32_t newId = 0;
        if (local > SDOM_EVENT_LOCAL_MAX) {
            std::lock_guard<std::mutex> lk(s_eventtype_id_mutex);
            // fallback: find next unused id after this category's block
            uint32_t base = (static_cast<uint32_t>(cidx + 1) << SDOM_EVENT_ID_SHIFT);
            uint32_t probe = base;
            while (idRegistry.find(probe) != idRegistry.end()) { ++probe; }
            newId = probe;
            id_ = newId;
            idRegistry[id_] = this;
            return id_;
        }

        newId = (static_cast<uint32_t>(cidx) << SDOM_EVENT_ID_SHIFT) | (local & SDOM_EVENT_LOCAL_MAX);

        {
            std::lock_guard<std::mutex> lk(s_eventtype_id_mutex);
            // collision check (should be rare)
            if (idRegistry.find(newId) != idRegistry.end()) {
                uint32_t probe = newId;
                while (idRegistry.find(++probe) != idRegistry.end()) { }
                newId = probe;
            }
            id_ = newId;
            idRegistry[id_] = this;
        }
        return id_;
    }

    void EventType::setId(IdType id)
    {
        if (id == 0) { return; }
        std::lock_guard<std::mutex> lk(s_eventtype_id_mutex);
        id_ = id;
        idRegistry[id_] = this;
    }

    EventType* EventType::fromId(IdType id)
    {
        if (id == 0) { return nullptr; }
        std::lock_guard<std::mutex> lk(s_eventtype_id_mutex);
        auto it = idRegistry.find(id);
        if (it != idRegistry.end()) { return it->second; }
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

    // 🧩 Core / General --------------------------------------------------------------
    EventType EventType::None("None", "Core", false, false, false, false,
        "A general-purpose EventType for testing or to represent a non-event.");

    EventType EventType::Quit("Quit", "Core", false, false, false, true,
        "Signals that the application or main stage is closing. Global only.");

    // 💻 Application Lifecycle ------------------------------------------------------
    EventType EventType::Added("Added", "Application", true, true, false, false,
        "Dispatched when an object is added as a child to another display object.");
    EventType EventType::Removed("Removed", "Application", true, true, false, false,
        "Dispatched when an object is removed from its parent container.");
    EventType EventType::AddedToStage("AddedToStage", "Application", true, true, false, false,
        "Emitted when an object becomes part of the active stage hierarchy.");
    EventType EventType::RemovedFromStage("RemovedFromStage", "Application", true, true, false, false,
        "Emitted when an object is detached from the stage hierarchy.");
    EventType EventType::StageOpened("StageOpened", "Application", false, false, false, true,
        "Indicates that a new stage or window has been opened and initialized.");
    EventType EventType::StageClosed("StageClosed", "Application", false, false, false, true,
        "Indicates that a stage or window has been closed and destroyed.");

    // 💻 Input ----------------------------------------------------------------------
    // Keyboard
    EventType EventType::KeyDown("KeyDown", "Input", true, true, false, false,
        "Keyboard key pressed down; bubbles through active hierarchy.");
    EventType EventType::KeyUp("KeyUp", "Input", true, true, false, false,
        "Keyboard key released; bubbles through active hierarchy.");
    EventType EventType::TextInput("TextInput", "Input", true, true, false, false,
        "Text input event carrying UTF-8 text from the input system.");

    // Mouse
    EventType EventType::MouseButtonDown("MouseButtonDown", "Input", true, true, false, false,
        "Mouse button pressed on a target object.");
    EventType EventType::MouseButtonUp("MouseButtonUp", "Input", true, true, false, false,
        "Mouse button released over a target object.");
    EventType EventType::MouseWheel("MouseWheel", "Input", true, true, false, false,
        "Mouse wheel scrolled; carries wheel delta values.");
    EventType EventType::MouseMove("MouseMove", "Input", false, false, true, false,
        "Mouse moved within or over a target object (target-only).");
    EventType EventType::MouseClick("MouseClick", "Input", true, true, false, false,
        "Mouse button clicked (press + release) on a target object.");
    EventType EventType::MouseDoubleClick("MouseDoubleClick", "Input", true, true, false, false,
        "Mouse double-clicked on a target object.");
    EventType EventType::MouseEnter("MouseEnter", "Input", true, true, false, false,
        "Pointer entered the bounds of an object; bubbles for hover tracking.");
    EventType EventType::MouseLeave("MouseLeave", "Input", true, true, false, false,
        "Pointer left the bounds of an object; bubbles for hover tracking.");

    // 💻 Window / Focus -------------------------------------------------------------
    // ⚠️ Focus events are compositor-controlled on Wayland and may not always fire
    EventType EventType::FocusGained("FocusGained", "Window", false, false, true, false,
        "The window or stage gained input focus.");
    EventType EventType::FocusLost("FocusLost", "Window", false, false, true, false,
        "The window or stage lost input focus.");
    EventType EventType::Resize("Resize", "Window", true, true, false, false,
        "Window or stage resized; width and height values updated.");
    EventType EventType::Move("Move", "Window", true, true, false, false,
        "Window or stage moved; position values updated.");
    EventType EventType::Show("Show", "Window", false, false, true, false,
        "Window or object became visible on screen.");
    EventType EventType::Hide("Hide", "Window", false, false, true, false,
        "Window or object hidden from view.");
    EventType EventType::EnterFullscreen("EnterFullscreen", "Window", true, true, false, false,
        "Application entered fullscreen mode.");
    EventType EventType::LeaveFullscreen("LeaveFullscreen", "Window", true, true, false, false,
        "Application exited fullscreen mode.");

    // 💻 UI / State -----------------------------------------------------------------
    EventType EventType::ValueChanged("ValueChanged", "UI", true, true, false, false,
        "Value of a control or property has changed.");
    EventType EventType::StateChanged("StateChanged", "UI", true, true, false, false,
        "UI element or component state changed (e.g., active, toggled).");
    EventType EventType::SelectionChanged("SelectionChanged", "UI", true, true, false, false,
        "User selection or highlight changed within a list or group.");
    EventType EventType::Enabled("Enabled", "UI", false, false, true, false,
        "Object or control has been enabled and can now receive input.");
    EventType EventType::Disabled("Disabled", "UI", false, false, true, false,
        "Object or control has been disabled and can no longer receive input.");
    EventType EventType::Visible("Visible", "UI", true, true, false, false,
        "Object became visible within its parent hierarchy.");
    EventType EventType::Hidden("Hidden", "UI", true, true, false, false,
        "Object became hidden within its parent hierarchy.");

    // 💻 Drag & Drop ---------------------------------------------------------------
    EventType EventType::Drag("Drag", "DragAndDrop", true, true, false, false,
        "Dragging in progress; position updated while dragging.");
    EventType EventType::Dragging("Dragging", "DragAndDrop", true, true, false, false,
        "Continuous drag event emitted while dragging is active.");
    EventType EventType::Drop("Drop", "DragAndDrop", true, true, false, false,
        "An item or data payload was dropped onto a valid target.");

    // ⏱️ Timer ----------------------------------------------------------------------
    EventType EventType::TimerStart("TimerStart", "Timer", false, false, false, false,
        "Timer started or resumed counting.");
    EventType EventType::TimerStop("TimerStop", "Timer", false, false, false, false,
        "Timer stopped and reset to initial state.");
    EventType EventType::TimerPause("TimerPause", "Timer", false, false, false, false,
        "Timer paused but not reset.");
    EventType EventType::TimerTick("TimerTick", "Timer", false, false, false, false,
        "Timer tick event; emitted on each interval step.");
    EventType EventType::TimerCycleComplete("TimerCycleComplete", "Timer", false, false, false, false,
        "Timer completed one full interval cycle.");
    EventType EventType::TimerComplete("TimerComplete", "Timer", false, false, false, false,
        "Timer finished all cycles and has reached completion.");

    // 📋 Clipboard ------------------------------------------------------------------
    EventType EventType::ClipboardCopy("ClipboardCopy", "Clipboard", true, true, false, false,
        "Data copied to the system clipboard.");
    EventType EventType::ClipboardPaste("ClipboardPaste", "Clipboard", true, true, false, false,
        "Data pasted from the system clipboard.");

    // 💻 Listener / Callback --------------------------------------------------------
    EventType EventType::OnInit("OnInit", "Listener", true, true, false, false,
        "Initialization callback for objects; called after creation.");
    EventType EventType::OnQuit("OnQuit", "Listener", false, false, false, true,
        "Global callback fired during shutdown sequence.");
    EventType EventType::OnEvent("OnEvent", "Listener", true, true, false, false,
        "Generic hook for catching all events before normal dispatch.");
    EventType EventType::OnUpdate("OnUpdate", "Listener", true, true, false, false,
        "Called once per frame before rendering; main update loop hook.");
    EventType EventType::OnRender("OnRender", "Listener", true, true, false, false,
        "Called each frame when object should perform its rendering.");
    EventType EventType::OnPreRender("OnPreRender", "Listener", false, false, false, false,
        "Called before OnRender for pre-draw logic such as layout or transforms.");

    // 💻 Frame / Misc ---------------------------------------------------------------
    EventType EventType::EnterFrame("EnterFrame", "Frame", false, false, false, false,
        "Legacy per-frame tick event (use OnUpdate instead).");
    EventType EventType::SDL_Event("SDL_Event", "Frame", true, true, false, false,
        "Raw SDL event wrapper; dispatched when unhandled by internal systems.");
    EventType EventType::User("User", "Frame", true, true, false, false,
        "Reserved for custom or user-defined events created at runtime.");

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
        // Use the insertion-ordered registry vector to preserve definition order
            for (EventType* et : registry_order) { out.push_back(et); }
        return out;
    }

    EventType* EventType::fromName(const std::string& name)
    {
        auto it = registry.find(name);
        if (it != registry.end()) { return it->second; }
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

    std::string EventType::getDoc() const
    {
        return doc_;
    }

    void EventType::setDoc(const std::string& s)
    {
        doc_ = s;
    }

} // namespace SDOM
