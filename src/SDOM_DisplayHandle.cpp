// SDOM_DisplayHandle.hpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <functional>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>
#include <SDOM/SDOM_IRangeControl.hpp>

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
    return factory_->getDisplayObjectPtr(name_);
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

        // Prefer the actual usertype table; avoid creating/replacing globals here.
        sol::table handleTable;
        try {
            handleTable = lua[typeName];
        } catch(...) {
            // As a fallback (shouldn't normally happen), create a local table
            // but do not publish it globally to avoid clobbering the usertype.
            handleTable = lua.create_table();
        }

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
        ut[sol::meta_function::index] = [](DisplayHandle self, const sol::object& key, sol::this_state ts) -> sol::object {
            sol::state_view L(ts);
            if (!key.valid() || key == sol::lua_nil) return sol::lua_nil;
            if (!key.is<std::string>()) return sol::lua_nil;
            std::string k;
            try { k = key.as<std::string>(); } catch(...) { return sol::lua_nil; }

            // Fast-path for common dynamic properties on specific types
            if (k == "value") {
                if (auto* r = self.as<IRangeControl>()) {
                    return sol::make_object(L, r->getValue());
                }
                return sol::lua_nil;
            } else if (k == "min") {
                if (auto* r = self.as<IRangeControl>()) {
                    return sol::make_object(L, r->getMin());
                }
                return sol::lua_nil;
            } else if (k == "max") {
                if (auto* r = self.as<IRangeControl>()) {
                    return sol::make_object(L, r->getMax());
                }
                return sol::lua_nil;
            }

            // Resolve from canonical SDOM_Bindings[type][k] first,
            // then fall back to legacy globals[type][k], then DisplayHandle table.
            sol::object member = sol::make_object(L, sol::lua_nil);
            std::string typeKeyDyn;
            try { typeKeyDyn = self.getType(); } catch(...) { typeKeyDyn.clear(); }

            // 1) SDOM_Bindings[type][k]
            if (!typeKeyDyn.empty()) {
                try {
                    sol::table bindingsRoot;
                    try { bindingsRoot = L["SDOM_Bindings"]; } catch(...) {}
                    if (bindingsRoot.valid()) {
                        sol::object typeTblObj = bindingsRoot.raw_get_or(typeKeyDyn, sol::lua_nil);
                        if (typeTblObj.valid() && typeTblObj != sol::lua_nil) {
                            sol::table typeTbl = typeTblObj.as<sol::table>();
                            sol::object mm = typeTbl.raw_get_or(k, sol::lua_nil);
                            if (mm.valid() && mm != sol::lua_nil) {
                                member = mm;
                            }
                        }
                    }
                } catch(...) {}
            }

            // 2) Legacy: globals[type][k]
            if (!member.valid() || member == sol::lua_nil) {
                try {
                    sol::table tbl;
                    if (!typeKeyDyn.empty()) {
                        sol::object maybe = L.globals().raw_get_or(typeKeyDyn, sol::lua_nil);
                        if (maybe.valid()) {
                            if (maybe.is<sol::table>()) {
                                tbl = maybe.as<sol::table>();
                            } else {
                                // Fallback: attempt to read as table directly
                                tbl = L[typeKeyDyn];
                            }
                        }
                    }
                    if (tbl.valid()) {
                        sol::object mm = tbl.raw_get_or(k, sol::lua_nil);
                        if (mm.valid() && mm != sol::lua_nil)
                            member = mm;
                    }
                } catch(...) {}
            }

            // 3) Minimal DisplayHandle surface
            if (!member.valid() || member == sol::lua_nil) {
                try {
                    sol::object maybeDH = L.globals().raw_get_or(SDOM::DisplayHandle::LuaHandleName, sol::lua_nil);
                    if (maybeDH.valid()) {
                        sol::table dhTbl = maybeDH.is<sol::table>() ? maybeDH.as<sol::table>() : L[SDOM::DisplayHandle::LuaHandleName];
                        if (dhTbl.valid()) {
                            sol::object mm2 = dhTbl.raw_get_or(k, sol::lua_nil);
                            if (mm2.valid() && mm2 != sol::lua_nil)
                                member = mm2;
                        }
                    }
                } catch(...) {}
                if (!member.valid() || member == sol::lua_nil) return sol::lua_nil;
            }

            if (member.get_type() == sol::type::function) {
                sol::protected_function fn = member;
                // Return a closure that gracefully supports both colon-style
                // (self auto-inserted) and dot-style (no self) calls. If a
                // call with DisplayHandle self fails, retry with raw pointer.
                auto wrapper = [fn, self](sol::variadic_args va, sol::this_state s) -> sol::object {
                    sol::state_view lua(s);
                    bool selfIsFirstHandle = false;
                    if (va.size() > 0) {
                        auto it = va.begin();
                        sol::object first = *it;
                        try {
                            if (first.is<DisplayHandle>()) {
                                DisplayHandle maybeSelf = first.as<DisplayHandle>();
                                if (maybeSelf == self) {
                                    selfIsFirstHandle = true;
                                }
                            }
                        } catch(...) {}
                    }

                    auto build_args_with = [&](const sol::object& selfObj) {
                        std::vector<sol::object> out;
                        if (!selfIsFirstHandle) {
                            out.reserve(1 + va.size());
                            out.push_back(selfObj);
                            for (auto v : va) out.push_back(sol::make_object(lua, v));
                        } else {
                            // Replace the first argument (self) with the provided selfObj
                            out.reserve(va.size());
                            bool first = true;
                            for (auto v : va) {
                                if (first) {
                                    out.push_back(selfObj);
                                    first = false;
                                } else {
                                    out.push_back(sol::make_object(lua, v));
                                }
                            }
                        }
                        return out;
                    };

                    // First attempt: pass DisplayHandle as self
                    std::vector<sol::object> args1 = build_args_with(sol::make_object(lua, self));
                    sol::protected_function_result r = fn.call(args1);
                    if (r.valid()) {
                        return r.get<sol::object>();
                    }

                    // Fallback: pass underlying IDisplayObject* as self
                    IDisplayObject* raw = self.get();
                    std::vector<sol::object> args2 = build_args_with(sol::make_object(lua, raw));
                    sol::protected_function_result r2 = fn.call(args2);
                    if (!r2.valid()) {
                        return sol::make_object(lua, sol::lua_nil);
                    }
                    return r2.get<sol::object>();
                };
                return sol::make_object(L, wrapper);
            }
            // Plain value (number/string/table/etc.)
            return member;
        };

        // Support property-style assignment: h.value = x, h.min = a, h.max = b
        ut[sol::meta_function::new_index] = [](DisplayHandle self, const sol::object& key, const sol::object& value) {
            if (!key.valid() || !key.is<std::string>()) return;
            std::string k;
            try { k = key.as<std::string>(); } catch(...) { return; }

            auto to_float = [](const sol::object& v, float def) -> float {
                try { if (v.is<double>()) return static_cast<float>(v.as<double>()); } catch(...) {}
                try { if (v.is<int>()) return static_cast<float>(v.as<int>()); } catch(...) {}
                try { if (v.is<long>()) return static_cast<float>(v.as<long>()); } catch(...) {}
                return def;
            };

            if (k == "value") {
                if (auto* r = self.as<IRangeControl>()) {
                    r->setValue(to_float(value, r->getValue()));
                }
                return;
            }
            if (k == "min") {
                if (auto* r = self.as<IRangeControl>()) {
                    r->setMin(to_float(value, r->getMin()));
                }
                return;
            }
            if (k == "max") {
                if (auto* r = self.as<IRangeControl>()) {
                    r->setMax(to_float(value, r->getMax()));
                }
                return;
            }
            // Unknown keys: ignore assignment to keep usertype stable
        };

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "DisplayHandle";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                      << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                      << typeName << CLR::RESET << std::endl;
        }
    }

    void DisplayHandle::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "DisplayHandle");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    

} // namespace SDOM
