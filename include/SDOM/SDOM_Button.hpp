// SDOM_Button.hpp
#pragma once

#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>

namespace SDOM
{

    class Button : public IPanelObject, public IButtonObject
    {
        using SUPER = SDOM::IPanelObject; 

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "Button";

        // --- Initialization Struct --- //
        struct InitStruct : IPanelObject::InitStruct
        {
            InitStruct() : IPanelObject::InitStruct() 
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // panel color
                tabEnabled = true;      // buttons are tab-enabled by default   
                // from IPanelObject
                base_index = PanelBaseIndex::ButtonUp; 
                icon_resource = "internal_icon_8x8"; // Default to internal 8x8 sprite sheet
                icon_width = 8;         // default icon width is 8
                icon_height = 8;        // default icon height is 8
                font_resource = "internal_font_8x8"; // Default to internal 8x8 font 
                font_width = 8;         // default font width is 8
                font_height = 8;        // default font height is 8
            }
            std::string text = "Button"; // default button text
            int font_size = 8;          // default font size is 8

        }; // END: InitStruct
    protected:
        // --- Constructors --- //
        Button(const InitStruct& init);  
        Button(const sol::table& config);

    public:

        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new Button(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& buttonInit = static_cast<const Button::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Button(buttonInit));
        }

        Button() = default;
        virtual ~Button() = default;     

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onRender() override;   // Called to render the display object
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onEvent(const Event& event) override;  // Called when an event occurs

        DisplayHandle getLabelObject() const { return labelObject_; }

    protected:
        DisplayHandle labelObject_; // internal label object for button text
        std::string text_;     // initialized label text
        std::string font_resource_name_ = "internal_font_8x8"; // default font resource name
        int font_size_ = 8;        // default font size
        int font_width_ = 8;       // default font width
        int font_height_ = 8;      // default font height

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END: class Button : public IPanelObject

} // END namespace SDOM