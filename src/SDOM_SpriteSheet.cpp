// SDOM_SpriteSheet.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>

namespace SDOM
{

    SpriteSheet::SpriteSheet(const InitStruct& init) : IAssetObject(init)
    {
        if (init.type != TypeName) 
        {
            ERROR("Error: SpriteSheet constructed with incorrect type: " + init.type);
        }
        name_ = init.name;
        type_ = init.type;
        filename_ = init.filename;
        spriteWidth_ = init.spriteWidth;
        spriteHeight_ = init.spriteHeight;
    }

    SpriteSheet::SpriteSheet(const sol::table& config) : IAssetObject(config)
    {
        std::string type = config["type"].valid() ? config["type"].get<std::string>() : TypeName;
        if (type != TypeName) 
        {
            ERROR("Error: SpriteSheet constructed with incorrect type: " + type);
        }
        // Extract spriteWidth and spriteHeight from config, with defaults
        spriteWidth_  = config["spriteWidth"].valid()  ? config["spriteWidth"].get<int>()  : 8; // default 8 pixels wide
        spriteHeight_ = config["spriteHeight"].valid() ? config["spriteHeight"].get<int>() : 8; // default 8 pixels tall
    }



    bool SpriteSheet::onInit()
    {
        // std::cout << CLR::LT_ORANGE << "SpriteSheet::" << CLR::YELLOW << "onInit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Initialization logic for SpriteSheet
        return true;
    }

    void SpriteSheet::onQuit()
    {
        // std::cout << CLR::LT_ORANGE << "SpriteSheet::" << CLR::YELLOW << "onQuit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Cleanup logic for SpriteSheet
        onUnload();
    }

    void SpriteSheet::onLoad()
    {
        onUnload();

        // std::cout << CLR::LT_ORANGE << "SpriteSheet::" << CLR::YELLOW << "onLoad()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Loading logic for SpriteSheet


        SDL_Renderer* renderer = getRenderer();
        if (!renderer)
        {
            ERROR("No valid SDL_Renderer available in Core instance.");
            return;
        }

        // Load the default icon/default_icon_8x8 SpriteSheet from internal memory
        if (filename_ == "default_icon_8x8")
        {
            SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(default_icon_8x8)), default_icon_8x8_len);
            if (!rw)
            {
                ERROR("Failed to create SDL_IOStream from default_icon_8x8[]");
                return;
            }
            texture_ = IMG_LoadTexture_IO(renderer, rw, 1); // 1 = close/free rw after use
            if (!texture_)
            {
                ERROR("Failed to load texture from sprite_8x8[]: " + std::string(SDL_GetError()));
                return;
            }
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);
        }
        // Load the default Bitmap Font 8x8 SpriteSheet from internal memory
        else if (filename_ == "default_bmp_8x8")
        {
            SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(default_bmp_8x8)), default_bmp_8x8_len);
            if (!rw)
            {
                ERROR("Failed to create SDL_IOStream from default_bmp_8x8[]");
                return;
            }
            texture_ = IMG_LoadTexture_IO(renderer, rw, 1); // 1 = close/free rw after use
            if (!texture_)
            {
                ERROR("Failed to load texture from font_8x8[]: " + std::string(SDL_GetError()));
                return;
            }
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);   
        }
        // Load from external file
        else
        {
            texture_ = IMG_LoadTexture(renderer, filename_.c_str());
            if (!texture_)
            {
                ERROR("Failed to load texture from file: " + filename_ + " - " + std::string(SDL_GetError()));
                return;
            }
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);   
        }
        isLoaded_ = true;
    }

    void SpriteSheet::onUnload()
    {
        // std::cout << CLR::LT_ORANGE << "SpriteSheet::" << CLR::YELLOW << "onUnload()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Unloading logic for SpriteSheet

        // Free the texture and clear sprite metadata
        if (texture_)
        {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }
        isLoaded_ = false;
    }

    bool SpriteSheet::onUnitTest()
    {
        // std::cout << CLR::LT_ORANGE << "SpriteSheet::" << CLR::YELLOW << "onUnitTest()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Unit test logic for SpriteSheet
        return true;
    }

     // public methods
    void SpriteSheet::setSpriteWidth(int width)
    {
        // Set sprite width
        spriteWidth_ = width;
    }

    void SpriteSheet::setSpriteHeight(int height)
    {
        // Set sprite height
        spriteHeight_ = height;
    }

    void SpriteSheet::setSpriteSize(int width, int height)
    {
        // Set sprite size
        spriteWidth_ = width;
        spriteHeight_ = height;
    }

    int SpriteSheet::getSpriteWidth() const
    {
        // Get sprite width
        return spriteWidth_;
    }

    int SpriteSheet::getSpriteHeight() const
    {
        // Get sprite height
        return spriteHeight_;
    }

    std::pair<int, int> SpriteSheet::getSpriteSize() const
    {
        // Get sprite size
        return {spriteWidth_, spriteHeight_};
    }

    int SpriteSheet::getSpriteCount() const
    {
        if (!texture_)
            ERROR("SpriteSheet::texture_ not loaded");

        int spritesPerRow = 0;
        int spritesPerColumn = 0;
        float texW = 0.0f;
        float texH = 0.0f;
        if (!SDL_GetTextureSize(texture_, &texW, &texH))
            ERROR("Failed to get texture size: " + std::string(SDL_GetError()));

        if (spriteWidth_ > 0)
            spritesPerRow = static_cast<int>(texW / spriteWidth_);
        else
            ERROR("SpriteSheet: spriteWidth_ is zero, cannot compute spritesPerRow.");

        if (spriteHeight_ > 0)
            spritesPerColumn = static_cast<int>(texH / spriteHeight_);
        else
            ERROR("SpriteSheet: spriteHeight_ is zero, cannot compute spritesPerColumn.");

        return spritesPerRow * spritesPerColumn;
    }

    int SpriteSheet::getSpriteX(int spriteIndex) const
    {
        if (!texture_) ERROR("SpriteSheet::texture_ not loaded");

        float texW = 0.0f, texH = 0.0f;
        if (!SDL_GetTextureSize(texture_, &texW, &texH))
            ERROR("Failed to get texture size: " + std::string(SDL_GetError()));

        if (spriteWidth_ <= 0) ERROR("SpriteSheet: spriteWidth_ is zero or negative.");

        int spritesPerRow = static_cast<int>(texW / spriteWidth_);
        if (spritesPerRow <= 0) ERROR("SpriteSheet: spritesPerRow computed as zero.");

        int total = static_cast<int>(texW / spriteWidth_) * static_cast<int>(texH / spriteHeight_);
        if (spriteIndex < 0 || spriteIndex >= total)
            ERROR("SpriteSheet::getSpriteX: spriteIndex out of range.");

        return (spriteIndex % spritesPerRow) * spriteWidth_;
    }


    int SpriteSheet::getSpriteY(int spriteIndex) const
    {
        if (!texture_) ERROR("SpriteSheet::texture_ not loaded");

        float texW = 0.0f, texH = 0.0f;
        if (!SDL_GetTextureSize(texture_, &texW, &texH))
            ERROR("Failed to get texture size: " + std::string(SDL_GetError()));

        if (spriteWidth_ <= 0) ERROR("SpriteSheet: spriteWidth_ is zero or negative.");

        int spritesPerRow = static_cast<int>(texW / spriteWidth_);
        if (spritesPerRow <= 0) ERROR("SpriteSheet: spritesPerRow computed as zero.");

        int total = static_cast<int>(texW / spriteWidth_) * static_cast<int>(texH / spriteHeight_);
        if (spriteIndex < 0 || spriteIndex >= total)
            ERROR("SpriteSheet::getSpriteY: spriteIndex out of range.");

        return (spriteIndex / spritesPerRow) * spriteHeight_;
    }



    void SpriteSheet::drawSprite(int x, int y, int spriteIndex, SDL_Color color, SDL_ScaleMode scaleMode)
    {
        // THIS SHOULD BE LOADEDD AT THIS POINT!  (BUT IT'S NOT)
        if (!texture_) onLoad();
        if (!texture_) { ERROR("No texture loaded in SpriteSheet to draw sprite."); }

        float texW = 0.0f, texH = 0.0f;
        if (!SDL_GetTextureSize(texture_, &texW, &texH))
            ERROR("Failed to get texture size: " + std::string(SDL_GetError()));

        if (spriteWidth_ <= 0 || spriteHeight_ <= 0) ERROR("Invalid sprite dimensions.");

        int spritesPerRow = static_cast<int>(texW / spriteWidth_);
        if (spritesPerRow <= 0) 
            ERROR("Invalid sprite dimensions or texture size.");

        int total = static_cast<int>(texW / spriteWidth_) * static_cast<int>(texH / spriteHeight_);
        if (spriteIndex < 0 || spriteIndex >= total) 
            ERROR("SpriteSheet::drawSprite: spriteIndex out of range.");

        // Calculate the source rectangle (sprite position in the texture)
        SDL_FRect srcRect;
        srcRect.x = static_cast<float>((spriteIndex % spritesPerRow) * spriteWidth_);
        srcRect.y = static_cast<float>((spriteIndex / spritesPerRow) * spriteHeight_);
        srcRect.w = static_cast<float>(spriteWidth_);
        srcRect.h = static_cast<float>(spriteHeight_);

        // Calculate the destination rectangle (where to draw the sprite)
        SDL_FRect destRect;
        destRect.x = static_cast<float>(x);
        destRect.y = static_cast<float>(y);
        destRect.w = static_cast<float>(spriteWidth_);
        destRect.h = static_cast<float>(spriteHeight_);

        // Render the sprite
        SDL_Renderer* renderer = getRenderer();
        if (!renderer)
        {
            ERROR("No valid SDL_Renderer available in Core instance.");
            return;
        }
        SDL_SetTextureColorMod(texture_, color.r, color.g, color.b);
        SDL_SetTextureScaleMode(texture_, scaleMode); 
        SDL_RenderTexture(renderer, texture_, &srcRect, &destRect);
    }

    void SpriteSheet::drawSprite(SDL_FRect& destRect, int spriteIndex, SDL_Color color, SDL_ScaleMode scaleMode)
    {
        // THIS SHOULD BE LOADEDD AT THIS POINT!  (BUT IT'S NOT)
        if (!texture_) onLoad();
        if (!texture_) { ERROR("No texture loaded in SpriteSheet to draw sprite."); }

        float texW = 0.0f, texH = 0.0f;
        if (!SDL_GetTextureSize(texture_, &texW, &texH))
            ERROR("Failed to get texture size: " + std::string(SDL_GetError()));

        if (spriteWidth_ <= 0 || spriteHeight_ <= 0) ERROR("Invalid sprite dimensions.");

        int spritesPerRow = static_cast<int>(texW / spriteWidth_);
        if (spritesPerRow <= 0) 
            ERROR("Invalid sprite dimensions or texture size.");

        int total = static_cast<int>(texW / spriteWidth_) * static_cast<int>(texH / spriteHeight_);
        if (spriteIndex < 0 || spriteIndex >= total) 
            ERROR("SpriteSheet::drawSprite: spriteIndex out of range.");

        // Calculate the source rectangle (sprite position in the texture)
        SDL_FRect srcRect;
        srcRect.x = static_cast<float>((spriteIndex % spritesPerRow) * spriteWidth_);
        srcRect.y = static_cast<float>((spriteIndex / spritesPerRow) * spriteHeight_);
        srcRect.w = static_cast<float>(spriteWidth_);
        srcRect.h = static_cast<float>(spriteHeight_);

        // Render the sprite
        SDL_Renderer* renderer = getRenderer();
        if (!renderer)
        {
            ERROR("No valid SDL_Renderer available in Core instance.");
            return;
        }
        SDL_SetTextureColorMod(texture_, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(texture_, color.a);
        SDL_SetTextureScaleMode(texture_, scaleMode); 
        SDL_RenderTexture(renderer, texture_, &srcRect, &destRect);
    }


    void SpriteSheet::drawSprite(const SDL_FRect& srcRect, const SDL_FRect& dstRect,
        int spriteIndex, SDL_Color color, SDL_ScaleMode scaleMode)
    {
        // THIS SHOULD BE LOADEDD AT THIS POINT!  (BUT IT'S NOT)
        if (!texture_) onLoad();
        if (!texture_) { ERROR("No texture loaded in SpriteSheet to draw sprite."); }

        float texW = 0.0f, texH = 0.0f;
        if (!SDL_GetTextureSize(texture_, &texW, &texH))
            ERROR("Failed to get texture size: " + std::string(SDL_GetError()));

        if (spriteWidth_ <= 0 || spriteHeight_ <= 0) ERROR("Invalid sprite dimensions.");

        int spritesPerRow = static_cast<int>(texW / spriteWidth_);
        if (spritesPerRow <= 0) 
            ERROR("Invalid sprite dimensions or texture size.");

        int total = static_cast<int>(texW / spriteWidth_) * static_cast<int>(texH / spriteHeight_);
        if (spriteIndex < 0 || spriteIndex >= total) 
            ERROR("SpriteSheet::drawSprite: spriteIndex out of range.");

        // Calculate the source rectangle (sprite position in the texture)
        SDL_FRect sRect;
        sRect.x = static_cast<float>((spriteIndex % spritesPerRow) * spriteWidth_);
        sRect.y = static_cast<float>((spriteIndex / spritesPerRow) * spriteHeight_);
        sRect.w = static_cast<float>(spriteWidth_);
        sRect.h = static_cast<float>(spriteHeight_);

        sRect.x += srcRect.x;
        sRect.y += srcRect.y;

        if (spritesPerRow == 0)
        {
            ERROR("SpriteSheet::drawSprite: spritesPerRow is zero, cannot compute sprite position.");
            return;
        }

        // Clamp starting point to within the sprite tile
        sRect.x = std::clamp(sRect.x, static_cast<float>((spriteIndex % spritesPerRow) * spriteWidth_),
                                    static_cast<float>((spriteIndex % spritesPerRow + 1) * spriteWidth_ - 1));
        sRect.y = std::clamp(sRect.y, static_cast<float>((spriteIndex / spritesPerRow) * spriteHeight_),
                                    static_cast<float>((spriteIndex / spritesPerRow + 1) * spriteHeight_ - 1));

        // Clamp width and height so we don't go outside the tile
        float maxW = static_cast<float>(spriteWidth_) - (sRect.x - static_cast<float>((spriteIndex % spritesPerRow) * spriteWidth_));
        float maxH = static_cast<float>(spriteHeight_) - (sRect.y - static_cast<float>((spriteIndex / spritesPerRow) * spriteHeight_));
        sRect.w = std::clamp(srcRect.w, 0.0f, maxW);
        sRect.h = std::clamp(srcRect.h, 0.0f, maxH);

        // Render the sprite
        SDL_Renderer* renderer = getRenderer();
        if (!renderer)
        {
            ERROR("No valid SDL_Renderer available in Core instance.");
            return;
        }
        SDL_SetTextureColorMod(texture_, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(texture_, color.a);
        SDL_SetTextureScaleMode(texture_, scaleMode);
        SDL_RenderTexture(renderer, texture_, &sRect, &dstRect);
    }   


    // --- Lua wrapper helpers for SpriteSheet --- //
    void SpriteSheet::setSpriteWidth_Lua(IAssetObject* obj, int width)
    {
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("SpriteSheet::setSpriteWidth_Lua: invalid SpriteSheet object"); }
        ss->setSpriteWidth(width);
    }

    void SpriteSheet::setSpriteHeight_Lua(IAssetObject* obj, int height)
    {
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("SpriteSheet::setSpriteHeight_Lua: invalid SpriteSheet object"); }
        ss->setSpriteHeight(height);
    }

    void SpriteSheet::setSpriteSize_Lua(IAssetObject* obj, int width, int height)
    {
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("SpriteSheet::setSpriteSize_Lua: invalid SpriteSheet object"); }
        ss->setSpriteSize(width, height);
    }

    int SpriteSheet::getSpriteWidth_Lua(IAssetObject* obj)
    {
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("SpriteSheet::getSpriteWidth_Lua: invalid SpriteSheet object"); }
        return ss->getSpriteWidth();
    }

    int SpriteSheet::getSpriteHeight_Lua(IAssetObject* obj)
    {
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("SpriteSheet::getSpriteHeight_Lua: invalid SpriteSheet object"); }
        return ss->getSpriteHeight();
    }

    sol::table SpriteSheet::getSpriteSize_Lua(IAssetObject* obj, sol::state_view lua)
    {
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("SpriteSheet::getSpriteSize_Lua: invalid SpriteSheet object"); }
        sol::table t = lua.create_table();
        t[1] = ss->getSpriteWidth();
        t[2] = ss->getSpriteHeight();
        t["w"] = ss->getSpriteWidth();
        t["h"] = ss->getSpriteHeight();
        return t;
    }

    int SpriteSheet::getSpriteCount_Lua(IAssetObject* obj)
    {
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("SpriteSheet::getSpriteCount_Lua: invalid SpriteSheet object"); }
        return ss->getSpriteCount();
    }

    int SpriteSheet::getSpriteX_Lua(IAssetObject* obj, int spriteIndex)
    {
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("SpriteSheet::getSpriteX_Lua: invalid SpriteSheet object"); }
        return ss->getSpriteX(spriteIndex);
    }

    int SpriteSheet::getSpriteY_Lua(IAssetObject* obj, int spriteIndex)
    {
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("SpriteSheet::getSpriteY_Lua: invalid SpriteSheet object"); }
        return ss->getSpriteY(spriteIndex);
    }


    void SpriteSheet::drawSprite_lua( int x, int y, int spriteIndex, SDL_Color color, SDL_ScaleMode scaleMode /*= SDL_SCALEMODE_NEAREST*/ )
    {
        // delegate to the concrete implementation
        drawSprite(x, y, spriteIndex, color, scaleMode);
    } 

    void SpriteSheet::drawSprite_dst_lua( SDL_FRect& destRect, int spriteIndex, SDL_Color color, SDL_ScaleMode scaleMode /*= SDL_SCALEMODE_NEAREST*/ )
    {
        // delegate to the concrete implementation that renders to a destination rect
        drawSprite(destRect, spriteIndex, color, scaleMode);
    }

    void SpriteSheet::drawSprite_ext_Lua( IAssetObject* obj, sol::table srcRect, sol::table dstRect,
                                    int spriteIndex, sol::object color, sol::object scaleMode)
    {
        // resolve target instance (defensive)
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("drawSprite_ext_Lua: invalid SpriteSheet object"); }

        SDL_FRect s = SDL_Utils::tableToFRect(srcRect);
        SDL_FRect d = SDL_Utils::tableToFRect(dstRect);
        SDL_Color c = SDL_Utils::colorFromSol(color);
        SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(scaleMode);

        // call concrete draw implementation on the instance
        ss->drawSprite(s, d, spriteIndex, c, sm);
    }


    // --- Lua Regisstration --- //

    void SpriteSheet::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "SpriteSheet:" + getName();
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << typeNameLocal 
                    << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA 
                    << typeName << CLR::RESET << std::endl;
        }

        // Augment the single shared DisplayObject handle usertype
        sol::table handle = DisplayObject::ensure_handle_table(lua);

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

        // small helper to validate and cast the IAssetObject -> SpriteSheet*
        auto cast_ss = [](IAssetObject* obj) -> SpriteSheet* {
            SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
            if (!ss) { ERROR("invalid SpriteSheet object"); }
            return ss;
        };

        // Register SpriteSheet-specific properties and commands here
        reg("setSpriteWidth", [cast_ss](IAssetObject* obj, int w) { cast_ss(obj)->setSpriteWidth(w); });
        reg("setSpriteHeight", [cast_ss](IAssetObject* obj, int h) { cast_ss(obj)->setSpriteHeight(h); });
        reg("setSpriteSize", [cast_ss](IAssetObject* obj, int w, int h) { cast_ss(obj)->setSpriteSize(w, h); });
        reg("getSpriteWidth", [cast_ss](IAssetObject* obj) -> int { return cast_ss(obj)->getSpriteWidth(); });
        reg("getSpriteHeight", [cast_ss](IAssetObject* obj) -> int { return cast_ss(obj)->getSpriteHeight(); });
        reg("getSpriteCount", [cast_ss](IAssetObject* obj) -> int { return cast_ss(obj)->getSpriteCount(); });
        reg("getSpriteX", [cast_ss](IAssetObject* obj, int idx) -> int { return cast_ss(obj)->getSpriteX(idx); });
        reg("getSpriteY", [cast_ss](IAssetObject* obj, int idx) -> int { return cast_ss(obj)->getSpriteY(idx); });

        reg("getSpriteSize",
            [lua, cast_ss](IAssetObject* obj) mutable -> sol::table {
                SpriteSheet* ss = cast_ss(obj);
                sol::table t = lua.create_table();
                t[1] = ss->getSpriteWidth();
                t[2] = ss->getSpriteHeight();
                t["w"] = ss->getSpriteWidth();
                t["h"] = ss->getSpriteHeight();
                return t;
            }
        );

        // register named Lua wrappers:
        // drawSprite(obj, x, y, index, color?, scaleMode?)
        reg("drawSprite",
            [lua, cast_ss](IAssetObject* obj, int x, int y, int spriteIndex, sol::object color = sol::nil, sol::object scaleMode = sol::nil) {
                SpriteSheet* ss = cast_ss(obj);
                SDL_Color c = SDL_Utils::colorFromSol(color);
                SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(scaleMode);
                ss->drawSprite_lua(x, y, spriteIndex, c, sm);
            }
        );

        // drawSprite_dst(obj, dstTbl, index, color?, scaleMode?)
        reg("drawSprite_dst",
            [lua, cast_ss](IAssetObject* obj, sol::table dstTbl, int spriteIndex, sol::object color = sol::nil, sol::object scaleMode = sol::nil) {
                SpriteSheet* ss = cast_ss(obj);
                SDL_FRect d = SDL_Utils::tableToFRect(dstTbl);
                SDL_Color c = SDL_Utils::colorFromSol(color);
                SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(scaleMode);
                ss->drawSprite_dst_lua(d, spriteIndex, c, sm);
            }
        );

        // drawSprite_EX(obj, srcTbl, dstTbl, index, color?, scaleMode?)
        reg("drawSprite_ext",
            [lua, cast_ss](IAssetObject* obj, sol::table srcTbl, sol::table dstTbl, int spriteIndex, sol::object color = sol::nil, sol::object scaleMode = sol::nil) {
                SpriteSheet* ss = cast_ss(obj);
                ss->drawSprite_ext_Lua(obj, srcTbl, dstTbl, spriteIndex, color, scaleMode);
            }
        );

    } // END _registerLuaBindings()
} // namespace SDOM_