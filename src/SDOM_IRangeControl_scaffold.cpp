// SDOM_IRangeControl_scaffold.cpp
#include <SDOM/SDOM.hpp>


#include <SDOM/SDOM_IRangeControl_scaffold.hpp>

namespace SDOM
{
    // --- Protected Constructors --- //
    IRangeControl_scaffold::IRangeControl_scaffold(const InitStruct& init) : IRangeControl(init)
    {
    } // END: IRangeControl_scaffold::IRangeControl_scaffold(const InitStruct& init)

    IRangeControl_scaffold::IRangeControl_scaffold(const sol::table& config) : IRangeControl(config)
    {
    } // END: IRangeControl_scaffold::IRangeControl_scaffold(const sol::table& config)

    // --- Virtual Methods --- //

    bool IRangeControl_scaffold::onInit()
    {
        // Call the base class onInit first to load the SpriteSheet
        return SUPER::onInit(); 
        // additional initialization as needed
        // ...
    } // END: bool IRangeControl_scaffold::onInit()

    void IRangeControl_scaffold::onQuit()
    {
        SUPER::onQuit();
    } // END: void IRangeControl_scaffold::onQuit()

    void IRangeControl_scaffold::onEvent(const Event& event)
    {
        SUPER::onEvent(event);
    } // END: void IRangeControl_scaffold::onEvent(const Event& event)

    void IRangeControl_scaffold::onUpdate(float fElapsedTime)
    {
        SUPER::onUpdate(fElapsedTime);
    } // END: void IRangeControl_scaffold::onUpdate(float fElapsedTime)

    void IRangeControl_scaffold::onRender()
    {
        SUPER::onRender();
    } // END: void IRangeControl_scaffold::onRender()

    bool IRangeControl_scaffold::onUnitTest()
    {
        return SUPER::onUnitTest();
    } // END: bool IRangeControl_scaffold::onUnitTest()




    // --- Public Accessors --- //




    // --- Public Mutators --- //



    
    // --- Protected Virtual Methods --- //        

    void IRangeControl_scaffold::_onValueChanged(float oldValue, float newValue)
    {
        SUPER::_onValueChanged(oldValue, newValue);
        // // dispatch event
        // queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
        //     ev.setPayloadValue("name", getName());
        //     ev.setPayloadValue("old_value", oldValue);
        //     ev.setPayloadValue("new_value", newValue);
        // });
    } // END: void IRangeControl_scaffold::_onValueChanged(float oldValue, float newValue)




    // --- Lua Registration --- //

    void IRangeControl_scaffold::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = typeName;
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

        // // EXAMPLE --- Expose min/max/value as properties (getters/setters)
        // if (absent("min")) {
        //     handle.set("min", sol::property(
        //         [](SDOM::DisplayHandle h) -> float {
        //             auto* r = h.as<IRangeControl>();
        //             return r ? r->getMin() : 0.0f;
        //         },
        //         [](SDOM::DisplayHandle h, double v) {
        //             auto* r = h.as<IRangeControl>();
        //             if (r) r->setMin(static_cast<float>(v));
        //         }
        //     ));
        // }

    } // END: void IRangeControl_scaffold::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM