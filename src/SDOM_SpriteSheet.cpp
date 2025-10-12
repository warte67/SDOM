// SDOM_SpriteSheet.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_Texture.hpp>

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

        // if (!textureAsset) 
        //     ERROR("SpriteSheet::onInit: textureAsset is null for: " + filename_);
        // return textureAsset->onInit();
        return true;
    }

    void SpriteSheet::onQuit()
    {
        // std::cout << CLR::LT_ORANGE << "SpriteSheet::" << CLR::YELLOW << "onQuit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Cleanup logic for SpriteSheet
        if (textureAsset) textureAsset->onUnload();
        onUnload();
    }

    void SpriteSheet::onLoad()
    {
        //INFO("SpriteSheet::onLoad() called for: " + getName());
        onUnload();

        // does this filename already exist in the factory?
        IAssetObject* existing = getFactory().getResObj(filename_);
        if (existing)
        {
            // Only reuse an existing asset when it is actually a Texture.
            if (existing->getType() == Texture::TypeName) {
                textureAsset = getFactory().getAssetObject(filename_);
                if (!textureAsset)
                {
                    ERROR("SpriteSheet::onLoad: Failed to retrieve existing Texture asset for filename: " + filename_);
                    return;
                }
                // Ensure the referenced Texture asset is loaded so getTexture() will be valid.
                try {
                    textureAsset->onLoad();
                } catch(...) {
                    ERROR("SpriteSheet::onLoad: Failed to load existing Texture asset for filename: " + filename_);
                    return;
                }
                isLoaded_ = true;
                return;
            }

            // If there is an existing non-Texture asset with the same name, avoid reusing it.
            // This can happen when different asset types share the same identifier (e.g. a BitmapFont
            // named the same as a texture filename). Generate a unique texture asset name instead.
            std::string baseName = filename_ + "_Texture";
            std::string newName = baseName;
            int suffix = 1;
            while (getFactory().getAssetObject(newName).isValid()) {
                newName = baseName + "_" + std::to_string(suffix++);
            }

            Texture::InitStruct init;
            init.name = newName;
            init.type = Texture::TypeName;
            init.filename = filename_;

            textureAsset = getFactory().createAsset(Texture::TypeName, init);
            if (!textureAsset)
            {
                ERROR("SpriteSheet::onLoad: Factory failed to create Texture asset for filename: " + filename_ + " (tried name: " + newName + ")");
                return;
            }
            Texture* texturePtr = textureAsset.as<Texture>();
            if (texturePtr)
            {
                texturePtr->registerLuaBindings(Texture::TypeName, getLua());
            }
            else
            {
                ERROR("SpriteSheet::onLoad: Created asset is not a Texture for filename: " + filename_);
            }
        }
        else
        {
            // No existing asset with this filename; create a new one using filename as the asset name.
            Texture::InitStruct init;
            init.name = filename_;
            init.type = Texture::TypeName;
            init.filename = filename_;

            textureAsset = getFactory().createAsset(Texture::TypeName, init);
            if (!textureAsset)
            {
                ERROR("SpriteSheet::onLoad: Factory failed to create Texture asset for filename: " + filename_);
                return;
            }
            // Defensive: ensure we actually got a Texture, and it is not ourselves
            Texture* texturePtr = textureAsset.as<Texture>();
            if (texturePtr)
            {
                texturePtr->registerLuaBindings(Texture::TypeName, getLua());
            }
            else
            {
                ERROR("SpriteSheet::onLoad: Created asset is not a Texture for filename: " + filename_);
            }
        }

        if (textureAsset) textureAsset->onLoad();
        isLoaded_ = true;
        return;
    } // END onLoad()

    void SpriteSheet::onUnload()
    {
        // std::cout << CLR::LT_ORANGE << "SpriteSheet::" << CLR::YELLOW << "onUnload()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Unloading logic for SpriteSheet

        if (textureAsset) textureAsset->onUnload();
        isLoaded_ = false;
    }

    bool SpriteSheet::onUnitTest()
    {
        // std::cout << CLR::LT_ORANGE << "SpriteSheet::" << CLR::YELLOW << "onUnitTest()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Unit test logic for SpriteSheet

        return textureAsset->onUnitTest();
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
        // If texture isn't loaded, attempt a guarded lazy-load so callers
        // that query sprite metadata (tests, etc.) can succeed without
        // forcing callers to explicitly load the asset first.
        static thread_local bool s_in_lazy_load = false;
        SDL_Texture* texture_ = getTexture();
        if (!texture_) 
        {
            if (!s_in_lazy_load) 
            {
                s_in_lazy_load = true;
                const_cast<SpriteSheet*>(this)->onLoad();
                s_in_lazy_load = false;
            }
            texture_ = getTexture();
        }
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
        static thread_local bool s_in_lazy_load = false;
        SDL_Texture* texture_ = getTexture();
        if (!texture_) {
            if (!s_in_lazy_load) {
                s_in_lazy_load = true;
                const_cast<SpriteSheet*>(this)->onLoad();
                s_in_lazy_load = false;
            }
            texture_ = getTexture();
        }
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
        static thread_local bool s_in_lazy_load = false;
        SDL_Texture* texture_ = getTexture();
        if (!texture_) {
            if (!s_in_lazy_load) {
                s_in_lazy_load = true;
                const_cast<SpriteSheet*>(this)->onLoad();
                s_in_lazy_load = false;
            }
            texture_ = getTexture();
        }
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



    void SpriteSheet::drawSprite(int spriteIndex, int x, int y, SDL_Color color, SDL_ScaleMode scaleMode)
    {
        SDL_Texture* texture_ = getTexture();
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

        // If using linear filtering, inset the source rect slightly to avoid sampling neighbor texels.
        if (scaleMode == SDL_SCALEMODE_LINEAR) {
            const float inset = 0.5f; // half a texel
            // ensure we don't invert the rect
            if (srcRect.w > 2.0f * inset && srcRect.h > 2.0f * inset) {
                srcRect.x += inset;
                srcRect.y += inset;
                srcRect.w -= 2.0f * inset;
                srcRect.h -= 2.0f * inset;
            }
        }

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

    void SpriteSheet::drawSprite(int spriteIndex, SDL_FRect& destRect, SDL_Color color, SDL_ScaleMode scaleMode)
    {
        SDL_Texture* texture_ = getTexture();
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
        
        // If using linear filtering, inset the source rect slightly to avoid sampling neighbor texels.
        if (scaleMode == SDL_SCALEMODE_LINEAR) {
            const float inset = 0.5f; // half a texel
            // ensure we don't invert the rect
            if (srcRect.w > 2.0f * inset && srcRect.h > 2.0f * inset) {
                srcRect.x += inset;
                srcRect.y += inset;
                srcRect.w -= 2.0f * inset;
                srcRect.h -= 2.0f * inset;
            }
        }

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


    void SpriteSheet::drawSprite(int spriteIndex, const SDL_FRect& srcRect, const SDL_FRect& dstRect,
        SDL_Color color, SDL_ScaleMode scaleMode)
    {
        SDL_Texture* texture_ = getTexture();
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

        // If using linear filtering, inset the source rect slightly to avoid sampling neighbor texels.
        if (scaleMode == SDL_SCALEMODE_LINEAR) {
            const float inset = 0.5f; // half a texel
            // ensure we don't invert the rect
            if (sRect.w > 2.0f * inset && sRect.h > 2.0f * inset) {
                sRect.x += inset;
                sRect.y += inset;
                sRect.w -= 2.0f * inset;
                sRect.h -= 2.0f * inset;
            }
        }

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


    void SpriteSheet::drawSprite_lua( int spriteIndex, int x, int y, SDL_Color color, SDL_ScaleMode scaleMode /*= SDL_SCALEMODE_NEAREST*/ )
    {
        // delegate to the concrete implementation
            drawSprite(spriteIndex, x, y, color, scaleMode);
    } 

    void SpriteSheet::drawSprite_dst_lua( int spriteIndex, SDL_FRect& destRect, SDL_Color color, SDL_ScaleMode scaleMode /*= SDL_SCALEMODE_NEAREST*/ )
    {
        // delegate to the concrete implementation that renders to a destination rect
            drawSprite(spriteIndex, destRect, color, scaleMode);
    }

    void SpriteSheet::drawSprite_ext_Lua( IAssetObject* obj, int spriteIndex, sol::table srcRect, sol::table dstRect,
                                    sol::object color, sol::object scaleMode)
    {
        // resolve target instance (defensive)
        SpriteSheet* ss = dynamic_cast<SpriteSheet*>(obj);
        if (!ss) { ERROR("drawSprite_ext_Lua: invalid SpriteSheet object"); }

        SDL_FRect s = SDL_Utils::tableToFRect(srcRect);
        SDL_FRect d = SDL_Utils::tableToFRect(dstRect);
        SDL_Color c = SDL_Utils::colorFromSol(color);
        SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(scaleMode);

        // call concrete draw implementation on the instance
        ss->drawSprite(spriteIndex, s, d, c, sm);
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

        // small helper to validate and cast the AssetHandle -> SpriteSheet*
        auto cast_ss_from_asset = [](const AssetHandle& asset) -> SpriteSheet* {
            if (!asset.isValid()) { ERROR("invalid AssetHandle provided to SpriteSheet method"); }
            IAssetObject* base = asset.get();
            SpriteSheet* ss = dynamic_cast<SpriteSheet*>(base);
            if (!ss) { ERROR("invalid SpriteSheet object"); }
            return ss;
        };

        // Register SpriteSheet-specific properties and commands here (bridge from AssetHandle handle)
        reg("setSpriteWidth", [cast_ss_from_asset](AssetHandle asset, int w) { cast_ss_from_asset(asset)->setSpriteWidth(w); });
        reg("setSpriteHeight", [cast_ss_from_asset](AssetHandle asset, int h) { cast_ss_from_asset(asset)->setSpriteHeight(h); });
        reg("setSpriteSize", [cast_ss_from_asset](AssetHandle asset, int w, int h) { cast_ss_from_asset(asset)->setSpriteSize(w, h); });
        reg("getSpriteWidth", [cast_ss_from_asset](AssetHandle asset) -> int { return cast_ss_from_asset(asset)->getSpriteWidth(); });
        reg("getSpriteHeight", [cast_ss_from_asset](AssetHandle asset) -> int { return cast_ss_from_asset(asset)->getSpriteHeight(); });
        reg("getSpriteCount", [cast_ss_from_asset](AssetHandle asset) -> int { return cast_ss_from_asset(asset)->getSpriteCount(); });
        reg("getSpriteX", [cast_ss_from_asset](AssetHandle asset, int idx) -> int { return cast_ss_from_asset(asset)->getSpriteX(idx); });
        reg("getSpriteY", [cast_ss_from_asset](AssetHandle asset, int idx) -> int { return cast_ss_from_asset(asset)->getSpriteY(idx); });

        reg("getSpriteSize",
            [lua, cast_ss_from_asset](AssetHandle asset) mutable -> sol::table {
                SpriteSheet* ss = cast_ss_from_asset(asset);
                sol::table t = lua.create_table();
                t[1] = ss->getSpriteWidth();
                t[2] = ss->getSpriteHeight();
                t["w"] = ss->getSpriteWidth();
                t["h"] = ss->getSpriteHeight();
                return t;
            }
        );

        // single Lua entrypoint: drawSprite(...)
        reg("drawSprite",
            [lua, cast_ss_from_asset](AssetHandle asset, sol::variadic_args va) {
                SpriteSheet* ss = cast_ss_from_asset(asset);
                if (!ss) return;

                std::vector<sol::object> args;
                for (auto v : va) args.push_back(v);
                if (args.empty()) { ERROR("drawSprite: missing arguments"); return; }

                auto is_table = [](const sol::object& o) -> bool {
                    return o.valid() && o.get_type() == sol::type::table;
                };

                auto is_color_table = [](const sol::object& o) -> bool {
                    if (!o.valid() || o.get_type() != sol::type::table) return false;
                    sol::table t = o.as<sol::table>();
                    bool numeric = t[1].valid() && t[2].valid() && t[3].valid();
                    bool named = (t["r"].valid() && t["g"].valid() && t["b"].valid());
                    return numeric || named;
                };

                // auto is_rect_table = [](const sol::object& o) -> bool {
                //     if (!o.valid() || o.get_type() != sol::type::table) return false;
                //     sol::table t = o.as<sol::table>();
                //     bool numeric = t[1].valid() && t[2].valid() && t[3].valid() && t[4].valid();
                //     bool named = (t["x"].valid() && t["y"].valid() && (t["w"].valid() || t["width"].valid()) && (t["h"].valid() || t["height"].valid()));
                //     // also accept (w,h,x,y) ordering by checking presence of all four named keys in any order
                //     bool named_alt = (t["w"].valid() && t["h"].valid() && t["x"].valid() && t["y"].valid());
                //     return numeric || named || named_alt;
                // };

                auto is_rect_table = [](const sol::object& o) -> bool {
                    if (!o.valid() || o.get_type() != sol::type::table) return false;
                    sol::table t = o.as<sol::table>();

                    // numeric form: {x, y, w, h} or any 4-element numeric sequence
                    bool numeric = t[1].valid() && t[2].valid() && t[3].valid() && t[4].valid();

                    // named form: x,y,w,h or x,y,width,height
                    bool named_xywh = t["x"].valid() && t["y"].valid() &&
                                      (t["w"].valid() || t["width"].valid()) &&
                                      (t["h"].valid() || t["height"].valid());

                    // alternate named order: w,h,x,y
                    bool named_whxy = t["w"].valid() && t["h"].valid() && t["x"].valid() && t["y"].valid();

                    // left/top/right/bottom form, accept aliases "l","t","r","b"
                    auto has_key = [&](const char* a, const char* b) -> bool {
                        return t[a].valid() || t[b].valid();
                    };
                    bool ltrb = has_key("left","l") && has_key("top","t") && has_key("right","r") && has_key("bottom","b");

                    return numeric || named_xywh || named_whxy || ltrb;
                };

                // first arg must be index:number
                sol::object a = args[0];
                if (a.get_type() != sol::type::number) {
                    ERROR("drawSprite: first argument must be spriteIndex (number)");
                    return;
                }
                int spriteIndex = a.as<int>();

                sol::object arg1 = (args.size() >= 2) ? args[1] : sol::nil;
                sol::object arg2 = (args.size() >= 3) ? args[2] : sol::nil;
                sol::object arg3 = (args.size() >= 4) ? args[3] : sol::nil;
                sol::object arg4 = (args.size() >= 5) ? args[4] : sol::nil;

                // CASE A: (index, x, y, [color], [scale])
                if (arg1.get_type() == sol::type::number && arg2.get_type() == sol::type::number) {
                    int x = arg1.as<int>();
                    int y = arg2.as<int>();
                    SDL_Color col = SDL_Utils::colorFromSol(arg3);
                    SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(arg4);
                    ss->drawSprite_lua(spriteIndex, x, y, col, sm);
                    return;
                }

                // CASE C: (index, srcTbl, dstTbl, [color], [scale])
                if (is_rect_table(arg1) && is_rect_table(arg2)) {
                    sol::table srcTbl = arg1.as<sol::table>();
                    sol::table dstTbl = arg2.as<sol::table>();
                    // SDL_Color c = SDL_Utils::colorFromSol(arg3);
                    // SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(arg4);
                    ss->drawSprite_ext_Lua(ss, spriteIndex, srcTbl, dstTbl, arg3, arg4);
                    return;
                }

                // CASE B: (index, dstTbl, [color], [scale])  -> drawSprite_dst
                if (is_rect_table(arg1)) {
                    sol::table dstTbl = arg1.as<sol::table>();
                    // If second provided and is a color, treat it as color; otherwise treat as scale (or nil)
                    if (is_color_table(arg2) || arg2 == sol::nil) {
                        SDL_Color c = SDL_Utils::colorFromSol(arg2);
                        SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(arg3);
                        SDL_FRect d = SDL_Utils::tableToFRect(dstTbl);
                        ss->drawSprite_dst_lua(spriteIndex, d, c, sm);
                        return;
                    } else if (is_table(arg2) && !is_color_table(arg2) && arg3 != sol::nil) {
                        // ambiguous: two tables but second isn't color -> prefer src+dst handled earlier; fallthrough
                    } else {
                        SDL_Color c = SDL_Utils::colorFromSol(arg2);
                        SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(arg3);
                        SDL_FRect d = SDL_Utils::tableToFRect(dstTbl);
                        ss->drawSprite_dst_lua(spriteIndex, d, c, sm);
                        return;
                    }
                }

                // nothing matched: helpful debug output
                {
                    std::string types = "";
                    auto tname = [](const sol::object& o)->std::string {
                        if (!o.valid()) return "nil";
                        switch (o.get_type()) {
                            case sol::type::nil: return "nil";
                            case sol::type::number: return "number";
                            case sol::type::string: return "string";
                            case sol::type::table: return "table";
                            case sol::type::boolean: return "boolean";
                            default: return "other";
                        }
                    };
                    for (size_t i=0;i<args.size();++i) {
                        if (i) types += ", ";
                        types += tname(args[i]);
                    }
                    ERROR("drawSprite: invalid argument pattern; types = " + types);
                }
            }
        );

        // drawSprite_dst( index, dstTbl, color?, scaleMode?)
        reg("drawSprite_dst",
            [lua, cast_ss_from_asset](AssetHandle asset, sol::object a, sol::object b, sol::object color = sol::nil, sol::object scaleMode = sol::nil) {
                SpriteSheet* ss = cast_ss_from_asset(asset);

                // validate spriteIndex
                if (!a.valid() || a.get_type() != sol::type::number) {
                    ERROR("drawSprite_dst: first argument must be spriteIndex (number)");
                    return;
                }
                int spriteIndex = a.as<int>();

                // validate destination rect (must be a table)
                if (!b.valid() || b.get_type() != sol::type::table) {
                    ERROR("drawSprite_dst: second argument must be a destination table");
                    return;
                }
                sol::table dstTbl = b.as<sol::table>();

                SDL_FRect d = SDL_Utils::tableToFRect(dstTbl);
                SDL_Color c = SDL_Utils::colorFromSol(color);
                SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(scaleMode);
                ss->drawSprite_dst_lua(spriteIndex, d, c, sm);
            }
        );

        // drawSprite_EX(asset, index, srcTbl, dstTbl, color?, scaleMode?)
        reg("drawSprite_ext",
            [lua, cast_ss_from_asset](AssetHandle asset, sol::object a, sol::object b, sol::object c, sol::object color = sol::nil, sol::object scaleMode = sol::nil) {
                SpriteSheet* ss = cast_ss_from_asset(asset);

                // validate spriteIndex
                if (!a.valid() || a.get_type() != sol::type::number) {
                    ERROR("drawSprite_ext: first argument must be spriteIndex (number)");
                    return;
                }
                int spriteIndex = a.as<int>();

                // validate src rect (must be a table)
                if (!b.valid() || b.get_type() != sol::type::table) {
                    ERROR("drawSprite_ext: second argument must be a source rect table");
                    return;
                }
                sol::table srcTbl = b.as<sol::table>();

                // validate dst rect (must be a table)
                if (!c.valid() || c.get_type() != sol::type::table) {
                    ERROR("drawSprite_ext: third argument must be a destination rect table");
                    return;
                }
                sol::table dstTbl = c.as<sol::table>();

                ss->drawSprite_ext_Lua(ss, spriteIndex, srcTbl, dstTbl, color, scaleMode);
            }
        );

    } // END _registerLuaBindings()
} // namespace SDOM_