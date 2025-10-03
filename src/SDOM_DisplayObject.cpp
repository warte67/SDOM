// SDOM_DisplayObject.hpp

#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IDataObject.hpp> 
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
        // add more bindings later...

        // Note: do not use 'this' at file/namespace scope. If you need to cache
        // the usertype, store it via a static variable or registry object instead.
    }

} // namespace SDOM