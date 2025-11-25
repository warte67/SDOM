// Event_CAPI_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_SDL_Utils.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_SubjectBinding.hpp>

#include <json.hpp>

#include <algorithm>
#include <cstring>

namespace SDOM
{
    namespace
    {
        SDL_Event makeKeyboardEvent()
        {
            SDL_Event evt{};
            evt.type = SDL_EVENT_KEY_DOWN;
            evt.key.timestamp = 0x12345678ABCDEFuLL;
            evt.key.windowID = 0x42u;
            evt.key.which = 0x77u;
            evt.key.scancode = SDL_SCANCODE_F5;
            evt.key.key = SDLK_F5;
            evt.key.mod = static_cast<SDL_Keymod>(SDL_KMOD_CTRL | SDL_KMOD_SHIFT);
            evt.key.raw = 0x33u;
            evt.key.down = true;
            evt.key.repeat = false;
            return evt;
        }

        bool eventsEqual(const SDL_Event& lhs, const SDL_Event& rhs)
        {
            return std::memcmp(&lhs, &rhs, sizeof(SDL_Event)) == 0;
        }
    } // namespace
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


    bool Event_CAPI_test2(std::vector<std::string>& errors)
    {
        const SDL_Event sample = makeKeyboardEvent();

        const std::string utilsJson = SDOM::SDL_Utils::eventToJsonString(sample);
        if (utilsJson.empty()) {
            errors.push_back("SDL_Utils::eventToJsonString returned an empty payload");
        }

        SDL_Event utilsRoundtrip{};
        if (!SDOM::SDL_Utils::eventFromJsonString(utilsJson, utilsRoundtrip)) {
            errors.push_back("SDL_Utils::eventFromJsonString failed to decode its own payload");
        } else if (!eventsEqual(sample, utilsRoundtrip)) {
            errors.push_back("SDL_Utils JSON round-trip did not preserve SDL_Event bytes");
        }

        const auto parsed = nlohmann::json::parse(utilsJson, nullptr, false);
        if (parsed.is_discarded()) {
            errors.push_back("SDL_Event JSON payload was not valid JSON");
        } else {
            const std::string typeLabel = parsed.value("type", std::string());
            if (typeLabel != "SDL_EVENT_KEY_DOWN") {
                errors.push_back("SDL_Event JSON type tag did not match expected SDL_EVENT_KEY_DOWN");
            }

            if (!parsed.contains("bytes") || !parsed["bytes"].is_array()) {
                errors.push_back("SDL_Event JSON payload missing 'bytes' array");
            }

            if (parsed.value("size", 0u) != sizeof(SDL_Event)) {
                errors.push_back("SDL_Event JSON payload recorded incorrect struct size");
            }
        }

        Event evt; // default Event instance is sufficient for SDL snapshot testing
        evt.setSDL_Event(sample);

        const std::string eventJson = evt.getSDL_EventJson();
        if (eventJson.empty()) {
            errors.push_back("Event::getSDL_EventJson returned an empty payload");
        }

        SDL_Event mutated = sample;
        mutated.key.key = SDLK_F1;
        mutated.key.scancode = SDL_SCANCODE_F1;
        mutated.key.mod = SDL_KMOD_ALT;
        mutated.key.down = false;
        mutated.key.repeat = true;
        evt.setSDL_Event(mutated);

        evt.setSDL_EventJson(utilsJson);
        const SDL_Event restored = evt.getSDL_Event();
        if (!eventsEqual(sample, restored)) {
            errors.push_back("Event::setSDL_EventJson failed to restore the original SDL_Event snapshot");
        }

        return true;
    }




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
            ut.add_test(objName, "SDL_Event JSON Roundtrip", Event_CAPI_test2);

            // ut.add_test(objName, "Lua: src/Event_CAPI_UnitTests.lua", Event_CAPI_LUA_Tests, false);

            registered = true;
        }

        // return ut.run_all(objName, "Event_CAPI");
        return true;
    } // END: Event_CAPI_UnitTests()



} // END: namespace SDOM
