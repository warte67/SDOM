// SDOM_DisplayHandle.hpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <functional>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/lua_IDisplayObject.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_LuaRegistry.hpp>
#include <SDOM/SDOM_LuaRegisterHelpers.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>

namespace SDOM
{        

    DisplayHandle::DisplayHandle()
    {
        // keep lightweight; ctor must be emitted in a TU to provide the class's key function
    }

    DisplayHandle::~DisplayHandle()
    {
        // default dtor; ensure emission for vtable
    }

    IDisplayObject* DisplayHandle::get() const
    {
        if (!factory_) return nullptr;
        return factory_->getDomObj(name_);
    }

    // NOTE: DisplayHandle is a lightweight handle. Do not forward rich APIs here.

    // Resolve child spec from Lua: accepts a DisplayHandle userdata, a string name,
    // or a table with { parent=<DisplayHandle|name> } or { name = "..." }.
    DisplayHandle DisplayHandle::resolveChildSpec(const sol::object& spec)
    {
        if (!spec.valid()) return DisplayHandle();
        // If it's already a DisplayHandle userdata
        try { if (spec.is<DisplayHandle>()) return spec.as<DisplayHandle>(); } catch(...) {}
        // If it's a string name
        try { if (spec.is<std::string>()) return getCore().getDisplayObject(spec.as<std::string>()); } catch(...) {}
        // If table, try fields 'parent' or 'name'
        if (spec.is<sol::table>()) {
            sol::table t = spec.as<sol::table>();
            // Accept 'child' as an alias for 'parent'/'name' used in some Lua helpers
            sol::object childObj = t.get<sol::object>("child");
            if (childObj.valid()) {
                try { return resolveChildSpec(childObj); } catch(...) {}
            }
            sol::object parentObj = t.get<sol::object>("parent");
            if (parentObj.valid()) {
                try { return resolveChildSpec(parentObj); } catch(...) {}
            }
            sol::object nameObj = t.get<sol::object>("name");
            if (nameObj.valid()) {
                try { if (nameObj.is<std::string>()) return getCore().getDisplayObject(nameObj.as<std::string>()); } catch(...) {}
            }
        }
        return DisplayHandle();
    }

    // Ensure the shared handle usertype exists. This returns the minimal
    // handle table which contains only core helpers. Per-type bindings
    // should be placed into per-type tables created by
    // ensure_type_bind_table().
    sol::table DisplayHandle::ensure_handle_table(sol::state_view lua) 
    {
        // Create the handle usertype once (no constructor), or reuse existing.
        if (!lua[LuaHandleName].valid()) 
        {
            lua.new_usertype<DisplayHandle>(LuaHandleName, sol::no_constructor);
        }

        // Ensure we have a minimal metatable installed that will route
        // lookups to per-type tables when necessary. bind_minimal will
        // populate the minimal function surface and set up the dispatcher.
        bind_minimal(lua);

        return lua[LuaHandleName];
    }

    // Create or return a per-type binding table. If parentTypeName is
    // provided, the returned table will have a metatable __index pointing
    // to the parent's table so inheritance works.
    sol::table DisplayHandle::ensure_type_bind_table(sol::state_view lua, const std::string& typeName, const std::string& parentTypeName)
    {
        // Create the central registry table: SDOM_Bindings
        sol::table bindingsRoot;
        if (!lua["SDOM_Bindings"].valid())
        {
            bindingsRoot = lua.create_table();
            lua["SDOM_Bindings"] = bindingsRoot;
            if (DEBUG_REGISTER_LUA) {
                try { std::cout << "[DEBUG] ensure_type_bind_table: created SDOM_Bindings root table" << std::endl; } catch(...) {}
            }
        }
        else
        {
            bindingsRoot = lua["SDOM_Bindings"];
        }

        // If the per-type table already exists, return it (diagnostic).
        if (bindingsRoot[typeName].valid())
        {
            if (DEBUG_REGISTER_LUA) {
                try { std::cout << "[DEBUG] ensure_type_bind_table: returning existing SDOM_Bindings[" << typeName << "]" << std::endl; } catch(...) {}
            }
            // try {
            //     sol::function tostring_fn = lua["tostring"];
            //     std::string existing = tostring_fn(bindingsRoot[typeName]);
            //     std::cout << "[ensure_type_bind_table] returning existing table for type=" << typeName
            //               << " table=" << existing << std::endl;
            // } catch(...) {}
            return bindingsRoot[typeName];
        }

        // Create and store a new per-type table.
        sol::table t = lua.create_table();
        bindingsRoot[typeName] = t;
        if (DEBUG_REGISTER_LUA) {
            try { std::cout << "[DEBUG] ensure_type_bind_table: created SDOM_Bindings[" << typeName << "]" << std::endl; } catch(...) {}
        }

        // try {
        //     sol::function tostring_fn = lua["tostring"];
        //     std::string created = tostring_fn(t);
        //     std::cout << "[ensure_type_bind_table] created table for type=" << typeName
        //               << " table=" << created << std::endl;
        // } catch(...) {}

        // If parent provided and parent table exists, set up metatable __index
        if (!parentTypeName.empty() && bindingsRoot[parentTypeName].valid())
        {
            sol::table parent = bindingsRoot[parentTypeName];
            sol::table mt = lua.create_table();
            mt["__index"] = parent;
            t[sol::metatable_key] = mt;
            try {
                sol::function tostring_fn = lua["tostring"];
                std::string parent_table_str;
                try {
                    sol::object obj = tostring_fn(parent);
                    parent_table_str = (obj.valid() && obj.is<std::string>()) ? obj.as<std::string>() : "<non-string>";
                } catch(...) {
                    parent_table_str = "<tostring-failed>";
                }
                // std::cout << "[ensure_type_bind_table] set parent index for type=" << typeName
                //           << " parent=" << parentTypeName
                //           << " parent_table=" << parent_table_str << std::endl;
            } catch(...) {}
        }

        return t;
    }


    static inline void set_if_absent(sol::table& handle, const char* name, auto&& fn) 
    {
        sol::object cur = handle.raw_get_or(name, sol::lua_nil);
        if (!cur.valid() || cur == sol::lua_nil) 
        {
            handle.set_function(name, std::forward<decltype(fn)>(fn));
        }
    }

    void DisplayHandle::bind_minimal(sol::state_view lua) 
    {
        // Ensure the DisplayHandle usertype/global exists so that reading
        // lua[LuaHandleName] yields a table (not nil). Some callers invoke
        // bind_minimal directly without creating the usertype first.
        if (!lua[LuaHandleName].valid()) {
            lua.new_usertype<DisplayHandle>(LuaHandleName, sol::no_constructor);
        }
        sol::table handle = lua[LuaHandleName];
        // Minimal handle surface ONLY


        // Bind minimal helpers as lambdas that accept the userdata (DisplayHandle&)
        // This ensures the Lua-colon call (obj:method()) correctly supplies the
        // userdata as the first argument and avoids mismatched argument errors.
        set_if_absent(handle, "isValid", [](DisplayHandle& self) { return self.isValid(); });
        set_if_absent(handle, "getName", [](DisplayHandle& self) { return self.getName(); });
        set_if_absent(handle, "getType", [](DisplayHandle& self) { return self.getType(); });
        set_if_absent(handle, "setName", [](DisplayHandle& self, const std::string& newName) { self.setName(newName); });
        set_if_absent(handle, "setType", [](DisplayHandle& self, const std::string& newType) { self.setType(newType); });


        // Install dispatcher metamethods on the handle usertype so that
        // lookups not present on the minimal table are forwarded to the
        // per-type binding tables inside SDOM_Bindings.
        // Only install once.
        sol::table mt = handle[sol::metatable_key];
        if (!mt.valid()) mt = lua.create_table();

        if (!mt["__index"].valid())
        {
            // __index will check the C++ registry first (per-type authoritative),
            // then fall back to SDOM_Bindings[type] and finally the minimal handle
            mt["__index"] = [=](DisplayHandle& h, const std::string& key) -> sol::object {
                sol::state_view L = lua;
                std::string rtype;
                try { rtype = h.getType(); } catch(...) { rtype = std::string(); }

                // Silent probe: no debug output here.

                // Special-case: create core-side wrappers for Label get/set so userdata conversions work
                if (rtype == "Label" && (key == "getFontSize" || key == "setFontSize")) {
                    try {
                        sol::state& core = SDOM::Core::getInstance().getLua();
                        if (key == "getFontSize") {
                            auto callable = [](DisplayHandle h)->int {
                                auto* l = h.as<Label>();
                                try { std::cout << "[WRAP_INVOKE] Label.getFontSize wrapper invoked; label_ptr=" << (void*)l << std::endl; } catch(...) {}
                                return l ? l->getFontSize() : 0;
                            };
                            sol::object obj = sol::make_object(core, callable);
                            try { std::cout << "[LABEL_RETURN] path=wrapper lookup_L=" << (void*)L.lua_state() << " core_L=" << (void*)core.lua_state() << " returned=" << (void*)obj.pointer() << std::endl; } catch(...) {}
                            return obj;
                        } else {
                            auto callable = [](DisplayHandle h, int v)->void {
                                auto* l = h.as<Label>();
                                try { std::cout << "[WRAP_INVOKE] Label.setFontSize wrapper invoked; label_ptr=" << (void*)l << " value=" << v << std::endl; } catch(...) {}
                                if (l) l->setFontSize(v);
                            };
                            sol::object obj = sol::make_object(core, callable);
                            try { std::cout << "[LABEL_RETURN] path=wrapper-set lookup_L=" << (void*)L.lua_state() << " core_L=" << (void*)core.lua_state() << " returned=" << (void*)obj.pointer() << std::endl; } catch(...) {}
                            return obj;
                        }
                    } catch(...) {}
                }

                // 1) Registry lookup (authoritative per-type)
                try {
                    if (!rtype.empty() && getLuaBindingRegistry().has(rtype, key)) {
                        sol::state_view sv = SDOM::getLua();
                        auto fn = getLuaBindingRegistry().get(rtype, key);
                        (void)L; (void)fn; (void)rtype; (void)key; // removed debug prints

                        // For Label:getFontSize, create a forwarding wrapper on the registry's state
                        if (rtype == "Label" && key == "getFontSize") {
                            try {
                                sol::protected_function pf = fn;
                                auto wrapper = [pf](DisplayHandle h)->int {
                                    (void)pf; (void)h; // forwarding wrapper - debug prints removed
                                    try {
                                        sol::protected_function_result r = pf(h);
                                        if (r.valid()) {
                                            int v = r.get<int>();
                                            (void)v; // registry call value - debug prints removed
                                            return v;
                                        }
                                    } catch(const std::exception &e) {
                                        (void)e; // removed debug print
                                    } catch(...) {
                                        ; // removed debug print
                                    }
                                    return 0;
                                };
                                sol::object retobj = sol::make_object(sv, wrapper);
                                (void)retobj; // removed debug print
                                return retobj;
                            } catch(...) {
                                return sol::make_object(sv, fn);
                            }
                        }

                        if (rtype == "Label" && key == "setFontSize") {
                            try {
                                sol::protected_function pf = fn;
                                auto wrapper = [pf](DisplayHandle h, int v)->void {
                                    (void)pf; (void)h; (void)v; // removed debug print
                                    try { pf(h, v); } catch(...) {}
                                };
                                sol::object retobj = sol::make_object(sv, wrapper);
                                (void)retobj; // removed debug print
                                return retobj;
                            } catch(...) {
                                return sol::make_object(sv, fn);
                            }
                        }

                        // Return the stored protected_function so Lua can call it normally
                        return sol::make_object(sv, fn);
                    }
                } catch(...) {}

                // 2) Legacy per-type Lua table
                try {
                    sol::table bindingsRoot = L["SDOM_Bindings"];
                    if (!rtype.empty() && bindingsRoot.valid() && bindingsRoot[rtype].valid()) {
                        sol::table t = bindingsRoot[rtype];
                        sol::object v = t[key];
                        if (v.valid() && v != sol::lua_nil) return v;
                    }
                } catch(...) {}

                // Fallback minimal
                try {
                    sol::table minimal = L[LuaHandleName];
                    sol::object mv = minimal[key];
                    return mv;
                } catch(...) { return sol::lua_nil; }
            };
                handle[sol::metatable_key] = mt;
            }

        // Always (re)install stronger forwarding wrappers for getFontSize/setFontSize
        // so that callers who obtain the function object will invoke a local
        // wrapper which logs and then forwards to the authoritative registry or
        // falls back to the direct C++ method. Overwriting ensures we see the
        // invocation reliably in logs even if prior registration order varied.
        try {
            handle.set_function("getFontSize", [lua](DisplayHandle h)->int {
                std::string t;
                try { t = h.getType(); } catch(...) { t = std::string(); }
                try {
                    (void)lua; (void)h; (void)t;
                } catch(...) {}
                try {
                    if (!t.empty() && getLuaBindingRegistry().has(t, "getFontSize")) {
                        sol::protected_function pf = getLuaBindingRegistry().get(t, "getFontSize");
                        (void)pf;
                        try {
                            sol::protected_function_result r = pf(h);
                            if (r.valid()) {
                                int v = r.get<int>();
                                (void)v;
                                return v;
                            }
                        } catch(const std::exception &e) {
                            (void)e;
                        } catch(...) {
                            (void)0;
                        }
                    }
                } catch(...) {}
                // Fallback direct C++ call
                try {
                    Label* l = h.as<Label>();
                    if (l) {
                        int v = l->getFontSize();
                        (void)v; (void)l;
                        return v;
                    }
                } catch(...) {}
                return 0;
            });

            handle.set_function("setFontSize", [lua](DisplayHandle h, int v) -> void {
                std::string t;
                try { t = h.getType(); } catch(...) { t = std::string(); }
                try {
                    (void)lua; (void)h; (void)t; (void)v;
                } catch(...) {}
                try {
                    if (!t.empty() && getLuaBindingRegistry().has(t, "setFontSize")) {
                        sol::protected_function pf = getLuaBindingRegistry().get(t, "setFontSize");
                        (void)pf;
                        try { pf(h, v); return; } catch(const std::exception &e) {
                            (void)e;
                        } catch(...) {
                            (void)0;
                        }
                    }
                } catch(...) {}
                try { Label* l = h.as<Label>(); if (l) { l->setFontSize(v); (void)l; } } catch(...) {}
            });
            // Also install strong forwarding wrappers for setFontWidth/setFontHeight
            handle.set_function("setFontWidth", [lua](DisplayHandle h, int v) -> void {
                std::string t;
                try { t = h.getType(); } catch(...) { t = std::string(); }
                (void)lua; (void)h; (void)t; (void)v;
                try {
                    if (!t.empty() && getLuaBindingRegistry().has(t, "setFontWidth")) {
                        sol::protected_function pf = getLuaBindingRegistry().get(t, "setFontWidth");
                        (void)pf;
                        try { pf(h, v); return; } catch(const std::exception &e) { (void)e; } catch(...) { (void)0; }
                    }
                } catch(...) {}
                try { Label* l = h.as<Label>(); if (l) { l->setFontWidth(v); (void)l; } } catch(...) {}
            });
            handle.set_function("setFontHeight", [lua](DisplayHandle h, int v) -> void {
                std::string t;
                try { t = h.getType(); } catch(...) { t = std::string(); }
                (void)lua; (void)h; (void)t; (void)v;
                try {
                    if (!t.empty() && getLuaBindingRegistry().has(t, "setFontHeight")) {
                        sol::protected_function pf = getLuaBindingRegistry().get(t, "setFontHeight");
                        (void)pf;
                        try { pf(h, v); return; } catch(const std::exception &e) { (void)e; } catch(...) { (void)0; }
                    }
                } catch(...) {}
                try { Label* l = h.as<Label>(); if (l) { l->setFontHeight(v); (void)l; } } catch(...) {}
            });
        } catch(...) {}

        // Also install strong forwarding wrappers for getFontWidth/getFontHeight
        try {
            handle.set_function("getFontWidth", [lua](DisplayHandle h)->int {
                std::string t;
                try { t = h.getType(); } catch(...) { t = std::string(); }
                try {
                    (void)lua; (void)h; (void)t;
                } catch(...) {}
                try {
                    if (!t.empty() && getLuaBindingRegistry().has(t, "getFontWidth")) {
                        sol::protected_function pf = getLuaBindingRegistry().get(t, "getFontWidth");
                        (void)pf;
                        try {
                            sol::protected_function_result r = pf(h);
                            if (r.valid()) {
                                int v = r.get<int>();
                                (void)v;
                                return v;
                            }
                        } catch(const std::exception &e) {
                            (void)e;
                        } catch(...) {
                            (void)0;
                        }
                    }
                } catch(...) {}
                try { IPanelObject* p = nullptr; try { p = dynamic_cast<IPanelObject*>(h.get()); } catch(...) {} if (p) return p->getFontWidth(); } catch(...) {}
                try { Label* l = h.as<Label>(); if (l) { int v = l->getFontWidth(); (void)v; return v; } } catch(...) {}
                return 0;
            });

            handle.set_function("getFontHeight", [lua](DisplayHandle h)->int {
                std::string t;
                try { t = h.getType(); } catch(...) { t = std::string(); }
                try {
                    (void)lua; (void)h; (void)t;
                } catch(...) {}
                try {
                    if (!t.empty() && getLuaBindingRegistry().has(t, "getFontHeight")) {
                        sol::protected_function pf = getLuaBindingRegistry().get(t, "getFontHeight");
                        (void)pf;
                        try {
                            sol::protected_function_result r = pf(h);
                            if (r.valid()) {
                                int v = r.get<int>();
                                (void)v;
                                return v;
                            }
                        } catch(const std::exception &e) {
                            (void)e;
                        } catch(...) {
                            (void)0;
                        }
                    }
                } catch(...) {}
                try { IPanelObject* p = nullptr; try { p = dynamic_cast<IPanelObject*>(h.get()); } catch(...) {} if (p) return p->getFontHeight(); } catch(...) {}
                try { Label* l = h.as<Label>(); if (l) { int v = l->getFontHeight(); (void)v; return v; } } catch(...) {}
                return 0;
            });
        } catch(...) {}

    }

    // Minimal _registerLuaBindings implementation for DisplayHandle so the
    // factory can invoke it when registering prototype types. This ensures the
    // minimal handle table and per-type table exist in the target Lua state.
    void DisplayHandle::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        try {
            // Ensure the shared handle table and minimal metatable are present
            ensure_handle_table(lua);
            // Ensure a per-type table exists so derived types can populate it
            ensure_type_bind_table(lua, typeName);
        } catch(...) {}
    }

} // namespace SDOM