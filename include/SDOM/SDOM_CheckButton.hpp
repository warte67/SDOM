// SDOM_CheckButton.hpp
#pragma once

#include <SDOM/SDOM_TristateButton.hpp>

namespace SDOM
{

    using SUPER = SDOM::IDisplayObject; 

    class CheckButton : public TristateButton
    {
        using SUPER = TristateButton;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "CheckButton";

        // --- Initialization Struct --- //
        struct InitStruct : TristateButton::InitStruct
        {
            InitStruct() : TristateButton::InitStruct()
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // Icon Color
                tabEnabled = false;
                isClickable = true;
                // from TristateButton 
                text = TypeName; 
                font_size = 8;          // default font size is 8
                label_color = {0, 255, 255, 255}; // default label color is white
                border_color = {0, 0, 0, 128};               // transparent
                state = ButtonState::Unchecked;
                icon_resource = "internal_icon_8x8"; // Default to internal 8x8 sprite sheet
                // IconIndex icon_index = IconIndex::Checkbox_Empty;
                icon_width = 8;
                icon_height = 8;
                font_resource = "internal_font_8x8"; 
                font_width = 8;
                font_height = 8;
                border = false;        // default to no border
            }
            // Additional CheckButton-specific initialization parameters can be added here in the future.

        }; // END: InitStruct


    protected:
        // --- Constructors --- //
        CheckButton(const InitStruct& init);  
        CheckButton(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new CheckButton(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& CheckButtonInit = static_cast<const CheckButton::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new CheckButton(CheckButtonInit));
        }

        // --- Default Constructor and Virtual Destructor --- //
        CheckButton() = default;
        virtual ~CheckButton() {};

        // --- Lifecycle & Core Virtuals --- //
        virtual bool onInit() override;
        virtual void onQuit() override;
        virtual void onUpdate(float fElapsedTime) override;
        virtual void onEvent(const Event& event) override;
        virtual void onRender() override;
        virtual bool onUnitTest() override;

        // --- Virtual State Accessors (From IButtonObject) --- //
        virtual ButtonState getState() const;
        virtual void setState(ButtonState state);

    protected:
        // --- Protected Virtual Methods (From IButtonObject) --- //
        virtual void onStateChanged(ButtonState oldState, ButtonState newState) override;
        virtual IconIndex iconIndexForState(ButtonState state) const override;

        // --- Data Members --- //
        // ...

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END: class TristateButton

} // END: namespace SDOM