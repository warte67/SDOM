/**
 * @class SDOM::Button
 * @brief A standard clickable text button for UI interaction.
 * @author Jay Faries (https://github.com/warte67)
 * @inherits SDOM::IPanelObject, SDOM::IButtonObject
 * @luaType Button
 *
 * The **Button** class represents a basic interactive UI element displaying a text label.
 * It inherits layout and rendering capabilities from `IPanelObject` and click, hover,
 * and focus behavior from `IButtonObject`. The text label is represented internally
 * as a child display object that can be accessed and customized via Lua.
 *
 * ---
 * @section Button_Properties Properties
 * | Property | Type | Description |
 * |-----------|------|-------------|
 * | `text` | string | The visible text displayed on the button face. |
 * | `label_color` | `ColorRGBA` | The text color used for rendering the label. |
 * | `font_resource` | string (read-only) | The font resource name used for text rendering. |
 *
 * ---
 * @section Button_Functions Functions
 * | Function | Returns | Description |
 * |-----------|----------|-------------|
 * | `getText()` | string | Returns the button’s current label text. |
 * | `setText(newText)` | void | Updates the button’s visible label text. |
 * | `getLabelColor()` | `ColorRGBA` | Returns the current text color. |
 * | `setLabelColor(color)` | void | Updates the text color used for rendering. |
 * | `getFontResource()` | string | Returns the name of the font resource (read-only). |
 * | `getLabelObject()` | `DisplayHandle` | Returns a handle to the label display object for customization. |
 * 
 * ---
 * @section Button_Events Events
 * | Event | Description |
 * |--------|-------------|
 * | `MouseClick` | Fired when the button is clicked. |
 * | `MouseEnter` | Fired when the cursor enters the button area. |
 * | `MouseLeave` | Fired when the cursor leaves the button area. |
 * | `FocusGained` | Fired when the button gains keyboard focus. |
 * | `FocusLost` | Fired when the button loses keyboard focus. |
 *
 * ---
 * @section Button_Notes Notes
 * - The button automatically creates a label display object for text rendering.
 * - Defaults use the `internal_font_8x8` sprite-based font from the factory.
 * - Colors, dimensions, and focus behavior are inherited from `IPanelObject`.
 * - All interaction events from `IButtonObject` are available to Lua scripts.
 *
 * ---
 * @section License ZLIB License
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */


#pragma once

#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>

namespace SDOM
{
    /**
     * @brief A standard clickable text button class.
     */
    class Button : public IPanelObject, public IButtonObject
    {
        using SUPER = SDOM::IPanelObject;

    public:
        // -----------------------------------------------------------------
        // 🏷️ Type Information
        // -----------------------------------------------------------------
        static constexpr const char* TypeName = "Button";

        // -----------------------------------------------------------------
        // 🧱 Initialization Structure
        // -----------------------------------------------------------------
        /**
         * @struct SDOM::Button::InitStruct
         * @brief Initialization parameters for constructing a Button.
         *
         * Extends `IPanelObject::InitStruct` with label and font properties.
         */
        struct InitStruct : public IPanelObject::InitStruct
        {
            InitStruct()
                : IPanelObject::InitStruct()
            {
                //
                // From IDisplayObject
                //
                name        = TypeName;
                type        = TypeName;
                color       = {96, 0, 96, 255};    // purple UI button color
                isClickable = true;                // buttons always clickable
                tabEnabled  = true;                // tab focus allowed

                //
                // From IPanelObject
                //
                base_index    = PanelBaseIndex::ButtonUp;
                icon_resource = "internal_icon_8x8";
                icon_width    = 8;
                icon_height   = 8;

                font_resource = "internal_font_8x8";
                font_width    = 8;
                font_height   = 8;

                //
                // Button-specific defaults
                //
                text        = "Button";
                font_size   = 8;
                label_color = {255, 255, 255, 255}; // white text
            }

            // --- Button-specific members ---
            std::string text = "Button";                  ///< Default button caption
            int font_size = 8;                            ///< Actual font size for Text rendering
            SDL_Color label_color = {255, 255, 255, 255}; ///< Color of the label (white)

            // ---------------------------------------------------------------------
            // JSON loader (inheritance-safe pattern)
            // ---------------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                // 1) Load IDisplayObject + IPanelObject fields
                IPanelObject::InitStruct::from_json(j, out);

                // 2) Load Button-specific fields
                if (j.contains("text"))
                    out.text = j["text"].get<std::string>();

                if (j.contains("font_size"))
                    out.font_size = j["font_size"].get<int>();

                if (j.contains("label_color") && j["label_color"].is_array() && j["label_color"].size() == 4)
                {
                    auto& c = j["label_color"];
                    out.label_color = {
                        c[0].get<uint8_t>(),
                        c[1].get<uint8_t>(),
                        c[2].get<uint8_t>(),
                        c[3].get<uint8_t>()
                    };
                }
            }
        }; // END: struct InitStruct


    protected:
        // -----------------------------------------------------------------
        // 🌱 Constructors
        // -----------------------------------------------------------------
        Button(const InitStruct& init);

    public:
        // -----------------------------------------------------------------
        // 🏭 Static Factory Methods
        // -----------------------------------------------------------------
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const Button::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Button(init));
        }

        static std::unique_ptr<IDisplayObject> CreateFromJson(const nlohmann::json& j)
        {
            Button::InitStruct init;
            Button::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new Button(init));
        }



        // -----------------------------------------------------------------
        // 🌱 Lifecycle Methods
        // -----------------------------------------------------------------
        Button() = default;
        ~Button() override = default;

        /** @brief Called when the button is initialized. */
        bool onInit() override;

        /** @brief Called before the button is destroyed. */
        void onQuit() override;

        /** @brief Called every frame to update the button state. */
        void onUpdate(float fElapsedTime) override;

        /** @brief Renders the button and its label. */
        void onRender() override;

        /** @brief Handles mouse and keyboard input events. */
        void onEvent(const Event& event) override;

        /** @brief Optional unit test hook for automated validation. */
        bool onUnitTest(int frame) override;

        // -----------------------------------------------------------------
        // 🧩 Accessors
        // -----------------------------------------------------------------
        /** @brief Returns the label DisplayHandle (for advanced Lua access). */
        DisplayHandle getLabelObject() const { return labelObject_; }

        /** @brief Returns the button’s current text label. */
        std::string getText() const { return text_; }

        /** @brief Updates the button’s text label. */
        void setText(const std::string& newText);

        /** @brief Returns the current text label color. */
        SDL_Color getLabelColor() const { return label_color_; }

        /** @brief Updates the text label color. */
        void setLabelColor(const SDL_Color& color) { label_color_ = color; }

        /** @brief Returns the font resource name used by this button (read-only). */
        std::string getFontResource() const { return font_resource_name_; }        

    protected:
        DisplayHandle labelObject_;                      ///< Internal label object for text rendering
        std::string text_;                               ///< Current label text
        std::string font_resource_name_ = "internal_font_8x8";
        int font_size_ = 8;
        int font_width_ = 8;
        int font_height_ = 8;
        SDL_Color label_color_ = {255, 255, 255, 255};

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;     
    };

} // namespace SDOM
