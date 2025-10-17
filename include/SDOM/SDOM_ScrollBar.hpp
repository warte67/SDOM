// SDOM_ScrollBar.hpp
#pragma once

#include <SDOM/SDOM_IRangeControl.hpp>
#include <SDOM/SDOM_ArrowButton.hpp>



namespace SDOM
{
    // --- Enums and Tables --- //

    // --- Slider Control --- //
    class ScrollBar : public IRangeControl 
    {
        using SUPER = IRangeControl;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "ScrollBar";

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
                // Default colors for ScrollBar
                color = { 128, 128, 128, 255 };             // Track and Arrow Color
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
            float step = 0.0f;              // 0.0 = continuous, >0.0 = discrete steps in units
            float page_size = 25.0f;        // visible content size (e.g., lines, pixels)
            float content_size = 100.0f;    // total content size (e.g., lines, pixels)
            float min_thumb_length = 8.0f;  // minimum thumb length in pixels
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
            ScrollBar(const InitStruct& init);  
            ScrollBar(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new ScrollBar(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& ScrollBarInit = static_cast<const ScrollBar::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new ScrollBar(ScrollBarInit));
        }

        // --- Default Constructor and Destructor --- //
        ScrollBar() = default;
        virtual ~ScrollBar() = default;

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
        DisplayHandle button_decrease_;
        DisplayHandle button_increase_;

        float step_ = 0.0f;             // 0.0 = continuous, >0.0 = discrete steps in units
        float page_size_ = 0.0f;        // visible content size (e.g., lines, pixels)
        float content_size_ = 0.0f;     // total content size (e.g., lines, pixels)
        float min_thumb_length_ = 8.0f; // minimum thumb length in pixels

        // --- Protected Virtual Methods --- //
        virtual void _onValueChanged(float oldValue, float newValue);
        ArrowButton* button_decrease_ptr_ = nullptr;
        ArrowButton* button_increase_ptr_ = nullptr;

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);   
    }; // END: class ScrollBar

} // END: namespace SDOM