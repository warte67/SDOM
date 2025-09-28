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
        // std::cout << "DomHandle: Registered Lua bindings." << std::endl;        
    }

    void DomHandle::_registerLua(const std::string& typeName, sol::state_view lua)
    {
        std::string typeNameLocal = "DomHandle";
        std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName << CLR::RESET << std::endl;

        return;  // JUST RETURN WHILE DEVELOPING

        // 1. Create and save usertype table first
        sol::usertype<DomHandle> objHandleType = lua.new_usertype<DomHandle>(
            typeName,
            sol::constructors<DomHandle(), DomHandle(const std::string&, const std::string&)>()
        );
        this->objHandleType_ = objHandleType; // Save in IDataObject

        // 2. Call base registration
        SUPER::_registerLua(typeName, lua);

        // // 3. Register properties/commands (custom logic)
        // factory_->registerLuaProperty(typeName, "get",
        //     [](const IDataObject& obj, sol::state_view lua) { 
        //         return sol::make_object(lua, static_cast<const DomHandle&>(obj).get()); 
        //     },
        //     nullptr);

        // factory_->registerLuaProperty(typeName, "isValid",
        //     [](const IDataObject& obj, sol::state_view lua) { 
        //         return sol::make_object(lua, static_cast<const DomHandle&>(obj).isValid()); 
        //     },
        //     nullptr);

        // factory_->registerLuaProperty(typeName, "getName",
        //     [](const IDataObject& obj, sol::state_view lua) { 
        //         return sol::make_object(lua, static_cast<const DomHandle&>(obj).getName()); 
        //     },
        //     nullptr);

        // factory_->registerLuaProperty(typeName, "getType",
        //     [](const IDataObject& obj, sol::state_view lua) { 
        //         return sol::make_object(lua, static_cast<const DomHandle&>(obj).getType()); 
        //     },
        //     nullptr);

        // 4. Register properties/commands using registry
        factory_->registerLuaPropertiesAndCommands(typeName, objHandleType_);

        // 5. Register meta functions
        objHandleType[sol::meta_function::to_string] = [](DomHandle& h) 
        {
            auto obj = h.get();
            if (obj)
            {
                return "DomHandle(" + obj->getName() + ":" + obj->getType() + ")";
            }
            return std::string("DomHandle(invalid)");
        };
    }


} // END namespace SDOM