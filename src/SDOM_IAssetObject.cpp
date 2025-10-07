// SDOM_IAssetObject.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>

namespace SDOM
{

    IAssetObject::IAssetObject(const InitStruct& init)
        : IDataObject()
        , name_(init.name)
        , type_(init.type)
        , filename_(init.filename)
        , isInternal_(init.isInternal), isLoaded_(false)
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
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IAssetObject";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // // Bind onto the single augmentable handle: "AssetObject"
        // sol::table handle = AssetObject::ensure_handle_table(lua);

        // --- Add properties and methods to the handle as needed --- //
        sol::table assetHandle = lua["AssetObject"];
        assetHandle.set_function("getName", &IAssetObject::getName, this);
        assetHandle.set_function("getType", &IAssetObject::getType, this);
        assetHandle.set_function("getFilename", &IAssetObject::getFilename, this);
        assetHandle.set_function("isInternal", &IAssetObject::isInternal, this);
        assetHandle.set_function("isLoaded", &IAssetObject::isLoaded, this);

    } // END  void IAssetObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)

} // namespace SDOM