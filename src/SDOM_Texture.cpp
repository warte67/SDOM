// SDOM_Texture.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>
#include <SDOM/SDOM_Texture.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_PathRegistry.hpp>


namespace
{
    inline bool isInternalTextureName(const std::string& name)
    {
        return name.rfind("internal_", 0) == 0;
    }

    inline std::string resolveTextureFilename(const std::string& filename)
    {
        if (filename.empty()) {
            return filename;
        }
        auto& registry = SDOM::PathRegistry::get();
        std::string resolved = registry.resolve(filename, SDOM::PathType::Images);
        if (!resolved.empty()) {
            return resolved;
        }
        return filename;
    }
}

namespace SDOM
{
    Texture::Texture(const InitStruct& init) : IAssetObject(init)
    {
        if (init.type != TypeName) 
        {
            ERROR("Error: Texture constructed with incorrect type: " + init.type);
        }
        name_ = init.name;
        type_ = init.type;
        filename_ = init.filename;
        // Initialize texture-specific parameters from init if any
    }


    bool Texture::onInit()
    {
        // std::cout << CLR::LT_ORANGE << "Texture::" << CLR::YELLOW << "onInit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Initialization logic for Texture
        return true;
    }

    void Texture::onQuit()
    {
        // std::cout << CLR::LT_ORANGE << "Texture::" << CLR::YELLOW << "onQuit()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Cleanup logic for Texture
        onUnload();
    }

    void Texture::onLoad()
    {
        onUnload();
        textureKey_.clear();

        SDL_Renderer* renderer = getRenderer();
        if (!renderer) { ERROR("No valid SDL_Renderer available in Core instance."); return; }

        // Resolve filename through PathRegistry unless this is one of the embedded assets.
        std::string resolvedFilename = filename_;
        const bool treatAsInternal = isInternal() || isInternalTextureName(filename_);
        if (!treatAsInternal) {
            resolvedFilename = resolveTextureFilename(filename_);
        }

        Factory& factory = getFactory();
        textureKey_ = resolvedFilename;
        if (treatAsInternal) {
            textureKey_ = filename_;
        }

        auto loadTexture = [&]() -> SDL_Texture* {
            if (treatAsInternal) {
                if (filename_ == "internal_icon_8x8") {
                    SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(internal_icon_8x8)), internal_icon_8x8_len);
                    if (!rw) { ERROR("Failed to create SDL_IOStream from internal_icon_8x8[]"); return nullptr; }
                    SDL_Texture* tex = IMG_LoadTexture_IO(renderer, rw, 1);
                    if (tex) SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
                    return tex;
                }
                if (filename_ == "internal_icon_12x12") {
                    SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(internal_icon_12x12)), internal_icon_12x12_len);
                    if (!rw) { ERROR("Failed to create SDL_IOStream from internal_icon_12x12[]"); return nullptr; }
                    SDL_Texture* tex = IMG_LoadTexture_IO(renderer, rw, 1);
                    if (tex) SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
                    return tex;
                }
                if (filename_ == "internal_icon_16x16") {
                    SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(internal_icon_16x16)), internal_icon_16x16_len);
                    if (!rw) { ERROR("Failed to create SDL_IOStream from internal_icon_16x16[]"); return nullptr; }
                    SDL_Texture* tex = IMG_LoadTexture_IO(renderer, rw, 1);
                    if (tex) SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
                    return tex;
                }
                if (filename_ == "internal_font_8x8") {
                    SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(internal_font_8x8)), internal_font_8x8_len);
                    if (!rw) { ERROR("Failed to create SDL_IOStream from internal_font_8x8[]"); return nullptr; }
                    SDL_Texture* tex = IMG_LoadTexture_IO(renderer, rw, 1);
                    if (tex) SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
                    return tex;
                }
                if (filename_ == "internal_font_8x12") {
                    SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(internal_font_8x12)), internal_font_8x12_len);
                    if (!rw) { ERROR("Failed to create SDL_IOStream from internal_font_8x12[]"); return nullptr; }
                    SDL_Texture* tex = IMG_LoadTexture_IO(renderer, rw, 1);
                    if (tex) SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
                    return tex;
                }
            }

            SDL_Texture* tex = IMG_LoadTexture(renderer, resolvedFilename.c_str());
            if (!tex) { ERROR("Failed to load texture from file: " + resolvedFilename + " - " + std::string(SDL_GetError())); return nullptr; }
            SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
            return tex;
        };

        texture_ = factory.retainTexture(textureKey_, loadTexture);
        if (!texture_) {
            textureKey_.clear();
            return;
        }

        if (!texture_) {
            ERROR("Texture::onLoad() - texture_ is null after loading for: " + getName());
            return;
        }

        if (!SDL_GetTextureSize(texture_, &textureWidth_, &textureHeight_))
            ERROR("Failed to query texture size for: " + getName() + " - " + std::string(SDL_GetError()));

        isLoaded_ = true;
    } // END Texture::onLoad()

    void Texture::onUnload()
    {
        // std::cout << CLR::LT_ORANGE << "Texture::" << CLR::YELLOW << "onUnload()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;

        // Free the texture and clear sprite metadata
        if (texture_)
        {
            getFactory().releaseTexture(textureKey_.empty() ? filename_ : textureKey_, texture_);
            texture_ = nullptr;
        }
        textureKey_.clear();
        isLoaded_ = false;    
    }

    bool Texture::onUnitTest(int frame)
    {
        // Run base checks first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Register tests only once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Validate filename
            ut.add_test(objName, "Texture Filename Validation", [this](std::vector<std::string>& errors)
            {
                if (filename_.empty())
                {
                    errors.push_back("Texture '" + getName() + "' has empty filename");
                }
                return true; // ✅ single-frame
            });

            // 🔹 2. Validate loaded texture state
            ut.add_test(objName, "Texture Loaded State", [this](std::vector<std::string>& errors)
            {
                if (isLoaded_)
                {
                    if (!texture_)
                    {
                        errors.push_back("Texture '" + getName() +
                                        "' is marked loaded but texture_ is null");
                    }
                    if (textureWidth_ <= 0 || textureHeight_ <= 0)
                    {
                        errors.push_back("Texture '" + getName() +
                                        "' has invalid size: w=" +
                                        std::to_string(textureWidth_) +
                                        " h=" + std::to_string(textureHeight_));
                    }
                }
                return true;
            });

            registered = true;
        }

        // ✅ Return false so the texture remains active while the unit test system processes
        return false;
    } // END: Texture::onUnitTest()


    
    void Texture::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "Texture");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    

} // END namespace SDOM
