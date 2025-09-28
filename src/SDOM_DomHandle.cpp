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
        std::cout << "DomHandle: Registered Lua bindings." << std::endl;        
    }
  

} // END namespace SDOM