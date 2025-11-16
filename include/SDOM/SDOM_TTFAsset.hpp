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
            InitStruct()
                : IAssetObject::InitStruct()
            {
                name      = TypeName;        // "TTFAsset"
                type      = TypeName;
                filename  = "internal_ttf";  // default virtual filename
                internal_font_size = 10;     // unified internal representation
            }

            // unified internal size representation
            int internal_font_size = 10;

            // alias for convenience – optional
            int size() const { return internal_font_size; }

            // ------------------------------------------------------------
            // JSON loader — correct signature & inheritance-safe
            // ------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                // 1) Load base asset fields
                IAssetObject::InitStruct::from_json(j, out);

                // 2) Unified size handling with multiple compatibility keys
                if (j.contains("size"))
                {
                    out.internal_font_size = j["size"].get<int>();
                }
                else if (j.contains("internal_font_size"))
                {
                    out.internal_font_size = j["internal_font_size"].get<int>();
                }
                else if (j.contains("internalFontSize"))
                {
                    out.internal_font_size = j["internalFontSize"].get<int>();
                }
                // else leave default (10)
            }
        };    

    protected:
        // --- Constructors --- //
        TTFAsset(const InitStruct& init);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IAssetObject> CreateFromInitStruct(const IAssetObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const TTFAsset::InitStruct&>(baseInit);
            return std::unique_ptr<IAssetObject>(new TTFAsset(init));
        }

        static std::unique_ptr<IAssetObject> CreateFromJson(const nlohmann::json& j)
        {
            TTFAsset::InitStruct init;
            TTFAsset::InitStruct::from_json(j, init);
            return std::unique_ptr<IAssetObject>(new TTFAsset(init));
        }



        TTFAsset() = default;
        ~TTFAsset() override;
        bool onInit() override;
        void onQuit() override;
        void onLoad() override;
        void onUnload() override;
        bool onUnitTest(int frame) override;

        TTF_Font* _getTTFFontPtr() const { return ttf_font_; }
        int getFontSize() const { return internalFontSize_; }

    protected:
        friend Factory;
        friend Core;

        int internalFontSize_ = 10;           // Uniform scaling for both font types

        TTF_Font* ttf_font_ = nullptr;

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         

    }; // END class TTFAsset

} // END namespace SDOM