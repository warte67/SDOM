// SDOM_TTFAsset.cpp
#include <SDOM/SDOM.hpp>


#include <SDOM/SDOM_TTFAsset.hpp>

namespace SDOM
{

    TTFAsset::TTFAsset(const InitStruct& init) : IAssetObject(init)
    {
        // --- Properties that will need to be initialized from init struct: --- //
        // struct InitStruct : public IAssetObject::InitStruct
        // {
        //     InitStruct() : IAssetObject::InitStruct() 
        //     { 
        //         name = TypeName;    // from IAssetObject
        //         type = TypeName;    // from IAssetObject
        //         filename = "";      // from IAssetObject
        //     }
        //     int internalFontSize_ = 10;     // Font size property for TrueType fonts
        // };        

        internalFontSize_ = init.internalFontSize;
        name_ = init.name;
        type_ = init.type;
        filename_ = init.type;

    } // END:  TTFAsset::TTFAsset(const InitStruct& init)

    TTFAsset::TTFAsset(const sol::table& config) : IAssetObject(config)
    {
        // Set defaults
        InitStruct init;        
        name_ = init.name;
        type_ = init.type;
        filename_ = init.filename;
        internalFontSize_ = init.internalFontSize;
        // Read from Lua table if present
        if (config.valid()) {
            if (config["name"].valid())        name_ = config["name"];
            if (config["type"].valid())        type_ = config["type"];
            if (config["filename"].valid())    filename_ = config["filename"];
            if (config["internalFontSize"].valid()) internalFontSize_ = config["internalFontSize"];
        }
    } // END: TTFAsset::TTFAsset(const sol::table& config)

    TTFAsset::~TTFAsset()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "~BitmapFont()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Destructor implementation if needed
        // ...
    } // END:  TTFAsset::~TTFAsset()

    bool TTFAsset::onInit() 
    {
        return true;
    } // END: TTFAsset::onInit()

    void TTFAsset::onQuit() 
    {

    } // END: TTFAsset::onQuit()

    void TTFAsset::onLoad() 
    {
        if (!ttf_font_) return;

        // Load internal default ttf font resource
        if (filename_ == "default_ttf")
        {
            SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(_default_ttf)), _default_ttf_len);
            if (!rw)
            {
                ERROR("Failed to create SDL_IOStream from sprite_8x8[]");
                return;
            }

            if (!ttf_font_)
            {
                ttf_font_ = TTF_OpenFontIO(rw, true, internalFontSize_);
                if (!ttf_font_)
                {
                    ERROR("Failed to open TTF font: " + std::string(SDL_GetError()));
                    return;
                }
            }
            isLoaded_ = true;   // Mark as created
            isInternal_ = true; // is an internal resource
        }
        else
        // Load TTF font from file
        {
            if (!ttf_font_)
            {
                ttf_font_ = TTF_OpenFont(filename_.c_str(), internalFontSize_);
                if (!ttf_font_)
                {
                    ERROR("Failed to open TTF font: " + std::string(SDL_GetError()));
                    return;
                }
            }
            isLoaded_ = true; // Mark as created
            isInternal_ = false; // not an internal resource
        }

    } // END: TTFAsset::onLoad()

    void TTFAsset::onUnload() 
    {
        if (ttf_font_)
        {
            TTF_CloseFont(ttf_font_);
            ttf_font_ = nullptr;
            isLoaded_ = false;
        }
    } // END: TTFAsset::onUnload()

    bool TTFAsset::onUnitTest() 
    {
        // std::cout << CLR::LT_ORANGE << "TTFAsset::" << CLR::YELLOW << "onUnitTest()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Unit test logic for TTFAsset

        return true;
    } // END: TTFAsset::onUnitTest()



    // --- Lua Registration --- //
    void TTFAsset::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "TTFAsset:" + getName();
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << typeNameLocal 
                    << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA 
                    << typeName << CLR::RESET << std::endl;
        }

        // // Augment the single shared AssetHandle handle usertype (assets are exposed via AssetHandle handles in Lua)
        // sol::table handle = AssetHandle::ensure_handle_table(lua);

        // // Helper to check if a property/command is already registered
        // auto absent = [&](const char* name) -> bool {
        //     sol::object cur = handle.raw_get_or(name, sol::lua_nil);
        //     return !cur.valid() || cur == sol::lua_nil;
        // };

        // // Helper to register a property/command if not already present
        // auto reg = [&](const char* name, auto&& fn) {
        //     if (absent(name)) {
        //         handle.set_function(name, std::forward<decltype(fn)>(fn));
        //     }
        // };

        // // small helper to validate and cast the AssetHandle -> TTFAsset*
        // auto cast_ss_from_asset = [](const AssetHandle& asset) -> TTFAsset* {
        //     if (!asset.isValid()) { ERROR("invalid AssetHandle provided to TTFAsset method"); }
        //     IAssetObject* base = asset.get();
        //     TTFAsset* ttf = dynamic_cast<TTFAsset*>(base);
        //     if (!ttf) { ERROR("invalid TTFAsset object"); }
        //     return ttf;
        // };

        // // Register TTFAsset-specific properties and commands here (bridge from AssetHandle handle)
        // reg("setBitmapFontWidth", [cast_ss_from_asset](AssetHandle asset, int w) { cast_ss_from_asset(asset)->setBitmapFontWidth(w); });

        
    } // END: TTFAsset::_registerLuaBindings()


} // END namespace SDOM


