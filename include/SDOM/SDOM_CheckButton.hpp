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
        struct InitStruct : public TristateButton::InitStruct
        {
            InitStruct()
                : TristateButton::InitStruct()
            {
                //
                // From IDisplayObject
                //
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};
                tabEnabled  = false;
                isClickable = true;

                //
                // From TristateButton — overridden defaults
                //
                text         = TypeName;
                font_size    = 8;
                label_color  = {0, 255, 255, 255};
                border_color = {0, 0, 0, 128};

                state = ButtonState::Unchecked;

                icon_resource = "internal_icon_8x8";
                icon_width    = 8;
                icon_height   = 8;

                font_resource = "internal_font_8x8";
                font_width    = 8;
                font_height   = 8;

                border = false;
            }

            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                //
                // 1. Load base hierarchy (IDisplayObject → TristateButton)
                //
                TristateButton::InitStruct::from_json(j, out);

                //
                // 2. Apply CheckButton-specific overrides
                //    (CheckButton adds no new fields, but can override inherited ones)
                //
                if (j.contains("text"))          out.text         = j["text"].get<std::string>();
                if (j.contains("font_size"))     out.font_size    = j["font_size"].get<int>();
                if (j.contains("label_color"))   out.label_color  = json_to_color(j["label_color"]);
                if (j.contains("border_color"))  out.border_color = json_to_color(j["border_color"]);
                if (j.contains("state"))         out.state        = static_cast<ButtonState>(j["state"].get<int>());

                if (j.contains("icon_resource")) out.icon_resource = j["icon_resource"].get<std::string>();
                if (j.contains("icon_width"))    out.icon_width    = j["icon_width"].get<int>();
                if (j.contains("icon_height"))   out.icon_height   = j["icon_height"].get<int>();

                if (j.contains("font_resource")) out.font_resource = j["font_resource"].get<std::string>();
                if (j.contains("font_width"))    out.font_width    = j["font_width"].get<int>();
                if (j.contains("font_height"))   out.font_height   = j["font_height"].get<int>();

                if (j.contains("border"))        out.border = j["border"].get<bool>();
            }

        }; // END InitStruct



    protected:
        // --- Constructors --- //
        CheckButton(const InitStruct& init);  

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject>
        CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const CheckButton::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new CheckButton(init));
        }

        static std::unique_ptr<IDisplayObject>
        CreateFromJson(const nlohmann::json& j)
        {
            CheckButton::InitStruct init;
            CheckButton::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new CheckButton(init));
        }


        // --- Default Constructor and Virtual Destructor --- //
        CheckButton() = default;
        ~CheckButton() override {};

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

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;     

    }; // END: class RadioButton

} // END: namespace SDOM