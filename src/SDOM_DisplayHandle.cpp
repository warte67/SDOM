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

    sol::table DisplayHandle::ensure_handle_table(sol::state_view lua) 
    {
        // Create the handle usertype once (no constructor), or reuse existing.
        if (!lua[LuaHandleName].valid()) 
        {
            lua.new_usertype<DisplayHandle>(LuaHandleName, sol::no_constructor);
        }
        return lua[LuaHandleName];
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
        sol::table handle = ensure_handle_table(lua);
        // Minimal handle surface ONLY
        set_if_absent(handle, "isValid", &DisplayHandle::isValid);
        set_if_absent(handle, "getName", &DisplayHandle::getName_lua);
        set_if_absent(handle, "getType", &DisplayHandle::getType_lua);
        set_if_absent(handle, "setName", [](DisplayHandle& self, const std::string& newName) { self.setName(newName); });
        set_if_absent(handle, "setType", [](DisplayHandle& self, const std::string& newType) { self.setType(newType); });

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