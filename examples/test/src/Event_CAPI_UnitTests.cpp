// Event_CAPI_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_SubjectBinding.hpp>

#include <algorithm>

namespace SDOM
{
    // --- Individual Event_CAPI Unit Tests --- //

    // ============================================================================
    //  Test 0: Event_CAPI Template
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
    bool Event_CAPI_test0([[maybe_unused]] std::vector<std::string>& errors)
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


    // 
    bool Event_CAPI_test1([[maybe_unused]] std::vector<std::string>& errors)
    {

        auto types = SDOM::Core::getInstance().getDataRegistry().getMergedTypes();
        const SDOM::BindingManifest manifest = SDOM::buildBindingManifest(types);

        const auto kindIt = std::find_if(
            manifest.subject_kinds.begin(),
            manifest.subject_kinds.end(),
            [](const SDOM::SubjectKindDescriptor& kind) {
                return kind.name == "Event";
            });

        if (kindIt == manifest.subject_kinds.end()) {
            errors.push_back("Event subject kind descriptor missing from manifest");
        } else {
            if (kindIt->dispatch_family != SDOM::SubjectDispatchFamily::EventRouter) {
                errors.push_back("Event subject kind is not mapped to event_router dispatch family");
            }
            if (kindIt->uses_handle) {
                errors.push_back("Event subject kind should not require SDOM_Handle instances");
            }
        }

        const auto typeIt = std::find_if(
            manifest.type_bindings.begin(),
            manifest.type_bindings.end(),
            [](const SDOM::SubjectTypeDescriptor& desc) {
                return desc.name == "Event";
            });

        if (typeIt == manifest.type_bindings.end()) {
            errors.push_back("Event type descriptor missing from manifest");
        } else {
            if (typeIt->dispatch_family != SDOM::SubjectDispatchFamily::EventRouter) {
                errors.push_back("Event type descriptor does not use event_router dispatch");
            }
            if (!typeIt->has_function_exports) {
                errors.push_back("Event type descriptor should report exported functions");
            }
        }

        static const char* kExpectedFns[] = {
            "SDOM_GetEventType",
            "SDOM_GetEventTypeName",
            "SDOM_GetEventPhase",
            "SDOM_SetEventPhase",
            "SDOM_GetEventPhaseString"
        };

        for (const char* fnName : kExpectedFns) {
            const auto fnIt = std::find_if(
                manifest.functions.begin(),
                manifest.functions.end(),
                [&](const SDOM::FunctionBindingDescriptor& fn) {
                    return fn.c_name == fnName;
                });

            if (fnIt == manifest.functions.end()) {
                errors.push_back(std::string("Missing Event binding: ") + fnName);
                continue;
            }

            if (fnIt->dispatch_family != SDOM::SubjectDispatchFamily::EventRouter) {
                errors.push_back(std::string("Event binding ") + fnName + " is not routed via event_router");
            }

            if (fnIt->subject_kind != "Event") {
                errors.push_back(std::string("Event binding ") + fnName + " has incorrect subject kind");
            }
        }

        return true;

    } // END: Event_CAPI_test1(std::vector<std::string>& errors)




    // --- Lua Integration Tests --- //

    bool Event_CAPI_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/Event_CAPI_UnitTests.lua");
    } // END: Event_CAPI_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool Event_CAPI_UnitTests()
    {
        const std::string objName = "Event_CAPI";
        UnitTests& ut = UnitTests::getInstance();
        // ut.clear_tests();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test Scaffold", Event_CAPI_test0);
            ut.add_test(objName, "Event Manifest", Event_CAPI_test1);

            // ut.add_test(objName, "Lua: src/Event_CAPI_UnitTests.lua", Event_CAPI_LUA_Tests, false);

            registered = true;
        }

        // return ut.run_all(objName, "Event_CAPI");
        return true;
    } // END: Event_CAPI_UnitTests()



} // END: namespace SDOM
