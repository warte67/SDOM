// SDOM_IButtonObject.hpp
#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <SDOM/SDOM_IconIndex.hpp>

namespace SDOM
{
    // Logical toggle state (canonical single source for checkbox/radio/tristate)
    enum class ButtonState : int
    {
        Inactive = 0,    // aliases: Released / Unchecked / Clear / Up / Off
        Active,          // aliases: Pressed / Checked / Set / Down / On
        Mixed,           // aliases: Indeterminate / Maybe / Option
        Disabled,        // aliases: Faded / Grayed
        // Inactive Aliases:
        Released = Inactive,
        Unchecked = Inactive,
        Clear = Inactive,
        Up = Inactive,
        Off = Inactive,
        // Active Aliases:
        Pressed = Active, 
        Checked = Active, 
        Set = Active, 
        Down = Active, 
        On = Active,
        // Mixed Aliases:
        Indeterminate = Mixed,
        Maybe = Mixed,
        Option = Mixed,
        // Disabled Aliases:
        Faded = Disabled,
        Gray = Disabled,
        Grayed = Disabled,
        Grey = Disabled,
        Greyed = Disabled,
        // Last Entry
        ButtonState_MAX
    }; // END: enum class ButtonState

    inline static const std::vector<std::pair<int, std::string>> button_state_pairs = {
        { static_cast<int>(ButtonState::Inactive),      "inactive" },
        { static_cast<int>(ButtonState::Active),        "active" },
        { static_cast<int>(ButtonState::Mixed),         "mixed" },
        { static_cast<int>(ButtonState::Disabled),      "disabled" },
        // Inactive Aliases:
        { static_cast<int>(ButtonState::Released),      "released" },
        { static_cast<int>(ButtonState::Unchecked),     "unchecked" },
        { static_cast<int>(ButtonState::Clear),         "clear" },
        { static_cast<int>(ButtonState::Up),            "up" },
        { static_cast<int>(ButtonState::Off),           "off" },
        // Active Aliases:
        { static_cast<int>(ButtonState::Pressed),       "pressed" },
        { static_cast<int>(ButtonState::Checked),       "checked" },
        { static_cast<int>(ButtonState::Set),           "set" },
        { static_cast<int>(ButtonState::Down),          "down" },
        { static_cast<int>(ButtonState::On),            "on" },
        // Mixed Aliases:
        { static_cast<int>(ButtonState::Indeterminate), "indeterminate" },
        { static_cast<int>(ButtonState::Maybe),         "maybe" },
        { static_cast<int>(ButtonState::Option),        "option" },
        // Disabled Aliases:
        { static_cast<int>(ButtonState::Faded),         "faded" },
        { static_cast<int>(ButtonState::Gray),          "gray" },
        { static_cast<int>(ButtonState::Grayed),        "grayed" },
        { static_cast<int>(ButtonState::Grey),          "grey" },
        { static_cast<int>(ButtonState::Greyed),        "greyed" },
        // Last Entry
        { static_cast<int>(ButtonState::ButtonState_MAX), "button_state_max" }
    };

    // canonical int -> name (first occurrence wins)
    inline static const std::unordered_map<int, std::string> button_state_to_string = []{
        std::unordered_map<int,std::string> m;
        for (auto &p : button_state_pairs) {
            if (m.find(p.first) == m.end()) m.emplace(p.first, p.second);
        }
        return m;
    }();

    // reverse mapping including all aliases (lowercased keys)
    #include <algorithm>
    #include <cctype>
    inline static const std::unordered_map<std::string, int> string_to_button_state = []{
        std::unordered_map<std::string,int> m;
        for (auto &p : button_state_pairs) {
            std::string key = p.second;
            std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
                return static_cast<char>(std::tolower(c));
            });
            m.emplace(std::move(key), p.first);
        }
        return m;
    }();
    
    // Helper functions (ensure they use lowered names)
    inline static std::optional<std::string> button_state_name_from_index(ButtonState idx)
    {
        auto it = button_state_to_string.find(static_cast<int>(idx));
        if (it == button_state_to_string.end()) return std::nullopt;
        std::string name = it->second;
        std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){
            return static_cast<char>(std::tolower(c));
        });
        return name;
    }

    inline static std::optional<ButtonState> button_state_from_name(const std::string &name)
    {
        std::string key = name;
        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
            return static_cast<char>(std::tolower(c));
        });
        auto it = string_to_button_state.find(key);
        if (it == string_to_button_state.end()) return std::nullopt;
        return static_cast<ButtonState>(it->second);
    }



    class IButtonObject 
    {
    public:
        // --- Construction / Destruction --- //
        IButtonObject() : buttonState_(ButtonState::Inactive) {}
        virtual ~IButtonObject() = default;

        // --- State Accessors --- //
        virtual ButtonState getState() const { return buttonState_; }
        virtual void setState(ButtonState state) { onStateChanged(buttonState_, state); buttonState_ = state; }

        // --- Additional State Info --- //
        bool isMouseHovered() const { return mouse_hovered_; }
        void setMouseHovered(bool v) { mouse_hovered_ = v; }
        bool isKeyFocused() const { return key_focused_; }
        void setKeyFocused(bool v) { key_focused_ = v; }

    protected:
        // --- Data Members --- //
        ButtonState buttonState_ = ButtonState::Inactive;
        bool mouse_hovered_ = false;
        bool key_focused_ = false;

        // --- Protected Virtual Hooks --- //
        virtual void onStateChanged(ButtonState oldState, ButtonState newState) {}
        virtual IconIndex iconIndexForState(ButtonState state) const { return IconIndex(); }

    public:
        // --- Lua Registration --- //
        static void registerLuaBindings(sol::state_view lua) 
        {
            // Idempotent / reentrant registration per Lua state.
            // Use a private sentinel in the Lua globals to avoid duplicate setup.
            if (lua["_SDOM_IButtonObject_registered"].valid() && lua["_SDOM_IButtonObject_registered"].get_or(false))
                return;

            // // Augment the single shared DisplayHandle handle usertype
            // sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);


        } // END: registerLuaBindings

    }; // END: class IButtonObject

} // END: namespace SDOM