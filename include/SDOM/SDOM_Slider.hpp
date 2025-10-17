// SDOM_Slider.hpp
#pragma once

#include <SDOM/SDOM_IRangeControl.hpp>

namespace SDOM
{
    // --- Enums and Tables --- //

    // --- Slider Control --- //
    class Slider : public IRangeControl 
    {
        using SUPER = IRangeControl;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "Slider";

        // --- Initialization Struct --- //
        struct InitStruct : IRangeControl::InitStruct
        {
            InitStruct() : IRangeControl::InitStruct()
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                tabEnabled = false;
                isClickable = true;
                tabEnabled = true;
                
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
            float step = 0.0f; // 0.0 = continuous, >0.0 = discrete steps in units

        }; // END: InitStruct
        
    protected:
        // --- Protected Constructors --- //
        Slider(const InitStruct& init);  
        Slider(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new Slider(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& SliderInit = static_cast<const Slider::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Slider(SliderInit));
        }

        // --- Default Constructor and Destructor --- //
        Slider() = default;
        virtual ~Slider() = default;

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onEvent(const Event& event) override;  // Called when an event occurs
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onRender() override;   // Called to render the display object
        virtual bool onUnitTest() override; // Unit test method

        // --- Public Accessors --- //
        float getStep() const { return step_; }

        // --- Public Mutators --- //
        void setStep(float step) { step_ = step; }

    protected:
        // --- Protected Data Members --- //
        float step_ = 0.0f; // 0.0 = continuous, >0.0 = discrete steps in units

        // --- Protected Virtual Methods --- //
        virtual void _onValueChanged(float oldValue, float newValue);

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);   
    }; // END: class Slider

} // END: namespace SDOM