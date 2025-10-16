// SDOM_CheckButton.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>

#include <SDOM/SDOM_CheckButton.hpp>

namespace SDOM
{
    // --- Constructors --- //
    CheckButton::CheckButton(const InitStruct& init): TristateButton(init)
    {
        // add custom properties here
    } // END: CheckButton::CheckButton(const InitStruct& init)

    CheckButton::CheckButton(const sol::table& config) : TristateButton(config)
    {
        // add custom properties here
    } // END: CheckButton::CheckButton(const sol::table& config)


    // --- Lifecycle & Core Virtuals --- //
    bool CheckButton::onInit()
    {
        return SUPER::onInit();
    } // END: CheckButton::onInit()

    void CheckButton::onQuit()
    {

    } // END: CheckButton::onQuit()

    void CheckButton::onUpdate(float fElapsedTime)
    {
        SUPER::onUpdate(fElapsedTime);
    } // END: CheckButton::onUpdate()

    void CheckButton::onEvent(const Event& event)
    {
        SUPER::onEvent(event);
    } // END: CheckButton::onEvent()

    void CheckButton::onRender()
    {
        SUPER::onRender();
    } // END: CheckButton::onRender()

    bool CheckButton::onUnitTest() 
    {
        return true;
    } // END: CheckButton::onUnitTest()

    // --- Virtual State Accessors (From IButtonObject) --- //
    ButtonState CheckButton::getState() const
    {
        return buttonState_;
    } // END: CheckButton::getState()

    void CheckButton::setState(ButtonState state)
    {
        onStateChanged(buttonState_, state); 
        // dispatch event
        queue_event(EventType::StateChanged, [this, state](Event& ev) {
            ev.setPayloadValue("old_state", static_cast<int>(buttonState_));
            ev.setPayloadValue("new_state", static_cast<int>(state));
            ev.setPayloadValue("buttonName", getName());
        });
        buttonState_ = state;
        // Update internal icon button if present. Prefer the internal child handle
        // created during onInit(); otherwise, try to resolve the conventional
        // child name '<thisname>_iconbutton' as a fallback.
        IconButton* ib = nullptr;
        if (iconButtonObject_.isValid()) {
            ib = iconButtonObject_.as<IconButton>();
        } else {
            // fallback: try to find the child by the conventional name
            std::string childName = getName() + "_iconbutton";
            DisplayHandle h = getFactory().getDisplayObject(childName);
            if (h.isValid()) ib = dynamic_cast<IconButton*>(h.get());
        }
        if (ib)
        {
            // int stateInt = static_cast<int>(state);
            // int iconIndex = stateInt + static_cast<int>(IconIndex::Checkbox_Empty);
            int iconIndex = static_cast<int>(iconIndexForState(state));
            ib->setIconIndex(static_cast<IconIndex>(iconIndex));
            setDirty(true);
        }
        else
        {
            // degrade gracefully: log debug instead of throwing an error so that
            // callers (including Factory/Init events) won't abort the program.
            DEBUG_LOG("TristateButton::setState() - internal IconButton not found for: " + getName());
        }
    } // END: CheckButton::setState()


    // --- Protected Virtual Methods (From IButtonObject) --- //
    void CheckButton::onStateChanged(ButtonState oldState, ButtonState newState)
    {
        // ...
    } // END: CheckButton::onStateChanged()

    IconIndex CheckButton::iconIndexForState(ButtonState state) const
    {
        switch (state) {
            case ButtonState::Inactive:   return IconIndex::Checkbox_Empty;
            case ButtonState::Active:     return IconIndex::Checkbox_Checked;
            default:                      return IconIndex::Checkbox_Empty;
        }
    } // END: CheckButton::iconIndexForState()

    // --- Data Members --- //
    // ...

    // --- Lua Registration --- //
    void CheckButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include IButtonObject bindings first
        IButtonObject::registerLuaBindings(lua);

        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "CheckButton";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // // Augment the single shared DisplayHandle handle usertype
        // sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);

        // // Helper to check if a property/command is already registered
        // auto absent = [&](const char* name) -> bool 
        // {
        //     sol::object cur = handle.raw_get_or(name, sol::lua_nil);
        //     return !cur.valid() || cur == sol::lua_nil;
        // };
        
    } // END: CheckButton::_registerLuaBindings()


} // END: namespace SDOM