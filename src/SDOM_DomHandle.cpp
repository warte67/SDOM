// SDOM_DomHandle.cpp

#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_DomHandle.hpp>

namespace SDOM
{
    IDisplayObject* SDOM::DomHandle::get() const 
    {
        if (!factory_) return nullptr;
        return factory_->getDomObj(name_);
    }

    // --- LUA Registsration --- //

    void DomHandle::_registerLua_Usertype(sol::state_view lua)      
    { 
        SUPER::_registerLua_Usertype(lua);

        lua.new_usertype<DomHandle>("DomHandle",
                sol::constructors<DomHandle(), DomHandle(const std::string&, const std::string&)>(),
                "getName", &DomHandle::getName,
                "getType", &DomHandle::getType,
                "isValid", &DomHandle::isValid
        );
    }

    void DomHandle::_registerLua_Properties(sol::state_view lua)    
    { 
        SUPER::_registerLua_Properties(lua); 
    }

    void DomHandle::_registerLua_Commands(sol::state_view lua)      
    { 
        SUPER::_registerLua_Commands(lua); 
    }

    void DomHandle::_registerLua_Meta(sol::state_view lua)          
    { 
        SUPER::_registerLua_Meta(lua); 
    }

    void DomHandle::_registerLua_Children(sol::state_view lua)      
    { 
        SUPER::_registerLua_Children(lua); 
    }

    void DomHandle::_registerLua_All(sol::state_view lua)       
    {
        SUPER::_registerLua_All(lua);
        _registerLua_Usertype(lua);
        _registerLua_Properties(lua);
        _registerLua_Commands(lua);
        _registerLua_Meta(lua);
        _registerLua_Children(lua);

        std::cout << "DomHandle: Registered Lua bindings." << std::endl;
    }    

} // END namespace SDOM