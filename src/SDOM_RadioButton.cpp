// SDOM_RadioButton.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>

#include <SDOM/SDOM_RadioButton.hpp>

namespace SDOM
{
    // --- Constructors --- //
    RadioButton::RadioButton(const InitStruct& init): TristateButton(init)
    {
        setType(TypeName);
        icon_index_ = iconIndexForState(buttonState_); // set icon index based on initial state

        // add custom properties here
    } // END: RadioButton::RadioButton(const InitStruct& init)

    RadioButton::RadioButton(const sol::table& config) : TristateButton(config, RadioButton::InitStruct())
    {
        setType(TypeName);
        icon_index_ = iconIndexForState(buttonState_); // set icon index based on initial state

        InitStruct init; // default init struct to fall back on
        
        // accept multiple aliases for the checked state
        if (config["is_checked"].valid()) {
            try { selected_ = config["is_checked"].get<bool>(); } catch(...) {}
        } else if (config["checked"].valid()) {
            try { selected_ = config["checked"].get<bool>(); } catch(...) {}
        } else if (config["is_selected"].valid()) {
            try { selected_ = config["is_selected"].get<bool>(); } catch(...) {}
        } else if (config["selected"].valid()) {
            try { selected_ = config["selected"].get<bool>(); } catch(...) {}
        } else {
            selected_ = init.selected;
        }
        // if (selected_)
        //     clearSiblings_();
        // setState(selected_ ? ButtonState::Checked : ButtonState::Unchecked);

        // add custom properties here
    } // END: RadioButton::RadioButton(const sol::table& config)


    // --- Lifecycle & Core Virtuals --- //
    bool RadioButton::onInit()
    {
        bool ret = SUPER::onInit();
        return ret;
    } // END: RadioButton::onInit()

    void RadioButton::onQuit()
    {

    } // END: RadioButton::onQuit()

    void RadioButton::onUpdate(float fElapsedTime)
    {
        // Only allow a single "selected" radio button in a group. 
        // This will make sure to select the last "selected" radio in the group
        if (selected_)
        {
            selected_ = false; // reset
            clearSiblings_();
            setState(ButtonState::Checked);
        }
        // Make a call to the SUPER class to handle the rest of the update
        SUPER::onUpdate(fElapsedTime);
    } // END: RadioButton::onUpdate()

    void RadioButton::onEvent(const Event& event)
    {
        // SUPER::onEvent(event);

        // only target phase
        if (event.getPhase() != Event::Phase::Target) { return; }
        if (event.getType() == EventType::MouseClick) 
        { 
            // clear all sibblings first
            clearSiblings_();
            // then mark this one checked
            setState(ButtonState::Checked);
        }
    } // END: RadioButton::onEvent()

    void RadioButton::onRender()
    {
        SUPER::onRender();

    } // END: RadioButton::onRender()

    bool RadioButton::onUnitTest() 
    {
        return true;
    } // END: RadioButton::onUnitTest()

    // --- Virtual State Accessors (From IButtonObject) --- //
    ButtonState RadioButton::getState() const
    {
        return buttonState_;
    } // END: RadioButton::getState()

    void RadioButton::setState(ButtonState state)
    {
        if (state == buttonState_) return; // no change
        INFO(getType() +"::setState() - on '" + getName() + "' new state: " + std::to_string(static_cast<int>(getState())));

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
    } // END: RadioButton::setState()


    // --- Protected Virtual Methods (From IButtonObject) --- //
    void RadioButton::onStateChanged(ButtonState oldState, ButtonState newState)
    {
        // ...
    } // END: RadioButton::onStateChanged()

    IconIndex RadioButton::iconIndexForState(ButtonState state) const
    {
        switch (state) {
            case ButtonState::Inactive:   return IconIndex::Radiobox_Unselected;
            case ButtonState::Active:     return IconIndex::Radiobox_Selected;
            default:                      return IconIndex::Hamburger;
        }
    } // END: RadioButton::iconIndexForState()



    // --- Protected Helpers --- //
    void RadioButton::clearSiblings_()
    {
        DisplayHandle parent = getParent();
        if (parent.isValid())
        {
            const auto& children = parent->getChildren();
            for (const auto& child : children)
            {
                if (!child.isValid()) continue;
                if (child->getType() != RadioButton::TypeName) continue;
                RadioButton* rb = child.as<RadioButton>();
                rb->setState(ButtonState::Unchecked);
            }   
        }
    } // END: RadioButton::clearSiblings_()


    // --- Lua Registration --- //
    void RadioButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include IButtonObject bindings first
        IButtonObject::registerLuaBindings(lua);

        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "RadioButton";
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
        
    } // END: RadioButton::_registerLuaBindings()


} // END: namespace SDOM