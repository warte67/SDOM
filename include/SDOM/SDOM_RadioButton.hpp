// SDOM_RadioButton.hpp
#pragma once

#include <SDOM/SDOM_TristateButton.hpp>

namespace SDOM
{

    using SUPER = SDOM::IDisplayObject; 

    class RadioButton : public TristateButton
    {
        using SUPER = TristateButton;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "RadioButton";

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
            bool selected = false; // whether this radio button is selected (checked) initially

        }; // END: InitStruct


    protected:
        // --- Constructors --- //
        RadioButton(const InitStruct& init);  
        RadioButton(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new RadioButton(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& RadioButtonInit = static_cast<const RadioButton::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new RadioButton(RadioButtonInit));
        }

        // --- Default Constructor and Virtual Destructor --- //
        RadioButton() = default;
        ~RadioButton() override {};

        // --- Lifecycle & Core Virtuals --- //
        bool onInit() override;
        void onQuit() override;
        void onUpdate(float fElapsedTime) override;
        void onEvent(const Event& event) override;
        void onRender() override;
        bool onUnitTest(int frame) override;

        // --- Virtual State Accessors (From IButtonObject) --- //
        ButtonState getState() const override;
        void setState(ButtonState state) override;

    protected:
        // --- Protected Virtual Methods (From IButtonObject) --- //
        void onStateChanged(ButtonState oldState, ButtonState newState) override;
        IconIndex iconIndexForState(ButtonState state) const override;

        // --- Protected Helpers --- //
        void clearSiblings_();

        // --- Data Members --- //
        bool selected_ = false; // whether this radio button is selected (checked)
        

        // ---------------------------------------------------------------------
        // 🔗 Legacy Lua Registration
        // ---------------------------------------------------------------------
        void _registerLuaBindings(const std::string& typeName, sol::state_view lua) override;


        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         

    }; // END: class RadioButton

} // END: namespace SDOM