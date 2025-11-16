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
        struct InitStruct : public IRangeControl::InitStruct
        {
            InitStruct()
                : IRangeControl::InitStruct()
            {
                //
                // From IDisplayObject
                //
                name       = TypeName;
                type       = TypeName;
                isClickable = false;  // Progress bars are typically not interactive
                tabEnabled  = false;

                //
                // ProgressBar-specific colors
                //
                trackColor      = {128, 128, 128, 255};  // background track
                fillColor       = {255, 255, 255, 255};  // progress fill
                backgroundColor = {16, 16, 16, 128};     // behind everything
                borderColor     = {0, 0, 0, 32};         // thin border

                //
                // IRangeControl fields — overridden defaults
                //
                min         = 0.0f;
                max         = 100.0f;
                value       = 0.0f;
                orientation = Orientation::Horizontal;

                //
                // Fix: assign to the **member**, not a local
                //
                icon_resource = "internal_icon_8x8";
            }

            //
            // ProgressBar-specific visual fields
            //
            SDL_Color trackColor      = {128,128,128,255};
            SDL_Color fillColor       = {255,255,255,255};
            SDL_Color backgroundColor = {16,16,16,128};
            SDL_Color borderColor     = {0,0,0,32};

            // ---------------------------------------------------------------------
            // JSON loader (inheritance-safe)
            // ---------------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                IRangeControl::InitStruct::from_json(j, out);

                if (j.contains("trackColor"))       { out.trackColor      = json_to_color(j["trackColor"]); }
                if (j.contains("fillColor"))        { out.fillColor       = json_to_color(j["fillColor"]); }
                if (j.contains("backgroundColor"))  { out.backgroundColor = json_to_color(j["backgroundColor"]); }
                if (j.contains("borderColor"))      { out.borderColor     = json_to_color(j["borderColor"]); }
                if (j.contains("icon_resource"))    { out.icon_resource   = j["icon_resource"].get<std::string>(); }
            }
        }; // END: InitStruct
        
    protected:
        // --- Protected Constructors --- //
        ProgressBar(const InitStruct& init);  

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const ProgressBar::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new ProgressBar(init));
        }

        static std::unique_ptr<IDisplayObject> CreateFromJson(const nlohmann::json& j)
        {
            ProgressBar::InitStruct init;
            ProgressBar::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new ProgressBar(init));
        }


        // --- Default Constructor and Destructor --- //
        ProgressBar() = default;
        ~ProgressBar() override = default;

        // --- Virtual Methods --- //
        bool onInit() override;     // Called when the display object is initialized
        void onQuit() override;     // Called when the display object is being destroyed
        void onEvent(const Event& event) override;  // Called when an event occurs
        void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        void onRender() override;   // Called to render the display object
        bool onUnitTest(int frame) override; // Unit test method

        // --- Public Accessors --- //
        // Add custom getters here

        // --- Public Mutators --- //
        // Add custom setters here

    protected:
        // --- Protected Data Members --- //
        // Add custom data members here
        
        // --- Protected Virtual Methods --- //
        void _onValueChanged(float oldValue, float newValue) override;

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         
        
    }; // END: class ProgressBar

} // END: namespace SDOM