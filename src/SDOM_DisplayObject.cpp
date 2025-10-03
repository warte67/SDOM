// SDOM_DisplayObject.hpp

#include <SDOM/SDOM_Factory.hpp>
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

        // Register minimal methods here (expand as needed)
        ut.set_function("isValid", &DisplayObject::isValid);
        ut.set_function("getName", &DisplayObject::getName);
        ut.set_function("getType", &DisplayObject::getType);
        // Forward common IDisplayObject operations through the DisplayObject handle
        ut.set_function("addEventListener", sol::overload(
            [](DisplayObject& self, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture, const sol::object& maybe_priority) {
                IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
                ::SDOM::addEventListener_lua_any(obj, descriptor, maybe_listener, maybe_useCapture, maybe_priority);
                return sol::lua_nil;
            },
            [](DisplayObject& self, const sol::object& descriptor) {
                IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
                ::SDOM::addEventListener_lua_any_short(obj, descriptor);
                return sol::lua_nil;
            }
        ));
        ut.set_function("removeEventListener", sol::overload(
            [](DisplayObject& self, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture) {
                IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
                ::SDOM::removeEventListener_lua_any(obj, descriptor, maybe_listener, maybe_useCapture);
                return sol::lua_nil;
            },
            [](DisplayObject& self, const sol::object& descriptor) {
                IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
                ::SDOM::removeEventListener_lua_any_short(obj, descriptor);
                return sol::lua_nil;
            }
        ));
        ut.set_function("addChild", [](DisplayObject& self, DisplayObject child) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            ::SDOM::addChild_lua(obj, child);
            return sol::lua_nil;
        });
        ut.set_function("hasChild", [](DisplayObject& self, DisplayObject child) -> bool {
            IDisplayObject* obj = self.get(); if (!obj) return false;
            return ::SDOM::hasChild_lua(obj, child);
        });
        ut.set_function("getParent", [](DisplayObject& self) -> DisplayObject {
            IDisplayObject* obj = self.get(); if (!obj) return DisplayObject();
            return ::SDOM::getParent_lua(obj);
        });
        // Type & property access
        ut.set_function("getBounds", [](DisplayObject& self) -> Bounds {
            IDisplayObject* obj = self.get(); if (!obj) return Bounds();
            return ::SDOM::getBounds_lua(obj);
        });
        ut.set_function("setBounds", [](DisplayObject& self, const Bounds& b) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            ::SDOM::setBounds_lua(obj, b);
            return sol::lua_nil;
        });
        ut.set_function("getColor", [](DisplayObject& self) -> SDL_Color {
            IDisplayObject* obj = self.get(); if (!obj) return SDL_Color{0,0,0,0};
            return ::SDOM::getColor_lua(obj);
        });
        ut.set_function("setColor", [](DisplayObject& self, const SDL_Color& c) {
            IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil;
            ::SDOM::setColor_lua(obj, c);
            return sol::lua_nil;
        });

        // Geometry & layout
        ut.set_function("getX", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getX_lua(obj); });
        ut.set_function("getY", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getY_lua(obj); });
        ut.set_function("getWidth", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getWidth_lua(obj); });
        ut.set_function("getHeight", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0; return ::SDOM::getHeight_lua(obj); });
        ut.set_function("setX", [](DisplayObject& self, int x) { IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil; ::SDOM::setX_lua(obj, x); return sol::lua_nil; });
        ut.set_function("setY", [](DisplayObject& self, int y) { IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil; ::SDOM::setY_lua(obj, y); return sol::lua_nil; });
        ut.set_function("setWidth", [](DisplayObject& self, int w) { IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil; ::SDOM::setWidth_lua(obj, w); return sol::lua_nil; });
        ut.set_function("setHeight", [](DisplayObject& self, int h) { IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil; ::SDOM::setHeight_lua(obj, h); return sol::lua_nil; });

        // Edge positions
        ut.set_function("getLeft", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0.0f; return ::SDOM::getLeft_lua(obj); });
        ut.set_function("getRight", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0.0f; return ::SDOM::getRight_lua(obj); });
        ut.set_function("getTop", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0.0f; return ::SDOM::getTop_lua(obj); });
        ut.set_function("getBottom", [](DisplayObject& self) { IDisplayObject* obj = self.get(); if (!obj) return 0.0f; return ::SDOM::getBottom_lua(obj); });
        ut.set_function("setLeft", [](DisplayObject& self, float v) { IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil; ::SDOM::setLeft_lua(obj, v); return sol::lua_nil; });
        ut.set_function("setRight", [](DisplayObject& self, float v) { IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil; ::SDOM::setRight_lua(obj, v); return sol::lua_nil; });
        ut.set_function("setTop", [](DisplayObject& self, float v) { IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil; ::SDOM::setTop_lua(obj, v); return sol::lua_nil; });
        ut.set_function("setBottom", [](DisplayObject& self, float v) { IDisplayObject* obj = self.get(); if (!obj) return sol::lua_nil; ::SDOM::setBottom_lua(obj, v); return sol::lua_nil; });
        ut.set_function("setParent", sol::overload(
            [](DisplayObject& self, const DisplayObject& parent) -> DisplayObject {
                IDisplayObject* obj = self.get(); if (!obj) return DisplayObject();
                try { std::cout << "[dbg:setParent] self='" << self.getName() << "' parent='" << parent.getName() << "'" << std::endl; } catch(...) {}
                ::SDOM::setParent_lua(obj, parent);
                return ::SDOM::getParent_lua(obj);
            },
            [](DisplayObject& self, const sol::object& descriptor) -> DisplayObject {
                IDisplayObject* obj = self.get(); if (!obj) return DisplayObject();
                DisplayObject parent = DisplayObject::resolveChildSpec(descriptor);
                try { std::string pname = parent.isValid() ? parent.getName() : std::string("<nil>"); std::cout << "[dbg:setParent] self='" << self.getName() << "' resolved parent='" << pname << "'" << std::endl; } catch(...) {}
                ::SDOM::setParent_lua(obj, parent);
                return ::SDOM::getParent_lua(obj);
            }
        ));
        // add more bindings later...

        // Note: do not use 'this' at file/namespace scope. If you need to cache
        // the usertype, store it via a static variable or registry object instead.
    }

} // namespace SDOM