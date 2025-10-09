// SDOM_Label.hpp
#pragma once

#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IFontObject.hpp>

namespace SDOM
{
    class Label : public IDisplayObject
    {
        using SUPER = IDisplayObject;
    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "Label";

        // --- Initialization Struct --- //
        struct InitStruct : public IDisplayObject::InitStruct
        {
            InitStruct() : IDisplayObject::InitStruct() 
            { 
                // --- Default InitStruct values from IDisplayObject: --- //
                name = TypeName; 
                type = TypeName;
                x = 0.0f;
                y = 0.0f;
                width = 100.0f;   // default width
                height = 30.0f;   // default height
                color = {255, 0, 255, 255}; // magenta text
                anchorTop = AnchorPoint::TOP_LEFT;
                anchorLeft = AnchorPoint::TOP_LEFT;
                anchorBottom = AnchorPoint::TOP_LEFT;
                anchorRight = AnchorPoint::TOP_LEFT;
                z_order = 0;
                priority = 0;
                isClickable = false; // Labels are not clickable by default
                isEnabled = true;
                isHidden = false;
                tabPriority = 0;
                tabEnabled = false; // Labels are not tabbable by default
            }
            // --- Label-specific properties --- //
            std::string text = "Label";
            std::string resourceName = "default_bmp_font_8x8"; // default font
            IFontObject::FontType fontType = IFontObject::FontType::Bitmap;
            int fontSize = 10;          // for TrueType fonts, or BitmapFont scaling
            int fontWidth = 8;          // for BitmapFont only (-1 to use fontSize)
            int fontHeight = 8;         // for BitmapFont only (-1 to use fontSize)
            LabelAlign alignment = LabelAlign::TOP_LEFT;        // Text alignment (default: top-left)
            SDL_Color foregroundColor = {255, 255, 255, 255};   // white
            SDL_Color backgroundColor = {0, 0, 0, 0};           // transparent
            SDL_Color borderColor = {0, 0, 0, 0};               // transparent
            SDL_Color outlineColor = {0, 0, 0, 255};            // black
            SDL_Color dropshadowColor = {0, 0, 0, 128};         // semi-transparent black
            bool bold = false;          // enable bold text
            bool italic = false;        // enable italic text
            bool underline = false;     // enable underline text
            bool strikethrough = false; // enable strikethrough text
            bool border = false;        // enable border
            bool background = false;    // enable background
            bool outline = false;       // enable outline
            bool dropshadow = false;    // enable drop shadow
            bool wordwrap = false;      // enable word wrap
            int maxWidth = 0;           // maximum allowable width for auto resizing (0 disables auto width)
            int maxHeight = 0;          // maximum allowable height for auto resizing (0 disables auto height)
            int borderThickness = 1;    // Border thickness (default: 0)
            int outlineThickness = 1;   // Outline thickness (default: 0)
            int padding_horiz = 0;      // Horizontal padding (left/right)
            int padding_vert = 0;       // Vertical padding (top/bottom)
            int dropshadowOffsetX = 1;  // Drop shadow offset (horizontal)
            int dropshadowOffsetY = 1;  // Drop shadow offset (vertical)
        };

        // -- NOTE:  Be sure to utilize the utility functions in SDOM_Utils.hpp for color parsing/handling -- //           
        // std::string normalizeAnchorString(const std::string& s);         // helper function to normalize anchor point strings
        // bool validateAnchorPointString(const std::string& anchorString); // helper function to validate anchor point strings            
        // bool validateAnchorAssignments(const sol::table& config);        // helper function to validate anchor point assignments in a config table

        inline static const std::unordered_map<LabelAlign, std::string> labelAlignToString_ = {
            { LabelAlign::DEFAULT,      "default" },
            { LabelAlign::LEFT,         "left" },
            { LabelAlign::CENTER,       "center" },
            { LabelAlign::RIGHT,        "right" },
            { LabelAlign::TOP,          "top" },
            { LabelAlign::TOP_LEFT,     "top_left" },
            { LabelAlign::TOP_CENTER,   "top_center" },
            { LabelAlign::TOP_RIGHT,    "top_right" },
            { LabelAlign::MIDDLE,       "middle" },
            { LabelAlign::MIDDLE_LEFT,  "middle_left" },
            { LabelAlign::MIDDLE_CENTER,"middle_center" },
            { LabelAlign::MIDDLE_RIGHT, "middle_right" },
            { LabelAlign::BOTTOM,       "bottom" },
            { LabelAlign::BOTTOM_LEFT,  "bottom_left" },
            { LabelAlign::BOTTOM_CENTER,"bottom_center" },
            { LabelAlign::BOTTOM_RIGHT, "bottom_right" }
        };

        inline static const std::unordered_map<std::string, LabelAlign> stringToLabelAlign_ = {
            { "default",        LabelAlign::DEFAULT },
            { "left",           LabelAlign::LEFT },
            { "center",         LabelAlign::CENTER },
            { "right",          LabelAlign::RIGHT },
            { "top",            LabelAlign::TOP },
            { "top_left",       LabelAlign::TOP_LEFT },
            { "top_center",     LabelAlign::TOP_CENTER },
            { "top_right",      LabelAlign::TOP_RIGHT },
            { "middle",         LabelAlign::MIDDLE },
            { "middle_left",    LabelAlign::MIDDLE_LEFT },
            { "middle_center",  LabelAlign::MIDDLE_CENTER },
            { "middle_right",   LabelAlign::MIDDLE_RIGHT },
            { "bottom",         LabelAlign::BOTTOM },
            { "bottom_left",    LabelAlign::BOTTOM_LEFT },
            { "bottom_center",  LabelAlign::BOTTOM_CENTER },
            { "bottom_right",   LabelAlign::BOTTOM_RIGHT },
        };

        enum class TokenType { Word, Escape, Space, Punctuation, Newline, Tab };
        struct LabelToken
        {
            TokenType type;
            std::string text;
            FontStyle style;
        };  

        // PhraseToken: a run of tokens with identical style/alignment on the same line
        struct PhraseToken {
            std::string text;      // Concatenated text of the phrase
            FontStyle style;       // Style for the entire phrase
            int lineIndex;         // Line number within the label
            float startX;          // X position for rendering
            float lineY;           // Y position for rendering
            float width = 0.0f;    // Width of the phrase (in pixels)
            float height = 0.0f;   // Height of the phrase (in pixels)
        };            

        enum class RenderPass { Dropshadow, Outline, Foreground };
    
    protected:
        // --- Constructors --- //
        Label(const InitStruct& init);
        Label(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new Label(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const Label::InitStruct& baseInit) {
            const auto& labelInit = static_cast<const Label::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Label(labelInit));
        }

        Label() = default;
        virtual ~Label() override;

        // virtual methods
        virtual bool onInit() override; 
        virtual void onQuit() override;
        virtual void onUpdate(float fElapsedTime) override;
        virtual void onEvent(const Event& event) override;
        virtual void onRender() override;
        virtual bool onUnitTest() override;

        void setText(std::string p_text);
        std::string getText() const { return text_; }
        AssetHandle getFont() const { return fontAsset; }
        IFontObject::FontType getFontType() const { return fontType_; }
        
        FontStyle& getDefaultStyle() { return defaultStyle_; }
        void setDefaultStyle(const FontStyle& style) { defaultStyle_ = style; setDirty(); }

        bool isPunctuation(char c);
        int tokenizeText();
        void renderLabel();
        void renderLabelPass(RenderPass pass);

    protected:

        friend Factory;
        friend Core;

        std::string text_;
        std::string lastTokenizedText_ = ""; // To track changes for re-tokenization
        int fontSize_;              // truetype fonts
        int fontWidth_;             // bitmap font width
        int fontHeight_;            // bitmap font height
        IFontObject::FontType fontType_ = IFontObject::FontType::Bitmap;
        AssetHandle fontAsset;      // Underlying font asset for the Label
        FontStyle defaultStyle_;    // default style for the label
        std::string resourceName_;  // Optional resource name for preloaded font

        // first pass token list
        std::vector<LabelToken> tokenList;

        // second pass token alignment lists
        enum class AlignQueue : uint8_t 
        {
            TOP_LEFT        = 0b0000,
            TOP_CENTER      = 0b0001,
            TOP_RIGHT       = 0b0010,
            MIDDLE_LEFT     = 0b0011,
            MIDDLE_CENTER   = 0b0100,
            MIDDLE_RIGHT    = 0b0101,
            BOTTOM_LEFT     = 0b0110,
            BOTTOM_CENTER   = 0b0111,
            BOTTOM_RIGHT    = 0b1000,
            DEFAULT         = 0b0000, // default to top-left
            LEFT            = 0b0011, // default to middle-left
            CENTER          = 0b0100, // default to middle-center
            RIGHT           = 0b0101, // default to middle-right
            TOP             = 0b0001, // default to top-center
            MIDDLE          = 0b0100, // default to middle-center
            BOTTOM          = 0b0111  // default to bottom-center
        };
        std::unordered_map<LabelAlign, AlignQueue> alignXRef_ =
        {
            { LabelAlign::DEFAULT       ,  AlignQueue::DEFAULT      },
            { LabelAlign::LEFT          ,  AlignQueue::LEFT         }, 
            { LabelAlign::CENTER        ,  AlignQueue::CENTER       }, 
            { LabelAlign::RIGHT         ,  AlignQueue::RIGHT        }, 
            { LabelAlign::TOP           ,  AlignQueue::TOP          }, 
            { LabelAlign::TOP_LEFT      ,  AlignQueue::TOP_LEFT     }, 
            { LabelAlign::TOP_CENTER    ,  AlignQueue::TOP_CENTER   }, 
            { LabelAlign::TOP_RIGHT     ,  AlignQueue::TOP_RIGHT    }, 
            { LabelAlign::MIDDLE        ,  AlignQueue::MIDDLE       }, 
            { LabelAlign::MIDDLE_LEFT   ,  AlignQueue::MIDDLE_LEFT  }, 
            { LabelAlign::MIDDLE_CENTER ,  AlignQueue::MIDDLE_CENTER}, 
            { LabelAlign::MIDDLE_RIGHT  ,  AlignQueue::MIDDLE_RIGHT }, 
            { LabelAlign::BOTTOM        ,  AlignQueue::BOTTOM       }, 
            { LabelAlign::BOTTOM_LEFT   ,  AlignQueue::BOTTOM_LEFT  }, 
            { LabelAlign::BOTTOM_CENTER ,  AlignQueue::BOTTOM_CENTER}, 
            { LabelAlign::BOTTOM_RIGHT  ,  AlignQueue::BOTTOM_RIGHT }
        };
        std::map<AlignQueue, std::vector<LabelToken>> tokenAlignLists_;

        // final pass phrase alignment lists
        std::map<AlignQueue, std::vector<PhraseToken>> phraseAlignLists_;

        // Helper to build alignment lists from tokenList
        void _buildTokenAlignLists();               // Helper to build alignment lists from tokenList
        void _debugToken(const LabelToken& token);  // Helper to print token info for debugging
        void _maxSize(float& width, float& height); // Helper to calculate max size needed
        void _buildPhraseAlignLists();              // Helper to build phrase groups from tokenAlignLists_


        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);


    };

} // namespace SDOM

