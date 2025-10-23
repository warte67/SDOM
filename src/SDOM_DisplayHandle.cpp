// SDOM_DisplayHandle.hpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <functional>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
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

    // NOTE: DisplayHandle is a lightweight handle. Do not forward rich APIs here.
    // Minimal _registerLuaBindings implementation for DisplayHandle so the
    // factory can invoke it when registering prototype types. This ensures the
    // minimal handle table and per-type table exist in the target Lua state.
    void DisplayHandle::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Ensure minimal shared handle exists.
        // This keeps the DisplayHandle lightweight while exposing a few
        // convenience accessors to Lua so scripts can inspect handle state.
        
        // auto ensure_handle_table = [&](sol::state_view L) -> sol::table {
        //     if (!L[DisplayHandle::LuaHandleName].valid()) {
        //         L.new_usertype<DisplayHandle>(DisplayHandle::LuaHandleName, sol::no_constructor);
        //     }
        //     return L[DisplayHandle::LuaHandleName];
        // };

        // static auto set_if_absent = [](sol::table& handle, const char* name, auto&& fn) {
        //     sol::object cur = handle.raw_get_or(name, sol::lua_nil);
        //     if (!cur.valid() || cur == sol::lua_nil) {
        //         handle.set_function(name, std::forward<decltype(fn)>(fn));
        //     }
        // };

        // sol::table handle = ensure_handle_table(lua);

        // Ensure the per-type table exists and add functions to it
        sol::table handle = SDOM::IDataObject::ensure_sol_table(lua, typeName);


        // Minimal surface similar to AssetHandle; expose basic getters and
        // geometry accessors that forward to the underlying IDisplayObject.
        set_if_absent(handle, "isValid", &DisplayHandle::isValid_lua);
        set_if_absent(handle, "getName", &DisplayHandle::getName_lua);
        set_if_absent(handle, "getType", &DisplayHandle::getType_lua);
        set_if_absent(handle, "getX", [](DisplayHandle& self) -> int { IDisplayObject* o = self.get(); return o ? o->getX() : 0; });
        set_if_absent(handle, "getY", [](DisplayHandle& self) -> int { IDisplayObject* o = self.get(); return o ? o->getY() : 0; });
        set_if_absent(handle, "getWidth", [](DisplayHandle& self) -> int { IDisplayObject* o = self.get(); return o ? o->getWidth() : 0; });
        set_if_absent(handle, "getHeight", [](DisplayHandle& self) -> int { IDisplayObject* o = self.get(); return o ? o->getHeight() : 0; });

        // Register the concrete usertype once (supply base classes if needed).
        // If DisplayHandle derives from IDataObject:
        auto ut = SDOM::IDataObject::register_usertype_with_table<DisplayHandle, IDataObject>(lua, typeName);

        // Optionally store `ut` in a derived-class member if you need it later.
        this->objHandleType_ = ut; // only possible if objHandleType_ has the correct type in the derived class

    }

} // namespace SDOM