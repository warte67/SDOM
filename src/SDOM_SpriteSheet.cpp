// SDOM_SpriteSheet.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_Texture.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>

namespace SDOM
{

    namespace {
        template <typename F>
        inline void ss_set_if_absent(sol::table& t, const char* name, F&& fn)
        {
            sol::object cur = t.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                t.set_function(name, std::forward<F>(fn));
            }
        }

        // Helper to safely get SpriteSheet* from an AssetHandle without requiring lambda capture
        inline SpriteSheet* as_ss_handle(AssetHandle& self)
        {
            SpriteSheet* ss = self.as<SpriteSheet>();
            if (!ss) { ERROR("SpriteSheet binding called on non-SpriteSheet AssetHandle"); }
            return ss;
        }
    }

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
        // Extract spriteWidth and spriteHeight from config, with defaults.
        // Accept both camelCase (spriteWidth/spriteHeight) and snake_case
        // (sprite_width/sprite_height) to make the Lua config authoring
        // more ergonomic.
        try {
            if (config["spriteWidth"].valid()) {
                spriteWidth_ = config["spriteWidth"].get<int>();
            } else if (config["sprite_width"].valid()) {
                spriteWidth_ = config["sprite_width"].get<int>();
            } else {
                spriteWidth_ = 8;
            }
        } catch(...) { spriteWidth_ = 8; }

        try {
            if (config["spriteHeight"].valid()) {
                spriteHeight_ = config["spriteHeight"].get<int>();
            } else if (config["sprite_height"].valid()) {
                spriteHeight_ = config["sprite_height"].get<int>();
            } else {
                spriteHeight_ = 8;
            }
        } catch(...) { spriteHeight_ = 8; }
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
        // INFO("SpriteSheet::onLoad() called for: " + getName() + " filename=" + filename_ + " (sprite " + std::to_string(spriteWidth_) + "x" + std::to_string(spriteHeight_) + ")");
        onUnload();

        // does this filename already exist in the factory?
        IAssetObject* existing = getFactory().getResObj(filename_);
        if (existing)
        {
            // try {
            //     INFO(std::string("SpriteSheet::onLoad() - found existing resObj name='") + existing->getName() + " type='" + existing->getType() + "'");
            // } catch(...) {}
            // Only reuse an existing asset when it is actually a Texture.
            // If the existing asset is a Texture, reuse it directly
            if (existing->getType() == Texture::TypeName) 
            {
                // INFO("SpriteSheet::onLoad: found existing Texture resource for filename: " + filename_ + " (name=" + existing->getName() + ")");
                textureAsset = getFactory().getAssetObject(filename_);
                if (!textureAsset)
                {
                    ERROR("SpriteSheet::onLoad: Failed to retrieve existing Texture asset for filename: " + filename_);
                    return;
                }
                // Ensure the referenced Texture asset is loaded so getTexture() will be valid.
                try {
                    // INFO(std::string("SpriteSheet::onLoad: calling onLoad() on Texture asset: ") + textureAsset.getName());
                    textureAsset->onLoad();
                } catch(...) {
                    ERROR("SpriteSheet::onLoad: Failed to load existing Texture asset for filename: " + filename_);
                    return;
                }
                isLoaded_ = true;
                // INFO("SpriteSheet::onLoad: textureAsset loaded successfully for SpriteSheet: " + getName());
                return;
            }

            // If the existing asset is itself a SpriteSheet, prefer to reuse its texture
            if (existing->getType() == SpriteSheet::TypeName) {
                SpriteSheet* existingSS = dynamic_cast<SpriteSheet*>(existing);
                if (existingSS) {
                    // If the existing SpriteSheet already has a textureAsset, reuse it
                    if (existingSS->textureAsset.isValid()) {
                        textureAsset = existingSS->textureAsset;
                        if (!textureAsset) {
                            ERROR("SpriteSheet::onLoad: Failed to retrieve texture asset from existing SpriteSheet for filename: " + filename_);
                            return;
                        }
                        try {
                            textureAsset->onLoad();
                        } catch(...) {
                            ERROR("SpriteSheet::onLoad: Failed to load texture asset from existing SpriteSheet for filename: " + filename_);
                            return;
                        }
                        isLoaded_ = true;
                        return;
                    }
                    // Otherwise, use the existing SpriteSheet's filename to create a Texture asset
                    std::string srcFilename;
                    try { srcFilename = existingSS->getFilename(); } catch(...) { srcFilename = filename_; }
                    std::string baseName = srcFilename + "_Texture";
                    std::string newName = baseName;
                    int suffix = 1;
                    while (getFactory().getAssetObject(newName).isValid()) {
                        newName = baseName + "_" + std::to_string(suffix++);
                    }
                    Texture::InitStruct init;
                    init.name = newName;
                    init.type = Texture::TypeName;
                    init.filename = srcFilename;

                    textureAsset = getFactory().createAsset(Texture::TypeName, init);
                    if (!textureAsset)
                    {
                        ERROR("SpriteSheet::onLoad: Factory failed to create Texture asset for filename: " + srcFilename + " (tried name: " + newName + ")");
                        return;
                    }
                    Texture* texturePtr = textureAsset.as<Texture>();
                    if (texturePtr)
                    {
                        texturePtr->registerLuaBindings(Texture::TypeName, getLua());
                    }
                }
                else {
                    // fallback to existing code path below
                }
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
                // INFO("SpriteSheet::onLoad: created Texture asset '" + textureAsset.getName() + "' for filename: " + filename_);
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
        // run base checks first
        if (!SUPER::onUnitTest()) return false;

        bool ok = true;

        // sprite tile dimensions must be positive
        if (spriteWidth_ <= 0 || spriteHeight_ <= 0) {
            DEBUG_LOG("[UnitTest] SpriteSheet '" << getName() << "' has invalid sprite size: w="
                      << spriteWidth_ << " h=" << spriteHeight_);
            ok = false;
        }

        // texture asset should exist and pass its own unit tests
        if (!textureAsset.isValid()) {
            DEBUG_LOG("[UnitTest] SpriteSheet '" << getName() << "' has no associated Texture asset (filename='" << filename_ << "')");
            ok = false;
        } else {
            try {
                if (!textureAsset->onUnitTest()) {
                    DEBUG_LOG("[UnitTest] SpriteSheet '" << getName() << "' Texture asset onUnitTest() failed");
                    ok = false;
                }
            } catch (...) {
                DEBUG_LOG("[UnitTest] SpriteSheet '" << getName() << "' Texture asset onUnitTest() threw an exception");
                ok = false;
            }
        }

        return ok;
    } // END onUnitTest()

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

    bool SpriteSheet::operator==(const SpriteSheet& other) const
    {
        try {
            if (filename_ != other.filename_) return false;
            if (spriteWidth_ != other.spriteWidth_) return false;
            if (spriteHeight_ != other.spriteHeight_) return false;
            // If both have valid texture assets, compare underlying texture filenames
            if (textureAsset.isValid() && other.textureAsset.isValid()) {
                try {
                    if (textureAsset.getFilename() != other.textureAsset.getFilename()) return false;
                } catch(...) {}
            }
            return true;
        } catch(...) {
            return false;
        }
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

        // Augment the single shared AssetHandle handle usertype (assets are exposed via AssetHandle in Lua)
        sol::table handle = AssetHandle::ensure_handle_table(lua);
        sol::optional<sol::usertype<AssetHandle>> maybeUT;
        try { maybeUT = lua[AssetHandle::LuaHandleName]; } catch(...) {}

        auto ss_bind_both = [&](const char* name, auto&& fn) {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                handle.set_function(name, std::forward<decltype(fn)>(fn));
            }
            if (maybeUT) {
                try { (*maybeUT)[name] = fn; } catch(...) {}
            }
        };

        // --- Size accessors --- //
        ss_bind_both("setSpriteWidth", [](AssetHandle& self, int w) {
            if (auto* ss = as_ss_handle(self)) ss->setSpriteWidth(w);
        });
        ss_bind_both("setSpriteHeight", [](AssetHandle& self, int h) {
            if (auto* ss = as_ss_handle(self)) ss->setSpriteHeight(h);
        });
        ss_bind_both("setSpriteSize", [](AssetHandle& self, int w, int h) {
            if (auto* ss = as_ss_handle(self)) ss->setSpriteSize(w, h);
        });
        ss_bind_both("getSpriteWidth", [](AssetHandle& self) -> int {
            if (auto* ss = as_ss_handle(self)) return ss->getSpriteWidth();
            return 0;
        });
        ss_bind_both("getSpriteHeight", [](AssetHandle& self) -> int {
            if (auto* ss = as_ss_handle(self)) return ss->getSpriteHeight();
            return 0;
        });
        ss_bind_both("getSpriteSize", [lua](AssetHandle& self) -> sol::table {
            sol::state_view L = lua; // capture state for table creation
            sol::table t = L.create_table();
            if (auto* ss = as_ss_handle(self)) {
                t[1] = ss->getSpriteWidth();
                t[2] = ss->getSpriteHeight();
                t["w"] = ss->getSpriteWidth();
                t["h"] = ss->getSpriteHeight();
            }
            return t;
        });
        ss_bind_both("getSpriteCount", [](AssetHandle& self) -> int {
            if (auto* ss = as_ss_handle(self)) return ss->getSpriteCount();
            return 0;
        });
        ss_bind_both("getSpriteX", [](AssetHandle& self, int index) -> int {
            if (auto* ss = as_ss_handle(self)) return ss->getSpriteX(index);
            return 0;
        });
        ss_bind_both("getSpriteY", [](AssetHandle& self, int index) -> int {
            if (auto* ss = as_ss_handle(self)) return ss->getSpriteY(index);
            return 0;
        });

        // --- Drawing --- //
        // Overloaded drawSprite supporting: (idx, x, y, [color], [scale])
        //                                  (idx, dstRectTable, [color], [scale])
        //                                  (idx, srcRectTable, dstRectTable, [color], [scale])
        auto draw_xy = [](AssetHandle& self, int idx, int x, int y, sol::object colorOpt, sol::object scaleOpt) {
            if (auto* ss = as_ss_handle(self)) {
                SDL_Color c = SDL_Utils::colorFromSol(colorOpt);
                SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(scaleOpt);
                ss->drawSprite(idx, x, y, c, sm);
            }
        };
        auto draw_dst = [](AssetHandle& self, int idx, sol::table dst, sol::object colorOpt, sol::object scaleOpt) {
            if (auto* ss = as_ss_handle(self)) {
                SDL_FRect d = SDL_Utils::tableToFRect(dst);
                SDL_Color c = SDL_Utils::colorFromSol(colorOpt);
                SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(scaleOpt);
                ss->drawSprite(idx, d, c, sm);
            }
        };
        auto draw_ext = [](AssetHandle& self, int idx, sol::table src, sol::table dst, sol::object colorOpt, sol::object scaleOpt) {
            if (auto* ss = as_ss_handle(self)) {
                SDL_FRect s = SDL_Utils::tableToFRect(src);
                SDL_FRect d = SDL_Utils::tableToFRect(dst);
                SDL_Color c = SDL_Utils::colorFromSol(colorOpt);
                SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(scaleOpt);
                ss->drawSprite(idx, s, d, c, sm);
            }
        };

        ss_bind_both("drawSprite", sol::overload(
            // idx, x, y
            [draw_xy](AssetHandle& self, int idx, int x, int y) { draw_xy(self, idx, x, y, sol::lua_nil, sol::lua_nil); },
            // idx, x, y, color
            [draw_xy](AssetHandle& self, int idx, int x, int y, sol::object color) { draw_xy(self, idx, x, y, color, sol::lua_nil); },
            // idx, x, y, color, scale
            draw_xy,
            // idx, dst
            [draw_dst](AssetHandle& self, int idx, sol::table dst) { draw_dst(self, idx, dst, sol::lua_nil, sol::lua_nil); },
            // idx, dst, color
            [draw_dst](AssetHandle& self, int idx, sol::table dst, sol::object color) { draw_dst(self, idx, dst, color, sol::lua_nil); },
            // idx, dst, color, scale
            draw_dst,
            // idx, src, dst
            [draw_ext](AssetHandle& self, int idx, sol::table src, sol::table dst) { draw_ext(self, idx, src, dst, sol::lua_nil, sol::lua_nil); },
            // idx, src, dst, color
            [draw_ext](AssetHandle& self, int idx, sol::table src, sol::table dst, sol::object color) { draw_ext(self, idx, src, dst, color, sol::lua_nil); },
            // idx, src, dst, color, scale
            draw_ext
        ));

        // Explicitly-named variants for clarity
        ss_bind_both("drawSprite_dst", [](AssetHandle& self, int idx, sol::table dst, sol::object colorOpt, sol::object scaleOpt) {
            if (auto* ss = as_ss_handle(self)) {
                SDL_FRect d = SDL_Utils::tableToFRect(dst);
                SDL_Color c = SDL_Utils::colorFromSol(colorOpt);
                SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(scaleOpt);
                ss->drawSprite(idx, d, c, sm);
            }
        });

        ss_bind_both("drawSprite_ext", [](AssetHandle& self, int idx, sol::table src, sol::table dst, sol::object colorOpt, sol::object scaleOpt) {
            if (auto* ss = as_ss_handle(self)) {
                SDL_FRect s = SDL_Utils::tableToFRect(src);
                SDL_FRect d = SDL_Utils::tableToFRect(dst);
                SDL_Color c = SDL_Utils::colorFromSol(colorOpt);
                SDL_ScaleMode sm = SDL_Utils::scaleModeFromSol(scaleOpt);
                ss->drawSprite(idx, s, d, c, sm);
            }
        });

    } // END _registerLuaBindings()
} // namespace SDOM_
