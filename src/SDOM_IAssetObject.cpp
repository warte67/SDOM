// SDOM_IAssetObject.cpp

#include <sol/sol.hpp> 
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>

namespace SDOM
{

    IAssetObject::IAssetObject(const InitStruct& init)
        : IDataObject()
        , name_(init.name)
        , type_(init.type)
        , filename_(init.filename)
        , isInternal_(init.isInternal)
        , isLoaded_(false)
    {
    }

    IAssetObject::IAssetObject(const sol::table& config)
        : IDataObject(), isLoaded_(false)
    {
        name_ = config["name"].get_or(std::string("IAssetObject"));
        type_ = config["type"].get_or(std::string("IAssetObject"));
        filename_ = config["filename"].get_or(std::string(""));
        isInternal_ = config["isInternal"].get_or(true);
    }

    IAssetObject::IAssetObject()
        : IDataObject()
        , name_(TypeName)
        , type_(TypeName)
        , filename_("")
        , isInternal_(true)
        , isLoaded_(false)
    {
    }

    IAssetObject::~IAssetObject()
    {
        // default dtor; ensure emission for vtable
    }

    void IAssetObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        if (DEBUG_REGISTER_LUA)
        {
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << "IAssetObject:" << getName()
                    << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA << typeName << CLR::RESET << std::endl;
        }

        // Create the handle usertype once (no constructor), or reuse existing.
        if (!lua[typeName].valid())
        {
            objHandleType_ = lua.new_usertype<IAssetObject>(typeName, sol::no_constructor);
        }

        sol::table handle = lua[typeName];

        // idempotent set-if-absent helper
        auto set_if_absent = [](sol::table& handle, const char* name, auto&& fn) {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                handle.set_function(name, std::forward<decltype(fn)>(fn));
            }
        };

        // Accessors
        set_if_absent(handle, "getType", &IAssetObject::getType);
        set_if_absent(handle, "getName", &IAssetObject::getName);
        set_if_absent(handle, "getFilename", &IAssetObject::getFilename);
        set_if_absent(handle, "isInternal", &IAssetObject::isInternal);
        set_if_absent(handle, "isLoaded", &IAssetObject::isLoaded);

        // Mutators
        set_if_absent(handle, "setType", &IAssetObject::setType);
        set_if_absent(handle, "setName", &IAssetObject::setName);
        set_if_absent(handle, "setFilename", &IAssetObject::setFilename);

        // Lifecycle aliases
        set_if_absent(handle, "load", [](IAssetObject& self) { self.onLoad(); });
        set_if_absent(handle, "unload", [](IAssetObject& self) { self.onUnload(); });

        // NOTE: do NOT expose virtual lifecycle method names (onLoad/onUnload) to Lua.
        // Expose only the stable 'load'/'unload' aliases that map to these operations.
    }

} // namespace SDOM