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
        The followinng unit tests should validate the functionality of these Methods of the Core class.


        // --- Callback/Hook Registration --- //
        registerOnInit(std::function<bool()>                        // Validated by: Core_test10
        registerOnQuit(std::function<void()>)                       // Validated by: Core_test10
        registerOnUpdate(std::function<void(float)>)                // Validated by: Core_test10
        registerOnEvent(std::function<void(const Event&)>)          // Validated by: Core_test10
        registerOnRender(std::function<void()>)                     // Validated by: Core_test10
        registerOnUnitTest(std::function<bool()>)                   // (Not directly tested, but similar to Core_test10)
        registerOnWindowResize(std::function<void(int, int)>)       // Validated by: Core_test10

        // --- Lua Registration Internal Helpers --- //
        _fnOnInit(std::function<bool()> fn)                         // Validated by: Core_test10
        _fnOnQuit(std::function<void()> fn)                         // Validated by: Core_test10
        _fnOnUpdate(std::function<void(float)> fn)                  // Validated by: Core_test10
        _fnOnEvent(std::function<void(const Event&)> fn)            // Validated by: Core_test10
        _fnOnRender(std::function<void()> fn)                       // Validated by: Core_test10
        _fnOnUnitTest(std::function<bool()> fn)                     // (Not directly tested, but similar to Core_test10)
        _fnOnWindowResize(std::function<void(int, int)> fn)         // Validated by: Core_test10
        _fnGetOnInit()                                              // Validated by: Core_test10
        _fnGetOnQuit()                                              // Validated by: Core_test10
        _fnGetOnUpdate()                                            // Validated by: Core_test10
        _fnGetOnEvent()                                             // Validated by: Core_test10
        _fnGetOnRender()                                            // Validated by: Core_test10
        _fnGetOnUnitTest()                                          // (Not directly tested, but similar to Core_test10)
        _fnGetOnWindowResize()                                      // Validated by: Core_test10

        // --- Stage/Root Node Management --- //
        setRootNode(const std::string& name)                        // Validated by: Core_test11
        setRootNode(const DisplayHandle& handle)                    // Validated by: Core_test11
        setStage(const std::string& name)                           // Validated by: Core_test11
        getStage()                                                  // Validated by: Core_test11 (via getStageHandle)
        getRootNodePtr()                                            // Validated by: Core_test11
        getRootNode()                                               // Validated by: Core_test11
        getStageHandle()                                            // Validated by: Core_test11

        // --- Window/Renderer/Texture/Config --- //
        getWindow()                                                 // Validated by: Core_test1
        SDL_GetWindowSize                                           // Validated by: Core_test1
        getPixelWidth()                                             // Validated by: Core_test2
        getPixelHeight()                                            // Validated by: Core_test2
        getPreserveAspectRatio()                                    // Validated by: Core_test3
        getAllowTextureResize()                                     // Validated by: Core_test4
        getPixelFormat()                                            // Validated by: Core_test5
        getRendererLogicalPresentation()                            // Validated by: Core_test6
        getWindowFlags()                                            // Validated by: Core_test7
        getColor()                                                  // Validated by: Core_test8
        getConfig()                                                 // Used in: Core_test2, Core_test3, Core_test4, Core_test5, Core_test6, Core_test7, Core_test8

        // --- SDL Resource Accessors --- //
        getWindow()                                                 // Validated by: Core_test12
        getRenderer()                                               // Validated by: Core_test12
        getTexture()                                                // Validated by: Core_test12
        getColor()                                                  // Validated by: Core_test12
        setColor(const SDL_Color&)                                  // Validated by: Core_test12

        // --- Configuration Getters --- //
        getConfig();                          
        getWindowWidth();                     
        getWindowHeight();                    
        getPixelWidth();                      
        getPixelHeight();                     
        getPreserveAspectRatio();             
        getAllowTextureResize();              
        getRendererLogicalPresentation();     
        getWindowFlags();                     
        getPixelFormat();   

        // --- Configuration Setters --- //
        setConfig(CoreConfig& config);
        setWindowWidth(float width);
        setWindowHeight(float height);
        setPixelWidth(float width);
        setPixelHeight(float height);
        setPreserveAspectRatio(bool preserve);
        setAllowTextureResize(bool allow);
        setRendererLogicalPresentation(presentation);
        setWindowFlags(SDL_WindowFlags flags);
        setPixelFormat(SDL_PixelFormat format);

        // --- Factory & EventManager Access --- //
        getFactory();                // Validated by: Core_test9
        getEventManager();
        getIsTraversing();
        setIsTraversing(bool traversing);

        // --- Focus & Hover Management --- //
        handleTabKeyPress();
        handleTabKeyPressReverse();
        setKeyboardFocusedObject(DisplayHandle obj);
        getKeyboardFocusedObject();
        clearKeyboardFocusedObject();
        setMouseHoveredObject(DisplayHandle obj);
        getMouseHoveredObject();

        // --- Lua State Access --- //
        getLua();


        //////////////////////////////
        // --- Factory Wrappers --- //
        //////////////////////////////

        // --- Object Creation --- //
        createDisplayObject(const std::string& typeName, const sol::table& config);
        createDisplayObject(const std::string& typeName, const SDOM::IDisplayObject::InitStruct& init);
        createDisplayObjectFromScript(const std::string& typeName, const std::string& luaScript);

        createAssetObject(const std::string& typeName, const sol::table& config);
        createAssetObject(const std::string& typeName, const SDOM::IAssetObject::InitStruct& init);
        createAssetObjectFromScript(const std::string& typeName, const std::string& luaScript);

        // --- Object Lookup --- //
        getDisplayObjectPtr(const std::string& name);
        getDisplayObject(const std::string& name);
        getDisplayObjectHandle(const std::string& name);
        hasDisplayObject(const std::string& name);

        getAssetObjectPtr(const std::string& name);
        getAssetObject(const std::string& name);
        hasAssetObject(const std::string& name);

        // --- Display Object Management --- //
        addDisplayObject(const std::string& name, std::unique_ptr<IDisplayObject> displayObject);
        destroyDisplayObject(const std::string& name);

        // --- Orphan Management --- //
        countOrphanedDisplayObjects();
        getOrphanedDisplayObjects();
        destroyOrphanedDisplayObjects();
        detachOrphans();

        // --- Future Child Management --- //
        attachFutureChildren();
        addToOrphanList(const DisplayHandle& orphan);
        addToFutureChildrenList(const DisplayHandle& child, const DisplayHandle& parent, bool useWorld=false, int worldX=0, int worldY=0);

        // --- Utility Methods --- //
        listDisplayObjectNames();
        clearFactory();
        printObjectRegistry();

        // --- Forwarding helpers to Factory for type-level introspection --- //
        getPropertyNamesForType(const std::string& typeName);
        getCommandNamesForType(const std::string& typeName);
        getFunctionNamesForType(const std::string& typeName);


****************/

namespace SDOM
{
    // scaffolding for the Core UnitTests
    bool Core_scaffolding(std::vector<std::string>& errors)   
    {
        bool ok = true;
        // Example test logic
        // if (some_condition_fails) {
        //     error += "Description of the failure.";
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
        bool ok = true;
        Core& core = getCore();
        SDL_Window* sdl_window = core.getWindow();
        int sdl_w = 0, sdl_h = 0;
        SDL_GetWindowSize(sdl_window, &sdl_w, &sdl_h);
        std::cout << "SDL_Window Size: " << sdl_w << "x" << sdl_h << std::endl;

        // Save original config
        Core::CoreConfig orig = core.getConfig();

        // Verify Original Window Size Matches SDL_Window
        if (static_cast<int>(core.getWindowWidth()) != sdl_w) {
            errors.push_back("Initial window width does not match SDL_Window width. Expected: " + 
                std::to_string(sdl_w) + ", Actual: " + std::to_string(core.getWindowWidth()));
            ok = false;
        }
        if (static_cast<int>(core.getWindowHeight()) != sdl_h) {
            errors.push_back("Initial window height does not match SDL_Window height. Expected: " + 
                std::to_string(sdl_h) + ", Actual: " + std::to_string(core.getWindowHeight()));
            ok = false;
        }

        // Set and verify window width
        float o_width = core.getWindowWidth();
        core.setWindowWidth(456.0f);
        if (core.getWindowWidth() != 456.0f) {
            errors.push_back("setWindowWidth/getWindowWidth failed. Expected: 456.0, Actual: " + std::to_string(core.getWindowWidth()));
            ok = false;
        }
        float n_width = core.getWindowWidth();
        if (n_width == o_width) {
            errors.push_back("setWindowWidth did not change the width. Expected different from: " + std::to_string(o_width));
            ok = false;
        }
        core.setWindowWidth(o_width); // Restore original width
        auto n_width_restored = core.getWindowWidth();
        if (n_width_restored != o_width) {
            errors.push_back("Restoring original window width failed. Expected: " + std::to_string(o_width) + ", Actual: " + std::to_string(n_width_restored));
            ok = false;
        }

        SDL_GetWindowSize(sdl_window, &sdl_w, &sdl_h);
        if (sdl_w != static_cast<int>(o_width)) {
            errors.push_back("SDL_Window width does not match after restoration. Expected: " + std::to_string(o_width) + ", Actual: " + std::to_string(sdl_w));
            ok = false;
        }

        // Set and verify window height
        float o_height = core.getWindowHeight();
        core.setWindowHeight(234.0f);
        if (core.getWindowHeight() != 234.0f) {
            errors.push_back("setWindowHeight/getWindowHeight failed.");
            ok = false;
        }
        float n_height = core.getWindowHeight();
        if (n_height == o_height) {
            errors.push_back("setWindowHeight did not change the height.");
            ok = false;
        }
        core.setWindowHeight(o_height); // Restore original height
        if (core.getWindowHeight() != o_height) {
            errors.push_back("Restoring original window height failed.");
            ok = false;
        }

        // Set and verify pixel width
        float o_pixel_width = core.getPixelWidth();
        core.setPixelWidth(3.0f);
        if (core.getPixelWidth() != 3.0f) {
            errors.push_back("setPixelWidth/getPixelWidth failed.");
            ok = false;
        }
        float n_pixel_width = core.getPixelWidth();
        if (n_pixel_width == o_pixel_width) {
            errors.push_back("setPixelWidth did not change the pixel width.");
            ok = false;
        }
        core.setPixelWidth(o_pixel_width); // Restore original pixel width
        if (core.getPixelWidth() != o_pixel_width) {
            errors.push_back("Restoring original pixel width failed.");
            ok = false;
        }

        // Set and verify pixel height
        float o_pixel_height = core.getPixelHeight();
        core.setPixelHeight(3.0f);
        if (core.getPixelHeight() != 3.0f) {
            errors.push_back("setPixelHeight/getPixelHeight failed.");
            ok = false;
        }
        float n_pixel_height = core.getPixelHeight();
        if (n_pixel_height == o_pixel_height) {
            errors.push_back("setPixelHeight did not change the pixel height.");
            ok = false;
        }
        core.setPixelHeight(o_pixel_height); // Restore original pixel height
        if (core.getPixelHeight() != o_pixel_height) {
            errors.push_back("Restoring original pixel height failed.");
            ok = false;
        }

        // Set and verify aspect ratio preservation
        bool o_aspect = core.getPreserveAspectRatio();
        core.setPreserveAspectRatio(!o_aspect);
        if (core.getPreserveAspectRatio() == o_aspect) {
            errors.push_back("setPreserveAspectRatio did not change the value.");
            ok = false;
        }
        core.setPreserveAspectRatio(o_aspect); // Restore original aspect ratio preservation
        if (core.getPreserveAspectRatio() != o_aspect) {
            errors.push_back("Restoring original preserve aspect ratio failed.");
            ok = false;
        }

        // Set and Verify texture resize allowance
        bool o_texture_resize = core.getAllowTextureResize();
        core.setAllowTextureResize(!o_texture_resize);
        if (core.getAllowTextureResize() == o_texture_resize) {
            errors.push_back("setAllowTextureResize did not change the value.");
            ok = false;
        }
        core.setAllowTextureResize(o_texture_resize); // Restore original texture resize allowance
        if (core.getAllowTextureResize() != o_texture_resize) { 
            errors.push_back("Restoring original allow texture resize failed.");
            ok = false;
        }

        // Set and verify renderer logical presentation
        SDL_RendererLogicalPresentation o_presentation = core.getRendererLogicalPresentation();
        core.setRendererLogicalPresentation(SDL_LOGICAL_PRESENTATION_DISABLED);
        if (core.getRendererLogicalPresentation() == o_presentation) {
            errors.push_back("setRendererLogicalPresentation did not change the value.");
            ok = false;
        }
        core.setRendererLogicalPresentation(o_presentation); // Restore original presentation
        if (core.getRendererLogicalPresentation() != o_presentation) {
            errors.push_back("Restoring original renderer logical presentation failed.");
            ok = false;
        }

        // Set and verify window flags
        SDL_WindowFlags o_flags = core.getWindowFlags();
        core.setWindowFlags(SDL_WINDOW_FULLSCREEN);
        if (core.getWindowFlags() == o_flags) {
            errors.push_back("setWindowFlags did not change the value.");
            ok = false;
        }
        core.setWindowFlags(o_flags); // Restore original window flags
        if (core.getWindowFlags() != o_flags) {
            errors.push_back("Restoring original window flags failed.");
            ok = false;
        }

        // Set and verify pixel format
        SDL_PixelFormat o_format = core.getPixelFormat();
        core.setPixelFormat(SDL_PIXELFORMAT_ARGB8888);
        if (core.getPixelFormat() == o_format) {
            errors.push_back("setPixelFormat did not change the value.");
            ok = false;
        }
        core.setPixelFormat(o_format); // Restore original pixel format
        if (core.getPixelFormat() != o_format) {
            errors.push_back("Restoring original pixel format failed.");
            ok = false;
        }

        // Set and verify config as a whole
        core.setConfig(orig); // Restore original config

        return ok;
    }



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

        return ut.run_all("Core");
    }



} // namespace SDOM

// #endif // SDOM_ENABLE_UNIT_TESTS
