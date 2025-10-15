// SDOM_IButtonObject.hpp
#pragma once

namespace SDOM
{
    enum class ButtonState 
    {
        // Index Groups
        Normal,             // Default, enabled, not hovered or pressed
        Hovered,            // Mouse is over, not pressed
        Pressed,            // Mouse is over, mouse button down
        Disabled,           // Not interactive
        Checked,            // For toggle/check/radio buttons (checked state)
        Unchecked,          // For toggle/check/radio buttons (unchecked state)
        Indeterminate,      // For tri-state buttons
        HoveredChecked,     // Mouse is over, checked (for toggle buttons)
        PressedChecked      // Mouse is over, pressed, checked
    };

    class IButtonObject 
    {
        public:
            IButtonObject() : state_(ButtonState::Normal) {}
            virtual ~IButtonObject() = default;
            virtual ButtonState getState() const { return state_; }
            virtual void setState(ButtonState state) { state_ = state; }

        protected:
            ButtonState state_ = ButtonState::Normal;
    }; // END: class IButtonObject
} // END: namespace SDOM