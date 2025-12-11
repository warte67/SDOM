// SDOM_SpriteSheet.hpp
#pragma once

#include <SDOM/SDOM_IAssetObject.hpp>
#include <SDOM/SDOM_Texture.hpp>

namespace SDOM
{
    class Factory;
    class Core;

    class SpriteSheet : public IAssetObject
    {
        using SUPER = IAssetObject;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "SpriteSheet";

        // --- Initialization Struct --- //
        struct InitStruct : public IAssetObject::InitStruct
        {
            // --- Defaults ---
            int spriteWidth  = 8;
            int spriteHeight = 8;

            InitStruct()
                : IAssetObject::InitStruct()
            {
                name     = TypeName;
                type     = TypeName;
                filename = TypeName;    // Default filename (typically overridden)
                is_internal = false;     // From IAssetObject::InitStruct
            }

            // --- JSON loader ---
            static InitStruct from_json(const nlohmann::json& j)
            {
                InitStruct init;

                // Standard IAssetObject::InitStruct fields
                if (j.contains("name"))       init.name       = j["name"].get<std::string>();
                if (j.contains("type"))       init.type       = j["type"].get<std::string>();
                if (j.contains("filename"))   init.filename   = j["filename"].get<std::string>();
                if (j.contains("is_internal")) init.is_internal = j["is_internal"].get<bool>();

                // SpriteSheet-specific fields
                if (j.contains("sprite_width"))  
                    init.spriteWidth = j["sprite_width"].get<int>();

                if (j.contains("sprite_height")) 
                    init.spriteHeight = j["sprite_height"].get<int>();

                return init;
            }
        };

        
    protected:
        // --- Constructors --- //
        SpriteSheet(const InitStruct& init);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IAssetObject> CreateFromInitStruct(
            const IAssetObject::InitStruct& baseInit)
        {
            const auto& spriteInit = static_cast<const SpriteSheet::InitStruct&>(baseInit);
            return std::unique_ptr<IAssetObject>(new SpriteSheet(spriteInit));
        }

        static std::unique_ptr<IAssetObject> CreateFromJson(const nlohmann::json& j)
        {
            auto init = SpriteSheet::InitStruct::from_json(j);
            return std::unique_ptr<IAssetObject>(new SpriteSheet(init));
        }


        SpriteSheet() = default;
        ~SpriteSheet() override = default;

        bool onInit() override;
        void onQuit() override;
        void onLoad() override;
        void onUnload() override;
        bool onUnitTest(int frame) override;

        // --- Additional sprite sheet specific methods will be added here --- //

        void setSpriteWidth(int width);
        void setSpriteHeight(int height);
        void setSpriteSize(int width, int height);
        SDL_Texture* getTexture() const noexcept
        {
            if (!textureAsset.isValid()) return nullptr;
            Texture* texturePtr = textureAsset.as<Texture>();
            return texturePtr ? texturePtr->getTexture() : nullptr;
        }
 
        int getSpriteWidth() const;
        int getSpriteHeight() const;
        std::pair<int, int> getSpriteSize() const;
        int getSpriteCount() const;
        int getSpriteX(int spriteIndex) const;
        int getSpriteY(int spriteIndex) const;

        // New API: spriteIndex comes first in all drawSprite variants
        void drawSprite(
            int spriteIndex,
            int x, int y,
            SDL_Color color = {255, 255, 255, 255}, 
            SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST,
            SDL_Texture* targetTexture = nullptr
        );
        void drawSprite(
            int spriteIndex,
            SDL_FRect& destRect, 
            SDL_Color color = {255, 255, 255, 255}, 
            SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST,
            SDL_Texture* targetTexture = nullptr
        );
        void drawSprite(
            int spriteIndex,
            const SDL_FRect& srcRect,   // Offset within the sprite tile (not the sheet)
            const SDL_FRect& dstRect,   // Destination on screen
            SDL_Color color = {255, 255, 255, 255},
            SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST,
            SDL_Texture* targetTexture = nullptr
        );

        // Render a 9-slice panel into the specified target texture using the
        // tile set starting at baseIndex. The target texture size determines
        // the output size. Color modulates the sprite color.
        void drawNineQuad(
            int baseIndex,
            SDL_Texture* targetTexture,
            SDL_Color color = {255, 255, 255, 255},
            SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST
        );


        // SDL_Texture* getTexture()
        // {
        //     Texture* texturePtr = textureAsset.as<Texture>();
        //     return texturePtr ? texturePtr->getTexture() : nullptr;
        // }

    protected:
        friend Factory;
        friend Core;

        // SDL_Texture* texture_ = nullptr; // Deprecated: use textureAsset instead

        AssetHandle textureAsset;      // Underlying texture asset for the sprite sheet

        // Accessor for the underlying texture asset
        AssetHandle getTextureAsset() const { return textureAsset; }

        // Context string for error logging
        std::string debugTextureContext(SDL_Texture* texture) const;

        int spriteWidth_ = 8;   // Default sprite width
        int spriteHeight_ = 8;  // Default sprite height

        // Equality comparison for parameter-based reuse checks
        bool operator==(const SpriteSheet& other) const;        
 
        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         


    }; // END class SpriteSheet

    // Test-only hook to force SDL_GetTextureSize failure paths
    void SpriteSheet_ForceSizeQueryFailureForTests(bool enable);

} // namespace SDOM
