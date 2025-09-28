// SDOM_ResHandle.cpp

#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_ResHandle.hpp>

namespace SDOM
{
    IResourceObject* ResHandle::get() const
    {
        if (!factory_) return nullptr;
        return factory_->getResObj(name_);
    }
    // --- LUA Registsration --- //

    void ResHandle::_registerLua_Usertype(sol::state_view lua)      
    { 
        SUPER::_registerLua_Usertype(lua);

        // lua.new_usertype<ResHandle>("ResHandle",
        //     sol::constructors<ResHandle(), ResHandle(const std::string&, const std::string&)>(),
        //     "getName", &ResHandle::getName,
        //     "getType", &ResHandle::getType,
        //     "isValid", &ResHandle::isValid,
        //     "str", &ResHandle::str
        //     // Add other methods/properties as needed
        // );
    }

    void ResHandle::_registerLua_Properties(sol::state_view lua)    
    { 
        SUPER::_registerLua_Properties(lua); 
    }

    void ResHandle::_registerLua_Commands(sol::state_view lua)      
    { 
        SUPER::_registerLua_Commands(lua); 
    }

    void ResHandle::_registerLua_Meta(sol::state_view lua)          
    { 
        SUPER::_registerLua_Meta(lua); 
    }

    void ResHandle::_registerLua_Children(sol::state_view lua)      
    { 
        SUPER::_registerLua_Children(lua); 
    }

    void ResHandle::_registerLua_All(sol::state_view lua)       
    {
        SUPER::_registerLua_All(lua);
        _registerLua_Usertype(lua);
        _registerLua_Properties(lua);
        _registerLua_Commands(lua);
        _registerLua_Meta(lua);
        _registerLua_Children(lua);

        std::cout << "ResHandle: Registered Lua bindings." << std::endl;
    } 
} // namespace SDOM
