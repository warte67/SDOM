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

    // Templated forwarder factories: convert C-style wrappers of the form
    //   R fn(IDisplayObject*, Args...)
    // into concrete std::function wrappers matching sol usertype expectations
    // e.g. std::function<sol::object(DisplayObject&, Args...)> for void-returning
    // wrappers that should return nil to Lua.
    template<typename... Args>
    std::function<sol::object(DisplayObject&, Args...)> forward_void_to_idobj(void(*fn)(IDisplayObject*, Args...)) {
        return [fn](DisplayObject& self, Args... args) -> sol::object {
            IDisplayObject* obj = self.get();
            if (!obj) return sol::lua_nil;
            fn(obj, std::forward<Args>(args)...);
            return sol::lua_nil;
        };
    }

    template<typename R, typename... Args>
    std::function<R(DisplayObject&, Args...)> forward_to_idobj(R(*fn)(IDisplayObject*, Args...)) {
        return [fn](DisplayObject& self, Args... args) -> R {
            IDisplayObject* obj = self.get();
            if (!obj) return R();
            return fn(obj, std::forward<Args>(args)...);
        };
    }

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

    void DisplayObject::_registerDisplayObject(const std::string& typeName, sol::state_view lua)
    {
        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "DisplayObject";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                      << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName << CLR::RESET << std::endl;
        }

        // Idempotent registration: do nothing if already registered.
        if (lua["DisplayObject"].valid()) return;

        // sol::usertype<DisplayObject> ut = lua.new_usertype<DisplayObject>(
        //     "DisplayObject",
        //     sol::constructors<DisplayObject(), DisplayObject(const std::string&, const std::string&)>(),
        //     sol::base_classes, sol::bases<IDataObject>()
        // );

        // minimal handle surface — expose only safe, local helpers
        objHandleType_ = lua.new_usertype<DisplayObject>(
            (typeName + "Handle").c_str(),
            sol::no_constructor
        );

    // Register minimal methods here (expand as needed)
        objHandleType_.set_function("isValid", &DisplayObject::isValid);
        // NOTE: Do NOT add a large set of forwarded bindings on the lightweight
        // DisplayObject handle.  The rich IDisplayObject API should be bound on
        // the concrete IDisplayObject usertype.  Keep the handle minimal and
        // safe for Lua code to hold (name/type helpers only).
        // Forward getName/getType to the underlying IDisplayObject when possible
        objHandleType_.set_function("getName", [](DisplayObject& self) -> std::string {
            IDisplayObject* obj = self.get(); if (obj) return obj->getName(); return self.getName();
        });
        objHandleType_.set_function("getType", [](DisplayObject& self) -> std::string {
            IDisplayObject* obj = self.get(); if (obj) return obj->getType(); return self.getType();
        });
        // Expose setters that forward to the underlying IDisplayObject when available
        objHandleType_.set_function("setType", forward_void_to_idobj(::SDOM::setType_lua));

        // Provide transparent method lookup: if a function is requested on the
        // handle that's not present, look it up on the IDisplayObject usertype
        // and return a bound closure that calls it with the underlying object.
        // This enables handle:method(...) without exposing get().
    objHandleType_[sol::meta_function::index] = [lua](DisplayObject& self, const sol::object& key) -> sol::object {
            // if key is a string, attempt to find method on IDisplayObject
            if (!key.valid() || !key.is<std::string>()) return sol::lua_nil;
            auto name = key.as<std::string>();
            IDisplayObject* obj = self.get();
            if (!obj) return sol::lua_nil;
            // Prefer resolving off the concrete usertype table (e.g., Box["method"]).
            sol::object method;
            sol::object typeTableObj;
            bool methodFromTypeTable = false;
            try {
                std::string tname = obj->getType();
                typeTableObj = lua[tname];
                if (typeTableObj.valid() && typeTableObj.get_type() == sol::type::table) {
                    method = typeTableObj.as<sol::table>()[name];
                    methodFromTypeTable = (method.valid() && method.get_type() == sol::type::function);
                }
            } catch(...) { method = sol::object(); }
            // Fallback: resolve directly from the underlying userdata if type table lookup failed
            sol::object udobj = sol::make_object(lua, obj);
            sol::userdata ud = udobj.as<sol::userdata>();
            bool methodFromUserdata = false;
            if (!methodFromTypeTable) {
                sol::object m2 = ud[name];
                if (m2.valid() && m2.get_type() == sol::type::function) {
                    method = m2;
                    methodFromUserdata = true;
                }
            }
            if (!method.valid() || method.get_type() != sol::type::function) return sol::lua_nil;
            // Debug: trace for tricky names
            bool trace = (name == std::string("addEventListener")) || (name == std::string("getX")) || (name == std::string("getWidth"));
            if (trace) {
                try {
                    std::cout << "[DO.__index] resolve name='" << name << "' fromTypeTable=" << (methodFromTypeTable?"true":"false") << ", fromUserdata=" << (methodFromUserdata?"true":"false") << std::endl;
                } catch(...) {}
            }
            // Return a closure that calls the resolved method with explicit self (underlying userdata),
            // while dropping the colon-injected DisplayObject handle from Lua.
            sol::function f = method.as<sol::function>();
            sol::object self_ud = udobj; // preserve underlying userdata as self
            bool isFromUserdata = methodFromUserdata;
            // Optional: tiny diagnostic for tricky cases
            bool diag = (name == std::string("getWidth"));
            return sol::make_object(lua, [f, self_ud, isFromUserdata, diag, trace](sol::variadic_args va, sol::this_state ts) -> sol::object {
                sol::state_view L(ts);
                // Build args: forward varargs, but drop colon-injected DisplayObject handle if present
                std::vector<sol::object> args;
                args.reserve(va.size());
                bool first = true;
                for (auto v : va) {
                    // Drop the colon-injected self if present as first vararg (DisplayObject handle)
                    if (first) {
                        first = false;
                        // heuristics: if it's a userdata convertible to DisplayObject, skip it
                        bool isDO = false;
                        try { isDO = v.is<DisplayObject>(); } catch(...) { isDO = false; }
                        if (isDO) {
                            if (trace) { try { std::cout << "[DO.__index] dropping colon-injected DisplayObject arg" << std::endl; } catch(...) {} }
                            continue;
                        }
                    }
                    args.emplace_back(sol::object(v));
                }
                if (diag) {
                    // Diagnostic path for getWidth: use protected_function and introspect the result
                    try {
                        sol::protected_function pf = f;
                        sol::protected_function_result r = pf(self_ud, sol::as_args(args));
                        if (!r.valid()) {
                            sol::error err = r;
                            std::cout << "[DO.__index] getWidth protected call error: " << err.what() << std::endl;
                            return sol::lua_nil;
                        }
                        sol::object ro = r.get<sol::object>();
                        std::cout << "[DO.__index] getWidth result type=" << static_cast<int>(ro.get_type()) << std::endl;
                        return ro;
                    } catch(...) {
                        // fallback to normal call with explicit self if diagnostics fail
                        return f(self_ud, sol::as_args(args));
                    }
                }
                // Always pass explicit self to avoid ambiguous boundness on retrieved functions
                return f(self_ud, sol::as_args(args));
            });
        };

    // NOTE: DisplayObject is just a handle.  We rely on the __index shim above
    // to delegate colon-calls to the rich IDisplayObject API without exposing get().

    }

} // namespace SDOM