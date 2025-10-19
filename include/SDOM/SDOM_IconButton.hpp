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
        struct InitStruct : IDisplayObject::InitStruct
        {
            InitStruct() : IDisplayObject::InitStruct() 
            { 
                // from IDisplayObject
                x = 0;
                y = 0;
                width = 8;             // default width is 8
                height = 8;            // default height is 8
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // IconButton color
                tabEnabled = false;     // IconButtons are not tab-enabled by default   
                isClickable = false;    // IconButtons are not clickable by default
            }
            IconIndex icon_index = IconIndex::Hamburger; // default icon is Hamburger menu
            std::string icon_resource = "internal_icon_8x8"; // Default to internal 8x8 sprite sheet
            int icon_width = 8;         // default icon width is 8
            int icon_height = 8;        // default icon height is 8

        }; // END: InitStruct
    protected:
        // --- Constructors --- //
        IconButton(const InitStruct& init);  
        IconButton(const sol::table& config);
        // Defaults-aware Lua constructor so derived classes can forward their InitStruct()
        IconButton(const sol::table& config, const InitStruct& defaults);

    public:

        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new IconButton(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& IconButtonInit = static_cast<const IconButton::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new IconButton(IconButtonInit));
        }

        IconButton() = default;
        virtual ~IconButton() = default;     

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onRender() override;   // Called to render the display object
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onEvent(const Event& event) override;  // Called when an event occurs
        virtual bool onUnitTest() override; // Called to perform unit tests on the object

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

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END: class IconButton : public IDisplayObject

} // END namespace SDOM