/**
 * @class SDOM::<ClassName>
 * @brief [Short one-line summary of class purpose]
 * @author Jay Faries (https://github.com/warte67)
 * @inherits SDOM::<BaseClass>
 * @luaType <LuaTypeName>
 *
 * [Expanded description of the class, its purpose, and typical use cases.]
 *
 * ---
 * @section <ClassName>_Properties Properties
 * | Property | Type | Description |
 * |-----------|------|-------------|
 * | `property_name` | `<type>` | [Brief description] |
 *
 * ---
 * @section <ClassName>_Functions Functions
 * | Function | Returns | Description |
 * |-----------|----------|-------------|
 * | `functionName()` | `<type>` | [What it does] |
 *
 * ---
 * @section <ClassName>_Events Events
 * | Event | Description |
 * |--------|-------------|
 * | `EventName` | [When it’s fired / purpose] |
 *
 * ---
 * @section <ClassName>_Notes Notes
 * - [Any caveats, performance notes, or Lua binding details]
 */

/**
 * @section License ZLIB License
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */


#pragma once
#include <SDL3/SDL.h>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>

namespace SDOM
{
    class SpriteSheet;

    /**
     * @class SDOM::BitmapFont
     * @brief Fixed-size bitmap-based font using sprite sheets for glyph rendering.
     *
     * The BitmapFont class loads and manages a pre-rendered sheet of glyphs
     * and provides efficient text drawing operations without the overhead
     * of dynamic font rasterization. Ideal for UI text, retro fonts, and
     * embedded systems.
     */
    class BitmapFont : public IFontObject
    {
        using SUPER = IFontObject;

    public:
        static constexpr const char* TypeName = "BitmapFont";

        // ---------------------------------------------------------------------
        // 🧱 Construction & Initialization
        // ---------------------------------------------------------------------
        struct InitStruct : public IFontObject::InitStruct
        {
            InitStruct()
                : IFontObject::InitStruct()
            {
                name = TypeName;
                type = TypeName;
                filename = TypeName; // default filename unless overridden
            }

            // BitmapFont-specific parameters
            int fontSize = 8;        // Uniform scaling, BitmapFont-compatible
            int font_width = -1;     // Optional override; if negative → use atlas metrics
            int font_height = -1;    // Optional override; if negative → use atlas metrics

            // JSON hydration
            static InitStruct from_json(const nlohmann::json& j)
            {
                InitStruct i;

                // --- inherited IAssetObject / IFontObject settings ---
                if (j.contains("name"))       i.name = j["name"].get<std::string>();
                if (j.contains("type"))       i.type = j["type"].get<std::string>();
                if (j.contains("filename"))   i.filename = j["filename"].get<std::string>();
                if (j.contains("isInternal")) i.is_internal = j["isInternal"].get<bool>();

                // inherited: TrueType/Bitmap-compatible uniform scaling
                if (j.contains("fontSize"))
                    i.fontSize = j["fontSize"].get<int>();

                // inherited IFontObject base (fontSize_) — keep synced
                i.font_size = i.fontSize;

                // --- BitmapFont-specific overrides ---
                if (j.contains("font_width"))
                    i.font_width = j["font_width"].get<int>();

                if (j.contains("font_height"))
                    i.font_height = j["font_height"].get<int>();

                return i;
            }
        };

    protected:
        BitmapFont(const InitStruct& init);

    public:
        // ---------------------------------------------------------------------
        // 🏗️ Static Factory Methods
        // ---------------------------------------------------------------------
        static std::unique_ptr<IAssetObject> CreateFromInitStruct(
            const IAssetObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const BitmapFont::InitStruct&>(baseInit);
            return std::unique_ptr<IAssetObject>(new BitmapFont(init));
        }

        static std::unique_ptr<IAssetObject> CreateFromJson(const nlohmann::json& j)
        {
            auto init = BitmapFont::InitStruct::from_json(j);
            return std::unique_ptr<IAssetObject>(new BitmapFont(init));
        }

        ~BitmapFont() override;

        // ---------------------------------------------------------------------
        // 🔤 Font Rendering Lifecycle (overrides in IFontObject)
        // ---------------------------------------------------------------------
        bool onInit() override;
        void onQuit() override;
        void onLoad() override;
        void onUnload() override;
        void create(const sol::table& config) override;

        // Rendering and metrics (bound in IFontObject)
        void drawGlyph(Uint32 ch, int x, int y, const FontStyle& style) override;
        void drawPhrase(const std::string& str, int x, int y, const FontStyle& style) override;
        void drawPhraseOutline(const std::string& str, int x, int y, const FontStyle& style) override;
        void drawPhraseDropshadow(const std::string& str, int x, int y, const FontStyle& style) override;
        bool getGlyphMetrics(Uint32 ch, int* minx, int* maxx, int* miny, int* maxy, int* advance) const override;
        int getFontSize() override;
        int getGlyphHeight(Uint32 ch) const override;
        int getGlyphWidth(Uint32 ch) const override;
        int getFontAscent() override;
        void setFontSize(int p_size) override;
        void setFontStyle(const FontStyle& style) override;
        FontStyle getFontStyle() override;

        // ---------------------------------------------------------------------
        // 💡 BitmapFont-Specific Accessors (Lua-Visible)
        // ---------------------------------------------------------------------
        /** @return The underlying SpriteSheet resource used by this font. */
        AssetHandle getResourceHandle() const { return spriteSheet_; }

        /** @return The configured glyph width in pixels. */
        int getBitmapFontWidth() const { return bitmapFontWidth_; }

        /** @return The configured glyph height in pixels. */
        int getBitmapFontHeight() const { return bitmapFontHeight_; }


    protected:
        AssetHandle spriteSheet_;  ///< Linked SpriteSheet asset
        int bitmapFontWidth_ = -1;
        int bitmapFontHeight_ = -1;

        std::vector<std::vector<SDL_Texture*>> outlineTextures;
        int activeFontWidth_ = -1;
        int activeFontHeight_ = -1;

        void initializeOutlineGlyph(Uint32 ch, int x, int y);
        void drawForegroundGlyph(Uint32 ch, int x, int y, const FontStyle& style);
        void drawOutlineGlyph(Uint32 ch, int x, int y, const FontStyle& style);
        void drawDropShadowGlyph(Uint32 ch, int x, int y, const FontStyle& style);

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;                
    };
} // namespace SDOM
