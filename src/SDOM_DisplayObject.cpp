// SDOM_DisplayObject.hpp

#include <SDOM/SDOM_Factory.hpp>
#include <functional>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/lua_IDisplayObject.hpp>
#include <SDOM/SDOM_DisplayObject.hpp>

namespace SDOM
{        

    DisplayObject::DisplayObject()
    {
        // keep lightweight; ctor must be emitted in a TU to provide the class's key function
    }

    DisplayObject::~DisplayObject()
    {
        // default dtor; ensure emission for vtable
    }

    IDisplayObject* DisplayObject::get() const
    {
        if (!factory_) return nullptr;
        return factory_->getDomObj(name_);
    }

    // NOTE: DisplayObject is a lightweight handle. Do not forward rich APIs here.

    // Resolve child spec from Lua: accepts a DisplayObject userdata, a string name,
    // or a table with { parent=<DisplayObject|name> } or { name = "..." }.
    DisplayObject DisplayObject::resolveChildSpec(const sol::object& spec)
    {
        if (!spec.valid()) return DisplayObject();
        // If it's already a DisplayObject userdata
        try { if (spec.is<DisplayObject>()) return spec.as<DisplayObject>(); } catch(...) {}
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
        return DisplayObject();
    }

    sol::table DisplayObject::ensure_handle_table(sol::state_view lua) 
    {
        // Create the handle usertype once (no constructor), or reuse existing.
        if (!lua[LuaHandleName].valid()) 
        {
            lua.new_usertype<DisplayObject>(LuaHandleName, sol::no_constructor);
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

    void DisplayObject::bind_minimal(sol::state_view lua) 
    {
        sol::table handle = ensure_handle_table(lua);
        // Minimal handle surface ONLY
        set_if_absent(handle, "isValid", &DisplayObject::isValid);
        set_if_absent(handle, "getName", &DisplayObject::getName_lua);
        set_if_absent(handle, "getType", &DisplayObject::getType_lua);
        set_if_absent(handle, "setName", [](DisplayObject& self, const std::string& newName) { self.setName(newName); });
        set_if_absent(handle, "setType", [](DisplayObject& self, const std::string& newType) { self.setType(newType); });
        
        // DO NOT add any other bindings here.
        // IDisplayObject and each descendant should call:
        //   auto t = DisplayObject::ensure_handle_table(lua);
        //   if (t["methodName"] == nil) t.set_function("methodName", ...);
        // Their methods must be idempotent and must not overwrite existing names.
    }

    void DisplayObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        if (DEBUG_REGISTER_LUA) 
        {
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << "DisplayObject"
                      << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName
                      << CLR::RESET << std::endl;
        }

        // Ensure the shared handle exists and has only minimal helpers.
        bind_minimal(lua);

        // DO NOT add any other bindings here.
        // IDisplayObject and each descendant should call:
        //   auto t = DisplayObject::ensure_handle_table(lua);
        //   if (t["methodName"] == nil) t.set_function("methodName", ...);
        // Their methods must be idempotent and must not overwrite existing names.
    }


} // namespace SDOM