// SDOM_Texture.hpp
#pragma once

#include <SDOM/SDOM_IAssetObject.hpp>

namespace SDOM
{
    class Factory;
    class Core;

    // internal 8x8 bitmap font sprite sheet
    extern unsigned char internal_font_8x8[];
    extern int internal_font_8x8_len;

    // internal 8x12 bitmap font sprite sheet
    extern unsigned char internal_font_8x12[];
    extern int internal_font_8x12_len;

    // internal 8x8 icon sprite sheet
    extern unsigned char internal_icon_8x8[];
    extern int internal_icon_8x8_len;

    // internal 12x12 icon sprite sheet
    extern unsigned char internal_icon_12x12[];
    extern int internal_icon_12x12_len;

    // internal 16x16 icon sprite sheet
    extern unsigned char internal_icon_16x16[];
    extern int internal_icon_16x16_len;



    class Texture : public IAssetObject
    {
        using SUPER = IAssetObject;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "Texture";

        // --- Initialization Struct --- //
        struct InitStruct : public IAssetObject::InitStruct
        {
            InitStruct() : IAssetObject::InitStruct() 
            { 
                name = TypeName; 
                type = TypeName;
                filename = TypeName; // Default filename, can be overridden
            }
            // add texture-specific initialization parameters here
            // ...
        };

    protected:
        // --- Constructors --- //
        Texture(const InitStruct& init);
        Texture(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IAssetObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IAssetObject>(new Texture(config));
        }
        static std::unique_ptr<IAssetObject> CreateFromInitStruct(const IAssetObject::InitStruct& baseInit) {
            const auto& textureInit = static_cast<const Texture::InitStruct&>(baseInit);
            return std::unique_ptr<IAssetObject>(new Texture(textureInit));
        }

        Texture() = default;
        virtual ~Texture() = default;
        virtual bool onInit() override;
        virtual void onQuit() override;
        virtual void onLoad() override;
        virtual void onUnload() override;
        virtual bool onUnitTest(int frame) override;

        SDL_Texture* getTexture() const { return texture_; }
        float getTextureWidth() const { return textureWidth_; }
        float getTextureHeight() const { return textureHeight_; }

    protected:
        friend Factory;
        friend Core;

        SDL_Texture* texture_ = nullptr;
        float textureWidth_ = 0;
        float textureHeight_ = 0;

        

        // ---------------------------------------------------------------------
        // 🔗 Legacy Lua Registration
        // ---------------------------------------------------------------------
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua) override;


        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        virtual void registerBindingsImpl(const std::string& typeName) override;         


    }; // END class Texture

} // END namespace SDOM