// Event_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_EventManager.hpp>   
#include <SDOM/SDOM_Stage.hpp>

#include "Box.hpp"

namespace SDOM
{
    // --- Individual Event Unit Tests --- //

    // Test 0: Test scaffolding
    bool Event_test0(std::vector<std::string>& errors)   
    {
        // ✅ Test Verified
        // 🔄 In Progress
        // ⚠️ Failing     
        // 🚫 Remove
        // ❌ Invalid
        // ☐ Planned./prog

        bool ok = true;

        // To send an error message to the test harness, use the following:
        // errors.push_error("Description of the error.");
        // ok = false;

        return ok;
    } // Event_test0(std::vector<std::string>& errors)   



    // --- Lua Integration Tests --- //

    // ✅ Test 1: Events are registered and accessible
    bool Event_test1(std::vector<std::string>& errors)
    {
        bool ok = true;

        // Get the registry
        const auto& reg = EventType::getRegistry();

        // 1. Sanity check: registry should not be empty
        if (reg.empty()) {
            errors.push_back("Event registry is empty — static instances not registered.");
            ok = false;
            return ok; // no point continuing
        }

        // 2. Verify that critical event types are registered
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
                ok = false;
            } else if (it->second == nullptr) {
                errors.push_back("Null pointer in Event registry for: " + name);
                ok = false;
            } else if (it->second->getName() != name) {
                errors.push_back("Event name mismatch for '" + name +
                                "': got '" + it->second->getName() + "'");
                ok = false;
            }
        }

        // 3. Check for duplicates (very unlikely, but defensive)
        std::unordered_set<std::string> uniqueNames;
        for (const auto& [name, ptr] : reg) {
            if (!uniqueNames.insert(name).second) {
                errors.push_back("Duplicate Event name in registry: " + name);
                ok = false;
            }
        }

        // 4. Optional sanity count check (not a failure if larger — custom events allowed)
        constexpr size_t expected_minimum_count = 48;
        if (reg.size() < expected_minimum_count) {
            errors.push_back("Event registry smaller than expected: " +
                            std::to_string(reg.size()) + " < " +
                            std::to_string(expected_minimum_count));
            ok = false;
        }

        // 5. Quick spot check: verify a few flags
        auto checkFlags = [&](const std::string& name, bool captures, bool bubbles, bool targetOnly, bool global) {
            auto it = reg.find(name);
            if (it == reg.end()) return;
            EventType* et = it->second;
            if (et->getCaptures() != captures)
                { errors.push_back(name + ": captures mismatch"); ok = false; }
            if (et->getBubbles() != bubbles)
                { errors.push_back(name + ": bubbles mismatch"); ok = false; }
            if (et->getTargetOnly() != targetOnly)
                { errors.push_back(name + ": targetOnly mismatch"); ok = false; }
            if (et->getGlobal() != global)
                { errors.push_back(name + ": global mismatch"); ok = false; }
        };

        checkFlags("Quit", false, false, false, true);
        checkFlags("MouseMove", false, false, true, false);
        checkFlags("OnQuit", false, false, false, true);

        return ok;
    }

    // ✅ Test 2: Verify that well-known static event types have expected flag states
    bool Event_test2(std::vector<std::string>& errors)
    {
        bool ok = true;

        // Use a well-known static event type
        EventType& et = EventType::None;

        // 1. Save the current flag states
        bool origCaptures   = et.getCaptures();
        bool origBubbles    = et.getBubbles();
        bool origTargetOnly = et.getTargetOnly();
        bool origGlobal     = et.getGlobal();

        // 2. Verify default (expected) flag states for None
        // According to SDOM_Event.cpp:
        // Event::None("None", false, false, false, false);
        if (origCaptures != false) { errors.push_back("EventType::None default captures flag incorrect"); ok = false; }
        if (origBubbles  != false) { errors.push_back("EventType::None default bubbles flag incorrect"); ok = false; }
        if (origTargetOnly != false) { errors.push_back("EventType::None default targetOnly flag incorrect"); ok = false; }
        if (origGlobal != false) { errors.push_back("EventType::None default global flag incorrect"); ok = false; }

        // 3. Change the flag states
        et.setCaptures(true)
        .setBubbles(true)
        .setTargetOnly(true)
        .setGlobal(true);


        // 4. Verify they have changed
        if (et.getCaptures() != true) { errors.push_back("EventType::setCaptures() failed to update flag"); ok = false; }
        if (et.getBubbles() != true) { errors.push_back("EventType::setBubbles() failed to update flag"); ok = false; }
        if (et.getTargetOnly() != true) { errors.push_back("EventType::setTargetOnly() failed to update flag"); ok = false; }
        if (et.getGlobal() != true) { errors.push_back("EventType::setGlobal() failed to update flag"); ok = false; }

        // 5. Restore original flag states
        et.setCaptures(origCaptures)
        .setBubbles(origBubbles)
        .setTargetOnly(origTargetOnly)
        .setGlobal(origGlobal);

        // 6. Verify they are back to defaults
        if (et.getCaptures() != origCaptures) { errors.push_back("EventType::setCaptures() restore failed"); ok = false; }
        if (et.getBubbles() != origBubbles) { errors.push_back("EventType::setBubbles() restore failed"); ok = false; }
        if (et.getTargetOnly() != origTargetOnly) { errors.push_back("EventType::setTargetOnly() restore failed"); ok = false; }
        if (et.getGlobal() != origGlobal) { errors.push_back("EventType::setGlobal() restore failed"); ok = false; }

        return ok;
    } // END: Event_test2()


    // ----------------------------------------------------
    // Shared helper for EventType round-trip verification
    // ----------------------------------------------------
    static bool runEventRoundTripTest(
        const std::vector<std::pair<std::string, EventType&>>& eventTests,
        const std::string& boxName,
        std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();
        DisplayHandle stage = core.getRootNode();

        // Create the Box object
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

        // Track which listeners fired
        std::unordered_map<std::string, bool> hits;
        for (const auto& [name, _] : eventTests)
            hits[name] = false;

        // Attach listeners
        for (auto& [name, type] : eventTests) {
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

        // Queue events
        for (const auto& [name, type] : eventTests) {
            box->queue_event(type, [&](Event& ev) { ev.setPayloadValue("info", "test"); });
        }

        // Dispatch events
        core.pumpEventsOnce();

        // Verify listener hits
        for (const auto& [name, hit] : hits) {
            if (!hit) {
                errors.push_back("Listener '" + name + "' was not hit.");
                ok = false;
            }
        }

        // Cleanup
        for (const auto& [name, type] : eventTests) {
            if (box->hasEventListener(type, false))
                box->removeEventListener(type, nullptr, false);
        }

        if (stage->hasChild(box))
            stage->removeChild(box);

        core.collectGarbage();

        return ok;
    }


    // Test 3: Core system event types
    bool Event_test3(std::vector<std::string>& errors)
    {
        const std::vector<std::pair<std::string, EventType&>> events = {
              {"None",              EventType::None}
            , {"SDL_Event",         EventType::SDL_Event}
            , {"Quit",              EventType::Quit}
            , {"EnterFrame",        EventType::EnterFrame}
            , {"StageClosed",       EventType::StageClosed}
            , {"KeyDown",           EventType::KeyDown}
            , {"KeyUp",             EventType::KeyUp}
            // // not yet implemented
            // , {"ClipboardCopy",     EventType::ClipboardCopy}
            // , {"ClipboardPaste",    EventType::ClipboardPaste}
            // , {"User",              EventType::User}    
        };
        return runEventRoundTripTest(events, "testBox_Event3", errors);
    } // END: Event_test3()


    // Test 4: Mouse-related event types
    bool Event_test4(std::vector<std::string>& errors)
    {
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
        return runEventRoundTripTest(events, "testBox_Event4", errors);
    } // END: Event_test4()


    // Test 5: Window event types
    bool Event_test5(std::vector<std::string>& errors)
    {
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
        return runEventRoundTripTest(events, "testBox_Event4", errors);
    } // END: Event_test5()


    // Test 6: General UI event types
    bool Event_test6(std::vector<std::string>& errors)
    {
        const std::vector<std::pair<std::string, EventType&>> events = {
              {"ValueChanged",      EventType::ValueChanged}
            , {"StateChanged",      EventType::StateChanged}
            , {"SelectionChanged",  EventType::SelectionChanged}
            , {"Enabled",           EventType::Enabled}
            , {"Disabled",          EventType::Disabled}
            , {"Visible",           EventType::Visible}
            , {"Hidden",            EventType::Hidden}
        };
        return runEventRoundTripTest(events, "testBox_Event6", errors);
    } // END: Event_test6()


    // Test 7: Application Lifecycle event types
    bool Event_test7(std::vector<std::string>& errors)
    {
        const std::vector<std::pair<std::string, EventType&>> events = {
              {"Added",                  EventType::Added}
            , {"Removed",                EventType::Removed}
            , {"AddedToStage",           EventType::AddedToStage}
            , {"RemovedFromStage",       EventType::RemovedFromStage}
            , {"OnInit",                 EventType::OnInit}
            , {"OnQuit",                 EventType::OnQuit}
            , {"OnEvent",                EventType::OnEvent}
            , {"OnUpdate",               EventType::OnUpdate}
            , {"OnRender",               EventType::OnRender}
            , {"OnPreRender",            EventType::OnPreRender}
        };
        return runEventRoundTripTest(events, "testBox_Event7", errors);
    } // END: Event_test7()

    // ----------------------------------------------
    // Shared helper for Event behavior verification
    // ----------------------------------------------
    static bool runEventBehaviorTest( const std::vector<std::pair<std::string, std::function<void(DisplayHandle)>>> &actions, 
                                        std::vector<std::string> &errors)
    {
        bool ok = true;
        Core& core = getCore();
        DisplayHandle stage = core.getRootNode();

        // Create test box
        Box::InitStruct init;
        init.name = "behaviorTestBox";
        init.x = 10; init.y = 10; init.width = 50; init.height = 50;
        init.color = {255, 0, 0, 255};
        DisplayHandle box = core.createDisplayObject("Box", init);
        stage->addChild(box);

        std::unordered_map<std::string, bool> hits;
        for (const auto& [name, _] : actions) hits[name] = false;

        for (auto& [name, _] : actions) 
        {
            EventType* et = nullptr;
            const auto& reg = EventType::getRegistry();
            auto it = reg.find(name);
            if (it != reg.end()) et = it->second;
            if (!et) { errors.push_back("Unknown EventType: " + name); ok = false; continue; }

            box->addEventListener(*et, [&](const Event& ev) {
                hits[ev.getTypeName()] = true;
            });
        }

        for (auto& [_, action] : actions)
            action(box);

        core.pumpEventsOnce();

        for (const auto& [name, hit] : hits)
            if (!hit) { errors.push_back("Behavior event '" + name + "' did not fire."); ok = false; }

        stage->removeChild(box);
        core.collectGarbage();
        return ok;
    }


    // Test 8: Behavioral Mouse Event Verification
    bool Event_test8(std::vector<std::string>& errors)
    {
        EventManager& em = getCore().getEventManager();

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
                // SDL_PushEvent(&e);
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
                // First push down
                SDL_Event down{}; 
                down.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                down.button.button = SDL_BUTTON_LEFT;
                down.button.clicks = 1;
                down.button.x = b->getX() + 5;
                down.button.y = b->getY() + 5;
                em.Queue_SDL_Event(down);

                // Push the up event with a simulated later timestamp
                SDL_Event up{};
                up.type = SDL_EVENT_MOUSE_BUTTON_UP;
                up.button.button = SDL_BUTTON_LEFT;
                up.button.clicks = 1;
                up.button.x = b->getX() + 5;
                up.button.y = b->getY() + 5;
                up.common.timestamp = down.common.timestamp + 50; // simulate ~50ms later
                em.Queue_SDL_Event(up);
            }},
            {"MouseDoubleClick", [&](DisplayHandle b){
                const int clickCount = 2;        // double click = 2
                uint32_t timestamp = SDL_GetTicks();  // start time base

                for (int i = 0; i < clickCount; ++i)
                {
                    // --- Mouse down ---
                    SDL_Event down{};
                    down.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                    down.button.button = SDL_BUTTON_LEFT;
                    down.button.clicks = i + 1;  // increment click count
                    down.button.x = b->getX() + 5;
                    down.button.y = b->getY() + 5;
                    down.common.timestamp = timestamp;
                    em.Queue_SDL_Event(down);

                    // --- Mouse up ---
                    SDL_Event up{};
                    up.type = SDL_EVENT_MOUSE_BUTTON_UP;
                    up.button.button = SDL_BUTTON_LEFT;
                    up.button.clicks = i + 1;
                    up.button.x = b->getX() + 5;
                    up.button.y = b->getY() + 5;
                    up.common.timestamp = timestamp + 50;  // short delay after press
                    em.Queue_SDL_Event(up);

                    // simulate short delay between clicks (e.g., <200ms)
                    timestamp += 100;
                }
            }},
            {"MouseEnter", [&](DisplayHandle b){
                // Move mouse just *outside* first, to ensure it’s not “inside” yet
                SDL_Event e1{};
                e1.type = SDL_EVENT_MOUSE_MOTION;
                e1.motion.x = b->getX() - 10;
                e1.motion.y = b->getY() - 10;
                em.Queue_SDL_Event(e1);

                // Then move inside the box’s bounds to trigger MouseEnter
                SDL_Event e2{};
                e2.type = SDL_EVENT_MOUSE_MOTION;
                e2.motion.x = b->getX() + b->getWidth() / 2;
                e2.motion.y = b->getY() + b->getHeight() / 2;
                e2.common.timestamp = e1.common.timestamp + 50;
                em.Queue_SDL_Event(e2);
            }},
            {"MouseLeave", [&](DisplayHandle b){
                // Start inside the box to ensure we’re “hovered”
                SDL_Event e1{};
                e1.type = SDL_EVENT_MOUSE_MOTION;
                e1.motion.x = b->getX() + b->getWidth() / 2;
                e1.motion.y = b->getY() + b->getHeight() / 2;
                em.Queue_SDL_Event(e1);

                // Then move far outside to simulate leaving
                SDL_Event e2{};
                e2.type = SDL_EVENT_MOUSE_MOTION;
                e2.motion.x = b->getX() + b->getWidth() + 50;
                e2.motion.y = b->getY() + b->getHeight() + 50;
                e2.common.timestamp = e1.common.timestamp + 50;
                em.Queue_SDL_Event(e2);
            }}
        };
        return runEventBehaviorTest(actions, errors);
    } // END: Event_test8()

    // Test 9: Keyboard Events
    bool Event_test9(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        DisplayHandle stage = core.getRootNode();
        if (!stage.isValid()) return false;
        EventManager& em = getCore().getEventManager();
        std::vector<std::pair<std::string, std::function<void(DisplayHandle)>>> actions = {
            {"KeyDown", [&](DisplayHandle b){
                SDL_Event e{};
                e.type = SDL_EVENT_KEY_DOWN;
                e.key.windowID = SDL_GetWindowID(core.getWindow());
                e.key.timestamp = SDL_GetTicks();
                e.key.repeat = 0;
                e.key.mod = 0;
                e.key.key = SDLK_A; // We are using SDL3
                em.Queue_SDL_Event(e);
            }},
            {"KeyUp", [&](DisplayHandle b){
                SDL_Event e{};
                e.type = SDL_EVENT_KEY_UP;
                e.key.windowID = SDL_GetWindowID(core.getWindow());
                e.key.timestamp = SDL_GetTicks();
                e.key.repeat = 0;
                e.key.mod = 0;
                e.key.key = SDLK_A;
                em.Queue_SDL_Event(e);
            }}
            // // EditBox: Not Yet Implemented
            // ,{"TextInput", [&](DisplayHandle b){
            //     SDL_Event e{};
            //     e.type = SDL_EVENT_TEXT_INPUT;
            //     e.text.windowID = SDL_GetWindowID(core.getWindow());
            //     e.text.timestamp = SDL_GetTicks();
            //     e.text.text = "a";
            //     em.Queue_SDL_Event(e);
            // }}
        };
        return runEventBehaviorTest(actions, errors);
    } // END: Event_test9()



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
        // ut.clear_tests();

        ut.add_test(objName, "Test scaffolding", Event_test0);
        ut.add_test(objName, "Verify Event table is registered and accessible", Event_test1);
        ut.add_test(objName, "Verify well-known static event types have expected flag states", Event_test2);
        ut.add_test(objName, "Core system event types round-trip", Event_test3);
        ut.add_test(objName, "Mouse-related event types round-trip", Event_test4);
        ut.add_test(objName, "Mouse-related event types round-trip", Event_test5);
        ut.add_test(objName, "General UI event types round-trip", Event_test6);
        ut.add_test(objName, "Application Lifecycle event types round-trip", Event_test7);
        ut.add_test(objName, "Behavioral Mouse Event Verification", Event_test8);
        ut.add_test(objName, "Keyboard Event Verification", Event_test9);

        ut.setLuaFilename("src/Event_UnitTests.lua"); // Lua test script path
        ut.add_test(objName, "Lua: " + ut.getLuaFilename(), Event_LUA_Tests, false);  // false = not implemented yet (dont run the lua file tests)


        // return ut.run_all(objName);
        return true;
    } // END: Event_UnitTests()



} // END: namespace SDOM