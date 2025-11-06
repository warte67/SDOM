/**
 * @class SDOM::ArrowButton
 * @brief A directional button displaying an arrow icon that can point up, down, left, or right.
 * @author Jay Faries (https://github.com/warte67)
 * @inherits SDOM::IconButton
 * @luaType ArrowButton
 *
 * The **ArrowButton** is a specialized clickable UI element derived from `IconButton`.
 * It provides directional control via an internal icon atlas, making it ideal for scrollbars,
 * navigation widgets, and stepper controls. The arrow’s direction and pressed state determine
 * which icon is displayed.
 *
 * Inherits hover, click, and focus handling from `IconButton`.
 *
 * ---
 * @section ArrowButton_Properties Properties
 * | Property | Type | Description |
 * |-----------|------|-------------|
 * | `direction` | string (`"up"`, `"down"`, `"left"`, `"right"`) | Sets or gets the arrow’s direction. |
 * | `color` | `ColorRGBA` | Tint color for the icon sprite. |
 * | `tab_enabled` | `bool` | Whether this button participates in tab navigation. |
 * | `is_clickable` | `bool` | Whether the button responds to click events. |
 *
 * ---
 * @section ArrowButton_Functions Functions
 * | Function | Returns | Description |
 * |-----------|----------|-------------|
 * | `setDirection(dir)` | void | Sets the arrow direction. |
 * | `getDirection()` | string | Returns the current arrow direction. |
 * | `getIconIndex()` | int | Returns the icon index based on direction and state. |
 *
 * ---
 * @section ArrowButton_Events Events
 * | Event | Description |
 * |--------|-------------|
 * | `MouseClick` | Fired when the arrow button is clicked. |
 * | `MouseEnter` | Fired when the cursor enters the button area. |
 * | `MouseLeave` | Fired when the cursor leaves the button area. |
 *
 * ---
 * @section ArrowButton_Notes Notes
 * - The internal icon lookup table maps four directions × two states (raised/depressed).
 * - Lua bindings automatically convert `"up"`, `"down"`, `"left"`, and `"right"` to enum values.
 * - Colors and dimensions are inherited from `IconButton::InitStruct`.
 */

/**
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

#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_IconIndex.hpp>

namespace SDOM
{
    /**
     * @brief A directional button with up/down/left/right arrow icons.
     */
    class ArrowButton : public IconButton
    {
        using SUPER = SDOM::IconButton;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "ArrowButton";

        // -----------------------------------------------------------------
        // 🧭 Enumerations
        // -----------------------------------------------------------------

        /**
         * @enum SDOM::ArrowButton::ArrowDirection
         * @brief Enumerates the four possible arrow directions.
         */
        enum class ArrowDirection : int { Left, Right, Up, Down };

        /**
         * @enum SDOM::ArrowButton::ArrowState
         * @brief Represents the visual pressed state of the arrow button.
         */
        enum class ArrowState : int { Raised, Depressed };

        // -----------------------------------------------------------------
        // 🗺️ Direction String Maps
        // -----------------------------------------------------------------

        /** @brief Converts enum direction to string. */
        inline static const std::map<ArrowDirection, std::string> arrow_direction_to_string = {
            { ArrowDirection::Left, "left" },
            { ArrowDirection::Right, "right" },
            { ArrowDirection::Up, "up" },
            { ArrowDirection::Down, "down" }
        };

        /** @brief Converts string names to enum direction. */
        inline static const std::map<std::string, ArrowDirection> string_to_arrow_direction = {
            { "left", ArrowDirection::Left },
            { "right", ArrowDirection::Right },
            { "up", ArrowDirection::Up },
            { "down", ArrowDirection::Down }
        };

        // -----------------------------------------------------------------
        // 🧮 Icon Lookup Table
        // -----------------------------------------------------------------
        /**
         * @brief Lookup table for arrow icons by direction and state.
         * Accessed as `ArrowIconTable[direction][state]`.
         */
        static constexpr IconIndex ArrowIconTable[4][2] = {
            { IconIndex::Arrow_Left_Raised,   IconIndex::Arrow_Left_Depressed   },
            { IconIndex::Arrow_Right_Raised,  IconIndex::Arrow_Right_Depressed  },
            { IconIndex::Arrow_Up_Raised,     IconIndex::Arrow_Up_Depressed     },
            { IconIndex::Arrow_Down_Raised,   IconIndex::Arrow_Down_Depressed   }
        };

        // -----------------------------------------------------------------
        // 🧱 Initialization Structure
        // -----------------------------------------------------------------

        /**
         * @struct SDOM::ArrowButton::InitStruct
         * @brief Initialization parameters for constructing an ArrowButton.
         *
         * Extends `IconButton::InitStruct` with a direction property.
         * Used by the Factory or Lua table construction.
         */
        struct InitStruct : IconButton::InitStruct
        {
            InitStruct() : IconButton::InitStruct()
            {
                x = 0;
                y = 0;
                width = 8;
                height = 8;
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};
                tabEnabled = false;
                isClickable = true;
                icon_resource = "internal_icon_8x8";
                icon_width = 8;
                icon_height = 8;
            }

            /** @brief The direction of the arrow (defaults to Up). */
            ArrowDirection direction = ArrowDirection::Up;
        };

    protected:
        // --- Protected Constructors --- //
        ArrowButton(const InitStruct& init);
        ArrowButton(const sol::table& config);

    public:
        // -----------------------------------------------------------------
        // 🏭 Static Factory Methods
        // -----------------------------------------------------------------

        /**
         * @brief Creates an ArrowButton instance from a Lua table.
         * @param config Lua configuration table.
         * @return Unique pointer to a new ArrowButton instance.
         */
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config)
        {
            return std::unique_ptr<IDisplayObject>(new ArrowButton(config));
        }

        /**
         * @brief Creates an ArrowButton instance from an InitStruct.
         * @param baseInit Base initialization data.
         * @return Unique pointer to a new ArrowButton instance.
         */
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& arrowInit = static_cast<const ArrowButton::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new ArrowButton(arrowInit));
        }

        // -----------------------------------------------------------------
        // 🌱 Lifecycle Methods
        // -----------------------------------------------------------------

        ArrowButton() = default;
        virtual ~ArrowButton() = default;

        /** @brief Called when the display object is initialized. */
        virtual bool onInit() override;

        /** @brief Called before the object is destroyed. */
        virtual void onQuit() override;

        /** @brief Handles an incoming event such as click or hover. */
        virtual void onEvent(const Event& event) override;

        /** @brief Updates the object each frame. */
        virtual void onUpdate(float fElapsedTime) override;

        /** @brief Renders the arrow button to the target surface. */
        virtual void onRender() override;

        /** @brief Optional unit test hook for automated testing. */
        virtual bool onUnitTest(int frame) override;

        // -----------------------------------------------------------------
        // 🧩 Accessors
        // -----------------------------------------------------------------

        /** @brief Returns the current arrow direction. */
        ArrowDirection getDirection() const { return direction_; }

        /** @brief Sets the arrow direction and marks object as dirty. */
        void setDirection(ArrowDirection dir) { direction_ = dir; setDirty(true); }

        /** @brief Returns the current arrow state (raised or depressed). */
        ArrowState getArrowState() const { return arrowState_; }

        /** @brief Returns the icon index corresponding to the direction/state. */
        IconIndex getIconIndex() const { return ArrowIconTable[static_cast<int>(direction_)][static_cast<int>(arrowState_)]; }

    private:
        ArrowState arrowState_ = ArrowState::Raised; ///< Internal state, not exposed to Lua.

    protected:
        ArrowDirection direction_ = ArrowDirection::Up; ///< Current arrow direction.

        // -----------------------------------------------------------------
        // 📜 Lua Integration
        // -----------------------------------------------------------------
        /** @brief Registers ArrowButton with Lua runtime. */
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);
    };

} // namespace SDOM
