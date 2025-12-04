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
#include <SDOM/CAPI/SDOM_CAPI_Version.h>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDL3/SDL.h>

#include <string>
#include <vector>
#include <functional>
#include <cmath>

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
        bool done = true;  // true for test complete, false for re-entrant test

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        // To fetch the current frame:
        // int current_frame = UnitTests::getInstance().get_frame_counter();

        // Use State Machines when re-entrant tests are required to test SDOM main loop

        // return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test
        return done;

    } // END: Core_scaffolding(std::vector<std::string>& errors)




    // ============================================================================
    //  Test 1: Core Version Metadata (CAPI getters)
    // ----------------------------------------------------------------------------
    //  Verifies that the generated Core C API exposes valid version/build strings
    //  and non-negative semantic version numbers via the getter family.
    //  Ensures each function returns data and that the string-based getters are
    //  non-empty so build information is visible to diagnostics.
    // ============================================================================
    bool Core_test1(std::vector<std::string>& errors)
    {
        bool done = true;

        const char* verStr = SDOM_GetVersionString();
        if (!verStr || std::string(verStr).empty())
        {
            errors.push_back("Core_test1: SDOM_GetVersionString returned null/empty");
        }

        const char* fullStr = SDOM_GetVersionFullString();
        if (!fullStr || std::string(fullStr).empty())
        {
            errors.push_back("Core_test1: SDOM_GetVersionFullString returned null/empty");
        }

        const int major = SDOM_GetVersionMajor();
        const int minor = SDOM_GetVersionMinor();
        const int patch = SDOM_GetVersionPatch();
        if (major < 0 || minor < 0 || patch < 0)
        {
            errors.push_back("Core_test1: version numbers must be non-negative");
        }

        const char* buildHash = SDOM_GetVersionCommit();
        if (!buildHash || std::string(buildHash).empty())
        {
            errors.push_back("Core_test1: SDOM_GetVersionCommit returned null/empty");
        }

        const char* buildDate = SDOM_GetVersionBuildDate();
        if (!buildDate || std::string(buildDate).empty())
        {
            errors.push_back("Core_test1: SDOM_GetVersionBuildDate returned null/empty");
        }
        return done;
    } // END: Core_test1()



    // ============================================================================
    //  Test 2: Window and Pixel Dimensions (CAPI get/set)
    // ----------------------------------------------------------------------------
    //  Verifies the Core dimension getters return positive values and that the
    //  paired setters propagate changes before restoring the prior configuration.
    // ============================================================================
    bool Core_test2(std::vector<std::string>& errors)
    {
        bool done = true;
        constexpr float epsilon = 0.05f;

        const float originalWindowWidth  = SDOM_GetWindowWidth();
        const float originalWindowHeight = SDOM_GetWindowHeight();
        const float originalPixelWidth   = SDOM_GetPixelWidth();
        const float originalPixelHeight  = SDOM_GetPixelHeight();

        auto restoreDimensions = [&]()
        {
            SDOM_SetWindowWidth(originalWindowWidth);
            SDOM_SetWindowHeight(originalWindowHeight);
            SDOM_SetPixelWidth(originalPixelWidth);
            SDOM_SetPixelHeight(originalPixelHeight);
        };

        if (originalWindowWidth <= 0.0f || originalWindowHeight <= 0.0f)
        {
            errors.push_back("Core_test2: initial window size must be positive");
            errors.push_back(SDOM_GetError());
        }
        if (originalPixelWidth <= 0.0f || originalPixelHeight <= 0.0f)
        {
            errors.push_back("Core_test2: initial pixel size must be positive");
            errors.push_back(SDOM_GetError());
        }

        const float testWindowWidth  = (originalWindowWidth  > 0.0f ? originalWindowWidth  + 13.5f : 640.0f);
        const float testWindowHeight = (originalWindowHeight > 0.0f ? originalWindowHeight + 19.5f : 480.0f);
        const float testPixelWidth   = (originalPixelWidth   > 0.0f ? originalPixelWidth   + 1.0f  : 2.0f);
        const float testPixelHeight  = (originalPixelHeight  > 0.0f ? originalPixelHeight  + 1.0f  : 2.0f);

        if (!SDOM_SetWindowWidth(testWindowWidth))
        {
            errors.push_back("Core_test2: SDOM_SetWindowWidth failed -- ");
            errors.push_back(SDOM_GetError());
        }
        if (!SDOM_SetWindowHeight(testWindowHeight))
        {
            errors.push_back("Core_test2: SDOM_SetWindowHeight failed");
            errors.push_back(SDOM_GetError());
        }
        if (!SDOM_SetPixelWidth(testPixelWidth))
        {
            errors.push_back("Core_test2: SDOM_SetPixelWidth failed");
            errors.push_back(SDOM_GetError());
        }
        if (!SDOM_SetPixelHeight(testPixelHeight))
        {
            errors.push_back("Core_test2: SDOM_SetPixelHeight failed");
            errors.push_back(SDOM_GetError());
        }

        const float appliedWindowWidth  = SDOM_GetWindowWidth();
        const float appliedWindowHeight = SDOM_GetWindowHeight();
        const float appliedPixelWidth   = SDOM_GetPixelWidth();
        const float appliedPixelHeight  = SDOM_GetPixelHeight();

        if (std::fabs(appliedWindowWidth - testWindowWidth) > epsilon)
        {
            errors.push_back("Core_test2: window width getter mismatch");
            errors.push_back(SDOM_GetError());
        }
        if (std::fabs(appliedWindowHeight - testWindowHeight) > epsilon)
        {
            errors.push_back("Core_test2: window height getter mismatch");
            errors.push_back(SDOM_GetError());
        }
        if (std::fabs(appliedPixelWidth - testPixelWidth) > epsilon)
        {
            errors.push_back("Core_test2: pixel width getter mismatch");
            errors.push_back(SDOM_GetError());
        }
        if (std::fabs(appliedPixelHeight - testPixelHeight) > epsilon)
        {
            errors.push_back("Core_test2: pixel height getter mismatch");
            errors.push_back(SDOM_GetError());
        }

        restoreDimensions();
        return done;
    } // END: Core_test2()



    // ============================================================================
    //  Test 3: Window Title and Border Color (CAPI get/set)
    // ----------------------------------------------------------------------------
    //  Verifies string-based setters (window title) and struct-based setters
    //  (border color), ensuring values round-trip through the Core C API and
    //  original state is restored afterward.
    // ============================================================================
    bool Core_test3(std::vector<std::string>& errors)
    {
        bool done = true;

        // --- Window title ---
        const char* titlePtr = SDOM_GetWindowTitle();
        std::string originalTitle = titlePtr ? std::string(titlePtr) : std::string();
        if (!titlePtr)
        {
            errors.push_back("Core_test3: SDOM_GetWindowTitle returned null");
        }
        else
        {
            std::string testTitle = originalTitle.empty()
                ? std::string("Core_test3 Window Title")
                : originalTitle + " [Core_test3]";

            if (!SDOM_SetWindowTitle(testTitle.c_str()))
            {
                errors.push_back("Core_test3: SDOM_SetWindowTitle failed");
                errors.push_back(SDOM_GetError());
            }
            else
            {
                const char* appliedTitle = SDOM_GetWindowTitle();
                if (!appliedTitle || testTitle != appliedTitle)
                {
                    errors.push_back("Core_test3: window title getter mismatch");
                }
            }

            // Restore original title (or empty string if none)
            const char* restoreTitle = originalTitle.c_str();
            if (!SDOM_SetWindowTitle(restoreTitle))
            {
                errors.push_back("Core_test3: failed to restore original window title");
                errors.push_back(SDOM_GetError());
            }
        }

        // --- Border color ---
        SDL_Color originalColor{};
        if (!SDOM_GetBorderColor(&originalColor))
        {
            errors.push_back("Core_test3: SDOM_GetBorderColor failed");
            errors.push_back(SDOM_GetError());
        }
        else
        {
            SDL_Color testColor = originalColor;
            testColor.r = static_cast<Uint8>(originalColor.r ^ 0xFF);
            testColor.g = static_cast<Uint8>((originalColor.g + 127) % 256);
            testColor.b = static_cast<Uint8>((originalColor.b + 53) % 256);
            testColor.a = 255;

            if (!SDOM_SetBorderColor(&testColor))
            {
                errors.push_back("Core_test3: SDOM_SetBorderColor failed");
                errors.push_back(SDOM_GetError());
            }
            else
            {
                SDL_Color applied{};
                if (!SDOM_GetBorderColor(&applied))
                {
                    errors.push_back("Core_test3: SDOM_GetBorderColor (post-set) failed");
                    errors.push_back(SDOM_GetError());
                }
                else if (applied.r != testColor.r || applied.g != testColor.g ||
                         applied.b != testColor.b || applied.a != testColor.a)
                {
                    errors.push_back("Core_test3: border color getter mismatch");
                }
            }

            if (!SDOM_SetBorderColor(&originalColor))
            {
                errors.push_back("Core_test3: failed to restore original border color");
                errors.push_back(SDOM_GetError());
            }
        }

        return done;
    } // END: Core_test3()



    // ============================================================================
    //  Test 4: Batch Core config toggles (CAPI get/set)
    // ----------------------------------------------------------------------------
    //  Exercises multiple getter/setter pairs (bool flags and enum-like values)
    //  to ensure state changes apply via the C API and can be safely restored.
    // ============================================================================
    bool Core_test4(std::vector<std::string>& errors)
    {
        // Several config setters defer SDL teardown/rebuild work, so use a
        // re-entrant state machine that waits a couple frames before verifying.
        constexpr int kApplyWaitFrames = 2;

        struct BoolAccessor
        {
            const char* name;
            std::function<bool()> getter;
            std::function<bool(bool)> setter;
            bool original = false;
        };

        static BoolAccessor boolProps[] = {
            {"PreserveAspectRatio",
                [](){ return SDOM_GetPreserveAspectRatio(); },
                [](bool v){ return SDOM_SetPreserveAspectRatio(v); }},
            {"AllowTextureResize",
                [](){ return SDOM_GetAllowTextureResize(); },
                [](bool v){ return SDOM_SetAllowTextureResize(v); }}
        };

        constexpr size_t kBoolCount = sizeof(boolProps) / sizeof(BoolAccessor);

        enum class Stage
        {
            Init,
            BoolSet,
            BoolVerify,
            BoolRestore,
            BoolRestoreVerify,
            LogicalCapture,
            LogicalSet,
            LogicalWait,
            LogicalVerify,
            LogicalRestore,
            LogicalRestoreWait,
            LogicalRestoreVerify,
            WindowFlagsCapture,
            WindowFlagsSet,
            WindowFlagsWait,
            WindowFlagsVerify,
            WindowFlagsRestore,
            WindowFlagsRestoreWait,
            WindowFlagsRestoreVerify,
            PixelFormatCapture,
            PixelFormatSet,
            PixelFormatWait,
            PixelFormatVerify,
            PixelFormatRestore,
            PixelFormatRestoreWait,
            PixelFormatRestoreVerify,
            Done
        };

        struct State
        {
            Stage stage = Stage::Init;
            size_t boolIndex = 0;
            int waitFrames = 0;
            SDL_RendererLogicalPresentation logicalOriginal = SDL_LOGICAL_PRESENTATION_DISABLED;
            SDL_RendererLogicalPresentation logicalAlt = SDL_LOGICAL_PRESENTATION_LETTERBOX;
            SDL_WindowFlags windowFlagsOriginal = SDL_WINDOW_RESIZABLE;
            SDL_WindowFlags windowFlagsAlt = SDL_WINDOW_BORDERLESS;
            SDL_PixelFormat pixelFormatOriginal = SDL_PIXELFORMAT_UNKNOWN;
            SDL_PixelFormat pixelFormatAlt = SDL_PIXELFORMAT_RGBA8888;
        };

        static State state;

        auto appendApiError = [&errors]()
        {
            if (const char* err = SDOM_GetError())
            {
                if (*err)
                    errors.emplace_back(err);
            }
        };

        while (true)
        {
            switch (state.stage)
            {
                case Stage::Init:
                    for (auto& prop : boolProps)
                        prop.original = prop.getter();
                    state.boolIndex = 0;
                    state.stage = Stage::BoolSet;
                    continue;

                case Stage::BoolSet:
                    if (state.boolIndex >= kBoolCount)
                    {
                        state.stage = Stage::LogicalCapture;
                        continue;
                    }
                    {
                        auto& prop = boolProps[state.boolIndex];
                        const bool toggled = !prop.original;
                        if (!prop.setter(toggled))
                        {
                            errors.push_back(std::string("Core_test4: set ") + prop.name + " failed");
                            appendApiError();
                        }
                        state.stage = Stage::BoolVerify;
                    }
                    continue;

                case Stage::BoolVerify:
                    {
                        auto& prop = boolProps[state.boolIndex];
                        const bool expected = !prop.original;
                        if (prop.getter() != expected)
                            errors.push_back(std::string("Core_test4: getter mismatch for ") + prop.name);
                        state.stage = Stage::BoolRestore;
                    }
                    continue;

                case Stage::BoolRestore:
                    {
                        auto& prop = boolProps[state.boolIndex];
                        if (!prop.setter(prop.original))
                        {
                            errors.push_back(std::string("Core_test4: restore ") + prop.name + " failed");
                            appendApiError();
                        }
                        state.stage = Stage::BoolRestoreVerify;
                    }
                    continue;

                case Stage::BoolRestoreVerify:
                    {
                        auto& prop = boolProps[state.boolIndex];
                        if (prop.getter() != prop.original)
                            errors.push_back(std::string("Core_test4: restore verification failed for ") + prop.name);
                        state.boolIndex++;
                        state.stage = Stage::BoolSet;
                    }
                    continue;

                case Stage::LogicalCapture:
                    state.logicalOriginal = SDOM_GetLogicalPresentation();
                    state.logicalAlt = (state.logicalOriginal == SDL_LOGICAL_PRESENTATION_LETTERBOX)
                        ? SDL_LOGICAL_PRESENTATION_INTEGER_SCALE
                        : SDL_LOGICAL_PRESENTATION_LETTERBOX;
                    if (state.logicalAlt == state.logicalOriginal)
                        state.logicalAlt = SDL_LOGICAL_PRESENTATION_DISABLED;
                    state.stage = Stage::LogicalSet;
                    continue;

                case Stage::LogicalSet:
                    if (!SDOM_SetLogicalPresentation(state.logicalAlt))
                    {
                        errors.push_back("Core_test4: SDOM_SetLogicalPresentation failed");
                        appendApiError();
                        state.stage = Stage::LogicalVerify;
                        continue;
                    }
                    state.waitFrames = kApplyWaitFrames;
                    state.stage = Stage::LogicalWait;
                    return false;

                case Stage::LogicalWait:
                    if (state.waitFrames > 0)
                    {
                        state.waitFrames--;
                        return false;
                    }
                    state.stage = Stage::LogicalVerify;
                    continue;

                case Stage::LogicalVerify:
                    if (SDOM_GetLogicalPresentation() != state.logicalAlt)
                        errors.push_back("Core_test4: logical presentation getter mismatch");
                    state.stage = Stage::LogicalRestore;
                    continue;

                case Stage::LogicalRestore:
                    if (!SDOM_SetLogicalPresentation(state.logicalOriginal))
                    {
                        errors.push_back("Core_test4: restore logical presentation failed");
                        appendApiError();
                        state.stage = Stage::LogicalRestoreVerify;
                        continue;
                    }
                    state.waitFrames = kApplyWaitFrames;
                    state.stage = Stage::LogicalRestoreWait;
                    return false;

                case Stage::LogicalRestoreWait:
                    if (state.waitFrames > 0)
                    {
                        state.waitFrames--;
                        return false;
                    }
                    state.stage = Stage::LogicalRestoreVerify;
                    continue;

                case Stage::LogicalRestoreVerify:
                    if (SDOM_GetLogicalPresentation() != state.logicalOriginal)
                        errors.push_back("Core_test4: logical presentation restore mismatch");
                    state.stage = Stage::WindowFlagsCapture;
                    continue;

                case Stage::WindowFlagsCapture:
                    state.windowFlagsOriginal = SDOM_GetWindowFlags();
                    state.windowFlagsAlt = static_cast<SDL_WindowFlags>(state.windowFlagsOriginal ^ SDL_WINDOW_BORDERLESS);
                    if (state.windowFlagsAlt == state.windowFlagsOriginal)
                        state.windowFlagsAlt = static_cast<SDL_WindowFlags>(state.windowFlagsOriginal ^ SDL_WINDOW_RESIZABLE);
                    state.stage = Stage::WindowFlagsSet;
                    continue;

                case Stage::WindowFlagsSet:
                    if (!SDOM_SetWindowFlags(state.windowFlagsAlt))
                    {
                        errors.push_back("Core_test4: SDOM_SetWindowFlags failed");
                        appendApiError();
                        state.stage = Stage::WindowFlagsVerify;
                        continue;
                    }
                    state.waitFrames = kApplyWaitFrames;
                    state.stage = Stage::WindowFlagsWait;
                    return false;

                case Stage::WindowFlagsWait:
                    if (state.waitFrames > 0)
                    {
                        state.waitFrames--;
                        return false;
                    }
                    state.stage = Stage::WindowFlagsVerify;
                    continue;

                case Stage::WindowFlagsVerify:
                    if (SDOM_GetWindowFlags() != state.windowFlagsAlt)
                        errors.push_back("Core_test4: window flags getter mismatch");
                    state.stage = Stage::WindowFlagsRestore;
                    continue;

                case Stage::WindowFlagsRestore:
                    if (!SDOM_SetWindowFlags(state.windowFlagsOriginal))
                    {
                        errors.push_back("Core_test4: restore window flags failed");
                        appendApiError();
                        state.stage = Stage::WindowFlagsRestoreVerify;
                        continue;
                    }
                    state.waitFrames = kApplyWaitFrames;
                    state.stage = Stage::WindowFlagsRestoreWait;
                    return false;

                case Stage::WindowFlagsRestoreWait:
                    if (state.waitFrames > 0)
                    {
                        state.waitFrames--;
                        return false;
                    }
                    state.stage = Stage::WindowFlagsRestoreVerify;
                    continue;

                case Stage::WindowFlagsRestoreVerify:
                    if (SDOM_GetWindowFlags() != state.windowFlagsOriginal)
                        errors.push_back("Core_test4: window flags restore mismatch");
                    state.stage = Stage::PixelFormatCapture;
                    continue;

                case Stage::PixelFormatCapture:
                    state.pixelFormatOriginal = SDOM_GetPixelFormat();
                    state.pixelFormatAlt = (state.pixelFormatOriginal == SDL_PIXELFORMAT_RGBA8888)
                        ? SDL_PIXELFORMAT_RGB565
                        : SDL_PIXELFORMAT_RGBA8888;
                    if (state.pixelFormatAlt == state.pixelFormatOriginal)
                        state.pixelFormatAlt = SDL_PIXELFORMAT_ARGB8888;
                    state.stage = Stage::PixelFormatSet;
                    continue;

                case Stage::PixelFormatSet:
                    if (!SDOM_SetPixelFormat(state.pixelFormatAlt))
                    {
                        errors.push_back("Core_test4: SDOM_SetPixelFormat failed");
                        appendApiError();
                        state.stage = Stage::PixelFormatVerify;
                        continue;
                    }
                    state.waitFrames = kApplyWaitFrames;
                    state.stage = Stage::PixelFormatWait;
                    return false;

                case Stage::PixelFormatWait:
                    if (state.waitFrames > 0)
                    {
                        state.waitFrames--;
                        return false;
                    }
                    state.stage = Stage::PixelFormatVerify;
                    continue;

                case Stage::PixelFormatVerify:
                    if (SDOM_GetPixelFormat() != state.pixelFormatAlt)
                        errors.push_back("Core_test4: pixel format getter mismatch");
                    state.stage = Stage::PixelFormatRestore;
                    continue;

                case Stage::PixelFormatRestore:
                    if (!SDOM_SetPixelFormat(state.pixelFormatOriginal))
                    {
                        errors.push_back("Core_test4: restore pixel format failed");
                        appendApiError();
                        state.stage = Stage::PixelFormatRestoreVerify;
                        continue;
                    }
                    state.waitFrames = kApplyWaitFrames;
                    state.stage = Stage::PixelFormatRestoreWait;
                    return false;

                case Stage::PixelFormatRestoreWait:
                    if (state.waitFrames > 0)
                    {
                        state.waitFrames--;
                        return false;
                    }
                    state.stage = Stage::PixelFormatRestoreVerify;
                    continue;

                case Stage::PixelFormatRestoreVerify:
                    if (SDOM_GetPixelFormat() != state.pixelFormatOriginal)
                        errors.push_back("Core_test4: pixel format restore mismatch");
                    state.stage = Stage::Done;
                    continue;

                case Stage::Done:
                    state = State{}; // reset for the next run
                    return true;
            }
        }
    } // END: Core_test4()


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
            ut.add_test(objName, "Version metadata", Core_test1);
            ut.add_test(objName, "Window dimensions", Core_test2);
            ut.add_test(objName, "Window title & border color", Core_test3);
            ut.add_test(objName, "Core config toggles", Core_test4);
            // call more unit tests from here



            // // Lua Integration Tests (Not Yet Implemented)
            // ut.add_test(objName, "Lua: 'src/Core_UnitTests.lua'", Core_LUA_Tests, false);

            registered = true;
        }

        return true;
    }


} // namespace SDOM

// #endif // SDOM_ENABLE_UNIT_TESTS
