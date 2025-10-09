// SDOM_BitmapFont.hpp
#pragma once

#include <SDL3/SDL.h>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_AssetObject.hpp>
 
namespace SDOM
{
    class SpriteSheet;

    class BitmapFont : public IFontObject
    {
            using SUPER = IFontObject;

        public:
            // --- Type Info --- //
            static constexpr const char* TypeName = "BitmapFont";

            // --- Construction & Initialization --- //
            struct InitStruct : public IFontObject::InitStruct
            {
                InitStruct() : IFontObject::InitStruct() 
                { 
                    name = TypeName; 
                    type = TypeName;
                    filename = TypeName; // Default filename, can be overridden
                }
                int fontSize_ = 8; // Font size property for TrueType fonts (and BitmapFont scaling)
            };

        protected:
            BitmapFont(const InitStruct& init);
            BitmapFont(const sol::table& config);

        public:
            // --- Static Factory Methods --- //
            static std::unique_ptr<IAssetObject> CreateFromLua(const sol::table& config) {
                return std::unique_ptr<IAssetObject>(new BitmapFont(config));
            }
            static std::unique_ptr<IAssetObject> CreateFromInitStruct(const IAssetObject::InitStruct& baseInit) {
                const auto& fontInit = static_cast<const BitmapFont::InitStruct&>(baseInit);
                return std::unique_ptr<IAssetObject>(new BitmapFont(fontInit));
            }

            virtual ~BitmapFont() override;

            // Override methods from IGlyphObject
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
            virtual void setFontStyle(FontStyle& style) override;
            virtual FontStyle getFontStyle() override;

            // --- Public BitmapFont-specific methods --- //
            AssetObject getSpriteSheet() const { return spriteSheet_; }

            int getBitmapFontWidth() const { return bitmapFontWidth_; }
            int getBitmapFontHeight() const { return bitmapFontHeight_; }
            void setBitmapFontWidth(int width) { bitmapFontWidth_ = width; }
            void setBitmapFontHeight(int height) { bitmapFontHeight_ = height; }

        protected:
            // Bitmap-specific data members
            AssetObject spriteSheet_;
            int bitmapFontWidth_ = 8;
            int bitmapFontHeight_ = 8;

            std::vector<std::vector<SDL_Texture*>> outlineTextures;

            void initializeOutlineGlyph(Uint32 ch, int x, int y);
            void drawForegroundGlyph(Uint32 ch, int x, int y, const FontStyle& style);
            void drawOutlineGlyph(Uint32 ch, int x, int y, const FontStyle& style);
            void drawDropShadowGlyph(Uint32 ch, int x, int y, const FontStyle& style);

            // --- Lua Registration --- //
            virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);
    };
}