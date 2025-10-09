// SDOM_BitmapFont.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_AssetObject.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>

namespace SDOM
{
    BitmapFont::BitmapFont(const InitStruct& init) : IFontObject(init)
    {

        bitmapFontWidth_ = init.fontWidth;
        bitmapFontHeight_ = init.fontHeight;
        if (init.fontWidth < 0)  bitmapFontWidth_ = init.fontSize_;
        if (init.fontHeight < 0) bitmapFontHeight_ = init.fontSize_;
    }

    BitmapFont::BitmapFont(const sol::table& config) : IFontObject(config)
    {
        // Prefer Lua keys: "size", "width", "height".
        // Also accept legacy keys: "fontSize", "fontWidth", "fontHeight".
        if (config["size"].valid())        fontSize_ = config["size"].get<int>(); else fontSize_ = 10;
        if (config["width"].valid())       bitmapFontWidth_ = config["width"].get<int>(); else bitmapFontWidth_ = 8;
        if (config["height"].valid())      bitmapFontHeight_ = config["height"].get<int>(); else bitmapFontHeight_ = 8;

        // legacy overrides
        if (config["fontSize"].valid())    fontSize_ = config["fontSize"].get<int>();
        if (config["fontWidth"].valid())   bitmapFontWidth_ = config["fontWidth"].get<int>();
        if (config["fontHeight"].valid())  bitmapFontHeight_ = config["fontHeight"].get<int>();

        // normalize invalid/zero dimensions to fontSize_
        if (bitmapFontWidth_ <= 0)  bitmapFontWidth_ = fontSize_;
        if (bitmapFontHeight_ <= 0) bitmapFontHeight_ = fontSize_;
    }


    BitmapFont::~BitmapFont()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "~BitmapFont()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Destructor implementation if needed
        // ...
    }


    bool BitmapFont::onInit()
    {
        SpriteSheet::InitStruct init;
        init.filename = filename_;
        init.name = name_ + "_SpriteSheet";
        init.type = SpriteSheet::TypeName;            // use SpriteSheet type constant
        init.spriteWidth = bitmapFontWidth_;
        init.spriteHeight = bitmapFontHeight_;

        AssetObject existing = getFactory().getAssetObject(init.name);
        if (existing.isValid())
        {
            ERROR("BitmapFont::onInit: Asset with name already exists: " + init.name);
            return false;
        }

        // create by type, not by name
        spriteSheet_ = getFactory().createAsset(SpriteSheet::TypeName, init);
        if (!spriteSheet_)
        {
            ERROR("BitmapFont::onInit: Factory failed to create SpriteSheet asset for filename: " + filename_);
            return false;
        }

        // Defensive: ensure we actually got a SpriteSheet
        if (spriteSheet_.as<SpriteSheet>() == nullptr)
        {
            ERROR("BitmapFont::onInit: Created asset is not a SpriteSheet for filename: " + filename_);
            spriteSheet_.reset();
            return false;
        }

        // Ensure the asset is loaded (use AssetObject API to do lazy load)
        spriteSheet_.get(); // ensures onLoad() is invoked
        return true;
    } // END onInit()
    
    void BitmapFont::onQuit()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onQuit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;

        // If we hold a SpriteSheet handle, ensure it's unloaded and release our reference.
        if (spriteSheet_.isValid()) 
        {
            auto* ss = spriteSheet_.as<SpriteSheet>();
            if (ss && ss->isLoaded()) 
            {
                ss->onUnload();   // unload texture/resources
            }
            spriteSheet_.reset(); // drop our handle so the Factory/cache can reclaim if unused
        }    
    } // END onQuit()

    // void BitmapFont::onLoad()
    // {
    //     // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onLoad()" 
    //     //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
    //     // Loading logic for BitmapFont
    // } // END onLoad()

    // void BitmapFont::onUnload()
    // {
    //     // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onUnload()" 
    //     //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
    //     // Unloading logic for BitmapFont
    // }

    void BitmapFont::onLoad()
    {
        static thread_local bool in_onload = false;
        if (in_onload) { 
            std::cerr << "BitmapFont::onLoad - re-entrant call detected for: " << getName() << std::endl;
            return;
        }
        in_onload = true;

        std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onLoad()"
                << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;

        if (isLoaded_) onUnload();

        // Resolve or create the backing SpriteSheet asset (use AssetObject API)
        std::string sheetName = name_ + "_SpriteSheet";
        AssetObject existing = getFactory().getAssetObject(sheetName);
        if (existing.isValid())
        {
            // protect against accidental self-reference
            IAssetObject* raw = existing.get();
            if (raw == static_cast<IAssetObject*>(this)) {
                ERROR("BitmapFont::onLoad: found self as SpriteSheet asset: " + sheetName);
                in_onload = false;
                return;
            }
            spriteSheet_ = existing;
        }
        else
        {
            SpriteSheet::InitStruct init;
            init.name = sheetName;
            init.type = SpriteSheet::TypeName;
            init.filename = filename_;
            init.spriteWidth = bitmapFontWidth_;
            init.spriteHeight = bitmapFontHeight_;

            spriteSheet_ = getFactory().createAsset(SpriteSheet::TypeName, init);
            if (!spriteSheet_) {
                ERROR("BitmapFont::onLoad: Factory failed to create SpriteSheet asset: " + sheetName);
                in_onload = false;
                return;
            }

            // sanity: ensure factory didn't hand back a handle to ourselves
            IAssetObject* raw = spriteSheet_.get();
            if (raw == static_cast<IAssetObject*>(this)) {
                ERROR("BitmapFont::onLoad: Factory returned self for created SpriteSheet: " + sheetName);
                spriteSheet_.reset();
                in_onload = false;
                return;
            }
        }

        // Validate & ensure loaded (avoid forcing load that can re-enter this code path)
        IAssetObject* raw = spriteSheet_.get();
        SpriteSheet* ss = raw ? dynamic_cast<SpriteSheet*>(raw) : nullptr;
        if (!ss) {
            ERROR("BitmapFont::onLoad: resolved asset is not a SpriteSheet: " + sheetName);
            spriteSheet_.reset();
            in_onload = false;
            return;
        }

        // Only request load if not already loaded (this can still recurse; guard prevents us looping)
        if (!ss->isLoaded()) {
            ss->onLoad();
        }

        // Sync metrics
        int sw = ss->getSpriteWidth();
        int sh = ss->getSpriteHeight();
        if (sw > 0) bitmapFontWidth_ = sw;
        if (sh > 0) bitmapFontHeight_ = sh;

        isLoaded_ = true;
        in_onload = false;
        
    } // END onLoad()

    void BitmapFont::onUnload()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onUnload()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;

        // Unload/release the sprite sheet we created/hold
        if (spriteSheet_ && isLoaded_) spriteSheet_->onUnload();
        isLoaded_ = false;
    } // END onUnload()

    void BitmapFont::create(const sol::table& config)
    {
        // Update basic identity/config from Lua table
        if (config["name"].valid())     name_ = config["name"].get<std::string>();
        if (config["filename"].valid()) filename_ = config["filename"].get<std::string>();

        // size/width/height (prefer new keys, accept legacy)
        if (config["size"].valid())        fontSize_ = config["size"].get<int>();
        if (config["width"].valid())       bitmapFontWidth_ = config["width"].get<int>();
        if (config["height"].valid())      bitmapFontHeight_ = config["height"].get<int>();

        if (config["fontSize"].valid())    fontSize_ = config["fontSize"].get<int>();
        if (config["fontWidth"].valid())   bitmapFontWidth_ = config["fontWidth"].get<int>();
        if (config["fontHeight"].valid())  bitmapFontHeight_ = config["fontHeight"].get<int>();

        // normalize invalid/zero dimensions to fontSize_
        if (bitmapFontWidth_ <= 0)  bitmapFontWidth_ = (fontSize_ > 0 ? fontSize_ : 8);
        if (bitmapFontHeight_ <= 0) bitmapFontHeight_ = (fontSize_ > 0 ? fontSize_ : 8);

        // Optional explicit spriteSheet name; default to "<name>_SpriteSheet"
        std::string sheetName;
        if (config["spriteSheetName"].valid()) sheetName = config["spriteSheetName"].get<std::string>();
        else sheetName = name_.empty() ? std::string("bitmap_sprite_sheet") : (name_ + "_SpriteSheet");

        // If we already hold a spriteSheet, unload and drop it so create/rebind proceeds cleanly.
        if (spriteSheet_.isValid())
        {
            SpriteSheet* ss = spriteSheet_.as<SpriteSheet>();
            if (ss && ss->isLoaded()) ss->onUnload();
            spriteSheet_.reset();
        }

        // Try to reuse an existing factory asset with that name, else create a new SpriteSheet asset.
        AssetObject existing = getFactory().getAssetObject(sheetName);
        if (existing.isValid())
        {
            spriteSheet_ = existing;
        }
        else
        {
            SpriteSheet::InitStruct init;
            init.filename = filename_;
            init.name = sheetName;
            init.type = SpriteSheet::TypeName;
            init.spriteWidth = bitmapFontWidth_;
            init.spriteHeight = bitmapFontHeight_;

            spriteSheet_ = getFactory().createAsset(SpriteSheet::TypeName, init);
            if (!spriteSheet_)
            {
                ERROR("BitmapFont::create - failed to create SpriteSheet asset: " + sheetName);
                return;
            }
        }

        // Ensure the sprite sheet is loaded and sync metrics
        SpriteSheet* ss = spriteSheet_.as<SpriteSheet>();
        if (!ss)
        {
            ERROR("BitmapFont::create - resolved asset is not a SpriteSheet: " + sheetName);
            spriteSheet_.reset();
            return;
        }
        if (!ss->isLoaded()) ss->onLoad();

        int sw = ss->getSpriteWidth();
        int sh = ss->getSpriteHeight();
        if (sw > 0) bitmapFontWidth_ = sw;
        if (sh > 0) bitmapFontHeight_ = sh;

        // mark this font as configured/loaded
        isLoaded_ = true;
    } // END create()

    void BitmapFont::drawGlyph(Uint32 ch, int x, int y, const FontStyle& style)
    {
        // Draw a single glyph at the specified position with the given style
    }

    void BitmapFont::drawPhrase(const std::string& str, int x, int y, const FontStyle& style)
    {
        // Draw a phrase (string) at the specified position with the given style
    }

    void BitmapFont::drawPhraseOutline(const std::string& str, int x, int y, const FontStyle& style)
    {
        // Draw a phrase with an outline effect
    }

    void BitmapFont::drawPhraseDropshadow(const std::string& str, int x, int y, const FontStyle& style)
    {
        // Draw a phrase with a drop shadow effect
    }

    bool BitmapFont::getGlyphMetrics(Uint32 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance) const
    {
        // Retrieve metrics for the specified glyph
        return false; // Placeholder return value
    }

    int BitmapFont::getGlyphHeight(Uint32 ch) const
    {
        // Return the height of the specified glyph
        return bitmapFontHeight_; // Placeholder return value
    }

    int BitmapFont::getGlyphWidth(Uint32 ch) const
    {
        // Return the width of the specified glyph
        return bitmapFontWidth_; // Placeholder return value
    }

    int BitmapFont::getFontAscent()
    {
        // Return the font ascent value
        return 0; // Placeholder return value
    }

    void BitmapFont::setFontSize(int p_size)
    {
        fontSize_ = p_size;
    }

    void BitmapFont::setFontStyle(FontStyle& style)
    {
        setFontSize(style.fontSize);
    }

    FontStyle BitmapFont::getFontStyle()
    {
        FontStyle style;
        style.fontSize = fontSize_;
        return style;  // Return current style (placeholder implementation)
    }

    // --- Protected Methods --- //

    void BitmapFont::initializeOutlineGlyph(Uint32 ch, int x, int y)
    {
        // Initialize outline glyph rendering
    }
    void BitmapFont::drawForegroundGlyph(Uint32 ch, int x, int y, const FontStyle& style)
    {
        // draw the foreground glyph
    }
    void BitmapFont::drawOutlineGlyph(Uint32 ch, int x, int y, const FontStyle& style)
    {
        // draw the outline glyph
    }
    void BitmapFont::drawDropShadowGlyph(Uint32 ch, int x, int y, const FontStyle& style)
    {
        // draw the drop shadow glyph
    }

    // --- Lua Registration --- //

    void BitmapFont::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "BitmapFont:" + getName();
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << typeNameLocal 
                    << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA 
                    << typeName << CLR::RESET << std::endl;
        }

        // Augment the single shared AssetObject handle usertype (assets are exposed via AssetObject handles in Lua)
        sol::table handle = AssetObject::ensure_handle_table(lua);

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

        // small helper to validate and cast the AssetObject -> BitmapFont*
        auto cast_ss_from_asset = [](const AssetObject& asset) -> BitmapFont* {
            if (!asset.isValid()) { ERROR("invalid AssetObject provided to BitmapFont method"); }
            IAssetObject* base = asset.get();
            BitmapFont* bmp = dynamic_cast<BitmapFont*>(base);
            if (!bmp) { ERROR("invalid BitmapFont object"); }
            return bmp;
        };

        // // Register BitmapFont-specific properties and commands here (bridge from AssetObject handle)
        reg("setBitmapFontWidth", [cast_ss_from_asset](AssetObject asset, int w) { cast_ss_from_asset(asset)->setBitmapFontWidth(w); });
        reg("setBitmapFontHeight", [cast_ss_from_asset](AssetObject asset, int h) { cast_ss_from_asset(asset)->setBitmapFontHeight(h); });
        reg("getBitmapFontWidth", [cast_ss_from_asset](AssetObject asset) { return cast_ss_from_asset(asset)->getBitmapFontWidth(); });
        reg("getBitmapFontHeight", [cast_ss_from_asset](AssetObject asset) { return cast_ss_from_asset(asset)->getBitmapFontHeight(); });



    } // END _registerLuaBindings()


} // END namespace SDOM