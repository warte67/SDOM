// SDOM_IRangeObject.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IRangeObject.hpp>

namespace SDOM
{
    // --- Protected Constructors --- //

    IRangeObject::IRangeObject(const InitStruct& init) : IDisplayObject(init), 
        min_(init.min), max_(init.max), value_(init.value), orientation_(init.orientation)
    {
    } // END: IRangeObject(const InitStruct& init)


    IRangeObject::IRangeObject(const sol::table& config) : IDisplayObject(config)
    {
        try 
        {
            if (config["min"].valid()) min_ = static_cast<float>(config.get_or("min", static_cast<double>(min_)));
            if (config["max"].valid()) max_ = static_cast<float>(config.get_or("max", static_cast<double>(max_)));
            if (config["value"].valid()) value_ = static_cast<float>(config.get_or("value", static_cast<double>(value_)));
            if (config["orientation"].valid()) 
            {
                if (config["orientation"].get_type() == sol::type::string) 
                {
                    std::string s = config.get<std::string>("orientation");
                    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
                    auto it = string_to_orientation.find(s);
                    if (it != string_to_orientation.end()) orientation_ = it->second;
                } 
                else if (config["orientation"].get_type() == sol::type::number) 
                {
                    int o = config.get<int>("orientation");
                    orientation_ = (o == 1) ? Orientation::Vertical : Orientation::Horizontal;
                }
            }
            // ensure value is clamped to min/max
            setValue(value_);
        } catch (...) { /* keep defaults on parse errors */ }
    } // END: IRangeObject(const sol::table& config)



    // --- Virtual Methods --- //

    bool IRangeObject::onInit() 
    {
        return SUPER::onInit();
    } // END: bool IRangeObject::onInit()


    void IRangeObject::onQuit() 
    {
        SUPER::onQuit();
    } // END: void IRangeObject::onQuit()


    void IRangeObject::onEvent(const Event& event) 
    {
        SUPER::onEvent(event);
    } // END: void IRangeObject::onEvent(const Event& event) 


    void IRangeObject::onUpdate(float fElapsedTime)
    {
        SUPER::onUpdate(fElapsedTime);
    } // END: void IRangeObject::onUpdate(float fElapsedTime)


    void IRangeObject::onRender()
    {
        // IDisplayObject does not have onRender to call

        // ... Custom rendering logic for the range object ...
    } // END: void IRangeObject::onRender()

    bool IRangeObject::onUnitTest() 
    {
        // Run base class tests
        bool baseTests = SUPER::onUnitTest();
        // Run IRangeObject specific tests
        bool allTests = baseTests;
        // Save current state
        float originalMin = getMin();
        float originalMax = getMax();
        float originalValue = getValue();
        Orientation originalOrientation = getOrientation();
        // Test setting and getting min
        setMin(10.0f);
        allTests &= (getMin() == 10.0f);
        // Test setting and getting max
        setMax(90.0f);
        allTests &= (getMax() == 90.0f);
        // Test setting and getting value within range
        setValue(50.0f);
        allTests &= (getValue() == 50.0f);
        // Test setting value below min clamps to min
        setValue(5.0f);
        allTests &= (getValue() == 10.0f);
        // Test setting value above max clamps to max
        setValue(95.0f);
        allTests &= (getValue() == 90.0f);
        // Test setting and getting orientation
        setOrientation(Orientation::Vertical);
        allTests &= (getOrientation() == Orientation::Vertical);
        // Test setting orientation back to horizontal
        setOrientation(Orientation::Horizontal);
        allTests &= (getOrientation() == Orientation::Horizontal);
        // Restore original state
        setMin(originalMin);
        setMax(originalMax);
        setValue(originalValue);
        setOrientation(originalOrientation);
        // Return the result of all tests
        return allTests;
    } // END: bool IRangeObject::onUnitTest()



    // --- Public Accessors --- //

    float IRangeObject::getMin() const { return min_; }
    float IRangeObject::getMax() const { return max_; }
    float IRangeObject::getValue() const { return value_; }
    Orientation IRangeObject::getOrientation() const { return orientation_; }



    // --- Public Mutators --- //

    void IRangeObject::setMin(float v) 
    {
        float clamped = std::max(0.0f, std::min(100.0f, v));
        if (clamped != min_) 
        {
            min_ = clamped;
            // Optionally clamp value_ if needed
            setValue(value_);
            setDirty(true);
        }
    } // END: void IRangeObject::setMin(float v)

    void IRangeObject::setMax(float v) 
    {
        float clamped = std::max(0.0f, std::min(100.0f, v));
        if (clamped != max_) 
        {
            max_ = clamped;
            // Optionally clamp value_ if needed
            setValue(value_);
            setDirty(true);
        }
    } // END: void IRangeObject::setMax(float v)

    void IRangeObject::setValue(float v) 
    {
        float clamped = std::max(min_, std::min(max_, v));
        if (clamped != value_) {
            float old = value_;
            value_ = clamped;
            _onValueChanged(old, value_);
            setDirty(true);
        }
    } // END: void IRangeObject::setValue(float v)

    // --- Protected Virtual Methods --- //

    void IRangeObject::setOrientation(Orientation o) 
    { 
        orientation_ = o; 
        setDirty(true); 
    } // END: void IRangeObject::setOrientation(Orientation o)



    // --- Protected Virtual Methods --- //

    void IRangeObject::_onValueChanged(float oldValue, float newValue)
    {
        // dispatch event
        queue_event(EventType::ValueChanged, [&, this](Event& ev) {
            ev.setPayloadValue("name", getName());
            ev.setPayloadValue("old_value", oldValue);
            ev.setPayloadValue("new_value", newValue);
        });

    } // END: void IRangeObject::_onValueChanged(float oldValue, float newValue)



    // --- Lua Registration --- //

    void IRangeObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IRangeObject";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // Augment the single shared DisplayHandle handle usertype
        sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);

        // Helper to check if a property/command is already registered
        auto absent = [&](const char* name) -> bool 
        {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        // Expose min/max/value as properties (getters/setters)
        if (absent("min")) {
            handle.set("min", sol::property(
                [](SDOM::DisplayHandle h) -> float {
                    auto* r = h.as<IRangeObject>();
                    return r ? r->getMin() : 0.0f;
                },
                [](SDOM::DisplayHandle h, double v) {
                    auto* r = h.as<IRangeObject>();
                    if (r) r->setMin(static_cast<float>(v));
                }
            ));
        }

        if (absent("max")) {
            handle.set("max", sol::property(
                [](SDOM::DisplayHandle h) -> float {
                    auto* r = h.as<IRangeObject>();
                    return r ? r->getMax() : 100.0f;
                },
                [](SDOM::DisplayHandle h, double v) {
                    auto* r = h.as<IRangeObject>();
                    if (r) r->setMax(static_cast<float>(v));
                }
            ));
        }

        if (absent("value")) {
            handle.set("value", sol::property(
                [](SDOM::DisplayHandle h) -> float {
                    auto* r = h.as<IRangeObject>();
                    return r ? r->getValue() : 0.0f;
                },
                [](SDOM::DisplayHandle h, double v) {
                    auto* r = h.as<IRangeObject>();
                    if (r) r->setValue(static_cast<float>(v));
                }
            ));
        }

        // Also expose explicit functions for script-friendly access
        if (absent("getMin")) {
            handle.set_function("getMin", [](SDOM::DisplayHandle h) {
                auto* r = h.as<IRangeObject>();
                return r ? r->getMin() : 0.0f;
            });
        }
        if (absent("getMax")) {
            handle.set_function("getMax", [](SDOM::DisplayHandle h) {
                auto* r = h.as<IRangeObject>();
                return r ? r->getMax() : 100.0f;
            });
        }
        if (absent("getValue")) {
            handle.set_function("getValue", [](SDOM::DisplayHandle h) {
                auto* r = h.as<IRangeObject>();
                return r ? r->getValue() : 0.0f;
            });
        }

        if (absent("setMin")) {
            handle.set_function("setMin", [](SDOM::DisplayHandle h, double v) {
                auto* r = h.as<IRangeObject>();
                if (r) r->setMin(static_cast<float>(v));
            });
        }
        if (absent("setMax")) {
            handle.set_function("setMax", [](SDOM::DisplayHandle h, double v) {
                auto* r = h.as<IRangeObject>();
                if (r) r->setMax(static_cast<float>(v));
            });
        }
        if (absent("setValue")) {
            handle.set_function("setValue", [](SDOM::DisplayHandle h, double v) {
                auto* r = h.as<IRangeObject>();
                if (r) r->setValue(static_cast<float>(v));
            });
        }

        // Expose orientation as property + helpers
        if (absent("orientation")) {
            handle.set("orientation", sol::property(
                [](SDOM::DisplayHandle h) -> int {
                    auto* r = h.as<IRangeObject>();
                    return r ? static_cast<int>(r->getOrientation()) : static_cast<int>(Orientation::Horizontal);
                },
                [](SDOM::DisplayHandle h, int v) {
                    auto* r = h.as<IRangeObject>();
                    if (r) r->setOrientation(static_cast<Orientation>(v));
                }
            ));
        }
        if (absent("getOrientation")) {
            handle.set_function("getOrientation", [](SDOM::DisplayHandle h) {
                auto* r = h.as<IRangeObject>();
                return r ? static_cast<int>(r->getOrientation()) : static_cast<int>(Orientation::Horizontal);
            });
        }
        if (absent("setOrientation")) {
            handle.set_function("setOrientation", [](SDOM::DisplayHandle h, int v) {
                auto* r = h.as<IRangeObject>();
                if (r) r->setOrientation(static_cast<Orientation>(v));
            });
        }

    } // END: void IRangeObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM