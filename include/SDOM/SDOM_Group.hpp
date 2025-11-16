// SDOM_Group.hpp
#pragma once

#include <SDOM/SDOM_IPanelObject.hpp>

namespace SDOM
{
    class Label;

    class Group : public IPanelObject
    {
        using SUPER = SDOM::IPanelObject; 

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "Group";

        // --- Initialization Struct --- //
        struct InitStruct : public IPanelObject::InitStruct
        {
            InitStruct()
                : IPanelObject::InitStruct()
            {
                //
                // From IDisplayObject
                //
                name        = TypeName;
                type        = TypeName;
                color       = {96, 0, 96, 255};     // purple-ish group color
                tabEnabled  = false;                // groups are NOT tab-focusable
                isClickable = false;                // groups are NOT clickable

                //
                // From IPanelObject
                //
                base_index    = PanelBaseIndex::Group;
                icon_resource = "internal_icon_8x8";
                icon_width    = 8;
                icon_height   = 8;

                font_resource = "internal_font_8x8";
                font_width    = 8;
                font_height   = 8;

                //
                // Group-specific
                //
                text        = "Group";
                font_size   = 8;
                label_color = {255, 255, 255, 255}; // white
            }

            // --- Group-specific members ---
            std::string text = "Group";
            int font_size = 8;
            SDL_Color label_color = {255, 255, 255, 255};

            // ---------------------------------------------------------------------
            // 📜 JSON Loader (mirrors Button::InitStruct::from_json)
            // ---------------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                // 1) Load inherited fields first
                IPanelObject::InitStruct::from_json(j, out);

                // 2) Load Group-specific fields
                if (j.contains("text"))
                    out.text = j["text"].get<std::string>();

                if (j.contains("font_size"))
                    out.font_size = j["font_size"].get<int>();

                if (j.contains("label_color"))
                    out.label_color = json_to_color(j["label_color"]);
            }
        }; // END: InitStruct

    protected:
        // --- Constructors --- //
        Group(const InitStruct& init);  

    public:

        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const Group::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Group(init));
        }

        static std::unique_ptr<IDisplayObject> CreateFromJson(const nlohmann::json& j)
        {
            Group::InitStruct init;
            Group::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new Group(init));
        }


        Group() = default;
        ~Group() override = default;     

        // --- Virtual Methods --- //

        bool onInit() override;                     // Called when the display object is initialized
        void onRender() override;                   // Called to render the display object
        void onQuit() override;                     // Called when the display object is being destroyed
        void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        void onEvent(const Event& event) override;  // Called when an event occurs
        bool onUnitTest(int frame) override;        // Called to perform unit tests on the object


        // --- Label Helpers (C++ / LUA)--- //

        DisplayHandle getLabel() const;                     // get the internal label object handle
        std::string getLabelText() const;                   // get the label text
        void setLabelText(const std::string& txt);          // set the label text
        SDL_Color getLabelColor() const;                    // get the label color

        int getFontSize() const;                            // Font Size Scales either Truetype or BitmapFonts
        int getFontWidth() const;                           // only relevant to BitmapFonts
        int getFontHeight() const;                          // only relevant to BitmapFonts
        void setFontSize(int s);                            // Adjust the size of either Truetype or Bitmap based fonts
        void setFontWidth(int w);                           // Adjust the width of the BitmapFont (no effect on Truetype)
        void setFontHeight(int h);                          // Adjust the height of the BitmapFont (no effect on Truetype)
        void setLabelColor(SDL_Color c);                    // set the label color


        // --- SpriteSheet Helpers (C++ / LUA)--- //           

        AssetHandle getSpriteSheet() const;                 // get the internal sprite sheet object handle
        int getSpriteWidth() const;                         // get the icon tile width
        int getSpriteHeight() const;                        // get the icon tile height
        SDL_Color getGroupColor() const;                    // get the group color
        void setGroupColor(const SDL_Color& c);             // set the group color

        // --- Raw Pointer Accessors (for C++ only) --- //

        Label* getLabelPtr() const;                         // get the internal label object pointer
        SpriteSheet* getSpriteSheetPtr() const;             // get the internal sprite sheet object pointer

    protected:
        DisplayHandle labelObject_;                         // internal label object for group text
        std::string text_;                                  // initialized label text
        std::string font_resource_ = "internal_font_8x8";   // default font resource name
        std::string icon_resource_ = "internal_icon_8x8";   // default icon resource name
        int font_size_ = 8;                                 // intended for use during initialization only
        int font_width_ = 8;                                // intended for use during initialization only
        int font_height_ = 8;                               // intended for use during initialization only
        SDL_Color label_color_ = {255, 255, 255, 255};      // default label color is white
        
        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;      

    }; // END: class Button : public IPanelObject

} // END namespace SDOM