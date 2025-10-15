// SDOM_IconButton.hpp

#pragma once

#include <SDOM/SDOM_IPanelObject.hpp>

namespace SDOM
{
    enum class IconIndex 
    {
        // Individual Indices
        Hamburger = 54,
        Arrow_Left_Raised,
        Arrow_Right_Raised,
        Arrow_Up_Raised,
        Arrow_Down_Raised,
        Arrow_Left_Depressed,
        Arrow_Right_Depressed,
        Arrow_Up_Depressed,
        Arrow_Down_Depressed,
        Knob_Horizontal,
        Knob_Vertical,
        Slider_Tick,
        HSlider_Rail,
        VSlider_Rail,
        Checkbox_Empty,
        Checkbox_Checked,
        Checkbox_X,
        Radiobox_Unselected,
        Radiobox_Selected,
        HProgress_Left,
        HProgress_Empty,
        HProgress_Thumb,
        HProgress_Right,
        VProgress_Top,
        VProgress_Empty,
        VProgress_Thumb,
        VProgress_Bottom
    };

    inline static const std::unordered_map<int, std::string> icon_index_to_string = {
        { static_cast<int>(IconIndex::Hamburger), "hamburger" },
        { static_cast<int>(IconIndex::Arrow_Left_Raised), "arrow_left_raised" },
        { static_cast<int>(IconIndex::Arrow_Right_Raised), "arrow_right_raised" },
        { static_cast<int>(IconIndex::Arrow_Up_Raised), "arrow_up_raised" },
        { static_cast<int>(IconIndex::Arrow_Down_Raised), "arrow_down_raised" },
        { static_cast<int>(IconIndex::Arrow_Left_Depressed), "arrow_left_depressed" },
        { static_cast<int>(IconIndex::Arrow_Right_Depressed), "arrow_right_depressed" },
        { static_cast<int>(IconIndex::Arrow_Up_Depressed), "arrow_up_depressed" },
        { static_cast<int>(IconIndex::Arrow_Down_Depressed), "arrow_down_depressed" },
        { static_cast<int>(IconIndex::Knob_Horizontal), "knob_horizontal" },
        { static_cast<int>(IconIndex::Knob_Vertical), "knob_vertical" },
        { static_cast<int>(IconIndex::Slider_Tick), "slider_tick" },
        { static_cast<int>(IconIndex::HSlider_Rail), "hslider_rail" },
        { static_cast<int>(IconIndex::VSlider_Rail), "vslider_rail" },
        { static_cast<int>(IconIndex::Checkbox_Empty), "empty_checkbox" },
        { static_cast<int>(IconIndex::Checkbox_Checked), "checked_checkbox" },
        { static_cast<int>(IconIndex::Checkbox_X), "x_checkbox" },
        { static_cast<int>(IconIndex::Radiobox_Unselected), "radiobox_unselected" },
        { static_cast<int>(IconIndex::Radiobox_Selected), "radiobox_selected" },
        { static_cast<int>(IconIndex::HProgress_Left), "hprogress_left" },
        { static_cast<int>(IconIndex::HProgress_Empty), "hprogress_empty" },
        { static_cast<int>(IconIndex::HProgress_Thumb), "hprogress_thumb" },
        { static_cast<int>(IconIndex::HProgress_Right), "hprogress_right" },
        { static_cast<int>(IconIndex::VProgress_Top), "vprogress_top" },
        { static_cast<int>(IconIndex::VProgress_Empty), "vprogress_empty" },
        { static_cast<int>(IconIndex::VProgress_Thumb), "vprogress_thumb" },
        { static_cast<int>(IconIndex::VProgress_Bottom), "vprogress_bottom" }
    };

    inline static const std::unordered_map<std::string, int> icon_string_to_index = []{
        std::unordered_map<std::string,int> m;
        for (auto &p : icon_index_to_string) m.emplace(p.second, p.first);
        return m;
    }();

    inline static std::optional<std::string> icon_name_from_index(IconIndex idx)
    {
        auto it = icon_index_to_string.find(static_cast<int>(idx));
        if (it == icon_index_to_string.end()) return std::nullopt;
        return it->second;
    }

    inline static std::optional<IconIndex> icon_index_from_name(const std::string &name)
    {
        auto it = icon_string_to_index.find(name);
        if (it == icon_string_to_index.end()) return std::nullopt;
        return static_cast<IconIndex>(it->second);
    }

    
    class IconButton : public IDisplayObject
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

        AssetHandle getIconObject() const { return iconSpriteSheet_; }

        IconIndex getIconIndex() const { return icon_index_; }
        void setIconIndex(IconIndex index) { icon_index_ = index; }

        // Return the raw AssetHandle for the internal SpriteSheet so callers
        // can manipulate the asset handle without reaching into internals.
        AssetHandle getSpriteSheetHandle() const { return iconSpriteSheet_; }

        // Convenience: return raw SpriteSheet* (or nullptr) for quick checks.
        SpriteSheet* getSpriteSheet() const { return iconSpriteSheet_.as<SpriteSheet>(); }

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