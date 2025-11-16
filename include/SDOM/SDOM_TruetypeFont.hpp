// SDOM_TruetypeFont.hpp
#pragma once

#include <SDL3/SDL.h>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_TTFAsset.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
 
namespace SDOM
{
    class TruetypeFont : public IFontObject
    {
        using SUPER = IFontObject;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "truetype";

        // --- Construction & Initialization --- //
        struct InitStruct : public IFontObject::InitStruct
        {
            InitStruct()
                : IFontObject::InitStruct()
            {
                name = TypeName;
                type = TypeName;
                filename = TypeName;   // default asset lookup name (usually overridden)
                font_size = 12;        // a more sensible TTF default
            }

            // Convert from JSON:
            static InitStruct from_json(const nlohmann::json& j)
            {
                InitStruct i;

                if (j.contains("name"))        i.name = j["name"];
                if (j.contains("type"))        i.type = j["type"];
                if (j.contains("filename"))    i.filename = j["filename"];
                if (j.contains("is_internal"))  i.is_internal = j["is_internal"];

                if (j.contains("font_size"))    i.font_size = j["font_size"];

                return i;
            }
        };

    protected:
        TruetypeFont(const InitStruct& init);

    public:
        // --- Static Factory Methods (JSON + InitStruct) --- //
        static std::unique_ptr<IAssetObject> CreateFromJson(const nlohmann::json& j)
        {
            using IS = TruetypeFont::InitStruct;
            return std::unique_ptr<IAssetObject>(new TruetypeFont(IS::from_json(j)));
        }

        static std::unique_ptr<IAssetObject> CreateFromInitStruct(const IAssetObject::InitStruct& baseInit)
        {
            const auto& tfInit = static_cast<const TruetypeFont::InitStruct&>(baseInit);
            return std::unique_ptr<IAssetObject>(new TruetypeFont(tfInit));
        }

        ~TruetypeFont() override;

        // --- Override methods from IFontObject --- //
        bool onInit() override;
        void onQuit() override;

        void onLoad() override;
        void onUnload() override;
        void create(const sol::table& config) override;

        void drawGlyph(Uint32 ch, int x, int y, const FontStyle& style) override;
        void drawPhrase(const std::string& str, int x, int y, const FontStyle& style) override;
        void drawPhraseOutline(const std::string& str, int x, int y, const FontStyle& style) override;
        void drawPhraseDropshadow(const std::string& str, int x, int y, const FontStyle& style) override;

        bool getGlyphMetrics(Uint32 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance) const override;
        int getGlyphHeight(Uint32 ch) const override;
        int getGlyphWidth(Uint32 ch) const override;

        int getFontAscent() override;         
        int getFontSize() override;    // TTF_GetFontSize(TTF_Font *font);   
        void setFontSize(int p_size) override;
        void setFontStyle(const FontStyle& style) override;
        FontStyle getFontStyle() override;

        // // --- Public TruetypeFont-specific methods --- //
        AssetHandle getResourceHandle() const { return ttf_font_handle_; }

        // int getBitmapFontWidth() const { return bitmapFontWidth_; }
        // int getBitmapFontHeight() const { return bitmapFontHeight_; }
        // void setBitmapFontWidth(int width) { bitmapFontWidth_ = width; }
        // void setBitmapFontHeight(int height) { bitmapFontHeight_ = height; }

    protected:
        // Bitmap-specific data members
        AssetHandle ttf_font_handle_;

        TTF_Font* _getValidTTFFontPtr() const;

        int last_size_ = 0; // last set font size (points)
        
        // Active per-style overrides (set via setFontStyle) - do NOT replace
        // the canonical sprite metrics (bitmapFontWidth_/bitmapFontHeight_).
        // These are temporary overrides used during measurement/render passes.
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
        
        
    }; // END: class TruetypeFont

} // END: namespace SDOM