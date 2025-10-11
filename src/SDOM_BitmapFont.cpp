// SDOM_BitmapFont.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>

namespace SDOM
{
    BitmapFont::BitmapFont(const InitStruct& init) : IFontObject(init)
    {

        bitmapFontWidth_ = init.fontWidth;
        bitmapFontHeight_ = init.fontHeight;
        if (init.fontWidth < 0)  bitmapFontWidth_ = init.fontSize;
        if (init.fontHeight < 0) bitmapFontHeight_ = init.fontSize;
    }

    BitmapFont::BitmapFont(const sol::table& config) : IFontObject(config)
    {
        InitStruct init;
        // Prefer Lua keys: "size", "width", "height".
        // Also accept legacy keys: "fontSize", "fontWidth", "fontHeight".
        if (config["size"].valid())        fontSize_ = config["size"].get<int>(); else fontSize_ = init.fontSize;
        if (config["width"].valid())       bitmapFontWidth_ = config["width"].get<int>(); else bitmapFontWidth_ = init.fontWidth;
        if (config["height"].valid())      bitmapFontHeight_ = config["height"].get<int>(); else bitmapFontHeight_ = init.fontHeight;

        // legacy overrides
        if (config["font_size"].valid())    fontSize_ = config["font_size"].get<int>(); else fontSize_ = init.fontSize;
        if (config["font_width"].valid())   bitmapFontWidth_ = config["font_width"].get<int>(); else bitmapFontWidth_ = init.fontWidth;
        if (config["font_height"].valid())  bitmapFontHeight_ = config["font_height"].get<int>(); else bitmapFontHeight_ = init.fontHeight;

        // normalize invalid/zero dimensions to fontSize_
        if (bitmapFontWidth_ <= 0)  bitmapFontWidth_ = fontSize_;
        if (bitmapFontHeight_ <= 0) bitmapFontHeight_ = fontSize_;
    }


    BitmapFont::~BitmapFont()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "~BitmapFont()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Destructor implementation if needed
        // ...
    }


    bool BitmapFont::onInit()
    {
        SpriteSheet::InitStruct init;
        init.filename = filename_;
        init.name = name_ + "_SpriteSheet";
        init.type = SpriteSheet::TypeName;            // use SpriteSheet type constant
        init.spriteWidth = bitmapFontWidth_;
        init.spriteHeight = bitmapFontHeight_;

        AssetHandle existing = getFactory().getAssetObject(init.name);
        if (existing.isValid())
        {
            ERROR("BitmapFont::onInit: Asset with name already exists: " + init.name);
            return false;
        }

        // create by type, not by name
        spriteSheet_ = getFactory().createAsset(SpriteSheet::TypeName, init);
        if (!spriteSheet_)
        {
            ERROR("BitmapFont::onInit: Factory failed to create SpriteSheet asset for filename: " + filename_);
            return false;
        }

        // Defensive: ensure we actually got a SpriteSheet
        if (spriteSheet_.as<SpriteSheet>() == nullptr)
        {
            ERROR("BitmapFont::onInit: Created asset is not a SpriteSheet for filename: " + filename_);
            spriteSheet_.reset();
            return false;
        }

        // Ensure the asset is loaded now (call onLoad at init-time to avoid render-path lazy-load)
        spriteSheet_.get(); // touch handle
        SpriteSheet* ss_init = spriteSheet_.as<SpriteSheet>();
        if (ss_init) {
            if (!ss_init->isLoaded()) {
                ss_init->onLoad();
            }
            // Sync metrics immediately if available
            int sw = ss_init->getSpriteWidth();
            int sh = ss_init->getSpriteHeight();
            if (sw > 0) bitmapFontWidth_ = sw;
            if (sh > 0) bitmapFontHeight_ = sh;
        }
        return true;
    } // END onInit()
    
    void BitmapFont::onQuit()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onQuit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;

        // If we hold a SpriteSheet handle, ensure it's unloaded and release our reference.
        if (spriteSheet_.isValid()) 
        {
            auto* ss = spriteSheet_.as<SpriteSheet>();
            if (ss && ss->isLoaded()) 
            {
                ss->onUnload();   // unload texture/resources
            }
            spriteSheet_.reset(); // drop our handle so the Factory/cache can reclaim if unused
        }    
    } // END onQuit()


    void BitmapFont::onLoad()
    {
        static thread_local bool in_onload = false;
        if (in_onload) { 
            // std::cerr << "BitmapFont::onLoad - re-entrant call detected for: " << getName() << std::endl;
            return;
        }
        in_onload = true;

        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onLoad()"
        //         << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;

        if (isLoaded_) onUnload();

        // Resolve or create the backing SpriteSheet asset (use AssetHandle API)
        std::string sheetName = name_ + "_SpriteSheet";
        AssetHandle existing = getFactory().getAssetObject(sheetName);
        if (existing.isValid())
        {
            // protect against accidental self-reference
            IAssetObject* raw = existing.get();
            if (raw == static_cast<IAssetObject*>(this)) {
                ERROR("BitmapFont::onLoad: found self as SpriteSheet asset: " + sheetName);
                in_onload = false;
                return;
            }
            spriteSheet_ = existing;
        }
        else
        {
            SpriteSheet::InitStruct init;
            init.name = sheetName;
            init.type = SpriteSheet::TypeName;
            init.filename = filename_;
            init.spriteWidth = bitmapFontWidth_;
            init.spriteHeight = bitmapFontHeight_;

            spriteSheet_ = getFactory().createAsset(SpriteSheet::TypeName, init);
            if (!spriteSheet_) {
                ERROR("BitmapFont::onLoad: Factory failed to create SpriteSheet asset: " + sheetName);
                in_onload = false;
                return;
            }

            // sanity: ensure factory didn't hand back a handle to ourselves
            IAssetObject* raw = spriteSheet_.get();
            if (raw == static_cast<IAssetObject*>(this)) {
                ERROR("BitmapFont::onLoad: Factory returned self for created SpriteSheet: " + sheetName);
                spriteSheet_.reset();
                in_onload = false;
                return;
            }
        }

        // Validate & ensure loaded (avoid forcing load that can re-enter this code path)
        IAssetObject* raw = spriteSheet_.get();
        SpriteSheet* ss = raw ? dynamic_cast<SpriteSheet*>(raw) : nullptr;
        if (!ss) {
            ERROR("BitmapFont::onLoad: resolved asset is not a SpriteSheet: " + sheetName);
            spriteSheet_.reset();
            in_onload = false;
            return;
        }

        // Only request load if not already loaded (this can still recurse; guard prevents us looping)
        if (!ss->isLoaded()) {
            ss->onLoad();
        }

        // Sync metrics
        int sw = ss->getSpriteWidth();
        int sh = ss->getSpriteHeight();
        if (sw > 0) bitmapFontWidth_ = sw;
        if (sh > 0) bitmapFontHeight_ = sh;

        // --- Prepare outline textures (pre-rendered per-thickness glyph outlines)
        // Clean up any existing outline textures first
        if (!outlineTextures.empty()) {
            for (auto &vec : outlineTextures) {
                for (auto *tex : vec) {
                    if (tex) SDL_DestroyTexture(tex);
                }
            }
            outlineTextures.clear();
        }

        SDL_Renderer* renderer = getRenderer();
        if (renderer) {
            int maxThickness = maxOutlineThickness; // use IFontObject constant
            int spriteCount = ss->getSpriteCount();

            outlineTextures.resize(maxThickness);

            // Save original render target so we can restore it afterwards
            SDL_Texture* originalTarget = SDL_GetRenderTarget(renderer);

            for (int t = 1; t <= maxThickness; ++t) {
                outlineTextures[t-1].resize(spriteCount);
                for (int i = 0; i < spriteCount; ++i) {
                    int texW = bitmapFontWidth_ + t * 2;
                    int texH = bitmapFontHeight_ + t * 2;
                    SDL_Texture* outTex = SDL_CreateTexture(
                        renderer,
                        SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET,
                        texW,
                        texH
                    );
                    if (!outTex) {
                        ERROR(std::string("Failed to create outline texture: ") + SDL_GetError());
                        outlineTextures[t-1][i] = nullptr;
                        continue;
                    }

                    // Clear the outline texture
                    SDL_SetRenderTarget(renderer, outTex);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    SDL_RenderClear(renderer);

                    // Render the glyph multiple times offset by dx/dy to form an outline
                    for (int dx = -t; dx <= t; ++dx) {
                        for (int dy = -t; dy <= t; ++dy) {
                            // initializeOutlineGlyph expects a character code (sprite index + 32)
                            initializeOutlineGlyph(i + 32, dx + t, dy + t);
                        }
                    }

                    // Store the created texture
                    outlineTextures[t-1][i] = outTex;
                }
            }

            // Restore original render target
            SDL_SetRenderTarget(renderer, originalTarget);
        }

        isLoaded_ = true;
        in_onload = false;

    } // END onLoad()

    void BitmapFont::onUnload()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "onUnload()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;

        // Unload/release the sprite sheet we created/hold (safe cast + reset handle)
        if (spriteSheet_.isValid()) 
        {
            SpriteSheet* ss = spriteSheet_.as<SpriteSheet>();
            if (ss && ss->isLoaded()) ss->onUnload();
            spriteSheet_.reset();
        }
        // Destroy any outline textures
        if (!outlineTextures.empty()) {
            for (auto &vec : outlineTextures) {
                for (auto *tex : vec) {
                    if (tex) SDL_DestroyTexture(tex);
                }
            }
            outlineTextures.clear();
        }
        isLoaded_ = false;
    } // END onUnload()

    void BitmapFont::create(const sol::table& config)
    {
        // Update basic identity/config from Lua table
        if (config["name"].valid())     name_ = config["name"].get<std::string>();
        if (config["filename"].valid()) filename_ = config["filename"].get<std::string>();

        // size/width/height (prefer new keys, accept legacy)
        if (config["size"].valid())        fontSize_ = config["size"].get<int>();
        if (config["width"].valid())       bitmapFontWidth_ = config["width"].get<int>();
        if (config["height"].valid())      bitmapFontHeight_ = config["height"].get<int>();

        if (config["fontSize"].valid())    fontSize_ = config["fontSize"].get<int>();
        if (config["fontWidth"].valid())   bitmapFontWidth_ = config["fontWidth"].get<int>();
        if (config["fontHeight"].valid())  bitmapFontHeight_ = config["fontHeight"].get<int>();

        // normalize invalid/zero dimensions to fontSize_
        if (bitmapFontWidth_ <= 0)  bitmapFontWidth_ = (fontSize_ > 0 ? fontSize_ : 8);
        if (bitmapFontHeight_ <= 0) bitmapFontHeight_ = (fontSize_ > 0 ? fontSize_ : 8);

        // Optional explicit spriteSheet name; default to "<name>_SpriteSheet"
        std::string sheetName;
        if (config["spriteSheetName"].valid()) sheetName = config["spriteSheetName"].get<std::string>();
        else sheetName = name_.empty() ? std::string("bitmap_sprite_sheet") : (name_ + "_SpriteSheet");

        // If we already hold a spriteSheet, unload and drop it so create/rebind proceeds cleanly.
        if (spriteSheet_.isValid())
        {
            SpriteSheet* ss = spriteSheet_.as<SpriteSheet>();
            if (ss && ss->isLoaded()) ss->onUnload();
            spriteSheet_.reset();
        }

        // Try to reuse an existing factory asset with that name, else create a new SpriteSheet asset.
        AssetHandle existing = getFactory().getAssetObject(sheetName);
        if (existing.isValid())
        {
            spriteSheet_ = existing;
        }
        else
        {
            SpriteSheet::InitStruct init;
            init.filename = filename_;
            init.name = sheetName;
            init.type = SpriteSheet::TypeName;
            init.spriteWidth = bitmapFontWidth_;
            init.spriteHeight = bitmapFontHeight_;

            spriteSheet_ = getFactory().createAsset(SpriteSheet::TypeName, init);
            if (!spriteSheet_)
            {
                ERROR("BitmapFont::create - failed to create SpriteSheet asset: " + sheetName);
                return;
            }
        }

        // Ensure the sprite sheet is loaded and sync metrics
        SpriteSheet* ss = spriteSheet_.as<SpriteSheet>();
        if (!ss)
        {
            ERROR("BitmapFont::create - resolved asset is not a SpriteSheet: " + sheetName);
            spriteSheet_.reset();
            return;
        }
        if (!ss->isLoaded()) ss->onLoad();

        int sw = ss->getSpriteWidth();
        int sh = ss->getSpriteHeight();
        if (sw > 0) bitmapFontWidth_ = sw;
        if (sh > 0) bitmapFontHeight_ = sh;

        // mark this font as configured/loaded
        isLoaded_ = true;
    } // END create()

    void BitmapFont::drawGlyph(Uint32 ch, int x, int y, const FontStyle& style)
    {
        if (!spriteSheet_) 
        {
            ERROR("SpriteSheet is null in BitmapFont::drawGlyph");
            return;
        }

        // Draw Drop Shadow
        drawDropShadowGlyph(ch, x, y, style);

        // Draw Outline
        drawOutlineGlyph(ch, x, y, style);

        // Draw Foreground Glyph
        drawForegroundGlyph(ch, x, y, style);
    } // END drawGlyph()

    void BitmapFont::drawPhrase(const std::string& str, int x, int y, const FontStyle& style)
    {
        if (!spriteSheet_) return;

        // Pass 3: Foreground
        int cursorX = x;
        for (char ch : str) 
        {
            drawForegroundGlyph(static_cast<Uint32>(ch), cursorX, y, style);
            cursorX += getGlyphWidth(ch);
        }
    } // END drawPhrase()

    void BitmapFont::drawPhraseOutline(const std::string& str, int x, int y, const FontStyle& style)
    {
        if (!spriteSheet_) return;

        // Pass 2: Outline
        if (style.outline && style.outlineThickness > 0) 
        {
            int cursorX = x;
            for (char ch : str) 
            {
                drawOutlineGlyph(static_cast<Uint32>(ch), cursorX, y, style);
                cursorX += getGlyphWidth(ch);
            }
        }
    } // END drawPhraseOutline()

    void BitmapFont::drawPhraseDropshadow(const std::string& str, int x, int y, const FontStyle& style)
    {
        if (!spriteSheet_) return;

        // Pass 1: Drop Shadow
        if (style.dropshadow) 
        {
            int cursorX = x;
            for (char ch : str) 
            {
                drawDropShadowGlyph(static_cast<Uint32>(ch), cursorX, y, style);
                cursorX += getGlyphWidth(ch);
            }
        }
    } // END drawPhraseDropshadow()

    bool BitmapFont::getGlyphMetrics(Uint32 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance) const
    {
        // Scale all metrics by the font size ratio
        float ratio = float(fontSize_) / float(bitmapFontHeight_);
        int scaledWidth = int(ratio * bitmapFontWidth_ + 0.5f);
        int scaledHeight = int(ratio * bitmapFontHeight_ + 0.5f);
        if (minx)    *minx    = 0;
        if (maxx)    *maxx    = scaledWidth;
        if (miny)    *miny    = 0;
        if (maxy)    *maxy    = scaledHeight;
        if (advance) *advance = scaledWidth; // Advance by scaled width for monospace
        return true;
    } // END getGlyphMetrics()

    int BitmapFont::getGlyphHeight(Uint32 ch) const
    {
        // Return the scaled height of the specified glyph
        float ratio = float(fontSize_) / float(bitmapFontHeight_);
        return int(ratio * bitmapFontHeight_ + 0.5f);
    } // END getGlyphHeight()

    int BitmapFont::getGlyphWidth(Uint32 ch) const
    {
        // Return the scaled width of the specified glyph
        float ratio = float(fontSize_) / float(bitmapFontHeight_);
        return int(ratio * bitmapFontWidth_ + 0.5f);
    }

    int BitmapFont::getFontAscent()
    {
        // Return the font ascent value
        return 0; // Placeholder return value
    }

    void BitmapFont::setFontSize(int p_size)
    {
        fontSize_ = p_size;
    }

    void BitmapFont::setFontStyle(const FontStyle& style)
    {
        setFontSize(style.fontSize);
    }

    FontStyle BitmapFont::getFontStyle()
    {
        FontStyle style;
        style.fontSize = fontSize_;
        return style;  // Return current style (placeholder implementation)
    }

    // --- Protected Methods --- //

    void BitmapFont::initializeOutlineGlyph(Uint32 ch, int x, int y)
    {
        if (!spriteSheet_) 
        {
            ERROR("SpriteSheet is null in BitmapFont::initializeOutlineGlyph");
            return;
        }
        SDL_Renderer* renderer = getRenderer();
        if (!renderer) 
        {
            ERROR("Renderer is null in BitmapFont::initializeOutlineGlyph");
            return;
        }
        int spriteIndex = static_cast<int>(ch) - 32;
        if (spriteIndex < 0)  return;

        // Only draw normally if not italic (to avoid double rendering)
        SpriteSheet* ss = spriteSheet_.as<SpriteSheet>();
        if (!ss) 
        {
            ERROR("SpriteSheet cast failed in BitmapFont::initializeOutlineGlyph");
            return;
        }
        int spriteCount = ss->getSpriteCount();
        spriteIndex = std::clamp(spriteIndex, 0, spriteCount - 1);

        SDL_FRect destRect;
        destRect.x = static_cast<float>(x);
        destRect.y = static_cast<float>(y);
        destRect.w = bitmapFontWidth_;
        destRect.h = bitmapFontHeight_;
        ss->drawSprite(spriteIndex, destRect, {255,255,255,255}, SDL_SCALEMODE_NEAREST);
    } // END initializeOutlineGlyph()

    void BitmapFont::drawForegroundGlyph(Uint32 ch, int x, int y, const FontStyle& style)
    {
        if (!spriteSheet_) 
        {
            ERROR("SpriteSheet is null in BitmapFont::drawForegroundGlyph");
            return;
        }
        SpriteSheet* ss = spriteSheet_.as<SpriteSheet>();
        if (!ss) 
        {
            ERROR("SpriteSheet cast failed in BitmapFont::drawForegroundGlyph");
            return;
        }
        int spriteIndex = static_cast<int>(ch) - 32;
        if (spriteIndex < 0)  return;
        
        SDL_Color color = style.foregroundColor;
        SDL_Renderer* renderer = getRenderer();
        if (!renderer) {
            ERROR("Renderer is null in BitmapFont::drawGlyph");
            return;
        }
        SDL_Texture* fontTexture = ss->getTexture();
        if (!fontTexture) {
            DEBUG_LOG("Font texture is null in BitmapFont::drawGlyph");
            //ss->onLoad();
            return;
        }
        // Apply font styles (bold, underline, etc.) if needed
        if (style.bold) {
            spriteIndex = static_cast<int>(ch) + 64; 
        }

        if (style.italic) 
        {
            // Apply italic style

            float ratio = float(style.fontSize) / float(bitmapFontHeight_);
            int scaledWidth = int(ratio * bitmapFontWidth_);
            int scaledHeight = int(ratio * bitmapFontHeight_);
            float slant = 1.0f * ratio; // Slant scales with font size

            int glyphX = ss->getSpriteX(spriteIndex);
            int glyphY = ss->getSpriteY(spriteIndex);
            int sheetW = fontTexture->w;
            int sheetH = fontTexture->h;

            // Texture coordinates (normalized)
            float u0 = float(glyphX) / sheetW;
            float v0 = float(glyphY) / sheetH;
            float u1 = float(glyphX + bitmapFontWidth_) / sheetW;
            float v1 = float(glyphY + bitmapFontHeight_) / sheetH;

            SDL_Vertex vertices[4] = {
                // Top-left (skewed right)
                { { float(x) + slant, float(y) }, { color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f }, { u0, v0 } },
                // Top-right (skewed right)
                { { float(x + scaledWidth) + slant, float(y) }, { color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f }, { u1, v0 } },
                // Bottom-right
                { { float(x + scaledWidth) - slant, float(y + scaledHeight) }, { color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f }, { u1, v1 } },
                // Bottom-left
                { { float(x) - slant, float(y + scaledHeight) }, { color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f }, { u0, v1 } }
            };
            int indices[6] = { 0, 1, 2, 0, 2, 3 };

            // SDL_SCALEMODE_NEAREST  
            // SDL_SCALEMODE_LINEAR   
            // SDL_SCALEMODE_PIXELART 
            SDL_SetTextureScaleMode(fontTexture, SDL_SCALEMODE_NEAREST);

            if (!SDL_RenderGeometry(renderer, fontTexture, vertices, 4, indices, 6)) {
                ERROR(std::string("Failed to render skewed glyph: ") + SDL_GetError());
            }

        }   
        if (style.underline) 
        {
            float ratio = float(style.fontSize) / float(bitmapFontHeight_);
            int scaledWidth = int(ratio * bitmapFontWidth_);
            int scaledHeight = int(ratio * bitmapFontHeight_);

            // Underline position: just above the bottom of the glyph box
            float underlineY = y + scaledHeight - 1;
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a - (color.a/3));
            SDL_RenderLine(renderer, float(x), underlineY, float(x + scaledWidth - 1), underlineY);
        }
        if (style.strikethrough) 
        {
            float ratio = float(style.fontSize) / float(bitmapFontHeight_);
            int scaledWidth = int(ratio * bitmapFontWidth_);
            int scaledHeight = int(ratio * bitmapFontHeight_);

            // Strikethrough position: centered vertically (favoring above center for even heights)
            float strikeY = y + (scaledHeight / 2) - (scaledHeight % 2 == 0 ? 1 : 0) + 1;
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a - (color.a/3));
            SDL_RenderLine(renderer, float(x), strikeY, float(x + scaledWidth - 1), strikeY);
        }

        // clamp ch to spriteSheet_->getSpriteCount()
        if (!style.italic) 
        {
            // Only draw normally if not italic (to avoid double rendering)
            int spriteCount = ss->getSpriteCount();
            spriteIndex = std::clamp(spriteIndex, 0, spriteCount - 1);
            // spriteSheet_->drawSprite(x, y, spriteIndex, color);

            // Calculate scaling ratio based on desired font size
            float scale = float(style.fontSize) / float(bitmapFontHeight_);
            float destW = bitmapFontWidth_ * scale;
            float destH = bitmapFontHeight_ * scale;

            SDL_FRect destRect;
            destRect.x = static_cast<float>(x);
            destRect.y = static_cast<float>(y);
            destRect.w = destW;
            destRect.h = destH;
            
            ss->drawSprite(spriteIndex, destRect, color, SDL_SCALEMODE_NEAREST);
        }
    } // END drawForegroundGlyph()

    
    void BitmapFont::drawOutlineGlyph(Uint32 ch, int x, int y, const FontStyle& style)
    {
        if (!spriteSheet_) 
        {
            ERROR("SpriteSheet is null in BitmapFont::drawForegroundGlyph");
            return;
        }
        SpriteSheet* ss = spriteSheet_.as<SpriteSheet>();
        if (!ss) 
        {
            ERROR("SpriteSheet cast failed in BitmapFont::drawForegroundGlyph");
            return;
        }
        SDL_Renderer* renderer = getRenderer();
        if (!renderer) {
            ERROR("Renderer is null in BitmapFont::drawGlyph");
            return;
        }
        if (style.outline && style.outlineThickness > 0) 
        {
            int thickness = std::clamp(style.outlineThickness, 1, maxOutlineThickness); // Clamp to your maxThickness
            int spriteIndex = static_cast<int>(ch) - 32;
            // Apply bold if needed
            if (style.bold) {
                spriteIndex = static_cast<int>(ch) + 64; 
            }

            int spriteCount = ss->getSpriteCount();
            spriteIndex = std::clamp(spriteIndex, 0, spriteCount - 1);

            // Correct scaling: scale the entire outline texture, not just the glyph + thickness
            float ratio = float(style.fontSize) / float(bitmapFontHeight_);
            float destW = (bitmapFontWidth_ + thickness * 2) * ratio;
            float destH = (bitmapFontHeight_ + thickness * 2) * ratio;

            SDL_FRect destRect;
            destRect.x = x - thickness * ratio;
            destRect.y = y - thickness * ratio;
            destRect.w = destW;
            destRect.h = destH;

            SDL_Texture* outlineTex = outlineTextures[thickness - 1][spriteIndex];
            if (outlineTex) {
                SDL_SetTextureColorMod(outlineTex, style.outlineColor.r, style.outlineColor.g, style.outlineColor.b);
                SDL_SetTextureAlphaMod(outlineTex, style.outlineColor.a);
                SDL_SetTextureScaleMode(outlineTex, SDL_SCALEMODE_LINEAR);
                SDL_RenderTexture(renderer, outlineTex, nullptr, &destRect);
            }
        }
    } // END drawOutlineGlyph()


    void BitmapFont::drawDropShadowGlyph(Uint32 ch, int x, int y, const FontStyle& style)
    {
        if (!spriteSheet_) 
        {
            ERROR("SpriteSheet is null in BitmapFont::drawForegroundGlyph");
            return;
        }
        SpriteSheet* ss = spriteSheet_.as<SpriteSheet>();
        if (!ss) 
        {
            ERROR("SpriteSheet cast failed in BitmapFont::drawForegroundGlyph");
            return;
        }
        SDL_Renderer* renderer = getRenderer();
        if (!renderer) 
        {
            ERROR("Renderer is null in BitmapFont::drawGlyph");
            return;
        }

        if (!style.dropshadow || style.dropshadowColor.a == 0)
            return;

        int thickness = 1;

        // Apply bold if needed
        int spriteIndex = static_cast<int>(ch) - 32;
        if (style.bold) {
            spriteIndex = static_cast<int>(ch) + 64; 
        }

        int spriteCount = ss->getSpriteCount();
        spriteIndex = std::clamp(spriteIndex, 0, spriteCount - 1);

        // Calculate destination rectangle (account for outline thickness offset)
        SDL_FRect destRect;
        float ratio = float(style.fontSize) / float(bitmapFontHeight_);
        int scaledWidth = int(ratio * bitmapFontWidth_);
        int scaledHeight = int(ratio * bitmapFontHeight_);
        float scaledThickness = ratio * thickness;

        // std::cout << "Drawing drop shadow for char " << char(ch) << " at (" << x << "," << y << ") with offset (" 
        //           << style.dropshadowOffsetX << "," << style.dropshadowOffsetY << ")\n";

        destRect.x = (x - scaledThickness) + style.dropshadowOffsetX;
        destRect.y = (y - scaledThickness) + style.dropshadowOffsetY;
        destRect.w = (scaledWidth + scaledThickness * 2);
        destRect.h = (scaledHeight + scaledThickness * 2);

        SDL_Texture* outlineTex = outlineTextures[thickness - 1][spriteIndex];
        if (outlineTex) {
            SDL_SetTextureColorMod(outlineTex, style.dropshadowColor.r, style.dropshadowColor.g, style.dropshadowColor.b);
            SDL_SetTextureAlphaMod(outlineTex, style.dropshadowColor.a);
            SDL_SetTextureScaleMode(outlineTex, SDL_SCALEMODE_LINEAR);
            SDL_RenderTexture(renderer, outlineTex, nullptr, &destRect);
        }       
    } // END drawDropShadowGlyph()

    // --- Lua Registration --- //

    void BitmapFont::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "BitmapFont:" + getName();
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << typeNameLocal 
                    << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA 
                    << typeName << CLR::RESET << std::endl;
        }

        // Augment the single shared AssetHandle handle usertype (assets are exposed via AssetHandle handles in Lua)
        sol::table handle = AssetHandle::ensure_handle_table(lua);

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

        // small helper to validate and cast the AssetHandle -> BitmapFont*
        auto cast_ss_from_asset = [](const AssetHandle& asset) -> BitmapFont* {
            if (!asset.isValid()) { ERROR("invalid AssetHandle provided to BitmapFont method"); }
            IAssetObject* base = asset.get();
            BitmapFont* bmp = dynamic_cast<BitmapFont*>(base);
            if (!bmp) { ERROR("invalid BitmapFont object"); }
            return bmp;
        };

        // // Register BitmapFont-specific properties and commands here (bridge from AssetHandle handle)
        reg("setBitmapFontWidth", [cast_ss_from_asset](AssetHandle asset, int w) { cast_ss_from_asset(asset)->setBitmapFontWidth(w); });
        reg("setBitmapFontHeight", [cast_ss_from_asset](AssetHandle asset, int h) { cast_ss_from_asset(asset)->setBitmapFontHeight(h); });
        reg("getBitmapFontWidth", [cast_ss_from_asset](AssetHandle asset) { return cast_ss_from_asset(asset)->getBitmapFontWidth(); });
        reg("getBitmapFontHeight", [cast_ss_from_asset](AssetHandle asset) { return cast_ss_from_asset(asset)->getBitmapFontHeight(); });



    } // END _registerLuaBindings()


} // END namespace SDOM