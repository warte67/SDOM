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
        // Only name/type helpers from IDataObject are allowed on the handle
        objHandleType_.set_function("getName", &DisplayObject::getName_lua);
        objHandleType_.set_function("getType", &DisplayObject::getType_lua);
        // Allow setting name/type through the underlying IDataObject interface semantics
        objHandleType_.set_function("setName", [](DisplayObject& self, const std::string& newName) {
            self.setName(newName);
        });
        objHandleType_.set_function("setType", [](DisplayObject& self, const std::string& newType) {
            self.setType(newType);
        });
    }

} // namespace SDOM