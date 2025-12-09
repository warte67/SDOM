// Core_UnitTestss.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS


#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp> 
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/CAPI/SDOM_CAPI_Handles.h>
#include <SDOM/CAPI/SDOM_CAPI_Variant.h>

#include <string>
#include <vector>
#include <functional>

#include "UnitTests.hpp"

// Checks: ⚠️ ✅ ✔  


namespace SDOM
{
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
    bool Core_test0([[maybe_unused]] std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        // bool done = false;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        // To fetch the current frame:
        // int current_frame = UnitTests::getInstance().get_frame_counter();

        // Use State Machines when re-entrant tests are required to test SDOM main loop

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

        // return done; // true to conclude the test, false to re-enter

    } // END: Core_scaffolding(std::vector<std::string>& errors)

    // Validates variant C API create/get/destroy parity for display objects.
    bool Core_Variant_Display_CreateGetDestroy(std::vector<std::string>& errors)
    {
        // Create via POD path
        const char* stage_pod_json = R"({"type":"Stage","name":"variant_stage_pod"})";
        SDOM_ClearError();
        SDOM_DisplayHandle pod{};
        if (!SDOM_CreateDisplayObjectFromJson("Stage", stage_pod_json, &pod)) {
            errors.push_back(std::string("POD create failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
            return true;
        }

        // Create via variant path (distinct name to avoid collisions)
        const char* stage_var_name = "variant_stage_var";
        const char* stage_var_json = R"({"type":"Stage","name":"variant_stage_var"})";
        SDOM_Variant stage_var = SDOM_MakeNull();
        SDOM_ClearError();
        if (!SDOM_CreateDisplayObjectFromJson_V("Stage", stage_var_json, &stage_var)) {
            errors.push_back(std::string("Variant create failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
            // Clean up POD stage before exit
            SDOM_DestroyDisplayObject(&pod);
            return true;
        }

        if (!SDOM_Handle_IsDisplay(&stage_var)) {
            errors.push_back("Variant create did not return a display handle");
        }
        const uint64_t stage_var_id = SDOM_Handle_ObjectId(&stage_var);
        if (stage_var_id == 0) {
            errors.push_back("Variant display handle has zero object id");
        }

        // Get via variant API and compare IDs
        SDOM_Variant fetched_var = SDOM_MakeNull();
        if (!SDOM_GetDisplayObject_V(stage_var_name, &fetched_var)) {
            errors.push_back(std::string("GetDisplayObject_V failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
        } else {
            const uint64_t fetched_id = SDOM_Handle_ObjectId(&fetched_var);
            if (fetched_id != stage_var_id) {
                errors.push_back("Variant get returned different object id than create");
            }
        }

        // Get via POD API and compare IDs
        SDOM_DisplayHandle fetched_pod{};
        if (!SDOM_GetDisplayObject(stage_var_name, &fetched_pod)) {
            errors.push_back(std::string("GetDisplayObject (POD) failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
        } else if (fetched_pod.object_id != stage_var_id) {
            errors.push_back("POD get returned different object id than variant create");
        }

        // Destroy via variant path and ensure lookup fails
        SDOM_ClearError();
        if (!SDOM_DestroyDisplayObject_V(&fetched_var)) {
            errors.push_back(std::string("DestroyDisplayObject_V failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
        } else if (SDOM_HasDisplayObject(stage_var_name)) {
            errors.push_back("Display still present after variant destroy");
        }

        // Cleanup POD-created stage
        SDOM_DestroyDisplayObject(&pod);

        return true;
    }

    // Validates keyboard focus and mouse hover variant setters/getters parity.
    bool Core_Variant_FocusHover(std::vector<std::string>& errors)
    {
        SDOM_Variant prev_root = SDOM_MakeNull();
        const bool had_prev_root = SDOM_GetRootNode_V(&prev_root);

        std::string root_name;
        SDOM_DisplayHandle prev_root_pod{};
        if (SDOM_GetRootNode(&prev_root_pod) && prev_root_pod.name) {
            root_name = prev_root_pod.name;
        }

        bool created_stage = false;
        SDOM_Variant stage_var = SDOM_MakeNull();
        if (!had_prev_root || root_name.empty()) {
            const char* stage_json = R"({"type":"Stage","name":"variant_stage_focus"})";
            SDOM_ClearError();
            if (!SDOM_CreateDisplayObjectFromJson_V("Stage", stage_json, &stage_var)) {
                errors.push_back(std::string("Stage create failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
                return true;
            }

            // Set stage as root using variant
            if (!SDOM_SetRootNode_V(&stage_var)) {
                errors.push_back(std::string("SetRootNode_V failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
            }
            root_name = "variant_stage_focus";
            created_stage = true;
        }

        const std::string box_json = root_name.empty()
            ? std::string("{\"type\":\"Box\",\"name\":\"variant_focus_box\"}")
            : std::string("{\"type\":\"Box\",\"name\":\"variant_focus_box\",\"parent\":\"") + root_name + "\"}";
        SDOM_Variant box_var = SDOM_MakeNull();
        SDOM_ClearError();
        if (!SDOM_CreateDisplayObjectFromJson_V("Box", box_json.c_str(), &box_var)) {
            errors.push_back(std::string("Box create failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
            return true;
        }

        // Ensure any deferred attachments are applied so the box sits under the stage.
        SDOM_AttachFutureChildren();

        // Start with a known state.
        SDOM_ClearKeyboardFocus();
        SDOM_ClearMouseHover();

        const uint64_t box_id = SDOM_Handle_ObjectId(&box_var);
        if (box_id == 0) {
            errors.push_back("Box variant handle has zero object id");
        }

        // Set/get keyboard focus via variant
        SDOM_ClearError();
        if (!SDOM_SetKeyboardFocus_V(&box_var)) {
            errors.push_back(std::string("SetKeyboardFocus_V failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
        }

        SDOM_Variant focus_out = SDOM_MakeNull();
        if (!SDOM_GetKeyboardFocus_V(&focus_out)) {
            errors.push_back(std::string("GetKeyboardFocus_V failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
        } else {
            const uint64_t focus_id = SDOM_Handle_ObjectId(&focus_out);
            if (focus_id != box_id) {
                errors.push_back("Keyboard focus variant id does not match box id (focus_id=" + std::to_string(focus_id) + ", box_id=" + std::to_string(box_id) + ")");
            }
        }

        // Set/get mouse hover via variant
        SDOM_ClearError();
        if (!SDOM_SetMouseHover_V(&box_var)) {
            errors.push_back(std::string("SetMouseHover_V failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
        }

        SDOM_Variant hover_out = SDOM_MakeNull();
        if (!SDOM_GetMouseHover_V(&hover_out)) {
            errors.push_back(std::string("GetMouseHover_V failed: ") + (SDOM_GetError() ? SDOM_GetError() : "<no error>"));
        } else {
            const uint64_t hover_id = SDOM_Handle_ObjectId(&hover_out);
            if (hover_id != box_id) {
                errors.push_back("Mouse hover variant id does not match box id (hover_id=" + std::to_string(hover_id) + ", box_id=" + std::to_string(box_id) + ")");
            }
        }

        // Clean up created objects
        SDOM_ClearKeyboardFocus();
        SDOM_ClearMouseHover();

        // Clean up box; restore prior root if we replaced it.
        SDOM_DestroyDisplayObject_V(&box_var);
        if (had_prev_root && SDOM_Handle_IsDisplay(&prev_root) && SDOM_Handle_ObjectId(&prev_root) != 0) {
            SDOM_SetRootNode_V(&prev_root);
            if (created_stage) {
                SDOM_DestroyDisplayObject_V(&stage_var);
            }
        }

        return true;
    }





    // --- Lua Integration Tests --- //

    bool Core_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/Core_UnitTests.lua");
    } // END: Core_LUA_Tests()


    // --- Main Core UnitTests Runner --- //
    bool Core_UnitTests()
    {
        const std::string objName = "Core";
        UnitTests& ut = UnitTests::getInstance();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Scaffolding", Core_test0);
            // call more unit tests from here

            ut.add_test(objName, "CAPI: display variant create/get/destroy", Core_Variant_Display_CreateGetDestroy);
            ut.add_test(objName, "CAPI: variant focus/hover parity", Core_Variant_FocusHover);



            // Lua Integration Tests (Not Yet Implemented)
            ut.add_test(objName, "Lua: 'src/Core_UnitTests.lua'", Core_LUA_Tests);

            registered = true;
        }

        return true;
    }


} // namespace SDOM

// #endif // SDOM_ENABLE_UNIT_TESTS
