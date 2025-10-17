// SDOM_ProgressBar.hpp
#pragma once

#include <SDOM/SDOM_IRangeControl.hpp>


namespace SDOM
{
    // --- Enums and Tables --- //

    // --- Slider Control --- //
    class ProgressBar : public IRangeControl 
    {
        using SUPER = IRangeControl;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "ProgressBar";

        // --- Initialization Struct --- //
        struct InitStruct : IRangeControl::InitStruct
        {
            InitStruct() : IRangeControl::InitStruct()
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // Icon Color
                tabEnabled = false;
                isClickable = true;
                
                // Default colors for Slider
                color = { 128, 128, 128, 255 };             // Track Color
                foregroundColor = { 255, 255, 255, 255 };   // Knob Color
                backgroundColor = { 16, 16, 16, 128 };      // Background Color
                borderColor = { 0, 0, 0, 32 };              // Border Color

                // from IRangeControl
                min = 0.0f;      // Minimum value (0.0% - 100.0%)
                max = 100.0f;    // Maximum value (0.0% - 100.0%)
                value = 0.0f;    // Current value (0.0% - 100.0%)
                orientation = Orientation::Horizontal;
                std::string icon_resource = "internal_icon_8x8"; 
            }
            // Additional ProgressBar Parameters as needed:
            // e.g., float step = 1.0f; // Step size for each increment/decrement
        }; // END: InitStruct
        
    protected:
        // --- Protected Constructors --- //
            // NOTE: when implementing a Lua constructor for a derived class you should
            // forward the derived class InitStruct() into the base-class Lua parser
            // so that derived defaults are applied when the Lua table omits keys.
            // Example:
            //   DerivedControl::DerivedControl(const sol::table& config)
            //     : IRangeControl(config, DerivedControl::InitStruct()) { }
            // This ensures color/anchor/font defaults from DerivedControl::InitStruct
            // are used by the IDisplayObject/IRangeControl parsing code.
            ProgressBar(const InitStruct& init);  
            ProgressBar(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new ProgressBar(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& ProgressBarInit = static_cast<const ProgressBar::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new ProgressBar(ProgressBarInit));
        }

        // --- Default Constructor and Destructor --- //
        ProgressBar() = default;
        virtual ~ProgressBar() = default;

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onEvent(const Event& event) override;  // Called when an event occurs
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onRender() override;   // Called to render the display object
        virtual bool onUnitTest() override; // Unit test method

        // --- Public Accessors --- //
        // Add custom getters here

        // --- Public Mutators --- //
        // Add custom setters here

    protected:
        // --- Protected Data Members --- //
        // Add custom data members here
        
        // --- Protected Virtual Methods --- //
        virtual void _onValueChanged(float oldValue, float newValue);

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);   
    }; // END: class ProgressBar

} // END: namespace SDOM