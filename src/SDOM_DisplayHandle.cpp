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
        // Register the DisplayHandle usertype (no constructor) once, and reuse thereafter.
        sol::usertype<DisplayHandle> ut = SDOM::IDataObject::register_usertype_with_table<DisplayHandle, SDOM::IDataObject>(lua, typeName);

        sol::table handleTable = SDOM::IDataObject::ensure_sol_table(lua, typeName);

        auto set_if_absent = [](sol::table& tbl, const char* name, auto&& fn) {
            sol::object cur = tbl.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                tbl.set_function(name, std::forward<decltype(fn)>(fn));
            }
        };

        // Keep the handle surface minimal and type-agnostic. Bind on both the
        // usertype and the backing table to ensure __index has entries.
        auto bind_both = [&](const char* name, auto&& fn) {
            // Assign on usertype
            ut[name] = fn;
            // Assign on table if missing
            set_if_absent(handleTable, name, std::forward<decltype(fn)>(fn));
        };

        bind_both("isValid", [](DisplayHandle& self) { return self.isValid(); });
        bind_both("getName", [](DisplayHandle& self) { return self.getName(); });
        bind_both("getType", [](DisplayHandle& self) { return self.getType(); });
        bind_both("setName", [](DisplayHandle& self, const std::string& n) { self.setName(n); });
        bind_both("setType", [](DisplayHandle& self, const std::string& t) { self.setType(t); });
        // Expose raw pointer fetch for advanced usage
        bind_both("get", [](DisplayHandle& self) { return self.get(); });

        // Provide a friendly __index that supports dot-style access by
        // returning a closure which binds the handle as the first argument.
        // This lets scripts use both `h:getName()` and `h.getName()`.
        ut[sol::meta_function::index] = [handleTable](DisplayHandle self, const sol::object& key, sol::this_state ts) -> sol::object {
            sol::state_view L(ts);
            if (!key.valid() || key == sol::lua_nil) return sol::lua_nil;
            if (!key.is<std::string>()) return sol::lua_nil;
            std::string k;
            try { k = key.as<std::string>(); } catch(...) { return sol::lua_nil; }

            if (!handleTable.valid()) return sol::lua_nil;
            sol::object member = handleTable.raw_get_or(k, sol::lua_nil);
            if (!member.valid() || member == sol::lua_nil) return sol::lua_nil;

            if (member.get_type() == sol::type::function) {
                sol::protected_function fn = member;
                // Return a closure that gracefully supports both colon-style
                // (self auto-inserted) and dot-style (no self) calls.
                auto wrapper = [fn, self](sol::variadic_args va, sol::this_state s) -> sol::object {
                    sol::state_view lua(s);
                    bool hasSelf = false;
                    if (va.size() > 0) {
                        auto it = va.begin();
                        sol::object first = *it;
                        try {
                            if (first.is<DisplayHandle>()) {
                                DisplayHandle maybeSelf = first.as<DisplayHandle>();
                                if (maybeSelf == self) {
                                    hasSelf = true;
                                }
                            }
                        } catch(...) {}
                    }

                    std::vector<sol::object> args;
                    if (!hasSelf) {
                        args.reserve(1 + va.size());
                        args.push_back(sol::make_object(lua, self));
                    } else {
                        args.reserve(va.size());
                    }
                    for (auto v : va) args.push_back(sol::make_object(lua, v));

                    sol::protected_function_result r = fn.call(args);
                    if (!r.valid()) {
                        return sol::make_object(lua, sol::lua_nil);
                    }
                    return r.get<sol::object>();
                };
                return sol::make_object(L, wrapper);
            }
            // Plain value (number/string/table/etc.)
            return member;
        };

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "DisplayHandle";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                      << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                      << typeName << CLR::RESET << std::endl;
        }
    }

} // namespace SDOM
