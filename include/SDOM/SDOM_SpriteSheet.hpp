// SDOM_SpriteSheet.hpp
#pragma once

#include <SDOM/SDOM_IAssetObject.hpp>
#include <SDOM/SDOM_Texture.hpp>

namespace SDOM
{
    class Factory;
    class Core;

    // internal 8x8 bitmap font sprite sheet
    extern unsigned char default_bmp_8x8[];
    extern int default_bmp_8x8_len;
    // internal 8x8 icon sprite sheet
    extern unsigned char default_icon_8x8[];
    extern int default_icon_8x8_len;

    class SpriteSheet : public IAssetObject
    {
        using SUPER = IAssetObject;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "SpriteSheet";

        // --- Initialization Struct --- //
        struct InitStruct : public IAssetObject::InitStruct
        {
            InitStruct() : IAssetObject::InitStruct() 
            { 
                name = TypeName; 
                type = TypeName;
                filename = TypeName; // Default filename, can be overridden
            }
            int spriteWidth = 8;   // Default sprite width
            int spriteHeight = 8;  // Default sprite height
        };
        
    protected:
        // --- Constructors --- //
        SpriteSheet(const InitStruct& init);
        SpriteSheet(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IAssetObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IAssetObject>(new SpriteSheet(config));
        }
        static std::unique_ptr<IAssetObject> CreateFromInitStruct(const IAssetObject::InitStruct& baseInit) {
            const auto& spriteSheetInit = static_cast<const SpriteSheet::InitStruct&>(baseInit);
            return std::unique_ptr<IAssetObject>(new SpriteSheet(spriteSheetInit));
        }

        SpriteSheet() = default;
        virtual ~SpriteSheet() = default;

        virtual bool onInit() override;
        virtual void onQuit() override;
        virtual void onLoad() override;
        virtual void onUnload() override;
        virtual bool onUnitTest() override;

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
            SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST
        );
        void drawSprite(
            int spriteIndex,
            SDL_FRect& destRect, 
            SDL_Color color = {255, 255, 255, 255}, 
            SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST
        );
        void drawSprite(
            int spriteIndex,
            const SDL_FRect& srcRect,   // Offset within the sprite tile (not the sheet)
            const SDL_FRect& dstRect,   // Destination on screen
            SDL_Color color = {255, 255, 255, 255},
            SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST
        );

        // --- Lua Wrappers --- //
        void setSpriteWidth_Lua(IAssetObject* obj, int width);
        void setSpriteHeight_Lua(IAssetObject* obj, int height);
        void setSpriteSize_Lua(IAssetObject* obj, int width, int height);
        int getSpriteWidth_Lua(IAssetObject* obj);
        int getSpriteHeight_Lua(IAssetObject* obj);
        sol::table getSpriteSize_Lua(IAssetObject* obj, sol::state_view lua);
        int getSpriteCount_Lua(IAssetObject* obj);
        int getSpriteX_Lua(IAssetObject* obj, int spriteIndex);
        int getSpriteY_Lua(IAssetObject* obj, int spriteIndex);

        void drawSprite_lua( int spriteIndex, int x, int y, SDL_Color color, SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST );    
        void drawSprite_dst_lua( int spriteIndex, SDL_FRect& destRect, SDL_Color color, SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST );

        // For the Lua extended variant the spriteIndex is the 2nd parameter after the object
        void drawSprite_ext_Lua(
            IAssetObject* obj,
            int spriteIndex,
            sol::table srcRect,                 // {x=,y=,w=,h=} or {x,y,w,h}
            sol::table dstRect,                 // {x=,y=,w=,h=} or {x,y,w,h}
            sol::object color = sol::nil,       // table {r,g,b,a} or nil
            sol::object scaleMode = sol::nil    // SDL_Utils::scaleModeFromSol(const sol::object& o)
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

        int spriteWidth_ = 8;   // Default sprite width
        int spriteHeight_ = 8;  // Default sprite height

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END class SpriteSheet

} // namespace SDOM