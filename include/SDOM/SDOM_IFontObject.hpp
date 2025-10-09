// SDOM_IFontObject.hpp
#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>
#include <SDOM/SDOM_IAssetObject.hpp>

namespace SDOM
{
    constexpr int maxOutlineThickness = 3; // up to 3, as needed

    // Text alignment options
    enum class LabelAlign : uint8_t 
    {
        DEFAULT        = 0b0000, // default to top-left
        LEFT           = 0b0001, // default to top-left
        CENTER         = 0b0010, // default to top-center
        RIGHT          = 0b0011, // default to top-right
        TOP            = 0b0100, // default to top-left
        TOP_LEFT       = 0b0101, // top-left
        TOP_CENTER     = 0b0110, // top-center
        TOP_RIGHT      = 0b0111, // top-right
        MIDDLE         = 0b1000, // default to middle-left
        MIDDLE_LEFT    = 0b1001, // middle-left
        MIDDLE_CENTER  = 0b1010, // middle-center
        MIDDLE_RIGHT   = 0b1011, // middle-right
        BOTTOM         = 0b1100, // default to bottom-left
        BOTTOM_LEFT    = 0b1101, // bottom-left
        BOTTOM_CENTER  = 0b1110, // bottom-center
        BOTTOM_RIGHT   = 0b1111  // bottom-right
    };

    // Font style structure for rendering options
    struct FontStyle 
    {
        bool bold = false;
        bool italic = false;
        bool underline = false;
        bool strikethrough = false;
        bool border = false;
        bool background = false;
        bool outline = false;
        bool dropshadow = false;

        int fontSize = 10;      // Uniform scaling for both font types
        // int fontWidth = -1;     // Optional: non-uniform width (bitmap only)          
        // int fontHeight = -1;    // Optional: non-uniform height (bitmap only)

        bool wordwrap = false;         // enable word wrap
        int maxWidth = 0;              // maximum allowable width for auto resizing (0 disables auto width)
        int maxHeight = 0;             // maximum allowable height for auto resizing (0 disables auto height)

        int borderThickness = 1;        // Border thickness (default: 0)
        int outlineThickness = 1;       // Outline thickness (default: 0)
        int padding_horiz = 0;          // Horizontal padding (left/right)
        int padding_vert = 0;           // Vertical padding (top/bottom)
        int dropshadowOffsetX = 1;      // Drop shadow offset (horizontal)
        int dropshadowOffsetY = 1;      // Drop shadow offset (vertical)

        LabelAlign alignment = LabelAlign::TOP_LEFT;      // Text alignment (default: top-left)

        SDL_Color foregroundColor = {255, 255, 255, 255}; // Text color (default: white)
        SDL_Color backgroundColor = {0, 0, 0, 0};         // Background color (default: transparent)
        SDL_Color borderColor = {0, 0, 0, 0};             // Border color (default: transparent)
        SDL_Color outlineColor = {0, 0, 0, 255};          // Default: black
        SDL_Color dropshadowColor = {0, 0, 0, 128};       // Default: semi-transparent black
    };

    // FontStyle inequality operator
    inline bool operator!=(const SDOM::FontStyle& a, const SDOM::FontStyle& b) {
        return
            a.bold              != b.bold              ||
            a.italic            != b.italic            ||
            a.underline         != b.underline         ||
            a.strikethrough     != b.strikethrough     ||
            a.border            != b.border            ||
            a.background        != b.background        ||
            a.outline           != b.outline           ||
            a.dropshadow        != b.dropshadow        ||
            a.fontSize          != b.fontSize          ||
            a.wordwrap          != b.wordwrap          ||
            a.maxWidth          != b.maxWidth          ||
            a.maxHeight         != b.maxHeight         ||
            a.borderThickness   != b.borderThickness   ||
            a.outlineThickness  != b.outlineThickness  ||
            a.padding_horiz     != b.padding_horiz     ||
            a.padding_vert      != b.padding_vert      ||
            a.dropshadowOffsetX != b.dropshadowOffsetX ||
            a.dropshadowOffsetY != b.dropshadowOffsetY ||
            a.alignment         != b.alignment         ||
            a.foregroundColor.r != b.foregroundColor.r ||
            a.foregroundColor.g != b.foregroundColor.g ||
            a.foregroundColor.b != b.foregroundColor.b ||
            a.foregroundColor.a != b.foregroundColor.a ||
            a.backgroundColor.r != b.backgroundColor.r ||
            a.backgroundColor.g != b.backgroundColor.g ||
            a.backgroundColor.b != b.backgroundColor.b ||
            a.backgroundColor.a != b.backgroundColor.a ||
            a.borderColor.r     != b.borderColor.r     ||
            a.borderColor.g     != b.borderColor.g     ||
            a.borderColor.b     != b.borderColor.b     ||
            a.borderColor.a     != b.borderColor.a     ||
            a.outlineColor.r    != b.outlineColor.r    ||
            a.outlineColor.g    != b.outlineColor.g    ||
            a.outlineColor.b    != b.outlineColor.b    ||
            a.outlineColor.a    != b.outlineColor.a    ||
            a.dropshadowColor.r != b.dropshadowColor.r ||
            a.dropshadowColor.g != b.dropshadowColor.g ||
            a.dropshadowColor.b != b.dropshadowColor.b ||
            a.dropshadowColor.a != b.dropshadowColor.a;
    }
    // FontStyle equality operator
    inline bool operator==(const SDOM::FontStyle& a, const SDOM::FontStyle& b) {
        return
            a.bold              == b.bold              &&
            a.italic            == b.italic            &&
            a.underline         == b.underline         &&
            a.strikethrough     == b.strikethrough     &&
            a.border            == b.border            &&
            a.background        == b.background        &&
            a.outline           == b.outline           &&
            a.dropshadow        == b.dropshadow        &&
            a.fontSize          == b.fontSize          &&
            a.wordwrap          == b.wordwrap          &&
            a.maxWidth          == b.maxWidth          &&
            a.maxHeight         == b.maxHeight         &&
            a.borderThickness   == b.borderThickness   &&
            a.outlineThickness  == b.outlineThickness  &&
            a.padding_horiz     == b.padding_horiz     &&
            a.padding_vert      == b.padding_vert      &&
            a.dropshadowOffsetX == b.dropshadowOffsetX &&
            a.dropshadowOffsetY == b.dropshadowOffsetY &&
            a.alignment         == b.alignment         &&
            a.foregroundColor.r == b.foregroundColor.r &&
            a.foregroundColor.g == b.foregroundColor.g &&
            a.foregroundColor.b == b.foregroundColor.b &&
            a.foregroundColor.a == b.foregroundColor.a &&
            a.backgroundColor.r == b.backgroundColor.r &&
            a.backgroundColor.g == b.backgroundColor.g &&
            a.backgroundColor.b == b.backgroundColor.b &&
            a.backgroundColor.a == b.backgroundColor.a &&
            a.borderColor.r     == b.borderColor.r     &&
            a.borderColor.g     == b.borderColor.g     &&
            a.borderColor.b     == b.borderColor.b     &&
            a.borderColor.a     == b.borderColor.a     &&
            a.outlineColor.r    == b.outlineColor.r    &&
            a.outlineColor.g    == b.outlineColor.g    &&
            a.outlineColor.b    == b.outlineColor.b    &&
            a.outlineColor.a    == b.outlineColor.a    &&
            a.dropshadowColor.r == b.dropshadowColor.r &&
            a.dropshadowColor.g == b.dropshadowColor.g &&
            a.dropshadowColor.b == b.dropshadowColor.b &&
            a.dropshadowColor.a == b.dropshadowColor.a;
    }


    class Factory;
    class Core;

    class IFontObject : public IAssetObject
    {
        using SUPER = IAssetObject;

        public:        
            // --- Type Info --- //
            static constexpr const char* TypeName = "IFontObject";

            enum class FontType { Bitmap, Truetype };
            inline static std::unordered_map<FontType, std::string> FontTypeToString = {
                { FontType::Bitmap,     "bitmap" },
                { FontType::Truetype,   "truetype" }
            };
            inline static std::unordered_map<std::string, FontType> StringToFontType = {
                { "bitmap",     FontType::Bitmap },
                { "truetype",   FontType::Truetype }
            };
            
            // --- Construction & Initialization --- //
            struct InitStruct : public IAssetObject::InitStruct
            {
                InitStruct() : IAssetObject::InitStruct() 
                { 
                    name = TypeName; 
                    type = TypeName;
                    filename = TypeName; // Default filename, can be overridden
                }
                int fontSize_ = 8; // Font size property for TrueType fonts (and BitmapFont scaling)
            };
                    

        protected:
            // --- Constructors --- //
            IFontObject(const InitStruct& init);
            IFontObject(const sol::table& config);

            // // The filename is immutable after initialization
            // explicit IFontObject(const std::string& assetName, int fontSize = 10);

        public:
            // // --- Static Factory Methods --- //
            // static std::unique_ptr<IFontObject> CreateFromLua(const sol::table& config) {
            //     return std::unique_ptr<IFontObject>(new IFontObject(config));
            // }
            // static std::unique_ptr<IFontObject> CreateFromInitStruct(const IAssetObject::InitStruct& baseInit) {
            //     const auto& fontInit = static_cast<const IFontObject::InitStruct&>(baseInit);
            //     return std::unique_ptr<IFontObject>(new IFontObject(fontInit));
            // }
        
            virtual ~IFontObject() override;

            // methods to be overridden from IDataObject
            virtual bool onInit() = 0;
            virtual void onQuit() = 0;

            // ResourceObject virtual methods
            virtual void onLoad() = 0; 
            virtual void onUnload() = 0;
            virtual void create(const sol::table& config) = 0;     
            
            virtual void drawGlyph(Uint32 ch, int x, int y, const FontStyle& style) = 0;
            virtual void drawPhrase(const std::string& str, int x, int y, const FontStyle& style) = 0;
            virtual void drawPhraseOutline(const std::string& str, int x, int y, const FontStyle& style) = 0;
            virtual void drawPhraseDropshadow(const std::string& str, int x, int y, const FontStyle& style) = 0;

            virtual bool getGlyphMetrics(Uint32 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance) const = 0;
            virtual int getGlyphHeight(Uint32 ch) const = 0;
            virtual int getGlyphWidth(Uint32 ch) const = 0;

            virtual int getFontAscent() = 0;  // int TTF_GetFontAscent(const TTF_Font *font)
            virtual void setFontSize(int p_size) = 0;
            virtual void setFontStyle(FontStyle& style) = 0;
            virtual FontStyle getFontStyle() = 0;

            int getWordWidth(const std::string& word) const;    
            int getWordHeight(const std::string& word) const;  
            int getFontSize() const { return fontSize_; }
            FontType getFontType() const { return fontType_; }  

        protected:
            friend Factory;
            friend Core;   

            int fontSize_ = 8; // Font size property for TrueType fonts (and BitmapFont scaling)
            FontType fontType_ = FontType::Bitmap;

            // --- Lua Registration --- //
            virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END class IFontObject

} // END namespace SDOM

