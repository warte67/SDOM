// SDOM_TTFAsset.hpp
#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <SDOM/SDOM_IAssetObject.hpp>

namespace SDOM
{
    class Factory;
    class Core;

    // internal 8x8 bitmap font sprite sheet
    extern unsigned char internal_ttf[];
    extern int internal_ttf_len;

    class TTFAsset : public IAssetObject
    {
        using SUPER = IAssetObject;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "TTFAsset";

        // --- Initialization Struct --- //
        struct InitStruct : public IAssetObject::InitStruct
        {
            InitStruct() : IAssetObject::InitStruct() 
            { 
                name = TypeName; 
                type = TypeName;
                filename = "_default_ttf"; // Default filename, can be overridden
            }
            int internalFontSize = 10;     // Font size property for TrueType fonts
        };
    

    protected:
        // --- Constructors --- //
        TTFAsset(const InitStruct& init);
        TTFAsset(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IAssetObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IAssetObject>(new TTFAsset(config));
        }
        static std::unique_ptr<IAssetObject> CreateFromInitStruct(const IAssetObject::InitStruct& baseInit) {
            const auto& ttfInit = static_cast<const TTFAsset::InitStruct&>(baseInit);
            return std::unique_ptr<IAssetObject>(new TTFAsset(ttfInit));
        }

        TTFAsset() = default;
        virtual ~TTFAsset();
        virtual bool onInit() override;
        virtual void onQuit() override;
        virtual void onLoad() override;
        virtual void onUnload() override;
        virtual bool onUnitTest() override;

        TTF_Font* _getTTFFontPtr() const { return ttf_font_; }
        int getFontSize() const { return internalFontSize_; }

    protected:
        friend Factory;
        friend Core;

        int internalFontSize_ = 10;           // Uniform scaling for both font types

        TTF_Font* ttf_font_ = nullptr;

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END class TTFAsset

} // END namespace SDOM