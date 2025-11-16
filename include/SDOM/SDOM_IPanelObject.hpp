// SDOM_IPanelObject.hpp

#pragma once

#include "SDOM/SDOM_SpriteSheet.hpp"
#include "SDOM/SDOM_IDisplayObject.hpp"

namespace SDOM
{   
    class IFontObject;
    class AssetHandle;

    enum class PanelBaseIndex
    {
        ButtonUp = 0,
        ButtonUpSelected = 9,
        ButtonDown = 18,
        ButtonDownSelected = 27,
        Frame = 36,
        Group = 45,
        // **Individual Indices:**
        // Hamburger = 54,          Left_Arrow_Raised,          Right_Arrow_Raised,
        // Up_Arrow_Raised,         Down_Arrow_Raised,          Left_Arrow_Depressed,
        // Right_Arrow_Depressed,   Up_Arrow_Depressed,         Down_Arrow_Depressed,
        // Horizontal_Knob,         Vertical_Knob,              Slider_Tick,
        // HSlider_Rail,            VSlider_Rail,               Empty_Checkbox,
        // Checked_Checkbox,        X_Checkbox,                 Unselected_Radio,
        // Selected_Radio,          Left_HProgress,             Empty_HProgress,
        // Thumb_HProgress,         Right_HProgress,            Top_VProgress,
        // Empty_VProgress,         Thumb_VProgress,            Bottom_VProgress
    };
    inline static const std::unordered_map<std::string, PanelBaseIndex> stringToPanelBaseIndex_ = 
    {
        { "button_up",          PanelBaseIndex::ButtonUp },
        { "button_up_selected", PanelBaseIndex::ButtonUpSelected },
        { "button_down",        PanelBaseIndex::ButtonDown },
        { "button_down_selected", PanelBaseIndex::ButtonDownSelected },
        { "frame",              PanelBaseIndex::Frame },
        { "group",              PanelBaseIndex::Group }
    };
    inline static const std::unordered_map<PanelBaseIndex, std::string> panelBaseIndexToString_ = 
    {
        { PanelBaseIndex::ButtonUp,         "button_up" },
        { PanelBaseIndex::ButtonUpSelected, "button_up_selected" },
        { PanelBaseIndex::ButtonDown,       "button_down" },
        { PanelBaseIndex::ButtonDownSelected, "button_down_selected" },
        { PanelBaseIndex::Frame,            "frame" },
        { PanelBaseIndex::Group,            "group" }
    };

    enum class PanelTileOffset
    {
        TopLeft = 0,
        TopCenter = 1,
        TopRight = 2,
        LeftCenter = 3,
        Center = 4,
        RightCenter = 5,
        BottomLeft = 6,
        BottomCenter = 7,
        BottomRight = 8
    };
    inline static const std::unordered_map<std::string, PanelTileOffset> stringToPanelTileOffset_ = 
    {
        { "top_left",     PanelTileOffset::TopLeft },
        { "top_center",   PanelTileOffset::TopCenter },
        { "top_right",    PanelTileOffset::TopRight },
        { "left_center",  PanelTileOffset::LeftCenter },
        { "center",       PanelTileOffset::Center },
        { "right_center", PanelTileOffset::RightCenter },
        { "bottom_left",  PanelTileOffset::BottomLeft },
        { "bottom_center",PanelTileOffset::BottomCenter },
        { "bottom_right", PanelTileOffset::BottomRight }
    };
    inline static const std::unordered_map<PanelTileOffset, std::string> panelTileOffsetToString_ = 
    {
        { PanelTileOffset::TopLeft,      "top_left" },
        { PanelTileOffset::TopCenter,    "top_center" },
        { PanelTileOffset::TopRight,     "top_right" },
        { PanelTileOffset::LeftCenter,   "left_center" },
        { PanelTileOffset::Center,       "center" },
        { PanelTileOffset::RightCenter,  "right_center" },
        { PanelTileOffset::BottomLeft,   "bottom_left" },
        { PanelTileOffset::BottomCenter, "bottom_center" },
        { PanelTileOffset::BottomRight,  "bottom_right" }
    };

    class IPanelObject : public IDisplayObject 
    {
        using SUPER = IDisplayObject;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "IPanelObject";

        // --- Initialization Struct --- //
        struct InitStruct : public IDisplayObject::InitStruct
        {
            InitStruct()
                : IDisplayObject::InitStruct()
            {
                //
                // Base DisplayObject defaults
                //
                name        = TypeName;
                type        = TypeName;
                color       = {96, 0, 96, 255}; // default panel tint

                //
                // Panel-specific defaults
                //
                base_index     = PanelBaseIndex::Frame;

                icon_resource  = "internal_icon_8x8";
                icon_width     = 8;
                icon_height    = 8;

                font_resource  = "internal_font_8x8";
                font_width     = 8;
                font_height    = 8;
            }

            //
            // IPanelObject expansion fields
            //
            PanelBaseIndex base_index = PanelBaseIndex::Frame;

            std::string icon_resource = "internal_icon_8x8";
            int icon_width            = 8;
            int icon_height           = 8;

            std::string font_resource = "internal_font_8x8";
            int font_width            = 8;
            int font_height           = 8;

            // ------------------------------------------------------------
            // JSON loader — correct signature & inheritance-safe
            // ------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                // --- 1) Parse all inherited DisplayObject fields first ---
                IDisplayObject::InitStruct::from_json(j, out);

                // --- 2) Parse Panel-specific fields ---
                
                // Panel color override
                if (j.contains("color"))
                    out.color = json_to_color(j["color"]);

                // base_index: string → enum
                if (j.contains("base_index"))
                {
                    std::string s = j["base_index"].get<std::string>();
                    auto it = stringToPanelBaseIndex_.find(s);
                    if (it != stringToPanelBaseIndex_.end())
                        out.base_index = it->second;
                }

                // icon settings
                if (j.contains("icon_resource"))
                    out.icon_resource = j["icon_resource"].get<std::string>();

                if (j.contains("icon_width"))
                    out.icon_width = j["icon_width"].get<int>();

                if (j.contains("icon_height"))
                    out.icon_height = j["icon_height"].get<int>();

                // font settings
                if (j.contains("font_resource"))
                    out.font_resource = j["font_resource"].get<std::string>();

                if (j.contains("font_width"))
                    out.font_width = j["font_width"].get<int>();

                if (j.contains("font_height"))
                    out.font_height = j["font_height"].get<int>();
            }
        }; // END: struct InitStruct

    protected:
        // --- Constructors --- //
        IPanelObject(const InitStruct& init);  
        // IPanelObject();

    public:

        // // --- Static Factory Methods --- //
        // static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
        //     return std::unique_ptr<IDisplayObject>(new IPanelObject(config));
        // }
        // static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
        //     const auto& panelInit = static_cast<const IPanelObject::InitStruct&>(baseInit);
        //     return std::unique_ptr<IDisplayObject>(new IPanelObject(panelInit));
        // }

        ~IPanelObject() override = default;     

        // --- Virtual Methods --- //
        bool onInit() override;                 // Called when the display object is initialized
        bool onLoad() override;                 // Allocate/refresh GPU resources after device rebuild
        void onUnload() override;               // Release GPU resources before device teardown
        void onRender() override;               // Called to render the display object
        void onQuit() override;                 // Called when the display object is being destroyed
        void onUpdate(float fElapsedTime) override =0;    // Called every frame to update the display object
        void onEvent(const Event& event) override =0;     // Called when an event occurs
        void onWindowResize(int logicalWidth, int logicalHeight) override;
        bool onUnitTest(int frame) override { (void)frame; return true; }

        // --- Helper Methods --- // 
        void renderPanel(); // render nine-panel based on the base_index_;

        AssetHandle getSpriteSheet() const { return spriteSheetAsset_; }
        SpriteSheet* getSpriteSheetPtr() const { return spriteSheetAsset_->as<SpriteSheet>(); }

        AssetHandle getFontAsset() const { return fontAsset_; }
        std::string getIconResourceName() const { return icon_resource_; }
        std::string getFontResourceName() const { return font_resource_; }
        int getIconWidth() const { return icon_width_; }
        int getIconHeight() const { return icon_height_; }
        int getFontWidth() const { return font_width_; }
        int getFontHeight() const { return font_height_; }
        PanelBaseIndex getBaseIndex() const { return base_index_; }

    protected:
        // std::string name_;       // Name for this panel object (inherited from IDisplayObject)
        AssetHandle spriteSheetAsset_;   // icon based sprite sheet, default: `internal_icon_8x8`
        AssetHandle fontAsset_;     // IFontObject based font resource, default: `internal_font_8x8`
        std::string icon_resource_ = "internal_icon_8x8"; // Default to internal 8x8 sprite sheet
        std::string font_resource_ = "internal_font_8x8"; // Default to internal 8x8 font
        int icon_width_ = 8;        // default icon width is 8
        int icon_height_ = 8;       // default icon height is 8
        int font_width_ = 8;        // default font width is 8
        int font_height_ = 8;       // default font height is 8
        PanelBaseIndex base_index_ = PanelBaseIndex::ButtonUp;
        PanelBaseIndex last_base_index_ = PanelBaseIndex::ButtonUp;

        SDL_Texture* cachedTexture_ = nullptr;
        SDL_Renderer* cached_renderer_ = nullptr; // renderer that created cachedTexture_
        int current_width_ = 0;
        int current_height_ = 0;
        SDL_PixelFormat current_pixel_format_ = SDL_PIXELFORMAT_UNKNOWN;

        bool rebuildPanelTexture_(int width, int height, SDL_PixelFormat fmt);
 
        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         
        
    }; // END: IPanelObject : public IDisplayObject 

} // namespace SDOM
// END SDOM_IPanelObject.hpp
