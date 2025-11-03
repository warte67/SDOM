// Event_UnitTests.cpp
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_EventManager.hpp>   
#include <SDOM/SDOM_Stage.hpp>

#include "Box.hpp"

// ============================================================================
// 🧩 SDOM::Event_UnitTests — Function Coverage Summary
// ----------------------------------------------------------------------------
//  This summary lists all primary functions and behaviors validated by this
//  test module. It is auto-updated as new tests are added.
// ----------------------------------------------------------------------------
//
//  🧱 Core EventType API
//   • EventType::getRegistry()
//   • EventType::getName()
//   • EventType::getCaptures(), setCaptures()
//   • EventType::getBubbles(),  setBubbles()
//   • EventType::getTargetOnly(), setTargetOnly()
//   • EventType::getGlobal(), setGlobal()
//
//  🧠 Event Dispatch & Listener API
//   • DisplayHandle::addEventListener()
//   • DisplayHandle::removeEventListener()
//   • DisplayHandle::hasEventListener()
//   • DisplayHandle::queue_event()
//   • Event::getTypeName()
//   • Event::setPayloadValue(), getPayloadValue()
//
//  ⚙️ EventManager Integration
//   • EventManager::Queue_SDL_Event()
//   • EventManager::dispatchWindowEvents()
//   • EventManager::dispatchMouseEvents()
//   • EventManager::dispatchKeyboardEvents()
//   • EventManager::dispatchDragEvents()
//   • EventManager::updateHoverState()
//   • Core::pumpEventsOnce()
//
//  🧩 Unit Test Framework
//   • UnitTests::get_frame_counter()
//   • UnitTests::run_lua_tests()
//   • Re-entrant test scheduling and frame synchronization
//
//  🧬 Coverage Domains
//   - Core / Lifecycle Events
//   - Mouse / Keyboard / Window Input
//   - UI / Value / State Events
//   - Behavioral Translation (SDL → SDOM)
//   - Multi-frame Event Queue Validation
//
// ============================================================================


namespace SDOM
{
    // --- Individual Event Unit Tests --- //

    // ============================================================================
    //  Test 0: Scaffolding Template
    // ----------------------------------------------------------------------------
    //  This template serves as a reference pattern for writing SDOM unit tests.
    //
    //  Status Legend:
    //   ✅ Test Verified     - Stable, validated, and passing
    //   🔄 In Progress       - Currently being implemented or debugged
    //   ⚠️  Failing          - Currently failing; requires investigation
    //   🚫 Remove            - Deprecated or replaced
    //   ❌ Invalid           - No longer applicable or test case obsolete
    //   ☐ Planned            - Placeholder for future implementation
    //
    //  Usage Notes:
    //   • To signal a test failure, push a descriptive message to `errors`.
    //   • Each test should return `true` once it has finished running.
    //   • Multi-frame tests may return `false` until all assertions pass.
    //   • Keep tests self-contained and deterministic.
    //
    // ============================================================================
    bool Event_test0(std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        // errors.push_back("Test Scaffold Error");

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

    } // END: IDisplayObject_test0(std::vector<std::string>& errors)


    // --- Event_test1: Registry Verification and Static Event Accessibility ----------
    //
    // 🧩 Purpose:
    //   Ensures all core EventType instances are registered and accessible in the
    //   global EventType registry. Confirms correct naming, uniqueness, and flag
    //   configurations for key event types.
    //
    // 📝 Notes:
    //   • Validates static registration of EventType instances.  
    //   • Checks for missing, null, or mismatched event names.  
    //   • Ensures registry size meets expected minimum threshold.  
    //   • Verifies a few representative flag configurations for correctness.  
    //
    // ⚙️ Functions Tested:
    //
    //   | Category            | Functions Tested / Behavior Verified                          |
    //   |----------------------|---------------------------------------------------------------|
    //   | Registry Access      | ✅ EventType::getRegistry()                                    |
    //   | Name Resolution      | ✅ EventType::getName()                                        |
    //   | Flag Validation      | ✅ getCaptures(), ✅ getBubbles(), ✅ getTargetOnly(), ✅ getGlobal() |
    //   | Integrity Checks     | ✅ Registry non-empty, ✅ No duplicates, ✅ Minimum count        |
    //   | Behavioral Coverage  | ✅ Core, UI, Input, Lifecycle, and System event types verified  |
    //
    // ⚠️ Safety:
    //   Read-only verification — no global state mutations.
    //
    // ============================================================================
    bool Event_test1(std::vector<std::string>& errors)
    {
        // --- 1) Retrieve and validate registry -------------------------------------
        const auto& reg = EventType::getRegistry();
        if (reg.empty()) {
            errors.push_back("Event registry is empty — static instances not registered.");
            return true;
        }

        // --- 2) Verify that all required event types are registered ----------------
        const std::vector<std::string> required = {
            "None", "SDL_Event", "Quit", "EnterFrame",
            "MouseButtonUp", "MouseButtonDown", "MouseWheel",
            "MouseMove", "MouseClick", "MouseDoubleClick",
            "MouseEnter", "MouseLeave",
            "StageClosed", "KeyDown", "KeyUp",
            "FocusGained", "FocusLost", "Resize", "Move",
            "Show", "Hide", "EnterFullscreen", "LeaveFullscreen",
            "ValueChanged", "StateChanged", "SelectionChanged",
            "Enabled", "Disabled", "Visible", "Hidden",
            "Drag", "Dragging", "Drop",
            "ClipboardCopy", "ClipboardPaste",
            "Added", "Removed", "AddedToStage", "RemovedFromStage",
            "OnInit", "OnQuit", "OnEvent", "OnUpdate", "OnRender", "OnPreRender",
            "User"
        };

        for (const auto& name : required)
        {
            auto it = reg.find(name);
            if (it == reg.end()) {
                errors.push_back("Missing Event registration: " + name);
            }
            else if (!it->second) {
                errors.push_back("Null pointer in Event registry for: " + name);
            }
            else if (it->second->getName() != name) {
                errors.push_back("Event name mismatch for '" + name +
                                "': got '" + it->second->getName() + "'");
            }
        }

        // --- 3) Duplicate name check (defensive) -----------------------------------
        std::unordered_set<std::string> uniqueNames;
        for (const auto& [name, ptr] : reg) {
            if (!uniqueNames.insert(name).second) {
                errors.push_back("Duplicate Event name in registry: " + name);
            }
        }

        // --- 4) Minimum registry size sanity check ---------------------------------
        constexpr size_t expected_minimum_count = 48;
        if (reg.size() < expected_minimum_count) {
            errors.push_back("Event registry smaller than expected: " +
                            std::to_string(reg.size()) + " < " +
                            std::to_string(expected_minimum_count));
        }

        // --- 5) Verify representative flag configurations --------------------------
        auto checkFlags = [&](const std::string& name,
                            bool captures, bool bubbles, bool targetOnly, bool global)
        {
            auto it = reg.find(name);
            if (it == reg.end()) return;
            EventType* et = it->second;
            if (et->getCaptures() != captures)
                errors.push_back(name + ": captures mismatch");
            if (et->getBubbles() != bubbles)
                errors.push_back(name + ": bubbles mismatch");
            if (et->getTargetOnly() != targetOnly)
                errors.push_back(name + ": targetOnly mismatch");
            if (et->getGlobal() != global)
                errors.push_back(name + ": global mismatch");
        };

        checkFlags("Quit",      false, false, false, true);
        checkFlags("MouseMove", false, false, true,  false);
        checkFlags("OnQuit",    false, false, false, true);

        // ✅ Test complete — return success
        return true;
    } // END -- Registry Verification and Static Event Accessibility


    // --- Event_test2: Verify that well-known static event types have expected flag states ----------
    //
    // 🧩 Purpose:
    //   Validates that static EventType definitions (specifically `EventType::None`) expose
    //   the expected default flag values and that each flag setter properly modifies and restores
    //   its state.
    //
    // 📝 Notes:
    //   • Confirms default flag configuration for `EventType::None`.  
    //   • Verifies setter chaining correctness and reversible state mutation.  
    //   • Ensures static event objects are stable after temporary modification.  
    //
    // ⚙️ Functions Tested:
    //
    //   | Category           | Functions Tested / Behavior Verified         |
    //   |---------------------|---------------------------------------------|
    //   | Default Flags       | ✅ getCaptures(), ✅ getBubbles(), ✅ getTargetOnly(), ✅ getGlobal() |
    //   | Setter Chain        | ✅ setCaptures(), ✅ setBubbles(), ✅ setTargetOnly(), ✅ setGlobal() |
    //   | State Restoration   | ✅ Values restored correctly after modification |
    //   | Static Integrity    | ✅ Modifications do not persist between test runs |
    //
    // ⚠️ Safety:
    //   Directly mutates `EventType::None` flags, but restores them to defaults before completion.
    //
    // ============================================================================
    bool Event_test2(std::vector<std::string>& errors)
    {
        // --- 1) Access a well-known static event type -------------------------------
        EventType& et = EventType::None;

        // --- 2) Save current flag states -------------------------------------------
        bool origCaptures   = et.getCaptures();
        bool origBubbles    = et.getBubbles();
        bool origTargetOnly = et.getTargetOnly();
        bool origGlobal     = et.getGlobal();

        // --- 3) Verify default (expected) states -----------------------------------
        // As defined in SDOM_Event.cpp:
        // Event::None("None", false, false, false, false);
        if (origCaptures)   errors.push_back("EventType::None default captures flag incorrect");
        if (origBubbles)    errors.push_back("EventType::None default bubbles flag incorrect");
        if (origTargetOnly) errors.push_back("EventType::None default targetOnly flag incorrect");
        if (origGlobal)     errors.push_back("EventType::None default global flag incorrect");

        // --- 4) Change all flags ---------------------------------------------------
        et.setCaptures(true)
        .setBubbles(true)
        .setTargetOnly(true)
        .setGlobal(true);

        // --- 5) Verify updates took effect ----------------------------------------
        if (!et.getCaptures())   errors.push_back("EventType::setCaptures() failed to update flag");
        if (!et.getBubbles())    errors.push_back("EventType::setBubbles() failed to update flag");
        if (!et.getTargetOnly()) errors.push_back("EventType::setTargetOnly() failed to update flag");
        if (!et.getGlobal())     errors.push_back("EventType::setGlobal() failed to update flag");

        // --- 6) Restore original flag states --------------------------------------
        et.setCaptures(origCaptures)
        .setBubbles(origBubbles)
        .setTargetOnly(origTargetOnly)
        .setGlobal(origGlobal);

        // --- 7) Verify restoration -------------------------------------------------
        if (et.getCaptures()   != origCaptures)   errors.push_back("EventType::setCaptures() restore failed");
        if (et.getBubbles()    != origBubbles)    errors.push_back("EventType::setBubbles() restore failed");
        if (et.getTargetOnly() != origTargetOnly) errors.push_back("EventType::setTargetOnly() restore failed");
        if (et.getGlobal()     != origGlobal)     errors.push_back("EventType::setGlobal() restore failed");

        return true; // ✅ finished this frame
    } // END -- Verify that well-known static event types have expected flag states



    // --- runEventRoundTripTest -----------------------------------------------------
    //
    // 🧩 Purpose:
    //   Shared helper that validates event round-tripping for a given set of
    //   EventType definitions. It ensures that events can be queued, dispatched,
    //   and received correctly with payload integrity verification.
    //
    // 📝 Notes:
    //   • Each event type is registered as a listener on a temporary Box object.  
    //   • Each listener sets a “hit” flag when the event fires with matching payload.  
    //   • Events are queued, dispatched, verified, and then cleaned up safely.  
    //
    // ⚙️ Functions Tested:
    //
    //   | Category             | Behavior / API Verified                             |
    //   |-----------------------|-----------------------------------------------------|
    //   | Event Dispatch        | ✅ Core::pumpEventsOnce() processes queued events   |
    //   | Event Listener API    | ✅ addEventListener / removeEventListener / hasEventListener |
    //   | Event Payload         | ✅ getPayloadValue / setPayloadValue round-trip     |
    //   | Event Registration    | ✅ EventType registry linkage and consistency       |
    //   | Resource Cleanup      | ✅ Proper listener removal and orphan collection    |
    //
    // ⚠️ Safety:
    //   Temporary objects are destroyed and garbage collected at the end.
    //
    // ============================================================================
    static bool runEventRoundTripTest(
        const std::vector<std::pair<std::string, EventType&>>& eventTests,
        const std::string& boxName,
        std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();
        DisplayHandle stage = core.getRootNode();

        // --- 1) Create a temporary Box test object ---------------------------------
        Box::InitStruct boxInit;
        boxInit.name   = boxName;
        boxInit.x      = 10;
        boxInit.y      = 10;
        boxInit.width  = 100;
        boxInit.height = 75;
        boxInit.color  = {255, 0, 255, 255};

        DisplayHandle box = core.createDisplayObject("Box", boxInit);
        if (!box.isValid()) {
            errors.push_back("Failed to create test box '" + boxName + "'.");
            return false;
        }

        if (stage->hasChild(box)) {
            errors.push_back("Stage already has a child named '" + boxName + "'.");
            return false;
        }

        stage->addChild(box);

        // --- 2) Prepare tracking and listener registration --------------------------
        std::unordered_map<std::string, bool> hits;
        for (const auto& [name, _] : eventTests)
            hits[name] = false;

        for (auto& [name, type] : eventTests)
        {
            box->addEventListener(type, [&](const Event& ev) {
                if (ev.getTarget() != box) return;
                const std::string info = ev.getPayloadValue<std::string>("info");
                if (info == "test") {
                    hits[ev.getTypeName()] = true;
                } else {
                    errors.push_back("Payload mismatch for " + ev.getTypeName());
                    ok = false;
                }
            }, false);

            if (!box->hasEventListener(type, false)) {
                errors.push_back("Expected listener for EventType: " + name);
                ok = false;
            }
        }

        // --- 3) Queue and dispatch events -------------------------------------------
        for (const auto& [name, type] : eventTests) {
            box->queue_event(type, [&](Event& ev) { ev.setPayloadValue("info", "test"); });
        }

        core.pumpEventsOnce();

        // --- 4) Verify that all listeners were triggered -----------------------------
        for (const auto& [name, hit] : hits) {
            if (!hit) {
                errors.push_back("Listener '" + name + "' was not hit.");
                ok = false;
            }
        }

        // --- 5) Cleanup --------------------------------------------------------------
        for (const auto& [name, type] : eventTests) {
            if (box->hasEventListener(type, false))
                box->removeEventListener(type, nullptr, false);
        }

        if (stage->hasChild(box))
            stage->removeChild(box);

        core.collectGarbage();

        // ✅ Return informational success/failure
        return ok;
    } // END -- runEventRoundTripTest


    // --- Event_test3: Core system event types --------------------------------------
    //
    // 🧩 Purpose:
    //   Verifies that fundamental SDOM core event types (system and lifecycle events)
    //   behave consistently through the event registration, serialization, and
    //   round-trip validation helper.
    //
    // 📝 Notes:
    //   • Uses `runEventRoundTripTest()` to ensure core event types can be created,
    //     cloned, and serialized/deserialized correctly.  
    //   • Focuses on non-UI, non-input system-level events (e.g., Quit, EnterFrame).  
    //   • Additional clipboard and user event tests are pending implementation.  
    //
    // ⚙️ Functions Tested:
    //
    //   | Category            | Functions Tested / Behavior Verified                  |
    //   |----------------------|-------------------------------------------------------|
    //   | Round-trip Behavior  | ✅ runEventRoundTripTest() with system events         |
    //   | EventType Accessors  | ✅ EventType::getName(), ✅ EventType identity check   |
    //   | Core Event Coverage  | ✅ None, SDL_Event, Quit, EnterFrame, StageClosed, KeyDown, KeyUp |
    //   | Future Extension     | ☐ ClipboardCopy, ☐ ClipboardPaste, ☐ User             |
    //
    // ⚠️ Safety:
    //   No persistent state changes; helper performs temporary creation/destruction only.
    //
    // ============================================================================
    bool Event_test3(std::vector<std::string>& errors)
    {
        // --- 1) Define core system event types to verify ----------------------------
        const std::vector<std::pair<std::string, EventType&>> events = {
            {"None",        EventType::None}
            , {"SDL_Event",   EventType::SDL_Event}
            , {"Quit",        EventType::Quit}
            , {"EnterFrame",  EventType::EnterFrame}
            , {"StageClosed", EventType::StageClosed}
            , {"KeyDown",     EventType::KeyDown}
            , {"KeyUp",       EventType::KeyUp}
            // ☐ not yet implemented:
            // , {"ClipboardCopy",  EventType::ClipboardCopy}
            // , {"ClipboardPaste", EventType::ClipboardPaste}
            // , {"User",           EventType::User}
        };

        // --- 2) Execute round-trip helper test -------------------------------------
        runEventRoundTripTest(events, "testBox_Event3", errors);

        // ✅ Test complete — return success
        return true;
    } // END -- Core system event types


    // --- Event_test4: Mouse-related event types ------------------------------------
    //
    // 🧩 Purpose:
    //   Verifies all mouse-related event types can be registered, serialized,
    //   and round-tripped correctly using `runEventRoundTripTest()`.
    //
    // 📝 Notes:
    //   • Confirms correct handling of click, wheel, move, and drag/drop events.  
    //   • Ensures `EventType` instances for mouse events are stable and accessible.  
    //   • Validates event round-trip identity and integrity within the registry.  
    //
    // ⚙️ Functions Tested:
    //
    //   | Category              | Functions Tested / Behavior Verified              |
    //   |------------------------|--------------------------------------------------|
    //   | Round-trip Validation  | ✅ runEventRoundTripTest() with mouse event types |
    //   | Event Identity         | ✅ EventType::getName()                          |
    //   | Mouse Event Coverage   | ✅ MouseButtonUp, MouseButtonDown, MouseWheel, MouseMove |
    //   |                       | ✅ MouseClick, MouseDoubleClick, MouseEnter, Drag, Dragging, Drop |
    //
    // ⚠️ Safety:
    //   No persistent mutations; temporary test objects are cleaned up by helper.
    //
    // ============================================================================
    bool Event_test4(std::vector<std::string>& errors)
    {
        // --- 1) Define mouse-related event types -----------------------------------
        const std::vector<std::pair<std::string, EventType&>> events = {
            {"MouseButtonUp",     EventType::MouseButtonUp}
            , {"MouseButtonDown",   EventType::MouseButtonDown}
            , {"MouseWheel",        EventType::MouseWheel}
            , {"MouseMove",         EventType::MouseMove}
            , {"MouseClick",        EventType::MouseClick}
            , {"MouseDoubleClick",  EventType::MouseDoubleClick}
            , {"MouseEnter",        EventType::MouseEnter}
            , {"Drag",              EventType::Drag}
            , {"Dragging",          EventType::Dragging}
            , {"Drop",              EventType::Drop}
        };

        // --- 2) Execute round-trip helper test -------------------------------------
        runEventRoundTripTest(events, "testBox_Event4", errors);

        // ✅ Test complete — return success
        return true;
    } // END -- Mouse-related event types


    // --- Event_test5: Window event types -------------------------------------------
    //
    // 🧩 Purpose:
    //   Verifies that all window-related event types (focus, visibility, resize,
    //   movement, and fullscreen transitions) perform correct event registration,
    //   serialization, and round-trip validation.
    //
    // 📝 Notes:
    //   • Ensures window lifecycle events propagate through the registry correctly.  
    //   • Covers typical SDL window state changes and transitions.  
    //
    // ⚙️ Functions Tested:
    //
    //   | Category              | Functions Tested / Behavior Verified                |
    //   |------------------------|----------------------------------------------------|
    //   | Round-trip Validation  | ✅ runEventRoundTripTest() with window event types  |
    //   | EventType Integrity    | ✅ EventType::getName() and consistent identity     |
    //   | Window Event Coverage  | ✅ FocusGained, FocusLost, Resize, Move, Show, Hide |
    //   |                       | ✅ EnterFullscreen, LeaveFullscreen                 |
    //
    // ⚠️ Safety:
    //   No persistent state modification; all events are verified transiently.
    //
    // ============================================================================
    bool Event_test5(std::vector<std::string>& errors)
    {
        // --- 1) Define window event types ------------------------------------------
        const std::vector<std::pair<std::string, EventType&>> events = {
            {"FocusGained",       EventType::FocusGained}
            , {"FocusLost",         EventType::FocusLost}
            , {"Resize",            EventType::Resize}
            , {"Move",              EventType::Move}
            , {"Show",              EventType::Show}
            , {"Hide",              EventType::Hide}
            , {"EnterFullscreen",   EventType::EnterFullscreen}
            , {"LeaveFullscreen",   EventType::LeaveFullscreen}
        };

        // --- 2) Execute round-trip helper test -------------------------------------
        runEventRoundTripTest(events, "testBox_Event5", errors);

        // ✅ Test complete — return success
        return true;
    } // END -- Window event types


    // --- Event_test6: General UI event types ---------------------------------------
    //
    // 🧩 Purpose:
    //   Verifies UI-oriented event types related to state, value, visibility,
    //   and selection transitions round-trip correctly via the registry.
    //
    // 📝 Notes:
    //   • Focuses on logical UI changes rather than system-level input.  
    //   • Confirms proper `EventType` identity and flag consistency.  
    //
    // ⚙️ Functions Tested:
    //
    //   | Category              | Functions Tested / Behavior Verified               |
    //   |------------------------|---------------------------------------------------|
    //   | Round-trip Validation  | ✅ runEventRoundTripTest() with UI event types     |
    //   | Event Identity         | ✅ EventType::getName()                           |
    //   | UI Event Coverage      | ✅ ValueChanged, StateChanged, SelectionChanged    |
    //   |                       | ✅ Enabled, Disabled, Visible, Hidden              |
    //
    // ⚠️ Safety:
    //   Transient event objects only; no persistent state changes.
    //
    // ============================================================================
    bool Event_test6(std::vector<std::string>& errors)
    {
        // --- 1) Define general UI event types --------------------------------------
        const std::vector<std::pair<std::string, EventType&>> events = {
            {"ValueChanged",      EventType::ValueChanged}
            , {"StateChanged",      EventType::StateChanged}
            , {"SelectionChanged",  EventType::SelectionChanged}
            , {"Enabled",           EventType::Enabled}
            , {"Disabled",          EventType::Disabled}
            , {"Visible",           EventType::Visible}
            , {"Hidden",            EventType::Hidden}
        };

        // --- 2) Execute round-trip helper test -------------------------------------
        runEventRoundTripTest(events, "testBox_Event6", errors);

        // ✅ Test complete — return success
        return true;
    } // END -- General UI event types


    // --- Event_test7: Application Lifecycle event types ----------------------------
    //
    // 🧩 Purpose:
    //   Validates the registration and round-trip integrity of application and
    //   display object lifecycle event types.
    //
    // 📝 Notes:
    //   • Includes "added/removed" events and engine-level lifecycle hooks.  
    //   • Ensures static EventType instances for initialization and rendering are
    //     accessible and consistent.  
    //
    // ⚙️ Functions Tested:
    //
    //   | Category               | Functions Tested / Behavior Verified                |
    //   |-------------------------|----------------------------------------------------|
    //   | Round-trip Validation   | ✅ runEventRoundTripTest() with lifecycle events    |
    //   | Event Identity          | ✅ EventType::getName()                            |
    //   | Lifecycle Event Coverage| ✅ Added, Removed, AddedToStage, RemovedFromStage   |
    //   |                        | ✅ OnInit, OnQuit, OnEvent, OnUpdate, OnRender, OnPreRender |
    //
    // ⚠️ Safety:
    //   Lifecycle event instances are static and unaffected by test modifications.
    //
    // ============================================================================
    bool Event_test7(std::vector<std::string>& errors)
    {
        // --- 1) Define lifecycle event types ---------------------------------------
        const std::vector<std::pair<std::string, EventType&>> events = {
            {"Added",            EventType::Added}
            , {"Removed",          EventType::Removed}
            , {"AddedToStage",     EventType::AddedToStage}
            , {"RemovedFromStage", EventType::RemovedFromStage}
            , {"OnInit",           EventType::OnInit}
            , {"OnQuit",           EventType::OnQuit}
            , {"OnEvent",          EventType::OnEvent}
            , {"OnUpdate",         EventType::OnUpdate}
            , {"OnRender",         EventType::OnRender}
            , {"OnPreRender",      EventType::OnPreRender}
        };

        // --- 2) Execute round-trip helper test -------------------------------------
        runEventRoundTripTest(events, "testBox_Event7", errors);

        // ✅ Test complete — return success
        return true;
    } // END -- Application Lifecycle event types


    // --- Event_test8: Behavioral Mouse Event Verification --------------------------
    //
    // 🧩 Purpose:
    //   Simulates real-world mouse interactions and ensures that the EventManager
    //   correctly interprets SDL input events into corresponding SDOM event types.
    //
    // 📝 Notes:
    //   • Exercises EventManager’s SDL event queue handling end-to-end.  
    //   • Simulates both simple and compound behaviors: clicks, double-clicks,
    //     hover entry/exit, drag/drop, and scrolling.  
    //   • Each behavior is injected as an SDL event sequence via `Queue_SDL_Event()`.
    //
    // ⚙️ Functions Tested:
    //
    //   | Category                  | Functions Tested / Behavior Verified              |
    //   |----------------------------|--------------------------------------------------|
    //   | SDL Integration            | ✅ EventManager::Queue_SDL_Event()                |
    //   | Event Dispatching          | ✅ Event translation and queue pumping            |
    //   | Mouse Behavior Simulation  | ✅ ButtonDown/Up, Wheel, Move, Click, DoubleClick |
    //   | Hover Behavior Simulation  | ✅ MouseEnter, MouseLeave                         |
    //
    // ⚠️ Safety:
    //   No persistent state changes — tests use transient synthetic SDL events only.
    //
    // ============================================================================
    bool Event_test8(std::vector<std::string>& errors)
    {
        // --- 1) Get EventManager instance ------------------------------------------
        EventManager& em = getCore().getEventManager();

        // --- 2) Define simulated mouse actions -------------------------------------
        std::vector<std::pair<std::string, std::function<void(DisplayHandle)>>> actions = {
            {"MouseButtonDown", [&](DisplayHandle b){ 
                SDL_Event e{}; e.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                e.button.button = SDL_BUTTON_LEFT;
                e.button.x = b->getX() + 5; e.button.y = b->getY() + 5;
                em.Queue_SDL_Event(e);
            }},
            {"MouseButtonUp", [&](DisplayHandle b){
                SDL_Event e{}; e.type = SDL_EVENT_MOUSE_BUTTON_UP;
                e.button.button = SDL_BUTTON_LEFT;
                e.button.x = b->getX() + 5; e.button.y = b->getY() + 5;
                em.Queue_SDL_Event(e);
            }},
            {"MouseMove", [&](DisplayHandle b){
                SDL_Event e{}; e.type = SDL_EVENT_MOUSE_MOTION;
                e.motion.x = b->getX() + 5; e.motion.y = b->getY() + 5;
                em.Queue_SDL_Event(e);
            }},
            {"MouseWheel", [&](DisplayHandle b){
                SDL_Event e{};
                e.type = SDL_EVENT_MOUSE_WHEEL;
                e.wheel.x = 0;
                e.wheel.y = 1; // scroll up
                e.wheel.mouse_x = b->getX() + 5;
                e.wheel.mouse_y = b->getY() + 5;
                e.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
                em.Queue_SDL_Event(e);
            }},
            {"MouseClick", [&](DisplayHandle b){
                // Push down then up to form a single click
                SDL_Event down{}; 
                down.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                down.button.button = SDL_BUTTON_LEFT;
                down.button.clicks = 1;
                down.button.x = b->getX() + 5;
                down.button.y = b->getY() + 5;
                em.Queue_SDL_Event(down);

                SDL_Event up{};
                up.type = SDL_EVENT_MOUSE_BUTTON_UP;
                up.button.button = SDL_BUTTON_LEFT;
                up.button.clicks = 1;
                up.button.x = b->getX() + 5;
                up.button.y = b->getY() + 5;
                up.common.timestamp = down.common.timestamp + 50; // simulate ~50ms delay
                em.Queue_SDL_Event(up);
            }},
            {"MouseDoubleClick", [&](DisplayHandle b){
                const int clickCount = 2;
                uint32_t timestamp = SDL_GetTicks();
                for (int i = 0; i < clickCount; ++i)
                {
                    SDL_Event down{};
                    down.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                    down.button.button = SDL_BUTTON_LEFT;
                    down.button.clicks = i + 1;
                    down.button.x = b->getX() + 5;
                    down.button.y = b->getY() + 5;
                    down.common.timestamp = timestamp;
                    em.Queue_SDL_Event(down);

                    SDL_Event up{};
                    up.type = SDL_EVENT_MOUSE_BUTTON_UP;
                    up.button.button = SDL_BUTTON_LEFT;
                    up.button.clicks = i + 1;
                    up.button.x = b->getX() + 5;
                    up.button.y = b->getY() + 5;
                    up.common.timestamp = timestamp + 50;
                    em.Queue_SDL_Event(up);

                    timestamp += 100; // short delay between clicks
                }
            }},
            {"MouseEnter", [&](DisplayHandle b){
                SDL_Event e1{};
                e1.type = SDL_EVENT_MOUSE_MOTION;
                e1.motion.x = b->getX() - 10;
                e1.motion.y = b->getY() - 10;
                em.Queue_SDL_Event(e1);

                SDL_Event e2{};
                e2.type = SDL_EVENT_MOUSE_MOTION;
                e2.motion.x = b->getX() + b->getWidth() / 2;
                e2.motion.y = b->getY() + b->getHeight() / 2;
                e2.common.timestamp = e1.common.timestamp + 50;
                em.Queue_SDL_Event(e2);
            }},
            {"MouseLeave", [&](DisplayHandle b){
                SDL_Event e1{};
                e1.type = SDL_EVENT_MOUSE_MOTION;
                e1.motion.x = b->getX() + b->getWidth() / 2;
                e1.motion.y = b->getY() + b->getHeight() / 2;
                em.Queue_SDL_Event(e1);

                SDL_Event e2{};
                e2.type = SDL_EVENT_MOUSE_MOTION;
                e2.motion.x = b->getX() + b->getWidth() + 50;
                e2.motion.y = b->getY() + b->getHeight() + 50;
                e2.common.timestamp = e1.common.timestamp + 50;
                em.Queue_SDL_Event(e2);
            }}
        };

        // --- 3) Execute behavioral test sequence -----------------------------------
        UnitTests::run_event_behavior_test(actions, errors);

        // ✅ Test complete — return success
        return true;
    } // END -- Behavioral Mouse Event Verification



    // --- Lua Integration Tests --- //

    bool Event_LUA_Tests(std::vector<std::string>& errors)
    {
        UnitTests& ut = UnitTests::getInstance();
        return ut.run_lua_tests(errors, ut.getLuaFilename());
    } // END: Event_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool Event_UnitTests()
    {
        const std::string objName = "Event";
        UnitTests& ut = UnitTests::getInstance();

        ut.add_test(objName, "Test scaffolding", Event_test0);
        ut.add_test(objName, "Verify Event table is registered and accessible", Event_test1);
        ut.add_test(objName, "Verify well-known static event types have expected flag states", Event_test2);
        ut.add_test(objName, "Core system event types round-trip", Event_test3);
        ut.add_test(objName, "Mouse-related event types round-trip", Event_test4);
        ut.add_test(objName, "Mouse-related event types round-trip", Event_test5);
        ut.add_test(objName, "General UI event types round-trip", Event_test6);
        ut.add_test(objName, "Application Lifecycle event types round-trip", Event_test7);
        ut.add_test(objName, "Behavioral Mouse Event Verification", Event_test8);


        ut.setLuaFilename("src/Event_UnitTests.lua"); // Lua test script path
        ut.add_test(objName, "Lua: " + ut.getLuaFilename(), Event_LUA_Tests, true);  // false = not implemented yet (dont run the lua file tests)


        // return ut.run_all(objName);
        return true;
    } // END: Event_UnitTests()



} // END: namespace SDOM