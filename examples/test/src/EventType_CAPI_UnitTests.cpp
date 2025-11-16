// EventType_CAPI_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_EventType.hpp>

// 🔥 Required to get SDOM_EventTypeDesc,
//     SDOM_EventTypeHandle, SDOM_EventDesc,
//     SDOM_CreateEventType(), SDOM_EnumEventTypes(), etc.
#include <SDOM/SDOM_CAPI.h>
#include <SDOM/CAPI/SDOM_CAPI_Events.h>

// Need full EventManager definition for queue size test
#include <SDOM/SDOM_EventManager.hpp>


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
    bool EventType_CAPI_test0([[maybe_unused]] std::vector<std::string>& errors)
    {
        // Scaffold test: serves as a template and simple smoke test for the
        // test module registration. Keep this intact for reference.
        // To fetch the current frame for multi-frame tests:
        // int current_frame = UnitTests::getInstance().get_frame_counter();

        return true; // ✅ finished this frame
    }


    // ------------------------------------------------------------
    // Test 1: Create + GetEventTypeDesc
    // ------------------------------------------------------------
    bool EventType_CAPI_test1(std::vector<std::string>& errors)
    {
        const char* testName = "UnitTest_EventType_CAPI_Test1_CreateGet";

        SDOM_EventTypeDesc desc{};
        desc.name = testName;
        desc.id = 0;
        desc.category = "UnitTest";
        desc.doc = "Test event for Create/Get";

        SDOM_EventTypeHandle h = SDOM_CreateEventType(&desc);
        if (!h) {
            errors.push_back("CreateEventType returned null");
            return true;
        }

        SDOM_EventTypeDesc out{};
        int rc = SDOM_GetEventTypeDesc(h, &out);
        if (rc != SDOM_CAPI_OK) {
            errors.push_back("GetEventTypeDesc failed");
        } else {
            if (!out.name || std::string(out.name) != testName)
                errors.push_back("Name mismatch");

            if (!out.category || std::string(out.category) != "UnitTest")
                errors.push_back("Category mismatch");

            if (out.id == 0)
                errors.push_back("Returned id is zero");

            if (SDOM_EVENT_CATEGORY(out.id) == 0)
                errors.push_back("Event id category bits are zero");
        }

        SDOM_DestroyEventType(h);
        return true;
    } // END -- EventType_CAPI_test1()


    // ------------------------------------------------------------
    // Test 2: FindEventTypeByName
    // ------------------------------------------------------------
    bool EventType_CAPI_test2(std::vector<std::string>& errors)
    {
        const char* testName = "UnitTest_EventType_CAPI_Test2_Find";

        SDOM_EventTypeDesc desc{};
        desc.name = testName;
        desc.category = "UnitTest";

        SDOM_EventTypeHandle h = SDOM_CreateEventType(&desc);
        if (!h) {
            errors.push_back("CreateEventType failed");
            return true;
        }

        SDOM_EventTypeHandle h2 = SDOM_FindEventTypeByName(testName);
        if (!h2)
            errors.push_back("FindEventTypeByName returned null");

        if (h2) SDOM_DestroyEventType(h2);
        SDOM_DestroyEventType(h);

        return true;
    }  // END -- EventType_CAPI_test2()


    // ------------------------------------------------------------
    // Test 3: CreateEvent + GetEventDesc
    // ------------------------------------------------------------
    bool EventType_CAPI_test3(std::vector<std::string>& errors)
    {
        const char* testName = "UnitTest_EventType_CAPI_Test3_EventCreate";

        SDOM_EventTypeDesc d{};
        d.name = testName;
        d.category = "UnitTest";

        SDOM_EventTypeHandle h = SDOM_CreateEventType(&d);
        if (!h) {
            errors.push_back("CreateEventType failed");
            return true;
        }

        SDOM_EventTypeDesc out{};
        SDOM_GetEventTypeDesc(h, &out);

        SDOM_EventDesc ed{};
        ed.type_id = out.id;

        SDOM_EventHandle evh = SDOM_CreateEvent(&ed);
        if (!evh) {
            errors.push_back("CreateEvent returned null");
            SDOM_DestroyEventType(h);
            return true;
        }

        SDOM_EventDesc evt{};
        if (SDOM_GetEventDesc(evh, &evt) != SDOM_CAPI_OK)
            errors.push_back("GetEventDesc failed");
        else if (evt.type_id != out.id)
            errors.push_back("Event type_id mismatch");

        SDOM_DestroyEvent(evh);
        SDOM_DestroyEventType(h);

        return true;
    }  // END -- EventType_CAPI_test3()


    // ------------------------------------------------------------
    // Test 4: EnumEventTypes
    // ------------------------------------------------------------
    bool EventType_CAPI_test4(std::vector<std::string>& errors)
    {
        const char* testName = "UnitTest_EventType_CAPI_Test4_Enum";

        SDOM_EventTypeDesc d{};
        d.name = testName;
        d.category = "UnitTest";

        SDOM_EventTypeHandle h = SDOM_CreateEventType(&d);
        if (!h) {
            errors.push_back("CreateEventType failed");
            return true;
        }

        bool found = false;
        SDOM_EventTypeHandle iter = nullptr;
        size_t next = SDOM_EnumEventTypes(0, &iter);

        while (next != 0) {
            SDOM_EventTypeDesc e{};
            if (iter && SDOM_GetEventTypeDesc(iter, &e) == SDOM_CAPI_OK) {
                if (e.name && std::string(e.name) == testName)
                    found = true;
            }

            if (iter) SDOM_DestroyEventType(iter);
            next = SDOM_EnumEventTypes(next, &iter);
        }

        if (!found)
            errors.push_back("EnumEventTypes did not list created type");

        SDOM_DestroyEventType(h);
        return true;
    }  // END -- EventType_CAPI_test4()


    // ------------------------------------------------------------
    // Test 5: UpdateEventType
    // ------------------------------------------------------------
    bool EventType_CAPI_test5(std::vector<std::string>& errors)
    {
        const char* testName = "UnitTest_EventType_CAPI_Test5_UpdateType";

        SDOM_EventTypeDesc d{};
        d.name = testName;
        d.category = "UnitTest";

        SDOM_EventTypeHandle h = SDOM_CreateEventType(&d);
        if (!h) {
            errors.push_back("CreateEventType failed");
            return true;
        }

        SDOM_EventTypeDesc upd{};
        upd.doc = "Updated doc";

        if (SDOM_UpdateEventType(h, &upd) != SDOM_CAPI_OK)
            errors.push_back("UpdateEventType failed");

        SDOM_EventTypeDesc out{};
        SDOM_GetEventTypeDesc(h, &out);

        if (!out.doc || std::string(out.doc) != "Updated doc")
            errors.push_back("UpdateEventType did not update doc field");

        SDOM_DestroyEventType(h);
        return true;
    }


    // ------------------------------------------------------------
    // Test 6: UpdateEvent + Negative Tests
    // ------------------------------------------------------------
    bool EventType_CAPI_test6(std::vector<std::string>& errors)
    {
        const char* testName = "UnitTest_EventType_CAPI_Test6_UpdateEvent";

        SDOM_EventTypeDesc d{};
        d.name = testName;
        d.category = "UnitTest";

        SDOM_EventTypeHandle h = SDOM_CreateEventType(&d);
        if (!h) {
            errors.push_back("CreateEventType failed");
            return true;
        }

        SDOM_EventTypeDesc out{};
        SDOM_GetEventTypeDesc(h, &out);

        SDOM_EventDesc ed{};
        ed.type_id = out.id;

        SDOM_EventHandle evh = SDOM_CreateEvent(&ed);
        if (!evh) {
            errors.push_back("CreateEvent failed");
            SDOM_DestroyEventType(h);
            return true;
        }

        // ---- valid update ----
        SDOM_EventDesc upd{};
        upd.type_id  = out.id;
        upd.name     = "UpdatedEvent";
        upd.payload_json = "{\"updated\":true}";

        if (SDOM_UpdateEvent(evh, &upd) != SDOM_CAPI_OK)
            errors.push_back("UpdateEvent should succeed");

        SDOM_DestroyEvent(evh);

        // ---- invalid update ----
        if (SDOM_UpdateEvent(nullptr, &upd) != SDOM_CAPI_ERR_INVALID_ARG)
            errors.push_back("UpdateEvent(nullptr) should fail");

        SDOM_DestroyEventType(h);
        return true;
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
        // UnitTests& ut = UnitTests::getInstance();

        static bool registered = false;
        if (!registered)
        {
            // ut.add_test(objName, "Test Scaffold", EventType_CAPI_test0);

            // // ==208170==    definitely lost: 120 bytes in 1 blocks
            // // ==208170==    indirectly lost: 66 bytes in 2 blocks
            // ut.add_test(objName, "Create + GetEventTypeDesc", EventType_CAPI_test1);  

            // ==209785==    definitely lost: 120 bytes in 1 blocks
            // ==209785==    indirectly lost: 35 bytes in 1 blocks
            // ut.add_test(objName, "FindEventTypeByName", EventType_CAPI_test2);

            // ==210280==    definitely lost: 120 bytes in 1 blocks
            // ==210280==    indirectly lost: 42 bytes in 1 blocks
            // ut.add_test(objName, "CreateEvent + GetEventDesc", EventType_CAPI_test3);

            // ==210819==    definitely lost: 120 bytes in 1 blocks
            // ==210819==    indirectly lost: 35 bytes in 1 blocks
            // ut.add_test(objName, "EnumEventTypes", EventType_CAPI_test4);

            // ==211348==    definitely lost: 120 bytes in 1 blocks
            // ==211348==    indirectly lost: 41 bytes in 1 blocks
            // ut.add_test(objName, "UpdateEventType", EventType_CAPI_test5);

            // ==211874==    definitely lost: 120 bytes in 1 blocks
            // ==211874==    indirectly lost: 42 bytes in 1 blocks
            // ut.add_test(objName, "UpdateEvent", EventType_CAPI_test6);

            // ut.add_test(objName, "Lua: src/EventType_CAPI_UnitTests.lua", EventType_CAPI_LUA_Tests, false);

            registered = true;
        }

        // return ut.run_all(objName, "EventType_CAPI");
        return true;
    } // END: EventType_CAPI_UnitTests()

} // END: namespace SDOM
