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
        std::cout << "ResHandle: Registered Lua bindings." << std::endl;        
    }

} // namespace SDOM
