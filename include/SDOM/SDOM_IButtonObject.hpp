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

    // Mapping tables for ButtonState <-> string
    inline static const std::unordered_map<int, std::string> button_state_to_string = {
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
    }; // END: button_state_to_string

    // reverse mapping
    inline static const std::unordered_map<std::string, int> string_to_button_state = []{
        std::unordered_map<std::string,int> m;
        for (auto &p : button_state_to_string) m.emplace(p.second, p.first);
        return m;
    }(); // END: string_to_button_state

    // Helper functions for ButtonState <-> string conversions
    inline static std::optional<std::string> button_state_name_from_index(ButtonState idx)
    {
        auto it = button_state_to_string.find(static_cast<int>(idx));
        if (it == button_state_to_string.end()) return std::nullopt;
        return it->second;
    } // END: button_state_name_from_index

    // returns std::nullopt if name not found
    inline static std::optional<ButtonState> button_state_from_name(const std::string &name)
    {
        auto it = string_to_button_state.find(name);
        if (it == string_to_button_state.end()) return std::nullopt;
        return static_cast<ButtonState>(it->second);
    } // END: button_state_from_name


    // --- Interface for Button-like objects (Checkbox, RadioButton, Toggle, etc) --- //

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

        // --- Lua Registration --- //
        static void registerLuaBindings(sol::state_view lua) 
        {
            // Idempotent / reentrant registration per Lua state.
            // Use a private sentinel in the Lua globals to avoid duplicate setup.
            if (lua["_SDOM_IButtonObject_registered"].valid() && lua["_SDOM_IButtonObject_registered"].get_or(false))
                return;

            // Augment the single shared DisplayHandle handle usertype
            sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);

            // Create / populate a global ButtonState table (canonical names + aliases)
            sol::table bs = lua["ButtonState"].valid() ? lua["ButtonState"] : lua.create_table();

            auto set_if_absent = [&](sol::table &t, const char* key, int val){
                if (!t[key].valid()) t[key] = val;
            };

            set_if_absent(bs, "inactive", static_cast<int>(ButtonState::Inactive));
            set_if_absent(bs, "active", static_cast<int>(ButtonState::Active));
            set_if_absent(bs, "mixed", static_cast<int>(ButtonState::Mixed));
            set_if_absent(bs, "disabled", static_cast<int>(ButtonState::Disabled));

            // aliases (convenience)
            set_if_absent(bs, "released", static_cast<int>(ButtonState::Released));
            set_if_absent(bs, "unchecked", static_cast<int>(ButtonState::Unchecked));
            set_if_absent(bs, "clear", static_cast<int>(ButtonState::Clear));
            set_if_absent(bs, "up", static_cast<int>(ButtonState::Up));
            set_if_absent(bs, "off", static_cast<int>(ButtonState::Off));

            set_if_absent(bs, "pressed", static_cast<int>(ButtonState::Pressed));
            set_if_absent(bs, "checked", static_cast<int>(ButtonState::Checked));
            set_if_absent(bs, "set", static_cast<int>(ButtonState::Set));
            set_if_absent(bs, "down", static_cast<int>(ButtonState::Down));
            set_if_absent(bs, "on", static_cast<int>(ButtonState::On));

            set_if_absent(bs, "indeterminate", static_cast<int>(ButtonState::Indeterminate));
            set_if_absent(bs, "maybe", static_cast<int>(ButtonState::Maybe));
            set_if_absent(bs, "option", static_cast<int>(ButtonState::Option));

            set_if_absent(bs, "faded", static_cast<int>(ButtonState::Faded));
            set_if_absent(bs, "gray", static_cast<int>(ButtonState::Gray));
            set_if_absent(bs, "grayed", static_cast<int>(ButtonState::Grayed));
            set_if_absent(bs, "grey", static_cast<int>(ButtonState::Grey));
            set_if_absent(bs, "greyed", static_cast<int>(ButtonState::Greyed));

            lua["ButtonState"] = bs;

            // Convenience conversion helpers exposed to Lua (register only if absent)
            if (!lua["button_state_from_name"].valid())
            {
                lua.set_function("button_state_from_name", [](const std::string &name) -> sol::optional<int> {
                    auto opt = button_state_from_name(name);
                    if (!opt) return sol::nullopt;
                    return static_cast<int>(*opt);
                });
            }

            if (!lua["button_state_name_from_index"].valid())
            {
                lua.set_function("button_state_name_from_index", [](int idx) -> sol::optional<std::string> {
                    auto opt = button_state_name_from_index(static_cast<ButtonState>(idx));
                    if (!opt) return sol::nullopt;
                    return *opt;
                });
            }

            // DisplayHandle augmentation: attach handle-level helpers only if absent.
            auto set_handle_if_absent = [&](const char* name, sol::object value) {
                if (!handle[name].valid())
                    handle[name] = value;
            };

            set_handle_if_absent("getState", sol::make_object(lua, [](DisplayHandle h) -> sol::optional<int> {
                if (!h.isValid()) return sol::nullopt;
                IButtonObject* btn = h.as<IButtonObject>();
                if (!btn) return sol::nullopt;
                return static_cast<int>(btn->getState());
            }));

            set_handle_if_absent("setState", sol::make_object(lua, [](DisplayHandle h, int state) {
                if (!h.isValid()) return;
                IButtonObject* btn = h.as<IButtonObject>();
                if (!btn) return;
                btn->setState(static_cast<ButtonState>(state));
            }));

            set_handle_if_absent("isMouseHovered", sol::make_object(lua, [](DisplayHandle h) -> bool {
                return h.isValid() && h.as<IButtonObject>() && h.as<IButtonObject>()->isMouseHovered();
            }));

            set_handle_if_absent("setMouseHovered", sol::make_object(lua, [](DisplayHandle h, bool v) {
                if (!h.isValid()) return;
                IButtonObject* btn = h.as<IButtonObject>();
                if (btn) btn->setMouseHovered(v);
            }));

            set_handle_if_absent("isKeyFocused", sol::make_object(lua, [](DisplayHandle h) -> bool {
                return h.isValid() && h.as<IButtonObject>() && h.as<IButtonObject>()->isKeyFocused();
            }));

            set_handle_if_absent("setKeyFocused", sol::make_object(lua, [](DisplayHandle h, bool v) {
                if (!h.isValid()) return;
                IButtonObject* btn = h.as<IButtonObject>();
                if (btn) btn->setKeyFocused(v);
            }));

            // Mark registration complete for this lua state
            lua["_SDOM_IButtonObject_registered"] = true;

            // DisplayHandle augmentation point: derived types may add more handle helpers
            (void)handle;

        } // END: registerLuaBindings

    }; // END: class IButtonObject

} // END: namespace SDOM