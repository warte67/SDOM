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

        defaultStyle_.alignment = init.alignment;
        defaultStyle_.foregroundColor = init.foregroundColor;
        defaultStyle_.backgroundColor = init.backgroundColor;
        defaultStyle_.borderColor = init.borderColor;
        defaultStyle_.outlineColor = init.outlineColor;
        defaultStyle_.dropshadowColor = init.dropshadowColor;
        defaultStyle_.fontWidth = fontWidth_;
        defaultStyle_.fontHeight = fontHeight_;
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
        defaultStyle_.wordwrap = init.wordwrap;
        defaultStyle_.auto_resize = init.auto_resize;        
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
        fontSize_ = get_int("font_size", init.fontSize);
        fontWidth_ = get_int("font_width", fontSize_);
        fontHeight_ = get_int("font_height", fontSize_);


    // Record whether the user provided explicit per-axis values.
    try { userFontWidthSpecified_ = config["font_width"].valid(); } catch(...) { userFontWidthSpecified_ = false; }
    try { userFontHeightSpecified_ = config["font_height"].valid(); } catch(...) { userFontHeightSpecified_ = false; }

    // Normalize width/height -> fallback to fontSize when unspecified/invalid
    if (fontWidth_ <= 0)  fontWidth_  = (fontSize_ > 0 ? fontSize_ : init.fontWidth);
    if (fontHeight_ <= 0) fontHeight_ = (fontSize_ > 0 ? fontSize_ : init.fontHeight);

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
        defaultStyle_.auto_resize = get_bool("auto_resize", init.auto_resize);
        defaultStyle_.padding_horiz = get_int("padding_horiz", defaultStyle_.padding_horiz);
        defaultStyle_.padding_vert = get_int("padding_vert", defaultStyle_.padding_vert);
        defaultStyle_.dropshadowOffsetX = get_int("dropshadow_offset_x", defaultStyle_.dropshadowOffsetX);
        defaultStyle_.dropshadowOffsetY = get_int("dropshadow_offset_y", defaultStyle_.dropshadowOffsetY);
        defaultStyle_.maxWidth = get_int("max_width", defaultStyle_.maxWidth);
        defaultStyle_.maxHeight = get_int("max_height", defaultStyle_.maxHeight);
        defaultStyle_.borderThickness = get_int("border_thickness", defaultStyle_.borderThickness);
        defaultStyle_.outlineThickness = get_int("outline_thickness", defaultStyle_.outlineThickness);

        // propagate per-axis bitmap font metrics into default style so fonts can read them
        defaultStyle_.fontWidth = fontWidth_;
        defaultStyle_.fontHeight = fontHeight_;

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

        // If the user did not explicitly set font_size on the Label config,
        // and we have a font asset, adopt the font asset's nominal fontSize so
        // bitmap fonts render at their native sprite height instead of the
        // default init font size (often 8px).
        bool userProvidedFontSize = false;
        try { userProvidedFontSize = config["font_size"].valid(); } catch(...) { userProvidedFontSize = false; }
        if (!userProvidedFontSize && fontAsset.isValid()) {
            IFontObject* f = fontAsset.as<IFontObject>();
            if (f) {
                FontStyle fs = f->getFontStyle();
                if (fs.fontSize > 0) {
                    fontSize_ = fs.fontSize;
                    defaultStyle_.fontSize = fontSize_;
                }
            }
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

        // If the user did not explicitly supply font_width/font_height for a BitmapFont,
        // default those Label fields to the sprite dimensions from the backing
        // SpriteSheet (so bitmap fonts render at their native per-glyph size).
        BitmapFont* bmp = dynamic_cast<BitmapFont*>(font);
        if (bmp) {
            int sw = bmp->getBitmapFontWidth();
            int sh = bmp->getBitmapFontHeight();
            if (!userFontWidthSpecified_ && sw > 0) {
                fontWidth_ = sw;
                defaultStyle_.fontWidth = fontWidth_;
            }
            if (!userFontHeightSpecified_ && sh > 0) {
                fontHeight_ = sh;
                defaultStyle_.fontHeight = fontHeight_;
                // Also adopt fontSize_ from sprite height if Label didn't set it.
                if (fontSize_ <= 0) {
                    fontSize_ = sh;
                    defaultStyle_.fontSize = fontSize_;
                }
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
        (void)fElapsedTime;


        // TODO: Fix this to handle parent resizing properly.
        // Currently disabled because it causes issues with certain layout scenarios.
        // It would be better to have the parent notify children of size changes.
        // This is a naive approach that can lead to excessive re-tokenization.

        // if (getParent()->getWidth() != parent_width_ || getParent()->getHeight() != parent_height_)
        // {
        //     parent_width_ = getParent()->getWidth();
        //     parent_height_ = getParent()->getHeight();
        //     tokenizeText();
        //     setDirty(true);
        //     INFO("Parent Size Change");
        // }
        
        if (lastTokenizedText_ != text_ || needsTextureRebuild_(getWidth(), getHeight(), getCore().getPixelFormat()))
        {
            lastTokenizedText_ = text_;
            tokenizeText();
            setDirty(true);
        }
        // Other dirty triggers can be handled elsewhere (bounds, style, etc.)
    } // END Label::onUpdate(float fElapsedTime)

    void Label::onEvent(const Event& event) 
    {
        (void)event; // Unused
    } // END Label::onEvent(const Event& event)

    void Label::onRender() 
    {       
        SDL_Renderer* renderer = getRenderer();
        SDL_Texture* target = SDL_GetRenderTarget(renderer);

        if (isDirty()) 
        {
            if (!rebuildTexture_(getWidth(), getHeight(), getCore().getPixelFormat())) 
            {
                ERROR("Label::onRender() -- texture rebuild failed");
                return;
            }
            // If we don't have a cached texture (e.g., size is 0x0), skip rendering this frame
            if (cachedTexture_) 
            {
                if (!SDL_SetRenderTarget(renderer, cachedTexture_)) 
                {
                    ERROR("Label::onRender -- Unable to set render target: " + std::string(SDL_GetError()));
                    return;
                }
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                SDL_RenderClear(renderer);
                // Pass 1: render background
                SDL_Color bgndColor = defaultStyle_.backgroundColor;
                if (bgndColor.a > 0 && defaultStyle_.background) 
                {
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, bgndColor.r, bgndColor.g, bgndColor.b, bgndColor.a);
                    SDL_FRect rect = { 0, 0, static_cast<float>(current_width), static_cast<float>(current_height) };
                    SDL_RenderFillRect(renderer, &rect);
                }
                // Pass 2: render border
                SDL_Color borderColor = defaultStyle_.borderColor;
                if (defaultStyle_.border) 
                {
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
                    SDL_FRect rect = { 0, 0, static_cast<float>(current_width), static_cast<float>(current_height) };
                    SDL_RenderRect(renderer, &rect);
                }
                // Render the Label
                renderLabel();
                SDL_SetRenderTarget(renderer, target);
            }
            // Mark clean even if we skipped rendering due to zero-sized texture
            setDirty(false);
        }

        // Draw the cached texture to the screen
        if (cachedTexture_) 
        {
            SDL_FRect dst = 
            {
                static_cast<float>(getX()),
                static_cast<float>(getY()),
                static_cast<float>(getWidth()),
                static_cast<float>(getHeight())
            };
            if (!SDL_RenderTexture(renderer, cachedTexture_, nullptr, &dst)) 
            {
                ERROR("Label::onRender() -- Unable to render to texture: " + std::string(SDL_GetError()));
                return;
            }
        }
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
            setDirty(true);
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

        // Determine whether we're rendering into the cached texture (local coords)
        SDL_Renderer* renderer = getRenderer();
        SDL_Texture* currentTarget = SDL_GetRenderTarget(renderer);
        bool renderingToTexture = (currentTarget == cachedTexture_);

        // Choose coordinates and sizes appropriate for the current render target.
        // When rasterizing into the cached texture we use local coords (origin 0,0),
        // otherwise use world/screen coords so text positions match the final blit.
        int labelX = renderingToTexture ? 0 : getX();
        int labelY = renderingToTexture ? 0 : getY();
        // When rendering into our cached texture, use the texture's actual size
        // (current_width/current_height) instead of local/world coordinates which
        // may be zero during layout. When rendering directly to the screen, use
        // the object's world width/height.
        int labelW = renderingToTexture ? static_cast<int>(current_width) : getWidth();
        int labelH = renderingToTexture ? static_cast<int>(current_height) : getHeight();

        // Respect parent clipping/available area when drawing directly to screen only.
        if (!renderingToTexture)
        {
            DisplayHandle parent = getParent();
            if (parent)
            {
                int parentW = parent->getWidth();
                int parentH = parent->getHeight();
                labelW = std::min(labelW, parentW);
                labelH = std::min(labelH, parentH);
            }
        }

        // Apply padding: center/layout should operate on the inner rect
        int padX = defaultStyle_.padding_horiz;
        int padY = defaultStyle_.padding_vert;
        int innerX = labelX + padX;
        int innerY = labelY + padY;
        int innerW = std::max(0, labelW - padX * 2);
        int innerH = std::max(0, labelH - padY * 2);

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

            float baseY = static_cast<float>(innerY);
            switch (align) {
                case AlignQueue::TOP_LEFT:
                case AlignQueue::TOP_CENTER:
                case AlignQueue::TOP_RIGHT:
                    baseY = static_cast<float>(innerY);
                    break;
                case AlignQueue::MIDDLE_LEFT:
                case AlignQueue::MIDDLE_CENTER:
                case AlignQueue::MIDDLE_RIGHT:
                    baseY = static_cast<float>(innerY) + (static_cast<float>(innerH) - totalTextHeight) / 2.0f;
                    break;
                case AlignQueue::BOTTOM_LEFT:
                case AlignQueue::BOTTOM_CENTER:
                case AlignQueue::BOTTOM_RIGHT:
                    baseY = static_cast<float>(innerY) + (static_cast<float>(innerH) - totalTextHeight);
                    break;
                default:
                    baseY = static_cast<float>(innerY);
            }

            float lineY = baseY;
            int lineIdx = 0;
            for (const auto& [_, linePhrases] : lines)
            {
                // Compute total width for this line
                float lineWidth = 0.0f;
                for (const auto& phrase : linePhrases)
                    lineWidth += phrase.width;

#define LABEL_DEBUG false
if (LABEL_DEBUG)
{
                // Debug: when rasterizing to texture, show computed inner rect and base positions
                if (renderingToTexture) {
                    // compute tentative baseX/baseY so we can print them
                    float dbg_baseX = static_cast<float>(innerX);
                    switch (align) {
                        case AlignQueue::TOP_CENTER:
                        case AlignQueue::MIDDLE_CENTER:
                        case AlignQueue::BOTTOM_CENTER:
                            dbg_baseX = static_cast<float>(innerX) + (static_cast<float>(innerW) / 2.0f) - (lineWidth / 2.0f);
                            break;
                        case AlignQueue::TOP_RIGHT:
                        case AlignQueue::MIDDLE_RIGHT:
                        case AlignQueue::BOTTOM_RIGHT:
                            dbg_baseX = static_cast<float>(innerX) + static_cast<float>(innerW) - lineWidth;
                            break;
                        default:
                            dbg_baseX = static_cast<float>(innerX);
                    }
                    float dbg_baseY = static_cast<float>(innerY);
                    switch (align) {
                        case AlignQueue::MIDDLE_LEFT:
                        case AlignQueue::MIDDLE_CENTER:
                        case AlignQueue::MIDDLE_RIGHT:
                            dbg_baseY = static_cast<float>(innerY) + (static_cast<float>(innerH) - totalTextHeight) / 2.0f;
                            break;
                        case AlignQueue::BOTTOM_LEFT:
                        case AlignQueue::BOTTOM_CENTER:
                        case AlignQueue::BOTTOM_RIGHT:
                            dbg_baseY = static_cast<float>(innerY) + (static_cast<float>(innerH) - totalTextHeight);
                            break;
                        default:
                            dbg_baseY = static_cast<float>(innerY);
                    }
                    std::cerr << "[Label DEBUG] name='" << name_ << "' inner=(" << innerX << "," << innerY << "," << innerW << "," << innerH << ")"
                              << " align=" << static_cast<int>(align) << " lineIdx=" << lineIdx << " lineWidth=" << lineWidth
                              << " dbg_baseX=" << dbg_baseX << " dbg_baseY=" << dbg_baseY << std::endl;
                }
} // END LABEL_DEBUG

                // Compute base X for alignment
                float baseX = static_cast<float>(innerX);
                switch (align) {
                    case AlignQueue::TOP_LEFT:
                    case AlignQueue::MIDDLE_LEFT:
                    case AlignQueue::BOTTOM_LEFT:
                        baseX = static_cast<float>(innerX);
                        break;
                    case AlignQueue::TOP_CENTER:
                    case AlignQueue::MIDDLE_CENTER:
                    case AlignQueue::BOTTOM_CENTER:
                        baseX = static_cast<float>(innerX) + (static_cast<float>(innerW) / 2.0f) - (lineWidth / 2.0f);
                        break;
                    case AlignQueue::TOP_RIGHT:
                    case AlignQueue::MIDDLE_RIGHT:
                    case AlignQueue::BOTTOM_RIGHT:
                        baseX = static_cast<float>(innerX) + static_cast<float>(innerW) - lineWidth;
                        break;
                    default:
                        baseX = static_cast<float>(innerX);
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

        // If wordwrap is disabled, treat the entire text as a single token
        if (!defaultStyle_.wordwrap) 
        {
            IFontObject* font_ = fontAsset.as<IFontObject>();
            // Prefer a stable width during early layout: fall back through
            // getWidth() and cached texture width if local width is not yet set.
            int labelWidth = static_cast<int>(getLocalWidth());
            if (labelWidth <= 0) labelWidth = getWidth();
            if (labelWidth <= 0 && static_cast<int>(current_width) > 0) labelWidth = static_cast<int>(current_width);

            // Compute parent's available width if needed
            int parentWidth = labelWidth;
            if (auto parent = getParent()) {
                int leftEdge = static_cast<int>(getLocalX()) - static_cast<int>(parent->getLocalX());
                int availW = static_cast<int>(parent->getLocalWidth()) - leftEdge;
                parentWidth = (availW > 0) ? availW : labelWidth;
            }

            // Use the minimum of labelWidth and parentWidth
            int effectiveWidth = std::min(labelWidth, parentWidth);

            std::vector<LabelToken> truncatedTokens;
            int widthSoFar = 0;
            // If we still don't have a reliable width, don't truncate: keep tokens.
            bool skipTruncate = (effectiveWidth <= 0);
            bool hasAny = false;
            for (size_t ti = 0; ti < tokenList.size(); ++ti)
            {
                const auto& token = tokenList[ti];
                if (skipTruncate) { truncatedTokens.push_back(token); hasAny = true; continue; }
                int tokenWidth = 0;
                if (font_) 
                {
                    if (token.type == TokenType::Word || token.type == TokenType::Punctuation)
                        tokenWidth = font_->getWordWidth(token.text);
                    else if (token.type == TokenType::Space)
                        tokenWidth = font_->getGlyphWidth(' ');
                    else if (token.type == TokenType::Tab)
                        tokenWidth = font_->getGlyphWidth(' ') * 4;
                }
                // Always allow the very first visible token even if it exceeds effectiveWidth;
                // this ensures we render something and let it be clipped.
                bool isVisible = (token.type == TokenType::Word || token.type == TokenType::Punctuation || token.type == TokenType::Space || token.type == TokenType::Tab);
                if (!hasAny && isVisible) {
                    truncatedTokens.push_back(token);
                    widthSoFar += tokenWidth;
                    hasAny = true;
                    continue;
                }
                if (widthSoFar + tokenWidth > effectiveWidth)
                    break;
                truncatedTokens.push_back(token);
                widthSoFar += tokenWidth;
                if (isVisible) hasAny = true;
            }
            AlignQueue queue = alignXRef_[defaultStyle_.alignment];
            tokenAlignLists_[queue] = truncatedTokens;
            return;
        }        
        // Wordwrap is enabled: distribute tokens into alignment queues
        for (const auto& token : tokenList)
        {
            AlignQueue queueKey = alignXRef_.at(token.style.alignment);
            tokenAlignLists_[queueKey].push_back(token);
        }
        // DEBUG: dump tokenAlignLists_ contents
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
        if (userMaxWidth == 0.0f) 
        {
            width = static_cast<float>(getWidth());
        } 
        else 
        {
            float maxW = 32767.0f;
            if (auto parent = getParent()) 
            {
                float labelW = static_cast<float>(getWidth());
                float parentW = static_cast<float>(parent->getWidth());
                parentW = std::min(parentW, labelW);
                // compute left edge relative to parent (avoid mixing world and local coords)
                float leftEdge = static_cast<float>(getX()) - static_cast<float>(parent->getX());
                float availW = (parentW > leftEdge) ? (parentW - leftEdge) : 0.0f;
                // If userMaxWidth < 0, use parent's available width
                if (userMaxWidth < 0.0f) 
                {
                    maxW = availW;
                } 
                else 
                {
                    // Use the smaller of userMaxWidth and parent's available width
                    maxW = std::min(userMaxWidth, availW);
                }
                // // Debugging info
                // std::cout << "  _maxSize debug: parentW=" << parentW << " leftEdge=" << leftEdge
                //           << " userMaxWidth=" << userMaxWidth << " availW=" << availW
                //           << " -> chosen maxW=" << maxW << std::endl;
            } 
            else if (userMaxWidth > 0.0f) 
            {
                maxW = userMaxWidth;
            }
            width = maxW;
            if (defaultStyle_.auto_resize)
                setWidth(static_cast<int>(width));
        }
        // If maxHeight == 0, height resizing is disabled; use current height as max
        if (userMaxHeight == 0.0f) 
        {
            height = static_cast<float>(getHeight());
        } 
        else 
        {
            float maxH = 32767.0f;
            if (auto parent = getParent()) 
            {
                float parentH = static_cast<float>(parent->getHeight());
                // compute top edge relative to parent
                float topEdge = static_cast<float>(getY()) - static_cast<float>(parent->getY());
                // If userMaxHeight < 0, use parent's available height
                if (userMaxHeight < 0.0f) 
                {
                    maxH = (parentH > topEdge) ? (parentH - topEdge) : 0.0f;
                } 
                else 
                {
                    float availH = (parentH > topEdge) ? (parentH - topEdge) : 0.0f;
                    maxH = std::min(userMaxHeight, availH);
                }
            } 
            else if (userMaxHeight > 0.0f) 
            {
                maxH = userMaxHeight;
            }
            height = maxH;
            if (defaultStyle_.auto_resize)
                setHeight(static_cast<int>(height));
        }
    } // END Label::_maxSize(float& width, float& height)

    void Label::_buildPhraseAlignLists()
    {
        if (!fontAsset) return;
        IFontObject* font_ = fontAsset.as<IFontObject>();
        if (!font_) return;
        if (!font_->isLoaded()) return;

        // Trim leading spaces from a line of tokens
        auto trim_leading_spaces = [&](std::vector<LabelToken>& line) 
        {
            while (!line.empty() && line.front().type == TokenType::Space) 
            {
                line.erase(line.begin());
            }
        };        
        // trim trailing spaces from a line of tokens and adjust lineWidth
        auto trim_trailing_spaces = [&](std::vector<LabelToken>& line, float& lineWidth, IFontObject* font) 
        {
            while (!line.empty() && line.back().type == TokenType::Space) 
            {
                if (font) lineWidth -= font->getGlyphWidth(' ');
                line.pop_back();
            }
        };
        // Clear existing phraseAlignLists_
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
            auto getLineHeight = [&](const std::vector<LabelToken>& line) -> float 
            {
                float maxH = 0.0f;
                for (const auto& token : line) 
                {
                    // font_->setFontSize(token.style.fontSize);
                    font_->setFontStyle(token.style);
                    int tokenHeight = font_->getGlyphHeight('H');
                    if (tokenHeight > maxH) maxH = tokenHeight;
                }
                return maxH;
            };

            size_t i = 0;
            while (i < tokens.size()) 
            {
                const auto& token = tokens[i];
                bool isVisible = (token.type == TokenType::Word ||
                                token.type == TokenType::Space ||
                                token.type == TokenType::Punctuation ||
                                token.type == TokenType::Tab);

                // font_->setFontSize(token.style.fontSize);
                font_->setFontStyle(token.style);

                // Group word + trailing punctuation for wrapping
                if (token.type == TokenType::Word && i + 1 < tokens.size() && tokens[i+1].type == TokenType::Punctuation) 
                {
                    int wordWidth = font_->getWordWidth(token.text);
                    // font_->setFontSize(tokens[i+1].style.fontSize);
                    font_->setFontStyle(tokens[i+1].style);

                    int punctWidth = font_->getWordWidth(tokens[i+1].text);
                    int clusterWidth = wordWidth + punctWidth;

                    if (maxWidth > 0.0f && currentLineWidth + clusterWidth > maxWidth && !currentLine.empty()) 
                    {
                        // Trim trailing spaces before pushing the line
                        trim_trailing_spaces(currentLine, currentLineWidth, font_);
                        // Trim leading spaces before pushing the line
                        trim_leading_spaces(currentLine);        
                        // If line is empty after trimming, skip adding it                
                        float lineHeight = getLineHeight(currentLine);
                        // Allow the first line to render even if it exceeds maxHeight
                        if (maxHeight > 0.0f && (totalHeight + lineHeight > maxHeight) && !lines.empty())
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
                    trim_trailing_spaces(currentLine, currentLineWidth, font_);
                    // Trim leading spaces before pushing the line
                    trim_leading_spaces(currentLine);    
                    // If line is empty after trimming, skip adding it                    
                    float lineHeight = getLineHeight(currentLine);
                    // Allow the first line to render even if it exceeds maxHeight
                    if (maxHeight > 0.0f && (totalHeight + lineHeight > maxHeight) && !lines.empty())
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
                    trim_trailing_spaces(currentLine, currentLineWidth, font_);
                    // Trim leading spaces before pushing the line
                    trim_leading_spaces(currentLine);             
                    // If line is empty after trimming, skip adding it           
                    float lineHeight = getLineHeight(currentLine);
                    // Allow the first line to render even if it exceeds maxHeight
                    if (maxHeight > 0.0f && (totalHeight + lineHeight > maxHeight) && !lines.empty())
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
                trim_trailing_spaces(currentLine, currentLineWidth, font_);
                // Trim leading spaces before pushing the line
                trim_leading_spaces(currentLine);    
                // If line is empty after trimming, skip adding it              
                float lineHeight = getLineHeight(currentLine);
                if (maxHeight <= 0.0f || (totalHeight + lineHeight <= maxHeight) || lines.empty()) {
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

    bool Label::needsTextureRebuild_(int width, int height, SDL_PixelFormat fmt) const 
    {
        return !cachedTexture_ ||
            current_pixel_format != fmt ||
            current_width != width ||
            current_height != height;
    } // END bool Label::needsTextureRebuild(int width, int height, SDL_PixelFormat fmt) const 

    bool Label::rebuildTexture_(int width, int height, SDL_PixelFormat fmt) 
    {
        // Check if a rebuild is needed
        if (!needsTextureRebuild_(width, height, fmt))
            return true;

        // Always destroy old texture if dimensions/pixel format changed
        if (cachedTexture_) {
            SDL_DestroyTexture(cachedTexture_);
            cachedTexture_ = nullptr;
        }

        // If target size is zero in either dimension, do not create a texture.
        // Just update internal state to avoid thrashing and return success.
        if (width <= 0 || height <= 0) {
            current_pixel_format = fmt;
            current_width = width;
            current_height = height;
            // Do NOT set dirty here; caller will clear it for this frame.
            return true;
        }

        // Create new texture
        cachedTexture_ = SDL_CreateTexture(getRenderer(), fmt, SDL_TEXTUREACCESS_TARGET, width, height);
        if (!cachedTexture_) {
            ERROR("Label::rebuildTexture_() -- Failed to create resized texture: " + std::string(SDL_GetError()));
            return false;
        }
        if (!SDL_SetTextureBlendMode(cachedTexture_, SDL_BLENDMODE_BLEND))
        {
            ERROR("Label::rebuildTexture_() -- Failed to set texture blend mode: " +std::string(SDL_GetError()));
            return false;
        }
        if (!SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND)) 
        {
            ERROR("Label::rebuildTexture_() -- Failed to set render draw blend mode: " + std::string(SDL_GetError()));
            return false;
        }

        // Update current texture info
        current_pixel_format = fmt;
        current_width = width;
        current_height = height;
        setDirty(true);
        return true;

    } // END Label::rebuildTexture_() const 

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