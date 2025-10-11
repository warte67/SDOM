// SDOM_IFontObject.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IFontObject.hpp>

namespace SDOM
{
    IFontObject::IFontObject(const InitStruct& init) : IAssetObject(init)
    {
        // requested type default -> "bitmap"
        std::string requested = init.type.empty() ? std::string("bitmap") : init.type;
        std::transform(requested.begin(), requested.end(), requested.begin(), [](unsigned char c){ return std::tolower(c); });

        auto it = StringToFontType.find(requested);
        if (it != StringToFontType.end())
        {
            fontType_ = it->second; // Set enum based on string
        }
        else
        {
            INFO("IFontObject constructed with unknown font type: " + requested + " — defaulting to 'bitmap'");
            fontType_ = FontType::Bitmap;
        }
        type_ = FontTypeToString.at(fontType_);
        fontSize_ = (init.fontSize_ > 0) ? init.fontSize_ : 8; // sane default



        
    }


    IFontObject::IFontObject(const sol::table& config) : IAssetObject(config)
    {
        // name (required)
        if (!config["name"].valid()) {
            ERROR("IFontObject constructed without required field 'name' in config");
        }
        name_ = config["name"].get<std::string>();

        // filename (required)
        if (!config["filename"].valid()) {
            ERROR("IFontObject constructed without required field 'filename' in config");
        }
        filename_ = config["filename"].get<std::string>();

        // type (required) -> normalize and map to FontType
        std::string requested = config["type"].valid() ? config["type"].get<std::string>() : std::string();
        if (requested.empty()) {
            INFO("IFontObject constructed without required field 'type' in config");
            requested = "bitmap";
        }
        std::transform(requested.begin(), requested.end(), requested.begin(), [](unsigned char c){ return std::tolower(c); });

        auto it = StringToFontType.find(requested);
        if (it != StringToFontType.end()) {
            fontType_ = it->second;
        } else {
            INFO("IFontObject constructed with unknown font type: " + requested);
            fontType_ = FontType::Bitmap;
        }
        type_ = FontTypeToString.at(fontType_);

        // optional size
        fontSize_ = config["size"].valid() ? config["size"].get<int>() : 8;
        if (fontSize_ <= 0) fontSize_ = 8;
    }


    IFontObject::~IFontObject()
    {
        // Destructor implementation if needed
        // ...
    }   

    int IFontObject::getWordWidth(const std::string& word) const
    {
        // Default implementation (can be overridden)
        int ret = 0;
        for (char c : word)
        {
            ret += getGlyphWidth(static_cast<Uint32>(c));
        }
        return ret;
    }

    int IFontObject::getWordHeight(const std::string& word) const
    {
        // Default implementation (can be overridden)
        if (word.empty()) return 0; // Handle empty word case
        return std::max(0, getGlyphHeight(static_cast<Uint32>(word[0])));
    }


    // --- Lua Regisstration --- //

    void IFontObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IFontObject:" + getName();
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << typeNameLocal 
                    << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA 
                    << typeName << CLR::RESET << std::endl;
        }

        // Augment the single shared AssetHandle handle usertype (assets are exposed via AssetHandle handles in Lua)
        sol::table handle = AssetHandle::ensure_handle_table(lua);

        // Helper to check if a property/command is already registered
        auto absent = [&](const char* name) -> bool {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        // Helper to register a property/command if not already present
        auto reg = [&](const char* name, auto&& fn) {
            if (absent(name)) {
                handle.set_function(name, std::forward<decltype(fn)>(fn));
            }
        };

        // small helper to validate and cast the AssetHandle -> IFontObject*
        auto cast_ifont_from_asset = [](const AssetHandle& asset) -> IFontObject* {
            if (!asset.isValid()) { ERROR("invalid AssetHandle provided to IFontObject method"); }
            IAssetObject* base = asset.get();
            IFontObject* ss = dynamic_cast<IFontObject*>(base);
            if (!ss) { ERROR("invalid IFontObject object"); }
            return ss;
        };

        // Register IFontObject-specific properties and commands here (bridge from AssetHandle handle)
        reg("setFontSize", [cast_ifont_from_asset](AssetHandle asset, int size) { cast_ifont_from_asset(asset)->setFontSize(size); });
    } // END _registerLuaBindings()

} // namespace SDOM