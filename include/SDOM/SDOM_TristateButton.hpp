/**
 * @class SDOM::TristateButton
 * @brief Three-state UI button combining an icon and label within the SDOM hierarchy.
 * @author Jay Faries (https://github.com/warte67)
 * @inherits SDOM::IDisplayObject, SDOM::IButtonObject
 * @luaType TristateButton
 *
 * The `TristateButton` class represents a composite display object that combines
 * an `IconButton` and a `Label` to create a fully functional tri-state toggle
 * control (`Unchecked`, `Checked`, `Mixed`). It inherits both the display
 * behavior of `IDisplayObject` and the logical button interface from
 * `IButtonObject`, enabling unified input handling, focus, and rendering.
 *
 * Typical use cases include toggle buttons, checkbox-like controls, and
 * multi-state options where visual feedback (icon + text) is important.
 *
 * ---
 * @section TristateButton_Properties Properties
 * | Property | Type | Description |
 * |-----------|------|-------------|
 * | `text` | `std::string` | The label text displayed beside the icon. |
 * | `font_resource` | `std::string` | Name of the font resource used by the label. |
 * | `icon_resource` | `std::string` | Name of the icon sprite sheet used by the internal icon button. |
 * | `font_size` | `int` | Logical point size for label font rendering. |
 * | `font_width` / `font_height` | `int` | Pixel dimensions of each character cell. |
 * | `icon_width` / `icon_height` | `int` | Pixel dimensions of the icon. |
 * | `label_color` | `SDL_Color` | RGBA color of the label text. |
 * | `border_color` | `SDL_Color` | RGBA color of the border (if enabled). |
 * | `use_border` | `bool` | Whether a border should be rendered around the button. |
 * | `state` | `ButtonState` | Logical tri-state of the button (`Inactive`, `Active`, `Mixed`). |
 *
 * ---
 * @section TristateButton_Functions Functions
 * | Function | Returns | Description |
 * |-----------|----------|-------------|
 * | `getText()` | `std::string` | Returns the current label text. |
 * | `setText(value)` | `void` | Updates the label text. |
 * | `getLabelObject()` | `DisplayHandle` | Returns the internal label object. |
 * | `getIconButtonObject()` | `DisplayHandle` | Returns the internal icon button display handle. |
 * | `getIconButton()` | `IconButton*` | Returns a raw pointer to the internal `IconButton`. |
 * | `getIconSpriteSheet()` | `SpriteSheet*` | Retrieves the sprite sheet used for the icon. |
 * | `getLabel()` | `Label*` | Retrieves the label object used for text rendering. |
 * | `getFontResource()` / `getIconResource()` | `std::string` | Returns associated resource names. |
 * | `getFontSize()` / `getFontWidth()` / `getFontHeight()` | `int` | Font dimension accessors. |
 * | `getIconWidth()` / `getIconHeight()` | `int` | Icon dimension accessors. |
 * | `getLabelColor()` / `getBorderColor()` | `SDL_Color` | Color accessors. |
 * | `getUseBorder()` | `bool` | Returns whether a border is used. |
 * | `getIconIndex()` | `IconIndex` | Returns the current icon index representing the button’s visual state. |
 * | `onStateChanged(old, new)` | `virtual void` | Invoked whenever the tri-state value changes. |
 * | `_registerLuaBindings(typeName, lua)` | `virtual void` | Registers Lua bindings for this type. |
 *
 * ---
 * @section TristateButton_Events Events
 * | Event | Description |
 * |--------|-------------|
 * | `StateChanged` | Fired whenever the logical state changes (e.g., Unchecked → Checked). |
 * | `Click` | Fired when the user activates the button via mouse or keyboard. |
 *
 * ---
 * @section TristateButton_Notes Notes
 * - Combines icon and label child objects; both are internally managed and attached to the display hierarchy.
 * - Derived from both `IDisplayObject` (for layout/rendering) and `IButtonObject` (for logical state).
 * - Supports full Lua binding exposure, allowing property-based control in scripts.
 * - `onStateChanged()` and `iconIndexForState()` are designed to be overridden for skinning or behavior customization.
 *
 * ---
 * @section License ZLIB License
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include <string>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_IconIndex.hpp>
#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_Label.hpp>

namespace SDOM
{
    // 🪣 Registered Factory Type: TristateButton
    class TristateButton : public IDisplayObject, public IButtonObject
    {
        using SUPER = SDOM::IDisplayObject;

    public:
        // --------------------------------------------------------------------
        // 🧩 Type Info
        // --------------------------------------------------------------------
        static constexpr const char* TypeName = "TristateButton";

        // --------------------------------------------------------------------
        // ⚙️ Initialization Struct
        // --------------------------------------------------------------------
        struct InitStruct : public IDisplayObject::InitStruct
        {
            InitStruct()
                : IDisplayObject::InitStruct()
            {
                //
                // From IDisplayObject
                //
                name = TypeName;
                type = TypeName;

                color = {96, 0, 96, 255};     // default purple-ish UI color
                tabEnabled  = false;
                isClickable = true;

                //
                // TristateButton-specific defaults
                //
                text          = TypeName;
                font_size     = 8;
                label_color   = {0, 255, 255, 255};   // cyan-ish label
                border_color  = {0, 0, 0, 128};       // semi-transparent black
                state         = ButtonState::Unchecked;

                icon_resource = "internal_icon_8x8";
                icon_width    = 8;
                icon_height   = 8;

                font_resource = "internal_font_8x8";
                font_width    = 8;
                font_height   = 8;

                border        = false;
            }

            // --- TristateButton-specific fields ---
            std::string text;
            int font_size;
            SDL_Color label_color;
            SDL_Color border_color;
            ButtonState state;

            std::string icon_resource;
            int icon_width;
            int icon_height;

            std::string font_resource;
            int font_width;
            int font_height;

            bool border;

            // ---------------------------------------------------------------------
            // 📜 JSON Loader (Inheritance-Safe)
            // ---------------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                // 1. Load base IDisplayObject values
                IDisplayObject::InitStruct::from_json(j, out);

                // 2. Load TristateButton-specific values

                if (j.contains("text"))
                    out.text = j["text"].get<std::string>();

                if (j.contains("font_size"))
                    out.font_size = j["font_size"].get<int>();

                if (j.contains("label_color") && j["label_color"].is_array() && j["label_color"].size() == 4)
                {
                    const auto& c = j["label_color"];
                    out.label_color = {
                        c[0].get<uint8_t>(),
                        c[1].get<uint8_t>(),
                        c[2].get<uint8_t>(),
                        c[3].get<uint8_t>()
                    };
                }

                if (j.contains("border_color") && j["border_color"].is_array() && j["border_color"].size() == 4)
                {
                    const auto& c = j["border_color"];
                    out.border_color = {
                        c[0].get<uint8_t>(),
                        c[1].get<uint8_t>(),
                        c[2].get<uint8_t>(),
                        c[3].get<uint8_t>()
                    };
                }

                if (j.contains("state"))
                    out.state = static_cast<ButtonState>( j["state"].get<int>() );

                if (j.contains("icon_resource"))
                    out.icon_resource = j["icon_resource"].get<std::string>();

                if (j.contains("icon_width"))
                    out.icon_width = j["icon_width"].get<int>();

                if (j.contains("icon_height"))
                    out.icon_height = j["icon_height"].get<int>();

                if (j.contains("font_resource"))
                    out.font_resource = j["font_resource"].get<std::string>();

                if (j.contains("font_width"))
                    out.font_width = j["font_width"].get<int>();

                if (j.contains("font_height"))
                    out.font_height = j["font_height"].get<int>();

                if (j.contains("border"))
                    out.border = j["border"].get<bool>();
            }
        }; // END: InitStruct

    protected:
        // --------------------------------------------------------------------
        // 🏭 Constructors
        // --------------------------------------------------------------------
        TristateButton(const InitStruct& init);

    public:
        // --------------------------------------------------------------------
        // 🏭 Static Factory Methods
        // --------------------------------------------------------------------
        static std::unique_ptr<IDisplayObject>
        CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const TristateButton::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new TristateButton(init));
        }

        static std::unique_ptr<IDisplayObject>
        CreateFromJson(const nlohmann::json& j)
        {
            TristateButton::InitStruct init;
            TristateButton::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new TristateButton(init));
        }



        // --------------------------------------------------------------------
        // 🌱 Lifecycle
        // --------------------------------------------------------------------
        TristateButton() = default;
        ~TristateButton() override = default;

        bool onInit() override;
        void onQuit() override;
        void onUpdate(float fElapsedTime) override;
        void onEvent(const Event& event) override;
        void onRender() override;
        bool onUnitTest(int frame) override;

        // --------------------------------------------------------------------
        // 🧩 Virtual State Accessors (From IButtonObject)
        // --------------------------------------------------------------------
        ButtonState getState() const override;
        void setState(ButtonState state) override;

        // --------------------------------------------------------------------
        // 🧩 Public Accessors
        // --------------------------------------------------------------------
        DisplayHandle getLabelObject() const;
        std::string getText() const;
        DisplayHandle getIconButtonObject() const;
        IconButton* getIconButton() const;
        SpriteSheet* getIconSpriteSheet() const;
        Label* getLabel() const;
        std::string getFontResource() const;
        std::string getIconResource() const;
        int getFontSize() const;
        int getFontWidth() const;
        int getFontHeight() const;
        bool getUseBorder() const;
        SDL_Color getLabelColor() const;
        SDL_Color getBorderColor() const;
        int getIconWidth() const;
        int getIconHeight() const;
        IconIndex getIconIndex() const;

        // --------------------------------------------------------------------
        // 🧩 Public Mutators
        // --------------------------------------------------------------------
        void setText(const std::string& newText);

    protected:
        // --------------------------------------------------------------------
        // ⚙️ Internal Virtuals
        // --------------------------------------------------------------------
        void onStateChanged(ButtonState oldState, ButtonState newState) override;
        IconIndex iconIndexForState(ButtonState state) const override;

        // --------------------------------------------------------------------
        // 🧱 Data Members
        // --------------------------------------------------------------------
        DisplayHandle labelObject_;
        std::string text_;
        std::string font_resource_ = "internal_font_8x8";
        std::string icon_resource_ = "internal_icon_8x8";
        int font_size_ = 8;
        int font_width_ = 8;
        int font_height_ = 8;
        bool use_border_ = false;
        SDL_Color label_color_ = {255, 255, 255, 255};
        SDL_Color border_color_ = {0, 0, 0, 128};
        DisplayHandle iconButtonObject_;
        IconIndex icon_index_ = IconIndex::Checkbox_Empty;
        int icon_width_ = 8;
        int icon_height_ = 8;

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         
        
    };

} // namespace SDOM
