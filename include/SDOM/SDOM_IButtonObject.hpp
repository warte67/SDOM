/**
 * @class SDOM::IButtonObject
 * @brief Base interface for all button-like objects within the SDOM hierarchy.
 * @author Jay Faries (https://github.com/warte67)
 * @inherits SDOM::IDataObject
 * @luaType IButtonObject
 *
 * The `IButtonObject` class defines the canonical state model and behavior for
 * all button-derived objects such as `IconButton`, `ArrowButton`, and
 * `CheckButton`. It provides a unified way to query and mutate a button’s
 * logical state (Active, Inactive, Mixed, Disabled), track focus and hover
 * conditions, and react to transitions via virtual hooks.
 *
 * Derived classes use these state hooks to trigger visual changes, animations,
 * or sound effects when the logical button state changes.
 *
 * ---
 * @section IButtonObject_Properties Properties
 * | Property | Type | Description |
 * |-----------|------|-------------|
 * | `state` | `ButtonState` | Logical state of the button (`Inactive`, `Active`, `Mixed`, `Disabled`). |
 * | `isMouseHovered` | `bool` | Whether the cursor is currently over the button. |
 * | `isKeyFocused` | `bool` | Whether the button currently has keyboard focus. |
 *
 * ---
 * @section IButtonObject_Functions Functions
 * | Function | Returns | Description |
 * |-----------|----------|-------------|
 * | `getState()` | `ButtonState` | Returns the current button state. |
 * | `setState(state)` | `void` | Sets the logical button state and triggers `onStateChanged()`. |
 * | `isMouseHovered()` | `bool` | Returns whether the mouse is hovering over this button. |
 * | `setMouseHovered(value)` | `void` | Manually sets the hover state flag. |
 * | `isKeyFocused()` | `bool` | Returns whether the button has keyboard focus. |
 * | `setKeyFocused(value)` | `void` | Sets the keyboard focus flag. |
 * | `registerLuaBindings(lua)` | `static void` | Registers this class with a Lua state. |
 *
 * ---
 * @section IButtonObject_Events Events
 * | Event | Description |
 * |--------|-------------|
 * | `StateChanged` | Fired whenever the logical button state changes. |
 *
 * ---
 * @section IButtonObject_Notes Notes
 * - Derived classes (e.g., `ArrowButton`, `IconButton`) typically override
 *   `onStateChanged()` and `iconIndexForState()` to update visuals or icons.
 * - The `ButtonState` enum includes semantic aliases such as
 *   “Pressed” → “Active” and “Released” → “Inactive”.
 * - Lua bindings provide both integer and string-based access for state values.
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
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <cctype>
#include <vector>
#include <SDOM/SDOM_IconIndex.hpp>

namespace SDOM
{

    // ------------------------------------------------------------------------
    // 🧩 ButtonState
    // ------------------------------------------------------------------------
    /**
     * @enum ButtonState
     * @brief Canonical logical state of any button-like object.
     *
     * Provides a consistent abstraction over various button and toggle states.
     * Multiple aliases are defined to unify naming across different UI
     * paradigms (e.g., checkbox, radio, or momentary button).
     *
     * | Primary State | Common Aliases | Semantic Meaning |
     * |----------------|----------------|------------------|
     * | Inactive | Released, Unchecked, Up, Off | Button is released or deselected |
     * | Active | Pressed, Checked, Down, On | Button is pressed or selected |
     * | Mixed | Indeterminate, Maybe | Tri-state or ambiguous condition |
     * | Disabled | Faded, Gray, Grayed | Non-interactive or visually muted |
     */
    enum class ButtonState : int
    {
        Inactive = 0,    ///< Default inactive (released/unchecked)
        Active,          ///< Active (pressed/checked)
        Mixed,           ///< Indeterminate/tri-state
        Disabled,        ///< Disabled/non-interactive

        // Aliases (semantic cross-mapping)
        Released = Inactive,
        Unchecked = Inactive,
        Clear = Inactive,
        Up = Inactive,
        Off = Inactive,

        Pressed = Active,
        Checked = Active,
        Set = Active,
        Down = Active,
        On = Active,

        Indeterminate = Mixed,
        Maybe = Mixed,
        Option = Mixed,

        Faded = Disabled,
        Gray = Disabled,
        Grayed = Disabled,
        Grey = Disabled,
        Greyed = Disabled,

        ButtonState_MAX ///< Sentinel value for iteration/debug
    };

    // ------------------------------------------------------------------------
    // 🔄 ButtonState ↔ String Conversion Tables
    // ------------------------------------------------------------------------
    /** @brief Ordered vector of integer-state ↔ name pairs (includes aliases). */
    inline static const std::vector<std::pair<int, std::string>> button_state_pairs = {
        { static_cast<int>(ButtonState::Inactive),      "inactive" },
        { static_cast<int>(ButtonState::Active),        "active" },
        { static_cast<int>(ButtonState::Mixed),         "mixed" },
        { static_cast<int>(ButtonState::Disabled),      "disabled" },

        // Inactive Aliases
        { static_cast<int>(ButtonState::Released),      "released" },
        { static_cast<int>(ButtonState::Unchecked),     "unchecked" },
        { static_cast<int>(ButtonState::Clear),         "clear" },
        { static_cast<int>(ButtonState::Up),            "up" },
        { static_cast<int>(ButtonState::Off),           "off" },

        // Active Aliases
        { static_cast<int>(ButtonState::Pressed),       "pressed" },
        { static_cast<int>(ButtonState::Checked),       "checked" },
        { static_cast<int>(ButtonState::Set),           "set" },
        { static_cast<int>(ButtonState::Down),          "down" },
        { static_cast<int>(ButtonState::On),            "on" },

        // Mixed Aliases
        { static_cast<int>(ButtonState::Indeterminate), "indeterminate" },
        { static_cast<int>(ButtonState::Maybe),         "maybe" },
        { static_cast<int>(ButtonState::Option),        "option" },

        // Disabled Aliases
        { static_cast<int>(ButtonState::Faded),         "faded" },
        { static_cast<int>(ButtonState::Gray),          "gray" },
        { static_cast<int>(ButtonState::Grayed),        "grayed" },
        { static_cast<int>(ButtonState::Grey),          "grey" },
        { static_cast<int>(ButtonState::Greyed),        "greyed" },

        { static_cast<int>(ButtonState::ButtonState_MAX), "button_state_max" }
    };

    /**
     * @brief Maps canonical `ButtonState` integer values to lowercase string names.
     * @details The first occurrence of a numeric key wins (aliases are ignored).
     */
    inline static const std::unordered_map<int, std::string> button_state_to_string = []{
        std::unordered_map<int,std::string> m;
        for (auto &p : button_state_pairs)
            if (m.find(p.first) == m.end()) m.emplace(p.first, p.second);
        return m;
    }();

    /**
     * @brief Maps lowercase string names (including aliases) to canonical integer values.
     */
    inline static const std::unordered_map<std::string, int> string_to_button_state = []{
        std::unordered_map<std::string,int> m;
        for (auto &p : button_state_pairs) {
            std::string key = p.second;
            std::transform(key.begin(), key.end(), key.begin(),
                [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
            m.emplace(std::move(key), p.first);
        }
        return m;
    }();

    // ------------------------------------------------------------------------
    // 🔧 Helper Functions
    // ------------------------------------------------------------------------
    /**
     * @brief Retrieves the lowercase string name for a given ButtonState.
     * @param idx ButtonState enum value.
     * @return std::optional<std::string> containing the name, or std::nullopt if not found.
     */
    inline static std::optional<std::string> button_state_name_from_index(ButtonState idx)
    {
        auto it = button_state_to_string.find(static_cast<int>(idx));
        if (it == button_state_to_string.end()) return std::nullopt;
        std::string name = it->second;
        std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
        return name;
    }

    /**
     * @brief Converts a string (case-insensitive) into a ButtonState enum.
     * @param name Lowercase or mixed-case state name or alias.
     * @return std::optional<ButtonState> if conversion succeeds, std::nullopt otherwise.
     */
    inline static std::optional<ButtonState> button_state_from_name(const std::string &name)
    {
        std::string key = name;
        std::transform(key.begin(), key.end(), key.begin(),
            [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
        auto it = string_to_button_state.find(key);
        if (it == string_to_button_state.end()) return std::nullopt;
        return static_cast<ButtonState>(it->second);
    }

    // ------------------------------------------------------------------------
    // 🧱 IButtonObject
    // ------------------------------------------------------------------------
    /**
     * @class IButtonObject
     * @brief Base interface for all SDOM button-like objects.
     *
     * Provides foundational button state management and event hooks for derived
     * classes. Handles logical state transitions (active/inactive/mixed), focus
     * flags (mouse/key), and defines a standard Lua binding pattern.
     *
     * Derived classes include:
     *  - `IconButton`
     *  - `ArrowButton`
     *  - `CheckButton`
     *  - `ToggleButton`
     */
    class IButtonObject
    {
    public:
        // --------------------------------------------------------------------
        // ⚙️ Construction / Destruction
        // --------------------------------------------------------------------
        /** @brief Default constructor (initializes state as Inactive). */
        IButtonObject() : buttonState_(ButtonState::Inactive) {}

        /** @brief Virtual destructor (defaulted). */
        virtual ~IButtonObject() = default;

        // --------------------------------------------------------------------
        // 🔄 State Accessors
        // --------------------------------------------------------------------
        /**
         * @brief Retrieves the current logical button state.
         * @return Current `ButtonState` of the button.
         */
        virtual ButtonState getState() const { return buttonState_; }

        /**
         * @brief Sets a new logical state for the button.
         * @param state New `ButtonState` value.
         * @details Invokes `onStateChanged()` prior to updating `buttonState_`.
         */
        virtual void setState(ButtonState state)
        {
            onStateChanged(buttonState_, state);
            buttonState_ = state;
        }

        // --------------------------------------------------------------------
        // 🖱️ Focus & Hover Flags
        // --------------------------------------------------------------------
        /** @brief Returns true if the mouse is currently hovering over this button. */
        bool isMouseHovered() const { return mouse_hovered_; }

        /** @brief Sets whether the mouse is currently hovering over this button. */
        void setMouseHovered(bool v) { mouse_hovered_ = v; }

        /** @brief Returns true if this button currently has keyboard focus. */
        bool isKeyFocused() const { return key_focused_; }

        /** @brief Sets whether this button currently has keyboard focus. */
        void setKeyFocused(bool v) { key_focused_ = v; }

    protected:
        // --------------------------------------------------------------------
        // 🧩 Data Members
        // --------------------------------------------------------------------
        ButtonState buttonState_ = ButtonState::Inactive; ///< Current button state.
        bool mouse_hovered_ = false; ///< Whether the button is under mouse hover.
        bool key_focused_ = false;   ///< Whether the button is keyboard focused.

        // --------------------------------------------------------------------
        // 🪄 Virtual Hooks (for Derived Classes)
        // --------------------------------------------------------------------
        /**
         * @brief Called whenever the button state changes.
         * @param oldState Previous state.
         * @param newState New state.
         * @note Derived classes should override this to trigger animations,
         *       sound effects, or visual updates on state transitions.
         */
        virtual void onStateChanged(ButtonState oldState, ButtonState newState) {}

        /**
         * @brief Retrieves the icon index associated with a specific state.
         * @param state The `ButtonState` for which to fetch the icon index.
         * @return An `IconIndex` value representing the visual asset.
         */
        virtual IconIndex iconIndexForState(ButtonState state) const { return IconIndex(); }

    public:
        // --------------------------------------------------------------------
        // 📜 Lua Registration
        // --------------------------------------------------------------------
        /**
         * @brief Registers the IButtonObject bindings with a Lua state.
         * @param lua Reference to a `sol::state_view` instance.
         * @details Ensures idempotent setup (safe to call multiple times per Lua state).
         */
        static void registerLuaBindings(sol::state_view lua)
        {
            // Idempotent / reentrant registration per Lua state.
            if (lua["_SDOM_IButtonObject_registered"].valid() && lua["_SDOM_IButtonObject_registered"].get_or(false))
                return;

            // // Acquire or create the DisplayHandle table (do not clobber usertype).
            // sol::table handleTbl;
            // try { handleTbl = lua[SDOM::DisplayHandle::LuaHandleName]; } catch(...) {}
            // if (!handleTbl.valid()) {
            //     handleTbl = SDOM::IDataObject::ensure_sol_table(lua, SDOM::DisplayHandle::LuaHandleName);
            // }                

            // Future: bind properties and enums here once DisplayHandle surface is ready.
        }
    };

} // namespace SDOM





