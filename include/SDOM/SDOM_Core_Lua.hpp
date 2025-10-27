// SDOM_Core_Lua.hpp --- Lua binding helpers
#pragma once

#include <SDOM/SDOM.hpp>

namespace SDOM
{
    class Event;

    
    // Basic binders (usertype method, CoreForward table entry, and optional global alias)
    // - objHandleType: sol::usertype<Core>&
    // - coreTable: sol::table&
    // - lua: sol::state_view

    // Zero-arg void functions (quit, shutdown, etc.)
    void core_bind_noarg(const std::string& name, std::function<void()> func,
                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Single-table argument: void(const sol::table&)
    void core_bind_table(const std::string& name, std::function<void(const sol::table&)> func,
                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Single-string argument: void(const std::string&)
    void core_bind_string(const std::string& name, std::function<void(const std::string&)> func,
                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Single-bool argument: void(bool)
    void core_bind_bool_arg(const std::string& name, std::function<void(bool)> func,
                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Single-DisplayHandle (or name) argument: void(const DisplayHandle&)
    void core_bind_do_arg(const std::string& name, std::function<void(const DisplayHandle&)> func,
                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Single-sol::object argument: void(const sol::object&)
    void core_bind_object_arg(const std::string& name, std::function<void(const sol::object&)> func,
                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Returners (no-arg) -----------------------------------------------------------

    void core_bind_return_displayobject(const std::string& name, std::function<DisplayHandle()> func,
                                sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return bool: bool ()
    void core_bind_return_bool(const std::string& name, std::function<bool()> func,
                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return string: std::string ()
    void core_bind_return_string(const std::string& name, std::function<std::string()> func,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return float: float ()
    void core_bind_return_float(const std::string& name, std::function<float()> func,
                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return int: int ()
    void core_bind_return_int(const std::string& name, std::function<int()> func,
                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return vector<DisplayHandle>: std::vector<DisplayHandle> ()
    void core_bind_return_vector_do(const std::string& name, std::function<std::vector<DisplayHandle>()> func,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return vector<string>: std::vector<std::string> ()
    void core_bind_return_vector_string(const std::string& name, std::function<std::vector<std::string>()> func,
                                sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // String+table -> DisplayHandle
    void core_bind_string_table_return_do(const std::string& name, std::function<DisplayHandle(const std::string&, const sol::table&)> func,
                                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    void core_bind_string_table_return_asset(const std::string& name, std::function<AssetHandle(const std::string&, const sol::table&)> func,
                                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // String -> DisplayHandle
    void core_bind_string_return_do(const std::string& name, std::function<DisplayHandle(const std::string&)> func,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    void core_bind_string_return_asset(const std::string& name, std::function<AssetHandle(const std::string&)> func,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // String -> bool
    void core_bind_string_return_bool(const std::string& name, std::function<bool(const std::string&)> func,
                                sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Callback registration binders ------------------------------------------------
    // Each accepts the registrar (a function that takes a std::function<...> to register the host callback).

    // bool() callbacks (registerOnInit, registerOnUnitTest)
    void core_bind_callback_bool(const std::string& name, std::function<void(std::function<bool()>)> registrar,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // void() callbacks (registerOnQuit, registerOnRender)
    void core_bind_callback_void(const std::string& name, std::function<void(std::function<void()>)> registrar,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // update(float) callbacks
    void core_bind_callback_update(const std::string& name, std::function<void(std::function<void(float)>)> registrar,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // event(const Event&) callbacks
    void core_bind_callback_event(const std::string& name, std::function<void(std::function<void(const Event&)>)> registrar,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // resize(int,int) callbacks
    void core_bind_callback_resize(const std::string& name, std::function<void(std::function<void(int,int)>)> registrar,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Custom forwarder: string + function -> calls host function(registerOn style)
    void core_bind_string_function_forwarder(const std::string& name, std::function<void(const std::string&, const sol::function&)> func,
                                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Bind a function that accepts either a name (string/table) or a DisplayHandle
    // and dispatches to the appropriate host-side overload (e.g. setStage/setRootNode alias)
    void core_bind_name_or_handle(const std::string& name, std::function<void(const std::string&)> nameFunc, std::function<void(const DisplayHandle&)> handleFunc,
                             sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);
    

         
    // ***************************** //                         
    // --- LUA Wrapper Functions --- //
    // ***************************** //                         

    // --- Main Loop & Event Dispatch --- //
    
    void core_configure_lua(const sol::table& config);
    void core_configureFromFile_lua(const std::string& filename);


    // --- Callback/Hook Registration --- //
    void core_registerOnInit_lua(std::function<bool()> fn);                 // used by core_registerOn()
    void core_registerOnQuit_lua(std::function<void()> fn);                 // used by core_registerOn()
    void core_registerOnUpdate_lua(std::function<void(float)> fn);          // used by core_registerOn()
    void core_registerOnEvent_lua(std::function<void(const Event&)> fn);    // used by core_registerOn()
    void core_registerOnRender_lua(std::function<void()> fn);               // used by core_registerOn()
    void core_registerOnUnitTest_lua(std::function<bool()> fn);             // used by core_registerOn()
    void core_registerOnWindowResize_lua(std::function<void(int, int)> fn); // used by core_registerOn()

    // Generic registration helper: registerOn("Init"|"Update"|...) from Lua
    void core_registerOn_lua(const std::string& name, const sol::function& f);

    // --- Stage/Root Node Management --- //
    void setRootNodeByName_lua(const std::string& name);
    void setRootNode_lua(const DisplayHandle& handle);
    void setStageByName_lua(const std::string& name);
    void setStage_lua(const DisplayHandle& handle);
    DisplayHandle getRoot_lua();
    DisplayHandle getStage_lua(); 

    // --- Factory & EventManager Access --- //
    bool getIsTraversing_lua();
    void setIsTraversing_lua(bool traversing);

    // --- Object Creation and Lookup--- //
    DisplayHandle createDisplayObject_lua(const std::string& typeName, const sol::table& config);
    DisplayHandle getDisplayObject_lua(const std::string& name);
    bool hasDisplayObject_lua(const std::string& name); 
    AssetHandle createAssetObject_lua(const std::string& typeName, const sol::table& config);
    AssetHandle getAssetObject_lua(const std::string& name);
    bool hasAssetObject_lua(const std::string& name);

    // --- Focus & Hover Management --- //
    void doTabKeyPressForward_lua();
    void doTabKeyPressReverse_lua();
    void setKeyboardFocusedObject_lua(const DisplayHandle& handle);
    DisplayHandle getKeyboardFocusedObject_lua();
    void setMouseHoveredObject_lua(const DisplayHandle& handle);
    DisplayHandle getMouseHoveredObject_lua();

    // --- Window Title & Timing --- //
    std::string getWindowTitle_lua();
    void setWindowTitle_lua(const std::string& title);
    float getElapsedTime_lua();	 // alias getDeltaTime()

    // --- Event helpers (exposed to Lua) --- //
    void pumpEventsOnce_lua();
    void pushMouseEvent_lua(const sol::object& args);
    void pushKeyboardEvent_lua(const sol::object& args);

    // --- Orphan / Future Child Management --- //
    void destroyDisplayObject_lua(const std::string& name);
    void destroyDisplayObject_any_lua(const sol::object& spec);
    int countOrphanedDisplayObjects_lua();
    std::vector<DisplayHandle> getOrphanedDisplayObjects_lua();
    // void destroyOrphanedDisplayObjects_lua();  // aliases:  "destroyOrphanedObjects" and "destroyOrphans"
    void collectGarbage_lua();

    // --- Future Child Management (exposed) --- //
    void attachFutureChildren_lua();
    void addToOrphanList_lua(const DisplayHandle& orphan);
    void addToFutureChildrenList_lua(const sol::object& args);

    // --- Factory utilities exposed on Core --- //
    void clearFactory_lua();
    AssetHandle findAssetByFilename_lua(const sol::object& spec);
    AssetHandle findSpriteSheetByParams_lua(const sol::object& spec);
    void unloadAllAssetObjects_lua();
    void reloadAllAssetObjects_lua();

    // --- Utility Methods --- //
    std::vector<std::string> listDisplayObjectNames_lua();
    void printObjectRegistry_lua();

} // namespace SDOM


    // // --- Callback/Hook Registration --- //
    // registerOn(std::string name, sol::function f)                // ✅ Verified in config.lua

    // // --- Lua Registration Internal Helpers --- //

    // // --- Stage/Root Node Management --- //
    // setRootNode(const std::string& name)                         // ✅ Validated by: Core_test11; Lua: Core_UnitTests.lua:test_stage_root
    // setRootNode(const DisplayHandle& handle)                     // ✅ Validated by: Core_test11; Lua: (planned)
    // setStage(const std::string& name)                            // ✅ Validated by: Core_test11; Lua: Core_UnitTests.lua:test_stage_root
    // setStage(const DisplayHandle& handle)                        // ✅ Validated by: Core_test11; Lua: Core_UnitTests.lua:test_stage_root
    // getStage()                                                   // ✅ Validated by: Core_test11; Lua: Core_UnitTests.lua:test_lookup
    // getRootNodePtr()                                             // ✅ Validated by: Core_test11 (C++ only)
    // getRootNode()                                                // ✅ Validated by: Core_test11; Lua: Core_UnitTests.lua:test_stage_root
    // getStageHandle()                                             // ✅ Validated by: Core_test11

    // // --- Window/Renderer/Texture/Config --- //
    // getWindow()                                                  // ✅ Validated by: Core_test1, Core_test12
    // SDL_GetWindowSize                                            // ✅ Validated by: Core_test1
    // getPixelWidth()                                              // ✅ Validated by: Core_test2, Core_test13; Lua: Core_UnitTests.lua:test_config_accessors
    // getPixelHeight()                                             // ✅ Validated by: Core_test2, Core_test13; Lua: Core_UnitTests.lua:test_config_accessors
    // getPreserveAspectRatio()                                     // ✅ Validated by: Core_test3, Core_test13
    // getAllowTextureResize()                                      // ✅ Validated by: Core_test4, Core_test13
    // getPixelFormat()                                             // ✅ Validated by: Core_test5, Core_test13; Lua: Core_UnitTests.lua:test_config_accessors
    // getRendererLogicalPresentation()                             // ✅ Validated by: Core_test6, Core_test13
    // getWindowFlags()                                             // ✅ Validated by: Core_test7, Core_test13; Lua: Core_UnitTests.lua:test_config_accessors
    // getColor()                                                   // ✅ Validated by: Core_test8, Core_test12
    // getWindowTitle(), setWindowTitle()                           // ✅ Lua: Core_UnitTests.lua:test_window_title
    // getElapsedTime(), getDeltaTime()                             // ✅ Lua: Core_UnitTests.lua:test_time_and_events
    // pumpEventsOnce(), pushMouseEvent(), pushKeyboardEvent()      // ✅ Lua: Core_UnitTests.lua:test_time_and_events
    // getConfig()                                                  // ✅ Validated/Used in: Core_test2–Core_test8, Core_test13
    // setColor(const SDL_Color&)                                   // ✅ Validated by: Core_test12

    // // --- SDL Resource Accessors --- //
    // getWindow()                                                  // ✅ Validated by: Core_test12; Lua: not exposed (planned with SDL3 object)
    // getRenderer()                                                // ✅ Validated by: Core_test12; Lua: not exposed (planned with SDL3 object)
    // getTexture()                                                 // ✅ Validated by: Core_test12; Lua: not exposed (planned with SDL3 object)

    // // --- Configuration Getters/Setters --- //
    // getConfig();                                                 // ✅ Validated by: Core_test13
    // getWindowWidth();                                            // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_accessors
    // getWindowHeight();                                           // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_accessors
    // getPixelWidth();                                             // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_accessors
    // getPixelHeight();                                            // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_accessors
    // getPreserveAspectRatio();                                    // ✅ Validated by: Core_test13
    // getAllowTextureResize();                                     // ✅ Validated by: Core_test13
    // getRendererLogicalPresentation();                            // ✅ Validated by: Core_test13
    // getWindowFlags();                                            // ✅ Validated by: Core_test13
    // getPixelFormat();                                            // ✅ Validated by: Core_test13

    // setConfig(CoreConfig& config);                               // ✅ Validated by: Core_test13
    // setWindowWidth(float width);                                 // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_setters
    // setWindowHeight(float height);                               // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_setters
    // setPixelWidth(float width);                                  // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_setters
    // setPixelHeight(float height);                                // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_setters
    // setPreserveAspectRatio(bool preserve);                       // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_setters
    // setAllowTextureResize(bool allow);                           // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_setters
    // setRendererLogicalPresentation(presentation);                // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_setters
    // setWindowFlags(SDL_WindowFlags flags);                       // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_setters
    // setPixelFormat(SDL_PixelFormat format);                      // ✅ Validated by: Core_test13; Lua: Core_UnitTests.lua:test_config_setters

    // // --- Factory & EventManager Access --- //
    // getFactory();                                                // ✅ Validated by: Core_test9, Core_test14
    // getEventManager();                                           // ✅ Validated by: Core_test14
    // getIsTraversing();                                           // ✅ Validated by: Core_test14; Lua: Core_UnitTests.lua:test_traversing_flag
    // setIsTraversing(bool traversing);                            // ✅ Validated by: Core_test14; Lua: Core_UnitTests.lua:test_traversing_flag

    // // --- Focus & Hover Management --- //
    // handleTabKeyPress();                                         // ✅ Validated by: Core_test15; Lua: (planned)
    // handleTabKeyPressReverse();                                  // ✅ Validated by: Core_test15; Lua: (planned)
    // setKeyboardFocusedObject(DisplayHandle obj);                 // ✅ Validated by: Core_test15; Lua: Core_UnitTests.lua:test_focus_hover
    // getKeyboardFocusedObject();                                  // ✅ Validated by: Core_test15; Lua: Core_UnitTests.lua:test_focus_hover
    // clearKeyboardFocusedObject();                                // ✅ Validated by: Core_test15
    // setMouseHoveredObject(DisplayHandle obj);                    // ✅ Validated by: Core_test15; Lua: Core_UnitTests.lua:test_focus_hover
    // getMouseHoveredObject();                                     // ✅ Validated by: Core_test15; Lua: Core_UnitTests.lua:test_focus_hover

    // // --- Lua State Access --- //
    // getLua();                                                    // ✅ Verified by: Core::onInit()

    // //////////////////////////////
    // // --- Factory Wrappers --- //
    // //////////////////////////////

    // // --- Object Creation --- //
    // createDisplayObject(const std::string& typeName, const sol::table& config); // ✅ Validated by: Core_test16; Lua: Core_UnitTests.lua:test_creation_orphans
    // createDisplayObject(const std::string& typeName,                            // ✅ Validated by: Core_test16
    //     const SDOM::IDisplayObject::InitStruct& init); 
    // createDisplayObjectFromScript(const std::string& typeName,                  // ✅ Validated by: Core_test16
    //     const std::string& luaScript);       

    // createAssetObject(const std::string& typeName, const sol::table& config);   // ✅ Validated by: Core_test16
    // createAssetObject(const std::string& typeName,                              // ✅ Validated by: Core_test16
    //     const SDOM::IAssetObject::InitStruct& init);     
    // createAssetObjectFromScript(const std::string& typeName,                    // ✅ Validated by: Core_test16
    //     const std::string& luaScript);         

    // // --- Object Lookup --- //
    // getDisplayObjectPtr(const std::string& name);                               // ✅ Validated by: Core_test15
    // getDisplayObject(const std::string& name);                                  // ✅ Validated by: Core_test15, Core_test16; Lua: Core_UnitTests.lua:test_lookup
    // hasDisplayObject(const std::string& name);                                  // ✅ Validated by: Core_test15

    // getAssetObjectPtr(const std::string& name);                                 // ✅ Validated by: Core_test16
    // getAssetObject(const std::string& name);                                    // ✅ Validated by: Core_test16; Lua: Core_UnitTests.lua:test_asset_api
    // hasAssetObject(const std::string& name);                                    // ✅ Validated by: Core_test16; Lua: Core_UnitTests.lua:test_asset_api

    // // --- Display Object Management --- //
    // destroyDisplayObject(const std::string& name);                              // ✅ Validated by: Core_test16; Lua: Core_UnitTests.lua:test_destroy_displayobject
    // destroyAssetObject(const std::string& name);                                // ✅ Validated by: Core_test16

    // // --- Orphan Management --- //
    // countOrphanedDisplayObjects();                                              // ✅ Validated by: Core_test16; Lua: Core_UnitTests.lua:test_creation_orphans
    // getOrphanedDisplayObjects();                                                // ✅ Validated by: Core_test16
    // detachOrphans();                                                            // ✅ Validated by: Core_test16
    // collectGarbage();                                                           // ✅ Validated by: Core_test16; Lua: Core_UnitTests.lua:test_creation_orphans
