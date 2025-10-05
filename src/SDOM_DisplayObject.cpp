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

        sol::usertype<DisplayObject> ut = lua.new_usertype<DisplayObject>(
            "DisplayObject",
            sol::constructors<DisplayObject(), DisplayObject(const std::string&, const std::string&)>(),
            sol::base_classes, sol::bases<IDataObject>()
        );

        // Use the templated forwarder factories declared at namespace scope above
        // (forward_void_to_idobj and forward_to_idobj). They produce concrete
        // std::function wrappers which satisfy sol2's usertype registration
        // requirements. Avoid defining local generic lambdas here which would
        // not expose the required function pointer traits to sol.

        // Register minimal methods here (expand as needed)
        ut.set_function("isValid", &DisplayObject::isValid);
        // Backwards-compat: return the handle itself for code that calls obj:get():method()
        ut.set_function("get", [](DisplayObject& self) -> DisplayObject { return self; });
        // Forward getName/getType to the underlying IDisplayObject when possible
        ut.set_function("getName", [](DisplayObject& self) -> std::string {
            IDisplayObject* obj = self.get(); if (obj) return obj->getName(); return self.getName();
        });
        ut.set_function("getType", [](DisplayObject& self) -> std::string {
            IDisplayObject* obj = self.get(); if (obj) return obj->getType(); return self.getType();
        });
        // Expose setters that forward to the underlying IDisplayObject when available
        ut.set_function("setType", forward_void_to_idobj(::SDOM::setType_lua));
        // Forward common IDisplayObject operations through the DisplayObject handle
        ut.set_function("addEventListener", sol::overload(
            forward_void_to_idobj(::SDOM::addEventListener_lua_any),
            forward_void_to_idobj(::SDOM::addEventListener_lua_any_short)
        ));
        ut.set_function("removeEventListener", sol::overload(
            forward_void_to_idobj(::SDOM::removeEventListener_lua_any),
            forward_void_to_idobj(::SDOM::removeEventListener_lua_any_short)
        ));
        ut.set_function("addChild", forward_void_to_idobj(::SDOM::addChild_lua));
        // Expose removeChild to Lua with two overloads: by handle or by name
        ut.set_function("removeChild", sol::overload(
            [](DisplayObject& self, DisplayObject child) -> bool {
                IDisplayObject* obj = self.get(); if (!obj) return false;
                return ::SDOM::removeChild_lua(obj, child);
            },
            [](DisplayObject& self, const std::string& name) -> bool {
                IDisplayObject* obj = self.get(); if (!obj) return false;
                return ::SDOM::removeChild_lua(obj, name);
            }
        ));
        ut.set_function("hasChild", [](DisplayObject& self, DisplayObject child) -> bool {
            IDisplayObject* obj = self.get(); if (!obj) return false;
            return ::SDOM::hasChild_lua(obj, child);
        });
        ut.set_function("getParent", [](DisplayObject& self) -> DisplayObject {
            IDisplayObject* obj = self.get(); if (!obj) return DisplayObject();
            return ::SDOM::getParent_lua(obj);
        });
        // Allow DisplayObject handles in Lua to call removeFromParent() convenience
        ut.set_function("removeFromParent", [](DisplayObject& self) -> bool {
            IDisplayObject* obj = self.get(); if (!obj) return false;
            return ::SDOM::removeFromParent_lua(obj);
        });
        ut.set_function("setName", [](DisplayObject& self, const std::string& newName) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil; obj->setName(newName); return sol::lua_nil;
        });
        // Forward removeDescendant to the underlying IDisplayObject so scripts
        // can call `handle:removeDescendant(child)` or `handle:removeDescendant("name")` on DisplayObject handles.
        ut.set_function("removeDescendant", sol::overload(
            [](DisplayObject& self, DisplayObject descendant) -> bool {
                IDisplayObject* obj = self.get(); if (!obj) return false;
                return ::SDOM::removeDescendant_lua(obj, descendant);
            },
            [](DisplayObject& self, const std::string& name) -> bool {
                IDisplayObject* obj = self.get(); if (!obj) return false;
                return ::SDOM::removeDescendant_lua(obj, name);
            }
        ));
        // Type & property access
        ut.set_function("getBounds", [](DisplayObject& self) -> Bounds {
            IDisplayObject* obj = self.get(); if (!obj) return Bounds();
            return ::SDOM::getBounds_lua(obj);
        });
    ut.set_function("setBounds", forward_void_to_idobj(static_cast<void(*)(IDisplayObject*, const sol::object&)>(&::SDOM::setBounds_lua)));
        ut.set_function("getColor", [](DisplayObject& self) -> SDL_Color {
            IDisplayObject* obj = self.get(); if (!obj) return SDL_Color{0,0,0,0};
            return ::SDOM::getColor_lua(obj);
        });
    ut.set_function("setColor", forward_void_to_idobj(static_cast<void(*)(IDisplayObject*, const sol::object&)>(&::SDOM::setColor_lua)));

        // Orphan retention helpers forwarded to the underlying IDisplayObject
        ut.set_function("orphanPolicyFromString", [](DisplayObject& self, const std::string& s) -> IDisplayObject::OrphanRetentionPolicy {
            IDisplayObject* obj = self.get(); if (!obj) return IDisplayObject::OrphanRetentionPolicy::RetainUntilManual;
            return ::SDOM::orphanPolicyFromString_lua(obj, s);
        });
        ut.set_function("orphanPolicyToString", [](DisplayObject& self, IDisplayObject::OrphanRetentionPolicy p) -> std::string {
            IDisplayObject* obj = self.get(); if (!obj) return std::string();
            return ::SDOM::orphanPolicyToString_lua(obj, p);
        });
        ut.set_function("setOrphanRetentionPolicy", forward_void_to_idobj(::SDOM::setOrphanRetentionPolicy_lua));
        ut.set_function("getOrphanRetentionPolicyString", [](DisplayObject& self) -> std::string {
            IDisplayObject* obj = self.get(); if (!obj) return std::string();
            return ::SDOM::getOrphanRetentionPolicyString_lua(obj);
        });

        // Geometry & layout
        ut.set_function("getX", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getX_lua(obj); });
        ut.set_function("getY", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getY_lua(obj); });
        ut.set_function("getWidth", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getWidth_lua(obj); });
        ut.set_function("getHeight", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getHeight_lua(obj); });
    ut.set_function("setX", forward_void_to_idobj(::SDOM::setX_lua));
    ut.set_function("setY", forward_void_to_idobj(::SDOM::setY_lua));
    ut.set_function("setWidth", forward_void_to_idobj(::SDOM::setWidth_lua));
    ut.set_function("setHeight", forward_void_to_idobj(::SDOM::setHeight_lua));

    // Priority & Z-Order helpers forwarded from IDisplayObject
    ut.set_function("setPriority", sol::overload(
        forward_void_to_idobj(static_cast<void(*)(IDisplayObject*, int)>(&::SDOM::setPriority_lua)),
        [](DisplayObject& self, const sol::object& descriptor, int p) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            DisplayObject target = DisplayObject::resolveChildSpec(descriptor);
            IDisplayObject* targetObj = target.get(); if (!targetObj) return sol::lua_nil;
            ::SDOM::setPriority_lua(targetObj, p);
            return sol::lua_nil;
        },
        // Accept a single-table form: setPriority({ priority = N })
        [](DisplayObject& self, const sol::object& descriptor) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            if (!descriptor.is<sol::table>()) return sol::lua_nil;
            sol::table t = descriptor.as<sol::table>();
            sol::object pobj = t.get<sol::object>("priority");
            if (!pobj.valid()) return sol::lua_nil;
            try {
                int p = pobj.as<int>();
                ::SDOM::setPriority_lua(obj, p);
            } catch(...) {
                // ignore conversion errors
            }
            return sol::lua_nil;
        }
    ));
    ut.set_function("getPriority", [](DisplayObject& self) -> int { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getPriority_lua(obj); });
    ut.set_function("getMaxPriority", [](DisplayObject& self) -> int { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getMaxPriority_lua(obj); });
    ut.set_function("getMinPriority", [](DisplayObject& self) -> int { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getMinPriority_lua(obj); });
    ut.set_function("setToHighestPriority", sol::overload(
        forward_void_to_idobj(::SDOM::setToHighestPriority_lua),
        [](DisplayObject& self, const sol::object& descriptor) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            DisplayObject target = DisplayObject::resolveChildSpec(descriptor);
            IDisplayObject* targetObj = target.get(); if (!targetObj) return sol::lua_nil;
            ::SDOM::setToHighestPriority_lua(targetObj);
            return sol::lua_nil;
        }
    ));
    ut.set_function("setToLowestPriority", sol::overload(
        forward_void_to_idobj(::SDOM::setToLowestPriority_lua),
        [](DisplayObject& self, const sol::object& descriptor) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            DisplayObject target = DisplayObject::resolveChildSpec(descriptor);
            IDisplayObject* targetObj = target.get(); if (!targetObj) return sol::lua_nil;
            ::SDOM::setToLowestPriority_lua(targetObj);
            return sol::lua_nil;
        }
    ));
    ut.set_function("sortChildrenByPriority", forward_void_to_idobj(::SDOM::sortChildrenByPriority_lua));
    ut.set_function("getChildrenPriorities", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return std::vector<int>(); return ::SDOM::getChildrenPriorities_lua(obj); });
    ut.set_function("getZOrder", [](DisplayObject& self) -> int { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getZOrder_lua(obj); });
    // setZOrder overloads: numeric, (descriptor, z), and single-table { child=..., z=... }
    ut.set_function("setZOrder", sol::overload(
        forward_void_to_idobj(::SDOM::setZOrder_lua),
        [](DisplayObject& self, const sol::object& descriptor, int z) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            DisplayObject target = DisplayObject::resolveChildSpec(descriptor);
            IDisplayObject* targetObj = target.get(); if (!targetObj) return sol::lua_nil;
            ::SDOM::setZOrder_lua(targetObj, z);
            return sol::lua_nil;
        },
        [](DisplayObject& self, const sol::object& descriptor) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            if (!descriptor.is<sol::table>()) return sol::lua_nil;
            sol::table t = descriptor.as<sol::table>();
            sol::object childObj = t.get<sol::object>("child");
            sol::object zObj = t.get<sol::object>("z");
            if (!childObj.valid() || !zObj.valid()) return sol::lua_nil;
            int z = 0;
            try { z = zObj.as<int>(); } catch(...) { return sol::lua_nil; }
            DisplayObject target = DisplayObject::resolveChildSpec(childObj);
            IDisplayObject* targetObj = target.get(); if (!targetObj) return sol::lua_nil;
            ::SDOM::setZOrder_lua(targetObj, z);
            return sol::lua_nil;
        }
    ));
    ut.set_function("moveToTop", sol::overload(
        forward_void_to_idobj(::SDOM::moveToTop_lua),
        [](DisplayObject& self, const sol::object& descriptor) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            DisplayObject target = DisplayObject::resolveChildSpec(descriptor);
            IDisplayObject* targetObj = target.get(); if (!targetObj) return sol::lua_nil;
            ::SDOM::moveToTop_lua(targetObj);
            return sol::lua_nil;
        }
    ));
    // Provide DisplayObject-level overloads that accept an optional child descriptor
    // so scripts can call stage:setToHighestPriority({ child = 'name' })
        ut.set_function("setToHighestPriority", sol::overload(
        forward_void_to_idobj(::SDOM::setToHighestPriority_lua),
        [](DisplayObject& self, const sol::object& descriptor) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            DisplayObject target = DisplayObject::resolveChildSpec(descriptor);
            IDisplayObject* targetObj = target.get(); if (!targetObj) return sol::lua_nil;
            ::SDOM::setToHighestPriority_lua(targetObj);
            return sol::lua_nil;
        }
    ));
    ut.set_function("setToLowestPriority", sol::overload(
        forward_void_to_idobj(::SDOM::setToLowestPriority_lua),
        [](DisplayObject& self, const sol::object& descriptor) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            DisplayObject target = DisplayObject::resolveChildSpec(descriptor);
            IDisplayObject* targetObj = target.get(); if (!targetObj) return sol::lua_nil;
            ::SDOM::setToLowestPriority_lua(targetObj);
            return sol::lua_nil;
        }
    ));

        // Edge positions
        ut.set_function("getLeft", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0.0f; return ::SDOM::getLeft_lua(obj); });
        ut.set_function("getRight", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0.0f; return ::SDOM::getRight_lua(obj); });
        ut.set_function("getTop", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0.0f; return ::SDOM::getTop_lua(obj); });
        ut.set_function("getBottom", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0.0f; return ::SDOM::getBottom_lua(obj); });
    ut.set_function("setLeft", forward_void_to_idobj(::SDOM::setLeft_lua));
    ut.set_function("setRight", forward_void_to_idobj(::SDOM::setRight_lua));
    ut.set_function("setTop", forward_void_to_idobj(::SDOM::setTop_lua));
    ut.set_function("setBottom", forward_void_to_idobj(::SDOM::setBottom_lua));
        ut.set_function("setParent", sol::overload(
            [](DisplayObject& self, const DisplayObject& parent) -> DisplayObject {
                IDisplayObject* obj = self.get(); if (!obj) return DisplayObject();
                ::SDOM::setParent_lua(obj, parent);
                return ::SDOM::getParent_lua(obj);
            },
            [](DisplayObject& self, const sol::object& descriptor) -> DisplayObject {
                IDisplayObject* obj = self.get(); if (!obj) return DisplayObject();
                DisplayObject parent = DisplayObject::resolveChildSpec(descriptor);
                ::SDOM::setParent_lua(obj, parent);
                return ::SDOM::getParent_lua(obj);
            }
        ));
        // Focus, interactivity and tab-related bindings forwarded from IDisplayObject
    ut.set_function("setKeyboardFocus", forward_void_to_idobj(::SDOM::setKeyboardFocus_lua));
        ut.set_function("isKeyboardFocused", [](DisplayObject& self) -> bool { IDisplayObject* obj = self.get(); if (!obj) return false; return ::SDOM::isKeyboardFocused_lua(obj); });
        ut.set_function("isMouseHovered", [](DisplayObject& self) -> bool { IDisplayObject* obj = self.get(); if (!obj) return false; return ::SDOM::isMouseHovered_lua(obj); });

        ut.set_function("isClickable", [](DisplayObject& self) -> bool { IDisplayObject* obj = self.get(); if (!obj) return false; return ::SDOM::isClickable_lua(obj); });
    ut.set_function("setClickable", forward_void_to_idobj(::SDOM::setClickable_lua));

        ut.set_function("isEnabled", [](DisplayObject& self) -> bool { IDisplayObject* obj = self.get(); if (!obj) return false; return ::SDOM::isEnabled_lua(obj); });
    ut.set_function("setEnabled", forward_void_to_idobj(::SDOM::setEnabled_lua));

        ut.set_function("isHidden", [](DisplayObject& self) -> bool { IDisplayObject* obj = self.get(); if (!obj) return false; return ::SDOM::isHidden_lua(obj); });
    ut.set_function("setHidden", forward_void_to_idobj(::SDOM::setHidden_lua));

        ut.set_function("isVisible", [](DisplayObject& self) -> bool { IDisplayObject* obj = self.get(); if (!obj) return false; return ::SDOM::isVisible_lua(obj); });
    ut.set_function("setVisible", forward_void_to_idobj(::SDOM::setVisible_lua));

        // Tab navigation helpers
        ut.set_function("getTabPriority", [](DisplayObject& self) -> int { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getTabPriority_lua(obj); });
    ut.set_function("setTabPriority", forward_void_to_idobj(::SDOM::setTabPriority_lua));
        ut.set_function("isTabEnabled", [](DisplayObject& self) -> bool { IDisplayObject* obj = self.get(); if (!obj) return false; return ::SDOM::isTabEnabled_lua(obj); });
    ut.set_function("setTabEnabled", forward_void_to_idobj(::SDOM::setTabEnabled_lua));

        // add more bindings later...

        // Note: do not use 'this' at file/namespace scope. If you need to cache
        // the usertype, store it via a static variable or registry object instead.
    }

} // namespace SDOM