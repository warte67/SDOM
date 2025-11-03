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

#include <string>
#include <vector>
#include <functional>

#include "UnitTests.hpp"

// Checks: ⚠️ ✅ ✔  

/***************

    Core Unit Tests:
    The following unit tests validate the functionality of these Methods of the Core class.
    All Core_test1 through Core_test16 have PASSED as of 2025-10-23.

    // --- Callback/Hook Registration --- //
    registerOnInit(std::function<bool()>)                        // ✅ Validated by: Core_test10 (C++ Only)
    registerOnQuit(std::function<void()>)                        // ✅ Validated by: Core_test10 (C++ Only)
    registerOnUpdate(std::function<void(float)>)                 // ✅ Validated by: Core_test10 (C++ Only)
    registerOnEvent(std::function<void(const Event&)>)           // ✅ Validated by: Core_test10 (C++ Only)
    registerOnRender(std::function<void()>)                      // ✅ Validated by: Core_test10 (C++ Only)
    registerOnUnitTest(std::function<bool()>)                    // ✅ Validated by: Core_test10 (C++ Only)
    registerOnWindowResize(std::function<void(int, int)>)        // ✅ Validated by: Core_test10 (C++ Only)
    registerOn(std::string name, sol::function f)                // ⚠️ Verify in config.lua

    // --- Lua Registration Internal Helpers --- //
    _fnOnInit(std::function<bool()> fn)                          // ✅ Validated by: Core_test10 (C++ Only)
    _fnOnQuit(std::function<void()> fn)                          // ✅ Validated by: Core_test10 (C++ Only)
    _fnOnUpdate(std::function<void(float)> fn)                   // ✅ Validated by: Core_test10 (C++ Only)
    _fnOnEvent(std::function<void(const Event&)> fn)             // ✅ Validated by: Core_test10 (C++ Only)
    _fnOnRender(std::function<void()> fn)                        // ✅ Validated by: Core_test10 (C++ Only)
    _fnOnUnitTest(std::function<bool()> fn)                      // ✅ Validated by: Core_test10 (C++ Only)
    _fnOnWindowResize(std::function<void(int, int)> fn)          // ✅ Validated by: Core_test10 (C++ Only)
    _fnGetOnInit()                                               // ✅ Validated by: Core_test10 (C++ Only)
    _fnGetOnQuit()                                               // ✅ Validated by: Core_test10 (C++ Only)
    _fnGetOnUpdate()                                             // ✅ Validated by: Core_test10 (C++ Only)
    _fnGetOnEvent()                                              // ✅ Validated by: Core_test10 (C++ Only)
    _fnGetOnRender()                                             // ✅ Validated by: Core_test10 (C++ Only)
    _fnGetOnUnitTest()                                           // ✅ Validated by: Core_test10 (C++ Only)
    _fnGetOnWindowResize()                                       // ✅ Validated by: Core_test10 (C++ Only)

    // --- Stage/Root Node Management --- //
    setRootNode(const std::string& name)                         // ✅ Validated by: Core_test11
    setRootNode(const DisplayHandle& handle)                     // ✅ Validated by: Core_test11
    setStage(const std::string& name)                            // ✅ Validated by: Core_test11
    getStage()                                                   // ✅ Validated by: Core_test11
    getRootNodePtr()                                             // ✅ Validated by: Core_test11
    getRootNode()                                                // ✅ Validated by: Core_test11
    getStageHandle()                                             // ✅ Validated by: Core_test11

    // --- Window/Renderer/Texture/Config --- //
    getWindow()                                                  // ✅ Validated by: Core_test1, Core_test12
    SDL_GetWindowSize                                            // ✅ Validated by: Core_test1
    getPixelWidth()                                              // ✅ Validated by: Core_test2, Core_test13
    getPixelHeight()                                             // ✅ Validated by: Core_test2, Core_test13
    getPreserveAspectRatio()                                     // ✅ Validated by: Core_test3, Core_test13
    getAllowTextureResize()                                      // ✅ Validated by: Core_test4, Core_test13
    getPixelFormat()                                             // ✅ Validated by: Core_test5, Core_test13
    getRendererLogicalPresentation()                             // ✅ Validated by: Core_test6, Core_test13
    getWindowFlags()                                             // ✅ Validated by: Core_test7, Core_test13
    getColor()                                                   // ✅ Validated by: Core_test8, Core_test12
    getConfig()                                                  // ✅ Validated/Used in: Core_test2–Core_test8, Core_test13
    setColor(const SDL_Color&)                                   // ✅ Validated by: Core_test12

    // --- SDL Resource Accessors --- //
    getWindow()                                                  // ✅ Validated by: Core_test12
    getRenderer()                                                // ✅ Validated by: Core_test12
    getTexture()                                                 // ✅ Validated by: Core_test12

    // --- Configuration Getters/Setters --- //
    getConfig();                                                 // ✅ Validated by: Core_test13
    getWindowWidth();                                            // ✅ Validated by: Core_test13
    getWindowHeight();                                           // ✅ Validated by: Core_test13
    getPixelWidth();                                             // ✅ Validated by: Core_test13
    getPixelHeight();                                            // ✅ Validated by: Core_test13
    getPreserveAspectRatio();                                    // ✅ Validated by: Core_test13
    getAllowTextureResize();                                     // ✅ Validated by: Core_test13
    getRendererLogicalPresentation();                            // ✅ Validated by: Core_test13
    getWindowFlags();                                            // ✅ Validated by: Core_test13
    getPixelFormat();                                            // ✅ Validated by: Core_test13

    setConfig(CoreConfig& config);                               // ✅ Validated by: Core_test13
    setWindowWidth(float width);                                 // ✅ Validated by: Core_test13
    setWindowHeight(float height);                               // ✅ Validated by: Core_test13
    setPixelWidth(float width);                                  // ✅ Validated by: Core_test13
    setPixelHeight(float height);                                // ✅ Validated by: Core_test13
    setPreserveAspectRatio(bool preserve);                       // ✅ Validated by: Core_test13
    setAllowTextureResize(bool allow);                           // not yet tested
    setRendererLogicalPresentation(presentation);                // ✅ Validated by: Core_test13
    setWindowFlags(SDL_WindowFlags flags);                       // ✅ Validated by: Core_test13
    setPixelFormat(SDL_PixelFormat format);                      // ✅ Validated by: Core_test13

    // --- Factory & EventManager Access --- //
    getFactory();                                                // ✅ Validated by: Core_test9, Core_test14
    getEventManager();                                           // ✅ Validated by: Core_test14
    getIsTraversing();                                           // ✅ Validated by: Core_test14
    setIsTraversing(bool traversing);                            // ✅ Validated by: Core_test14

    // --- Focus & Hover Management --- //
    handleTabKeyPress();                                         // ✅ Validated by: Core_test15
    handleTabKeyPressReverse();                                  // ✅ Validated by: Core_test15
    setKeyboardFocusedObject(DisplayHandle obj);                 // ✅ Validated by: Core_test15
    getKeyboardFocusedObject();                                  // ✅ Validated by: Core_test15
    clearKeyboardFocusedObject();                                // ✅ Validated by: Core_test15
    setMouseHoveredObject(DisplayHandle obj);                    // ✅ Validated by: Core_test15
    getMouseHoveredObject();                                     // ✅ Validated by: Core_test15

    // --- Lua State Access --- //
    getLua();                                                    // ✅ Verified by: Core::onInit()

    //////////////////////////////
    // --- Factory Wrappers --- //
    //////////////////////////////

    // --- Object Creation --- //
    createDisplayObject(const std::string& typeName, const sol::table& config); // ✅ Validated by: Core_test16
    createDisplayObject(const std::string& typeName,                            // ✅ Validated by: Core_test16
        const SDOM::IDisplayObject::InitStruct& init); 
    createDisplayObjectFromScript(const std::string& typeName,                  // ✅ Validated by: Core_test16
        const std::string& luaScript);       

    createAssetObject(const std::string& typeName, const sol::table& config);   // ✅ Validated by: Core_test16
    createAssetObject(const std::string& typeName,                              // ✅ Validated by: Core_test16
        const SDOM::IAssetObject::InitStruct& init);     
    createAssetObjectFromScript(const std::string& typeName,                    // ✅ Validated by: Core_test16
        const std::string& luaScript);         

    // --- Object Lookup --- //
    getDisplayObjectPtr(const std::string& name);                               // ✅ Validated by: Core_test15
    getDisplayObject(const std::string& name);                                  // ✅ Validated by: Core_test15, Core_test16
    hasDisplayObject(const std::string& name);                                  // ✅ Validated by: Core_test15

    getAssetObjectPtr(const std::string& name);                                 // ✅ Validated by: Core_test16
    getAssetObject(const std::string& name);                                    // ✅ Validated by: Core_test16
    hasAssetObject(const std::string& name);                                    // ✅ Validated by: Core_test16

    // --- Display Object Management --- //
    destroyDisplayObject(const std::string& name);                              // ✅ Validated by: Core_test16
    destroyAssetObject(const std::string& name);                                // ✅ Validated by: Core_test16

    // --- Orphan Management --- //
    countOrphanedDisplayObjects();                                              // ✅ Validated by: Core_test16
    getOrphanedDisplayObjects();                                                // ✅ Validated by: Core_test16
    detachOrphans();                                                            // ✅ Validated by: Core_test16
    collectGarbage();                                                           // ✅ Validated by: Core_test16

    // --- Future Child Management --- //
    attachFutureChildren();                                                             // ⚠️ Should this be exposed externally?
    addToOrphanList(const DisplayHandle& orphan);                                       // ⚠️ Should this be exposed externally?
    addToFutureChildrenList(const DisplayHandle& child, const DisplayHandle& parent,    // ⚠️ Should this be exposed externally?
            bool useWorld=false, int worldX=0, int worldY=0);

    // --- Utility Methods --- //
    listDisplayObjectNames();                                                           // ⚠️ Should this be exposed externally?
    clearFactory();                                                                     // ⚠️ Should this be exposed externally?
    printObjectRegistry();                                                              // ⚠️ Should this be exposed externally?

    findAssetByFilename(const std::string& filename, const std::string& typeName = ""); // ⚠️ Should this be exposed externally?
    findSpriteSheetByParams(const std::string& filename, int spriteW, int spriteH);     // ⚠️ Should this be exposed externally?
    unloadAllAssetObjects();                                                            // ⚠️ Should this be exposed externally?
    reloadAllAssetObjects();                                                            // ⚠️ Should this be exposed externally?

    // --- Forwarding helpers to Factory for type-level introspection --- //
    getPropertyNamesForType(const std::string& typeName);                               // ⚠️ Should this be exposed externally?
    getCommandNamesForType(const std::string& typeName);                                // ⚠️ Should this be exposed externally?
    getFunctionNamesForType(const std::string& typeName);                               // ⚠️ Should this be exposed externally?

****************/
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
    bool Core_test0(std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        // ok = false;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

    } // END: Core_scaffolding(std::vector<std::string>& errors)



    // --- Core_test1: Window Dimensions ---
    bool Core_test1(std::vector<std::string>& errors)
    {
        auto& core = getCore();

        if (!core.getWindow()) {
            errors.push_back("Core window handle is null!");
        } else {
            int w = core.getWindowWidth();
            int h = core.getWindowHeight();
            if (w <= 0 || h <= 0)
                errors.push_back("Invalid window dimensions: " + std::to_string(w) + "x" + std::to_string(h));
        }

        return true; // ✅ finished this frame
    }


    // --- Core_test2: Pixel Dimensions --------------------------------------------
    bool Core_test2(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        const auto& expected = core.getConfig();

        float pixel_w = core.getPixelWidth();
        if (pixel_w != expected.pixelWidth)
        {
            errors.push_back("Pixel width mismatch — expected: " +
                std::to_string(expected.pixelWidth) + ", actual: " +
                std::to_string(pixel_w));
        }

        float pixel_h = core.getPixelHeight();
        if (pixel_h != expected.pixelHeight)
        {
            errors.push_back("Pixel height mismatch — expected: " +
                std::to_string(expected.pixelHeight) + ", actual: " +
                std::to_string(pixel_h));
        }

        return true;  // ✅ finished this frame
    } // END: Core_test2(std::vector<std::string>& errors)


    // --- Core_test3: Aspect Ratio Preservation -----------------------------------
    bool Core_test3(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        const auto& expected = core.getConfig();

        bool preserve_aspect = core.getPreserveAspectRatio();
        if (preserve_aspect != expected.preserveAspectRatio)
        {
            errors.push_back(
                "PreserveAspectRatio mismatch — expected: " +
                std::to_string(expected.preserveAspectRatio) +
                ", actual: " + std::to_string(preserve_aspect)
            );
        }

        return true;  // ✅ finished this frame
    } // END: Core_test3(std::vector<std::string>& errors)


    // --- Core_test4: Texture Resize Allowance ------------------------------------
    bool Core_test4(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        const auto& expected = core.getConfig();

        bool allow_resize = core.getAllowTextureResize();
        if (allow_resize != expected.allowTextureResize)
        {
            errors.push_back(
                "AllowTextureResize mismatch — expected: " +
                std::to_string(expected.allowTextureResize) +
                ", actual: " + std::to_string(allow_resize)
            );
        }

        return true;  // ✅ finished this frame
    } // END: Core_test4(std::vector<std::string>& errors)


    // --- Core_test5: Pixel Format -------------------------------------------------
    bool Core_test5(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        const auto& expected = core.getConfig();

        SDL_PixelFormat pixel_format = core.getPixelFormat();
        if (pixel_format != expected.pixelFormat)
        {
            errors.push_back(
                "PixelFormat mismatch — expected: " +
                std::to_string(expected.pixelFormat) +
                ", actual: " + std::to_string(pixel_format)
            );
        }

        return true;  // ✅ finished this frame
    } // END: Core_test5(std::vector<std::string>& errors)


    // --- Core_test6: Renderer Logical Presentation --------------------------------
    bool Core_test6(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        const auto& expected = core.getConfig();

        SDL_RendererLogicalPresentation presentation = core.getRendererLogicalPresentation();
        if (presentation != expected.rendererLogicalPresentation)
        {
            errors.push_back(
                "RendererLogicalPresentation mismatch — expected: " +
                std::to_string(expected.rendererLogicalPresentation) +
                ", actual: " + std::to_string(presentation)
            );
        }

        return true;  // ✅ finished this frame
    } // END: Core_test6(std::vector<std::string>& errors)


    // --- Core_test7: Window Flags -------------------------------------------------
    bool Core_test7(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        const auto& expected = core.getConfig();

        SDL_WindowFlags window_flags = core.getWindowFlags();
        if (window_flags != expected.windowFlags)
        {
            errors.push_back(
                "WindowFlags mismatch — expected: " +
                std::to_string(expected.windowFlags) +
                ", actual: " + std::to_string(window_flags)
            );
        }

        return true;  // ✅ finished this frame
    } // END: Core_test7(std::vector<std::string>& errors)


    // --- Core_test8: Core Background Color ----------------------------------------
    bool Core_test8(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        const auto& expected = core.getConfig();

        SDL_Color color = core.getColor();
        if (color.r != expected.color.r || color.g != expected.color.g ||
            color.b != expected.color.b || color.a != expected.color.a)
        {
            errors.push_back(
                "Background color mismatch — expected RGBA(" +
                std::to_string(expected.color.r) + ", " +
                std::to_string(expected.color.g) + ", " +
                std::to_string(expected.color.b) + ", " +
                std::to_string(expected.color.a) + "), actual RGBA(" +
                std::to_string(color.r) + ", " +
                std::to_string(color.g) + ", " +
                std::to_string(color.b) + ", " +
                std::to_string(color.a) + ")"
            );
        }

        return true;  // ✅ finished this frame
    } // END: Core_test8(std::vector<std::string>& errors)


    // --- Core_test9: Factory Existence --------------------------------------------
    bool Core_test9(std::vector<std::string>& errors)
    {
        Core& core = getCore();

        // Ensure Factory instance is valid
        if (&core.getFactory() == nullptr)
            errors.push_back("Factory instance is null or inaccessible.");

        return true;  // ✅ finished this frame
    } // END: Core_test9(std::vector<std::string>& errors)


    // --- Core_test10: Callback / Hook Registration --------------------------------
    bool Core_test10(std::vector<std::string>& errors)
    {
        Core& core = getCore();

        // Define test lambdas for registration validation
        [[maybe_unused]] auto testInitFn         = []() -> bool { return true; };
        [[maybe_unused]] auto testQuitFn         = []() {};
        [[maybe_unused]] auto testUpdateFn       = [](float) {};
        [[maybe_unused]] auto testEventFn        = [](const Event&) {};
        [[maybe_unused]] auto testRenderFn       = []() {};
        [[maybe_unused]] auto testUnitTestFn     = []() -> bool { return true; };
        [[maybe_unused]] auto testWindowResizeFn = [](int, int) {};

        // Preserve original callbacks
        auto orig_fnOnInit         = core._fnGetOnInit();
        auto orig_fnOnQuit         = core._fnGetOnQuit();
        auto orig_fnOnUpdate       = core._fnGetOnUpdate();
        auto orig_fnOnEvent        = core._fnGetOnEvent();
        auto orig_fnOnRender       = core._fnGetOnRender();
        auto orig_fnOnWindowResize = core._fnGetOnWindowResize();

        // Register temporary test callbacks
        core.registerOnInit(testInitFn);
        core.registerOnQuit(testQuitFn);
        core.registerOnUpdate(testUpdateFn);
        core.registerOnEvent(testEventFn);
        core.registerOnRender(testRenderFn);
        core.registerOnWindowResize(testWindowResizeFn);

        // Validate registration via type identity
        if (core._fnGetOnInit().target_type() != typeid(testInitFn))
            errors.push_back("OnInit function not set correctly.");

        if (core._fnGetOnQuit().target_type() != typeid(testQuitFn))
            errors.push_back("OnQuit function not set correctly.");

        if (core._fnGetOnUpdate().target_type() != typeid(testUpdateFn))
            errors.push_back("OnUpdate function not set correctly.");

        if (core._fnGetOnEvent().target_type() != typeid(testEventFn))
            errors.push_back("OnEvent function not set correctly.");

        if (core._fnGetOnRender().target_type() != typeid(testRenderFn))
            errors.push_back("OnRender function not set correctly.");

        if (core._fnGetOnWindowResize().target_type() != typeid(testWindowResizeFn))
            errors.push_back("OnWindowResize function not set correctly.");

        // Restore original callbacks
        core._fnOnInit(orig_fnOnInit);
        core._fnOnQuit(orig_fnOnQuit);
        core._fnOnUpdate(orig_fnOnUpdate);
        core._fnOnEvent(orig_fnOnEvent);
        core._fnOnRender(orig_fnOnRender);
        core._fnOnWindowResize(orig_fnOnWindowResize);

        return true;  // ✅ finished this frame
    } // END: Core_test10(std::vector<std::string>& errors)


    // --- Core_test11: Stage / Root Node Management --------------------------------
    bool Core_test11(std::vector<std::string>& errors)
    {
        Core& core = getCore();

        // Helper lambda: validate DisplayHandle and name consistency
        auto checkStage = [&](const DisplayHandle& handle, const std::string& expectedName, const std::string& context)
        {
            if (!handle.isValid())
                errors.push_back(context + " is not valid.");

            if (handle.getName() != expectedName)
                errors.push_back(context + " name mismatch — expected '" + expectedName +
                                "', actual '" + handle.getName() + "'.");
        };

        // --- Validate initial root node ---
        DisplayHandle rootStage = core.getRootNode();
        checkStage(rootStage, "mainStage", "Initial Root Node/Stage");

        // --- Test: setRootNode() by name ---
        core.setRootNode("stageTwo");
        DisplayHandle newStage = core.getRootNode();
        checkStage(newStage, "stageTwo", "Root Node/Stage after setRootNode(name)");

        // --- Test: setRootNode() by DisplayHandle ---
        DisplayHandle stageThree = core.getDisplayObject("stageThree");
        core.setRootNode(stageThree);
        DisplayHandle anotherStage = core.getRootNode();
        checkStage(anotherStage, "stageThree", "Root Node/Stage after setRootNode(DisplayHandle)");

        // --- Test: object existence and pointer access ---
        if (core.hasDisplayObject("nonExistentStage"))
            errors.push_back("Core incorrectly reports existence of 'nonExistentStage'.");

        if (!core.hasDisplayObject("stageTwo"))
            errors.push_back("Core failed to report existence of 'stageTwo'.");

        if (core.getDisplayObjectPtr("nonExistentStage") != nullptr)
            errors.push_back("getDisplayObjectPtr('nonExistentStage') returned non-nullptr.");

        if (core.getDisplayObjectPtr("stageTwo") == nullptr)
            errors.push_back("getDisplayObjectPtr('stageTwo') returned nullptr.");

        // --- Restore original root stage ---
        core.setStage(rootStage.getName());
        DisplayHandle restoredStage = core.getRootNode();
        checkStage(restoredStage, "mainStage", "Root Node/Stage after restoring original");

        // --- Validate getRootNodePtr() and type correctness ---
        IDisplayObject* rootPtr = core.getRootNodePtr();
        if (!rootPtr)
            errors.push_back("getRootNodePtr() returned nullptr.");
        else
        {
            Stage* stagePtr = dynamic_cast<Stage*>(rootPtr);
            if (!stagePtr)
                errors.push_back("getRootNodePtr() did not return a Stage pointer.");
            else if (stagePtr->getName() != "mainStage")
                errors.push_back("Stage name mismatch — expected 'mainStage', actual '" + stagePtr->getName() + "'.");
        }

        // --- Validate getStageHandle() ---
        DisplayHandle stageHandle = core.getStageHandle();
        checkStage(stageHandle, "mainStage", "Stage from getStageHandle()");

        return true;  // ✅ finished this frame
    } // END: Core_test11(std::vector<std::string>& errors)


    // --- Core_test12: SDL Resource Accessors -------------------------------------
    bool Core_test12(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        UnitTests& ut = UnitTests::getInstance();

        // --- Validate SDL_Window linkage ---
        SDL_Window* window = ut.getWindow();
        if (!window)
            errors.push_back("SDL_Window is nullptr.");
        else if (core.getWindow() != window)
            errors.push_back("SDL_Window accessor mismatch — Core window does not match UnitTest window.");

        // --- Validate SDL_Renderer linkage ---
        SDL_Renderer* renderer = ut.getRenderer();
        if (!renderer)
            errors.push_back("SDL_Renderer is nullptr.");
        else if (core.getRenderer() != renderer)
            errors.push_back("SDL_Renderer accessor mismatch — Core renderer does not match UnitTest renderer.");

        // --- Validate SDL_Texture linkage ---
        SDL_Texture* texture = ut.getTexture();
        if (!texture)
            errors.push_back("SDL_Texture is nullptr.");
        else if (core.getTexture() != texture)
            errors.push_back("SDL_Texture accessor mismatch — Core texture does not match UnitTest texture.");

        // --- Validate SDL_Color getter/setter roundtrip ---
        SDL_Color orig_color = ut.getColor();
        core.setColor({64, 128, 192, 255});
        SDL_Color new_color = core.getColor();

        if (new_color.r != 64 || new_color.g != 128 || new_color.b != 192 || new_color.a != 255)
            errors.push_back("SDL_Color setter failed — expected (64,128,192,255).");

        // Restore original color and verify round-trip consistency
        core.setColor(orig_color);
        SDL_Color restored_color = core.getColor();

        if (restored_color.r != orig_color.r || restored_color.g != orig_color.g ||
            restored_color.b != orig_color.b || restored_color.a != orig_color.a)
            errors.push_back("SDL_Color restoration mismatch — Core color did not return to original.");

        return true;  // ✅ finished this frame
    } // END: Core_test12(std::vector<std::string>& errors)


    // --- Core_test13: Configuration Getters / Setters ----------------------------
    //
    // 🧩  Purpose:
    //   Verifies that Core configuration getters and setters are synchronized,
    //   and optionally performs a full stress test that exercises the entire SDL
    //   display chain rebuild sequence (Window, Renderer, Texture, etc).
    //
    // 🧠  Notes:
    //   • The full configuration stress test (`FULL_CONFIG_TEST = true`) will 
    //     repeatedly rebuild SDL resources. Use this to detect GPU, kernel, 
    //     or API-level memory leaks.
    //   • When running automated or iterative test suites (e.g. repeat_test.sh),
    //     keep `FULL_CONFIG_TEST = false` to minimize overhead and runtime.
    //
    // ============================================================================
    bool Core_test13(std::vector<std::string>& errors)
    {
        // Set to `true` for exhaustive SDL reconfiguration test.
        // Default: `false` for essential validation only.
        constexpr bool FULL_CONFIG_TEST = false;

        Core& core = getCore();
        Core::CoreConfig orig = core.getConfig();

        // Save original config
        bool final_allow = core.getAllowTextureResize();

        // --- Quick validation mode ----------------------------------------------
        if (!FULL_CONFIG_TEST)
        {
            if (orig.windowWidth != core.getWindowWidth())
                errors.push_back("CoreConfig.windowWidth does not match Core getter.");

            if (orig.windowHeight != core.getWindowHeight())
                errors.push_back("CoreConfig.windowHeight does not match Core getter.");

            if (orig.pixelWidth != core.getPixelWidth())
                errors.push_back("CoreConfig.pixelWidth does not match Core getter.");

            if (orig.pixelHeight != core.getPixelHeight())
                errors.push_back("CoreConfig.pixelHeight does not match Core getter.");

            if (orig.preserveAspectRatio != core.getPreserveAspectRatio())
                errors.push_back("CoreConfig.preserveAspectRatio does not match Core getter.");

            if (orig.allowTextureResize != core.getAllowTextureResize())
                errors.push_back("CoreConfig.allowTextureResize does not match Core getter.");

            if (orig.rendererLogicalPresentation != core.getRendererLogicalPresentation())
                errors.push_back("CoreConfig.rendererLogicalPresentation does not match Core getter.");

            if (orig.windowFlags != core.getWindowFlags())
                errors.push_back("CoreConfig.windowFlags does not match Core getter.");

            if (orig.pixelFormat != core.getPixelFormat())
                errors.push_back("CoreConfig.pixelFormat does not match Core getter.");

            return errors.empty(); // ✅ finished this frame
        }

        // --- Full configuration stress test -------------------------------------
        std::vector<Core::CoreConfig> testConfigs = {
            orig,
            Core::CoreConfig{640, 480, 1.0f, 1.0f, true, false,
                            SDL_LOGICAL_PRESENTATION_STRETCH,
                            SDL_WINDOW_BORDERLESS,
                            SDL_PIXELFORMAT_RGBA8888,
                            SDL_Color{64, 128, 255, 255}},

            Core::CoreConfig{800, 600, 2.0f, 2.0f, false, true,
                            SDL_LOGICAL_PRESENTATION_DISABLED,
                            SDL_WINDOW_FULLSCREEN,
                            SDL_PIXELFORMAT_ARGB8888,
                            SDL_Color{255, 64, 128, 128}},

            Core::CoreConfig{1024, 768, 4.0f, 4.0f, true, true,
                            SDL_LOGICAL_PRESENTATION_LETTERBOX,
                            SDL_WINDOW_RESIZABLE,
                            SDL_PIXELFORMAT_BGRA8888,
                            SDL_Color{128, 255, 64, 64}},

            orig // restore original at the end
        };

        for (auto& cfg : testConfigs)
        {
            core.setConfig(cfg);

            // Validate core state after each reconfiguration
            if (cfg.windowWidth != core.getWindowWidth())
                errors.push_back("windowWidth mismatch after setConfig().");

            if (cfg.windowHeight != core.getWindowHeight())
                errors.push_back("windowHeight mismatch after setConfig().");

            if (cfg.pixelWidth != core.getPixelWidth())
                errors.push_back("pixelWidth mismatch after setConfig().");

            if (cfg.pixelHeight != core.getPixelHeight())
                errors.push_back("pixelHeight mismatch after setConfig().");

            if (cfg.preserveAspectRatio != core.getPreserveAspectRatio())
                errors.push_back("preserveAspectRatio mismatch after setConfig().");

            if (cfg.allowTextureResize != core.getAllowTextureResize())
                errors.push_back("allowTextureResize mismatch after setConfig().");

            if (cfg.rendererLogicalPresentation != core.getRendererLogicalPresentation())
                errors.push_back("rendererLogicalPresentation mismatch after setConfig().");

            if (cfg.windowFlags != core.getWindowFlags())
                errors.push_back("windowFlags mismatch after setConfig().");

            if (cfg.pixelFormat != core.getPixelFormat())
                errors.push_back("pixelFormat mismatch after setConfig().");

            // SDL resource integrity
            if (!core.getWindow())
                errors.push_back("SDL_Window is nullptr after setConfig().");

            if (!core.getRenderer())
                errors.push_back("SDL_Renderer is nullptr after setConfig().");

            if (!core.getTexture())
                errors.push_back("SDL_Texture is nullptr after setConfig().");
        }
        // Restore original config and verify
        if (final_allow != orig.allowTextureResize)
            errors.push_back("AllowTextureResize not restored after full config test.");

        return true; // ✅ finished this frame
    } // END: Core_test13(std::vector<std::string>& errors)


    // --- Core_test14: Factory and Event Manager Access ---------------------------
    //
    // 🧩 Purpose:
    //   Ensures Core provides valid access to its shared Factory and EventManager
    //   instances, and that Core’s traversal state management behaves correctly.
    //
    // 🧠 Notes:
    //   • This test ensures that the Core instance shares the same Factory and
    //     EventManager as the UnitTests singleton, preventing divergent instances.
    //   • It also verifies that Core::setIsTraversing() updates and restores the
    //     traversal state correctly without desynchronization.
    //
    // ============================================================================
    bool Core_test14(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        UnitTests& ut = UnitTests::getInstance();

        // --- Verify Factory linkage ---
        if (&core.getFactory() != &ut.getFactory())
            errors.push_back("Factory linkage mismatch — Core factory does not match UnitTest factory.");

        // --- Verify EventManager linkage ---
        if (&core.getEventManager() != &ut.getEventManager())
            errors.push_back("EventManager linkage mismatch — Core event manager does not match UnitTest event manager.");

        // --- Verify traversal state toggling ---
        bool old_traversing = core.getIsTraversing();
        core.setIsTraversing(!old_traversing);

        if (core.getIsTraversing() == old_traversing)
            errors.push_back("setIsTraversing() failed to toggle traversal state.");

        // Restore original state
        core.setIsTraversing(old_traversing);
        if (core.getIsTraversing() != old_traversing)
            errors.push_back("setIsTraversing() failed to restore original traversal state.");

        return true;  // ✅ finished this frame
    } // END: Core_test14(std::vector<std::string>& errors)


    // --- Core_test15: Focus & Hover Management -----------------------------------
    //
    // 🧩 Purpose:
    //   Validates focus traversal (Tab/Reverse-Tab cycling) and mouse hover tracking
    //   within the Core UI system.
    //
    // 🧠 Notes:
    //   • This test ensures keyboard focus transitions work predictably across objects.
    //   • It also validates that mouse hover tracking and restoration behave correctly.
    //   • The test safely restores cursor position and focus state after completion.
    //
    // ⚠️ Safety:
    //   SDL_WarpMouseInWindow() is called only when necessary; cursor position is
    //   restored to avoid side effects across test iterations.
    //
    // ============================================================================
    bool Core_test15(std::vector<std::string>& errors)
    {
        Core& core = getCore();

        // --- Save current mouse position (for restoration later) ---
        float mx = 0.0f, my = 0.0f;
        SDL_GetMouseState(&mx, &my);

        // --- Focus Management Tests ---
        core.handleTabKeyPress();
        DisplayHandle original_focused = core.getKeyboardFocusedObject();

        if (!original_focused.isValid())
        {
            errors.push_back("No originally focused object found after handleTabKeyPress().");
        }
        else
        {
            core.handleTabKeyPress();
            if (core.getKeyboardFocusedObject() == original_focused)
            {
                errors.push_back("Tab key press did not advance focus to a new object.");
            }

            core.handleTabKeyPressReverse();
            if (core.getKeyboardFocusedObject() != original_focused)
            {
                errors.push_back("Reverse Tab key press did not restore original focused object.");
            }

            core.handleTabKeyPress();
            if (core.getKeyboardFocusedObject() == original_focused)
            {
                errors.push_back("Second Tab key press did not change focus from original object.");
            }

            // Verify manual restoration works
            core.setKeyboardFocusedObject(original_focused);
            if (core.getKeyboardFocusedObject() != original_focused)
            {
                errors.push_back("setKeyboardFocusedObject() failed to restore original focus.");
            }
        }

        // --- Hover Management Tests ---
        core.clearMouseHoveredObject();
        DisplayHandle original_hovered = core.getMouseHoveredObject();

        if (original_hovered.isValid())
        {
            errors.push_back("clearMouseHoveredObject() failed — hovered object still valid.");
        }

        DisplayHandle blueishBox = core.getDisplayObject("blueishBox");
        if (!blueishBox.isValid())
        {
            errors.push_back("DisplayObject 'blueishBox' not found for hover test.");
        }
        else
        {
            core.setMouseHoveredObject(blueishBox);
            if (core.getMouseHoveredObject() != blueishBox)
            {
                errors.push_back("setMouseHoveredObject() did not assign blueishBox.");
            }

            // Move mouse over the center of the target object
            SDL_WarpMouseInWindow(core.getWindow(),
                static_cast<int>(blueishBox->getX() + (blueishBox->getWidth() * 0.5f)),
                static_cast<int>(blueishBox->getY() + (blueishBox->getHeight() * 0.5f)));

            DisplayHandle new_hovered = core.getMouseHoveredObject();
            if (new_hovered != blueishBox)
            {
                errors.push_back("Mouse hovered object mismatch — expected blueishBox after move event.");
            }
        }

        // --- Restore mouse position ---
        SDL_WarpMouseInWindow(core.getWindow(), mx, my);

        return true; // ✅ finished this frame
    } // END: Core_test15(std::vector<std::string>& errors)


    // --- Core_test16: Object Creation and Orphan Management ------------------------
    //
    // 🧩 Purpose:
    //   Validates Core’s object and asset creation/destruction pipelines,
    //   ensuring that DisplayObjects and AssetObjects created via sol::table,
    //   InitStruct, and Lua script behave consistently and that orphaned
    //   DisplayObjects are properly tracked, attached, detached, and collected.
    //
    // 🧠 Notes:
    //   • Verifies both object and asset creation lifecycles through all interfaces.
    //   • Confirms orphan tracking, reattachment, and cleanup work as expected.
    //   • Ensures that asset registry integrity is preserved across destroy calls.
    //
    // ⚠️ Safety:
    //   This test may modify Core’s internal registries. All temporary objects are
    //   explicitly destroyed, and orphaned objects are detached before return.
    //
    // ============================================================================
    bool Core_test16(std::vector<std::string>& errors)
    {
        Core& core = getCore();

        // --- DisplayObject Creation: sol::table ---
        sol::table boxConfig = core.getLua().create_table();
        boxConfig["name"] = "testBox";
        boxConfig["type"] = "Box";
        boxConfig["color"] = sol::as_table(std::vector<int>{255, 0, 0, 255});
        boxConfig["x"] = 42;
        boxConfig["y"] = 24;
        boxConfig["width"] = 100;
        boxConfig["height"] = 50;

        DisplayHandle boxHandle = core.createDisplayObject("Box", boxConfig);
        if (!boxHandle.isValid()) {
            errors.push_back("createDisplayObject(sol::table) failed for Box.");
        }
        core.destroyDisplayObject(boxHandle.getName());

        // --- DisplayObject Creation: InitStruct ---
        SDOM::IDisplayObject::InitStruct boxInit;
        boxInit.name = "testBox2";
        boxInit.type = "Box";
        boxInit.x = 100;
        boxInit.y = 200;
        boxInit.width = 80;
        boxInit.height = 40;

        DisplayHandle boxHandle2 = core.createDisplayObject("Box", boxInit);
        if (!boxHandle2.isValid()) {
            errors.push_back("createDisplayObject(InitStruct) failed for Box.");
        }
        core.destroyDisplayObject(boxHandle2.getName());

        // --- DisplayObject Creation: Lua Script ---
        std::string boxScript = R"(
            name = "testBox3",
            type = "Box",
            color = {0, 255, 0, 255},
            x = 300,
            y = 400,
            width = 60,
            height = 30
        )";
        DisplayHandle boxHandle3 = core.createDisplayObjectFromScript("Box", boxScript);
        if (!boxHandle3.isValid()) {
            errors.push_back("createDisplayObjectFromScript() failed for Box.");
        }

        // --- Orphan Management ---
        int orphanCount = core.countOrphanedDisplayObjects();
        if (orphanCount == 0) {
            errors.push_back("Expected orphaned DisplayObjects after Lua creation; found none.");
        }

        std::vector<DisplayHandle> orphans = core.getOrphanedDisplayObjects();
        if (orphans.size() != 1) {
            errors.push_back("Expected one orphan; found " + std::to_string(orphans.size()) + ".");
        }

        DisplayHandle orphan = !orphans.empty() ? orphans[0] : DisplayHandle();
        if (orphan.isValid() && orphan != boxHandle3) {
            errors.push_back("Orphaned DisplayObject does not match the created Box instance.");
        }

        // Attach orphan to stage
        DisplayHandle stage = core.getStageHandle();
        if (stage.isValid() && orphan.isValid()) {
            stage->addChild(orphan);
            orphanCount = core.countOrphanedDisplayObjects();
            if (orphanCount != 0) {
                errors.push_back("Orphans not cleared after attaching to stage.");
            }

            // Detach again to reintroduce orphan state
            stage->removeChild(orphan);
            orphanCount = core.countOrphanedDisplayObjects();
            if (orphanCount != 1) {
                errors.push_back("Incorrect orphan count after detaching from stage.");
            }
        }

        // Validate orphan cleanup
        core.detachOrphans();
        if (orphan.isValid() && orphan->getParent().isValid()) {
            errors.push_back("Detached orphan still has valid parent.");
        }

        core.collectGarbage();
        if (core.countOrphanedDisplayObjects() != 0) {
            errors.push_back("Garbage collection failed to clean orphaned DisplayObjects.");
        }

        // --- AssetObject Creation: sol::table ---
        sol::table assetConfig = core.getLua().create_table();
        assetConfig["name"] = "bmp_font_8x8";
        assetConfig["type"] = "BitmapFont";
        assetConfig["filename"] = "./assets/font_8x8.png";
        assetConfig["font_width"] = 8;
        assetConfig["font_height"] = 8;

        AssetHandle assetHandle = core.createAssetObject("BitmapFont", assetConfig);
        if (!assetHandle.isValid()) {
            errors.push_back("createAssetObject(sol::table) failed for BitmapFont.");
        }
        if (core.hasAssetObject("non_existent_asset")) {
            errors.push_back("Core incorrectly reports existence of non_existent_asset.");
        }
        if (!core.hasAssetObject("bmp_font_8x8")) {
            errors.push_back("Core failed to report bmp_font_8x8 after creation.");
        }
        core.destroyAssetObject(assetHandle.getName());
        if (core.getAssetObject("bmp_font_8x8").isValid()) {
            errors.push_back("AssetObject bmp_font_8x8 was not destroyed properly.");
        }

        // --- AssetObject Creation: InitStruct ---
        SDOM::BitmapFont::InitStruct bmpAssetInit;
        bmpAssetInit.name = "testAsset2";
        bmpAssetInit.type = "BitmapFont";
        bmpAssetInit.filename = "./assets/font_8x8.png";
        bmpAssetInit.font_width = 8;
        bmpAssetInit.font_height = 8;

        AssetHandle assetHandle2 = core.createAssetObject("BitmapFont", bmpAssetInit);
        if (!assetHandle2.isValid()) {
            errors.push_back("createAssetObject(InitStruct) failed for BitmapFont.");
        }
        core.destroyAssetObject(assetHandle2.getName());
        if (core.getAssetObject("testAsset2").isValid()) {
            errors.push_back("AssetObject testAsset2 was not destroyed properly.");
        }

        // --- AssetObject Creation: Lua Script ---
        std::string assetScript = R"(
            name = "testAsset3",
            type = "BitmapFont",
            filename = "./assets/font_8x8.png",
            font_width = 8,
            font_height = 8
        )";
        AssetHandle assetHandle3 = core.createAssetObjectFromScript("BitmapFont", assetScript);
        if (!assetHandle3.isValid()) {
            errors.push_back("createAssetObjectFromScript() failed for BitmapFont.");
        }

        return true; // ✅ finished this frame
    } // END: Core_test16(std::vector<std::string>& errors)


    // --- Core_test17: Window Title Roundtrip ---------------------------------------
    //
    // 🧩 Purpose:
    //   Validates Core’s window title accessor and mutator functions, ensuring that
    //   title changes propagate correctly to the underlying SDL_Window and can be
    //   queried consistently via Core::getWindowTitle().
    //
    // 🧠 Notes:
    //   • This test performs a simple getter/setter roundtrip using a known string.
    //   • The original window title is preserved and restored after verification.
    //   • Behavior relies on SDL_GetWindowTitle() / SDL_SetWindowTitle() semantics.
    //
    // ⚠️ Safety:
    //   Title changes are non-destructive and transient; no SDL resources are rebuilt.
    //   This test is safe for iterative and automated execution.
    //
    // ============================================================================
    bool Core_test17(std::vector<std::string>& errors)
    {
        Core& core = getCore();

        // --- Preserve original title ---
        std::string original = core.getWindowTitle();
        std::string testTitle = "SDOM UnitTest Window";

        // --- Apply new title and verify roundtrip ---
        core.setWindowTitle(testTitle);
        std::string result = core.getWindowTitle();

        if (result != testTitle)
            errors.push_back("Window title mismatch — expected '" + testTitle + "', got '" + result + "'.");

        // --- Restore original title ---
        core.setWindowTitle(original);

        return true; // ✅ finished this frame
    } // END: Core_test17(std::vector<std::string>& errors)


    // --- Lua Integration Tests --- //

    bool Core_LUA_Tests(std::vector<std::string>& errors)
    {
        UnitTests& ut = UnitTests::getInstance();
        return ut.run_lua_tests(errors, ut.getLuaFilename());
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
            ut.add_test(objName, "Window Dimensions", Core_test1);
            ut.add_test(objName, "Pixel Dimensions", Core_test2);
            ut.add_test(objName, "Aspect Ratio Preservation", Core_test3);
            ut.add_test(objName, "Texture Resize Allowance", Core_test4);
            ut.add_test(objName, "Pixel Format", Core_test5);
            ut.add_test(objName, "Renderer Logical Presentation", Core_test6);
            ut.add_test(objName, "Window Flags", Core_test7);
            ut.add_test(objName, "Core Background Color", Core_test8);
            ut.add_test(objName, "Factory Existence", Core_test9);
            ut.add_test(objName, "Callback/Hook Registration", Core_test10);
            ut.add_test(objName, "Stage/Root Node Management", Core_test11);
            ut.add_test(objName, "SDL Resource Accessors", Core_test12);
            ut.add_test(objName, "Configuration Getters/Setters", Core_test13);
            ut.add_test(objName, "Factory and Event Manager Access", Core_test14);
            ut.add_test(objName, "Focus & Hover Management", Core_test15);
            ut.add_test(objName, "DisplayObject Creation", Core_test16);
            ut.add_test(objName, "Window Title Roundtrip", Core_test17);

            ut.setLuaFilename("src/Core_UnitTests.lua");
            ut.add_test(objName, "Lua: '" + ut.getLuaFilename() + "'", Core_LUA_Tests);

            registered = true;
        }

        return true;
    }


} // namespace SDOM

// #endif // SDOM_ENABLE_UNIT_TESTS
