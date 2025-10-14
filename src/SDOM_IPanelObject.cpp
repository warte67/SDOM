// IPanelObject.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <SDOM/SDOM_Texture.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>

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
        // We dont want to reset the assets here.  Let the factory do it
        // ...
    } // END: void IPanelObject::onQuit() 

    

    // void IPanelObject::onUpdate(float fElapsedTime)=0;
    // void IPanelObject::onEvent(const Event& event)=0;



    void IPanelObject::onRender() 
    {
        renderPanel();
    } // END: void IPanelObject::onRender() 


    // --- Helper Methods --- // 
    void IPanelObject::renderPanel()
    {
        if (!spriteSheetAsset_) return;

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

        // SDL_Color color = getColor();
        // SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        using PTO = PanelTileOffset;

        SpriteSheet* sprite_sheet = spriteSheetAsset_->as<SpriteSheet>();
        if (!sprite_sheet)
        {
            getFactory().printAssetTree();
            std::string atype = spriteSheetAsset_.isValid() ? spriteSheetAsset_->getType() : std::string("<invalid>");
            std::string afile = spriteSheetAsset_.isValid() ? spriteSheetAsset_->getFilename() : std::string("<invalid>");
            ERROR("IPanelObject::renderPanel: sprite asset (" + spriteSheetAsset_->getName() + ") is not a SpriteSheet; resolved type='" + atype + "' file='" + afile + "'");
            return;
        }

    // Ensure the sprite sheet's underlying texture is loaded before drawing
    try { sprite_sheet->onLoad(); } catch(...) {}

    // Top row
        sprite_sheet->drawSprite(idx(PTO::TopLeft), {0, 0, fleftW, ftopH}, {fx, fy, fleftW, ftopH}, color);
        if (centerW > 0)
            sprite_sheet->drawSprite(idx(PTO::TopCenter), {0, 0, fcw, ftopH}, {fx + fleftW, fy, fcenterW, ftopH}, color);
        sprite_sheet->drawSprite(idx(PTO::TopRight), {fcw - frightW, 0, frightW, ftopH}, {fx + fleftW + fcenterW, fy, frightW, ftopH}, color);

        // Middle row
        if (centerH > 0)
            sprite_sheet->drawSprite(idx(PTO::LeftCenter), {0, 0, fleftW, fch}, {fx, fy + ftopH, fleftW, fcenterH}, color);
        if (centerW > 0 && centerH > 0)
            sprite_sheet->drawSprite(idx(PTO::Center), {0, 0, fcw, fch}, {fx + fleftW, fy + ftopH, fcenterW, fcenterH}, color);
        if (centerH > 0)
            sprite_sheet->drawSprite(idx(PTO::RightCenter), {fcw - frightW, 0, frightW, fch}, {fx + fleftW + fcenterW, fy + ftopH, frightW, fcenterH}, color);

        // Bottom row
        sprite_sheet->drawSprite(idx(PTO::BottomLeft), {0, fch - fbottomH, fleftW, fbottomH}, {fx, fy + ftopH + fcenterH, fleftW, fbottomH}, color);
        if (centerW > 0)
            sprite_sheet->drawSprite(idx(PTO::BottomCenter), {0, fch - fbottomH, fcw, fbottomH}, {fx + fleftW, fy + ftopH + fcenterH, fcenterW, fbottomH}, color);
        sprite_sheet->drawSprite(idx(PTO::BottomRight), {fcw - frightW, fch - fbottomH, frightW, fbottomH}, {fx + fleftW + fcenterW, fy + ftopH + fcenterH, frightW, fbottomH}, color);
    } // END: void IPanelObject::renderPanel()


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

        // Augment the single shared DisplayHandle handle usertype
        sol::table handle = DisplayHandle::ensure_handle_table(lua);

        // Helper to check if a property/command is already registered
        auto absent = [&](const char* name) -> bool {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        // small helper to validate and cast the DisplayHandle -> IPanelObject*
        auto cast_panel_from_handle = [](DisplayHandle& h) -> IPanelObject* {
            if (!h.isValid()) { ERROR("invalid DisplayHandle provided to IPanelObject method"); }
            IDisplayObject* base = dynamic_cast<IDisplayObject*>(h.get());
            IPanelObject* panel = dynamic_cast<IPanelObject*>(base);
            if (!panel) { ERROR("invalid IPanelObject object"); }
            return panel;
        };

        // Register accessors on the DisplayHandle shared table
        if (absent("getIconResourceName")) {
            handle.set_function("getIconResourceName", [cast_panel_from_handle](DisplayHandle& self) -> std::string {
                IPanelObject* p = cast_panel_from_handle(self);
                return p->getIconResourceName();
            });
        }
        if (absent("setIconResourceName")) {
            handle.set_function("setIconResourceName", [cast_panel_from_handle](DisplayHandle& self, const std::string& s) {
                IPanelObject* p = cast_panel_from_handle(self);
                // set by name and update internal asset handle lazily
                p->icon_resource_ = s;
                p->spriteSheetAsset_ = AssetHandle();
            });
        }

        if (absent("getFontResourceName")) {
            handle.set_function("getFontResourceName", [cast_panel_from_handle](DisplayHandle& self) -> std::string {
                IPanelObject* p = cast_panel_from_handle(self);
                return p->getFontResourceName();
            });
        }
        if (absent("setFontResourceName")) {
            handle.set_function("setFontResourceName", [cast_panel_from_handle](DisplayHandle& self, const std::string& s) {
                IPanelObject* p = cast_panel_from_handle(self);
                p->font_resource_ = s;
                p->fontAsset_ = AssetHandle();
            });
        }

        // icon/font dimensions
        if (absent("getIconWidth")) { handle.set_function("getIconWidth", [cast_panel_from_handle](DisplayHandle& self) -> int { return cast_panel_from_handle(self)->getIconWidth(); }); }
        if (absent("setIconWidth")) { handle.set_function("setIconWidth", [cast_panel_from_handle](DisplayHandle& self, int v) { cast_panel_from_handle(self)->icon_width_ = v; }); }
        if (absent("getIconHeight")) { handle.set_function("getIconHeight", [cast_panel_from_handle](DisplayHandle& self) -> int { return cast_panel_from_handle(self)->getIconHeight(); }); }
        if (absent("setIconHeight")) { handle.set_function("setIconHeight", [cast_panel_from_handle](DisplayHandle& self, int v) { cast_panel_from_handle(self)->icon_height_ = v; }); }
        if (absent("getFontWidth")) { handle.set_function("getFontWidth", [cast_panel_from_handle](DisplayHandle& self) -> int { return cast_panel_from_handle(self)->getFontWidth(); }); }
        if (absent("setFontWidth")) { handle.set_function("setFontWidth", [cast_panel_from_handle](DisplayHandle& self, int v) { cast_panel_from_handle(self)->font_width_ = v; }); }
        if (absent("getFontHeight")) { handle.set_function("getFontHeight", [cast_panel_from_handle](DisplayHandle& self) -> int { return cast_panel_from_handle(self)->getFontHeight(); }); }
        if (absent("setFontHeight")) { handle.set_function("setFontHeight", [cast_panel_from_handle](DisplayHandle& self, int v) { cast_panel_from_handle(self)->font_height_ = v; }); }

        // base index getter/setter (expose strings)
        if (absent("getBaseIndex")) {
            handle.set_function("getBaseIndex", [cast_panel_from_handle]() -> std::string {
                // NOTE: this closure doesn't have a DisplayHandle param so fetch panel via Core is not appropriate; provide overload that accepts DisplayHandle
                return std::string();
            });
        }
        if (absent("getBaseIndex")) {
            handle.set_function("getBaseIndex", [cast_panel_from_handle](DisplayHandle& self) -> std::string {
                IPanelObject* p = cast_panel_from_handle(self);
                auto it = panelBaseIndexToString_.find(p->getBaseIndex());
                return (it != panelBaseIndexToString_.end()) ? it->second : std::string("frame");
            });
        }
        if (absent("setBaseIndex")) {
            handle.set_function("setBaseIndex", [cast_panel_from_handle](DisplayHandle& self, const std::string& s) {
                IPanelObject* p = cast_panel_from_handle(self);
                auto it = stringToPanelBaseIndex_.find(s);
                if (it != stringToPanelBaseIndex_.end()) p->base_index_ = it->second;
            });
        }

        // Expose AssetHandles for spriteSheet and fontAsset
        if (absent("getSpriteSheet")) {
            handle.set_function("getSpriteSheet", [cast_panel_from_handle](DisplayHandle& self) -> AssetHandle {
                IPanelObject* p = cast_panel_from_handle(self);
                // ensure lazy load
                if (!p->spriteSheetAsset_) p->spriteSheetAsset_ = getFactory().getAssetObject(p->icon_resource_);
                return p->spriteSheetAsset_;
            });
        }
        if (absent("getFontAsset")) {
            handle.set_function("getFontAsset", [cast_panel_from_handle](DisplayHandle& self) -> AssetHandle {
                IPanelObject* p = cast_panel_from_handle(self);
                if (!p->fontAsset_ && !p->font_resource_.empty()) p->fontAsset_ = getFactory().getAssetObject(p->font_resource_);
                return p->fontAsset_;
            });
        }


    } // END: void IPanelObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END namespace SDOM