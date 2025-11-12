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
        if (textureAsset) textureAsset->unload();
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
                    // INFO(std::string("SpriteSheet::onLoad: calling load() on Texture asset: ") + textureAsset.getName());
                    textureAsset->load();
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
                            textureAsset->load();
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

        if (textureAsset) textureAsset->load();
        isLoaded_ = true;
        return;
    } // END onLoad()

    void SpriteSheet::onUnload()
    {
        // std::cout << CLR::LT_ORANGE << "SpriteSheet::" << CLR::YELLOW << "onUnload()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Unloading logic for SpriteSheet

        if (textureAsset) textureAsset->unload();
        isLoaded_ = false;
    }

    
    bool SpriteSheet::onUnitTest(int frame)
    {
        // Run base checks first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Only register once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Validate sprite dimensions
            ut.add_test(objName, "SpriteSheet Dimensions", [this](std::vector<std::string>& errors)
            {
                if (spriteWidth_ <= 0 || spriteHeight_ <= 0)
                {
                    errors.push_back("SpriteSheet '" + getName() +
                                    "' has invalid sprite size: w=" +
                                    std::to_string(spriteWidth_) +
                                    " h=" + std::to_string(spriteHeight_));
                }
                return true; // ✅ single-frame test
            });

            // 🔹 2. Validate texture asset existence and test it recursively
            ut.add_test(objName, "SpriteSheet Texture Asset Validation", [this, frame](std::vector<std::string>& errors)
            {
                if (!textureAsset.isValid())
                {
                    errors.push_back("SpriteSheet '" + getName() +
                                    "' has no associated Texture asset (filename='" + filename_ + "')");
                    return true;
                }

                try
                {
                    if (!textureAsset->onUnitTest(frame))
                    {
                        errors.push_back("SpriteSheet '" + getName() +
                                        "' Texture asset onUnitTest() failed");
                    }
                }
                catch (...)
                {
                    errors.push_back("SpriteSheet '" + getName() +
                                    "' Texture asset onUnitTest() threw an exception");
                }

                return true;
            });

            registered = true;
        }

        // ✅ Return false so the object remains active while tests are processed
        return false;
    } // END: SpriteSheet::onUnitTest()


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



    void SpriteSheet::drawSprite(int spriteIndex, int x, int y, SDL_Color color, SDL_ScaleMode scaleMode, SDL_Texture* targetTexture)
    {
        SDL_Texture* texture_ = getTexture();
        if (!texture_) onLoad();
        if (!texture_) { ERROR("No texture loaded in SpriteSheet to draw sprite."); }

        SDL_Renderer* renderer = getRenderer();
        SDL_Texture* prevTarget = nullptr;
        bool switched = false;
        if (targetTexture) {
            prevTarget = SDL_GetRenderTarget(renderer);
            if (!SDL_SetRenderTarget(renderer, targetTexture)) {
                ERROR("SpriteSheet::drawSprite(x,y): Unable to set render target: " + std::string(SDL_GetError()));
                return;
            }
            switched = true;
        }

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
        if (!renderer)
        {
            ERROR("No valid SDL_Renderer available in Core instance.");
            if (switched) SDL_SetRenderTarget(renderer, prevTarget);
            return;
        }
        SDL_SetTextureColorMod(texture_, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(texture_, color.a);
        SDL_SetTextureScaleMode(texture_, scaleMode); 
        SDL_RenderTexture(renderer, texture_, &srcRect, &destRect);

        if (switched) SDL_SetRenderTarget(renderer, prevTarget);
    }

    void SpriteSheet::drawSprite(int spriteIndex, SDL_FRect& destRect, SDL_Color color, SDL_ScaleMode scaleMode, SDL_Texture* targetTexture)
    {
        SDL_Texture* texture_ = getTexture();
        if (!texture_) onLoad();
        if (!texture_) { ERROR("No texture loaded in SpriteSheet to draw sprite."); }

        SDL_Renderer* renderer = getRenderer();
        SDL_Texture* prevTarget = nullptr;
        bool switched = false;
        if (targetTexture) {
            prevTarget = SDL_GetRenderTarget(renderer);
            if (!SDL_SetRenderTarget(renderer, targetTexture)) {
                ERROR("SpriteSheet::drawSprite(dst): Unable to set render target: " + std::string(SDL_GetError()));
                return;
            }
            switched = true;
        }

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
        if (!renderer)
        {
            ERROR("No valid SDL_Renderer available in Core instance.");
            if (switched) SDL_SetRenderTarget(renderer, prevTarget);
            return;
        }
        SDL_SetTextureColorMod(texture_, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(texture_, color.a);
        SDL_SetTextureScaleMode(texture_, scaleMode); 
        SDL_RenderTexture(renderer, texture_, &srcRect, &destRect);

        if (switched) SDL_SetRenderTarget(renderer, prevTarget);
    }


    void SpriteSheet::drawSprite(int spriteIndex, const SDL_FRect& srcRect, const SDL_FRect& dstRect,
        SDL_Color color, SDL_ScaleMode scaleMode, SDL_Texture* targetTexture)
    {
        SDL_Texture* texture_ = getTexture();
        if (!texture_) onLoad();
        if (!texture_) { ERROR("No texture loaded in SpriteSheet to draw sprite."); }

        SDL_Renderer* renderer = getRenderer();
        SDL_Texture* prevTarget = nullptr;
        bool switched = false;
        if (targetTexture) {
            prevTarget = SDL_GetRenderTarget(renderer);
            if (!SDL_SetRenderTarget(renderer, targetTexture)) {
                ERROR("SpriteSheet::drawSprite(src,dst): Unable to set render target: " + std::string(SDL_GetError()));
                return;
            }
            switched = true;
        }

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
        if (!renderer)
        {
            ERROR("No valid SDL_Renderer available in Core instance.");
            if (switched) SDL_SetRenderTarget(renderer, prevTarget);
            return;
        }
        SDL_SetTextureColorMod(texture_, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(texture_, color.a);
        SDL_SetTextureScaleMode(texture_, scaleMode);
        SDL_RenderTexture(renderer, texture_, &sRect, &dstRect);

        if (switched) SDL_SetRenderTarget(renderer, prevTarget);
    }   


    void SpriteSheet::drawNineQuad(int baseIndex, SDL_Texture* targetTexture, SDL_Color color, SDL_ScaleMode scaleMode)
    {
        if (!targetTexture) { ERROR("SpriteSheet::drawNineQuad: targetTexture is null"); return; }

        SDL_Renderer* renderer = getRenderer();
        if (!renderer) { ERROR("SpriteSheet::drawNineQuad: renderer is null"); return; }

        // Switch to the target texture, clear it, draw, then restore
        SDL_Texture* prevTarget = SDL_GetRenderTarget(renderer);
        if (!SDL_SetRenderTarget(renderer, targetTexture))
        {
            ERROR("SpriteSheet::drawNineQuad: Unable to set render target: " + std::string(SDL_GetError()));
            return;
        }

        float w = 0.0f, h = 0.0f;
        if (!SDL_GetTextureSize(targetTexture, &w, &h))
        {
            ERROR("SpriteSheet::drawNineQuad: Failed to query target texture size: " + std::string(SDL_GetError()));
            SDL_SetRenderTarget(renderer, prevTarget);
            return;
        }

        // Clear target to transparent
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        // Tile dimensions from this spritesheet
        const int cw = spriteWidth_;
        const int ch = spriteHeight_;
        const float fcw = static_cast<float>(cw);
        const float fch = static_cast<float>(ch);

        const int iw = static_cast<int>(w);
        const int ih = static_cast<int>(h);

        // Compute 9-slice layout dimensions
        const int leftW   = std::min(cw, iw / 2);
        const int rightW  = std::min(cw, iw - leftW);
        const int topH    = std::min(ch, ih / 2);
        const int bottomH = std::min(ch, ih - topH);

        const int centerW = std::max(0, iw - leftW - rightW);
        const int centerH = std::max(0, ih - topH - bottomH);

        const float fleftW = static_cast<float>(leftW);
        const float frightW = static_cast<float>(rightW);
        const float ftopH = static_cast<float>(topH);
        const float fbottomH = static_cast<float>(bottomH);
        const float fcenterW = static_cast<float>(centerW);
        const float fcenterH = static_cast<float>(centerH);

        auto idx = [baseIndex](int offset) { return baseIndex + offset; };

        // Helper to make a source rect clipped within a single tile (offset is tile-local)
        auto make_src_for_clipped = [&](float clipW, float clipH, bool alignRight, bool alignBottom) -> SDL_FRect {
            float src_w = (clipW >= fcw) ? fcw : (fcw * (clipW / fcw));
            float src_h = (clipH >= fch) ? fch : (fch * (clipH / fch));
            float src_x = alignRight ? (fcw - src_w) : 0.0f;
            float src_y = alignBottom ? (fch - src_h) : 0.0f;
            return SDL_FRect{ src_x, src_y, src_w, src_h };
        };

        // Top-left corner
        drawSprite(idx(0), make_src_for_clipped(fleftW, ftopH, false, false), SDL_FRect{0.0f, 0.0f, fleftW, ftopH}, color, scaleMode);
        // Top-center (stretch horizontally; clip vertically if needed)
        if (centerW > 0)
            drawSprite(idx(1), make_src_for_clipped(fcw, ftopH, false, false), SDL_FRect{fleftW, 0.0f, fcenterW, ftopH}, color, scaleMode);
        // Top-right corner (align src to right)
        drawSprite(idx(2), make_src_for_clipped(frightW, ftopH, true, false), SDL_FRect{fleftW + fcenterW, 0.0f, frightW, ftopH}, color, scaleMode);

        // Left-center (stretch vertically; clip horizontally if needed)
        if (centerH > 0)
            drawSprite(idx(3), make_src_for_clipped(fleftW, fch, false, false), SDL_FRect{0.0f, ftopH, fleftW, fcenterH}, color, scaleMode);

        // Center (stretch both axes)
        if (centerW > 0 && centerH > 0)
            drawSprite(idx(4), SDL_FRect{0,0,fcw,fch}, SDL_FRect{fleftW, ftopH, fcenterW, fcenterH}, color, scaleMode);

        // Right-center (stretch vertically; align src to right if clipped horizontally)
        if (centerH > 0)
            drawSprite(idx(5), make_src_for_clipped(frightW, fch, true, false), SDL_FRect{fleftW + fcenterW, ftopH, frightW, fcenterH}, color, scaleMode);

        // Bottom-left corner (align src to bottom)
        drawSprite(idx(6), make_src_for_clipped(fleftW, fbottomH, false, true), SDL_FRect{0.0f, ftopH + fcenterH, fleftW, fbottomH}, color, scaleMode);

        // Bottom-center (stretch horizontally; align src to bottom if clipped vertically)
        if (centerW > 0)
            drawSprite(idx(7), make_src_for_clipped(fcw, fbottomH, false, true), SDL_FRect{fleftW, ftopH + fcenterH, fcenterW, fbottomH}, color, scaleMode);

        // Bottom-right corner (align src to right+bottom)
        drawSprite(idx(8), make_src_for_clipped(frightW, fbottomH, true, true), SDL_FRect{fleftW + fcenterW, ftopH + fcenterH, frightW, fbottomH}, color, scaleMode);

        SDL_SetRenderTarget(renderer, prevTarget);
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
        // Acquire or create the AssetHandle table/usertype (do not clobber; idempotent).
        auto ut = SDOM::IDataObject::register_usertype_with_table<AssetHandle, SDOM::IDataObject>(lua, AssetHandle::LuaHandleName);
        sol::table handle = SDOM::IDataObject::ensure_sol_table(lua, AssetHandle::LuaHandleName);

        auto ss_bind_both = [&](const char* name, auto&& fn) {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                handle.set_function(name, std::forward<decltype(fn)>(fn));
            }
            // Bind on usertype as well for method-call syntax
            try { ut[name] = fn; } catch(...) {}
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

    
    void SpriteSheet::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "SpriteSheet");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    


} // namespace SDOM_
