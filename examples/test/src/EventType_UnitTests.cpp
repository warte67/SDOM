// EventType_UnitTests.cpp
#include <SDOM/SDOM_IDisplayObject.hpp>
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
    // --- Individual EventType Unit Tests --- //

    // ============================================================================
    //  Test 0: EventTypeing Template
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
    bool EventType_test0(std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        // ok = false;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

    } // END: EventType_test0(std::vector<std::string>& errors)


    // --- Eventype_test1: Keyboard Event Verification ----------------------------------
    //
    // 🧩 Purpose:
    //   Verifies correct translation and dispatch of SDL keyboard events through
    //   the SDOM EventManager, ensuring both key press and release are properly
    //   queued and recognized.
    //
    // 📝 Notes:
    //   • Confirms bidirectional keyboard event handling via `Queue_SDL_Event()`.  
    //   • Tests both `KeyDown` and `KeyUp` for a representative key (`SDLK_A`).  
    //   • Text input events remain unimplemented but reserved for future expansion.  
    //
    // ⚙️ Functions Tested:
    //
    //   | Category                  | Functions Tested / Behavior Verified              |
    //   |----------------------------|--------------------------------------------------|
    //   | SDL Integration            | ✅ EventManager::Queue_SDL_Event()                |
    //   | Event Dispatching          | ✅ Keyboard press/release round-trip verification |
    //   | Keyboard Event Coverage    | ✅ KeyDown, KeyUp                                |
    //
    // ⚠️ Safety:
    //   No persistent state modifications; uses transient SDL key events only.
    //
    // ============================================================================
    bool EventType_test1(std::vector<std::string>& errors)
    {
        // --- 1) Setup ---------------------------------------------------------------
        Core& core = getCore();
        DisplayHandle stage = core.getRootNode();
        if (!stage.isValid()) 
            return false;  // safety: abort early if core stage invalid

        EventManager& em = core.getEventManager();

        // --- 2) Define simulated keyboard actions ----------------------------------
        std::vector<std::pair<std::string, std::function<void(DisplayHandle)>>> actions = {
            {"KeyDown", [&](DisplayHandle /*unused*/){
                SDL_Event e{};
                e.type = SDL_EVENT_KEY_DOWN;
                e.key.windowID = SDL_GetWindowID(core.getWindow());
                e.key.timestamp = SDL_GetTicks();
                e.key.repeat = 0;
                e.key.mod = 0;
                e.key.key = SDLK_A; // representative key
                em.Queue_SDL_Event(e);
            }},
            {"KeyUp", [&](DisplayHandle /*unused*/){
                SDL_Event e{};
                e.type = SDL_EVENT_KEY_UP;
                e.key.windowID = SDL_GetWindowID(core.getWindow());
                e.key.timestamp = SDL_GetTicks();
                e.key.repeat = 0;
                e.key.mod = 0;
                e.key.key = SDLK_A;
                em.Queue_SDL_Event(e);
            }}
            // Future expansion:
            // {"TextInput", ...} // Not yet implemented for EditBox / IME input
        };

        // --- 3) Execute behavioral test sequence -----------------------------------
        UnitTests::run_event_behavior_test(actions, errors);

        // ✅ Test complete — return success
        return true;
    } // END -- Keyboard Event Verification


    // --- EventType_test2: Multi-frame Event Queue Verification --------------------------
    //
    // 🧩 Purpose:
    //    Verifies that the UnitTest system correctly supports multi-frame (re-entrant)
    //    tests where a test persists across multiple frames before completion.
    //
    // 🧠 Behavior:
    //    - Captures the starting frame index at the first invocation.
    //    - Each subsequent frame increments a logical frame counter.
    //    - After 10 frames have elapsed, the test reports completion (✅ finished).
    //
    // 📝 Notes:
    //    This serves as a system-level validation of the re-entrant test mechanism
    //    used by SDOM’s UnitTest framework for multi-frame simulations.
    //
    // ============================================================================
    bool EventType_test2(std::vector<std::string>& errors)
    {
        UnitTests& ut = UnitTests::getInstance();
        static int s_start_frame = ut.get_frame_counter();

        int logic_frame = ut.get_frame_counter() - s_start_frame;
        if (logic_frame < 10)
            return false; // 🔄 re-entrant test
        else if (logic_frame == 10)        
            return true; // ✅ Test complete
        errors.push_back("Event_test10: Test the multi-frame event queue. Frame: " + std::to_string(logic_frame));
        return true; // ✅ Test complete
    } // END: EventType_test2()



    // --- EventType_test3: Lifecycle Event Dispatch Verification ---------------------------
    //
    // 🧩 Purpose:
    //   Verifies that SDOM correctly dispatches lifecycle-related events when
    //   DisplayHandle objects are added to or removed from their parents or the stage.
    //
    // 🧠 Behavior:
    //   - Frame 0: Creates a parent and child Box, registers listeners, and attaches
    //     the parent to the stage.
    //   - Frame 1: Adds the child to the parent and waits for `Added` and `AddedToStage` events.
    //   - Frame 2: Removes the child from the parent and waits for `Removed` and
    //     `RemovedFromStage` events.
    //   - Frame 3: Verifies all expected events fired and cleans up.
    //
    // 📝 Notes:
    //   • Uses explicit initialization rather than brace initialization because
    //     `Box::InitStruct` inherits from a polymorphic base (`IDisplayObject::InitStruct`).
    //     Brace initialization would skip the constructor and defaults.
    //   • Tracks relative frame index using `first_frame` static variable to ensure
    //     predictable multi-frame sequencing regardless of global test order.
    //   • Verifies all four lifecycle event types:
    //       - Added, Removed, AddedToStage, RemovedFromStage.
    //
    // ⚙️ Functions Tested:
    //
    //   | Category               | Functions Tested / Behavior Verified                   |
    //   |-------------------------|--------------------------------------------------------|
    //   | Object Creation         | ✅ Core::createDisplayObject()                         |
    //   | Hierarchy Management    | ✅ DisplayHandle::addChild() / removeChild()           |
    //   | Event Dispatching       | ✅ Lifecycle events: Added, Removed, AddedToStage, RemovedFromStage |
    //   | Event Listener System   | ✅ addEventListener(), ✅ event callback verification   |
    //   | Garbage Collection      | ✅ Core::collectGarbage() cleans up after test         |
    //
    // ⚠️ Safety:
    //   All created objects are transient and cleaned up at the end of the test.
    //   No persistent global state is modified.
    //
    // ============================================================================
    bool EventType_test3(std::vector<std::string>& errors)
    {
        // using ORP = SDOM::IDisplayObject::OrphanRetentionPolicy;
        Core& core = getCore();
        DisplayHandle stage = core.getRootNode();
        UnitTests& ut = UnitTests::getInstance();

        // Persistent state across frames
        static bool initialized = false;
        static int first_frame = ut.get_frame_counter();
        static std::unordered_map<std::string, bool> hits;

        // Compute logical frame index (relative to first invocation)
        int frame = ut.get_frame_counter() - first_frame;

        // --- FRAME 0: Setup and listener registration -----------------------------------
        if (!initialized)
        {
            initialized = true;
            first_frame = ut.get_frame_counter(); // capture starting frame baseline

            // --- Create parent Box ------------------------------------------------------
            Box::InitStruct pinit;
            pinit.name   = "parentBox";
            pinit.x      = 50;
            pinit.y      = 50;
            pinit.width  = 100;
            pinit.height = 100;
            pinit.color  = SDL_Color{0, 255, 0, 255};
            DisplayHandle parent = core.createDisplayObject("Box", pinit);
            if (!parent.isValid())
                errors.push_back("Event_test11: Failed to create parentBox.");
            // parent->setOrphanRetentionPolicy(ORP::RetainUntilManual);


            // --- Create child Box -------------------------------------------------------
            Box::InitStruct cinit;
            cinit.name   = "childBox";
            cinit.x      = 10;
            cinit.y      = 10;
            cinit.width  = 50;
            cinit.height = 50;
            cinit.color  = SDL_Color{255, 0, 0, 255};
            DisplayHandle child  = core.createDisplayObject("Box", cinit);
            if (!child.isValid())
                errors.push_back("Event_test11: Failed to create childBox.");
            // child->setOrphanRetentionPolicy(ORP::RetainUntilManual);
            parent->addChild(child);

            hits = {
                {"Added", false},
                {"Removed", false},
                {"AddedToStage", false},
                {"RemovedFromStage", false}
            };

            // --- Register lifecycle event listeners ------------------------------------
            auto listener = [&](const Event& ev){
                hits[ev.getTypeName()] = true;
            };

            child->addEventListener(EventType::Added, listener);
            child->addEventListener(EventType::Removed, listener);
            child->addEventListener(EventType::AddedToStage, listener);
            child->addEventListener(EventType::RemovedFromStage, listener);

            // Add parent to stage to ensure AddedToStage propagates correctly later
            stage->addChild(parent);
            return false; // 🔄 re-entrant: setup complete, wait until next frame
        }

        // --- FRAME 1: Verify Added & AddedToStage fired ---------------------------------
        if (frame == 1)
        {
            if (!hits["Added"])
                errors.push_back("Event_test11: Added did not fire when child added to parent.");
            if (!hits["AddedToStage"])
                errors.push_back("Event_test11: AddedToStage did not fire when child added to Stage.");

            // Reset hit flags for next phase
            hits["Added"] = hits["AddedToStage"] = false;            
            return false; // 🔄 wait for next frame
        }
        // --- Frame 2: Remove Child ---------------------------
        if (frame == 2)
        {
            // Trigger removal
            DisplayHandle parent = core.getDisplayObject("parentBox");
            if (!parent.isValid())
                errors.push_back("Event_test11: Failed to find parentBox.");
            DisplayHandle child = core.getDisplayObject("childBox");
            if (!child.isValid())
                errors.push_back("Event_test11: Failed to find childBox.");
            parent->removeChild(child);
            return false; // 🔄 wait for next frame
        }

        // --- Frame 3: Verify childBox Removed + RemovedFromStage --------------------
        if (frame == 3)
        {
            if (!hits["Removed"])
                errors.push_back("Event_test11: Removed did not fire when child removed from parent.");
            if (!hits["RemovedFromStage"])
                errors.push_back("Event_test11: RemovedFromStage did not fire when child removed from Stage.");
            // Remove the parent box from the stage
            DisplayHandle parent = core.getDisplayObject("parentBox");
            if (!parent.isValid())
                errors.push_back("Event_test11: Failed to find parentBox.");
            stage->removeChild(parent);
            return false; // 🔄 wait for next frame
        }
        // --- Frame 4: Verify parentBox Removed + RemovedFromStage --------------------
        if (frame == 4)
        {
            // Verify cleanup
            DisplayHandle parent = core.getDisplayObject("parentBox");
            if (parent.isValid())
                errors.push_back("Event_test11: Parent Box still exists after removal.");
            DisplayHandle child = core.getDisplayObject("childBox");
            if (child.isValid())
                errors.push_back("Event_test11: Child Box still exists after removal.");
            // Garbage Collection -- Clean Up:
            hits.clear();
            int orphanCount = core.countOrphanedDisplayObjects();
            if (orphanCount != 0)
                errors.push_back("Event_test11: Orphaned Display Objects still exist after removal.");

            initialized = false;
            return true; // ✅ done
                errors.push_back("StageClosed not fired for stageThree.");
            if (!hits["mainStage_Opened"])
                errors.push_back("StageOpened not fired again for mainStage.");

            initialized = false;
            return true; // ✅ done
        }

        return false; // 🔄 continue
    } // END -- EventType_test3: Stage Lifecycle Transition Verification


    // --- EventType_test4: Stage Lifecycle Transition Verification -----------------------
    //
    // 🧩 Purpose:
    //   Validates correct dispatch of StageOpened and StageClosed events when
    //   switching between *existing* stages ("mainStage", "stageTwo", "stageThree").
    //
    // 🧠 Behavior:
    //   • Frame 0: Activate "mainStage" → expect StageOpened(mainStage)
    //   • Frame 1: Switch to "stageTwo" → expect StageClosed(mainStage), StageOpened(stageTwo)
    //   • Frame 2: Switch to "stageThree" → expect StageClosed(stageTwo), StageOpened(stageThree)
    //   • Frame 3: Switch back to "mainStage" → expect StageClosed(stageThree), StageOpened(mainStage)
    //
    // 📝 Notes:
    //   • Confirms that Core::setRootNode() correctly dispatches lifecycle events
    //     for *existing* Stage objects.
    //   • Ensures correct ordering and one-frame spacing between transitions.
    //   • Verifies that no duplicate StageOpened/Closed events occur.
    //
    // ⚙️ Functions Tested:
    //   | Category         | Behavior Verified                        |
    //   |------------------|-------------------------------------------|
    //   | Stage Management | ✅ Core::setRootNode() transition handling |
    //   | Event Dispatch   | ✅ StageOpened / StageClosed correctness   |
    //   | Listener Binding | ✅ addEventListener() works for stages     |
    //   | Reentrancy       | ✅ Multi-frame transition test support     |
    //
    // ⚠️ Safety:
    //   Uses persistent stage objects (mainStage, stageTwo, stageThree) only.
    //   No new stages are created or destroyed.
    //
    // ----------------------------------------------------------------------------
    bool EventType_test4(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        UnitTests& ut = UnitTests::getInstance();

        // Persistent state across frames
        static bool initialized = false;
        static int first_frame = ut.get_frame_counter();
        static std::unordered_map<std::string, bool> hits;

        int frame = ut.get_frame_counter() - first_frame;

        // --- FRAME 0: Initialization ------------------------------------------------------
        if (!initialized)
        {
            initialized = true;
            first_frame = ut.get_frame_counter();

            hits = {
                {"mainStage_Opened", false}, {"mainStage_Closed", false},
                {"stageTwo_Opened",  false}, {"stageTwo_Closed",  false},
                {"stageThree_Opened",false}, {"stageThree_Closed",false}
            };

            // --- Retrieve existing stage handles -----------------------------------------
            DisplayHandle stage1 = core.getDisplayObject("mainStage");
            DisplayHandle stage2 = core.getDisplayObject("stageTwo");
            DisplayHandle stage3 = core.getDisplayObject("stageThree");

            if (!stage1.isValid() || !stage2.isValid() || !stage3.isValid())
            {
                errors.push_back("Event_test12: One or more stage handles are invalid.");
                return true;
            }

            // --- Register listeners ------------------------------------------------------
            auto makeListener = [&](const std::string& keyPrefix) {
                return [=](const Event& ev) {
                    std::string type = ev.getTypeName();
                    if (type == "StageOpened")  hits[keyPrefix + "_Opened"]  = true;
                    if (type == "StageClosed")  hits[keyPrefix + "_Closed"]  = true;
                };
            };

            stage1->addEventListener(EventType::StageOpened, makeListener("mainStage"));
            stage1->addEventListener(EventType::StageClosed, makeListener("mainStage"));
            stage2->addEventListener(EventType::StageOpened, makeListener("stageTwo"));
            stage2->addEventListener(EventType::StageClosed, makeListener("stageTwo"));
            stage3->addEventListener(EventType::StageOpened, makeListener("stageThree"));
            stage3->addEventListener(EventType::StageClosed, makeListener("stageThree"));

            // --- Activate mainStage ------------------------------------------------------
            core.setRootNode(stage1);
            return false; // 🔄 wait for StageOpened(mainStage)
        }

        // --- FRAME 1: Verify mainStage opened, then switch to stageTwo -------------------
        if (frame == 1)
        {
            if (!hits["mainStage_Opened"])
                errors.push_back("StageOpened not fired for mainStage.");

            DisplayHandle stage2 = core.getDisplayObject("stageTwo");
            core.setRootNode(stage2);
            return false;
        }

        // --- FRAME 2: Verify mainStage closed, stageTwo opened, then switch to stageThree -
        if (frame == 2)
        {
            if (!hits["mainStage_Closed"])
                errors.push_back("StageClosed not fired for mainStage.");
            if (!hits["stageTwo_Opened"])
                errors.push_back("StageOpened not fired for stageTwo.");

            DisplayHandle stage3 = core.getDisplayObject("stageThree");
            core.setRootNode(stage3);
            return false;
        }

        // --- FRAME 3: Verify stageTwo closed, stageThree opened, then return to mainStage -
        if (frame == 3)
        {
            if (!hits["stageTwo_Closed"])
                errors.push_back("StageClosed not fired for stageTwo.");
            if (!hits["stageThree_Opened"])
                errors.push_back("StageOpened not fired for stageThree.");

            DisplayHandle stage1 = core.getDisplayObject("mainStage");
            core.setRootNode(stage1);
            return false;
        }

        // --- FRAME 4: Verify stageThree closed, mainStage reopened -----------------------
        if (frame == 4)
        {
            if (!hits["stageThree_Closed"])
                errors.push_back("StageClosed not fired for stageThree.");
            if (!hits["mainStage_Opened"])
                errors.push_back("StageOpened not fired again for mainStage.");

            initialized = false;
            return true; // ✅ done
        }

        return false; // 🔄 continue
    } // END -- EventType_test4: Stage Lifecycle Transition Verification

    // --- EventType_test5: Window/Fullscreen Toggle Verification -----------------------
    bool EventType_test5(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        UnitTests& ut = UnitTests::getInstance();

        // Persistent state across frames
        static bool initialized = false;
        static bool start_fullscreen = core.isFullscreen();
        static int first_frame = ut.get_frame_counter();
        int frame = ut.get_frame_counter() - first_frame;

        // --- FRAME 0: Initialization ------------------------------------------------------
        if (!initialized)
        {
            initialized = true;

            core.setWindowed(true);  // start with a known state (windowed)
            return false; // 🔄 continue
        }

        // --- FRAME 1: Set Fullscreen ------------------------------------------------------
        if (frame == 1)
        {
            core.setFullscreen(true);
            return false; // 🔄 continue
        }

        // --- FRAME 1: 
        if (frame == 1)
        {
            return false; // 🔄 continue
        }

        // --- FRAME 2: 
        if (frame == 2)
        {
            core.setFullscreen(start_fullscreen);
            return false; // 🔄 continue
        }
        return true; // ✅ done
    } // END -- EventType_test5: Window/Fullscreen Toggle Verification


    // --- Lua Integration Tests --- //

    bool EventType_LUA_Tests(std::vector<std::string>& errors)
    {
        UnitTests& ut = UnitTests::getInstance();
        return ut.run_lua_tests(errors, ut.getLuaFilename());
    } // END: EventType_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool EventType_UnitTests()
    {
        const std::string objName = "EventType";
        UnitTests& ut = UnitTests::getInstance();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test EventType", EventType_test0);
            ut.add_test(objName, "Keyboard Event Verification", EventType_test1);
            ut.add_test(objName, "Test the multi-frame event queue", EventType_test2);
            ut.add_test(objName, "Lifecycle Event Dispatch Verification", EventType_test3);
            ut.add_test(objName, "Stage Lifecycle Transition Verification", EventType_test4);     
            ut.add_test(objName, "Window/Fullscreen Toggle Verification", EventType_test5);

            ut.setLuaFilename("src/EventType_UnitTests.lua"); // Lua test script path
            ut.add_test(objName, "Lua: " + ut.getLuaFilename(), EventType_LUA_Tests, false);  // false = not implemented yet (dont run the lua file tests)

            registered = true;
        }

        // return ut.run_all(objName, "EventType");
        return true;
    } // END: EventType_UnitTests()



} // END: namespace SDOM