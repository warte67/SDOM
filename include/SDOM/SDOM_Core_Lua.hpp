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

    // Single-sol::object argument returning AssetHandle
    void core_bind_object_return_asset(const std::string& name, std::function<AssetHandle(const sol::object&)> func,
                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Returners (no-arg) -----------------------------------------------------------

    void core_bind_return_displayobject(const std::string& name, std::function<DisplayHandle()> func,
                                sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    void core_bind_return_asset(const std::string& name, std::function<AssetHandle()> func,
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
        // ✅ Test Verified
        // 🔄 In Progress
        // ⚠️ Failing     
        // 🚫 Remove
        // ❌ Invalid
        // ☐ Planned

    // --- ✅ Main Loop & Event Dispatch --- //
    
    void core_configure_lua(const sol::table& config);
    void core_configureFromFile_lua(const std::string& filename);


    // --- ✅ Callback/Hook Registration --- //
    void core_registerOnInit_lua(std::function<bool()> fn);                 // ✅ used by core_registerOn()
    void core_registerOnQuit_lua(std::function<void()> fn);                 // ✅ used by core_registerOn()
    void core_registerOnUpdate_lua(std::function<void(float)> fn);          // ✅ used by core_registerOn()
    void core_registerOnEvent_lua(std::function<void(const Event&)> fn);    // ✅ used by core_registerOn()
    void core_registerOnRender_lua(std::function<void()> fn);               // ✅ used by core_registerOn()
    void core_registerOnUnitTest_lua(std::function<bool()> fn);             // ✅ used by core_registerOn()
    void core_registerOnWindowResize_lua(std::function<void(int, int)> fn); // ✅ used by core_registerOn()

    // --- ✅ Generic registration helper: registerOn("Init"|"Update"|...) from Lua --- //
    void core_registerOn_lua(const std::string& name, const sol::function& f);  // ✅ used in config.lua

    // --- ✅ Stage/Root Node Management --- //
    void setRootNodeByName_lua(const std::string& name);                    // ✅ Lua Test 1
    void setRootNode_lua(const DisplayHandle& handle);                      // ✅ Lua Test 1
    void setStageByName_lua(const std::string& name);                       // ✅ Lua Test 1 
    void setStage_lua(const DisplayHandle& handle);                         // ✅ Lua Test 1
    DisplayHandle getRoot_lua();                                            // ✅ Lua Test 1
    DisplayHandle getStage_lua();                                           // ✅ Lua Test 1

    // --- ✅ Factory & EventManager Access --- //
    bool getIsTraversing_lua();                                             // ✅ Lua Test 2      
    void setIsTraversing_lua(bool traversing);                              // ✅ Lua Test 2

    // --- ✅ Object Creation and Lookup--- //
    DisplayHandle createDisplayObject_lua(const std::string& typeName, const sol::table& config);  // ✅ Lua Test 3
    DisplayHandle getDisplayObject_lua(const std::string& name);            // ✅ Lua Test 3
    bool hasDisplayObject_lua(const std::string& name);                     // ✅ Lua Test 3
    AssetHandle createAssetObject_lua(const std::string& typeName, const sol::table& config);  // ✅ Lua Test 3
    AssetHandle getAssetObject_lua(const std::string& name);                // ✅ Lua Test 3
    bool hasAssetObject_lua(const std::string& name);                       // ✅ Lua Test 3

    // --- ✅ Focus & Hover Management --- //
    void doTabKeyPressForward_lua();                                        // ✅ Lua Test 7
    void doTabKeyPressReverse_lua();                                        // ✅ Lua Test 7
    void setKeyboardFocusedObject_lua(const DisplayHandle& handle);         // ✅ Lua Test 7
    void clearKeyboardFocusedObject_lua();                                  // ✅ Lua Test 7
    DisplayHandle getKeyboardFocusedObject_lua();                           // ✅ Lua Test 7
    void setMouseHoveredObject_lua(const DisplayHandle& handle);            // ✅ Lua Test 7
    DisplayHandle getMouseHoveredObject_lua();                              // ✅ Lua Test 7
    void clearMouseHoveredObject_lua();                                     // ✅ Lua Test 7

    // --- ✅ Window Title & Timing --- //
    std::string getWindowTitle_lua();                                       // ✅ Lua Test 6
    void setWindowTitle_lua(const std::string& title);                      // ✅ Lua Test 6
    float getElapsedTime_lua();                                             // ✅ Lua Test 8
    float getDeltaTime_lua();                                               // ✅ Lua Test 8

    // --- ✅ Event helpers (exposed to Lua) --- //
    void pumpEventsOnce_lua();                                              // ✅ Lua Test 8
    void pushMouseEvent_lua(const sol::object& args);                       // ✅ Lua Test 8
    void pushKeyboardEvent_lua(const sol::object& args);                    // ✅ Lua Test 8

    // --- ✅ Orphan / Future Child Management --- //
    void destroyDisplayObject_lua(const std::string& name);                 // ✅ Lua Test 10, 11
    void destroyDisplayObject_any_lua(const sol::object& spec);             // ✅ Lua Test 10
    void destroyAssetObject_lua(const std::string& name);                   // ✅ Lua Test 3
    void destroyAssetObject_any_lua(const sol::object& spec);               // ✅ Lua Test 3
    int countOrphanedDisplayObjects_lua();                                  // ✅ Lua Test 3
    std::vector<DisplayHandle> getOrphanedDisplayObjects_lua();             // ✅ Lua Test 3
    void collectGarbage_lua();                                              // ✅ Lua Test 3 

    // --- 🚫 Future Child Management (remove) --- //
    void attachFutureChildren_lua();                                        // ❌ invalid
    void addToOrphanList_lua(const DisplayHandle& orphan);                  // ❌ invalid
    void addToFutureChildrenList_lua(const sol::object& args);              // ❌ invalid

    // --- 🔄 Factory utilities exposed on Core --- //
    void clearFactory_lua();                                                // ✅ Lua Test 12 (commented out, but verified)
    AssetHandle findAssetByFilename_lua(std::string filename);              // ✅ Lua Test 12
    AssetHandle findSpriteSheetByParams_lua(const sol::object& spec);       // ✅ Lua Test 12
    void unloadAllAssetObjects_lua();                                       // ✅ Lua Test 12    
    void reloadAllAssetObjects_lua();                                       // ✅ Lua Test 12

    // --- 🔄 Utility Methods --- //
    std::vector<std::string> getDisplayObjectNames_lua();                   // ✅ Verified
    void printObjectRegistry_lua();                                         // ✅ Verified

    // --- 🔄 Version Methods --- //
    std::string getVersionString_lua();                                     // ☐ pending
    std::string getVersionFullString_lua();                                 // ☐ pending
    int getVersionMajor_lua();                                              // ☐ pending
    int getVersionMinor_lua();                                              // ☐ pending
    int getVersionPatch_lua();                                              // ☐ pending
    std::string getVersionCodename_lua();                                   // ☐ pending
    std::string getVersionBuild_lua();                                      // ☐ pending
    std::string getVersionBuildDate_lua();                                  // ☐ pending
    std::string getVersionCommit_lua();                                     // ☐ pending
    std::string getVersionBranch_lua();                                     // ☐ pending
    std::string getVersionCompiler_lua();                                   // ☐ pending        
    std::string getVersionPlatform_lua();                                   // ☐ pending

} // namespace SDOM
