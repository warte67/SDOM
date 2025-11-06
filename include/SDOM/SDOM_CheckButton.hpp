/**
 * @class SDOM::CheckButton
 * @brief A tri-state checkbox-style button supporting checked, unchecked, and indeterminate states.
 * @author Jay Faries (https://github.com/warte67)
 * @inherits SDOM::TristateButton
 * @luaType CheckButton
 *
 * The **CheckButton** is a UI control derived from `TristateButton` that visually
 * represents and toggles between multiple logical states — typically used for
 * boolean or mixed-value options in settings panels and dialog boxes.
 *
 * It provides built-in icon support for each state and can display a text label
 * beside the checkbox graphic. Behavior and appearance are inherited from
 * `TristateButton`, including event handling and hover/press transitions.
 *
 * ---
 * @section CheckButton_Properties Properties
 * | Property | Type | Description |
 * |-----------|------|-------------|
 * | `state` | string (`"unchecked"`, `"checked"`, `"indeterminate"`) | Current state of the check button. |
 * | `text` | string | The label text displayed beside the checkbox. |
 * | `label_color` | `ColorRGBA` | The text color of the label. |
 * | `font_resource` | string | The font resource name (read-only). |
 * | `icon_resource` | string | The sprite sheet used for checkbox icons. |
 * | `tab_enabled` | bool | Whether the button participates in tab navigation. |
 * | `is_clickable` | bool | Whether the button responds to click events. |
 *
 * ---
 * @section CheckButton_Functions Functions
 * | Function | Returns | Description |
 * |-----------|----------|-------------|
 * | `setState(state)` | void | Sets the button’s logical state (`"checked"`, `"unchecked"`, `"indeterminate"`). |
 * | `getState()` | string | Returns the current logical state as a string. |
 * | `setText(newText)` | void | Updates the button label text. |
 * | `getText()` | string | Returns the current label text. |
 * | `getLabelColor()` | table | Returns a color table `{r,g,b,a}` representing the label’s tint. |
 * | `setLabelColor(color)` | void | Sets the label color using a color table or `"r,g,b,a"` string. |
 * | `getFontResource()` | string | Returns the read-only name of the associated font resource. |
 *
 * ---
 * @section CheckButton_Events Events
 * | Event | Description |
 * |--------|-------------|
 * | `MouseClick` | Fired when the checkbox is toggled by user input. |
 * | `StateChanged` | Fired when the state changes (checked/unchecked/indeterminate). |
 * | `FocusGained` | Fired when the checkbox gains keyboard focus. |
 * | `FocusLost` | Fired when the checkbox loses keyboard focus. |
 *
 * ---
 * @section CheckButton_Notes Notes
 * - The checkbox icon is drawn from `internal_icon_8x8` by default.
 * - Text labels use the `internal_font_8x8` resource unless overridden.
 * - Derived from `TristateButton`, inheriting its focus, hover, and click behavior.
 * - Lua bindings automatically convert state strings to internal enums.
 *
 * ---
 * @section License ZLIB License
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include <SDOM/SDOM_TristateButton.hpp>

namespace SDOM
{
    /**
     * @brief A tri-state checkbox-style UI element for user toggles.
     */
    class CheckButton : public TristateButton
    {
        using SUPER = TristateButton;

    public:
        // -----------------------------------------------------------------
        // 🪣 Type Information
        // -----------------------------------------------------------------
        static constexpr const char* TypeName = "CheckButton";

        // -----------------------------------------------------------------
        // 🏗️ Initialization Structure
        // -----------------------------------------------------------------
        struct InitStruct : TristateButton::InitStruct
        {
            InitStruct() : TristateButton::InitStruct()
            {
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};
                tabEnabled = false;
                isClickable = true;
                text = TypeName;
                font_size = 8;
                label_color = {0, 255, 255, 255};
                border_color = {0, 0, 0, 128};
                state = ButtonState::Unchecked;
                icon_resource = "internal_icon_8x8";
                icon_width = 8;
                icon_height = 8;
                font_resource = "internal_font_8x8";
                font_width = 8;
                font_height = 8;
                border = false;
            }
        };

    protected:
        // -----------------------------------------------------------------
        // 🌱 Constructors
        // -----------------------------------------------------------------
        CheckButton(const InitStruct& init);
        CheckButton(const sol::table& config);

    public:
        // -----------------------------------------------------------------
        // 🏭 Factory Methods
        // -----------------------------------------------------------------
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config)
        {
            return std::unique_ptr<IDisplayObject>(new CheckButton(config));
        }

        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const CheckButton::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new CheckButton(init));
        }

        CheckButton() = default;
        virtual ~CheckButton() override = default;

        // -----------------------------------------------------------------
        // 🌱 Lifecycle
        // -----------------------------------------------------------------
        virtual bool onInit() override;
        virtual void onQuit() override;
        virtual void onUpdate(float fElapsedTime) override;
        virtual void onEvent(const Event& event) override;
        virtual void onRender() override;
        virtual bool onUnitTest(int frame) override;

        // -----------------------------------------------------------------
        // 🧩 State Accessors
        // -----------------------------------------------------------------
        virtual ButtonState getState() const override;
        virtual void setState(ButtonState state) override;

    protected:
        // -----------------------------------------------------------------
        // ⚙️ Internal State Logic
        // -----------------------------------------------------------------
        virtual void onStateChanged(ButtonState oldState, ButtonState newState) override;
        virtual IconIndex iconIndexForState(ButtonState state) const override;

        // -----------------------------------------------------------------
        // 📜 Lua Integration
        // -----------------------------------------------------------------
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua) override;
    };

} // namespace SDOM
