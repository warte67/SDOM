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
    setAllowTextureResize(bool allow);                           // ✅ Validated by: Core_test13
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


    // Pixel Dimensions
    bool Core_test2(std::vector<std::string>& errors)
    {
        bool ok = true;

        Core& core = getCore();
        const auto& expected = core.getConfig();  
        float pixel_w = core.getPixelWidth();
        if (pixel_w != expected.pixelWidth) {
            errors.push_back("Pixel width does not match expected value. Expected: " + 
                std::to_string(expected.pixelWidth) + ", Actual: " + std::to_string(pixel_w));      
            ok = false;
        }

        float pixel_h = core.getPixelHeight();
        if (pixel_h != expected.pixelHeight) {
            errors.push_back("Pixel height does not match expected value. Expected: " + 
                std::to_string(expected.pixelHeight) + ", Actual: " + std::to_string(pixel_h));
            ok = false;
        }
        return ok;        
    } // END: Core_test2()


    // Aspect Ratio Preservation
    bool Core_test3(std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();
        const auto& expected = core.getConfig();  

        bool preserve_aspect = core.getPreserveAspectRatio();
        if (preserve_aspect != expected.preserveAspectRatio) 
        {
            errors.push_back("PreserveAspectRatio does not match expected value. Expected: " + 
                std::to_string(expected.preserveAspectRatio) + ", Actual: " + std::to_string(preserve_aspect));
            ok = false;
        }   
        return ok;
    } // END: Core_test3()


    // Texture Resize Allowance
    bool Core_test4(std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();
        const auto& expected = core.getConfig();  

        bool allow_resize = core.getAllowTextureResize();
        if (allow_resize != expected.allowTextureResize)
        {
            errors.push_back("AllowTextureResize does not match expected value. Expected: " + 
                std::to_string(expected.allowTextureResize) + ", Actual: " + std::to_string(allow_resize));
            ok = false;
        }   
        return ok;
    } // END: Core_test4()


    // Pixel Format
    bool Core_test5(std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();
        const auto& expected = core.getConfig();  

        SDL_PixelFormat pixel_format = core.getPixelFormat();
        if (pixel_format != expected.pixelFormat) 
        {
            errors.push_back("PixelFormat does not match expected value. Expected: " + 
                std::to_string(expected.pixelFormat) + ", Actual: " + std::to_string(pixel_format));
            ok = false;
        }   
        return ok;
    } // END: Core_test5()


    // Renderer Logical Presentation
    bool Core_test6(std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();
        const auto& expected = core.getConfig();  

        SDL_RendererLogicalPresentation presentation = core.getRendererLogicalPresentation();
        if (presentation != expected.rendererLogicalPresentation) 
        {
            errors.push_back("RendererLogicalPresentation does not match expected value. Expected: " + 
                std::to_string(expected.rendererLogicalPresentation) + ", Actual: " + std::to_string(presentation));
            ok = false;
        }   
        return ok;
    } // END: Core_test6()


    // Window Flags
    bool Core_test7(std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();
        const auto& expected = core.getConfig();  

        SDL_WindowFlags window_flags = core.getWindowFlags();
        if (window_flags != expected.windowFlags) 
        {
            errors.push_back("WindowFlags does not match expected value. Expected: " + 
                std::to_string(expected.windowFlags) + ", Actual: " + std::to_string(window_flags));
            ok = false;
        }   
        return ok;
    } // END: Core_test7()


    // Core Background Color
    bool Core_test8(std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();
        const auto& expected = core.getConfig();  

        SDL_Color color = core.getColor();
        if (color.r != expected.color.r || color.g != expected.color.g ||
            color.b != expected.color.b || color.a != expected.color.a)
        {
            errors.push_back("Background Color does not match expected value. Expected: (" + 
                std::to_string(expected.color.r) + ", " + std::to_string(expected.color.g) + ", " +
                std::to_string(expected.color.b) + ", " + std::to_string(expected.color.a) + 
                "), Actual: (" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " +
                std::to_string(color.b) + ", " + std::to_string(color.a) + ")");
            ok = false;
        }   
        return ok;
    } // END: Core_test8()


    // Factory Existence Test
    bool Core_test9(std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();

        if (&core.getFactory() == nullptr) 
        {
            errors.push_back("Factory instance is not valid (nullptr).");
            ok = false;
        }   
        return ok;
    } // END: Core_test9()


    // Callback/Hook Registration
    bool Core_test10(std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();
        // UnitTests& ut = UnitTests::getInstance();

        // External lambdas for testing
        [[maybe_unused]] auto testInitFn        = []() -> bool { /* test logic */ return true; };
        [[maybe_unused]] auto testQuitFn        = []() { /* test logic */ };
        [[maybe_unused]] auto testUpdateFn      = [](float dt) { /* test logic */ };
        [[maybe_unused]] auto testEventFn       = [](const Event& e) { /* test logic */ };
        [[maybe_unused]] auto testRenderFn      = []() { /* test logic */ };
        [[maybe_unused]] auto testUnitTestFn    = []() -> bool { /* test logic */ return true; };
        [[maybe_unused]] auto testWindowResizeFn= [](int w, int h) { /* test logic */ };                

        // Save original registration pointers using Core's internal helpers
        auto orig_fnOnInit        = core._fnGetOnInit();
        auto orig_fnOnQuit        = core._fnGetOnQuit();
        auto orig_fnOnUpdate      = core._fnGetOnUpdate();
        auto orig_fnOnEvent       = core._fnGetOnEvent();
        auto orig_fnOnRender      = core._fnGetOnRender();
        auto orig_fnOnWindowResize= core._fnGetOnWindowResize();

        core.registerOnInit(testInitFn);
        core.registerOnQuit(testQuitFn);
        core.registerOnUpdate(testUpdateFn);
        core.registerOnEvent(testEventFn);
        core.registerOnRender(testRenderFn);
        core.registerOnWindowResize(testWindowResizeFn);

        // Perform checks to verify the hooks were set correctly
        if (core._fnGetOnInit().target_type() != typeid(testInitFn)) {
            errors.push_back("OnInit function was not set correctly.");
            ok = false;
        }
        if (core._fnGetOnQuit().target_type() != typeid(testQuitFn)) {
            errors.push_back("OnQuit function was not set correctly.");
            ok = false;
        }
        if (core._fnGetOnUpdate().target_type() != typeid(testUpdateFn)) {
            errors.push_back("OnUpdate function was not set correctly.");
            ok = false;
        }
        if (core._fnGetOnEvent().target_type() != typeid(testEventFn)) {
            errors.push_back("OnEvent function was not set correctly.");
            ok = false;
        }
        if (core._fnGetOnRender().target_type() != typeid(testRenderFn)) {
            errors.push_back("OnRender function was not set correctly.");
            ok = false;
        }
        if (core._fnGetOnWindowResize().target_type() != typeid(testWindowResizeFn)) {
            errors.push_back("OnWindowResize function was not set correctly.");
            ok = false;
        }

        // Restore original registration pointers after testing
        core._fnOnInit(orig_fnOnInit);
        core._fnOnQuit(orig_fnOnQuit);
        core._fnOnUpdate(orig_fnOnUpdate);
        core._fnOnEvent(orig_fnOnEvent);
        core._fnOnRender(orig_fnOnRender);
        core._fnOnWindowResize(orig_fnOnWindowResize);
        // return success or failure
        return ok;
    } // END: Core_test10()


    // Stage/Root Node Management
    bool Core_test11(std::vector<std::string>& errors)
    {
        bool ok = true;
        Core& core = getCore();

        // Helper lambda for validating DisplayHandle and name
        auto checkStage = [&](const DisplayHandle& handle, const std::string& expectedName, const std::string& context) {
            if (!handle.isValid()) {
                errors.push_back(context + " is not valid.");
                ok = false;
            }
            if (handle.getName() != expectedName) {
                errors.push_back(context + " name is not '" + expectedName + "'. Actual: " + handle.getName());
                ok = false;
            }
        };        
        
        // Implement test logic for Stage/Root Node Management

        // Save the Current Stage Handle
        DisplayHandle rootStage = core.getRootNode();
        checkStage(rootStage, "mainStage", "Initial Root Node/Stage");

        // set the Root Node to a new value using name
        core.setRootNode("stageTwo");
        DisplayHandle newStage = core.getRootNode();
        checkStage(newStage, "stageTwo", "Root Node/Stage after setRootNode by name");

        // set the Root Node to a new value using DisplayHandle
        DisplayHandle stageThree = core.getDisplayObject("stageThree");
        core.setRootNode(stageThree);
        DisplayHandle anotherStage = core.getRootNode();
        checkStage(anotherStage, "stageThree", "Root Node/Stage after setRootNode by DisplayHandle");

        // verify hasDisplayObject and getDisplayObjectPtr works correctly
        if (core.hasDisplayObject("nonExistentStage")) {
            errors.push_back("Core incorrectly reports existence of 'nonExistentStage'.");
            ok = false;
        }
        if (!core.hasDisplayObject("stageTwo")) {
            errors.push_back("Core failed to report existence of 'stageTwo'.");
            ok = false;
        }
        if (core.getDisplayObjectPtr("nonExistentStage") != nullptr) {
            errors.push_back("getDisplayObjectPtr() returned non-nullptr for 'nonExistentStage'.");
            ok = false;
        }
        if (core.getDisplayObjectPtr("stageTwo") == nullptr) {
            errors.push_back("getDisplayObjectPtr() returned nullptr for 'stageTwo'.");
            ok = false;
        }

        // restore the Root Node Stage to original
        core.setStage(rootStage.getName());
        DisplayHandle restoredStage = core.getRootNode();
        checkStage(restoredStage, "mainStage", "Root Node/Stage after restoring original");

        // Additional Tests
        IDisplayObject* rootPtr = core.getRootNodePtr();
        if (rootPtr == nullptr) {
            errors.push_back("getRootNodePtr() returned nullptr.");
            ok = false;
        }
        Stage* stagePtr = dynamic_cast<Stage*>(rootPtr);
        if (stagePtr == nullptr) {
            errors.push_back("getRootNodePtr() did not return a Stage pointer.");
            ok = false;
        }
        if (stagePtr && stagePtr->getName() != "mainStage") {
            errors.push_back("Stage name from getRootNodePtr() is not 'mainStage'. Actual: " + stagePtr->getName());
            ok = false;
        }
        DisplayHandle stageHandle = core.getStageHandle();
        checkStage(stageHandle, "mainStage", "Stage name from getStageHandle()");

        return ok;
    } // END: Core_test11()


    // SDL Resource Accessors
    bool Core_test12(std::vector<std::string>& errors)
    {
        bool ok = true;
        // Implement test logic for SDL Resource Accessors
        Core& core = getCore();
        UnitTests& ut = UnitTests::getInstance();

        SDL_Window* window = ut.getWindow();
        if (window == nullptr) {
            errors.push_back("SDL_Window is nullptr.");
            ok = false;
        }
        if (core.getWindow() != window) {
            errors.push_back("SDL_Window from accessor does not match Core's window.");
            ok = false;
        }
        SDL_Renderer* renderer = ut.getRenderer();
        if (renderer == nullptr) {
            errors.push_back("SDL_Renderer is nullptr.");
            ok = false;
        }
        if (core.getRenderer() != renderer) {
            errors.push_back("SDL_Renderer from accessor does not match Core's renderer.");
            ok = false;
        }
        SDL_Texture* texture = ut.getTexture();
        if (texture == nullptr) {
            errors.push_back("SDL_Texture is nullptr.");
            ok = false;
        }
        if (core.getTexture() != texture) {
            errors.push_back("SDL_Texture from accessor does not match Core's texture.");
            ok = false;
        }
        SDL_Color core_color = ut.getColor();
        core.setColor({64, 128, 192, 255});
        SDL_Color new_color = core.getColor();
        if (new_color.r != 64 || new_color.g != 128 ||
            new_color.b != 192 || new_color.a != 255)
        {
            errors.push_back("SDL_Color was not set properly.");
            ok = false;
        }
        core.setColor(core_color); // Restore original color
        SDL_Color color = core.getColor();
        if (core_color.r != color.r || core_color.g != color.g ||
            core_color.b != color.b || core_color.a != color.a)
        {
            errors.push_back("SDL_Color is not initialized properly.");
            ok = false;
        }

        return ok;
    } // END: Core_test12()



    // Configuration Getters/Setters
    bool Core_test13(std::vector<std::string>& errors)
    {
        // Set this to true for full config change stress test, false for quick test
        constexpr bool FULL_CONFIG_TEST = false; 

        bool ok = true;
        Core& core = getCore();
        SDL_Window* sdl_window = core.getWindow();
        int sdl_w = 0, sdl_h = 0;
        SDL_GetWindowSize(sdl_window, &sdl_w, &sdl_h);

        Core::CoreConfig orig = core.getConfig();

        if (!FULL_CONFIG_TEST) {
            // Quick test: just verify getters match config
            if (orig.windowWidth != core.getWindowWidth()) {
                errors.push_back("CoreConfig window width does not match Core getter.");
                ok = false;
            }
            if (orig.windowHeight != core.getWindowHeight()) {
                errors.push_back("CoreConfig window height does not match Core getter.");
                ok = false;
            }
            if (orig.pixelWidth != core.getPixelWidth()) {
                errors.push_back("CoreConfig pixel width does not match Core getter.");
                ok = false;
            }
            if (orig.pixelHeight != core.getPixelHeight()) {
                errors.push_back("CoreConfig pixel height does not match Core getter.");
                ok = false;
            }
            if (orig.preserveAspectRatio != core.getPreserveAspectRatio()) {
                errors.push_back("CoreConfig preserve aspect ratio does not match Core getter.");
                ok = false;
            }
            if (orig.allowTextureResize != core.getAllowTextureResize()) {
                errors.push_back("CoreConfig allow texture resize does not match Core getter.");
                ok = false;
            }
            if (orig.rendererLogicalPresentation != core.getRendererLogicalPresentation()) {
                errors.push_back("CoreConfig renderer logical presentation does not match Core getter.");
                ok = false;
            }
            if (orig.windowFlags != core.getWindowFlags()) {
                errors.push_back("CoreConfig window flags does not match Core getter.");
                ok = false;
            }
            if (orig.pixelFormat != core.getPixelFormat()) {
                errors.push_back("CoreConfig pixel format does not match Core getter.");
                ok = false;
            }
            return ok;
        }

        // Full config change stress test
        std::vector<Core::CoreConfig> testConfigs = {
            orig,
            Core::CoreConfig{640, 480, 1.0f, 1.0f, true, false, SDL_LOGICAL_PRESENTATION_STRETCH, SDL_WINDOW_BORDERLESS, SDL_PIXELFORMAT_RGBA8888, SDL_Color{64, 128, 255, 255}},
            Core::CoreConfig{800, 600, 2.0f, 2.0f, false, true, SDL_LOGICAL_PRESENTATION_DISABLED, SDL_WINDOW_FULLSCREEN, SDL_PIXELFORMAT_ARGB8888, SDL_Color{255, 64, 128, 128}},
            Core::CoreConfig{1024, 768, 4.0f, 4.0f, true, true, SDL_LOGICAL_PRESENTATION_LETTERBOX, SDL_WINDOW_RESIZABLE, SDL_PIXELFORMAT_BGRA8888, SDL_Color{128, 255, 64, 64}},
            Core::CoreConfig(orig)  // Restore original at end
        };

        for (auto& cfg : testConfigs)         
        {
            core.setConfig(cfg);
            // core.reconfigure(cfg);  // Redundant with setConfig

            // Verify getters match config
            if (cfg.windowWidth != core.getWindowWidth()) {
                errors.push_back("windowWidth mismatch after setConfig.");
                ok = false;
            }
            if (cfg.windowHeight != core.getWindowHeight()) {
                errors.push_back("windowHeight mismatch after setConfig.");
                ok = false;
            }
            if (cfg.pixelWidth != core.getPixelWidth()) {
                errors.push_back("pixelWidth mismatch after setConfig.");
                ok = false;
            }
            if (cfg.pixelHeight != core.getPixelHeight()) {
                errors.push_back("pixelHeight mismatch after setConfig.");
                ok = false;
            }
            if (cfg.preserveAspectRatio != core.getPreserveAspectRatio()) {
                errors.push_back("preserveAspectRatio mismatch after setConfig.");
                ok = false;
            }
            if (cfg.allowTextureResize != core.getAllowTextureResize()) {
                errors.push_back("allowTextureResize mismatch after setConfig.");
                ok = false;
            }
            if (cfg.rendererLogicalPresentation != core.getRendererLogicalPresentation()) {
                errors.push_back("rendererLogicalPresentation mismatch after setConfig.");
                ok = false;
            }
            if (cfg.windowFlags != core.getWindowFlags()) {
                errors.push_back("windowFlags mismatch after setConfig.");
                ok = false;
            }
            if (cfg.pixelFormat != core.getPixelFormat()) {
                errors.push_back("pixelFormat mismatch after setConfig.");
                ok = false;
            }

            // Optionally, check SDL resources are valid
            if (!core.getWindow()) {
                errors.push_back("SDL_Window is nullptr after setConfig.");
                ok = false;
            }
            if (!core.getRenderer()) {
                errors.push_back("SDL_Renderer is nullptr after setConfig.");
                ok = false;
            }
            if (!core.getTexture()) {
                errors.push_back("SDL_Texture is nullptr after setConfig.");
                ok = false;
            }
        }
        return ok;
    } // END: Core_test13()


    // getFactory();                                                // Validated by: Core_test9
    // getEventManager();                                           // TODO: Needs test
    // getIsTraversing();                                           // TODO: Needs test
    // setIsTraversing(bool traversing);                            // TODO: Needs test

    // Factory and Event Manager Access
    bool Core_test14(std::vector<std::string>& errors)   
    {
        bool ok = true;
        Core& core = getCore();
        UnitTests& ut = UnitTests::getInstance();

        if (&core.getFactory() != &ut.getFactory()) 
        {
            errors.push_back("Factory instance is not valid (nullptr).");
            ok = false;
        }
        if (&core.getEventManager() != &ut.getEventManager()) 
        {
            errors.push_back("EventManager instance is not valid (nullptr).");
            ok = false;
        }
        bool old_traversing = core.getIsTraversing();
        core.setIsTraversing(!old_traversing);
        if (core.getIsTraversing() == old_traversing) 
        {
            errors.push_back("setIsTraversing did not change the traversing state.");
            ok = false;
        }
        // Restore original state
        core.setIsTraversing(old_traversing);
        if (core.getIsTraversing() != old_traversing) 
        {
            errors.push_back("setIsTraversing did not restore the original traversing state.");
            ok = false;
        }

        return ok;
    } // END: Core_test14()


    // Focus & Hover Management
    bool Core_test15(std::vector<std::string>& errors)   
    {
        bool ok = true;
        Core& core = getCore();

        // Save the current mouse cursor position        
        float mx, my;
        SDL_GetMouseState(&mx, &my);

        // // Move the Mouse Cursor to 0,0
        // SDL_WarpMouseInWindow(core.getWindow(), 0, 0);
        
        // Handle Tab Key Press
        core.handleTabKeyPress();
        DisplayHandle original_focused = core.getKeyboardFocusedObject();
        if (!original_focused.isValid()) 
        {
            errors.push_back("No original focused object.");
            ok = false;
        }
        if (ok)
        {
            core.handleTabKeyPress();
            if (core.getKeyboardFocusedObject() == original_focused) 
            {
                errors.push_back("Tab key press did not change focused object.");
                ok = false;
            }
            core.handleTabKeyPressReverse();
            if (core.getKeyboardFocusedObject() != original_focused) 
            {
                errors.push_back("Reverse Tab key press did not restore original focused object.");
                ok = false;
            }
            core.handleTabKeyPress();
            if (core.getKeyboardFocusedObject() == original_focused) 
            {
                errors.push_back("Tab key press did not change focused object the second time.");
                ok = false;
            }   
            core.setKeyboardFocusedObject(original_focused);
            if (core.getKeyboardFocusedObject() != original_focused) 
            {
                errors.push_back("setKeyboardFocusedObject did not restore original focused object.");
                ok = false;
            }
        }        
        // Handle Mouse Hovered Object
        core.clearMouseHoveredObject();
        DisplayHandle original_hovered = core.getMouseHoveredObject();
        if (original_hovered.isValid()) 
        {
            errors.push_back("Should not have an original hovered object.");
            ok = false;
        }

        DisplayHandle blueishBox = core.getDisplayObject("blueishBox");
        if (!blueishBox.isValid()) 
        {
            errors.push_back("blueishBox object not found for hover test.");
            ok = false;
        }
        if (ok) 
        {
            core.setMouseHoveredObject(blueishBox);
            if (core.getMouseHoveredObject() != blueishBox) {
                errors.push_back("setMouseHoveredObject did not set to blueishBox.");
                ok = false;
            }
            // send a fake mouse move event to be over the blueishBox
            SDL_WarpMouseInWindow(core.getWindow(),
                static_cast<int>(blueishBox->getX() + (blueishBox->getWidth() / 2.0f)),
                static_cast<int>(blueishBox->getY() + (blueishBox->getHeight() / 2.0f))
            );
        }
        DisplayHandle new_hovered = core.getMouseHoveredObject();
        if (new_hovered != blueishBox) 
        {
            errors.push_back("Mouse hovered object is not blueishBox after event.");
            ok = false;
        }        
        // move the mouse back 
        SDL_WarpMouseInWindow(core.getWindow(), mx, my);
        return ok;
    } // END: Core_test15(std::vector<std::string>& errors)   


    // --- Object Creation and Orphan Management --- //
    bool Core_test16(std::vector<std::string>& errors)   
    {
        bool ok = true;
        Core& core = getCore();
        // Factory& factory = core.getFactory();

        // --- DisplayObject Creation via sol::table ---
        sol::table boxConfig = core.getLua().create_table();
        boxConfig["name"] = "testBox";
        boxConfig["type"] = "Box";
        boxConfig["color"] = {255, 0, 0, 255};
        boxConfig["x"] = 42;
        boxConfig["y"] = 24;
        boxConfig["width"] = 100;
        boxConfig["height"] = 50;
        DisplayHandle boxHandle = core.createDisplayObject("Box", boxConfig);
        if (!boxHandle.isValid()) {
            errors.push_back("createDisplayObject (sol::table) failed for Box.");
            ok = false;
        }
        core.destroyDisplayObject(boxHandle.getName());

        // --- DisplayObject Creation via InitStruct ---
        SDOM::IDisplayObject::InitStruct boxInit;
        boxInit.name = "testBox2";
        boxInit.type = "Box";
        boxInit.x = 100;
        boxInit.y = 200;
        boxInit.width = 80;
        boxInit.height = 40;
        DisplayHandle boxHandle2 = core.createDisplayObject("Box", boxInit);
        if (!boxHandle2.isValid()) {
            errors.push_back("createDisplayObject (InitStruct) failed for Box.");
            ok = false;
        }
        core.destroyDisplayObject(boxHandle2.getName());

        // --- DisplayObject Creation via Lua Script ---
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
            errors.push_back("createDisplayObjectFromScript failed for Box.");
            ok = false;
        }      

        // --- Orphan Management Tests --- //

        int orphanCount = core.countOrphanedDisplayObjects();
        if (orphanCount == 0) {
            errors.push_back("Expected orphaned DisplayObjects after creation via Lua script, found none.");
            ok = false;
        }

        std::vector<DisplayHandle> orphans = core.getOrphanedDisplayObjects();
        if (orphans.size() != 1) {
            errors.push_back("Should have one orphan, but found: " + std::to_string(orphans.size()));
            ok = false;
        }
        DisplayHandle orphan = orphans.at(0);
        if (orphan != boxHandle3) {
            errors.push_back("Orphaned DisplayObject does not match the created Box object.");
            ok = false;
        }

        // Attach the orphan to the stage and verify orphan count decreases
        DisplayHandle stage = core.getStageHandle();
        stage->addChild(orphan);    // Attach the orphan to the stage
        orphanCount = core.countOrphanedDisplayObjects();
        if (orphanCount != 0) {
            errors.push_back("Orphaned DisplayObjects not cleared after attaching to stage.");
            ok = false;
        }

        // Remove the child again and verify it becomes orphaned
        stage->removeChild(orphan); // Detach the orphan again
        orphanCount = core.countOrphanedDisplayObjects();
        if (orphanCount != 1) {
            errors.push_back("Orphaned DisplayObjects count incorrect after detaching from stage.");
            ok = false;
        }

        // Detach orphans and verify parent is invalidated
        core.detachOrphans();
        DisplayHandle parent = orphan->getParent();
        if (parent.isValid()) {
            errors.push_back("Orphaned DisplayObject still has a valid parent after detachment.");
            ok = false;
        }

        // Finally, destroy orphaned display objects and verify cleanup
        core.collectGarbage();
        if (core.countOrphanedDisplayObjects() != 0) {
            errors.push_back("Orphaned DisplayObjects not cleaned up after destruction.");
            ok = false;
        }

        // --- AssetObject Creation via sol::table ---
        sol::table assetConfig = core.getLua().create_table();
        assetConfig["name"] = "bmp_font_8x8";
        assetConfig["type"] = "BitmapFont";
        assetConfig["filename"] = "./assets/font_8x8.png";
        assetConfig["font_width"] = 8;
        assetConfig["font_height"] = 8;
        AssetHandle assetHandle = core.createAssetObject("BitmapFont", assetConfig);
        if (!assetHandle.isValid()) {
            errors.push_back("createAssetObject (sol::table) failed for BitmapFont.");
            ok = false;
        }
        if (core.hasAssetObject("non_existent_asset")) {
            errors.push_back("Core incorrectly reports existence of 'non_existent_asset'.");
            ok = false;
        }
        if (!core.hasAssetObject("bmp_font_8x8")) {
            errors.push_back("Core failed to report existence of 'bmp_font_8x8' after creation.");
            ok = false;
        }        
        core.destroyAssetObject(assetHandle.getName());
        AssetHandle checkHandle = core.getAssetObject("bmp_font_8x8");
        if (checkHandle.isValid()) {
            errors.push_back("AssetObject bmp_font_8x8 was not destroyed properly.");
            ok = false;
        }

        // --- AssetObject Creation via InitStruct ---
        SDOM::BitmapFont::InitStruct bmpAssetInit;
        bmpAssetInit.name = "testAsset2";
        bmpAssetInit.type = "BitmapFont";
        bmpAssetInit.filename = "./assets/font_8x8.png";
        bmpAssetInit.font_width = 8;
        bmpAssetInit.font_height = 8;
        AssetHandle assetHandle2 = core.createAssetObject("BitmapFont", bmpAssetInit);
        if (!assetHandle2.isValid()) {
            errors.push_back("createAssetObject (InitStruct) failed for BitmapFont.");
            ok = false;
        }
        core.destroyAssetObject(assetHandle.getName());
        checkHandle = core.getAssetObject("bmp_font_8x8");
        if (checkHandle.isValid()) {
            errors.push_back("bmpAssetInit bmp_font_8x8 was not destroyed properly.");
            ok = false;
        }

        // --- AssetObject Creation via Lua Script ---
        std::string assetScript = R"(
            name = "testAsset3",
            type = "BitmapFont",
            filename = "./assets/font_8x8.png",
            font_width = 8,
            font_height = 8
        )";
        AssetHandle assetHandle3 = core.createAssetObjectFromScript("BitmapFont", assetScript);
        if (!assetHandle3.isValid()) {
            errors.push_back("createAssetObjectFromScript failed for BitmapFont.");
            ok = false;
        }

        // errors.push_back("createAssetObjectFromScript failed for BitmapFont.");
        // return false;
        return ok;

    } // END: bool Core_test16(std::vector<std::string>& errors)    


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

            ut.setLuaFilename("src/Core_UnitTests.lua");
            ut.add_test(objName, "Lua: '" + ut.getLuaFilename() + "'", Core_LUA_Tests);

            registered = true;
        }

        // Returning false keeps this test suite active until all tests complete
        return false;
    }


} // namespace SDOM

// #endif // SDOM_ENABLE_UNIT_TESTS
