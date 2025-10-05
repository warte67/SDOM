// lua_BindHelpers.hpp --- Lua binding helpers

#pragma once

#include <SDOM/SDOM.hpp>

namespace SDOM
{
    // Basic binders (usertype method, CoreForward table entry, and optional global alias)
    // - objHandleType: sol::usertype<Core>&
    // - coreTable: sol::table&
    // - lua: sol::state_view

    // Zero-arg void functions (quit, shutdown, etc.)
    void bind_noarg(const std::string& name, std::function<void()> func,
                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Single-table argument: void(const sol::table&)
    void bind_table(const std::string& name, std::function<void(const sol::table&)> func,
                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Single-string argument: void(const std::string&)
    void bind_string(const std::string& name, std::function<void(const std::string&)> func,
                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Single-bool argument: void(bool)
    void bind_bool_arg(const std::string& name, std::function<void(bool)> func,
                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Single-DisplayObject (or name) argument: void(const DisplayObject&)
    void bind_do_arg(const std::string& name, std::function<void(const DisplayObject&)> func,
                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Single-sol::object argument: void(const sol::object&)
    void bind_object_arg(const std::string& name, std::function<void(const sol::object&)> func,
                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Returners (no-arg) -----------------------------------------------------------
    // Return DisplayObject: DisplayObject ()
    void bind_return_displayobject(const std::string& name, std::function<DisplayObject()> func,
                                sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return bool: bool ()
    void bind_return_bool(const std::string& name, std::function<bool()> func,
                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return string: std::string ()
    void bind_return_string(const std::string& name, std::function<std::string()> func,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return float: float ()
    void bind_return_float(const std::string& name, std::function<float()> func,
                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return int: int ()
    void bind_return_int(const std::string& name, std::function<int()> func,
                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return vector<DisplayObject>: std::vector<DisplayObject> ()
    void bind_return_vector_do(const std::string& name, std::function<std::vector<DisplayObject>()> func,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Return vector<string>: std::vector<std::string> ()
    void bind_return_vector_string(const std::string& name, std::function<std::vector<std::string>()> func,
                                sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // String+table -> DisplayObject
    void bind_string_table_return_do(const std::string& name, std::function<DisplayObject(const std::string&, const sol::table&)> func,
                                    sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // String -> DisplayObject
    void bind_string_return_do(const std::string& name, std::function<DisplayObject(const std::string&)> func,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // String -> bool
    void bind_string_return_bool(const std::string& name, std::function<bool(const std::string&)> func,
                                sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Callback registration binders ------------------------------------------------
    // Each accepts the registrar (a function that takes a std::function<...> to register the host callback).

    // bool() callbacks (registerOnInit, registerOnUnitTest)
    void bind_callback_bool(const std::string& name, std::function<void(std::function<bool()>)> registrar,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // void() callbacks (registerOnQuit, registerOnRender)
    void bind_callback_void(const std::string& name, std::function<void(std::function<void()>)> registrar,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // update(float) callbacks
    void bind_callback_update(const std::string& name, std::function<void(std::function<void(float)>)> registrar,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // event(const Event&) callbacks
    void bind_callback_event(const std::string& name, std::function<void(std::function<void(const Event&)>)> registrar,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // resize(int,int) callbacks
    void bind_callback_resize(const std::string& name, std::function<void(std::function<void(int,int)>)> registrar,
                            sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Custom forwarder: string + function -> calls host function(registerOn style)
    void bind_string_function_forwarder(const std::string& name, std::function<void(const std::string&, const sol::function&)> func,
                                        sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);

    // Bind a function that accepts either a name (string/table) or a DisplayObject handle
    // and dispatches to the appropriate host-side overload (e.g. setStage/setRootNode alias)
    void bind_name_or_handle(const std::string& name, std::function<void(const std::string&)> nameFunc, std::function<void(const DisplayObject&)> handleFunc,
                             sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua);
    

} // namespace SDOM
