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


    // 🔄 Test 3: Verify Predefined EventTypes round-trip properly
    bool Event_test3(std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();
        DisplayHandle stage = core.getRootNode();

        // Create a Box object
        Box::InitStruct boxInit;
        boxInit.name = "testBox";
        boxInit.x = 10;
        boxInit.y = 10;
        boxInit.width = 100;
        boxInit.height = 75;
        boxInit.color = {255, 0, 255, 255};
        DisplayHandle box = core.createDisplayObject("Box", boxInit);
        if (!box.isValid()) {
            errors.push_back("Failed to create 'testBox' object.");
            return false;
        }
        if (stage->hasChild(box)) {
            errors.push_back("Stage already has a child named 'testBox'.");
            return false;
        }
        stage->addChild(box);  // add to the stage so it can receive events


        // Tracks which listeners were hit
        std::unordered_map<std::string, bool> hits = {
            {"None", false}, {"SDL_Event", false}, {"Quit", false}, {"EnterFrame", false}
        };       

        // Lambda helper to attach
        auto attachListener = [&](EventType& et) 
        {            
            box->addEventListener(et, [&](const Event& ev) 
            {
                if (ev.getTarget() != box) return;                
                if (ev.getPayloadValue<std::string>("info") == "test") {
                    hits[ev.getTypeName()] = true;
                } else {
                    errors.push_back("payload info mismatch.");
                    ok = false;
                }
            }, false); // false = don't capture
        };
        // Attach event listeners
        attachListener(EventType::None);
        if (!box->hasEventListener(EventType::None, false)) { errors.push_back("Expected None listener to be registered."); ok = false; }
        attachListener(EventType::SDL_Event);
        if (!box->hasEventListener(EventType::SDL_Event, false)) { errors.push_back("Expected SDL_Event listener to be registered."); ok = false; }
        attachListener(EventType::Quit);
        if (!box->hasEventListener(EventType::Quit, false)) { errors.push_back("Expected Quit listener to be registered."); ok = false; }
        attachListener(EventType::EnterFrame);        
        if (!box->hasEventListener(EventType::EnterFrame, false)) { errors.push_back("Expected EnterFrame listener to be registered."); ok = false; }

        // one of each
        box->queue_event(EventType::None,       [&](Event& ev) { ev.setPayloadValue("info", "test"); });
        box->queue_event(EventType::SDL_Event,  [&](Event& ev) { ev.setPayloadValue("info", "test"); });
        box->queue_event(EventType::Quit,       [&](Event& ev) { ev.setPayloadValue("info", "test"); });
        box->queue_event(EventType::EnterFrame, [&](Event& ev) { ev.setPayloadValue("info", "test"); });

        // Dispatch all events
        core.pumpEventsOnce();

        // Verify all listeners were hit
        for (const auto& [name, hit] : hits) {
            if (!hit) {
                errors.push_back("Listener '" + name + "' was not hit.");
                ok = false;
            }
        }

        // Cleanup
        if (box->hasEventListener(EventType::None, false)) box->removeEventListener(EventType::None, nullptr, false);
        if (box->hasEventListener(EventType::SDL_Event, false)) box->removeEventListener(EventType::SDL_Event, nullptr, false);
        if (box->hasEventListener(EventType::Quit, false)) box->removeEventListener(EventType::Quit, nullptr, false);
        if (box->hasEventListener(EventType::EnterFrame, false)) box->removeEventListener(EventType::EnterFrame, nullptr, false);

        if (stage && stage->hasChild(box)) stage->removeChild(box);
        core.collectGarbage();

        // return the final test result
        return ok;
    } // END: Event_test3()


    // Test 4: Verify Mouse EventTypes round-trip properly
    bool Event_test4(std::vector<std::string>& errors)   
    {
        // ✅ Test Verified
        // 🔄 In Progress
        // ⚠️ Failing     
        // 🚫 Remove
        // ❌ Invalid
        // ☐ Planned./prog

        bool ok = true;  // 33

        // To send an error message to the test harness, use the following:
        // errors.push_error("Description of the error.");
        // ok = false;

        return ok;
    } // Event_test4(std::vector<std::string>& errors)       




    // --- Lua Integration Tests --- //

    bool Event_LUA_Tests(std::vector<std::string>& errors)
    {
        UnitTests& ut = UnitTests::getInstance();
        return ut.run_lua_tests(errors, ut.getLuaFilename());
    } // END: Event_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool Event_UnitTests()
    {
        UnitTests& ut = UnitTests::getInstance();
        ut.clear_tests();

        ut.add_test("Test scaffolding", Event_test0);
        ut.add_test("Verify Event table is registered and accessible", Event_test1);
        ut.add_test("Verify well-known static event types have expected flag states", Event_test2);
        ut.add_test("Verify Predefined EventTypes round-trip properly", Event_test3);
        ut.add_test("Verify Mouse EventTypes round-trip properly", Event_test4);




        ut.setLuaFilename("src/Event_UnitTests.lua"); // Lua test script path
        ut.add_test("Lua: " + ut.getLuaFilename(), Event_LUA_Tests, false);  // false = not implemented yet (dont run the lua file tests)


        return ut.run_all("Event");
    } // END: Event_UnitTests()



} // END: namespace SDOM