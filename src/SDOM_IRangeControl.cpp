// SDOM_IRangeControl.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IRangeControl.hpp>

namespace SDOM
{
    // --- Protected Constructors --- //

    IRangeControl::IRangeControl(const InitStruct& init) : IDisplayObject(init), 
        min_(init.min), max_(init.max), value_(init.value), orientation_(init.orientation), icon_resource_(init.icon_resource),
        pending_old_min_(std::numeric_limits<float>::quiet_NaN()), pending_old_max_(std::numeric_limits<float>::quiet_NaN())        
    {
    } // END: IRangeControl(const InitStruct& init)


    IRangeControl::IRangeControl(const sol::table& config) : IDisplayObject(config)
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
            if (config["icon_resource"].valid())
                icon_resource_ = config.get<std::string>("icon_resource");
            // ensure value is clamped to min/max
            setValue(value_);
        } catch (...) { /* keep defaults on parse errors */ }
    } // END: IRangeControl(const sol::table& config)


    IRangeControl::IRangeControl(const sol::table& config, const InitStruct& defaults)
        : IDisplayObject(config, defaults)
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
            if (config["icon_resource"].valid())
                icon_resource_ = config.get<std::string>("icon_resource");
            // ensure value is clamped to min/max
            setValue(value_);
        } catch (...) { /* keep defaults on parse errors */ }
    } // END: IRangeControl(const sol::table& config, const InitStruct& defaults)



    // --- Virtual Methods --- //

    bool IRangeControl::onInit() 
    {
        Factory& factory = getFactory();
        bool ret = SUPER::onInit();
        if (ret) 
        {
            // Load icon sprite sheet
            std::string resource_name = icon_resource_ + "_SpriteSheet";
            AssetHandle ss = factory.getAssetObject(resource_name);
            if (!ss.isValid()) {
                // Also accept a SpriteSheet or Texture registered under the plain
                // icon resource name (common when resources are created from Lua)
                ss = factory.getAssetObject(icon_resource_);
            }

            if (ss.isValid())
            {
                // If the asset exists, handle either SpriteSheet or Texture
                icon_sprite_sheet_ = ss.as<SpriteSheet>();
                if (icon_sprite_sheet_)
                {
                    if (!icon_sprite_sheet_->isLoaded())
                        icon_sprite_sheet_->onLoad();
                }
                else
                {
                    // If the registered asset is a Texture, create a SpriteSheet wrapper
                    IAssetObject* obj = ss.get();
                    if (obj && obj->getType() == Texture::TypeName)
                    {
                        // Create a SpriteSheet that references the existing texture filename.
                        // Prefer to reuse the sprite tile size from any existing SpriteSheet
                        // registered for the same filename so we don't default to 8x8.
                        int chosenW = 8;
                        int chosenH = 8;
                        try {
                            // If a SpriteSheet with matching filename exists, use its tile size
                            AssetHandle candidate = factory.findAssetByFilename(icon_resource_, SpriteSheet::TypeName);
                            if (candidate.isValid()) {
                                AssetHandle candidateSS = candidate;
                                SpriteSheet* ssPtr = candidateSS.as<SpriteSheet>();
                                if (ssPtr) {
                                    chosenW = ssPtr->getSpriteWidth();
                                    chosenH = ssPtr->getSpriteHeight();
                                }
                            } else {
                                // Try name-based inference as a fallback (e.g., icon_12x12)
                                auto pos = icon_resource_.rfind('_');
                                if (pos != std::string::npos && pos + 1 < icon_resource_.size()) {
                                    std::string suffix = icon_resource_.substr(pos + 1);
                                    auto xPos = suffix.find('x');
                                    if (xPos != std::string::npos) {
                                        std::string wstr = suffix.substr(0, xPos);
                                        std::string hstr = suffix.substr(xPos + 1);
                                        if (!wstr.empty() && !hstr.empty()) {
                                            int w = std::stoi(wstr);
                                            int h = std::stoi(hstr);
                                            if (w > 0 && h > 0) {
                                                chosenW = w;
                                                chosenH = h;
                                            }
                                        }
                                    }
                                }
                            }
                        } catch(...) {}

                        SpriteSheet::InitStruct init;
                        init.name = resource_name;
                        init.type = "SpriteSheet";
                        init.filename = icon_resource_;
                        init.isInternal = true;
                        init.spriteWidth = chosenW;
                        init.spriteHeight = chosenH;

                        AssetHandle created = factory.createAsset("SpriteSheet", init);
                        if (created.isValid()) {
                            icon_sprite_sheet_ = created.as<SpriteSheet>();
                            if (icon_sprite_sheet_ && !icon_sprite_sheet_->isLoaded()) icon_sprite_sheet_->onLoad();
                        } else {
                            ERROR("Error: IRangeControl '" + getName() + "' failed to create SpriteSheet wrapper for Texture resource '" + icon_resource_ + "'");
                            ret = false;
                        }
                    }
                    else
                    {
                        // Found an asset with that name but wrong type
                        ERROR("Error: IRangeControl '" + getName() + "' found asset '" + icon_resource_ + "' but it is not a SpriteSheet or Texture");
                        ret = false;
                    }
                }
            }
            else
            {
                // Asset not found — create the SpriteSheet asset.
                // Try to infer sprite tile size from the icon resource name by
                // parsing a trailing pattern like "_12x12". If parsing fails
                // fall back to the historical default of 8x8.
                int inferredW = 8;
                int inferredH = 8;
                try {
                    // Look for the last '_' and then look for an 'x' between numbers
                    auto pos = icon_resource_.rfind('_');
                    if (pos != std::string::npos && pos + 1 < icon_resource_.size()) {
                        std::string suffix = icon_resource_.substr(pos + 1);
                        auto xPos = suffix.find('x');
                        if (xPos != std::string::npos) {
                            std::string wstr = suffix.substr(0, xPos);
                            std::string hstr = suffix.substr(xPos + 1);
                            if (!wstr.empty() && !hstr.empty()) {
                                int w = std::stoi(wstr);
                                int h = std::stoi(hstr);
                                if (w > 0 && h > 0) {
                                    inferredW = w;
                                    inferredH = h;
                                }
                            }
                        }
                    }
                } catch (...) { /* if parsing fails, keep defaults */ }

                SpriteSheet::InitStruct init;
                init.name = resource_name;
                init.type = "SpriteSheet";
                init.filename = icon_resource_;
                init.isInternal = true;
                init.spriteWidth = inferredW;
                init.spriteHeight = inferredH;
                ss = factory.createAsset("SpriteSheet", init);
                if (ss.isValid())
                {
                    icon_sprite_sheet_ = ss.as<SpriteSheet>();
                    if (icon_sprite_sheet_)
                    {
                        if (!icon_sprite_sheet_->isLoaded())
                            icon_sprite_sheet_->onLoad();
                    }
                    else
                    {
                        ERROR("Error: IRangeControl '" + getName() + "' failed to create SpriteSheet asset for icon resource '" + icon_resource_ + "'");
                        ret = false;
                    }
                }
                else
                {
                    ERROR("Error: IRangeControl '" + getName() + "' failed to create AssetHandle for icon resource '" + icon_resource_ + "'");
                    ret = false;
                }
            }
        }
        return ret;
    } // END: bool IRangeControl::onInit()

    void IRangeControl::onQuit() 
    {
        SUPER::onQuit();
    } // END: void IRangeControl::onQuit()


    void IRangeControl::onEvent(const Event& event) 
    {
        SUPER::onEvent(event);
    } // END: void IRangeControl::onEvent(const Event& event) 


    void IRangeControl::onUpdate(float fElapsedTime)
    {
        SUPER::onUpdate(fElapsedTime);
    } // END: void IRangeControl::onUpdate(float fElapsedTime)


    void IRangeControl::onRender()
    {
        // IDisplayObject does not have onRender to call

        // ... Custom rendering logic for the range object ...
    } // END: void IRangeControl::onRender()

    bool IRangeControl::onUnitTest() 
    {
        // Run base class tests
        bool baseTests = SUPER::onUnitTest();
        // Run IRangeControl specific tests
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
    } // END: bool IRangeControl::onUnitTest()



    // --- Public Accessors --- //

    float IRangeControl::getMin() const { return min_; }
    float IRangeControl::getMax() const { return max_; }
    float IRangeControl::getValue() const { return value_; }
    Orientation IRangeControl::getOrientation() const { return orientation_; }
    std::string IRangeControl::getIconResource() const { return icon_resource_; }



    // --- Public Mutators --- //

    void IRangeControl::setMin(float v) 
    {
        float clamped = std::max(0.0f, std::min(100.0f, v));
        if (clamped != min_) 
        {
            // capture old min for the event payload
            pending_old_min_ = min_;
            pending_old_max_ = std::numeric_limits<float>::quiet_NaN(); // clear unrelated pending
            min_ = clamped;
            // Optionally clamp value_ if needed
            setValue(value_);
            setDirty(true);
        }
    } // END: void IRangeControl::setMin(float v)

    void IRangeControl::setMax(float v) 
    {
        float clamped = std::max(0.0f, std::min(100.0f, v));
        if (clamped != max_) 
        {
            // capture old max for the event payload
            pending_old_max_ = max_;
            pending_old_min_ = std::numeric_limits<float>::quiet_NaN(); // clear unrelated pending
            max_ = clamped;
            // Optionally clamp value_ if needed
            setValue(value_);
            setDirty(true);
        }
    } // END: void IRangeControl::setMax(float v)

    void IRangeControl::setValue(float v) 
    {
        float clamped = std::max(min_, std::min(max_, v));
        if (clamped != value_) {
                float old = value_;
                value_ = clamped;
            _onValueChanged(old, value_);
            setDirty(true);
        }
    } // END: void IRangeControl::setValue(float v)

    // --- Protected Virtual Methods --- //

    void IRangeControl::setOrientation(Orientation o) 
    { 
        orientation_ = o; 
        setDirty(true); 
    } // END: void IRangeControl::setOrientation(Orientation o)



    // --- Protected Virtual Methods --- //

    void IRangeControl::_onValueChanged(float oldValue, float newValue)
    {
    // dispatch event
        queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
            ev.setPayloadValue("name", getName());
            ev.setPayloadValue("old_value", oldValue);
            ev.setPayloadValue("new_value", newValue);
            // always include current bounds
            ev.setPayloadValue("current_min", getMin());
            ev.setPayloadValue("current_max", getMax());
            // include old bounds if we captured them
            if (!std::isnan(pending_old_min_)) {
                ev.setPayloadValue("old_min", pending_old_min_);
            }
            if (!std::isnan(pending_old_max_)) {
                ev.setPayloadValue("old_max", pending_old_max_);
            }
            // clear pending markers
            pending_old_min_ = std::numeric_limits<float>::quiet_NaN();
            pending_old_max_ = std::numeric_limits<float>::quiet_NaN();
        });

    } // END: void IRangeControl::_onValueChanged(float oldValue, float newValue)




    // --- Lua Registration --- //

    void IRangeControl::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IRangeControl";
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
                    auto* r = h.as<IRangeControl>();
                    return r ? r->getMin() : 0.0f;
                },
                [](SDOM::DisplayHandle h, double v) {
                    auto* r = h.as<IRangeControl>();
                    if (r) r->setMin(static_cast<float>(v));
                }
            ));
        }

        if (absent("max")) {
            handle.set("max", sol::property(
                [](SDOM::DisplayHandle h) -> float {
                    auto* r = h.as<IRangeControl>();
                    return r ? r->getMax() : 100.0f;
                },
                [](SDOM::DisplayHandle h, double v) {
                    auto* r = h.as<IRangeControl>();
                    if (r) r->setMax(static_cast<float>(v));
                }
            ));
        }

        if (absent("value")) {
            handle.set("value", sol::property(
                [](SDOM::DisplayHandle h) -> float {
                    auto* r = h.as<IRangeControl>();
                    return r ? r->getValue() : 0.0f;
                },
                [](SDOM::DisplayHandle h, double v) {
                    auto* r = h.as<IRangeControl>();
                    if (r) r->setValue(static_cast<float>(v));
                }
            ));
        }

        if (absent("icon_resource")) {
            handle.set("icon_resource", sol::property(
                [](SDOM::DisplayHandle h) -> std::string {
                    auto* r = h.as<IRangeControl>();
                    return r ? r->getIconResource() : "internal_icon_8x8";
                }
            ));
        }

        // Also expose explicit functions for script-friendly access
        if (absent("getMin")) {
            handle.set_function("getMin", [](SDOM::DisplayHandle h) {
                auto* r = h.as<IRangeControl>();
                return r ? r->getMin() : 0.0f;
            });
        }
        if (absent("getMax")) {
            handle.set_function("getMax", [](SDOM::DisplayHandle h) {
                auto* r = h.as<IRangeControl>();
                return r ? r->getMax() : 100.0f;
            });
        }
        if (absent("getValue")) {
            handle.set_function("getValue", [](SDOM::DisplayHandle h) {
                auto* r = h.as<IRangeControl>();
                return r ? r->getValue() : 0.0f;
            });
        }
        if (absent("getIconResource")) {
            handle.set_function("getIconResource", [](SDOM::DisplayHandle h) {
                auto* r = h.as<IRangeControl>();
                return r ? r->getIconResource() : "internal_icon_8x8";
            });
        }

        if (absent("setMin")) {
            handle.set_function("setMin", [](SDOM::DisplayHandle h, double v) {
                auto* r = h.as<IRangeControl>();
                if (r) r->setMin(static_cast<float>(v));
            });
        }
        if (absent("setMax")) {
            handle.set_function("setMax", [](SDOM::DisplayHandle h, double v) {
                auto* r = h.as<IRangeControl>();
                if (r) r->setMax(static_cast<float>(v));
            });
        }
        if (absent("setValue")) {
            handle.set_function("setValue", [](SDOM::DisplayHandle h, double v) {
                auto* r = h.as<IRangeControl>();
                if (r) r->setValue(static_cast<float>(v));
            });
        }        

        // Expose orientation as property + helpers
        if (absent("orientation")) {
            handle.set("orientation", sol::property(
                [](SDOM::DisplayHandle h) -> int {
                    auto* r = h.as<IRangeControl>();
                    return r ? static_cast<int>(r->getOrientation()) : static_cast<int>(Orientation::Horizontal);
                },
                [](SDOM::DisplayHandle h, int v) {
                    auto* r = h.as<IRangeControl>();
                    if (r) r->setOrientation(static_cast<Orientation>(v));
                }
            ));
        }
        if (absent("getOrientation")) {
            handle.set_function("getOrientation", [](SDOM::DisplayHandle h) {
                auto* r = h.as<IRangeControl>();
                return r ? static_cast<int>(r->getOrientation()) : static_cast<int>(Orientation::Horizontal);
            });
        }
        if (absent("setOrientation")) {
            handle.set_function("setOrientation", [](SDOM::DisplayHandle h, int v) {
                auto* r = h.as<IRangeControl>();
                if (r) r->setOrientation(static_cast<Orientation>(v));
            });
        }

    } // END: void IRangeControl::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM