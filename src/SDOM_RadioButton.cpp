// SDOM_RadioButton.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>

#include <SDOM/SDOM_RadioButton.hpp>

namespace SDOM
{
    namespace {
        inline IconIndex radioIconIndexFromState(ButtonState state) {
            switch (state) {
                case ButtonState::Inactive:   return IconIndex::Radiobox_Unselected;
                case ButtonState::Active:     return IconIndex::Radiobox_Selected;
                default:                      return IconIndex::Hamburger;
            }
        }
    }
    // --- Constructors --- //
    RadioButton::RadioButton(const InitStruct& init): TristateButton(init)
    {
        setType(TypeName);
        // compute icon index with local helper to avoid virtual dispatch during construction
        icon_index_ = radioIconIndexFromState(buttonState_);

        selected_ = init.selected;
        if (selected_)
        {
            buttonState_ = ButtonState::Checked;
            icon_index_ = radioIconIndexFromState(buttonState_);
        }

        // add custom properties here
    } // END: RadioButton::RadioButton(const InitStruct& init)


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
        

    bool RadioButton::onUnitTest(int frame)
    {
        // Run base class tests first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Register tests once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Validate state enum correctness
            ut.add_test(objName, "RadioButton State Validity", [this](std::vector<std::string>& errors)
            {
                ButtonState s = getState();
                switch (s)
                {
                    case ButtonState::Inactive:
                    case ButtonState::Active:
                    case ButtonState::Mixed:
                        break;
                    default:
                        errors.push_back("RadioButton '" + getName() +
                                        "' has invalid state: " + std::to_string(static_cast<int>(s)));
                        break;
                }
                return true; // ✅ single-frame test
            });

            // 🔹 2. Validate icon sprite sheet (if configured)
            ut.add_test(objName, "RadioButton Icon SpriteSheet Dimensions", [this](std::vector<std::string>& errors)
            {
                SpriteSheet* ss = getIconSpriteSheet();
                if (ss)
                {
                    if (ss->getSpriteWidth() <= 0 || ss->getSpriteHeight() <= 0)
                    {
                        errors.push_back("RadioButton '" + getName() +
                                        "' has invalid icon sprite size: w=" +
                                        std::to_string(ss->getSpriteWidth()) +
                                        " h=" + std::to_string(ss->getSpriteHeight()));
                    }
                }
                return true; // ✅ single-frame test
            });

            registered = true;
        }

        // ✅ Return false to stay consistent with the global unit test update loop
        return false;
    } // END: RadioButton::onUnitTest()



    // --- Virtual State Accessors (From IButtonObject) --- //
    ButtonState RadioButton::getState() const
    {
        return buttonState_;
    } // END: RadioButton::getState()

    void RadioButton::setState(ButtonState state)
    {
        if (state == buttonState_) return; // no change
        // INFO(getType() +"::setState() - on '" + getName() + "' new state: " + std::to_string(static_cast<int>(getState())));

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
        (void)oldState; // stop compiler warning
        (void)newState; // stop compiler warning
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


    
    void RadioButton::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "RadioButton");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    

} // END: namespace SDOM
