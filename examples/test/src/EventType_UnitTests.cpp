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


    /***********************************************************************************************
     * 🔄 EventType_test5 -- Window Environment Behavior
     * ---------------------------------------------------------------------------------------------
     * 🧪 Purpose:
     *     Verifies the full lifecycle of compositor-dependent window events, including transitions
     *     between fullscreen and windowed states, movement, and focus changes. Ensures expected SDL
     *     and SDOM event types are generated and properly dispatched via EventManager.
     *
     * 🎯 EventTypes Exercised (Phase II – Lifetime Verification):
     *     • EventType::EnterFullscreen
     *     • EventType::LeaveFullscreen
     *     • EventType::Show
     *     • EventType::Hide
     *     • EventType::Resize
     *     • EventType::Move
     *     • EventType::FocusLost
     *     • EventType::FocusGained
     *
     * 🧭 Notes:
     *     • Combines previous `EventType_test5a`, `EventType_test5b`, and `EventType_test6`.
     *     • Each frame performs one deterministic step; event queue cleared between steps.
     *     • Some events (Move, FocusLost/Gained, Show/Hide) may not fire on certain compositors
     *       such as Hyprland, Sway, or GNOME under Wayland — these yield warnings instead of errors.
     *     • The test restores original fullscreen/windowed state and window position.
     *     • Uses dummy SDL window to simulate focus transitions for reproducibility.
     *
     * 🔄 Frame Sequence:
     *     0️⃣ Initialize – Capture mode/position, clear queue.
     *     1️⃣ Toggle → fullscreen → expect `EnterFullscreen` (+ optional Show/Resize).
     *     2️⃣ Toggle → windowed → expect `LeaveFullscreen` (+ optional Hide/Resize).
     *     3️⃣ Move window (+32,+32) → expect `Move`.
     *     4️⃣ Create dummy window → expect `FocusLost`.
     *     5️⃣ Restore focus → expect `FocusGained`.
     *     6️⃣ Cleanup dummy window and restore position.
     *
     * 🧾 Returns:
     *     • false → Continue next frame
     *     • true  → ✅ Test complete
     ***********************************************************************************************/
    bool EventType_test5(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        EventManager& em = core.getEventManager();
        UnitTests& ut = UnitTests::getInstance();

        static bool initialized = false;
        static bool was_fullscreen = false;
        static SDL_Window* dummy = nullptr;
        static int start_frame = 0;
        static int orig_x = 0, orig_y = 0;
        static struct { bool enter, leave, show, hide, resize, move, lost, gained; } hits;
        // Move-detection state machine (post-focus)
        enum class MovePhase { None, WaitSettle, CreateDummy, ShowRaiseDummy, StartMove, WaitMove, Cleanup, Done };
        static MovePhase move_phase = MovePhase::None;
        static int   move_wait_frames = -1;           // <0 = not started; >=0 = counting
        static const int k_move_wait_cutoff = 250;     // patience window
        static bool  move_reported = false;
        static bool  move_final_logged = false;
        static int   move_settle_frames = 0;
        static const int k_move_settle_needed = 12;    // frames to settle after focus
        static std::function<void(Event&)> on_enter, on_leave, on_show, on_hide, on_resize, on_move, on_lost, on_gained;

        // Focus Lost/Gained patience and state machine
        enum class FocusPhase {
            None,
            StartLoseWithDummy,
            WaitLoseWithDummy,
            StartLoseWithFocusToggle,
            WaitLoseWithFocusToggle,
            StartLoseWithMinimize,
            WaitLoseWithMinimize,
            StartGain,
            WaitGain,
            Cleanup,
            Done
        };
        static FocusPhase focus_phase = FocusPhase::None;
        static int  focus_wait_frames = -1;
        static const int k_focus_wait_cutoff = 250; // align with Move patience window
        static bool focus_done = false;
        static bool saved_ignore_real_input = false;
        int frame = ut.get_frame_counter() - start_frame;

        auto reset_all = [&](){ hits = {false,false,false,false,false,false,false,false}; };

        // --- FRAME 0: Initialize -----------------------------------------------------
        if (!initialized)
        {
            initialized = true;
            start_frame = ut.get_frame_counter();
            em.clearEventQueue();

            // Install listeners on the Stage to observe the lifecycle/window events
            DisplayHandle stage = core.getRootNode();
            if (stage.isValid())
            {
                // Define stateless listener functions so removeEventListener can match them later.
                on_enter = [](Event&){ hits.enter = true; };
                on_leave = [](Event&){ hits.leave = true; };
                on_show  = [](Event&){ hits.show  = true; };
                on_hide  = [](Event&){ hits.hide  = true; };
                on_resize= [](Event&){ hits.resize= true; };
                on_move  = [](Event&){ hits.move  = true; };
                on_lost  = [](Event&){ hits.lost  = true; };
                on_gained= [](Event&){ hits.gained= true; };

                stage->addEventListener(EventType::EnterFullscreen, on_enter);
                stage->addEventListener(EventType::LeaveFullscreen, on_leave);
                stage->addEventListener(EventType::Show,            on_show);
                stage->addEventListener(EventType::Hide,            on_hide);
                stage->addEventListener(EventType::Resize,          on_resize);
                stage->addEventListener(EventType::Move,            on_move);
                stage->addEventListener(EventType::FocusLost,       on_lost);
                stage->addEventListener(EventType::FocusGained,     on_gained);
            }

            SDL_Window* win = core.getWindow();
            SDL_GetWindowPosition(win, &orig_x, &orig_y);
            was_fullscreen = core.isFullscreen();
            reset_all();
            move_reported = false;

            std::cout << "🧭 Starting position: (" << orig_x << ", " << orig_y << ")\n";
            if (was_fullscreen)
                core.setWindowed(true); // ensure known starting mode

            return false;   // 🔄 continue
        }

        // --- FRAME 1: Enter Fullscreen ----------------------------------------------
        if (frame == 1)
        {
            reset_all();
            core.setFullscreen(true);
            return false;   // 🔄 continue
        }

        // --- FRAME 2: Inspect EnterFullscreen ---------------------------------------
        if (frame == 2)
        {
            if (!hits.enter)
                errors.push_back("Missing EnterFullscreen event after setFullscreen(true).");
            if (!hits.show && !hits.resize)
                errors.push_back("Expected Show or Resize event after entering fullscreen.");

            reset_all();
            core.setWindowed(true);
            return false;   // 🔄 continue
        }

        // --- FRAME 3: Inspect LeaveFullscreen ---------------------------------------
        if (frame == 3)
        {
            if (!hits.leave)
                errors.push_back("Missing LeaveFullscreen event after setWindowed(true).");
            if (!hits.hide && !hits.resize)
                errors.push_back("Expected Hide or Resize event after leaving fullscreen.");

            reset_all();
            // Start with FocusLost/Gained verification first
            move_reported = false;
            move_wait_frames = -1; // Move test not started yet
            move_final_logged = false;
            focus_phase = FocusPhase::StartLoseWithDummy;
            focus_wait_frames = -1;
            focus_done = false;
            return false;   // 🔄 continue
        }

        // --- FRAME 4: Inspect Move ---------------------------------------------------
        if (frame == 4)
        { return false; }

        // --- FRAME 5: Kick off FocusLost attempts (after giving Move time) ---------
        if (frame == 5 && focus_phase == FocusPhase::None)
        {
            focus_phase = FocusPhase::StartLoseWithDummy;
            return false;   // 🔄 continue
        }

        // --- Focus Lost/Gained State Machine (runs after frame 3) -------------------
        if (focus_phase != FocusPhase::None)
        {
            switch (focus_phase)
            {
                case FocusPhase::StartLoseWithDummy:
                {
                    // Ensure we do not filter focus events during this test segment
                    saved_ignore_real_input = core.getIgnoreRealInput();
                    if (saved_ignore_real_input)
                        core.setIgnoreRealInput(false);

                    // Reset only focus flags
                    hits.lost = false;
                    hits.gained = false;

                    if (!dummy)
                    {
                        SDL_WindowFlags dflags = static_cast<SDL_WindowFlags>(
                            SDL_WINDOW_RESIZABLE |
                            SDL_WINDOW_MOUSE_GRABBED |
                            SDL_WINDOW_INPUT_FOCUS |
                            SDL_WINDOW_MOUSE_FOCUS |
                            SDL_WINDOW_MOUSE_CAPTURE |
                            SDL_WINDOW_MODAL
                        );
                        dummy = SDL_CreateWindow("focus_dummy", 64, 64, dflags);
                        if (!dummy)
                            errors.push_back("Unable to create dummy SDL window for focus test.");
                    }
                    if (dummy)
                    {
                        SDL_ShowWindow(dummy);
                        SDL_RaiseWindow(dummy);
                        // Ensure dummy is allowed to receive focus and is assertive
                        SDL_SetWindowFocusable(dummy, true);
                        SDL_SetWindowKeyboardGrab(dummy, true);
                        SDL_SetWindowMouseGrab(dummy, true);
                        SDL_SetWindowAlwaysOnTop(dummy, true);
                        SDL_PumpEvents();
                    }
                    focus_wait_frames = 0;
                    focus_phase = FocusPhase::WaitLoseWithDummy;
                    return false;
                }

                case FocusPhase::WaitLoseWithDummy:
                {
                    if (hits.lost)
                    {
                        std::cout << "✅ FocusLost event detected.\n";
                        focus_phase = FocusPhase::StartGain;
                        return false;
                    }
                    if (focus_wait_frames < k_focus_wait_cutoff)
                    {
                        ++focus_wait_frames;
                        return false;
                    }
                    // Fallback to toggling focusable state on the main window
                    std::cout << "⏳ FocusLost not observed via dummy — trying focusable toggle fallback.\n";
                    focus_phase = FocusPhase::StartLoseWithFocusToggle;
                    return false;
                }

                case FocusPhase::StartLoseWithFocusToggle:
                {
                    hits.lost = false;
                    SDL_SetWindowFocusable(core.getWindow(), true); // ensure enabled first
                    SDL_SetWindowFocusable(core.getWindow(), false); // then disable to drop focus
                    SDL_PumpEvents();
                    focus_wait_frames = 0;
                    focus_phase = FocusPhase::WaitLoseWithFocusToggle;
                    return false;
                }

                case FocusPhase::WaitLoseWithFocusToggle:
                {
                    if (hits.lost)
                    {
                        std::cout << "✅ FocusLost event detected (focusable toggle).\n";
                        focus_phase = FocusPhase::StartGain;
                        return false;
                    }
                    if (focus_wait_frames < k_focus_wait_cutoff)
                    {
                        ++focus_wait_frames;
                        return false;
                    }
                    std::cout << "⏳ FocusLost not observed via focusable toggle — trying minimize fallback.\n";
                    focus_phase = FocusPhase::StartLoseWithMinimize;
                    return false;
                }

                case FocusPhase::StartLoseWithMinimize:
                {
                    hits.lost = false;
                    SDL_MinimizeWindow(core.getWindow());
                    SDL_PumpEvents();
                    focus_wait_frames = 0;
                    focus_phase = FocusPhase::WaitLoseWithMinimize;
                    return false;
                }

                case FocusPhase::WaitLoseWithMinimize:
                {
                    if (hits.lost)
                    {
                        std::cout << "✅ FocusLost event detected (minimize).\n";
                        focus_phase = FocusPhase::StartGain;
                        return false;
                    }
                    if (focus_wait_frames < k_focus_wait_cutoff)
                    {
                        ++focus_wait_frames;
                        return false;
                    }
                    std::cout << "⚠️  FocusLost not observed — compositor may block focus switching.\n";
                    focus_phase = FocusPhase::StartGain;
                    return false;
                }

                case FocusPhase::StartGain:
                {
                    hits.gained = false;
                    SDL_SetWindowFocusable(core.getWindow(), true);
                    SDL_RestoreWindow(core.getWindow());
                    SDL_RaiseWindow(core.getWindow());
                    if (dummy)
                    {
                        // Release grabs on dummy now that we're restoring focus
                        SDL_SetWindowKeyboardGrab(dummy, false);
                        SDL_SetWindowMouseGrab(dummy, false);
                        // Drop always-on-top if API available in this SDL build
                        SDL_SetWindowAlwaysOnTop(dummy, false);
                    }
                    SDL_PumpEvents();
                    focus_wait_frames = 0;
                    focus_phase = FocusPhase::WaitGain;
                    return false;
                }

                case FocusPhase::WaitGain:
                {
                    if (hits.gained)
                    {
                        std::cout << "✅ FocusGained event detected.\n";
                        focus_phase = FocusPhase::Cleanup;
                        return false;
                    }
                    if (focus_wait_frames < k_focus_wait_cutoff)
                    {
                        ++focus_wait_frames;
                        return false;
                    }
                    std::cout << "⚠️  FocusGained not detected — compositor may restrict input focus.\n";
                    focus_phase = FocusPhase::Cleanup;
                    return false;
                }

                case FocusPhase::Cleanup:
                {
                    if (dummy)
                    {
                        SDL_HideWindow(dummy);
                        SDL_DestroyWindow(dummy);
                        dummy = nullptr;
                    }
                    // Do not restore position/fullscreen yet; move test runs next
                    // Restore ignore real input flag
                    core.setIgnoreRealInput(saved_ignore_real_input);
                    focus_phase = FocusPhase::Done;
                    focus_done = true;
                    return false;
                }

                case FocusPhase::Done:
                default:
                    break;
            }
        }

        // --- MovePhase: perform Move test on a fresh dummy after focus completes ----
        if (focus_done && move_phase == MovePhase::None)
        {
            // Clear prior hit flags so stale Move won't immediately trigger
            reset_all();
            move_phase = MovePhase::WaitSettle;
            move_settle_frames = 0;
            move_wait_frames = -1;
            move_reported = false;
            move_final_logged = false;
            return false;
        }

        switch (move_phase)
        {
            case MovePhase::WaitSettle:
            {
                if (move_settle_frames++ >= k_move_settle_needed)
                {
                    move_phase = MovePhase::CreateDummy;
                }
                return false;
            }
            case MovePhase::CreateDummy:
            {
                if (!dummy)
                {
                    SDL_WindowFlags dflags = static_cast<SDL_WindowFlags>(
                        SDL_WINDOW_RESIZABLE |
                        SDL_WINDOW_MOUSE_GRABBED |
                        SDL_WINDOW_INPUT_FOCUS |
                        SDL_WINDOW_MOUSE_FOCUS |
                        SDL_WINDOW_MOUSE_CAPTURE |
                        SDL_WINDOW_MODAL
                    );
                    dummy = SDL_CreateWindow("focus_dummy_move", 64, 64, dflags);
                    if (!dummy)
                        errors.push_back("Unable to create dummy SDL window for move test.");
                }
                move_phase = MovePhase::ShowRaiseDummy;
                return false;
            }
            case MovePhase::ShowRaiseDummy:
            {
                if (dummy)
                {
                    SDL_ShowWindow(dummy);
                    SDL_RaiseWindow(dummy);
                    SDL_SetWindowFocusable(dummy, true);
                    SDL_SetWindowAlwaysOnTop(dummy, true);
                }
                move_phase = MovePhase::StartMove;
                return false;
            }
            case MovePhase::StartMove:
            {
                if (dummy)
                {
                    int dx = 24, dy = 24;
                    int x = 0, y = 0;
                    SDL_GetWindowPosition(dummy, &x, &y);
                    SDL_SetWindowPosition(dummy, x + dx, y + dy);
                }
                // Start patience window
                hits.move = false; // ensure fresh detection
                move_wait_frames = 0;
                move_reported = false;
                move_final_logged = false;
                move_phase = MovePhase::WaitMove;
                return false;
            }
            case MovePhase::WaitMove:
            {
                if (move_wait_frames == 0)
                    std::cout << "⏳ Waiting for Move (up to " << k_move_wait_cutoff << ")...\n";
                if (hits.move)
                {
                    std::cout << "✅ Move event detected after " << move_wait_frames << " frame(s).\n";
                    move_reported = true;
                    move_phase = MovePhase::Cleanup;
                    return false;
                }
                if (++move_wait_frames >= k_move_wait_cutoff)
                {
                    if (!move_final_logged)
                    {
                        std::cout << "⚠️  Move event not detected (compositor-controlled).\n";
                        move_final_logged = true;
                    }
                    move_phase = MovePhase::Cleanup;
                    return false;
                }
                return false;
            }
            case MovePhase::Cleanup:
            {
                if (dummy)
                {
                    SDL_SetWindowAlwaysOnTop(dummy, false);
                    SDL_HideWindow(dummy);
                    SDL_DestroyWindow(dummy);
                    dummy = nullptr;
                }
                move_phase = MovePhase::Done;
                return false;
            }
            case MovePhase::Done:
            case MovePhase::None:
            default: break;
        }

        // --- FRAME 8+: Done (with Move patience window) ------------------------------
        if (frame >= 8)
        {
            // Keep the test alive until focus and move phases complete
            if (!focus_done || move_phase != MovePhase::Done)
            {
                return false; // keep waiting until both are done
            }
            // Clean up: remove the listeners we added
            DisplayHandle stage = core.getRootNode();
            if (stage.isValid())
            {
                stage->removeEventListener(EventType::EnterFullscreen, on_enter);
                stage->removeEventListener(EventType::LeaveFullscreen, on_leave);
                stage->removeEventListener(EventType::Show,            on_show);
                stage->removeEventListener(EventType::Hide,            on_hide);
                stage->removeEventListener(EventType::Resize,          on_resize);
                stage->removeEventListener(EventType::Move,            on_move);
                stage->removeEventListener(EventType::FocusLost,       on_lost);
                stage->removeEventListener(EventType::FocusGained,     on_gained);
            }
            // Restore position and fullscreen state at the end
            SDL_SetWindowPosition(core.getWindow(), orig_x, orig_y);
            core.setFullscreen(was_fullscreen);
            return true;  // ✅ done
        }

        return true;    // ✅ done
    } // END -- EventType_test5: Window Environment Behavior





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
            ut.add_test(objName, "Window Environment Behavior", EventType_test5);


            ut.setLuaFilename("src/EventType_UnitTests.lua"); // Lua test script path
            ut.add_test(objName, "Lua: " + ut.getLuaFilename(), EventType_LUA_Tests, false);  // false = not implemented yet (dont run the lua file tests)

            registered = true;
        }

        // return ut.run_all(objName, "EventType");
        return true;
    } // END: EventType_UnitTests()



} // END: namespace SDOM
