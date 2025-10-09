// SDOM_BitmapFont.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_AssetObject.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>

namespace SDOM
{
    BitmapFont::BitmapFont(const InitStruct& init) : IFontObject(init)
    {
    }

    BitmapFont::BitmapFont(const sol::table& config) : IFontObject(config)
    {
    }


    BitmapFont::~BitmapFont()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "~BitmapFont()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
    }


    bool BitmapFont::onInit()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onInit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Initialization logic for BitmapFont
        return true;
    }

    void BitmapFont::onQuit()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onQuit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Cleanup logic for BitmapFont
    }

    void BitmapFont::onLoad()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onLoad()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Loading logic for BitmapFont
    } // END onLoad()

    void BitmapFont::onUnload()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onUnload()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Unloading logic for BitmapFont
    }

    void BitmapFont::create(const sol::table& config)
    {
        // Create or reconfigure the BitmapFont based on the provided Lua table
    }

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