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
        // std::cout << "ResHandle: Registered Lua bindings." << std::endl;        
    }

    void ResHandle::_registerLua(const std::string& typeName, sol::state_view lua)
    {
        std::string typeNameLocal = "ResHandle";
        std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName << CLR::RESET << std::endl;

        // 1. Call base class registration to include inherited properties/commands
        SUPER::_registerLua(typeName, lua);

        // 2. Register this class's properties and commands
        //    factory_->registerLuaProperty(typeName, ...);
        //    factory_->registerLuaCommand(typeName, ...);

        // 3. Register the Lua usertype using the registry
        factory_->registerLuaUsertype<ResHandle>(typeName, lua);
        // getFactory().registerLuaUsertype<ResHandle>(typeName, lua);          
    }



} // namespace SDOM
