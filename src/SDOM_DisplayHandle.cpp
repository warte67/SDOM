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
        // IMPORTANT: Always register under the canonical DisplayHandle name, never per-type.
        sol::usertype<DisplayHandle> ut = SDOM::IDataObject::register_usertype_with_table<DisplayHandle, SDOM::IDataObject>(lua, SDOM::DisplayHandle::LuaHandleName);

        // No shared DisplayHandle table creation; usertype-only bindings

        // Minimal usertype-only surface (do not rely on a shared table)
        ut["isValid"] = [](DisplayHandle& self) { return self.isValid(); };
        ut["getName"] = [](DisplayHandle& self) { return self.getName(); };
        ut["getType"] = [](DisplayHandle& self) { return self.getType(); };
        ut["setName"] = [](DisplayHandle& self, const std::string& n) { self.setName(n); };
        ut["setType"] = [](DisplayHandle& self, const std::string& t) { self.setType(t); };
        ut["get"]     = [](DisplayHandle& self) { return self.get(); };

        // Provide a friendly __index that supports dot-style access by
        // returning a closure which binds the handle as the first argument.
        // This lets scripts use both `h:getName()` and `h.getName()`.
        ut[sol::meta_function::index] = [](DisplayHandle self, const sol::object& key, sol::this_state ts) -> sol::object {
            sol::state_view L(ts);
            if (!key.valid() || key == sol::lua_nil) return sol::lua_nil;
            if (!key.is<std::string>()) return sol::lua_nil;
            std::string k;
            try { k = key.as<std::string>(); } catch(...) { return sol::lua_nil; }

            // Resolve from canonical SDOM_Bindings[type][k] only.
            sol::object member = sol::make_object(L, sol::lua_nil);
            std::string typeKeyDyn;
            try { typeKeyDyn = self.getType(); } catch(...) { typeKeyDyn.clear(); }

            // 1) Prefer storage: _SDOM_Bindings_Storage[type][k]
            if (!typeKeyDyn.empty()) {
                try {
                    sol::object storObj = L["_SDOM_Bindings_Storage"];
                    if (storObj.valid() && storObj.is<sol::table>()) {
                        sol::table stor = storObj.as<sol::table>();
                        sol::object typeTblObj = stor.raw_get_or(typeKeyDyn, sol::lua_nil);
                        if (typeTblObj.valid() && typeTblObj.is<sol::table>()) {
                            sol::table typeTbl = typeTblObj.as<sol::table>();
                            sol::object mm = typeTbl.raw_get_or(k, sol::lua_nil);
                            if (mm.valid() && mm != sol::lua_nil) {
                                member = mm;
                            }
                        }
                    }
                } catch(...) {}
            }

            // 2) Fallback to SDOM_Bindings proxy in case storage unavailable
            if ((!member.valid() || member == sol::lua_nil) && !typeKeyDyn.empty()) {
                try {
                    sol::table bindingsRoot = L["SDOM_Bindings"];
                    if (bindingsRoot.valid()) {
                        sol::object typeTblObj = bindingsRoot[typeKeyDyn];
                        if (typeTblObj.valid() && typeTblObj.is<sol::table>()) {
                            sol::table typeTbl = typeTblObj.as<sol::table>();
                            sol::object mm = typeTbl.raw_get_or(k, sol::lua_nil);
                            if (mm.valid() && mm != sol::lua_nil) {
                                member = mm;
                            }
                        }
                    }
                } catch(...) {}
            }

            // Strict failure if missing binding
            if (!member.valid() || member == sol::lua_nil) {
                // Build a brief list of available keys to aid debugging
                std::string keys;
                bool typeTableWasTable = false;
                bool rootValid = false;
                bool rootIsTable = false;
                bool guardPresent = false;
                int typeKind = -1;
                int storageKind = -1;
                try {
                    sol::object rootObj = L["SDOM_Bindings"];
                    if (rootObj.valid() && rootObj != sol::lua_nil) {
                        rootValid = true;
                        rootIsTable = rootObj.get_type() == sol::type::table;
                    }
                    sol::table bindingsRoot;
                    if (rootIsTable) bindingsRoot = rootObj.as<sol::table>();
                    sol::object typeObj = bindingsRoot[typeKeyDyn];
                    if (typeObj.valid()) typeKind = static_cast<int>(typeObj.get_type());
                    // Also inspect storage directly if available (diagnostic)
                    try {
                        sol::object storObj = L["_SDOM_Bindings_Storage"];
                        if (storObj.valid() && storObj.is<sol::table>()) {
                            sol::table stor = storObj.as<sol::table>();
                            sol::object st = stor.raw_get_or(typeKeyDyn, sol::lua_nil);
                            if (st.valid()) storageKind = static_cast<int>(st.get_type());
                        }
                    } catch(...) {}

                    if (typeObj.valid() && typeObj.is<sol::table>()) {
                        typeTableWasTable = true;
                        sol::table t = typeObj.as<sol::table>();
                        try { sol::object g = t.raw_get_or("__sdom_guard", sol::lua_nil); guardPresent = g.valid() && g != sol::lua_nil; } catch(...) {}
                        int count = 0;
                        for (auto&& kv : t) {
                            if (count >= 20) { keys += ", ..."; break; }
                            sol::object keyo = kv.first;
                            if (keyo.valid() && keyo.is<std::string>()) {
                                if (!keys.empty()) keys += ", ";
                                keys += keyo.as<std::string>();
                                ++count;
                            }
                        }
                    }
                } catch(...) {}
                try {
                    std::cout << "[LuaLookup] Missing type='" << typeKeyDyn
                              << "' key='" << k << "' available=[" << keys << "]"
                              << " rootValid=" << (rootValid ? "true" : "false")
                              << " rootIsTable=" << (rootIsTable ? "true" : "false")
                              << " tableWasTable=" << (typeTableWasTable ? "true" : "false")
                              << " guard=" << (guardPresent ? "present" : "absent")
                              << " kind=" << typeKind
                              << " storageKind=" << storageKind
                              << std::endl;
                } catch(...) {}
                std::string msg = std::string("[Lua Binding Missing] type='") + typeKeyDyn + "' key='" + k + "'";
                if (!keys.empty()) msg += std::string(" available=[") + keys + "]";
                ERROR(msg);
                return sol::lua_nil;
            }

            // No fallback to DisplayHandle table: enforce strict per-type registry

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

        // Disallow property-style assignment; require explicit setters in SDOM_Bindings
        ut[sol::meta_function::new_index] = [](DisplayHandle self, const sol::object& key, const sol::object& /*value*/) {
            if (!key.valid() || !key.is<std::string>()) return;
            std::string k;
            try { k = key.as<std::string>(); } catch(...) { return; }
            std::string typeKeyDyn;
            try { typeKeyDyn = self.getType(); } catch(...) { typeKeyDyn.clear(); }
            std::string msg = std::string("[Lua Assignment Unsupported] type='") + typeKeyDyn + "' key='" + k + "' (use explicit setter in SDOM_Bindings)";
            ERROR(msg);
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
