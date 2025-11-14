// EventType_CAPI_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/CAPI/SDOM_CAPI_Events.h>
// // include runtime C API header (installed into include/SDOM by the build)
// #include <SDOM/SDOM_CAPI_Events_runtime.h>

namespace SDOM
{
    // --- Individual EventType_CAPI Unit Tests --- //

    // ============================================================================
    //  Test 0: EventType_CAPI Template
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
    bool EventType_CAPI_test0(std::vector<std::string>& errors)
    {
        // Scaffold test: serves as a template and simple smoke test for the
        // test module registration. Keep this intact for reference.
        // To fetch the current frame for multi-frame tests:
        // int current_frame = UnitTests::getInstance().get_frame_counter();

        return true; // ✅ finished this frame
    }


    // Test 1: C API register / lookup
    bool EventType_CAPI_test1(std::vector<std::string>& errors)
    {
        bool done = true;
        // const char* testName = "UnitTest_EventType_CAPI_TestEvent";

        // SDOM_EventTypeId id = SDOM_CAPI_register_event_type(testName);
        const char* testName = "UnitTest_EventType_CAPI_TestEvent";

        SDOM_EventTypeDesc desc{};
        desc.name = testName;
        desc.id = 0;
        desc.category = "UnitTest";
        desc.doc = "C API test event";

        SDOM_EventTypeHandle h = SDOM_CreateEventType(&desc);
        if (!h) {
            errors.push_back("SDOM_CreateEventType returned null handle");
            return true;
        }

        SDOM_EventTypeDesc out{};
        int rc = SDOM_GetEventTypeDesc(h, &out);
        if (rc != 0) {
            errors.push_back("SDOM_GetEventTypeDesc failed");
        } else {
            if (!out.name) errors.push_back("Returned name is null");
            else if (std::string(out.name) != testName) errors.push_back("Name mismatch from C API");
            if (out.id == 0) errors.push_back("Returned id is zero");
            if (!out.category) errors.push_back("Returned category is null");
        }

        // Find by name
        SDOM_EventTypeHandle fh = SDOM_FindEventTypeByName(testName);
        if (!fh) errors.push_back("SDOM_FindEventTypeByName returned null");

        // Create an event for this type
        SDOM_EventDesc ed{};
        ed.type_id = out.id;
        SDOM_EventHandle evh = SDOM_CreateEvent(&ed);
        if (!evh) {
            errors.push_back("SDOM_CreateEvent returned null");
        } else {
            SDOM_EventDesc outEv{};
            if (SDOM_GetEventDesc(evh, &outEv) != 0) {
                errors.push_back("SDOM_GetEventDesc failed");
            } else {
                if (outEv.type_id != out.id) errors.push_back("Event type_id mismatch");
            }
            if (SDOM_SendEvent(evh) != 0) errors.push_back("SDOM_SendEvent failed");
            SDOM_DestroyEvent(evh);
        }

        // Clean up handles
        if (fh) SDOM_DestroyEventType(fh);
        SDOM_DestroyEventType(h);

        //     errors.push_back("SDOM_CAPI_register_event_type returned 0");
        //     ok = false;
        // }

        // SDOM_EventTypeId id2 = SDOM_CAPI_event_type_id_for_name(testName);
        // if (id2 == 0) {
        //     errors.push_back("SDOM_CAPI_event_type_id_for_name returned 0");
        //     ok = false;
        // } else if (id != id2) {
        //     errors.push_back("Mismatched ids from register vs lookup");
        //     ok = false;
        // }

        // const char* name = SDOM_CAPI_event_type_name_for_id(id);
        // if (!name) {
        //     errors.push_back("SDOM_CAPI_event_type_name_for_id returned nullptr");
        //     ok = false;
        // } else if (std::string(name) != testName) {
        //     errors.push_back(std::string("Name mismatch: expected ") + testName + ", got " + name);
        //     ok = false;
        // }

        return done;
    }




    // --- Lua Integration Tests --- //

    bool EventType_CAPI_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/EventType_CAPI_UnitTests.lua");
    } // END: EventType_CAPI_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool EventType_CAPI_UnitTests()
    {
        const std::string objName = "EventType_CAPI";
        UnitTests& ut = UnitTests::getInstance();
        // ut.clear_tests();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test Scaffold", EventType_CAPI_test0);
            ut.add_test(objName, "C API register/lookup", EventType_CAPI_test1);

            // ut.add_test(objName, "Lua: src/EventType_CAPI_UnitTests.lua", EventType_CAPI_LUA_Tests, false);

            registered = true;
        }

        // return ut.run_all(objName, "EventType_CAPI");
        return true;
    } // END: EventType_CAPI_UnitTests()



} // END: namespace SDOM
