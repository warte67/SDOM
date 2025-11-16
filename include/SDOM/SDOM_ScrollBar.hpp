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
                isClickable = true;
                tabEnabled  = true;  // ScrollBars should be keyboard accessible

                //
                // ScrollBar colors map onto the base IDisplayObject fields so
                // downstream rendering and child ArrowButtons automatically pick
                // up the configured palette.
                color           = {128, 128, 128, 255};   // track + arrow tint
                foregroundColor = {255, 255, 255, 255};   // draggable thumb
                backgroundColor = {16, 16, 16, 128};      // surrounding backdrop
                borderColor     = {0, 0, 0, 32};          // thin outline

                //
                // IRangeControl values (override parent defaults)
                //
                min         = 0.0f;
                max         = 100.0f;
                value       = 0.0f;
                orientation = Orientation::Horizontal;

                //
                // Fix: assign to member, not create a local variable
                //
                icon_resource = "internal_icon_8x8";

                //
                // ScrollBar-specific fields
                //
                step            = 0.0f;    // 0 = continuous
                page_size       = 25.0f;   // visible content
                content_size    = 100.0f;  // total scrollable content
                min_thumb_length = 8.0f;   // minimum pixel size of thumb
            }

            float step            = 0.0f;
            float page_size       = 25.0f;
            float content_size    = 100.0f;
            float min_thumb_length = 8.0f;

            // ---------------------------------------------------------------------
            // JSON loader (inheritance-safe)
            // ---------------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                IRangeControl::InitStruct::from_json(j, out);

                if (j.contains("trackColor"))       { out.color             = json_to_color(j["trackColor"]); }
                if (j.contains("thumbColor"))       { out.foregroundColor   = json_to_color(j["thumbColor"]); }
                if (j.contains("backgroundColor"))  { out.backgroundColor   = json_to_color(j["backgroundColor"]); }
                if (j.contains("borderColor"))      { out.borderColor       = json_to_color(j["borderColor"]); }
                if (j.contains("icon_resource"))    { out.icon_resource     = j["icon_resource"].get<std::string>(); }
                if (j.contains("step"))             { out.step              = j["step"].get<float>(); }
                if (j.contains("page_size"))        { out.page_size         = j["page_size"].get<float>(); }
                if (j.contains("content_size"))     { out.content_size      = j["content_size"].get<float>(); }
                if (j.contains("min_thumb_length")) { out.min_thumb_length  = j["min_thumb_length"].get<float>(); }
            }
        }; // END: InitStruct
        
    protected:
        // --- Protected Constructors --- //
        ScrollBar(const InitStruct& init);  

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const ScrollBar::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new ScrollBar(init));
        }

        static std::unique_ptr<IDisplayObject> CreateFromJson(const nlohmann::json& j)
        {
            ScrollBar::InitStruct init;
            ScrollBar::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new ScrollBar(init));
        }


        // --- Default Constructor and Destructor --- //
        ScrollBar() = default;
        ~ScrollBar() override = default;

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
        DisplayHandle button_decrease_;
        DisplayHandle button_increase_;

        float step_ = 0.0f;             // 0.0 = continuous, >0.0 = discrete steps in units
        float page_size_ = 0.0f;        // visible content size (e.g., lines, pixels)
        float content_size_ = 0.0f;     // total content size (e.g., lines, pixels)
        float min_thumb_length_ = 8.0f; // minimum thumb length in pixels

        // --- Protected Virtual Methods --- //
        void _onValueChanged(float oldValue, float newValue) override;
        ArrowButton* button_decrease_ptr_ = nullptr;
        ArrowButton* button_increase_ptr_ = nullptr;
        

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         


    }; // END: class ScrollBar

} // END: namespace SDOM