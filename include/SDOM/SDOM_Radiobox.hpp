// SDOM_Radiobox.hpp
#pragma once

#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>

namespace SDOM
{

    class Radiobox : public IPanelObject, IButtonObject
    {
        using SUPER = SDOM::IPanelObject; 

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "Radiobox";

        // --- Initialization Struct --- //
        struct InitStruct : IPanelObject::InitStruct
        {
            InitStruct() : IPanelObject::InitStruct() 
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // Radiobox color
                tabEnabled = false;     // Radioboxes are not tab-enabled by default
                isClickable = true;     // Radioboxes are clickable by default

                icon_resource = "internal_icon_8x8"; // Default to internal 8x8 sprite sheet
                icon_width = 8;         // default icon width is 8
                icon_height = 8;        // default icon height is 8
                font_resource = "internal_font_8x8"; // Default to internal 8x8 font 
                font_width = 8;         // default font width is 8
                font_height = 8;        // default font height is 8
            }
            IconIndex icon_index = IconIndex::Radiobox_Unselected; 
            std::string text = "Radiobox"; // default Radiobox text
            int font_size = 8;          // default font size is 8
            SDL_Color label_color = {255, 255, 255, 255}; // default label color is white
            SDL_Color border_color = {0, 0, 0, 128};               // transparent
            bool isChecked = false;     // default to unchecked
            bool border = false;        // default to no border
        }; // END: InitStruct
    protected:
        // --- Constructors --- //
        Radiobox(const InitStruct& init);  
        Radiobox(const sol::table& config);

    public:

        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new Radiobox(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& RadioboxInit = static_cast<const Radiobox::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Radiobox(RadioboxInit));
        }

        Radiobox() = default;
        virtual ~Radiobox() = default;

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onRender() override;   // Called to render the display object
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onEvent(const Event& event) override;  // Called when an event occurs

        DisplayHandle getLabelObject() const { return labelObject_; }
        std::string getText() const { return text_; }   
        void setText(const std::string& newText);
        DisplayHandle getIconButtonObject() const { return iconButtonObject_; }

        bool isChecked() const;
        void setChecked(bool checked);


    protected:
        DisplayHandle labelObject_; // internal label object for group text
        std::string text_;     // initialized label text
        std::string font_resource_ = "internal_font_8x8"; // default font resource name
        std::string icon_resource_ = "internal_icon_8x8"; // default icon resource name
        int font_size_ = 8;        // default font size
        int font_width_ = 8;       // default font width
        int font_height_ = 8;      // default font height
        bool use_border_ = false;
        bool isChecked_  = false;
        SDL_Color label_color_ = {255, 255, 255, 255}; // default label color is white
        SDL_Color border_color_ = {0, 0, 0, 128};               // transparent
        DisplayHandle iconButtonObject_; // internal icon button object for Radiobox icon
        IconIndex icon_index_ = IconIndex::Radiobox_Unselected;
        int icon_width_ = 8;        // default icon width
        int icon_height_ = 8;       // default icon height
        
        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END: class Radiobox : public IPanelObject

} // END namespace SDOM