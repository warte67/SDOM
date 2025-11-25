// EventType_CAPI_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_SubjectBinding.hpp>

#include <algorithm>



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

    } // END: Event_CAPI_test0(std::vector<std::string>& errors)



    bool EventType_CAPI_test1([[maybe_unused]] std::vector<std::string>& errors)
    {

        auto types = SDOM::Core::getInstance().getDataRegistry().getMergedTypes();
        const SDOM::BindingManifest manifest = SDOM::buildBindingManifest(types);

        const auto kindIt = std::find_if(
            manifest.subject_kinds.begin(),
            manifest.subject_kinds.end(),
            [](const SDOM::SubjectKindDescriptor& kind) {
                return kind.name == "EventType";
            });

        if (kindIt == manifest.subject_kinds.end()) {
            errors.push_back("EventType subject kind descriptor missing");
        } else {
            if (kindIt->dispatch_family != SDOM::SubjectDispatchFamily::EventRouter) {
                errors.push_back("EventType subject kind not mapped to event_router");
            }
            if (kindIt->uses_handle) {
                errors.push_back("EventType subject kind should not require handles");
            }
        }

        const auto typeIt = std::find_if(
            manifest.type_bindings.begin(),
            manifest.type_bindings.end(),
            [](const SDOM::SubjectTypeDescriptor& desc) {
                return desc.name == "EventType";
            });

        if (typeIt == manifest.type_bindings.end()) {
            errors.push_back("EventType type descriptor missing");
        } else if (typeIt->dispatch_family != SDOM::SubjectDispatchFamily::EventRouter) {
            errors.push_back("EventType descriptor exists but is not event_router");
        }

        for (const auto& fn : manifest.functions) {
            if (fn.subject_kind != "EventType") {
                continue;
            }

            if (fn.dispatch_family != SDOM::SubjectDispatchFamily::EventRouter) {
                errors.push_back("EventType binding '" + fn.c_name + "' is not routed via event_router");
            }
        }

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
        UnitTests& ut = UnitTests::getInstance();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test Scaffold", EventType_CAPI_test0);
            ut.add_test(objName, "EventType Manifest", EventType_CAPI_test1);

            // ut.add_test(objName, "Lua: src/EventType_CAPI_UnitTests.lua", EventType_CAPI_LUA_Tests, false);

            registered = true;
        }

        // return ut.run_all(objName, "EventType_CAPI");
        return true;
    } // END: EventType_CAPI_UnitTests()

} // END: namespace SDOM
