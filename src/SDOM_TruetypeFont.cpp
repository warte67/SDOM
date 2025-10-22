// SDOM_TruetypeFont.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <SDOM/SDOM_TTFAsset.hpp>
#include <SDOM/SDOM_TruetypeFont.hpp>

namespace SDOM
{
    TruetypeFont::TruetypeFont(const InitStruct& init) : IFontObject(init)
    {    
    } // END: TruetypeFont(const InitStruct& init) : IFontObject(init)

    TruetypeFont::TruetypeFont(const sol::table& config) : IFontObject(config)
    {
    } // END: TruetypeFont(const sol::table& config) : IFontObject(config)

    TruetypeFont::~TruetypeFont()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "~BitmapFont()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Destructor implementation if needed
        // ...
    } // END:  TruetypeFont::~TruetypeFont()


    // --- Override methods from IFontObject --- //

    bool TruetypeFont::onInit()
    {
        // Allow two forms:
        // 1) filename_ is the name of a pre-existing TTFAsset
        // 2) filename_ is a path to a .ttf file: create a backing TTFAsset automatically
        ttf_font_handle_ = getFactory().getAssetObject(filename_);
        if (!ttf_font_handle_.isValid()) {
            // attempt to interpret filename_ as a .ttf path
            std::string lower = filename_;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            bool looksLikePath = (lower.find(".ttf") != std::string::npos);
            if (looksLikePath) {
                // build a synthetic asset name
                std::string assetName = name_.empty() ? std::string("truetype_font_asset") : (name_ + std::string("_TTFAsset"));
                // create TTFAsset init
                TTFAsset::InitStruct init;
                init.name = assetName;
                init.type = TTFAsset::TypeName;
                init.filename = filename_;
                // use currently configured font size if available
                init.internalFontSize = (fontSize_ > 0 ? fontSize_ : 10);
                std::cout << "TruetypeFont::onInit -> creating TTFAsset: name='" << init.name << "' filename='" << init.filename << "' size=" << init.internalFontSize << "\n";
                AssetHandle h = getFactory().createAsset(TTFAsset::TypeName, init);
                if (h.isValid()) {
                    ttf_font_handle_ = h;
                    // Switch our filename_ to reference the asset by name to match later lookups
                    filename_ = assetName;
                }
            }
        }
        if (!ttf_font_handle_.isValid()) {
            ERROR("TruetypeFont: TTFAsset '" + filename_ + "' not found/created in Factory!");
            return false;
        }
        return true;
    } // END: TruetypeFont::onInit() 

    void TruetypeFont::onQuit() 
    {
        if (ttf_font_handle_.isValid()) {
            ttf_font_handle_.get()->onQuit();
        }
    } // END: TruetypeFont::onQuit() // END: TruetypeFont::onQuit() 

    void TruetypeFont::onLoad() 
    {
        if (ttf_font_handle_.isValid()) {
            // Forward load to underlying TTFAsset
            ttf_font_handle_.get()->onLoad();

            // If the underlying asset reports loaded, mark this font as loaded too
            TTFAsset* ttfAsset = ttf_font_handle_->as<TTFAsset>();
            if (ttfAsset && ttfAsset->isLoaded()) {
                isLoaded_ = true;
                int loadedSize = ttfAsset->getFontSize();
                if (loadedSize > 0) fontSize_ = loadedSize;
            } else {
                isLoaded_ = false;
            }
        }
    } // END: TruetypeFont::onLoad() 

    void TruetypeFont::onUnload() 
    {
        if (ttf_font_handle_.isValid()) {
            ttf_font_handle_.get()->onUnload();
        }
    } // END: TruetypeFont::onUnload() 

    void TruetypeFont::create(const sol::table& config) 
    {
        // Update basic identity/config from Lua table
        if (config["name"].valid())     name_ = config["name"].get<std::string>();
        if (config["filename"].valid()) filename_ = config["filename"].get<std::string>();

        // Font size (accept common variants)
        if (config["fontSize"].valid())         fontSize_ = config["fontSize"].get<int>();
        else if (config["size"].valid())        fontSize_ = config["size"].get<int>();
        else if (config["font_size"].valid())   fontSize_ = config["font_size"].get<int>();

        // Optional explicit TTF asset name; default to "<name>_TTFAsset"
        std::string ttfAssetName;
        if (config["ttfAssetName"].valid()) ttfAssetName = config["ttfAssetName"].get<std::string>();
        else ttfAssetName = name_.empty() ? std::string("truetype_font_asset") : (name_ + "_TTFAsset");

        // If we already hold a TTF asset, unload and drop it so create/rebind proceeds cleanly.
        if (ttf_font_handle_.isValid())
        {
            IAssetObject* asset = ttf_font_handle_.get();
            if (asset && asset->isLoaded()) asset->onUnload();
            ttf_font_handle_.reset();
        }

        // Try to reuse an existing factory asset with that name, else create a new TTFAsset.
        AssetHandle existing = getFactory().getAssetObject(ttfAssetName);
        if (existing.isValid())
        {
            ttf_font_handle_ = existing;
        }
        else
        {
            // If filename_ looks like a path, create new TTFAsset from file.
            std::string lower = filename_;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            bool looksLikePath = (lower.find(".ttf") != std::string::npos);
            TTFAsset::InitStruct init;
            init.name = ttfAssetName;
            init.type = TTFAsset::TypeName;
            init.filename = looksLikePath ? filename_ : ttfAssetName;
            init.internalFontSize = fontSize_;

            ttf_font_handle_ = getFactory().createAsset(TTFAsset::TypeName, init);
            if (!ttf_font_handle_)
            {
                ERROR("TruetypeFont::create - failed to create TTFAsset: " + ttfAssetName);
                return;
            }
        }

        // Ensure the TTF asset is loaded
        TTFAsset* ttfAsset = dynamic_cast<TTFAsset*>(ttf_font_handle_.get());
        if (!ttfAsset)
        {
            ERROR("TruetypeFont::create - resolved asset is not a TTFAsset: " + ttfAssetName);
            ttf_font_handle_.reset();
            return;
        }
        if (!ttfAsset->isLoaded()) ttfAsset->onLoad();

        // Sync font size if available
        int loadedSize = ttfAsset->getFontSize();
        if (loadedSize > 0) fontSize_ = loadedSize;

        // mark this font as configured/loaded
        isLoaded_ = true;
    } // END create()

    void TruetypeFont::drawGlyph(Uint32 ch, int x, int y, const FontStyle& style) 
    {
        if (!ttf_font_handle_.isValid())   
        {
            DEBUG_LOG("TruetypeFont::drawGlyph - invalid TTF asset handle");
            return;
        }
        TTFAsset* ttfAsset = ttf_font_handle_->as<TTFAsset>();
        if (!ttfAsset || !ttfAsset->isLoaded()) 
        {
            DEBUG_LOG("TruetypeFont::drawGlyph - TTF asset not loaded: " + filename_);
            return;
        }
        TTF_Font* ttf_font_ = ttfAsset->_getTTFFontPtr();
        if (!ttf_font_) 
        {
            DEBUG_LOG("TruetypeFont::drawGlyph - TTF font pointer is null: " + filename_);
            return;
        }

        // Lambda for rendering a glyph with a given color and outline
        auto render_glyph = [&](SDL_Color drawColor, float px, float py, int outline) 
        {
            TTF_SetFontOutline(ttf_font_, outline);
            SDL_Surface* glyphSurface = TTF_RenderGlyph_Blended(ttf_font_, ch, drawColor);
            if (glyphSurface)
            {
                SDL_Renderer* renderer = getRenderer();
                if (renderer)
                {
                    SDL_Texture* glyphTexture = SDL_CreateTextureFromSurface(renderer, glyphSurface);
                    if (glyphTexture)
                    {
                        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                        SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
                        SDL_FRect destRect = { (float)px, (float)py, (float)glyphSurface->w, (float)glyphSurface->h };
                        if (!SDL_RenderTexture(renderer, glyphTexture, nullptr, &destRect))
                        {
                            ERROR(std::string("Failed to render glyph texture: ") + SDL_GetError());
                        }
                        SDL_DestroyTexture(glyphTexture);
                    }
                }
                SDL_DestroySurface(glyphSurface);
            }
        };

        // Render glyph using TTF font
        if (ttf_font_)
        {
            if (TTF_FontHasGlyph(ttf_font_, ch))
            {
                // Render glyph using TTF font
                if (!ttf_font_) return;
                if (!TTF_FontHasGlyph(ttf_font_, ch)) return;

                // Build style bitmask
                int styleMask = TTF_STYLE_NORMAL;
                if (style.bold) styleMask |= TTF_STYLE_BOLD;
                if (style.italic) styleMask |= TTF_STYLE_ITALIC;
                if (style.underline) styleMask |= TTF_STYLE_UNDERLINE;
                if (style.strikethrough) styleMask |= TTF_STYLE_STRIKETHROUGH;
                TTF_SetFontStyle(ttf_font_, styleMask);

                // Drop shadow pass (if enabled)
                if (style.dropshadow) 
                {
                    render_glyph(style.dropshadowColor, x+style.dropshadowOffsetX, y+style.dropshadowOffsetY, style.outlineThickness);
                    render_glyph(style.dropshadowColor, x+style.dropshadowOffsetX, y+style.dropshadowOffsetY, 0);
                }

                // Outline pass (if enabled)
                if (style.outline) {
                    render_glyph(style.outlineColor, x-style.outlineThickness, y-style.outlineThickness, style.outlineThickness);
                }

                // Foreground pass (always)
                render_glyph(style.foregroundColor, x, y, 0);
            }
        }
    } // END: TruetypeFont::drawGlyph(Uint32 ch, int x, int y, const FontStyle& style) 

    void TruetypeFont::drawPhrase(const std::string& str, int x, int y, const FontStyle& style) 
    {
        TTF_Font* ttf_font_ = _getValidTTFFontPtr();
        if (!ttf_font_ || str.empty()) return;
        // Set font style
        int styleMask = TTF_STYLE_NORMAL;
        if (style.bold) styleMask |= TTF_STYLE_BOLD;
        if (style.italic) styleMask |= TTF_STYLE_ITALIC;
        if (style.underline) styleMask |= TTF_STYLE_UNDERLINE;
        if (style.strikethrough) styleMask |= TTF_STYLE_STRIKETHROUGH;
        TTF_SetFontStyle(ttf_font_, styleMask);
        // Lambda for rendering a phrase with a given color and outline
        auto render_phrase = [&](SDL_Color drawColor, int px, int py, int outline) 
        {
            TTF_SetFontOutline(ttf_font_, outline);
            SDL_Surface* textSurface = TTF_RenderText_Blended(ttf_font_, str.c_str(), str.length(), drawColor);
            if (textSurface) 
            {
                SDL_Renderer* renderer = getRenderer();
                if (renderer) 
                {
                    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    if (textTexture) 
                    {
                        SDL_FRect destRect = { (float)px, (float)py, (float)textSurface->w, (float)textSurface->h };
                        // SDL_SetTextureAlphaMod(textTexture, drawColor.a);
                        SDL_RenderTexture(renderer, textTexture, nullptr, &destRect);
                        SDL_DestroyTexture(textTexture);
                    }
                }
                SDL_DestroySurface(textSurface);
            }
        };
        // Foreground pass (always)
        render_phrase(style.foregroundColor, x, y, 0);
    } // END: TruetypeFont::drawPhrase(const std::string& str, int x, int y, const FontStyle& style) 

    void TruetypeFont::drawPhraseOutline(const std::string& str, int x, int y, const FontStyle& style) 
    {
        TTF_Font* ttf_font_ = _getValidTTFFontPtr();
        if (!ttf_font_ || str.empty()) return;
        if (!style.outline && style.outlineThickness < 1) return;

        // Set font style
        int styleMask = TTF_STYLE_NORMAL;
        if (style.bold) styleMask |= TTF_STYLE_BOLD;
        if (style.italic) styleMask |= TTF_STYLE_ITALIC;
        if (style.underline) styleMask |= TTF_STYLE_UNDERLINE;
        if (style.strikethrough) styleMask |= TTF_STYLE_STRIKETHROUGH;
        TTF_SetFontStyle(ttf_font_, styleMask);
        // Lambda for rendering a phrase with a given color and outline
        auto render_phrase = [&](SDL_Color drawColor, int px, int py, int outline) 
        {
            TTF_SetFontOutline(ttf_font_, outline);
            SDL_Surface* textSurface = TTF_RenderText_Blended(ttf_font_, str.c_str(), str.length(), drawColor);
            if (textSurface) 
            {
                SDL_Renderer* renderer = getRenderer();
                if (renderer) 
                {
                    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    if (textTexture) 
                    {
                        SDL_FRect destRect = { (float)px, (float)py, (float)textSurface->w, (float)textSurface->h };
                        SDL_RenderTexture(renderer, textTexture, nullptr, &destRect);
                        SDL_DestroyTexture(textTexture);
                    }
                }
                SDL_DestroySurface(textSurface);
            }
        };
        // Outline pass
        render_phrase(style.outlineColor, x - style.outlineThickness, y - style.outlineThickness, style.outlineThickness);
    } // END: TruetypeFont::drawPhraseOutline(const std::string& str, int x, int y, const FontStyle& style) 

    void TruetypeFont::drawPhraseDropshadow(const std::string& str, int x, int y, const FontStyle& style) 
    {
        TTF_Font* ttf_font_ = _getValidTTFFontPtr();
        if (!ttf_font_ || str.empty()) return;

        if (!style.dropshadow)
            return;

        // Set font style
        int styleMask = TTF_STYLE_NORMAL;
        if (style.bold) styleMask |= TTF_STYLE_BOLD;
        if (style.italic) styleMask |= TTF_STYLE_ITALIC;
        if (style.underline) styleMask |= TTF_STYLE_UNDERLINE;
        if (style.strikethrough) styleMask |= TTF_STYLE_STRIKETHROUGH;
        TTF_SetFontStyle(ttf_font_, styleMask);
        // Lambda for rendering a phrase with a given color and outline
        auto render_phrase = [&](SDL_Color drawColor, int px, int py, int outline) 
        {
            TTF_SetFontOutline(ttf_font_, outline);
            SDL_Surface* textSurface = TTF_RenderText_Blended(ttf_font_, str.c_str(), str.length(), drawColor);
            if (textSurface) 
            {
                SDL_Renderer* renderer = getRenderer();
                if (renderer) 
                {
                    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    if (textTexture) 
                    {
                        SDL_FRect destRect = { (float)px, (float)py, (float)textSurface->w, (float)textSurface->h };
                        SDL_RenderTexture(renderer, textTexture, nullptr, &destRect);
                        SDL_DestroyTexture(textTexture);
                    }
                }
                SDL_DestroySurface(textSurface);
            }
        };
        // Drop shadow pass
        render_phrase(style.dropshadowColor, x + style.dropshadowOffsetX, y + style.dropshadowOffsetY, style.outlineThickness);
        render_phrase(style.dropshadowColor, x + style.dropshadowOffsetX, y + style.dropshadowOffsetY, 0);
    } // END: TruetypeFont::drawPhraseDropshadow(const std::string& str, int x, int y, const FontStyle& style) 

    bool TruetypeFont::getGlyphMetrics(Uint32 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance) const 
    {
        // Get glyph metrics
        TTF_Font* ttf_font_ = _getValidTTFFontPtr();
        if (ttf_font_)
        {
            if (TTF_FontHasGlyph(ttf_font_, ch))
            {
                if (TTF_GetGlyphMetrics(ttf_font_, ch, minx, maxx, miny, maxy, advance) == 0)
                {
                    return true;
                }
            }
        }
        return false;
    } // END: TruetypeFont::getGlyphMetrics(Uint32 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance) const 

    int  TruetypeFont::getGlyphHeight(Uint32 ch) const 
    {
        // Return glyph height
        TTF_Font* ttf_font_ = _getValidTTFFontPtr();
        if (ttf_font_)
        {
            return TTF_GetFontHeight(ttf_font_);
        }
        return 0;
    } // END: TruetypeFont::getGlyphHeight(Uint32 ch) const 

    int  TruetypeFont::getGlyphWidth(Uint32 ch) const 
    {
        TTF_Font* ttf_font_ = _getValidTTFFontPtr();
        if (ttf_font_)
        {
            int minx, maxx, miny, maxy, advance;
            if (TTF_GetGlyphMetrics(ttf_font_, ch, &minx, &maxx, &miny, &maxy, &advance)) 
            {
                // // DEBUG: Print values
                // std::cout << "Glyph " << (char)ch << ": advance=" << advance << " minx=" << minx << " maxx=" << maxx << std::endl;
                return advance; // Use advance for correct spacing
            }
            else
            {
                std::cout << "getGlyphMetrics failed for char: " << (char)ch << std::endl;
            }
        }
        else
        {
            std::cout << "ttf_font_ is null!" << std::endl;
        }
        return 0;
    } // END: TruetypeFont::getGlyphWidth(Uint32 ch) const

    int  TruetypeFont::getFontAscent() 
    {
        // Return font ascent
        TTF_Font* ttf_font_ = _getValidTTFFontPtr();
        if (ttf_font_)
        {
            return TTF_GetFontAscent(ttf_font_);
        }
        return 0;
    } // END: TruetypeFont::getFontAscent() 

    int TruetypeFont::getFontSize() 
    {
        return TTF_GetFontSize(_getValidTTFFontPtr());
    } // END: TruetypeFont::getFontSize() const

    void TruetypeFont::setFontSize(int p_size) 
    {
        TTF_Font* ttf_font_ = _getValidTTFFontPtr();
        if (!ttf_font_) return;
        // Only set if size is positive and different from current
        if (p_size > 0 && p_size != last_size_) 
        {
            fontSize_ = p_size; 
            last_size_ = p_size;
            if (!TTF_SetFontSize(ttf_font_, p_size))
            {
                ERROR("Error setting font size: " + std::string(SDL_GetError()) );
            }
        }
    } // END: TruetypeFont::setFontSize(int p_size) 

    void TruetypeFont::setFontStyle(const FontStyle& style) 
    {
        TTF_Font* ttf_font_ = _getValidTTFFontPtr();
        if (!ttf_font_) return;
        // Sync font size if changed
        if (style.fontSize != fontSize_)
        {
            setFontSize(style.fontSize);
        }
        // Set font style
        int styleMask = TTF_STYLE_NORMAL;
        if (style.bold) styleMask |= TTF_STYLE_BOLD;
        if (style.italic) styleMask |= TTF_STYLE_ITALIC;
        if (style.underline) styleMask |= TTF_STYLE_UNDERLINE;
        if (style.strikethrough) styleMask |= TTF_STYLE_STRIKETHROUGH;
        TTF_SetFontStyle(ttf_font_, styleMask);
        TTF_SetFontOutline(ttf_font_, style.outlineThickness);
    } // END: TruetypeFont::setFontStyle(const FontStyle& style) 

    FontStyle TruetypeFont::getFontStyle()
    {
        FontStyle style;
        style.fontSize = fontSize_;
        return style;  // Return current style (placeholder implementation)
    } // END: TruetypeFont::getFontStyle()

    TTF_Font* TruetypeFont::_getValidTTFFontPtr() const
    {
        if (!ttf_font_handle_.isValid()) {
            DEBUG_LOG("TruetypeFont::_getValidTTFFontPtr - invalid TTF asset handle");
            return nullptr;
        }
        TTFAsset* ttfAsset = ttf_font_handle_->as<TTFAsset>();
        if (!ttfAsset || !ttfAsset->isLoaded()) {
            DEBUG_LOG("TruetypeFont::_getValidTTFFontPtr - TTF asset not loaded");
            return nullptr;
        }
        TTF_Font* ttf_font = ttfAsset->_getTTFFontPtr();
        if (!ttf_font) {
            DEBUG_LOG("TruetypeFont::_getValidTTFFontPtr - TTF font pointer is null");
            return nullptr;
        }
        return ttf_font;
    } // END: TruetypeFont::_getValidTTFFontPtr() const   

    // --- Lua Registration --- //

    void TruetypeFont::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "TruetypeFont:" + getName();
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << typeNameLocal 
                    << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA 
                    << typeName << CLR::RESET << std::endl;
        }

        // // Augment the single shared AssetHandle handle usertype (assets are exposed via AssetHandle handles in Lua)
        // sol::table handle = AssetHandle::ensure_handle_table(lua);


    } // END _registerLuaBindings()


} // END: namespace SDOM
