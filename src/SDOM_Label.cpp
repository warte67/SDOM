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
#include <cmath>

// Enable to trace parent resize computations for Labels
static constexpr bool LABEL_PARENT_RESIZE_DEBUG = false;


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

        // defaultStyle_.foregroundColor = init.foregroundColor;
        // defaultStyle_.backgroundColor = init.backgroundColor;
        // defaultStyle_.borderColor = init.borderColor;
        // defaultStyle_.outlineColor = init.outlineColor;
        // defaultStyle_.dropshadowColor = init.dropshadowColor;

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
        // auto read_color = [&](const char* k, SDL_Color d = {255,0,255,255}) -> SDL_Color 
        // {
        //     if (!config[k].valid()) return d;
        //     sol::table t = config[k];
        //     SDL_Color c = d;
        //     if (t["r"].valid()) c.r = (Uint8)t["r"].get<int>();
        //     if (t["g"].valid()) c.g = (Uint8)t["g"].get<int>();
        //     if (t["b"].valid()) c.b = (Uint8)t["b"].get<int>();
        //     if (t["a"].valid()) c.a = (Uint8)t["a"].get<int>();
        //     // array-style [r,g,b,a]
        //     if (!t["r"].valid() && t[1].valid()) 
        //     {
        //         c.r = (Uint8)t[1].get<int>();
        //         if (t[2].valid()) c.g = (Uint8)t[2].get<int>();
        //         if (t[3].valid()) c.b = (Uint8)t[3].get<int>();
        //         if (t[4].valid()) c.a = (Uint8)t[4].get<int>();
        //     }
        //     return c;
        // };

        // --- Initialized Label Fields --- //
        text_ = get_str("text", init.text);
        resourceName_ = get_str("resource_name", get_str("font", init.resourceName) );
        fontType_ = IFontObject::StringToFontType.at(get_str("font_type", IFontObject::FontTypeToString.at(init.fontType)));
        fontSize_ = get_int("font_size", init.fontSize);
        fontWidth_ = get_int("font_width", fontSize_);
        fontHeight_ = get_int("font_height", fontSize_);


        // // Record whether the user provided explicit per-axis values.
        // try { userFontWidthSpecified_ = config["font_width"].valid(); } catch(...) { userFontWidthSpecified_ = false; }
        // try { userFontHeightSpecified_ = config["font_height"].valid(); } catch(...) { userFontHeightSpecified_ = false; }

        // // Normalize width/height -> fallback to fontSize when unspecified/invalid
        // if (fontWidth_ <= 0)  fontWidth_  = (fontSize_ > 0 ? fontSize_ : init.fontWidth);
        // if (fontHeight_ <= 0) fontHeight_ = (fontSize_ > 0 ? fontSize_ : init.fontHeight);

        // Style defaults
        defaultStyle_.alignment = stringToLabelAlign_.at(normalizeAnchorString(get_str("alignment", "top_left")));
        // defaultStyle_.foregroundColor = read_color("foreground_color", defaultStyle_.foregroundColor);
        // defaultStyle_.backgroundColor = read_color("background_color", defaultStyle_.backgroundColor);
        // defaultStyle_.borderColor = read_color("border_color", defaultStyle_.borderColor);
        // defaultStyle_.outlineColor = read_color("outline_color", defaultStyle_.outlineColor);
        // defaultStyle_.dropshadowColor = read_color("dropshadow_color", defaultStyle_.dropshadowColor);

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
        setClickable(get_bool("is_clickable", init.isClickable));
        setTabPriority(get_int("tab_priority", init.tabPriority));
        setTabEnabled(get_bool("tab_enabled", init.tabEnabled));

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

        // Adopt the font's declared type (bitmap or truetype) so downstream
        // rendering logic knows which code path to use. This avoids relying on
        // the Label's default which may not match the referenced asset.
        fontType_ = font->getFontType();

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

        // For TrueType fonts, if the Label didn't explicitly set a fontSize,
        // adopt the IFontObject's font size so measurements/rendering use the
        // font's nominal size.
        if (fontType_ == IFontObject::FontType::Truetype) {
            if (fontSize_ <= 0) {
                fontSize_ = font->getFontSize();
                defaultStyle_.fontSize = fontSize_;
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

        // Capture initial parent dimensions so we can detect changes later
        if (auto parent = getParent()) {
            parent_width_ = parent->getWidth();
            parent_height_ = parent->getHeight();
        } else {
            parent_width_ = 0;
            parent_height_ = 0;
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
        SUPER::onUpdate(fElapsedTime); // Call base class update handler


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
        
        // setDirty(true); // temporary force rebuild every frame

        // Detect parent resize: if the parent's width/height changed since the
        // last frame, we need to re-tokenize and mark dirty so auto_resize and
        // wordwrap can recompute using the new available space.
        if (auto parent = getParent()) 
        {
            int pw = parent->getWidth();
            int ph = parent->getHeight();
            if (pw != parent_width_ || ph != parent_height_) 
            {
                parent_width_ = pw;
                parent_height_ = ph;
                // Only retokenize/rebuild if the label depends on parent sizing
                if (defaultStyle_.auto_resize || defaultStyle_.wordwrap || defaultStyle_.maxWidth < 0) 
                {
                    // Recompute layout constraints and apply auto-resize directly so
                    // height updates are not dependent purely on tokenization ordering.
                    float computedW = 0.0f, computedH = 0.0f;
                    _maxSize(computedW, computedH);
                    if (defaultStyle_.auto_resize) 
                    {
                        int newW = static_cast<int>(std::lround(computedW));
                        int newH = static_cast<int>(std::lround(computedH));
                        if (LABEL_PARENT_RESIZE_DEBUG) 
                        {
                            std::cerr << "[Label DEBUG] '" << name_ << "' parent resized: parent=(" << pw << "," << ph << ") computed=(" << computedW << "," << computedH << ") current=(" << getWidth() << "," << getHeight() << ") -> new=(" << newW << "," << newH << ")" << std::endl;
                        }
                        if (newW != getWidth()) setWidth(newW);
                        if (newH != getHeight()) setHeight(newH);
                    }
                    lastTokenizedText_ = text_;
                    tokenizeText();
                    setDirty(true);
                }
            }
        }

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
        SUPER::onEvent(event); // Call base class event handler

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

        // Stacks to support nested numeric-style escapes (push old value on open, restore on close)
        std::vector<int> borderThicknessStack;
        std::vector<int> outlineThicknessStack;
        std::vector<std::pair<int,int>> paddingStack; // horiz, vert
        std::vector<std::pair<int,int>> dropshadowOffsetStack; // x,y

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
            if (handleEscape(i, 10, "[/outline]", [&]{
                // Toggle outline boolean AND restore outline thickness if we had a numeric override
                currentStyle.outline = false;
                if (!outlineThicknessStack.empty()) {
                    currentStyle.outlineThickness = outlineThicknessStack.back();
                    outlineThicknessStack.pop_back();
                } else {
                    currentStyle.outlineThickness = defaultStyle_.outlineThickness;
                }
            })) continue;

            if (handleEscape(i, 12, "[dropshadow]", [&]{ currentStyle.dropshadow = true; })) continue;
            if (handleEscape(i, 13, "[/dropshadow]", [&]{
                // Toggle dropshadow boolean AND restore numeric offsets if they were overridden
                currentStyle.dropshadow = false;
                if (!dropshadowOffsetStack.empty()) {
                    auto d = dropshadowOffsetStack.back();
                    dropshadowOffsetStack.pop_back();
                    currentStyle.dropshadowOffsetX = d.first;
                    currentStyle.dropshadowOffsetY = d.second;
                } else {
                    currentStyle.dropshadowOffsetX = defaultStyle_.dropshadowOffsetX;
                    currentStyle.dropshadowOffsetY = defaultStyle_.dropshadowOffsetY;
                }
            })) continue;

            if (text_.substr(i, 7) == "[reset]")
            {
                currentStyle = defaultStyle_;
                tokenList.push_back({TokenType::Escape, "[reset]", currentStyle});
                i += 7;
                continue;
            }

            // Handle Escapes/Tags that use an '=' form (color, align, size, numeric tags)
            // This parser tolerates optional spaces around '=' so tags like "[border = 3]"
            // or "[border=    3]" will be accepted.
            if (text_[i] == '[') {
                // scan to find '=' or closing ']' quickly
                size_t j = i + 1;
                while (j < text_.size() && text_[j] != ']' && text_[j] != '=') ++j;
                if (j < text_.size() && text_[j] == '=') {
                    // tagName is text_[i+1 .. j-1]
                    std::string tagName = text_.substr(i + 1, j - (i + 1));
                    // trim spaces from tagName
                    auto trim = [](std::string s){ while(!s.empty() && std::isspace((unsigned char)s.front())) s.erase(s.begin()); while(!s.empty() && std::isspace((unsigned char)s.back())) s.pop_back(); return s; };
                    tagName = to_lower(trim(tagName));
                    size_t tagLenLocal = (j - i) + 1; // include '='
                    size_t endPos = text_.find(']', i + tagLenLocal);
                    if (endPos != std::string::npos) {
                        std::string escapeContent = text_.substr(i + tagLenLocal, endPos - (i + tagLenLocal));
                        // trim content
                        escapeContent = trim(escapeContent);
                        std::string fullEscape = text_.substr(i, endPos - i + 1);

                        // numeric helpers
                        auto is_number_local = [](const std::string& s) {
                            if (s.empty()) return false;
                            size_t start = 0;
                            if (s[0] == '+' || s[0] == '-') start = 1;
                            if (start >= s.size()) return false;
                            return std::all_of(s.begin() + start, s.end(), [](unsigned char c){ return std::isdigit(c); });
                        };

                        // handle border / outline numeric thickness: [border=3], [outline=2]
                        if ((tagName == "border" || tagName == "outline") && !escapeContent.empty()) {
                            if (is_number_local(escapeContent)) {
                                if (tagName == "border") {
                                    borderThicknessStack.push_back(currentStyle.borderThickness);
                                    currentStyle.borderThickness = std::stoi(escapeContent);
                                } else {
                                    outlineThicknessStack.push_back(currentStyle.outlineThickness);
                                    currentStyle.outlineThickness = std::stoi(escapeContent);
                                }
                                tokenList.push_back({TokenType::Escape, fullEscape, currentStyle});
                                i = endPos + 1;
                                continue;
                            }
                        }

                        // color name map
                        std::unordered_map<std::string, SDL_Color> colorIDs = {
                            // grayscale gradient (6 steps)
                            { "black",      SDL_Color{   0,   0,   0, 255 } },
                            { "dk_gray",    SDL_Color{  51,  51,  51, 255 } },
                            { "md_gray",    SDL_Color{ 102, 102, 102, 255 } },
                            { "gray",       SDL_Color{ 153, 153, 153, 255 } },
                            { "lt_gray",    SDL_Color{ 204, 204, 204, 255 } },
                            { "white",      SDL_Color{ 255, 255, 255, 255 } },

                            // light colors:
                            { "lt_red",     SDL_Color{ 255,  32,  32, 255 } },
                            { "lt_green",   SDL_Color{  32, 255,  32, 255 } },
                            { "lt_yellow",  SDL_Color{ 255, 255,  32, 255 } },
                            { "lt_blue",    SDL_Color{  32,  32, 255, 255 } },
                            { "lt_magenta", SDL_Color{ 255,  32, 255, 255 } },
                            { "lt_cyan",    SDL_Color{  32, 255, 255, 255 } },

                            // normal colors:
                            { "red",        SDL_Color{ 176,   0,   0, 255 } },
                            { "green",      SDL_Color{   0, 176,   0, 255 } },
                            { "yellow",     SDL_Color{ 255, 255,   0, 255 } },
                            { "blue",       SDL_Color{   0,   0, 176, 255 } },
                            { "magenta",    SDL_Color{ 176,   0, 176, 255 } },
                            { "cyan",       SDL_Color{   0, 176, 176, 255 } },

                            // dark colors:
                            { "dk_red",     SDL_Color{  96,   0,   0, 255 } },
                            { "dk_green",   SDL_Color{   0,  96,   0, 255 } },
                            { "dk_yellow",  SDL_Color{  96,  96,   0, 255 } },
                            { "dk_blue",    SDL_Color{   0,   0,  96, 255 } },
                            { "dk_magenta", SDL_Color{  96,   0,  96, 255 } },
                            { "dk_cyan",    SDL_Color{   0,  96,  96, 255 } }, 

                            // new colors:
                            { "tan",            SDL_Color{ 255, 204, 153, 255 } }, 
                            { "orange",         SDL_Color{ 255, 128,   0, 255 } },
                            { "brown",          SDL_Color{ 128,  96,   0, 255 } },
                            { "pink",           SDL_Color{ 255, 128, 192, 255 } },
                            { "purple",         SDL_Color{ 128,   0, 128, 255 } },
                            { "violet",         SDL_Color{ 165,  42, 255, 255 } },
                            { "lavender",       SDL_Color{ 230, 230, 250, 255 } },
                            { "gold",           SDL_Color{ 255, 215,   0, 255 } },
                            { "silver",         SDL_Color{ 192, 192, 192, 255 } },
                            { "bronze",         SDL_Color{ 205, 127,  50, 255 } },
                            { "lime",           SDL_Color{ 128, 255,   0, 255 } },
                            { "mint",           SDL_Color{ 170, 255, 195, 255 } },
                            { "dk_olive",       SDL_Color{  64,  64,   0, 255 } },
                            { "olive",          SDL_Color{  96,  96,   0, 255 } },
                            { "olive_drab",     SDL_Color{ 107, 142,  35, 255 } },
                            { "sea_green",      SDL_Color{  46, 139,  87, 255 } },
                            { "forest_green",   SDL_Color{  34, 139,  34, 255 } },
                            { "sky_blue",       SDL_Color{ 135, 206, 235, 255 } },
                            { "royal_blue",     SDL_Color{  65, 105, 225, 255 } },
                            { "steel_blue",     SDL_Color{  70, 130, 180, 255 } },
                            { "midnight_blue",  SDL_Color{  25,  25, 112, 255 } },
                            { "crimson",        SDL_Color{ 220,  20,  60, 255 } },
                            { "maroon",         SDL_Color{ 128,   0,   0, 255 } },
                            { "coral",          SDL_Color{ 255, 127,  80, 255 } },
                            { "salmon",         SDL_Color{ 250, 128, 114, 255 } },
                            { "khaki",          SDL_Color{ 240, 230, 140, 255 } },
                            { "beige",          SDL_Color{ 245, 245, 220, 255 } },
                            { "wheat",          SDL_Color{ 245, 222, 179, 255 } },
                            { "chocolate",      SDL_Color{ 210, 105,  30, 255 } },
                            { "sienna",         SDL_Color{ 160,  82,  45, 255 } },
                            { "peru",           SDL_Color{ 205, 133,  63, 255 } },
                            { "indigo",         SDL_Color{  75,   0, 130, 255 } },
                            { "turquoise",      SDL_Color{  64, 224, 208, 255 } },
                            { "aquamarine",     SDL_Color{ 127, 255, 212, 255 } },
                            { "chartreuse",     SDL_Color{ 127, 255,   0, 255 } },
                            { "teal",           SDL_Color{   0, 128, 128, 255 } },
                            { "navy",           SDL_Color{   0,   0, 128, 255 } }

                        };

                        std::unordered_map<std::string, SDL_Color*> colorTargets = {
                            { "fgnd",       &foregroundColor_ },
                            { "bgnd",       &backgroundColor_ },
                            { "border",     &borderColor_ },
                            { "outline",    &outlineColor_ },
                            { "shadow",     &dropshadowColor_ }
                        };

                        // [pad=WxH] or [padding=WxH]
                        if ((tagName == "pad" || tagName == "padding") && !escapeContent.empty()) {
                            auto findSep = [&](const std::string& s)->size_t {
                                size_t p = s.find('x'); if (p != std::string::npos) return p;
                                p = s.find('X'); if (p != std::string::npos) return p;
                                p = s.find(','); return p;
                            };
                            size_t sep = findSep(escapeContent);
                            if (sep != std::string::npos) {
                                std::string a = escapeContent.substr(0, sep);
                                std::string b = escapeContent.substr(sep+1);
                                // trim already applied to escapeContent, but trim parts as well
                                auto trim_local = [](std::string s){ while(!s.empty() && std::isspace((unsigned char)s.front())) s.erase(s.begin()); while(!s.empty() && std::isspace((unsigned char)s.back())) s.pop_back(); return s; };
                                a = trim_local(a); b = trim_local(b);
                                // reuse is_number_local
                                if (is_number_local(a) && is_number_local(b)) {
                                    paddingStack.push_back({currentStyle.padding_horiz, currentStyle.padding_vert});
                                    currentStyle.padding_horiz = std::stoi(a);
                                    currentStyle.padding_vert = std::stoi(b);
                                    tokenList.push_back({TokenType::Escape, fullEscape, currentStyle});
                                    i = endPos + 1;
                                    continue;
                                }
                            }
                        }

                        // [dropshadow=X,Y]
                        if (tagName == "dropshadow" && !escapeContent.empty()) {
                            size_t comma = escapeContent.find(',');
                            if (comma != std::string::npos) {
                                std::string a = escapeContent.substr(0, comma);
                                std::string b = escapeContent.substr(comma+1);
                                auto trim_local = [](std::string s){ while(!s.empty() && std::isspace((unsigned char)s.front())) s.erase(s.begin()); while(!s.empty() && std::isspace((unsigned char)s.back())) s.pop_back(); return s; };
                                a = trim_local(a); b = trim_local(b);
                                if (is_number_local(a) && is_number_local(b)) {
                                    dropshadowOffsetStack.push_back({currentStyle.dropshadowOffsetX, currentStyle.dropshadowOffsetY});
                                    currentStyle.dropshadowOffsetX = std::stoi(a);
                                    currentStyle.dropshadowOffsetY = std::stoi(b);
                                    tokenList.push_back({TokenType::Escape, fullEscape, currentStyle});
                                    i = endPos + 1;
                                    continue;
                                }
                            }
                        }

                        // color/fgnd/bgnd/outline/shadow/ color hex parsing and explicit rgb/rgba target handling
                        std::string colorValue = to_lower(escapeContent);
                        SDL_Color color = {0,0,0,255};
                        bool validColor = false;
                        auto colorIt = colorIDs.find(colorValue);
                        if (colorIt != colorIDs.end()) {
                            color = colorIt->second; validColor = true;
                        } else if (colorValue.length() == 6 || colorValue.length() == 8) {
                            auto is_hexstr = [](const std::string& s) { return std::all_of(s.begin(), s.end(), [](unsigned char c){ return std::isxdigit(c); }); };
                            if (is_hexstr(colorValue)) {
                                color.r = std::stoi(colorValue.substr(0,2), nullptr, 16);
                                color.g = std::stoi(colorValue.substr(2,2), nullptr, 16);
                                color.b = std::stoi(colorValue.substr(4,2), nullptr, 16);
                                if (colorValue.length() == 8) color.a = std::stoi(colorValue.substr(6,2), nullptr, 16);
                                else color.a = 255;
                                validColor = true;
                            }
                        }

                        if (validColor) {
                            std::string targetName = "fgnd";
                            if (tagName == "fgnd") targetName = "fgnd";
                            else if (tagName == "bgnd") targetName = "bgnd";
                            else if (tagName == "border") targetName = "border";
                            else if (tagName == "outline") targetName = "outline";
                            else if (tagName == "shadow") targetName = "shadow";
                            auto targetIt = colorTargets.find(targetName);
                            if (targetIt != colorTargets.end()) *(targetIt->second) = color;
                        }

                        if (tagName == "rgb" || tagName == "rgba") {
                            size_t colonPos = escapeContent.find(':');
                            if (colonPos != std::string::npos) {
                                std::string hex = escapeContent.substr(0, colonPos);
                                std::string explicitTarget = to_lower(trim(escapeContent.substr(colonPos + 1)));
                                SDL_Color color2 = {0,0,0,255};
                                bool valid2 = false;
                                auto is_hexstr = [](const std::string& s) { return std::all_of(s.begin(), s.end(), [](unsigned char c){ return std::isxdigit(c); }); };
                                if ((hex.length() == 6 || hex.length() == 8) && is_hexstr(hex)) {
                                    color2.r = std::stoi(hex.substr(0,2), nullptr, 16);
                                    color2.g = std::stoi(hex.substr(2,2), nullptr, 16);
                                    color2.b = std::stoi(hex.substr(4,2), nullptr, 16);
                                    if (hex.length() == 8) color2.a = std::stoi(hex.substr(6,2), nullptr, 16);
                                    else color2.a = 255;
                                    valid2 = true;
                                }
                                if (valid2) {
                                    auto targetIt = colorTargets.find(explicitTarget);
                                    if (targetIt != colorTargets.end()) *(targetIt->second) = color2;
                                }
                            }
                        }

                        if (tagName == "align") {
                            std::string alignValue = to_lower(escapeContent);
                            auto it = stringToLabelAlign_.find(alignValue);
                            if (it != stringToLabelAlign_.end()) currentStyle.alignment = it->second;
                            tokenList.push_back({TokenType::Escape, fullEscape, currentStyle});
                            i = endPos + 1;
                            continue;
                        }

                        if (tagName == "size") {
                            try {
                                int newSize = std::stoi(escapeContent);
                                if (newSize > 0) { fontSize_ = newSize; currentStyle.fontSize = newSize; }
                            } catch(...) {}
                            tokenList.push_back({TokenType::Escape, fullEscape, currentStyle});
                            i = endPos + 1;
                            continue;
                        }

                        // Update currentStyle colors from colorTargets
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
            }

            // // Handle Numeric-style Escapes: [border=N], [/border], [outline=N], [/outline],
            // // [pad=WxH]/[padding=WxH], [/pad], [/padding], [dropshadow=X,Y], [/dropshadow]
            // auto is_number = [](const std::string& s) {
            //     if (s.empty()) return false;
            //     size_t start = 0;
            //     if (s[0] == '+' || s[0] == '-') start = 1;
            //     if (start >= s.size()) return false;
            //     return std::all_of(s.begin() + start, s.end(), [](unsigned char c){ return std::isdigit(c); });
            // };

            // Closing tags: restore from stacks
            if (handleEscape(i, 9, "[/border]", [&]{
                if (!borderThicknessStack.empty()) { currentStyle.borderThickness = borderThicknessStack.back(); borderThicknessStack.pop_back(); }
                else currentStyle.borderThickness = defaultStyle_.borderThickness;
            })) continue;
            if (handleEscape(i, 10, "[/outline]", [&]{
                if (!outlineThicknessStack.empty()) { currentStyle.outlineThickness = outlineThicknessStack.back(); outlineThicknessStack.pop_back(); }
                else currentStyle.outlineThickness = defaultStyle_.outlineThickness;
            })) continue;
            if (handleEscape(i, 6, "[/pad]", [&]{
                if (!paddingStack.empty()) { auto p = paddingStack.back(); paddingStack.pop_back(); currentStyle.padding_horiz = p.first; currentStyle.padding_vert = p.second; }
                else { currentStyle.padding_horiz = defaultStyle_.padding_horiz; currentStyle.padding_vert = defaultStyle_.padding_vert; }
            })) continue;
            if (handleEscape(i, 11, "[/padding]", [&]{
                if (!paddingStack.empty()) { auto p = paddingStack.back(); paddingStack.pop_back(); currentStyle.padding_horiz = p.first; currentStyle.padding_vert = p.second; }
                else { currentStyle.padding_horiz = defaultStyle_.padding_horiz; currentStyle.padding_vert = defaultStyle_.padding_vert; }
            })) continue;
            if (handleEscape(i, 12, "[/dropshadow]", [&]{
                if (!dropshadowOffsetStack.empty()) { auto d = dropshadowOffsetStack.back(); dropshadowOffsetStack.pop_back(); currentStyle.dropshadowOffsetX = d.first; currentStyle.dropshadowOffsetY = d.second; }
                else { currentStyle.dropshadowOffsetX = defaultStyle_.dropshadowOffsetX; currentStyle.dropshadowOffsetY = defaultStyle_.dropshadowOffsetY; }
            })) continue;

            

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
        // If auto_resize is disabled, the Label should not consult parent
        // dimensions — it stays fixed to its own bounds. Return early.
        if (!defaultStyle_.auto_resize) {
            width = static_cast<float>(getWidth());
            height = static_cast<float>(getHeight());
            return;
        }

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
                // float labelW = static_cast<float>(getWidth());
                float parentW = static_cast<float>(parent->getWidth());
                // compute left edge relative to parent (avoid mixing world and local coords)
                float leftEdge = static_cast<float>(getX()) - static_cast<float>(parent->getX());
                // Do not clamp parentW to labelW here — clamping caused progressive shrink when
                // the label kept reducing size. Use the parent's full width to compute available
                // space from the left edge.
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
            if (defaultStyle_.auto_resize) {
                // Round to nearest integer to avoid fractional drift toggling size every frame.
                int newW = static_cast<int>(std::lround(width));
                if (newW != getWidth())
                    setWidth(newW);
            }
        }
        // If maxHeight == 0, height resizing is disabled by default; however,
        // when auto_resize is enabled we want the label to be able to resize
        // vertically to the parent's available height (symmetry with width).
        if (userMaxHeight == 0.0f)
        {
            // If auto_resize is disabled, respect the Label's own height and do
            // not consider the parent's available height — the Label should be
            // fixed to its configured bounds.
            if (!defaultStyle_.auto_resize) {
                height = static_cast<float>(getHeight());
            }
            else if (getParent()) {
                // compute parent's available height from label's top edge
                float parentH = static_cast<float>(getParent()->getHeight());
                float topEdge = static_cast<float>(getY()) - static_cast<float>(getParent()->getY());
                float availH = (parentH > topEdge) ? (parentH - topEdge) : 0.0f;
                height = availH;
                int newH = static_cast<int>(std::lround(height));
                if (newH != getHeight())
                    setHeight(newH);
            } else {
                height = static_cast<float>(getHeight());
            }
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
            if (defaultStyle_.auto_resize) {
                // Round to nearest integer to avoid fractional drift toggling size every frame.
                int newH = static_cast<int>(std::lround(height));
                if (newH != getHeight())
                    setHeight(newH);
            }
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

        // Augment the single shared AssetHandle handle usertype (assets are exposed via AssetHandle handles in Lua)
        sol::table handle = DisplayHandle::ensure_handle_table(lua);

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
            // Helper to check if a property/command is already registered (reuse pattern from IDisplayObject)
            auto absent = [&](const char* name) -> bool {
                sol::object cur = handle.raw_get_or(name, sol::lua_nil);
                return !cur.valid() || cur == sol::lua_nil;
            };

            // small helper to validate and cast the DisplayHandle -> Label*
            auto cast_label_from_handle = [](DisplayHandle& h) -> Label* {
                if (!h.isValid()) { ERROR("invalid DisplayHandle provided to Label method"); }
                IDisplayObject* base = dynamic_cast<IDisplayObject*>(h.get());
                Label* label = dynamic_cast<Label*>(base);
                if (!label) { ERROR("invalid Label object"); }
                return label;
            };

            // Register Label inspection accessors on the DisplayHandle shared table
            if (absent("getTokenList")) {
                handle.set_function("getTokenList", [cast_label_from_handle](DisplayHandle& self) -> sol::table {
                    Label* lbl = cast_label_from_handle(self);
                    sol::state_view sv = SDOM::Core::getInstance().getLua();
                    sol::table t = sv.create_table();
                    const auto& list = lbl->getTokenList();
                    int idx = 1;
                    for (const auto& tk : list) {
                        sol::table e = sv.create_table();
                        // expose type as string for readability
                        std::string typeStr;
                        switch (tk.type) {
                            case Label::TokenType::Word: typeStr = "word"; break;
                            case Label::TokenType::Escape: typeStr = "escape"; break;
                            case Label::TokenType::Space: typeStr = "space"; break;
                            case Label::TokenType::Punctuation: typeStr = "punct"; break;
                            case Label::TokenType::Newline: typeStr = "newline"; break;
                            case Label::TokenType::Tab: typeStr = "tab"; break;
                            default: typeStr = "unknown"; break;
                        }
                        e["type"] = typeStr;
                        e["text"] = tk.text;
                        // expose full FontStyle to Lua so tests can assert style flags
                        sol::table style = sv.create_table();
                        // boolean style flags
                        style["bold"] = tk.style.bold;
                        style["italic"] = tk.style.italic;
                        style["underline"] = tk.style.underline;
                        style["strikethrough"] = tk.style.strikethrough;
                        style["border"] = tk.style.border;
                        style["background"] = tk.style.background;
                        style["outline"] = tk.style.outline;
                        style["dropshadow"] = tk.style.dropshadow;
                        // sizing and layout
                        style["font_size"] = tk.style.fontSize;
                        style["font_width"] = tk.style.fontWidth;
                        style["font_height"] = tk.style.fontHeight;
                        style["wordwrap"] = tk.style.wordwrap;
                        style["auto_resize"] = tk.style.auto_resize;
                        style["max_width"] = tk.style.maxWidth;
                        style["max_height"] = tk.style.maxHeight;
                        style["border_thickness"] = tk.style.borderThickness;
                        style["outline_thickness"] = tk.style.outlineThickness;
                        style["padding_horiz"] = tk.style.padding_horiz;
                        style["padding_vert"] = tk.style.padding_vert;
                        style["dropshadow_offset_x"] = tk.style.dropshadowOffsetX;
                        style["dropshadow_offset_y"] = tk.style.dropshadowOffsetY;
                        style["align"] = Label::labelAlignToString_.at(tk.style.alignment);
                        // colors as subtables
                        sol::table fg = sv.create_table(); fg["r"] = tk.style.foregroundColor.r; fg["g"] = tk.style.foregroundColor.g; fg["b"] = tk.style.foregroundColor.b; fg["a"] = tk.style.foregroundColor.a; style["foreground_color"] = fg;
                        sol::table bg = sv.create_table(); bg["r"] = tk.style.backgroundColor.r; bg["g"] = tk.style.backgroundColor.g; bg["b"] = tk.style.backgroundColor.b; bg["a"] = tk.style.backgroundColor.a; style["background_color"] = bg;
                        sol::table bord = sv.create_table(); bord["r"] = tk.style.borderColor.r; bord["g"] = tk.style.borderColor.g; bord["b"] = tk.style.borderColor.b; bord["a"] = tk.style.borderColor.a; style["border_color"] = bord;
                        sol::table outl = sv.create_table(); outl["r"] = tk.style.outlineColor.r; outl["g"] = tk.style.outlineColor.g; outl["b"] = tk.style.outlineColor.b; outl["a"] = tk.style.outlineColor.a; style["outline_color"] = outl;
                        sol::table ds = sv.create_table(); ds["r"] = tk.style.dropshadowColor.r; ds["g"] = tk.style.dropshadowColor.g; ds["b"] = tk.style.dropshadowColor.b; ds["a"] = tk.style.dropshadowColor.a; style["dropshadow_color"] = ds;
                        e["style"] = style;
                        t[idx++] = e;
                    }
                    return t;
                });
            }

            if (absent("getLastTokenizedText")) {
                handle.set_function("getLastTokenizedText", [cast_label_from_handle](DisplayHandle& self) -> std::string {
                    Label* lbl = cast_label_from_handle(self);
                    return lbl->getLastTokenizedText();
                });
            }

            // Expose tokenizeText() so tests or scripts can force re-tokenization and get the token count
            if (absent("tokenizeText")) {
                handle.set_function("tokenizeText", [cast_label_from_handle](DisplayHandle& self) -> int {
                    Label* lbl = cast_label_from_handle(self);
                    return lbl->tokenizeText();
                });
            }

            if (absent("getTokenAlignLists")) {
                handle.set_function("getTokenAlignLists", [cast_label_from_handle](DisplayHandle& self) -> sol::table {
                    Label* lbl = cast_label_from_handle(self);
                    sol::state_view sv = SDOM::Core::getInstance().getLua();
                    sol::table out = sv.create_table();
                    const auto& m = lbl->getTokenAlignLists();
                    int outer = 1;
                    for (const auto& [align, vec] : m) {
                        sol::table inner = sv.create_table();
                        int i = 1;
                        for (const auto& tk : vec) {
                            sol::table e = sv.create_table();
                            std::string typeStr;
                            switch (tk.type) {
                                case Label::TokenType::Word: typeStr = "word"; break;
                                case Label::TokenType::Escape: typeStr = "escape"; break;
                                case Label::TokenType::Space: typeStr = "space"; break;
                                case Label::TokenType::Punctuation: typeStr = "punct"; break;
                                case Label::TokenType::Newline: typeStr = "newline"; break;
                                case Label::TokenType::Tab: typeStr = "tab"; break;
                                default: typeStr = "unknown"; break;
                            }
                            e["type"] = typeStr;
                            e["text"] = tk.text;
                            inner[i++] = e;
                        }
                        out[outer++] = inner;
                    }
                    return out;
                });
            }

            if (absent("getPhraseAlignLists")) {
                handle.set_function("getPhraseAlignLists", [cast_label_from_handle](DisplayHandle& self) -> sol::table {
                    Label* lbl = cast_label_from_handle(self);
                    sol::state_view sv = SDOM::Core::getInstance().getLua();
                    sol::table out = sv.create_table();
                    const auto& m = lbl->getPhraseAlignLists();
                    int outer = 1;
                    for (const auto& [align, vec] : m) {
                        sol::table inner = sv.create_table();
                        int i = 1;
                        for (const auto& ph : vec) {
                            sol::table e = sv.create_table();
                            e["text"] = ph.text;
                            e["lineIndex"] = ph.lineIndex;
                            e["startX"] = ph.startX;
                            e["lineY"] = ph.lineY;
                            e["width"] = ph.width;
                            e["height"] = ph.height;
                            inner[i++] = e;
                        }
                        out[outer++] = inner;
                    }
                    return out;
                });
            }

    } // END Label::_registerLuaBindings(const std::string& typeName, sol::state_view lua)

} // END namespace SDOM