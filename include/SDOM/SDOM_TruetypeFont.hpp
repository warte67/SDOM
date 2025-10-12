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
                InitStruct() : IFontObject::InitStruct() 
                { 
                    name = TypeName; 
                    type = TypeName;
                    filename = TypeName; // Default filename, can be overridden
                }
            };

        protected:
            TruetypeFont(const InitStruct& init);
            TruetypeFont(const sol::table& config);

        public:
            // --- Static Factory Methods --- //
            static std::unique_ptr<IAssetObject> CreateFromLua(const sol::table& config) {
                return std::unique_ptr<IAssetObject>(new TruetypeFont(config));
            }
            static std::unique_ptr<IAssetObject> CreateFromInitStruct(const IAssetObject::InitStruct& baseInit) {
                const auto& fontInit = static_cast<const TruetypeFont::InitStruct&>(baseInit);
                return std::unique_ptr<IAssetObject>(new TruetypeFont(fontInit));
            }

            virtual ~TruetypeFont() override;

            // --- Override methods from IFontObject --- //
            virtual bool onInit() override;
            virtual void onQuit() override;

            virtual void onLoad() override;
            virtual void onUnload() override;
            virtual void create(const sol::table& config) override;

            virtual void drawGlyph(Uint32 ch, int x, int y, const FontStyle& style) override;
            virtual void drawPhrase(const std::string& str, int x, int y, const FontStyle& style) override;
            virtual void drawPhraseOutline(const std::string& str, int x, int y, const FontStyle& style) override;
            virtual void drawPhraseDropshadow(const std::string& str, int x, int y, const FontStyle& style) override;

            virtual bool getGlyphMetrics(Uint32 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance) const override;
            virtual int getGlyphHeight(Uint32 ch) const override;
            virtual int getGlyphWidth(Uint32 ch) const override;

            virtual int getFontAscent() override;
            virtual void setFontSize(int p_size) override;
            virtual void setFontStyle(const FontStyle& style) override;
            virtual FontStyle getFontStyle() override;

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
            
            // Active per-style overrides (set via setFontStyle) - do NOT replace
            // the canonical sprite metrics (bitmapFontWidth_/bitmapFontHeight_).
            // These are temporary overrides used during measurement/render passes.
            int activeFontWidth_ = -1;   
            int activeFontHeight_ = -1;  

            void initializeOutlineGlyph(Uint32 ch, int x, int y);
            void drawForegroundGlyph(Uint32 ch, int x, int y, const FontStyle& style);
            void drawOutlineGlyph(Uint32 ch, int x, int y, const FontStyle& style);
            void drawDropShadowGlyph(Uint32 ch, int x, int y, const FontStyle& style);

            // --- Lua Registration --- //
            virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END: class TruetypeFont

} // END: namespace SDOM