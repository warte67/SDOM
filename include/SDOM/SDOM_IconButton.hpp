// SDOM_IconButton.hpp

#pragma once

#include <SDOM/SDOM_IPanelObject.hpp>
#include <SDOM/SDOM_IconIndex.hpp>
#include <SDOM/SDOM_IButtonObject.hpp>

namespace SDOM
{

    
    class IconButton : public IDisplayObject, IButtonObject
    {
        using SUPER = SDOM::IDisplayObject; 

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "IconButton";

        // --- Initialization Struct --- //
        struct InitStruct : public IDisplayObject::InitStruct
        {
            InitStruct()
                : IDisplayObject::InitStruct()
            {
                //
                // From IDisplayObject
                //
                x = 0;
                y = 0;
                width  = 8;
                height = 8;

                name  = TypeName;
                type  = TypeName;
                color = {96, 0, 96, 255};      // IconButton default purple

                tabEnabled   = false;          // no tab focus
                isClickable  = false;          // no interactions by default
                hasBackground = false;         // no panel background
                hasBorder     = false;         // no border lines

                //
                // IconButton-specific
                //
                icon_index    = IconIndex::Hamburger;
                icon_resource = "internal_icon_8x8";
                icon_width    = 8;
                icon_height   = 8;
            }

            // --- IconButton-specific members ---
            IconIndex icon_index = IconIndex::Hamburger;
            std::string icon_resource = "internal_icon_8x8";
            int icon_width  = 8;
            int icon_height = 8;

            // ---------------------------------------------------------------------
            // 📜 JSON Loader (Inheritance-Safe)
            // ---------------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                // 1. Load base IDisplayObject fields
                IDisplayObject::InitStruct::from_json(j, out);

                // 2. Load IconButton-specific fields
                if (j.contains("icon_index"))
                    out.icon_index = static_cast<IconIndex>( j["icon_index"].get<int>() );

                if (j.contains("icon_resource"))
                    out.icon_resource = j["icon_resource"].get<std::string>();

                if (j.contains("icon_width"))
                    out.icon_width = j["icon_width"].get<int>();

                if (j.contains("icon_height"))
                    out.icon_height = j["icon_height"].get<int>();
            }
        }; // END: InitStruct


    protected:
        // --- Constructors --- //
        IconButton(const InitStruct& init);  

    public:

        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const IconButton::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new IconButton(init));
        }

        static std::unique_ptr<IDisplayObject> CreateFromJson(const nlohmann::json& j)
        {
            IconButton::InitStruct init;
            IconButton::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new IconButton(init));
        }

        IconButton() = default;
        ~IconButton() override = default;     

        // --- Virtual Methods --- //
        bool onInit() override;     // Called when the display object is initialized
        void onRender() override;   // Called to render the display object
        void onQuit() override;     // Called when the display object is being destroyed
        void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        void onEvent(const Event& event) override;  // Called when an event occurs
        bool onUnitTest(int frame) override;        // Called to perform unit tests on the object

        AssetHandle getIconObject() const { return iconSpriteSheet_; }

        IconIndex getIconIndex() const { return icon_index_; }
        void setIconIndex(IconIndex index) { icon_index_ = index; }

        // Return the raw AssetHandle for the internal SpriteSheet so callers
        // can manipulate the asset handle without reaching into internals.
        AssetHandle getSpriteSheetHandle() const { return iconSpriteSheet_; }

        // Convenience: return raw SpriteSheet* (or nullptr) for quick checks.
        SpriteSheet* getSpriteSheetPtr() const { return iconSpriteSheet_.as<SpriteSheet>(); }

    protected:
        AssetHandle iconSpriteSheet_; // internal icon SpriteSheet object for IconButton
        std::string icon_resource_ = "internal_icon_8x8"; // default icon resource name
        IconIndex icon_index_ = IconIndex::Hamburger; // default icon is Hamburger menu
        int icon_width_ = 8;         // default icon width is 8
        int icon_height_ = 8;        // default icon height is 8

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;       
        
    }; // END: class IconButton : public IDisplayObject

} // END namespace SDOM