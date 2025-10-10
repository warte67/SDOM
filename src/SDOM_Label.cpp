// SDOM_Label.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
// #include <SDOM/SDOM_TruetypeFont.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
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

        // std::string name = TypeName;
        // std::string type = TypeName;
        // float x = 0.0f;
        // float y = 0.0f;
        // float width = 0.0f;
        // float height = 0.0f;
        // SDL_Color color = {255, 0, 255, 255};
        // AnchorPoint anchorTop = AnchorPoint::TOP_LEFT;
        // AnchorPoint anchorLeft = AnchorPoint::TOP_LEFT;
        // AnchorPoint anchorBottom = AnchorPoint::TOP_LEFT;
        // AnchorPoint anchorRight = AnchorPoint::TOP_LEFT;
        // int z_order = 0;
        // int priority = 0;
        // bool isClickable = true;
        // bool isEnabled = true;
        // bool isHidden = false;
        // int tabPriority = 0;
        // bool tabEnabled = true;

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
        setTabEnabled(init.tabEnabled);
        setClickable(false); // Labels are not clickable by default

        // Resolve font asset by name (store handle only; do not force loading)
        if (!resourceName_.empty())
        {
            AssetHandle asset = getFactory().getAssetObject(resourceName_);
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
        InitStruct init; // to get default values

        // --- Initialization Lambdas --- //
        auto get_str = [&](const char* k, const std::string& d = "") -> std::string 
        {
            return config[k].valid() ? config[k].get<std::string>() : d;
        };
        auto get_int = [&](const char* k, int d = 0) -> int 
        {
            return config[k].valid() ? config[k].get<int>() : d;
        };
        // auto get_float = [&](const char* k, float d = 0.0f) -> float {
        //     if (!config[k].valid()) return d;
        //     sol::object o = config[k];
        //     try {
        //         if (o.is<double>()) return static_cast<float>(o.as<double>());
        //         if (o.is<int>()) return static_cast<float>(o.as<int>());
        //         if (o.is<std::string>()) {
        //             std::string s = o.as<std::string>();
        //             if (s.empty()) return d;
        //             return std::stof(s);
        //         }
        //     } catch(...) {}
        //     return d;
        // };
        auto get_bool = [&](const char* k, bool d = false) -> bool 
        {
            return config[k].valid() ? config[k].get<bool>() : d;
        };
        auto read_color = [&](const char* k, SDL_Color d = {255,0,255,255}) -> SDL_Color 
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

        // --- Initialized Label Fields --- //
        text_ = get_str("text", init.text);
        resourceName_ = get_str("resource_name", get_str("font", init.resourceName) );
        fontType_ = IFontObject::StringToFontType.at(get_str("font_type", IFontObject::FontTypeToString.at(init.fontType)));
        fontSize_ = get_int("font_size", 10);
        fontWidth_ = get_int("font_width", fontSize_);
        fontHeight_ = get_int("font_height", fontSize_);

        // Normalize width/height -> fallback to fontSize when unspecified/invalid
        if (fontWidth_ <= 0)  fontWidth_  = (fontSize_ > 0 ? fontSize_ : 8);
        if (fontHeight_ <= 0) fontHeight_ = (fontSize_ > 0 ? fontSize_ : 8);

        // Style defaults
        defaultStyle_.alignment = stringToLabelAlign_.at(normalizeAnchorString(get_str("alignment", "top_left")));
        defaultStyle_.foregroundColor = read_color("foreground_color", defaultStyle_.foregroundColor);
        defaultStyle_.backgroundColor = read_color("background_color", defaultStyle_.backgroundColor);
        defaultStyle_.borderColor = read_color("border_color", defaultStyle_.borderColor);
        defaultStyle_.outlineColor = read_color("outline_color", defaultStyle_.outlineColor);
        defaultStyle_.dropshadowColor = read_color("dropshadow_color", defaultStyle_.dropshadowColor);

        defaultStyle_.bold = get_bool("bold", defaultStyle_.bold);
        defaultStyle_.italic = get_bool("italic", defaultStyle_.italic);
        defaultStyle_.underline = get_bool("underline", defaultStyle_.underline);
        defaultStyle_.strikethrough = get_bool("strikethrough", defaultStyle_.strikethrough);
        defaultStyle_.border = get_bool("border", defaultStyle_.border);
        defaultStyle_.background = get_bool("background", defaultStyle_.background);
        defaultStyle_.outline = get_bool("outline", defaultStyle_.outline);
        defaultStyle_.dropshadow = get_bool("dropshadow", defaultStyle_.dropshadow);
        defaultStyle_.wordwrap = get_bool("wordwrap", init.wordwrap);

        defaultStyle_.padding_horiz = get_int("padding_horiz", defaultStyle_.padding_horiz);
        defaultStyle_.padding_vert = get_int("padding_vert", defaultStyle_.padding_vert);
        defaultStyle_.dropshadowOffsetX = get_int("dropshadow_offset_x", defaultStyle_.dropshadowOffsetX);
        defaultStyle_.dropshadowOffsetY = get_int("dropshadow_offset_y", defaultStyle_.dropshadowOffsetY);
        defaultStyle_.maxWidth = get_int("max_width", defaultStyle_.maxWidth);
        defaultStyle_.maxHeight = get_int("max_height", defaultStyle_.maxHeight);
        defaultStyle_.borderThickness = get_int("border_thickness", defaultStyle_.borderThickness);
        defaultStyle_.outlineThickness = get_int("outline_thickness", defaultStyle_.outlineThickness);

        // runtime/init flags
        setDirty(true); // layout needs building
        lastTokenizedText_.clear();
        setClickable(get_bool("is_clickable", false)); // Labels are not clickable by default

        // Resolve font asset by name (store handle only; do not force loading)
        if (!resourceName_.empty())
        {
            AssetHandle asset = getFactory().getAssetObject(resourceName_);
            if (asset.isValid())
                fontAsset = asset; // keep handle; resolve concrete when needed
        }
        else
        {
            DEBUG_LOG("Label '" + name_ + "' has no font resource specified.");
        }        

        // // Resolve font asset by name (store handle only; do not force loading)
        // if (!resourceName_.empty())
        // {
        //     AssetHandle asset = getFactory().getAssetObject(resourceName_);
        //     if (asset.isValid())
        //     {
        //         fontAsset = asset; // keep handle; resolve concrete when needed
        //         // light sanity check only (no onLoad or heavy calls).
        //         // Do NOT hard-fail here: some configs may point at a SpriteSheet/Texture
        //         // (which can be used as a font source). Defer strict validation to onInit().
        //         if (fontAsset.as<IFontObject>())
        //         {
        //             // OK: asset implements IFontObject
        //         }
        //         else if (fontAsset.as<SpriteSheet>())
        //         {
        //             // Accept SpriteSheet as a fallback font source for now.
        //             DEBUG_LOG("Label '" + name_ + "' resource '" + resourceName_
        //                       + "' is a SpriteSheet; accepting as font source (deferred validation).");
        //         }
        //         else
        //         {
        //             // Unknown asset type — log and defer final validation/loading to onInit().
        //             DEBUG_LOG("Label '" + name_ + "' resource '" + resourceName_
        //                       + "' is not an IFontObject; deferring validation to onInit().");
        //         }            
        //     }
        //     else
        //     {
        //         DEBUG_LOG("Label '" + name_ + "' could not find font resource: " + resourceName_);
        //     }
        // }
        // else
        // {
        //     DEBUG_LOG("Label '" + name_ + "' has no font resource specified.");
        // }

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
        AssetHandle asset = getFactory().getAssetObject(resourceName_);
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
        // (Factory/AssetHandle owns their lifecycle).
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
        static float s_old_width = getWidth();
        static float s_old_height = getHeight();

        if (s_old_width != getWidth() || s_old_height != getHeight())
        {
            setDirty(true);
            s_old_width = getWidth();
            s_old_height = getHeight();
        }

        if (lastTokenizedText_ != text_ || isDirty())
        {
            lastTokenizedText_ = text_;
            tokenizeText();
            setDirty(false);
        }    
    } // END Label::onUpdate(float fElapsedTime)

    void Label::onEvent(const Event& event) 
    {
        (void)event; // Unused
    } // END Label::onEvent(const Event& event)

    void Label::onRender() 
    {       
        SDL_Renderer* renderer = getRenderer();

        // --- DEBUG / Sanity tests (enable temporarily to diagnose glyph corruption) ---
        const bool DEBUG_RENDER_TEST = false;
        if (DEBUG_RENDER_TEST)
        {
            // quick token/phrase diagnostics
            std::cout << "Label DEBUG '" << name_ << "' text length=" << text_.size()
                      << " tokens=" << tokenList.size()
                      << " phrases=" << phraseAlignLists_.size()
                      << " resource='" << resourceName_ << "'" << std::endl;

            if (!fontAsset)
            {
                std::cout << "Label DEBUG: no fontAsset for label '" << name_ << "'" << std::endl;
            }
            else
            {
                IFontObject* dbgFont = fontAsset.as<IFontObject>();
                if (!dbgFont)
                {
                    std::cout << "Label DEBUG: fontAsset is not an IFontObject for '" << resourceName_ << "'" << std::endl;
                }
                else if (!dbgFont->isLoaded())
                {
                    std::cout << "Label DEBUG: font '" << resourceName_ << "' is not loaded yet." << std::endl;
                }
                else
                {
                    // draw a short sample phrase using the font's draw API to assert rendering pipeline
                    dbgFont->setFontSize(fontSize_);
                    FontStyle dbgStyle = defaultStyle_;
                    dbgStyle.foregroundColor = SDL_Color{255,255,0,255};
                    dbgStyle.outline = true;
                    dbgStyle.dropshadow = true;
                    dbgFont->setFontStyle(dbgStyle);

                    int sampleX = getX() + 4;
                    int sampleY = getY() + 4;
                    const std::string sample = "DEBUG: ABC 0123 !@#";

                    // draw all passes explicitly so we exercise each codepath
                    dbgFont->drawPhraseDropshadow(sample, sampleX, sampleY, dbgStyle);
                    dbgFont->drawPhraseOutline(sample, sampleX, sampleY, dbgStyle);
                    dbgFont->drawPhrase(sample, sampleX, sampleY, dbgStyle);

                    // draw vertical guides at glyph boundaries to reveal glyph widths
                    int x = sampleX;
                    for (char ch : sample)
                    {
                        int gw = dbgFont->getGlyphWidth(ch);
                        int gh = dbgFont->getGlyphHeight('H');
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                        SDL_RenderLine(renderer, x, sampleY, x, sampleY + gh);
                        x += gw;
                    }
                    // trailing guide
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    SDL_RenderLine(renderer, x, sampleY, x, sampleY + dbgFont->getGlyphHeight('H'));
                }
            }
        }
        // --- END DEBUG ---


        // std::cout << getName() << "-- x:" << getX() << " y:" << getY() 
        //           << " w:" << getWidth() << " h:" << getHeight() << std::endl;

        // std::cout << getName() << "-- left: " << getLeft() << " top: " << getTop()
        //           << " right: " << getRight() << " bottom: " << getBottom() << std::endl;




        // Pass 1: render a background color if alpha > 0
        SDL_Color bgndColor = defaultStyle_.backgroundColor;
        if (bgndColor.a > 0 && defaultStyle_.background) 
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, bgndColor.r, bgndColor.g, bgndColor.b, bgndColor.a);
            SDL_FRect rect = 
            { 
                static_cast<float>(getX()), 
                static_cast<float>(getY()), 
                static_cast<float>(getWidth()), 
                static_cast<float>(getHeight()) 
            };
            SDL_RenderFillRect(renderer, &rect);
        }

        // Pass 2: render a border to verify the bounds
        SDL_Color borderColor = defaultStyle_.borderColor;

        if (defaultStyle_.border) 
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);

            // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


            SDL_FRect rect = 
            { 
                static_cast<float>(getX()), 
                static_cast<float>(getY()), 
                static_cast<float>(getWidth()), 
                static_cast<float>(getHeight()) 
            };
            SDL_RenderRect(renderer, &rect);
        }

        // tokenizeText();

        // Render the Label
        renderLabel();
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
        return std::ispunct(c);
    } // END Label::isPunctuation(char c)

    int Label::tokenizeText()
    {
        auto to_lower = [](const std::string& s) {
            std::string out = s;
            std::transform(out.begin(), out.end(), out.begin(),
                [](unsigned char c){ return std::tolower(c); });
            return out;
        };

        defaultStyle_.fontSize = fontSize_;

        FontStyle currentStyle = defaultStyle_;
        tokenList.clear();
        size_t i = 0;

        while (i < text_.size())
        {
            // Handle Spaces
            if (text_[i] == ' ')
            {
                tokenList.push_back({TokenType::Space, " ", currentStyle});
                ++i;
                continue;
            }
            // Handle Newlines
            if (text_[i] == '\n')
            {
                tokenList.push_back({TokenType::Newline, "\n", currentStyle});
                ++i;
                continue;
            }
            // Handle Tabs
            if (text_[i] == '\t')
            {
                tokenList.push_back({TokenType::Tab, "\t", currentStyle});
                ++i;
                continue;
            }

            // [[] and []]
            if (text_.substr(i, 3) == "[[]") {
                tokenList.push_back({TokenType::Word, "[", currentStyle});
                i += 3;
                continue;
            }
            if (text_.substr(i, 3) == "[]]") {
                tokenList.push_back({TokenType::Word, "]", currentStyle});
                i += 3;
                continue;
            }

            // Handle Escapes (bold, italic, underline, etc.)
            auto handleEscape = [&](size_t& i, size_t subst_size, const std::string& escape_str,
                                    std::function<void()> styleAction) -> bool {
                if (i + subst_size <= text_.size() &&
                    (text_.substr(i, subst_size) == escape_str ||
                    to_lower(text_.substr(i, subst_size)) == to_lower(escape_str)))
                {
                    tokenList.push_back({TokenType::Escape, to_lower(text_.substr(i, subst_size)), currentStyle});
                    styleAction();
                    i += subst_size;
                    return true;
                }
                return false;
            };
            if (handleEscape(i, 6, "[bold]", [&]{ currentStyle.bold = true; })) continue;
            if (handleEscape(i, 7, "[/bold]", [&]{ currentStyle.bold = false; })) continue;

            if (handleEscape(i, 8, "[italic]", [&]{ currentStyle.italic = true; })) continue;
            if (handleEscape(i, 9, "[/italic]", [&]{ currentStyle.italic = false; })) continue;

            if (handleEscape(i, 11, "[underline]", [&]{ currentStyle.underline = true; })) continue;
            if (handleEscape(i, 12, "[/underline]", [&]{ currentStyle.underline = false; })) continue;

            if (handleEscape(i, 8, "[strike]", [&]{ currentStyle.strikethrough = true; })) continue;
            if (handleEscape(i, 9, "[/strike]", [&]{ currentStyle.strikethrough = false; })) continue;

            if (handleEscape(i, 9, "[outline]", [&]{ currentStyle.outline = true; })) continue;
            if (handleEscape(i, 10, "[/outline]", [&]{ currentStyle.outline = false; })) continue;

            if (handleEscape(i, 12, "[dropshadow]", [&]{ currentStyle.dropshadow = true; })) continue;
            if (handleEscape(i, 13, "[/dropshadow]", [&]{ currentStyle.dropshadow = false; })) continue;

            if (text_.substr(i, 7) == "[reset]")
            {
                currentStyle = defaultStyle_;
                tokenList.push_back({TokenType::Escape, "[reset]", currentStyle});
                i += 7;
                continue;
            }

            // Handle Color Escapes
            std::vector<std::string> colorTags = {
                "[color=", "[rgb=", "[rgba=", "[fgnd=", "[bgnd=", "[border=", "[outline=", "[shadow=", "[align=", "[size="
            };

            auto startsWithCI = [&](const std::string& text, const std::string& prefix) {
                if (text.size() < prefix.size()) return false;
                return to_lower(text.substr(0, prefix.size())) == to_lower(prefix);
            };

            bool foundColorEscape = false;
            size_t tagLen = 0;
            std::string matchedTag;
            for (const auto& tag : colorTags) {
                if (startsWithCI(text_.substr(i), tag)) {
                    foundColorEscape = true;
                    tagLen = tag.size();
                    matchedTag = tag;
                    break;
                }
            }

            if (foundColorEscape) {
                size_t endPos = text_.find(']', i + tagLen);
                if (endPos != std::string::npos)
                {
                    std::string escapeContent = text_.substr(i + tagLen, endPos - (i + tagLen));
                    std::string fullEscape = text_.substr(i, endPos - i + 1);

                    // Color name map
                    std::unordered_map<std::string, SDL_Color> colorIDs = {
                        { "black",   SDL_Color{ 0,   0,   0,   255 } },
                        { "red",     SDL_Color{ 255, 0,   0,   255 } },
                        { "green",   SDL_Color{ 0,   255, 0,   255 } },
                        { "yellow",  SDL_Color{ 255, 255, 0,   255 } },
                        { "blue",    SDL_Color{ 0,   0,   255, 255 } },
                        { "magenta", SDL_Color{ 255, 0,   255, 255 } },
                        { "cyan",    SDL_Color{ 0,   255, 255, 255 } },
                        { "white",   SDL_Color{ 255, 255, 255, 255 } }
                    };

                    std::unordered_map<std::string, SDL_Color*> colorTargets = {
                        { "fgnd",       &currentStyle.foregroundColor },
                        { "bgnd",       &currentStyle.backgroundColor },
                        { "border",     &currentStyle.borderColor },
                        { "outline",    &currentStyle.outlineColor },
                        { "shadow",     &currentStyle.dropshadowColor }
                    };

                    // Determine target from tag
                    std::string targetName;
                    if (matchedTag == "[fgnd=")      targetName = "fgnd";
                    else if (matchedTag == "[bgnd=") targetName = "bgnd";
                    else if (matchedTag == "[border=") targetName = "border";
                    else if (matchedTag == "[outline=") targetName = "outline";
                    else if (matchedTag == "[shadow=")  targetName = "shadow";
                    else targetName = "fgnd"; // default for [color=], [rgb=], etc.

                    std::string colorValue = escapeContent;
                    colorValue = to_lower(colorValue);

                    SDL_Color color = {0,0,0,255};
                    bool validColor = false;

                    // Try colorIDs first
                    auto colorIt = colorIDs.find(colorValue);
                    if (colorIt != colorIDs.end()) {
                        color = colorIt->second;
                        validColor = true;
                    } else if (colorValue.length() == 6) { // RGB hex
                        color.r = std::stoi(colorValue.substr(0,2), nullptr, 16);
                        color.g = std::stoi(colorValue.substr(2,2), nullptr, 16);
                        color.b = std::stoi(colorValue.substr(4,2), nullptr, 16);
                        color.a = 255;
                        validColor = true;
                    } else if (colorValue.length() == 8) { // RGBA hex
                        color.r = std::stoi(colorValue.substr(0,2), nullptr, 16);
                        color.g = std::stoi(colorValue.substr(2,2), nullptr, 16);
                        color.b = std::stoi(colorValue.substr(4,2), nullptr, 16);
                        color.a = std::stoi(colorValue.substr(6,2), nullptr, 16);
                        validColor = true;
                    }

                    if (validColor) {
                        auto targetIt = colorTargets.find(targetName);
                        if (targetIt != colorTargets.end()) {
                            *(targetIt->second) = color;
                        }
                    }

                    // [rgb=rrggbb:target] and [rgba=rrggbbaa:target] with explicit target
                    if (matchedTag == "[rgb=" || matchedTag == "[rgba=") {
                        size_t colonPos = escapeContent.find(':');
                        if (colonPos != std::string::npos) {
                            std::string hex = escapeContent.substr(0, colonPos);
                            std::string explicitTarget = escapeContent.substr(colonPos + 1);
                            explicitTarget = to_lower(explicitTarget);
                            SDL_Color color2 = {0,0,0,255};
                            bool valid2 = false;
                            if (hex.length() == 6) {
                                color2.r = std::stoi(hex.substr(0,2), nullptr, 16);
                                color2.g = std::stoi(hex.substr(2,2), nullptr, 16);
                                color2.b = std::stoi(hex.substr(4,2), nullptr, 16);
                                color2.a = 255;
                                valid2 = true;
                            } else if (hex.length() == 8) {
                                color2.r = std::stoi(hex.substr(0,2), nullptr, 16);
                                color2.g = std::stoi(hex.substr(2,2), nullptr, 16);
                                color2.b = std::stoi(hex.substr(4,2), nullptr, 16);
                                color2.a = std::stoi(hex.substr(6,2), nullptr, 16);
                                valid2 = true;
                            }
                            if (valid2) {
                                auto targetIt = colorTargets.find(explicitTarget);
                                if (targetIt != colorTargets.end()) {
                                    *(targetIt->second) = color2;
                                }
                            }
                        }
                    }

                    // [align={value}]
                    if (matchedTag == "[align=")
                    {
                        std::string alignValue = to_lower(escapeContent);
                        auto it = stringToLabelAlign_.find(alignValue);
                        if (it != stringToLabelAlign_.end())
                        {
                            currentStyle.alignment = it->second;
                        }
                        tokenList.push_back({TokenType::Escape, fullEscape, currentStyle});
                        i = endPos + 1;
                        continue;
                    }

                    // [size=n]
                    if (matchedTag == "[size=")
                    {
                        int newSize = std::stoi(escapeContent);
                        if (newSize > 0)
                        {
                            fontSize_ = newSize;
                            currentStyle.fontSize = newSize;
                        }
                        tokenList.push_back({TokenType::Escape, fullEscape, currentStyle});
                        i = endPos + 1;
                        continue;
                    }

                    // Update currentStyle with new colors
                    currentStyle.foregroundColor = *colorTargets["fgnd"];
                    currentStyle.backgroundColor = *colorTargets["bgnd"];
                    currentStyle.borderColor = *colorTargets["border"];
                    currentStyle.outlineColor = *colorTargets["outline"];
                    currentStyle.dropshadowColor = *colorTargets["shadow"];

                    tokenList.push_back({TokenType::Escape, fullEscape, currentStyle});
                    i = endPos + 1;
                    continue;
                }
            }

            // Handle Punctuation
            if (isPunctuation(text_[i]))
            {
                tokenList.push_back({TokenType::Punctuation, std::string(1, text_[i]), currentStyle});
                ++i;
                continue;
            }

            // Handle word
            size_t start = i;
            while (i < text_.size() && text_[i] != ' ' &&
                text_[i] != '\n' && text_[i] != '\t' && !isPunctuation(text_[i]))
            {
                ++i;
            }
            if (start != i)
            {
                tokenList.push_back({TokenType::Word, text_.substr(start, i - start), currentStyle});
            }
        }

        _buildTokenAlignLists();
        _buildPhraseAlignLists();

        return tokenList.size();
    } // END Label::tokenizeText()

    void Label::renderLabel()
    {
        renderLabelPass(RenderPass::Dropshadow);
        renderLabelPass(RenderPass::Outline);
        renderLabelPass(RenderPass::Foreground);
    } // END Label::renderLabel()
    
    void Label::renderLabelPass(RenderPass pass)
    {
        if (!fontAsset) return;
        IFontObject* font_ = fontAsset.as<IFontObject>();
        if (!font_) return;
        if (!font_->isLoaded()) return;

        int labelX = getX();
        int labelY = getY();
        int labelW = getWidth();
        int labelH = getHeight();

        for (const auto& [align, phrases] : phraseAlignLists_)
        {
            // Group phrases by lineIndex
            std::map<int, std::vector<PhraseToken>> lines;
            for (const auto& phrase : phrases)
                lines[phrase.lineIndex].push_back(phrase);

            // Compute total text height for vertical alignment
            float totalTextHeight = 0.0f;
            std::vector<float> lineHeights;
            for (const auto& [_, linePhrases] : lines) {
                float maxHeight = 0.0f;
                for (const auto& phrase : linePhrases)
                    if (phrase.height > maxHeight) maxHeight = phrase.height;
                lineHeights.push_back(maxHeight);
                totalTextHeight += maxHeight;
            }

            float baseY = labelY;
            switch (align) {
                case AlignQueue::TOP_LEFT:
                case AlignQueue::TOP_CENTER:
                case AlignQueue::TOP_RIGHT:
                    baseY = labelY;
                    break;
                case AlignQueue::MIDDLE_LEFT:
                case AlignQueue::MIDDLE_CENTER:
                case AlignQueue::MIDDLE_RIGHT:
                    baseY = labelY + (labelH - totalTextHeight) / 2.0f;
                    break;
                case AlignQueue::BOTTOM_LEFT:
                case AlignQueue::BOTTOM_CENTER:
                case AlignQueue::BOTTOM_RIGHT:
                    baseY = labelY + (labelH - totalTextHeight);
                    break;
                default:
                    baseY = labelY;
            }

            float lineY = baseY;
            int lineIdx = 0;
            for (const auto& [_, linePhrases] : lines)
            {
                // Compute total width for this line
                float lineWidth = 0.0f;
                for (const auto& phrase : linePhrases)
                    lineWidth += phrase.width;

                // Compute base X for alignment
                float baseX = labelX;
                switch (align) {
                    case AlignQueue::TOP_LEFT:
                    case AlignQueue::MIDDLE_LEFT:
                    case AlignQueue::BOTTOM_LEFT:
                        baseX = labelX;
                        break;
                    case AlignQueue::TOP_CENTER:
                    case AlignQueue::MIDDLE_CENTER:
                    case AlignQueue::BOTTOM_CENTER:
                        baseX = labelX + (labelW / 2.0f) - (lineWidth / 2.0f);
                        break;
                    case AlignQueue::TOP_RIGHT:
                    case AlignQueue::MIDDLE_RIGHT:
                    case AlignQueue::BOTTOM_RIGHT:
                        baseX = labelX + labelW - lineWidth;
                        break;
                    default:
                        baseX = labelX;
                }

                // Render all phrases in this line, left-to-right
                float phraseX = baseX;
                for (const auto& phrase : linePhrases) {
                    font_->setFontStyle(phrase.style);

                    switch (pass) {
                        case RenderPass::Dropshadow:
                            font_->drawPhraseDropshadow(phrase.text, phraseX, lineY, phrase.style);
                            break;
                        case RenderPass::Outline:
                            font_->drawPhraseOutline(phrase.text, phraseX, lineY, phrase.style);
                            break;
                        case RenderPass::Foreground:
                            font_->drawPhrase(phrase.text, phraseX, lineY, phrase.style);
                            break;
                    }
                    phraseX += phrase.width;
                }
                lineY += lineHeights[lineIdx++];
            }
        }
    } // END Label::renderLabelPass(RenderPass pass)

    // --- Helper to build alignment lists from tokenList --- //
    void Label::_buildTokenAlignLists()
    {
        tokenAlignLists_.clear();
        for (const auto& token : tokenList)
        {
            AlignQueue queueKey = alignXRef_.at(token.style.alignment);
            tokenAlignLists_[queueKey].push_back(token);
        }
        
        #define DEBUG_TEXT false
        if (DEBUG_TEXT)
        {
            // display a tree of tokenAlignLists_ in key order
            const std::map<AlignQueue, std::string> alignQueueToString_ = 
            {
                { AlignQueue::TOP_LEFT, "top_left" },
                { AlignQueue::TOP_CENTER, "top_center" },
                { AlignQueue::TOP_RIGHT, "top_right" },
                { AlignQueue::MIDDLE_LEFT, "middle_left" },
                { AlignQueue::MIDDLE_CENTER, "middle_center" },
                { AlignQueue::MIDDLE_RIGHT, "middle_right" },
                { AlignQueue::BOTTOM_LEFT, "bottom_left" },
                { AlignQueue::BOTTOM_CENTER, "bottom_center" },
                { AlignQueue::BOTTOM_RIGHT, "bottom_right" },
                { AlignQueue::DEFAULT, "default" },
                { AlignQueue::LEFT, "left" },
                { AlignQueue::CENTER, "center" },
                { AlignQueue::RIGHT, "right" },
                { AlignQueue::TOP, "top" },
                { AlignQueue::MIDDLE, "middle" },
                { AlignQueue::BOTTOM, "bottom" }
            };
            std::cout << "text_: " << text_ << std::endl;
            for (const auto& [queue, tokens] : tokenAlignLists_)
            {
                auto nameIt = alignQueueToString_.find(queue);
                std::string alignName = (nameIt != alignQueueToString_.end()) ? nameIt->second : std::to_string(static_cast<int>(queue));
                std::cout << "Alignment: " << alignName << "\n";
                for (const auto& token : tokens)
                {
                    std::cout << "  - Token: " << token.text << "\n";
                }
            }
        } // END: DEBUG_TEXT
    } // END Label::_buildTokenAlignLists()

    void Label::_debugToken(const LabelToken& token)
    {
        std::string type;
        switch (token.type) {
            case TokenType::Word: type = "Word"; break;
            case TokenType::Escape: type = "Escape"; break;
            case TokenType::Space: type = "Space"; break;
            case TokenType::Newline: type = "Newline"; break;
            case TokenType::Tab: type = "Tab"; break;
            case TokenType::Punctuation: type = "Punctuation"; break;
            default: type = "Unknown"; break;
        }
        // Show style details for debugging
        std::cout << CLR::YELLOW << "Token: '" << CLR::WHITE << token.text << CLR::YELLOW << "'" << CLR::RESET << std::endl;
        std::cout << "[\n"
                << "\tType: " << type << ", \n"
                << "\tbold=" << token.style.bold << ", \n"
                << "\titalic=" << token.style.italic << ", \n"
                << "\tunderline=" << token.style.underline << ", \n"
                << "\tstrikethrough=" << token.style.strikethrough << ", \n"
                << "\toutline=" << token.style.outline << ", \n"
                << "\tdropshadow=" << token.style.dropshadow << ", \n"
                << "\tfontSize=" << token.style.fontSize << ", \n"
                << "\twordwrap=" << token.style.wordwrap << ", \n"
                << "\talign=" << labelAlignToString_.at(token.style.alignment) << ", \n"
                << "\tfg=(" << (int)token.style.foregroundColor.r << "," << (int)token.style.foregroundColor.g << "," << (int)token.style.foregroundColor.b << "," << (int)token.style.foregroundColor.a << ")" << ", \n"
                << "\tbg=(" << (int)token.style.backgroundColor.r << "," << (int)token.style.backgroundColor.g << "," << (int)token.style.backgroundColor.b << "," << (int)token.style.backgroundColor.a << ")" << ", \n"
                << "\tborder=(" << (int)token.style.borderColor.r << "," << (int)token.style.borderColor.g << "," << (int)token.style.borderColor.b << "," << (int)token.style.borderColor.a << ")" << ", \n"
                << "\toutlineColor=(" << (int)token.style.outlineColor.r << "," << (int)token.style.outlineColor.g << "," << (int)token.style.outlineColor.b << "," << (int)token.style.outlineColor.a << ")" << ", \n"
                << "\tdropshadowColor=(" << (int)token.style.dropshadowColor.r << "," << (int)token.style.dropshadowColor.g << "," << (int)token.style.dropshadowColor.b << "," << (int)token.style.dropshadowColor.a << ")" << ", \n"
                << "]" << std::endl;
    } // END Label::_debugToken(const LabelToken& token)

    void Label::_maxSize(float& width, float& height)
    {
        float userMaxWidth = static_cast<float>(defaultStyle_.maxWidth);
        float userMaxHeight = static_cast<float>(defaultStyle_.maxHeight);

        // If maxWidth == 0, width resizing is disabled; use current width as max
        if (userMaxWidth == 0.0f) {
            width = static_cast<float>(getWidth());
        } else {
            float maxW = 32767.0f;
            if (auto parent = getParent()) {
                float parentW = static_cast<float>(parent->getWidth());
                // compute left edge relative to parent (avoid mixing world and local coords)
                float leftEdge = static_cast<float>(getX()) - static_cast<float>(parent->getX());
                float availW = (parentW > leftEdge) ? (parentW - leftEdge) : 0.0f;
                // If userMaxWidth < 0, use parent's available width
                if (userMaxWidth < 0.0f) {
                    maxW = availW;
                } else {
                    // Use the smaller of userMaxWidth and parent's available width
                    maxW = std::min(userMaxWidth, availW);
                }
                // // Debugging info
                // std::cout << "  _maxSize debug: parentW=" << parentW << " leftEdge=" << leftEdge
                //           << " userMaxWidth=" << userMaxWidth << " availW=" << availW
                //           << " -> chosen maxW=" << maxW << std::endl;
            } else if (userMaxWidth > 0.0f) {
                maxW = userMaxWidth;
            }
            width = maxW;
        }
        // If maxHeight == 0, height resizing is disabled; use current height as max
        if (userMaxHeight == 0.0f) {
            height = static_cast<float>(getHeight());
        } else {
            float maxH = 32767.0f;
            if (auto parent = getParent()) {
                float parentH = static_cast<float>(parent->getHeight());
                // compute top edge relative to parent
                float topEdge = static_cast<float>(getY()) - static_cast<float>(parent->getY());
                // If userMaxHeight < 0, use parent's available height
                if (userMaxHeight < 0.0f) {
                    maxH = (parentH > topEdge) ? (parentH - topEdge) : 0.0f;
                } else {
                    float availH = (parentH > topEdge) ? (parentH - topEdge) : 0.0f;
                    maxH = std::min(userMaxHeight, availH);
                }
            } else if (userMaxHeight > 0.0f) {
                maxH = userMaxHeight;
            }
            height = maxH;
        }
    } // END Label::_maxSize(float& width, float& height)

    void Label::_buildPhraseAlignLists()
    {
        if (!fontAsset) return;
        IFontObject* font_ = fontAsset.as<IFontObject>();
        if (!font_) return;
        if (!font_->isLoaded()) return;

        phraseAlignLists_.clear();
        float maxWidth = 0, maxHeight = 0;
        _maxSize(maxWidth, maxHeight);
        // Debugging wrapper internals
        const bool DEBUG_WRAP = false;
        if (DEBUG_WRAP) {
            std::cout << CLR::YELLOW << "Label::_buildPhraseAlignLists -- '" << name_ << "'" << CLR::RESET << std::endl;
            std::cout << "  getWidth()=" << getWidth()
                      << " defaultStyle_.maxWidth=" << defaultStyle_.maxWidth
                      << " -> computed maxWidth=" << maxWidth
                      << " maxHeight=" << maxHeight
                      << " fontSize=" << fontSize_ << std::endl;
        }
        font_->setFontSize(fontSize_);

        for (auto& [align, tokens] : tokenAlignLists_)
        {
            std::vector<std::vector<LabelToken>> lines;
            std::vector<LabelToken> currentLine;
            float currentLineWidth = 0.0f;
            float totalHeight = 0.0f;
            auto getLineHeight = [&](const std::vector<LabelToken>& line) -> float {
                float maxH = 0.0f;
                for (const auto& token : line) {
                    // font_->setFontSize(token.style.fontSize);
                    font_->setFontStyle(token.style);
                    int tokenHeight = font_->getGlyphHeight('H');
                    if (tokenHeight > maxH) maxH = tokenHeight;
                }
                return maxH;
            };

            size_t i = 0;
            while (i < tokens.size()) {
                const auto& token = tokens[i];
                bool isVisible = (token.type == TokenType::Word ||
                                token.type == TokenType::Space ||
                                token.type == TokenType::Punctuation ||
                                token.type == TokenType::Tab);

                // font_->setFontSize(token.style.fontSize);
                font_->setFontStyle(token.style);

                // Group word + trailing punctuation for wrapping
                if (token.type == TokenType::Word && i + 1 < tokens.size() && tokens[i+1].type == TokenType::Punctuation) {
                    int wordWidth = font_->getWordWidth(token.text);
                    // font_->setFontSize(tokens[i+1].style.fontSize);
                    font_->setFontStyle(tokens[i+1].style);

                    int punctWidth = font_->getWordWidth(tokens[i+1].text);
                    int clusterWidth = wordWidth + punctWidth;

                    if (maxWidth > 0.0f && currentLineWidth + clusterWidth > maxWidth && !currentLine.empty()) {
                        // Trim trailing spaces before pushing the line
                        while (!currentLine.empty() && currentLine.back().type == TokenType::Space) {
                            currentLineWidth -= font_->getGlyphWidth(' ');
                            currentLine.pop_back();
                        }
                        float lineHeight = getLineHeight(currentLine);
                        if (maxHeight > 0.0f && (totalHeight + lineHeight > maxHeight))
                            break;
                        totalHeight += lineHeight;
                        lines.push_back(currentLine);
                        currentLine.clear();
                        currentLineWidth = 0.0f;
                    }
                    currentLine.push_back(token);
                    currentLine.push_back(tokens[i+1]);
                    currentLineWidth += clusterWidth;
                    i += 2;
                    continue;
                }

                // Handle explicit newline
                if (token.type == TokenType::Newline)
                {
                    // Trim trailing spaces before pushing the line
                    while (!currentLine.empty() && currentLine.back().type == TokenType::Space) {
                        currentLineWidth -= font_->getGlyphWidth(' ');
                        currentLine.pop_back();
                    }
                    float lineHeight = getLineHeight(currentLine);
                    if (maxHeight > 0.0f && (totalHeight + lineHeight > maxHeight))
                        break;
                    totalHeight += lineHeight;
                    lines.push_back(currentLine);
                    currentLine.clear();
                    currentLineWidth = 0.0f;
                    ++i;
                    continue;
                }

                // Word wrap for other tokens
                int tokenWidth = 0;
                if (isVisible)
                {
                    if (token.type == TokenType::Word || token.type == TokenType::Punctuation)
                        tokenWidth = font_->getWordWidth(token.text);
                    else if (token.type == TokenType::Space)
                        tokenWidth = font_->getGlyphWidth(' ');
                    else if (token.type == TokenType::Tab)
                        tokenWidth = font_->getGlyphWidth(' ') * 4;
                }
                if (isVisible && maxWidth > 0.0f && currentLineWidth + tokenWidth > maxWidth && !currentLine.empty())
                {
                    // Trim trailing spaces before pushing the line
                    while (!currentLine.empty() && currentLine.back().type == TokenType::Space) {
                        currentLineWidth -= font_->getGlyphWidth(' ');
                        currentLine.pop_back();
                    }
                    float lineHeight = getLineHeight(currentLine);
                    if (maxHeight > 0.0f && (totalHeight + lineHeight > maxHeight))
                        break;
                    totalHeight += lineHeight;
                    lines.push_back(currentLine);
                    currentLine.clear();
                    currentLineWidth = 0.0f;
                }

                currentLine.push_back(token);
                currentLineWidth += tokenWidth;
                ++i;
            }
            // Push last line if not empty and within height
            if (!currentLine.empty()) {
                // Trim trailing spaces before pushing the line
                while (!currentLine.empty() && currentLine.back().type == TokenType::Space) {
                    currentLineWidth -= font_->getGlyphWidth(' ');
                    currentLine.pop_back();
                }
                float lineHeight = getLineHeight(currentLine);
                if (maxHeight <= 0.0f || (totalHeight + lineHeight <= maxHeight)) {
                    lines.push_back(currentLine);
                }
            }

            // --- Second pass: Build phrases within each line ---
            std::vector<PhraseToken> phrases;
            for (int lineIdx = 0; lineIdx < (int)lines.size(); ++lineIdx)
            {
                const auto& lineTokens = lines[lineIdx];
                PhraseToken currentPhrase;
                float phraseWidth = 0.0f;
                float phraseHeight = 0.0f;
                bool inPhrase = false;

                for (const auto& token : lineTokens)
                {
                    bool isVisible = (token.type == TokenType::Word ||
                                    token.type == TokenType::Space ||
                                    token.type == TokenType::Punctuation ||
                                    token.type == TokenType::Tab);

                    // font_->setFontSize(token.style.fontSize);
                    font_->setFontStyle(token.style);

                    int tokenWidth = 0, tokenHeight = 0;
                    if (isVisible)
                    {
                        if (token.type == TokenType::Word || token.type == TokenType::Punctuation)
                            tokenWidth = font_->getWordWidth(token.text);
                        else if (token.type == TokenType::Space)
                            tokenWidth = font_->getGlyphWidth(' ');
                        else if (token.type == TokenType::Tab)
                            tokenWidth = font_->getGlyphWidth(' ') * 4;
                        tokenHeight = font_->getGlyphHeight('H');
                    }

                    // Start new phrase on style change
                    if (!inPhrase || token.style != currentPhrase.style)
                    {
                        if (inPhrase && !currentPhrase.text.empty())
                        {
                            currentPhrase.width = phraseWidth;
                            currentPhrase.height = phraseHeight;
                            currentPhrase.lineIndex = lineIdx;
                            phrases.push_back(currentPhrase);
                            currentPhrase = {};
                            phraseWidth = 0.0f;
                            phraseHeight = 0.0f;
                        }
                        if (isVisible)
                        {
                            currentPhrase.text = token.text;
                            currentPhrase.style = token.style;
                            inPhrase = true;
                            phraseWidth = tokenWidth;
                            phraseHeight = tokenHeight;
                        }
                    }
                    else if (isVisible)
                    {
                        currentPhrase.text += token.text;
                        phraseWidth += tokenWidth;
                        if (tokenHeight > phraseHeight)
                            phraseHeight = tokenHeight;
                    }
                }
                if (inPhrase && !currentPhrase.text.empty())
                {
                    currentPhrase.width = phraseWidth;
                    currentPhrase.height = phraseHeight;
                    currentPhrase.lineIndex = lineIdx;
                    phrases.push_back(currentPhrase);
                }
            }
            phraseAlignLists_[align] = std::move(phrases);
        }
    } // END Label::_buildPhraseAlignLists()

    // --- Lua Registration --- //

    void Label::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Label:" + getName();
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN 
                    << typeName << CLR::RESET << std::endl;
        }

        // // Augment the single shared AssetHandle handle usertype (assets are exposed via AssetHandle handles in Lua)
        // sol::table handle = AssetHandle::ensure_handle_table(lua);

        // // Helper to check if a property/command is already registered
        // auto absent = [&](const char* name) -> bool {
        //     sol::object cur = handle.raw_get_or(name, sol::lua_nil);
        //     return !cur.valid() || cur == sol::lua_nil;
        // };

        // // Helper to register a property/command if not already present
        // auto reg = [&](const char* name, auto&& fn) {
        //     if (absent(name)) {
        //         handle.set_function(name, std::forward<decltype(fn)>(fn));
        //     }
        // };

        // // small helper to validate and cast the AssetHandle -> Label*
        // auto cast_label_from_asset = [](const AssetHandle& asset) -> Label* {
        //     if (!asset.isValid()) { ERROR("invalid AssetHandle provided to Label method"); }
        //     IAssetObject* base = asset.get();
        //     Label* label = dynamic_cast<Label*>(base);
        //     if (!label) { ERROR("invalid Label object"); }
        //     return label;
        // };

        // // Register Label-specific properties and commands here (bridge from AssetHandle handle)
        // reg("setLabelWidth", [cast_label_from_asset](AssetHandle asset, int w) { cast_label_from_asset(asset)->setLabelWidth(w); });
        // reg("setLabelHeight", [cast_label_from_asset](AssetHandle asset, int h) { cast_label_from_asset(asset)->setLabelHeight(h); });
        // ...



    } // END Label::_registerLuaBindings(const std::string& typeName, sol::state_view lua)

} // END namespace SDOM