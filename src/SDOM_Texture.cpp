// SDOM_Texture.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>
#include <SDOM/SDOM_Texture.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Core.hpp>


namespace SDOM
{
    Texture::Texture(const InitStruct& init) : IAssetObject(init)
    {
        if (init.type != TypeName) 
        {
            ERROR("Error: Texture constructed with incorrect type: " + init.type);
        }
        name_ = init.name;
        type_ = init.type;
        filename_ = init.filename;
        // Initialize texture-specific parameters from init if any
    }

    Texture::Texture(const sol::table& config) : IAssetObject(config)
    {
        std::string type = config["type"].valid() ? config["type"].get<std::string>() : TypeName;
        if (type != TypeName) 
        {
            ERROR("Error: Texture constructed with incorrect type: " + type);
        }
        // Extract texture-specific parameters from config if any
    }

    bool Texture::onInit()
    {
        // std::cout << CLR::LT_ORANGE << "Texture::" << CLR::YELLOW << "onInit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Initialization logic for Texture
        return true;
    }

    void Texture::onQuit()
    {
        // std::cout << CLR::LT_ORANGE << "Texture::" << CLR::YELLOW << "onQuit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Cleanup logic for Texture
        onUnload();
    }

    void Texture::onLoad()
    {
        onUnload();

        SDL_Renderer* renderer = getRenderer();
        if (!renderer) { ERROR("No valid SDL_Renderer available in Core instance."); return; }

        // If Factory already has an object for this filename -> reuse it
        IAssetObject* existing = getFactory().getResObj(filename_);
        if (existing)
        {
            // Try to reuse an already-loaded Texture asset to avoid redundant loads / null texture
            Texture* other = dynamic_cast<Texture*>(existing);
            if (other && other->isLoaded())
            {
                // Reuse the SDL_Texture* and cached size (do NOT destroy other's texture_ in this instance;
                // prefer sharing ownership via Factory in a later refactor)
                texture_ = other->texture_;
                textureWidth_  = other->textureWidth_;
                textureHeight_ = other->textureHeight_;
                isLoaded_ = true;
                return;
            }
            // If an existing asset is present but not loaded yet, fall through and attempt to load here.
        }

        // special internal names
        if (filename_ == "internal_icon_8x8")
        {
            SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(internal_icon_8x8)), internal_icon_8x8_len);
            if (!rw) { ERROR("Failed to create SDL_IOStream from internal_icon_8x8[]"); return; }
            texture_ = IMG_LoadTexture_IO(renderer, rw, 1);
            if (!texture_) { ERROR("Failed to load texture from sprite_8x8[]: " + std::string(SDL_GetError())); return; }
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);
        }
        else if (filename_ == "internal_font_8x8")
        {
            SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(internal_font_8x8)), internal_font_8x8_len);
            if (!rw) { ERROR("Failed to create SDL_IOStream from internal_font_8x8[]"); return; }
            texture_ = IMG_LoadTexture_IO(renderer, rw, 1);
            if (!texture_) { ERROR("Failed to load texture from font_8x8[]: " + std::string(SDL_GetError())); return; }
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);
        }
        else if (filename_ == "internal_font_8x12")
        {
            SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(internal_font_8x12)), internal_font_8x12_len);
            if (!rw) { ERROR("Failed to create SDL_IOStream from internal_font_8x12[]"); return; }
            texture_ = IMG_LoadTexture_IO(renderer, rw, 1);
            if (!texture_) { ERROR("Failed to load texture from font_8x12[]: " + std::string(SDL_GetError())); return; }
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);
        }        
        else
        {
            texture_ = IMG_LoadTexture(renderer, filename_.c_str());
            if (!texture_) { ERROR("Failed to load texture from file: " + filename_ + " - " + std::string(SDL_GetError())); return; }
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);
        }

        if (!texture_) {
            ERROR("Texture::onLoad() - texture_ is null after loading for: " + getName());
            return;
        }

        if (!SDL_GetTextureSize(texture_, &textureWidth_, &textureHeight_))
            ERROR("Failed to query texture size for: " + getName() + " - " + std::string(SDL_GetError()));

        isLoaded_ = true;
    } // END Texture::onLoad()

    void Texture::onUnload()
    {
        // std::cout << CLR::LT_ORANGE << "Texture::" << CLR::YELLOW << "onUnload()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;

        // Free the texture and clear sprite metadata
        if (texture_)
        {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }
        isLoaded_ = false;    
    }

    bool Texture::onUnitTest()
    {
        // run base checks first
        if (!SUPER::onUnitTest()) return false;

        bool ok = true;

        // filename should be present (internal assets have names too)
        if (filename_.empty()) {
            DEBUG_LOG("[UnitTest] Texture '" << getName() << "' has empty filename");
            ok = false;
        }

        // If the texture is loaded, ensure SDL_Texture and dimensions look valid
        if (isLoaded_) {
            if (!texture_) {
                DEBUG_LOG("[UnitTest] Texture '" << getName() << "' is marked loaded but texture_ is null");
                ok = false;
            }
            if (textureWidth_ <= 0 || textureHeight_ <= 0) {
                DEBUG_LOG("[UnitTest] Texture '" << getName() << "' has invalid size: w="
                          << textureWidth_ << " h=" << textureHeight_);
                ok = false;
            }
        }

        return ok;
    } // END Texture::onUnitTest()


    // --- Lua Regisstration --- //

    void Texture::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Texture:" + getName();
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << typeNameLocal 
                    << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA 
                    << typeName << CLR::RESET << std::endl;
        }

        // Augment the single shared AssetHandle handle usertype (assets are exposed via AssetHandle in Lua)
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

        // small helper to validate and cast the AssetHandle -> Texture*
        auto cast_tex_from_asset = [](const AssetHandle& asset) -> Texture* {
            if (!asset.isValid()) { ERROR("invalid AssetHandle provided to Texture method"); }
            IAssetObject* base = asset.get();
            Texture* tex = dynamic_cast<Texture*>(base);
            if (!tex) { ERROR("invalid Texture object"); }
            return tex;
        };

        // Register Texture-specific properties and commands here (bridge from AssetHandle handle)
        reg("getTextureWidth", [cast_tex_from_asset](AssetHandle asset) -> float { return cast_tex_from_asset(asset)->getTextureWidth(); });
        reg("getTextureHeight", [cast_tex_from_asset](AssetHandle asset) -> float { return cast_tex_from_asset(asset)->getTextureHeight(); });

    }

} // END namespace SDOM