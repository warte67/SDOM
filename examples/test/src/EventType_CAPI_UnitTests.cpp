// EventType_CAPI_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/CAPI/SDOM_CAPI_Events.h>
// Need full EventManager definition for testing queue size
#include <SDOM/SDOM_EventManager.hpp>
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
            else if (std::string(out.category) != "UnitTest") errors.push_back("Category mismatch");

            // Basic sanity for numeric id: ensure category bits are non-zero and index fits in 8 bits
            uint32_t cat = SDOM_EVENT_CATEGORY(out.id);
            uint32_t idx = SDOM_EVENT_INDEX(out.id);
            if (cat == 0) errors.push_back("EventType id category bits are zero");
            if (idx >= 0x100) errors.push_back("EventType index out of range");
        }

        // Find by name
        SDOM_EventTypeHandle fh = SDOM_FindEventTypeByName(testName);
        if (!fh) errors.push_back("SDOM_FindEventTypeByName returned null");

        // Create an event for this type
        // Cache id before freeing strings below
        SDOM_EventTypeId created_id = out.id;

        SDOM_EventDesc ed{};
        ed.type_id = created_id;
        SDOM_EventHandle evh = SDOM_CreateEvent(&ed);
        if (!evh) {
            errors.push_back("SDOM_CreateEvent returned null");
        } else {
            SDOM_EventDesc outEv{};
            if (SDOM_GetEventDesc(evh, &outEv) != 0) {
                errors.push_back("SDOM_GetEventDesc failed");
            } else {
                if (outEv.type_id != out.id) errors.push_back("Event type_id mismatch");
                // Strings returned by SDOM_GetEventDesc are owned by the event handle
                // and will be freed when the handle is destroyed. Do not free them here.
            }
            if (SDOM_SendEvent(evh) != 0) errors.push_back("SDOM_SendEvent failed");
            SDOM_DestroyEvent(evh);
        }

        // Verify enumeration yields the created type and matches FindByName
        SDOM_EventTypeHandle enumH = nullptr;
        size_t next = SDOM_EnumEventTypes(0, &enumH);
        bool found = false;
        while (next != 0) {
            SDOM_EventTypeDesc e{};
            if (enumH && SDOM_GetEventTypeDesc(enumH, &e) == 0) {
                if (e.name && std::string(e.name) == testName) found = true;
                // The strings returned for enumerated handles are owned by the handle
                // and will be freed by SDOM_DestroyEventType(enumH). Do not free here.
            }
            if (enumH) SDOM_DestroyEventType(enumH);
            enumH = nullptr;
            next = SDOM_EnumEventTypes(next, &enumH);
        }
        if (!found) errors.push_back("SDOM_EnumEventTypes failed to enumerate the created type");

        // Test UpdateEventType modifies both the wrapper-visible fields and the underlying C++ object
        SDOM_EventTypeDesc updatedDesc{};
        updatedDesc.doc = "Updated doc";
        if (SDOM_UpdateEventType(h, &updatedDesc) != SDOM_CAPI_OK) {
            errors.push_back("SDOM_UpdateEventType failed");
        } else {
            SDOM_EventTypeDesc out2{};
            if (SDOM_GetEventTypeDesc(h, &out2) != SDOM_CAPI_OK) {
                errors.push_back("SDOM_GetEventTypeDesc failed after update");
            } else {
                if (!out2.doc || std::string(out2.doc) != std::string(updatedDesc.doc))
                    errors.push_back("UpdateEventType did not modify doc");
                // out2 pointers refer to memory owned by the handle `h`; do not free here.
            }

            // Validate underlying C++ EventType was updated
            SDOM::EventType* impl = SDOM::EventType::fromName(std::string(testName));
            if (!impl) errors.push_back("Underlying EventType not found via C++ API");
            else if (impl->getDoc() != std::string(updatedDesc.doc)) errors.push_back("Underlying EventType object was not updated");
        }

        // Verify event queue increased when sending an event
        int before = SDOM_GetEventQueueSize();
        // create/send a fresh event and observe queue size
        SDOM_EventDesc ed2{}; ed2.type_id = created_id;
        SDOM_EventHandle evh2 = SDOM_CreateEvent(&ed2);
        if (evh2) {
            SDOM_SendEvent(evh2);
            int after = SDOM_GetEventQueueSize();
            if (after != before + 1) errors.push_back("SDOM_SendEvent did not increase event queue size");
            SDOM_DestroyEvent(evh2);
            evh2 = nullptr; // avoid use-after-free: tests must not dereference freed handles
        }

        // Negative tests for UpdateEventType with null args
        if (SDOM_UpdateEventType(nullptr, &updatedDesc) != SDOM_CAPI_ERR_INVALID_ARG)
            errors.push_back("SDOM_UpdateEventType should fail on null handle");
        if (SDOM_UpdateEventType(h, nullptr) != SDOM_CAPI_ERR_INVALID_ARG)
            errors.push_back("SDOM_UpdateEventType should fail on null desc");

        // Negative test for GetEventTypeDesc with null handle
        SDOM_EventTypeDesc bad{};
        if (SDOM_GetEventTypeDesc(nullptr, &bad) != SDOM_CAPI_ERR_INVALID_ARG)
            errors.push_back("SDOM_GetEventTypeDesc(NULL,…) should return INVALID_ARG");

        // Test UpdateEvent (no-op stub) on a live handle
        // Populate a realistic update descriptor so future UpdateEvent
        // implementations receive concrete data to apply.
        SDOM_EventDesc updEv{}; updEv.type_id = created_id;
        updEv.name = "UnitTest_UpdatedEvent";
        updEv.payload_json = "{\"updated\":true}";
        SDOM_EventHandle evh3 = SDOM_CreateEvent(&ed2);
        if (!evh3) {
            errors.push_back("SDOM_CreateEvent for UpdateEvent test returned null");
        } else {
            if (SDOM_UpdateEvent(evh3, &updEv) != SDOM_CAPI_OK)
                errors.push_back("SDOM_UpdateEvent should succeed (no-op stub)");
            SDOM_DestroyEvent(evh3);
        }

        // UpdateEvent on previously destroyed handle (evh2) should fail when passed a null/invalid handle
        if (SDOM_UpdateEvent(evh2, &updEv) != SDOM_CAPI_ERR_INVALID_ARG)
            errors.push_back("UpdateEvent should fail on destroyed (null) handle");

        // Enum with null out pointer should still advance
        size_t n2 = SDOM_EnumEventTypes(0, nullptr);
        if (n2 == 0) errors.push_back("Enum with null out should still return next index");

        // Clean up handles. Strings returned by SDOM_GetEventTypeDesc are
        // owned by the handle and will be freed by `SDOM_DestroyEventType`.
        if (fh) {
            SDOM_DestroyEventType(fh);
            fh = nullptr; // avoid use-after-free
        }
        SDOM_DestroyEventType(h);
        h = nullptr; // avoid use-after-free

        // Confirm behavior after destruction: operations should fail gracefully
        SDOM_EventTypeDesc invalidCheck{};
        int rc_after = SDOM_GetEventTypeDesc(h, &invalidCheck);
        if (rc_after == SDOM_CAPI_OK) errors.push_back("Destroyed handle should not succeed on GetEventTypeDesc (null handle)");

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
