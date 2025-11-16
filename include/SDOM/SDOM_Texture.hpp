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
            InitStruct()
                : IAssetObject::InitStruct()
            {
                name     = TypeName;   // registry key
                type     = TypeName;   // concrete type
                filename = TypeName;   // default resource identifier
                is_internal = true;    // inherited from IAssetObject
            }

            // JSON loader (correct signature!)
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                // 1) Load all base-class asset fields
                IAssetObject::InitStruct::from_json(j, out);

                // 2) Texture-specific fields
                if (j.contains("filename"))
                    out.filename = j["filename"].get<std::string>();
            }
        };


    protected:
        friend Factory;
        friend Core;    
        
        // --- Constructors --- //
        Texture(const InitStruct& init);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IAssetObject> CreateFromInitStruct(const IAssetObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const Texture::InitStruct&>(baseInit);
            return std::unique_ptr<IAssetObject>(new Texture(init));
        }
        static std::unique_ptr<IAssetObject> CreateFromJson(const nlohmann::json& j)
        {
            Texture::InitStruct init;                // defaults
            Texture::InitStruct::from_json(j, init); // JSON overrides
            return std::unique_ptr<IAssetObject>(new Texture(init));
        }


        Texture() = default;  // "null" default constructor     
        ~Texture() override = default;
        bool onInit() override;
        void onQuit() override;
        void onLoad() override;
        void onUnload() override;
        bool onUnitTest(int frame) override;

        SDL_Texture* getTexture() const { return texture_; }
        float getTextureWidth() const { return textureWidth_; }
        float getTextureHeight() const { return textureHeight_; }

    protected:
        SDL_Texture* texture_ = nullptr;
        float textureWidth_ = 0;
        float textureHeight_ = 0;

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         


    }; // END class Texture

} // END namespace SDOM