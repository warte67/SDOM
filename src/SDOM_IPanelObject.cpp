// IPanelObject.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <SDOM/SDOM_Texture.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>
#include <SDOM/SDOM_Label.hpp>

namespace SDOM
{
    IPanelObject::IPanelObject(const InitStruct& init) : IDisplayObject(init)
    {
        // Allow derived panel types (e.g., Frame) to pass their concrete type name.

        icon_resource_ = init.icon_resource;
        font_resource_ = init.font_resource;
        icon_width_ = init.icon_width;
        icon_height_ = init.icon_height;
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        base_index_ = init.base_index;
    } // END: IPanelObject::IPanelObject(const InitStruct& init)


    IPanelObject::IPanelObject(const sol::table& config) : IDisplayObject(config)
    {
        // Accept concrete derived types (e.g., "Frame") in the Lua config.
        std::string type = config["type"].valid() ? config["type"].get<std::string>() : TypeName;
        // --- Initialization Lambdas --- //
        auto get_str = [&](const char* k, const std::string& d = "") -> std::string 
        {
            return config[k].valid() ? config[k].get<std::string>() : d;
        };
        auto get_int = [&](const char* k, int d = 0) -> int 
        {
            return config[k].valid() ? config[k].get<int>() : d;
        };

        InitStruct init;

        // Accept both the legacy keys with trailing underscore (e.g. "icon_resource_")
        // and the more natural keys without the underscore (e.g. "icon_resource").
        if (config["icon_resource"].valid()) init.icon_resource = config["icon_resource"].get<std::string>();
        else init.icon_resource = get_str("icon_resource_", init.icon_resource);
        if (config["font_resource"].valid()) init.font_resource = config["font_resource"].get<std::string>();
        else init.font_resource = get_str("font_resource_", init.font_resource);

        // icon_width and icon_height
        if (config["icon_width"].valid()) init.icon_width = config["icon_width"].get<int>();
        else init.icon_width = get_int("icon_width_", init.icon_width);
        if (config["icon_height"].valid()) init.icon_height = config["icon_height"].get<int>();
        else init.icon_height = get_int("icon_height_", init.icon_height);

        // alias sprite_width and sprite_height
        if (config["sprite_width"].valid()) init.icon_width = config["sprite_width"].get<int>();
        else init.icon_width = get_int("sprite_width_", init.icon_width);
        if (config["sprite_height"].valid()) init.icon_height = config["sprite_height"].get<int>();
        else init.icon_height = get_int("sprite_height_", init.icon_height);

        // font_width and font_height
        if (config["font_width"].valid()) init.font_width = config["font_width"].get<int>();
        else init.font_width = get_int("font_width_", init.font_width);
        if (config["font_height"].valid()) init.font_height = config["font_height"].get<int>();
        else init.font_height = get_int("font_height_", init.font_height);

        // Convert base_index from string if present
        std::string base_index_str = get_str("base_index_", "frame");
        auto it = stringToPanelBaseIndex_.find(base_index_str);
        init.base_index = (it != stringToPanelBaseIndex_.end()) ? it->second : PanelBaseIndex::Frame;

        // Assign to members
        icon_resource_ = init.icon_resource;
        font_resource_ = init.font_resource;
        icon_width_ = init.icon_width;
        icon_height_ = init.icon_height;
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        base_index_ = init.base_index;
    } // END: IPanelObject::IPanelObject(const sol::table& config)


    IPanelObject::IPanelObject(const sol::table& config, const InitStruct& defaults)
        : IDisplayObject(config, defaults)
    {
        // Accept concrete derived types (e.g., "Frame") in the Lua config.
        std::string type = config["type"].valid() ? config["type"].get<std::string>() : TypeName;
        // --- Initialization Lambdas --- //
        auto get_str = [&](const char* k, const std::string& d = "") -> std::string 
        {
            return config[k].valid() ? config[k].get<std::string>() : d;
        };
        auto get_int = [&](const char* k, int d = 0) -> int 
        {
            return config[k].valid() ? config[k].get<int>() : d;
        };

        InitStruct init = defaults;

        // Accept both the legacy keys with trailing underscore (e.g. "icon_resource_")
        // and the more natural keys without the underscore (e.g. "icon_resource").
        if (config["icon_resource"].valid()) init.icon_resource = config["icon_resource"].get<std::string>();
        else init.icon_resource = get_str("icon_resource_", init.icon_resource);
        if (config["font_resource"].valid()) init.font_resource = config["font_resource"].get<std::string>();
        else init.font_resource = get_str("font_resource_", init.font_resource);

        // icon_width and icon_height
        if (config["icon_width"].valid()) init.icon_width = config["icon_width"].get<int>();
        else init.icon_width = get_int("icon_width_", init.icon_width);
        if (config["icon_height"].valid()) init.icon_height = config["icon_height"].get<int>();
        else init.icon_height = get_int("icon_height_", init.icon_height);

        // alias sprite_width and sprite_height
        if (config["sprite_width"].valid()) init.icon_width = config["sprite_width"].get<int>();
        else init.icon_width = get_int("sprite_width_", init.icon_width);
        if (config["sprite_height"].valid()) init.icon_height = config["sprite_height"].get<int>();
        else init.icon_height = get_int("sprite_height_", init.icon_height);

        // font_width and font_height
        if (config["font_width"].valid()) init.font_width = config["font_width"].get<int>();
        else init.font_width = get_int("font_width_", init.font_width);
        if (config["font_height"].valid()) init.font_height = config["font_height"].get<int>();
        else init.font_height = get_int("font_height_", init.font_height);

        // Convert base_index from string if present
        std::string base_index_str = get_str("base_index_", "frame");
        auto it = stringToPanelBaseIndex_.find(base_index_str);
        init.base_index = (it != stringToPanelBaseIndex_.end()) ? it->second : PanelBaseIndex::Frame;

        // Assign to members
        icon_resource_ = init.icon_resource;
        font_resource_ = init.font_resource;
        icon_width_ = init.icon_width;
        icon_height_ = init.icon_height;
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        base_index_ = init.base_index;
    } // END: IPanelObject::IPanelObject(const sol::table& config, const InitStruct& defaults)


    // --- Virtual Methods --- //
    bool IPanelObject::onInit() 
    {
        // Load sprite sheet asset
        if (!spriteSheetAsset_) 
        {
            // Try configured name first
            if (!icon_resource_.empty()) {
                // INFO("IPanelObject::onInit: attempting to load sprite asset by name: " + icon_resource_);
                spriteSheetAsset_ = getFactory().getAssetObject(icon_resource_);
                if (spriteSheetAsset_) {
                    // INFO("IPanelObject::onInit: found asset '" + spriteSheetAsset_->getName() + "' type='" + spriteSheetAsset_->getType() + "' file='" + spriteSheetAsset_->getFilename() + "'");
                }
            }

            // If not found, many resources register a SpriteSheet under the filename + "_SpriteSheet"
            // If we found a Texture, it's common that a SpriteSheet exists which uses
            // that texture as its filename. Prefer the SpriteSheet when available.
            if (spriteSheetAsset_ && spriteSheetAsset_->getType() == Texture::TypeName) 
            {
                // INFO("IPanelObject::onInit: resolved asset is a Texture; attempting to find matching SpriteSheet for file: " + spriteSheetAsset_->getFilename());
                // Use the public API to find any SpriteSheet asset that references the same texture filename
                AssetHandle candidate = getFactory().findAssetByFilename(spriteSheetAsset_->getFilename(), SpriteSheet::TypeName);
                if (candidate.isValid()) 
                {
                    SpriteSheet* ssPtr = candidate.as<SpriteSheet>();
                    if (ssPtr && ssPtr->getSpriteWidth() == icon_width_ && ssPtr->getSpriteHeight() == icon_height_) {
                        // INFO("IPanelObject::onInit: found SpriteSheet '" + candidate.getName() + "' matching texture file='" + candidate.getFilename() + "' and sprite size");
                        spriteSheetAsset_ = candidate;
                        icon_resource_ = candidate.getName();
                    } else {
                        // INFO("IPanelObject::onInit: found SpriteSheet candidate '" + candidate.getName() + "' but sprite size did not match (expected " + std::to_string(icon_width_) + "x" + std::to_string(icon_height_) + ")");
                    }
                } else {
                    // INFO("IPanelObject::onInit: no SpriteSheet asset found referencing texture file: " + spriteSheetAsset_->getFilename());
                }
            }

            // Additional fallback: if the configured name resolved to a Texture but
            // a SpriteSheet resource of the form <name>_SpriteSheet exists, prefer it.
            // This handles common usage where Lua config references the texture name
            // while the engine registers a SpriteSheet wrapper under the '<name>_SpriteSheet' key.
            if (spriteSheetAsset_ && spriteSheetAsset_->getType() == Texture::TypeName) {
                std::string altName = icon_resource_ + std::string("_SpriteSheet");
                AssetHandle altHandle = getFactory().getAssetObject(altName);
                if (altHandle.isValid() && altHandle->getType() == SpriteSheet::TypeName) {
                    spriteSheetAsset_ = altHandle;
                    icon_resource_ = altName;
                }
            }

            if (!spriteSheetAsset_) {
                std::string alt = icon_resource_ + std::string("_SpriteSheet");
                // INFO("IPanelObject::onInit: attempting fallback sprite asset name: " + alt);
                spriteSheetAsset_ = getFactory().getAssetObject(alt);
                if (spriteSheetAsset_) {
                    // INFO("IPanelObject::onInit: found fallback asset '" + spriteSheetAsset_->getName() + "' type='" + spriteSheetAsset_->getType() + "' file='" + spriteSheetAsset_->getFilename() + "'");
                    // Update our internal name to the resolved resource so later diagnostics match
                    icon_resource_ = alt;
                }
            }

            if (!spriteSheetAsset_) 
            {
                ERROR("IPanelObject::onInit: Failed to load sprite sheet asset: " + icon_resource_);
                getFactory().printAssetTree();
                return false;
            }
        }
        SpriteSheet* ss = spriteSheetAsset_.as<SpriteSheet>();
        if (!ss)
        {
            ERROR("IPanelObject::onInit: spriteSheetAsset_ is not a SpriteSheet type for asset: " + icon_resource_);
            return false;
        }
        icon_width_ = ss->getSpriteWidth();
        icon_height_ = ss->getSpriteHeight();

        // Load font asset
        if (!font_resource_.empty() && !fontAsset_)
        {
            fontAsset_ = getFactory().getAssetObject(font_resource_);
            if (!fontAsset_)
            {
                ERROR("IPanelObject::onInit: Failed to load font asset: " + font_resource_);
                return false;
            }
        }

        return true;
    } // END: bool IPanelObject::onInit() 


    void IPanelObject::onQuit() 
    {
        // Release any renderer-owned resources so device rebuilds don't leave
        // stale textures around. Factory manages asset lifetimes separately.
        if (cachedTexture_)
        {
            SDL_DestroyTexture(cachedTexture_);
            cachedTexture_ = nullptr;
        }
        current_width_ = 0;
        current_height_ = 0;
        current_pixel_format_ = SDL_PIXELFORMAT_UNKNOWN;
        setDirty(true);
    } // END: void IPanelObject::onQuit() 

    // Cached-texture + window-resize contract
    //
    // Panel objects render a 9-slice into a cached SDL_Texture. When SDL
    // resources are recreated due to configuration changes or window resizes,
    // the cached texture may be invalid. Core broadcasts onWindowResize in
    // those cases; invalidate and mark dirty so onRender() rebuilds next frame.
    void IPanelObject::onWindowResize(int /*logicalWidth*/, int /*logicalHeight*/)
    {
        if (cachedTexture_)
        {
            SDL_DestroyTexture(cachedTexture_);
            cachedTexture_ = nullptr;
        }
        current_width_ = 0;
        current_height_ = 0;
        current_pixel_format_ = SDL_PIXELFORMAT_UNKNOWN;
        setDirty(true);
    }

    

    // void IPanelObject::onEvent(const Event& event)=0;

    // void IPanelObject::onUpdate(float fElapsedTime)=0;


    void IPanelObject::onRender() 
    {
        SDL_Renderer* renderer = getRenderer();
        if (!renderer) { ERROR("IPanelObject::onRender: renderer is null"); return; }

        // After device rebuilds, cached textures can become invalid even if the
        // pointer is non-null. Validate against the renderer and mark dirty to
        // force a rebuild before drawing if needed.
        if (cachedTexture_)
        {
            float tw = 0.0f, th = 0.0f;
            if (!SDL_GetTextureSize(cachedTexture_, &tw, &th))
            {
                SDL_DestroyTexture(cachedTexture_);
                cachedTexture_ = nullptr;
                setDirty(true);
            }
        }
        // If format/size changed since last build, force a rebuild.
        if (current_pixel_format_ != getCore().getPixelFormat() ||
            current_width_ != getWidth() || current_height_ != getHeight())
        {
            setDirty(true);
        }

        // If dirty, (re)build the cached panel texture and render the 9-slice into it
        if (isDirty())
        {
            if (!rebuildPanelTexture_(getWidth(), getHeight(), getCore().getPixelFormat()))
            {
                ERROR("IPanelObject::onRender: failed to rebuild panel texture");
                return;
            }

            if (cachedTexture_)
            {
                SpriteSheet* ss = getSpriteSheetPtr();
                if (!ss) { ERROR("IPanelObject::onRender: invalid SpriteSheet"); return; }

                try { ss->onLoad(); } catch(...) {}
                SDL_Color color = getColor();
                ss->drawNineQuad(static_cast<int>(base_index_), cachedTexture_, color, SDL_SCALEMODE_NEAREST);
            }
            setDirty(false);
        }

        // Draw the cached panel texture
        if (cachedTexture_)
        {
            SDL_FRect dst = {
                static_cast<float>(getX()),
                static_cast<float>(getY()),
                static_cast<float>(getWidth()),
                static_cast<float>(getHeight())
            };
            if (!SDL_RenderTexture(renderer, cachedTexture_, nullptr, &dst))
            {
                ERROR("IPanelObject::onRender: failed to render cached panel texture: " + std::string(SDL_GetError()));
            }
        }
        else
        {
            // Fallback: draw live if caching unavailable
            renderPanel();
        }
    } // END: void IPanelObject::onRender() 


    // --- Helper Methods --- // 
    void IPanelObject::renderPanel()
    {
        if (!spriteSheetAsset_) return;
        SpriteSheet* ss = getSpriteSheetPtr(); 
        if (!ss) { ERROR("IPanelObject::renderPanel(): No valid SpriteSheet for icon."); return; }

        int x = getX(), y = getY(), w = getWidth(), h = getHeight();
        int cw = icon_width_, ch = icon_height_;

        int leftW   = std::min(cw, w / 2);
        int rightW  = std::min(cw, w - leftW);
        int topH    = std::min(ch, h / 2);
        int bottomH = std::min(ch, h - topH);

        int centerW = std::max(0, w - leftW - rightW);
        int centerH = std::max(0, h - topH - bottomH);

        float fx = static_cast<float>(x), fy = static_cast<float>(y);
        float fleftW = static_cast<float>(leftW), frightW = static_cast<float>(rightW);
        float ftopH = static_cast<float>(topH), fbottomH = static_cast<float>(bottomH);
        float fcenterW = static_cast<float>(centerW), fcenterH = static_cast<float>(centerH);
        float fcw = static_cast<float>(cw), fch = static_cast<float>(ch);
        int base = static_cast<int>(base_index_);
        auto idx = [base](PanelTileOffset pto) { return base + static_cast<int>(pto); };
        SDL_Color color = getColor();

        using PTO = PanelTileOffset;

        // Ensure the sprite sheet's underlying texture is loaded before drawing
        try { ss->onLoad(); } catch(...) {}

        // Top row (src rects use texture pixel sizes)
        // For 9-slice rendering we treat the sprite tile as the native size (tileW x tileH).
        // Corners are drawn at native size (clipped if panel smaller). Edges are stretched in one axis.
        float tileW = static_cast<float>(ss->getSpriteWidth());
        float tileH = static_cast<float>(ss->getSpriteHeight());

        auto make_src_for_clipped = [&](int spriteIndex, float clipW, float clipH, bool alignRight, bool alignBottom) -> SDL_FRect {
            // srcRect is tile-local (offset within the tile). If clipped, select the appropriate sub-rect
            float src_w = (clipW >= static_cast<float>(cw)) ? tileW : (tileW * (clipW / static_cast<float>(cw)));
            float src_h = (clipH >= static_cast<float>(ch)) ? tileH : (tileH * (clipH / static_cast<float>(ch)));
            float src_x = alignRight ? (tileW - src_w) : 0.0f;
            float src_y = alignBottom ? (tileH - src_h) : 0.0f;
            return SDL_FRect{ src_x, src_y, src_w, src_h };
        };

        // Top-left corner
        ss->drawSprite(idx(PTO::TopLeft), make_src_for_clipped(idx(PTO::TopLeft), fleftW, ftopH, false, false), {fx, fy, fleftW, ftopH}, color);

        // Top-center (stretch horizontally; clip vertically if needed)
        if (centerW > 0)
            ss->drawSprite(idx(PTO::TopCenter), make_src_for_clipped(idx(PTO::TopCenter), fcw, ftopH, false, false), {fx + fleftW, fy, fcenterW, ftopH}, color);

        // Top-right corner (align src to right)
        ss->drawSprite(idx(PTO::TopRight), make_src_for_clipped(idx(PTO::TopRight), frightW, ftopH, true, false), {fx + fleftW + fcenterW, fy, frightW, ftopH}, color);

        // Left-center (stretch vertically; clip horizontally if needed)
        if (centerH > 0)
            ss->drawSprite(idx(PTO::LeftCenter), make_src_for_clipped(idx(PTO::LeftCenter), fleftW, fch, false, false), {fx, fy + ftopH, fleftW, fcenterH}, color);

        // Center (stretch both axes)
        if (centerW > 0 && centerH > 0)
            ss->drawSprite(idx(PTO::Center), SDL_FRect{0,0,tileW,tileH}, {fx + fleftW, fy + ftopH, fcenterW, fcenterH}, color);

        // Right-center (stretch vertically; align src to right if clipped horizontally)
        if (centerH > 0)
            ss->drawSprite(idx(PTO::RightCenter), make_src_for_clipped(idx(PTO::RightCenter), frightW, fch, true, false), {fx + fleftW + fcenterW, fy + ftopH, frightW, fcenterH}, color);

        // Bottom-left corner (align src to bottom)
        ss->drawSprite(idx(PTO::BottomLeft), make_src_for_clipped(idx(PTO::BottomLeft), fleftW, fbottomH, false, true), {fx, fy + ftopH + fcenterH, fleftW, fbottomH}, color);

        // Bottom-center (stretch horizontally; align src to bottom if clipped vertically)
        if (centerW > 0)
            ss->drawSprite(idx(PTO::BottomCenter), make_src_for_clipped(idx(PTO::BottomCenter), fcw, fbottomH, false, true), {fx + fleftW, fy + ftopH + fcenterH, fcenterW, fbottomH}, color);

        // Bottom-right corner (align src to right+bottom)
        ss->drawSprite(idx(PTO::BottomRight), make_src_for_clipped(idx(PTO::BottomRight), frightW, fbottomH, true, true), {fx + fleftW + fcenterW, fy + ftopH + fcenterH, frightW, fbottomH}, color);
    } // END: void IPanelObject::renderPanel()


    bool IPanelObject::rebuildPanelTexture_(int width, int height, SDL_PixelFormat fmt)
    {
        // Destroy and recreate if format/size changed
        bool needCreate = false;
        if (!cachedTexture_)
        {
            needCreate = true;
        }
        else
        {
            float tw = 0.0f, th = 0.0f;
            if (!SDL_GetTextureSize(cachedTexture_, &tw, &th))
            {
                // texture became invalid; recreate
                needCreate = true;
            }
            else if (static_cast<int>(tw) != width || static_cast<int>(th) != height || current_pixel_format_ != fmt)
            {
                needCreate = true;
            }
        }

        if (!needCreate)
            return true;

        if (cachedTexture_)
        {
            SDL_DestroyTexture(cachedTexture_);
            cachedTexture_ = nullptr;
        }

        if (width <= 0 || height <= 0)
        {
            current_pixel_format_ = fmt;
            current_width_ = width;
            current_height_ = height;
            return true; // nothing to create at zero size
        }

        cachedTexture_ = SDL_CreateTexture(getRenderer(), fmt, SDL_TEXTUREACCESS_TARGET, width, height);
        if (!cachedTexture_)
        {
            ERROR("IPanelObject::rebuildPanelTexture_: failed to create texture: " + std::string(SDL_GetError()));
            return false;
        }
        if (!SDL_SetTextureBlendMode(cachedTexture_, SDL_BLENDMODE_BLEND))
        {
            ERROR("IPanelObject::rebuildPanelTexture_: failed to set texture blend mode: " + std::string(SDL_GetError()));
            return false;
        }
        if (!SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND))
        {
            ERROR("IPanelObject::rebuildPanelTexture_: failed to set render blend mode: " + std::string(SDL_GetError()));
            return false;
        }

        current_pixel_format_ = fmt;
        current_width_ = width;
        current_height_ = height;
        return true;
    }


    // --- Lua Registration --- //
    void IPanelObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IPanelObject";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN 
                    << typeName << CLR::RESET << std::endl;
        }

        // // Augment the single shared DisplayHandle handle usertype
        // sol::table handle = DisplayHandle::ensure_handle_table(lua);

    } // END: void IPanelObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END namespace SDOM
