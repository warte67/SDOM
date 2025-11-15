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
    bool EventType_test0([[maybe_unused]] std::vector<std::string>& errors)
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
     * 🔄 EventType_test5 -- Window Environment Behavior (Revised)
     * ---------------------------------------------------------------------------------------------
     * 🧪 Purpose:
     *     Verifies compositor-dependent window behavior for fullscreen transitions and window
     *     movement. Ensures SDL/SDOM events are dispatched correctly across all supported platforms.
     *
     * 🎯 EventTypes Exercised:
     *     • EventType::EnterFullscreen
     *     • EventType::LeaveFullscreen
     *     • EventType::Show
     *     • EventType::Hide
     *     • EventType::Resize
     *     • EventType::Move
     *
     * 🧭 Notes:
     *     • FocusLost/FocusGained checks removed — not consistently supported under Wayland/X11.
     *     • Wayland detection remains for Move/Fullscreen warning context.
     *     • Deterministic, frame-based progression for stable automation.
     *     • Restores original position and mode upon completion.
     *
     * 🔄 Frame Sequence:
     *     0️⃣ Initialize – capture mode/position, attach listeners, clear event queue.
     *     1️⃣ Enter fullscreen → expect `EnterFullscreen` (+ optional Show/Resize).
     *     2️⃣ Return to windowed → expect `LeaveFullscreen` (+ optional Hide/Resize).
     *     3️⃣ Move window (+24,+24) → expect `Move`.
     *     4️⃣ Restore position/state and complete.
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
        static int start_frame = 0;
        static int orig_x = 0, orig_y = 0;
        static float orig_win_w = 0.0f, orig_win_h = 0.0f;
        static struct { bool enter, leave, show, hide, resize, move; } hits;
        static bool is_wayland = false; (void)is_wayland;

        // Move-detection state machine
        enum class MovePhase { None, WaitSettle, StartMove, WaitMove, Done };
        static MovePhase move_phase = MovePhase::None;
        static int move_wait_frames = -1;
        static int move_settle_frames = 0;
        static bool move_final_logged = false;
        static const int k_move_wait_cutoff = 250;
        static const int k_move_settle_needed = 12;

        // Diagnostics                                      // stop the compiler from complaining
        static int move_start_x = 0, move_start_y = 0;      (void)move_start_x; (void)move_start_y;
        static int move_target_x = 0, move_target_y = 0;    (void)move_target_x; (void)move_target_y;
        static int move_detect_frame = -1;                  (void)move_detect_frame;
        static int move_event_x = -1, move_event_y = -1;    (void)move_event_x; (void)move_event_y;
        static Uint32 move_event_window_id = 0;             (void)move_event_window_id;

        static std::function<void(Event&)> on_enter, on_leave, on_show, on_hide, on_resize, on_move;

        auto reset_all = [&]() { hits = {false,false,false,false,false,false}; };

        int frame = ut.get_frame_counter() - start_frame;

        // --- FRAME 0: Initialize ---------------------------------------------------------
        if (!initialized)
        {
            initialized = true;
            start_frame = ut.get_frame_counter();
            em.clearEventQueue();

            // Install listeners
            DisplayHandle stage = core.getRootNode();
            if (stage.isValid())
            {
                on_enter = [](Event&){ hits.enter = true; };
                on_leave = [](Event&){ hits.leave = true; };
                on_show  = [](Event&){ hits.show  = true; };
                on_hide  = [](Event&){ hits.hide  = true; };
                on_resize= [](Event&){ hits.resize= true; };
                on_move  = [&](Event& ev){
                    hits.move = true;
                    move_detect_frame = ut.get_frame_counter();
                    SDL_Event se = ev.getSDL_Event();
                    if (se.type == SDL_EVENT_WINDOW_MOVED)
                    {
                        move_event_x = se.window.data1;
                        move_event_y = se.window.data2;
                        move_event_window_id = se.window.windowID;
                    }
                };

                stage->addEventListener(EventType::EnterFullscreen, on_enter);
                stage->addEventListener(EventType::LeaveFullscreen, on_leave);
                stage->addEventListener(EventType::Show,            on_show);
                stage->addEventListener(EventType::Hide,            on_hide);
                stage->addEventListener(EventType::Resize,          on_resize);
                stage->addEventListener(EventType::Move,            on_move);
            }

            SDL_Window* win = core.getWindow();
            SDL_GetWindowPosition(win, &orig_x, &orig_y);
            Uint32 init_flags = SDL_GetWindowFlags(win); (void)init_flags;  
            orig_win_w = core.getWindowWidth();
            orig_win_h = core.getWindowHeight();
            was_fullscreen = core.isFullscreen();
            reset_all();

            // Force windowed state before beginning
            core.setWindowed(true);

            // Wayland detection
            {
                const char* xdg = SDL_getenv("XDG_SESSION_TYPE");
                const char* wdisp = SDL_getenv("WAYLAND_DISPLAY");
                const char* drv = SDL_GetCurrentVideoDriver();
                if ((xdg && std::string(xdg) == "wayland") ||
                    (wdisp && *wdisp) ||
                    (drv && std::string(drv).find("wayland") != std::string::npos))
                    is_wayland = true;
            }
            // We now have a wayland flag to use when/if we need it for custom logic

            // if (is_wayland)
            //     std::cout << "⚠️ Wayland session detected — compositor may ignore focus/raise and movement requests.\n";

            return false; // 🔄 continue
        }

        // --- FRAME 1: Enter Fullscreen ---------------------------------------------------
        if (frame == 1)
        {
            reset_all();
            core.setFullscreen(true);
            return false;
        }

        // --- FRAME 2: Inspect EnterFullscreen --------------------------------------------
        if (frame == 2)
        {
            if (!hits.enter)
                errors.push_back("Missing EnterFullscreen event after setFullscreen(true).");

            if (!hits.show && !hits.resize)
                errors.push_back("Expected Show or Resize event after entering fullscreen.");

            reset_all();
            core.setWindowed(true);
            return false;
        }

        // --- FRAME 3: Inspect LeaveFullscreen --------------------------------------------
        if (frame == 3)
        {
            if (!hits.leave)
            {
                // if (is_wayland)
                //     std::cout << "⚠️ Wayland: LeaveFullscreen not observed (compositor policy).\n";
                // else
                    errors.push_back("Missing LeaveFullscreen event after setWindowed(true).");
            }

            if (!hits.hide && !hits.resize)
            {
                // if (is_wayland)
                //     std::cout << "⚠️ Wayland: Hide/Resize not observed after leaving fullscreen.\n";
                // else
                    errors.push_back("Expected Hide or Resize event after leaving fullscreen.");
            }

            reset_all();
            move_phase = MovePhase::WaitSettle;
            move_settle_frames = 0;
            return false;
        }

        // --- MovePhase State Machine -----------------------------------------------------
        switch (move_phase)
        {
            case MovePhase::WaitSettle:
            {
                if (move_settle_frames++ >= k_move_settle_needed)
                    move_phase = MovePhase::StartMove;
                return false;
            }

            case MovePhase::StartMove:
            {
                int x = 0, y = 0;
                SDL_GetWindowPosition(core.getWindow(), &x, &y);
                move_start_x = x; move_start_y = y;
                move_target_x = x + 24; move_target_y = y + 24;
                // std::cout << "🧭 MoveAttempt: main window from (" << move_start_x << "," << move_start_y
                //         << ") to (" << move_target_x << "," << move_target_y << ")\n";
                SDL_SetWindowPosition(core.getWindow(), move_target_x, move_target_y);
                SDL_SyncWindow(core.getWindow());
                hits.move = false;
                move_wait_frames = 0;
                move_final_logged = false;
                move_detect_frame = -1;
                move_event_x = move_event_y = -1;
                move_event_window_id = 0;
                move_phase = MovePhase::WaitMove;
                return false;
            }

            case MovePhase::WaitMove:
            {
                if (hits.move)
                {
                    // std::cout << "✅ Move event detected (main window) after " << move_wait_frames
                    //         << " frame(s). Event windowID=" << move_event_window_id
                    //         << " pos=(" << move_event_x << "," << move_event_y << ")"
                    //         << "; target=(" << move_target_x << "," << move_target_y << ")\n";
                    move_phase = MovePhase::Done;
                    return false;
                }

                if (++move_wait_frames >= k_move_wait_cutoff)
                {
                    if (!move_final_logged)
                    {
                        int cx = 0, cy = 0;
                        SDL_GetWindowPosition(core.getWindow(), &cx, &cy);
                        std::ostringstream msg;
                        msg << "⚠️  Move event not detected (compositor-controlled). FinalPos=("
                            << cx << "," << cy << ") target=("
                            << move_target_x << "," << move_target_y << ")";
                        std::string message = msg.str();
                        errors.push_back(message);

                        move_final_logged = true;
                    }
                    move_phase = MovePhase::Done;
                    return false;
                }
                return false;
            }

            case MovePhase::Done:
            case MovePhase::None:
            default:
                break;
        }

        // --- FRAME ≥ 8: Cleanup ----------------------------------------------------------
        if (frame >= 8 && move_phase == MovePhase::Done)
        {
            DisplayHandle stage = core.getRootNode();
            if (stage.isValid())
            {
                stage->removeEventListener(EventType::EnterFullscreen, on_enter);
                stage->removeEventListener(EventType::LeaveFullscreen, on_leave);
                stage->removeEventListener(EventType::Show,            on_show);
                stage->removeEventListener(EventType::Hide,            on_hide);
                stage->removeEventListener(EventType::Resize,          on_resize);
                stage->removeEventListener(EventType::Move,            on_move);
            }

            SDL_SetWindowPosition(core.getWindow(), orig_x, orig_y);
            core.setWindowWidth(orig_win_w);
            core.setWindowHeight(orig_win_h);
            core.setFullscreen(was_fullscreen);

            return true; // ✅ done
        }

        return false; // keep running until completion
    } // END -- EventType_test5 (Window Environment Behavior)






    // --- Lua Integration Tests --- //

    bool EventType_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/EventType_UnitTests.lua");
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


            ut.add_test(objName, "Lua: src/EventType_UnitTests.lua", EventType_LUA_Tests, false);

            registered = true;
        }

        // return ut.run_all(objName, "EventType");
        return true;
    } // END: EventType_UnitTests()



} // END: namespace SDOM
