// Core_UnitTestss.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS


#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp> 
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_Stage.hpp>

#include <string>
#include <vector>
#include <functional>

#include "UnitTests.hpp"

/***************

    Core Unit Tests:
    The following unit tests should validate the functionality of these Methods of the Core class.

    // --- Callback/Hook Registration --- //
    registerOnInit(std::function<bool()>)                        // Validated by: Core_test10
    registerOnQuit(std::function<void()>)                        // Validated by: Core_test10
    registerOnUpdate(std::function<void(float)>)                 // Validated by: Core_test10
    registerOnEvent(std::function<void(const Event&)>)           // Validated by: Core_test10
    registerOnRender(std::function<void()>)                      // Validated by: Core_test10
    registerOnUnitTest(std::function<bool()>)                    // (Not directly tested, but similar to Core_test10)
    registerOnWindowResize(std::function<void(int, int)>)        // Validated by: Core_test10

    // --- Lua Registration Internal Helpers --- //
    _fnOnInit(std::function<bool()> fn)                          // Validated by: Core_test10
    _fnOnQuit(std::function<void()> fn)                          // Validated by: Core_test10
    _fnOnUpdate(std::function<void(float)> fn)                   // Validated by: Core_test10
    _fnOnEvent(std::function<void(const Event&)> fn)             // Validated by: Core_test10
    _fnOnRender(std::function<void()> fn)                        // Validated by: Core_test10
    _fnOnUnitTest(std::function<bool()> fn)                      // (Not directly tested, but similar to Core_test10)
    _fnOnWindowResize(std::function<void(int, int)> fn)          // Validated by: Core_test10
    _fnGetOnInit()                                               // Validated by: Core_test10
    _fnGetOnQuit()                                               // Validated by: Core_test10
    _fnGetOnUpdate()                                             // Validated by: Core_test10
    _fnGetOnEvent()                                              // Validated by: Core_test10
    _fnGetOnRender()                                             // Validated by: Core_test10
    _fnGetOnUnitTest()                                           // (Not directly tested, but similar to Core_test10)
    _fnGetOnWindowResize()                                       // Validated by: Core_test10

    // --- Stage/Root Node Management --- //
    setRootNode(const std::string& name)                         // Validated by: Core_test11
    setRootNode(const DisplayHandle& handle)                     // Validated by: Core_test11
    setStage(const std::string& name)                            // Validated by: Core_test11
    getStage()                                                   // Validated by: Core_test11 (via getStageHandle)
    getRootNodePtr()                                             // Validated by: Core_test11
    getRootNode()                                                // Validated by: Core_test11
    getStageHandle()                                             // Validated by: Core_test11

    // --- Window/Renderer/Texture/Config --- //
    getWindow()                                                  // Validated by: Core_test1, Core_test12
    SDL_GetWindowSize                                            // Validated by: Core_test1
    getPixelWidth()                                              // Validated by: Core_test2, Core_test13
    getPixelHeight()                                             // Validated by: Core_test2, Core_test13
    getPreserveAspectRatio()                                     // Validated by: Core_test3, Core_test13
    getAllowTextureResize()                                      // Validated by: Core_test4, Core_test13
    getPixelFormat()                                             // Validated by: Core_test5, Core_test13
    getRendererLogicalPresentation()                             // Validated by: Core_test6, Core_test13
    getWindowFlags()                                             // Validated by: Core_test7, Core_test13
    getColor()                                                   // Validated by: Core_test8, Core_test12
    getConfig()                                                  // Used in: Core_test2, Core_test3, Core_test4, Core_test5, Core_test6, Core_test7, Core_test8, Core_test13
    setColor(const SDL_Color&)                                   // Validated by: Core_test12

    // --- SDL Resource Accessors --- //
    getWindow()                                                  // Validated by: Core_test12
    getRenderer()                                                // Validated by: Core_test12
    getTexture()                                                 // Validated by: Core_test12

    // --- Configuration Getters/Setters --- //
    getConfig();                                                 // Validated by: Core_test13
    getWindowWidth();                                            // Validated by: Core_test13
    getWindowHeight();                                           // Validated by: Core_test13
    getPixelWidth();                                             // Validated by: Core_test13
    getPixelHeight();                                            // Validated by: Core_test13
    getPreserveAspectRatio();                                    // Validated by: Core_test13
    getAllowTextureResize();                                     // Validated by: Core_test13
    getRendererLogicalPresentation();                            // Validated by: Core_test13
    getWindowFlags();                                            // Validated by: Core_test13
    getPixelFormat();                                            // Validated by: Core_test13

    setConfig(CoreConfig& config);                               // Validated by: Core_test13
    setWindowWidth(float width);                                 // Validated by: Core_test13
    setWindowHeight(float height);                               // Validated by: Core_test13
    setPixelWidth(float width);                                  // Validated by: Core_test13
    setPixelHeight(float height);                                // Validated by: Core_test13
    setPreserveAspectRatio(bool preserve);                       // Validated by: Core_test13
    setAllowTextureResize(bool allow);                           // Validated by: Core_test13
    setRendererLogicalPresentation(presentation);                // Validated by: Core_test13
    setWindowFlags(SDL_WindowFlags flags);                       // Validated by: Core_test13
    setPixelFormat(SDL_PixelFormat format);                      // Validated by: Core_test13

    // --- Factory & EventManager Access --- //
    getFactory();                                                // Validated by: Core_test9, Core_test14
    getEventManager();                                           // Validated by: Core_test14
    getIsTraversing();                                           // Validated by: Core_test14
    setIsTraversing(bool traversing);                            // Validated by: Core_test14

    // --- Focus & Hover Management --- //
    handleTabKeyPress();                                         // TODO: Needs test
    handleTabKeyPressReverse();                                  // TODO: Needs test
    setKeyboardFocusedObject(DisplayHandle obj);                 // TODO: Needs test
    getKeyboardFocusedObject();                                  // TODO: Needs test
    clearKeyboardFocusedObject();                                // TODO: Needs test
    setMouseHoveredObject(DisplayHandle obj);                    // TODO: Needs test
    getMouseHoveredObject();                                     // TODO: Needs test

    // --- Lua State Access --- //
    getLua();                                                    // TODO: Needs test

    //////////////////////////////
    // --- Factory Wrappers --- //
    //////////////////////////////

    // --- Object Creation --- //
    createDisplayObject(const std::string& typeName, const sol::table& config);         // TODO: Needs test
    createDisplayObject(const std::string& typeName,                                    // TODO: Needs test
        const SDOM::IDisplayObject::InitStruct& init); 
    createDisplayObjectFromScript(const std::string& typeName,                          // TODO: Needs test
        const std::string& luaScript);       

    createAssetObject(const std::string& typeName, const sol::table& config);           // TODO: Needs test
    createAssetObject(const std::string& typeName,                                      // TODO: Needs test
        const SDOM::IAssetObject::InitStruct& init);     
    createAssetObjectFromScript(const std::string& typeName,                            // TODO: Needs test
        const std::string& luaScript);         

    // --- Object Lookup --- //
    getDisplayObjectPtr(const std::string& name);                                       // TODO: Needs test
    getDisplayObject(const std::string& name);                                          // TODO: Needs test
    getDisplayObjectHandle(const std::string& name);                                    // TODO: Needs test
    hasDisplayObject(const std::string& name);                                          // TODO: Needs test

    getAssetObjectPtr(const std::string& name);                                         // TODO: Needs test
    getAssetObject(const std::string& name);                                            // TODO: Needs test
    hasAssetObject(const std::string& name);                                            // TODO: Needs test

    // --- Display Object Management --- //
    addDisplayObject(const std::string& name,                                           // TODO: Needs test
            std::unique_ptr<IDisplayObject> displayObject); 
    destroyDisplayObject(const std::string& name);                                      // TODO: Needs test
    destroyAssetObject(const std::string& name);                                        // TODO: Needs test

    // --- Orphan Management --- //
    countOrphanedDisplayObjects();                                                      // TODO: Needs test
    getOrphanedDisplayObjects();                                                        // TODO: Needs test
    destroyOrphanedDisplayObjects();                                                    // TODO: Needs test
    detachOrphans();                                                                    // TODO: Needs test
    collectGarbage();                                                                   // TODO: Needs test

    // --- Future Child Management --- //
    attachFutureChildren();                                                             // TODO: Needs test
    addToOrphanList(const DisplayHandle& orphan);                                       // TODO: Needs test
    addToFutureChildrenList(const DisplayHandle& child, const DisplayHandle& parent,    // TODO: Needs test
            bool useWorld=false, int worldX=0, int worldY=0);

    // --- Utility Methods --- //
    listDisplayObjectNames();                                                           // TODO: Needs test
    clearFactory();                                                                     // TODO: Needs test
    printObjectRegistry();                                                              // TODO: Needs test

    findAssetByFilename(const std::string& filename, const std::string& typeName = ""); // TODO: Needs test
    findSpriteSheetByParams(const std::string& filename, int spriteW, int spriteH);     // TODO: Needs test
    unloadAllAssetObjects();                                                            // TODO: Needs test
    reloadAllAssetObjects();                                                            // TODO: Needs test

    // --- Forwarding helpers to Factory for type-level introspection --- //
    getPropertyNamesForType(const std::string& typeName);                               // TODO: Needs test
    getCommandNamesForType(const std::string& typeName);                                // TODO: Needs test
    getFunctionNamesForType(const std::string& typeName);                               // TODO: Needs test

****************/


namespace SDOM
{
    // scaffolding for the Core UnitTests
    bool Core_scaffolding(std::vector<std::string>& errors)   
    {
        bool ok = true;
        // Example test logic
        // if (some_condition_fails) {
        //     errors.push_back("Description of the failure.");
        //     ok = false;
        // }
        return ok;
    } // END: bool Core_test0(std::vector<std::string>& errors)


    // Window Dimensions
    bool Core_test1(std::vector<std::string>& errors)   // scaffolding for the Core UnitTests
    {
        bool ok = true;

        Core& core = getCore();
        int actualW = 0, actualH = 0;
        SDL_GetWindowSize(core.getWindow(), &actualW, &actualH);
        if (actualW <= 0) {
            errors.push_back("Window width is not positive.");
            ok = false;
        }   
        if (actualH <= 0) {
            errors.push_back("Window height is not positive.");
            ok = false;
        }
        return ok;
    } // END: bool Core_test1(std::vector<std::string>& errors)


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

        // Set new hooks for testing
        core._fnOnInit(testInitFn);
        core._fnOnQuit(testQuitFn);
        core._fnOnUpdate(testUpdateFn);
        core._fnOnEvent(testEventFn);
        core._fnOnRender(testRenderFn);
        core._fnOnWindowResize(testWindowResizeFn);

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
        DisplayHandle stageThree = getFactory().getDisplayObject("stageThree");
        core.setRootNode(stageThree);
        DisplayHandle anotherStage = core.getRootNode();
        checkStage(anotherStage, "stageThree", "Root Node/Stage after setRootNode by DisplayHandle");

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


    // --- Main Core UnitTests Runner --- //

    bool Core_UnitTests()
    {
        UnitTests& ut = UnitTests::getInstance();
        ut.clear_tests();

        ut.add_test("Scaffolding", Core_scaffolding);
        ut.add_test("Window Dimensions", Core_test1);
        ut.add_test("Pixel Dimensions", Core_test2);
        ut.add_test("Aspect Ratio Preservation", Core_test3);
        ut.add_test("Texture Resize Allowance", Core_test4);
        ut.add_test("Pixel Format", Core_test5);
        ut.add_test("Renderer Logical Presentation", Core_test6);
        ut.add_test("Window Flags", Core_test7);
        ut.add_test("Core Background Color", Core_test8);
        ut.add_test("Factory Existence", Core_test9);
        ut.add_test("Callback/Hook Registration", Core_test10);
        ut.add_test("Stage/Root Node Management", Core_test11);
        ut.add_test("SDL Resource Accessors", Core_test12);
        ut.add_test("Configuration Getters/Setters", Core_test13);
        ut.add_test("Factory and Event Manager Access", Core_test14);

        return ut.run_all("Core");
    }



} // namespace SDOM

// #endif // SDOM_ENABLE_UNIT_TESTS
