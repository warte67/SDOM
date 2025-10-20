// SDOM_DisplayHandle.hpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <functional>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/lua_IDisplayObject.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>

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
        }
        else
        {
            bindingsRoot = lua["SDOM_Bindings"];
        }

        // If the per-type table already exists, return it (diagnostic).
        if (bindingsRoot[typeName].valid())
        {
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
            // __index will check SDOM_Bindings[type] first, then fall back
            // to the minimal handle table (which is 'handle' itself).
            mt["__index"] = [=](DisplayHandle& h, const std::string& key) -> sol::object {
                sol::state_view L = lua;
                sol::table bindingsRoot = L["SDOM_Bindings"];
                std::string rtype;
                try { rtype = h.getType(); } catch(...) { rtype = std::string(); }
                // try { std::cout << "[DisplayHandle::__index] lookup type='" << rtype << "' key='" << key << "'" << std::endl; } catch(...) {}
                try {
                    if (!rtype.empty() && bindingsRoot.valid() && bindingsRoot[rtype].valid())
                    {
                        sol::table t = bindingsRoot[rtype];
                        // Use operator[] so Lua's metatable __index chain is respected.
                        sol::object v = t[key];
                        if (v.valid() && v != sol::lua_nil) {
                            // try { std::cout << "[DisplayHandle::__index] found in type table for key='" << key << "'" << std::endl; } catch(...) {}
                            return v;
                        } else {
                            // try { std::cout << "[DisplayHandle::__index] not found in type table for key='" << key << "' (fallback)" << std::endl; } catch(...) {}
                        }
                    }
                } catch(...) {}
                // fallback: return whatever is on the minimal handle table using operator[]
                sol::table minimal = L[LuaHandleName];
                sol::object mv = minimal[key];
                // try { if (mv.valid() && mv != sol::lua_nil) std::cout << "[DisplayHandle::__index] found in minimal table for key='" << key << "'" << std::endl; else std::cout << "[DisplayHandle::__index] not found anywhere for key='" << key << "'" << std::endl; } catch(...) {}
                return mv;
            };
            handle[sol::metatable_key] = mt;
        }

        // Install a __newindex metamethod that forwards assignments on the
        // userdata to the appropriate per-type or minimal setter function.
        // This avoids Lua trying to write into the userdata (which sol2
        // disallows) while keeping assignment semantics for properties.
        if (!mt["__newindex"].valid()) {
            mt["__newindex"] = [=](DisplayHandle& h, const std::string& key, const sol::object& val) {
                sol::state_view L = lua;
                sol::table bindingsRoot = L["SDOM_Bindings"];
                std::string rtype;
                try { rtype = h.getType(); } catch(...) { rtype = std::string(); }

                // Helper to compute setter name: snake_case -> CamelCase with 'set' prefix
                auto make_setter = [](const std::string& k)->std::string {
                    std::string out = "set";
                    bool cap = true;
                    for (char c : k) {
                        if (c == '_') { cap = true; continue; }
                        if (cap) { out.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c)))); cap = false; }
                        else out.push_back(c);
                    }
                    return out;
                };

                std::string setter = make_setter(key);

                // Try per-type table first
                try {
                    if (!rtype.empty() && bindingsRoot.valid() && bindingsRoot[rtype].valid()) {
                        sol::table t = bindingsRoot[rtype];
                        sol::object cand = t.raw_get_or(key, sol::lua_nil);
                        // If the per-type table has a property entry or a setter function
                        sol::object fnobj = t.raw_get_or(setter, sol::lua_nil);
                        if (fnobj.valid() && fnobj != sol::lua_nil && fnobj.is<sol::function>()) {
                            sol::function f = fnobj.as<sol::function>();
                            // Call the setter with (DisplayHandle, value). Let sol2 coerce types.
                            try { f(h, val); return; } catch(...) {}
                        }
                    }
                } catch(...) {}

                // Fallback: try minimal handle table
                try {
                    sol::table minimal = L[LuaHandleName];
                    sol::object fnobj = minimal.raw_get_or(setter, sol::lua_nil);
                    if (fnobj.valid() && fnobj != sol::lua_nil && fnobj.is<sol::function>()) {
                        sol::function f = fnobj.as<sol::function>();
                        try { f(h, val); return; } catch(...) {}
                    }
                } catch(...) {}

                // If we get here, no setter was found. Let Lua/runtime raise the usual error by doing nothing
                // (sol2 will report that new_index is not defined). Alternatively, we could log/debug.
                return;
            };
            handle[sol::metatable_key] = mt;
        }

        // DO NOT add any other bindings here.
        // IDisplayObject and each descendant should call:
        //   auto t = DisplayHandle::ensure_handle_table(lua);
        //   if (t["methodName"] == nil) t.set_function("methodName", ...);
        // Their methods must be idempotent and must not overwrite existing names.
    }

    void DisplayHandle::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        if (DEBUG_REGISTER_LUA) 
        {
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << "DisplayHandle"
                      << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName
                      << CLR::RESET << std::endl;
        }

        // Ensure the shared handle exists and has only minimal helpers.
        bind_minimal(lua);

        // DO NOT add any other bindings here.
        // IDisplayObject and each descendant should call:
        //   auto t = DisplayHandle::ensure_handle_table(lua);
        //   if (t["methodName"] == nil) t.set_function("methodName", ...);
        // Their methods must be idempotent and must not overwrite existing names.
    }


} // namespace SDOM