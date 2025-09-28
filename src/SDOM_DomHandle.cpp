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
        // std::cout << CLR::YELLOW << "DomHandle: Registered Lua usertype" << CLR::RESET << std::endl;

        SUPER::_registerLua_Usertype(lua);

        lua.new_usertype<DomHandle>("DomHandle",
            sol::constructors<DomHandle(), DomHandle(const std::string&, const std::string&)>(),
            "get", &DomHandle::get,
            "isValid", &DomHandle::isValid,
            "getName", &DomHandle::getName,
            "getType", &DomHandle::getType,
            sol::meta_function::to_string, [](DomHandle& h) 
            {
                auto obj = h.get();
                if (obj) 
                {
                    return "DomHandle(" + obj->getName() + ":" + obj->getType() + ")";
                }
                return std::string("DomHandle(invalid)");
            }
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

        // sol::usertype<DomHandle> domHandleMeta = lua["DomHandle"];
        // domHandleMeta[sol::meta_function::to_string] = [](DomHandle& h) {
        //     auto obj = h.get();
        //     if (obj) {
        //         return "DomHandle(" + obj->getName() + ":" + obj->getType() + ")";
        //     }
        //     return std::string("DomHandle(invalid)");
        // };
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