// SDOM_Group.hpp
#pragma once

#include <SDOM/SDOM_IPanelObject.hpp>

namespace SDOM
{

    class Group : public IPanelObject
    {
        using SUPER = SDOM::IPanelObject; 

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "Group";

        // --- Initialization Struct --- //
        struct InitStruct : IPanelObject::InitStruct
        {
            InitStruct() : IPanelObject::InitStruct() 
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // Group color
                tabEnabled = false;     // groups are not tab-enabled by default   
                isClickable = false;    // groups are not clickable by default
                // from IPanelObject
                base_index = PanelBaseIndex::Group; 
                icon_resource = "internal_icon_8x8"; // Default to internal 8x8 sprite sheet
                icon_width = 8;         // default icon width is 8
                icon_height = 8;        // default icon height is 8
                font_resource = "internal_font_8x8"; // Default to internal 8x8 font 
                font_width = 8;         // default font width is 8
                font_height = 8;        // default font height is 8
            }
            std::string text = "Group"; // default group text
            int font_size = 8;          // default font size is 8
            SDL_Color label_color_ = {255, 255, 255, 255}; // default label color is white

        }; // END: InitStruct
    protected:
        // --- Constructors --- //
        Group(const InitStruct& init);  
        Group(const sol::table& config);

    public:

        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new Group(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& groupInit = static_cast<const Group::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Group(groupInit));
        }

        Group() = default;
        virtual ~Group() = default;     

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onRender() override;   // Called to render the display object
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onEvent(const Event& event) override;  // Called when an event occurs

        DisplayHandle getLabelObject() const { return labelObject_; }

    protected:
        DisplayHandle labelObject_; // internal label object for group text
        std::string text_;     // initialized label text
        std::string font_resource_ = "internal_font_8x8"; // default font resource name
        std::string icon_resource_ = "internal_icon_8x8"; // default icon resource name
        int font_size_ = 8;        // default font size
        int font_width_ = 8;       // default font width
        int font_height_ = 8;      // default font height
        SDL_Color label_color_ = {255, 255, 255, 255}; // default label color is white

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END: class Button : public IPanelObject

} // END namespace SDOM