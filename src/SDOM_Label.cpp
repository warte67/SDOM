// SDOM_Label.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
// #include <SDOM/SDOM_TruetypeFont.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_AssetObject.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_Utils.hpp>


namespace SDOM
{
    Label::Label(const InitStruct& init) : IDisplayObject(init)
    {
        text_ = init.text;
        resourceName_ = init.resourceName;
        fontType_ = init.fontType;
        fontSize_ = init.fontSize;
        fontWidth_ = init.fontWidth;
        fontHeight_ = init.fontHeight;

        // Normalize width/height -> fallback to fontSize when unspecified/invalid
        if (fontWidth_ <= 0)  fontWidth_  = (fontSize_ > 0 ? fontSize_ : 8);
        if (fontHeight_ <= 0) fontHeight_ = (fontSize_ > 0 ? fontSize_ : 8);

        defaultStyle_.alignment = init.alignment;
        defaultStyle_.foregroundColor = init.foregroundColor;
        defaultStyle_.backgroundColor = init.backgroundColor;
        defaultStyle_.borderColor = init.borderColor;
        defaultStyle_.outlineColor = init.outlineColor;
        defaultStyle_.dropshadowColor = init.dropshadowColor;
        defaultStyle_.bold = init.bold;
        defaultStyle_.italic = init.italic;
        defaultStyle_.underline = init.underline;
        defaultStyle_.strikethrough = init.strikethrough;
        defaultStyle_.border = init.border;
        defaultStyle_.background = init.background;
        defaultStyle_.outline = init.outline;
        defaultStyle_.dropshadow = init.dropshadow;
        defaultStyle_.padding_horiz = init.padding_horiz;
        defaultStyle_.padding_vert = init.padding_vert;
        defaultStyle_.dropshadowOffsetX = init.dropshadowOffsetX;
        defaultStyle_.dropshadowOffsetY = init.dropshadowOffsetY;
        defaultStyle_.maxWidth = init.maxWidth;           
        defaultStyle_.maxHeight = init.maxHeight;          
        defaultStyle_.borderThickness = init.borderThickness;    
        defaultStyle_.outlineThickness = init.outlineThickness;   
        defaultStyle_.padding_horiz = init.padding_horiz;      
        defaultStyle_.padding_vert = init.padding_vert;       
        defaultStyle_.dropshadowOffsetX = init.dropshadowOffsetX;  
        defaultStyle_.dropshadowOffsetY = init.dropshadowOffsetY;  
            
        // runtime/init flags
        setDirty(true); // layout needs building
        lastTokenizedText_.clear();

        // Resolve font asset by name (store handle only; do not force loading)
        if (!resourceName_.empty())
        {
            AssetObject asset = getFactory().getAssetObject(resourceName_);
            if (asset.isValid())
            {
                fontAsset = asset; // keep handle; resolve concrete when needed
                // light sanity check only (no onLoad or heavy calls)
                if (!fontAsset.as<IFontObject>())
                {
                    ERROR("Label '" + name_ + "' resource '" + resourceName_ + "' is not a valid IFontObject.");
                    fontAsset.reset();
                }
            }
            else
            {
                DEBUG_LOG("Label '" + name_ + "' could not find font resource: " + resourceName_);
            }
        }
        else
        {
            DEBUG_LOG("Label '" + name_ + "' has no font resource specified.");
        }
    } // END Label::Label(const InitStruct& init)


    Label::Label(const sol::table& config) : IDisplayObject(config)
    {
        auto get_str = [&](const char* k, const std::string& d = "") -> std::string 
        {
            return config[k].valid() ? config[k].get<std::string>() : d;
        };
        auto get_int = [&](const char* k, int d = 0) -> int 
        {
            return config[k].valid() ? config[k].get<int>() : d;
        };
        auto get_bool = [&](const char* k, bool d = false) -> bool 
        {
            return config[k].valid() ? config[k].get<bool>() : d;
        };
        auto read_color = [&](const char* k, SDL_Color d = {255,255,255,255}) -> SDL_Color 
        {
            if (!config[k].valid()) return d;
            sol::table t = config[k];
            SDL_Color c = d;
            if (t["r"].valid()) c.r = (Uint8)t["r"].get<int>();
            if (t["g"].valid()) c.g = (Uint8)t["g"].get<int>();
            if (t["b"].valid()) c.b = (Uint8)t["b"].get<int>();
            if (t["a"].valid()) c.a = (Uint8)t["a"].get<int>();
            // array-style [r,g,b,a]
            if (!t["r"].valid() && t[1].valid()) 
            {
                c.r = (Uint8)t[1].get<int>();
                if (t[2].valid()) c.g = (Uint8)t[2].get<int>();
                if (t[3].valid()) c.b = (Uint8)t[3].get<int>();
                if (t[4].valid()) c.a = (Uint8)t[4].get<int>();
            }
            return c;
        };

        // Basic fields
        text_ = get_str("text", "");
        resourceName_ = get_str("resourceName", get_str("font", "default_bmp_8x8") );
        fontType_ = IFontObject::StringToFontType.at(get_str("fontType", "bitmap"));
        fontSize_ = get_int("fontSize", 10);
        fontWidth_ = get_int("fontWidth", fontSize_);
        fontHeight_ = get_int("fontHeight", fontSize_);

        // Normalize width/height -> fallback to fontSize when unspecified/invalid
        if (fontWidth_ <= 0)  fontWidth_  = (fontSize_ > 0 ? fontSize_ : 8);
        if (fontHeight_ <= 0) fontHeight_ = (fontSize_ > 0 ? fontSize_ : 8);

        // Style defaults
        defaultStyle_.alignment = stringToLabelAlign_.at(normalizeAnchorString(get_str("alignment", "top_left")));
        defaultStyle_.foregroundColor = read_color("foregroundColor", defaultStyle_.foregroundColor);
        defaultStyle_.backgroundColor = read_color("backgroundColor", defaultStyle_.backgroundColor);
        defaultStyle_.borderColor = read_color("borderColor", defaultStyle_.borderColor);
        defaultStyle_.outlineColor = read_color("outlineColor", defaultStyle_.outlineColor);
        defaultStyle_.dropshadowColor = read_color("dropshadowColor", defaultStyle_.dropshadowColor);

        defaultStyle_.bold = get_bool("bold", defaultStyle_.bold);
        defaultStyle_.italic = get_bool("italic", defaultStyle_.italic);
        defaultStyle_.underline = get_bool("underline", defaultStyle_.underline);
        defaultStyle_.strikethrough = get_bool("strikethrough", defaultStyle_.strikethrough);
        defaultStyle_.border = get_bool("border", defaultStyle_.border);
        defaultStyle_.background = get_bool("background", defaultStyle_.background);
        defaultStyle_.outline = get_bool("outline", defaultStyle_.outline);
        defaultStyle_.dropshadow = get_bool("dropshadow", defaultStyle_.dropshadow);

        defaultStyle_.padding_horiz = get_int("padding_horiz", defaultStyle_.padding_horiz);
        defaultStyle_.padding_vert = get_int("padding_vert", defaultStyle_.padding_vert);
        defaultStyle_.dropshadowOffsetX = get_int("dropshadowOffsetX", defaultStyle_.dropshadowOffsetX);
        defaultStyle_.dropshadowOffsetY = get_int("dropshadowOffsetY", defaultStyle_.dropshadowOffsetY);
        defaultStyle_.maxWidth = get_int("maxWidth", defaultStyle_.maxWidth);
        defaultStyle_.maxHeight = get_int("maxHeight", defaultStyle_.maxHeight);
        defaultStyle_.borderThickness = get_int("borderThickness", defaultStyle_.borderThickness);
        defaultStyle_.outlineThickness = get_int("outlineThickness", defaultStyle_.outlineThickness);

        // runtime/init flags
        setDirty(true); // layout needs building
        lastTokenizedText_.clear();

        // Resolve font asset by name (store handle only; do not force loading)
        if (!resourceName_.empty())
        {
            AssetObject asset = getFactory().getAssetObject(resourceName_);
            if (asset.isValid())
            {
                fontAsset = asset; // keep handle; resolve concrete when needed
                // light sanity check only (no onLoad or heavy calls)
                if (!fontAsset.as<IFontObject>())
                {
                    ERROR("Label '" + name_ + "' resource '" + resourceName_ + "' is not a valid IFontObject.");
                    fontAsset.reset();
                }
            }
            else
            {
                DEBUG_LOG("Label '" + name_ + "' could not find font resource: " + resourceName_);
            }
        }
        else
        {
            DEBUG_LOG("Label '" + name_ + "' has no font resource specified.");
        }
    } // END Label::Label(const sol::table& config)

    Label::~Label()
    {
        // Cleanup if necessary
        // (no dynamic resources to free in current implementation)
    }

    bool Label::onInit()
    {
        static thread_local bool in_oninit = false;
        if (in_oninit) 
        {
            ERROR("Label::onInit: re-entrant call for: " + name_);
            return false;
        }
        in_oninit = true;

        if (resourceName_.empty()) 
        {
            ERROR("Label::onInit() --> No font resource specified.");
            in_oninit = false;
            return false;
        }

        // Resolve asset handle (do not create new assets here)
        AssetObject asset = getFactory().getAssetObject(resourceName_);
        if (!asset.isValid()) 
        {
            ERROR("Label::onInit() --> Resource '" + resourceName_ + "' was not found.");
            in_oninit = false;
            return false;
        }

        // Must be an IFontObject (BitmapFont or TruetypeFont)
        IFontObject* font = asset.as<IFontObject>();
        if (!font) 
        {
            ERROR("Label::onInit() --> Resource '" + resourceName_ + "' is not an IFontObject.");
            in_oninit = false;
            return false;
        }

        // store the handle for later use
        fontAsset = asset;

        // Ensure the font is loaded (guarded by the font implementation)
        if (!font->isLoaded()) 
        {
            font->onLoad();
            if (!font->isLoaded()) 
            {
                ERROR("Label::onInit() --> Failed to load font resource: " + resourceName_);
                in_oninit = false;
                return false;
            }
        }

        in_oninit = false;
        return true;
    } // END Label::onInit()


    void Label::onQuit() 
    {
        // Clear transient layout/token state. Do not force-unload shared font assets
        // (Factory/AssetObject owns their lifecycle).
        tokenList.clear();
        tokenAlignLists_.clear();
        phraseAlignLists_.clear();
        lastTokenizedText_.clear();

        // Drop our handle to the font (release reference only).
        fontAsset.reset();

        // Mark dirty so a rebuilt layout will occur if the object is re-used.
        setDirty(true);

        // Let base class perform any additional cleanup.
        SUPER::onQuit();
    } // END Label::onQuit()
    

    void Label::onUpdate(float fElapsedTime) 
    {
        (void)fElapsedTime; // Unused
    } // END Label::onUpdate(float fElapsedTime)

    void Label::onEvent(const Event& event) 
    {
        (void)event; // Unused
    } // END Label::onEvent(const Event& event)

    void Label::onRender() 
    {

    } // END Label::onRender()

    bool Label::onUnitTest() 
    {
        return true;
    } // END Label::onUnitTest()
    
    void Label::setText(std::string p_text)
    {
        if (lastTokenizedText_ != p_text)
        {
            text_ = p_text;
            lastTokenizedText_ = text_;
            tokenizeText();     // Text has changed, retokenize.
            setDirty();
        }
    } // END Label::setText(std::string p_text)

    bool Label::isPunctuation(char c)
    {
        return false;
    } // END Label::isPunctuation(char c)

    int Label::tokenizeText()
    {
        return 0;
    } // END Label::tokenizeText()

    void Label::renderLabel()
    {

    } // END Label::renderLabel()
    
    void Label::renderLabelPass(RenderPass pass)
    {

    } // END Label::renderLabelPass(RenderPass pass)

    // --- Helper to build alignment lists from tokenList --- //
    void Label::_buildTokenAlignLists()
    {

    } // END Label::_buildTokenAlignLists()

    void Label::_debugToken(const LabelToken& token)
    {

    } // END Label::_debugToken(const LabelToken& token)

    void Label::_maxSize(float& width, float& height)
    {

    } // END Label::_maxSize(float& width, float& height)

    void Label::_buildPhraseAlignLists()
    {

    } // END Label::_buildPhraseAlignLists()

    // --- Lua Registration --- //

    void Label::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Label:" + getName();
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

        // small helper to validate and cast the AssetObject -> Label*
        auto cast_label_from_asset = [](const AssetObject& asset) -> Label* {
            if (!asset.isValid()) { ERROR("invalid AssetObject provided to Label method"); }
            IAssetObject* base = asset.get();
            Label* label = dynamic_cast<Label*>(base);
            if (!label) { ERROR("invalid Label object"); }
            return label;
        };

        // // Register Label-specific properties and commands here (bridge from AssetObject handle)
        // reg("setLabelWidth", [cast_label_from_asset](AssetObject asset, int w) { cast_label_from_asset(asset)->setLabelWidth(w); });
        // reg("setLabelHeight", [cast_label_from_asset](AssetObject asset, int h) { cast_label_from_asset(asset)->setLabelHeight(h); });
        // ...



    } // END Label::_registerLuaBindings(const std::string& typeName, sol::state_view lua)

} // END namespace SDOM