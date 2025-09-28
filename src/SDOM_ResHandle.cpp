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

        return;  // JUST RETURN WHILE DEVELOPING

        // 1. Create and save usertype table first
        sol::usertype<ResHandle> objHandleType = lua.new_usertype<ResHandle>(
            typeName,
            sol::constructors<ResHandle(), ResHandle(const std::string&, const std::string&)>()
        );
        this->objHandleType_ = objHandleType; // Save in IDataObject

        // 2. Call base registration
        SUPER::_registerLua(typeName, lua);

        // // 3. Register properties/commands (custom logic)
        // factory_->registerLuaProperty(typeName, "get",
        //     [](const IDataObject& obj, sol::state_view lua) { 
        //         return sol::make_object(lua, static_cast<const ResHandle&>(obj).get()); 
        //     },
        //     nullptr);

        // factory_->registerLuaProperty(typeName, "isValid",
        //     [](const IDataObject& obj, sol::state_view lua) { 
        //         return sol::make_object(lua, static_cast<const ResHandle&>(obj).isValid()); 
        //     },
        //     nullptr);

        // factory_->registerLuaProperty(typeName, "getName",
        //     [](const IDataObject& obj, sol::state_view lua) { 
        //         return sol::make_object(lua, static_cast<const ResHandle&>(obj).getName()); 
        //     },
        //     nullptr);

        // factory_->registerLuaProperty(typeName, "getType",
        //     [](const IDataObject& obj, sol::state_view lua) { 
        //         return sol::make_object(lua, static_cast<const ResHandle&>(obj).getType()); 
        //     },
        //     nullptr);

        // 4. Register properties/commands using registry
        factory_->registerLuaPropertiesAndCommands(typeName, objHandleType_);

        // 5. Register meta functions
        objHandleType[sol::meta_function::to_string] = [](ResHandle& h) 
        {
            auto obj = h.get();
            if (obj)
            {
                return "ResHandle(" + obj->getName() + ":" + obj->getType() + ")";
            }
            return std::string("ResHandle(invalid)");
        };
    }




} // namespace SDOM
