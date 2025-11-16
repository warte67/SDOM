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
        struct InitStruct : public IRangeControl::InitStruct
        {
            InitStruct()
                : IRangeControl::InitStruct()
            {
                //
                // From IDisplayObject (overrides)
                //
                name       = TypeName;
                type       = TypeName;
                isClickable = true;
                tabEnabled  = true;   // sliders should be keyboard accessible

                //
                // Slider-specific colors for track/knob/background/border
                //
                color           = {128, 128, 128, 255};   // track
                foregroundColor = {255, 255, 255, 255};   // knob
                backgroundColor = {16, 16, 16, 128};      // background
                borderColor     = {0, 0, 0, 32};          // border

                //
                // IRangeControl fields already initialized by parent InitStruct
                // but we override to ensure slider-specific defaults
                //
                min         = 0.0f;
                max         = 100.0f;
                value       = 0.0f;
                orientation = Orientation::Horizontal;

                //
                // Fix: icon_resource must NOT be shadowed
                //
                icon_resource = "internal_icon_8x8";

                //
                // Slider-specific addition
                //
                step = 0.0f;   // 0 = continuous, >0 = discrete stepping
            }

            float step = 0.0f;   // Slider-specific

            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                //
                // 1. Load IRangeControl (and IDisplayObject) fields first
                //
                IRangeControl::InitStruct::from_json(j, out);

                //
                // 2. Slider-specific overrides
                //

                // Track color
                if (j.contains("color"))
                    out.color = json_to_color(j["color"]);

                // Knob color
                if (j.contains("foreground_color"))
                    out.foregroundColor = json_to_color(j["foreground_color"]);

                // Background color
                if (j.contains("background_color"))
                    out.backgroundColor = json_to_color(j["background_color"]);

                // Border color
                if (j.contains("border_color"))
                    out.borderColor = json_to_color(j["border_color"]);

                // Override icon pack
                if (j.contains("icon_resource"))
                    out.icon_resource = j["icon_resource"].get<std::string>();

                // Slider-only field: step interval
                if (j.contains("step"))
                    out.step = j["step"].get<float>();
            }
        }; // END: InitStruct
        
    protected:
        // --- Protected Constructors --- //
        Slider(const InitStruct& init);  

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const Slider::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Slider(init));
        }

        static std::unique_ptr<IDisplayObject> CreateFromJson(const nlohmann::json& j)
        {
            Slider::InitStruct init;
            Slider::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new Slider(init));
        }


        // --- Default Constructor and Destructor --- //
        Slider() = default;
        ~Slider() override = default;

        // --- Virtual Methods --- //
        bool onInit() override;     // Called when the display object is initialized
        void onQuit() override;     // Called when the display object is being destroyed
        void onEvent(const Event& event) override;  // Called when an event occurs
        void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        void onRender() override;   // Called to render the display object
        bool onUnitTest(int frame) override; // Unit test method

        // --- Public Accessors --- //
        float getStep() const { return step_; }

        // --- Public Mutators --- //
        void setStep(float step) { step_ = step; }

    protected:
        // --- Protected Data Members --- //
        float step_ = 0.0f; // 0.0 = continuous, >0.0 = discrete steps in units

        // --- Protected Virtual Methods --- //
        void _onValueChanged(float oldValue, float newValue) override;
        
        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;          


    }; // END: class Slider

} // END: namespace SDOM