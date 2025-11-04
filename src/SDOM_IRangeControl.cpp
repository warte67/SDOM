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

    bool IRangeControl::onLoad()
    {
        if (icon_sprite_sheet_ && !icon_sprite_sheet_->isLoaded())
        {
            try { icon_sprite_sheet_->onLoad(); } catch(...) {}
        }
        setDirty(true);
        return true;
    }

    void IRangeControl::onUnload()
    {
        if (cachedTexture_) { cachedTexture_ = nullptr; }
        current_width_ = 0;
        current_height_ = 0;
        current_pixel_format_ = SDL_PIXELFORMAT_UNKNOWN;
        cached_renderer_ = nullptr;
        setDirty(true);
    }

    void IRangeControl::onQuit() 
    {
        // destroy cached render texture if present
        if (cachedTexture_) {
            SDL_DestroyTexture(cachedTexture_);
            cachedTexture_ = nullptr;
        }
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
        // Base IRangeControl does not implement direct rendering; derived classes do.
    } // END: void IRangeControl::onRender()


    bool IRangeControl::onUnitTest(int frame)
    {
        // --- 1) Run base class tests first ------------------------------------------
        SUPER::onUnitTest(frame);
        UnitTests& ut = UnitTests::getInstance();
        constexpr const char* TypeName = "IRangeControl";

        // --- 2) Register tests once -------------------------------------------------
        static bool registered = false;
        if (!registered)
        {
            // --- Save original state to restore after all tests ---------------------
            const float originalMin = getMin();
            const float originalMax = getMax();
            const float originalVal = getValue();
            const Orientation originalOrient = getOrientation();

            // --- Individual Behavior Tests ------------------------------------------

            ut.add_test(TypeName, "Set and Get Min", [this](std::vector<std::string>& errors)
            {
                setMin(10.0f);
                if (getMin() != 10.0f)
                    errors.push_back("setMin/getMin failed — expected 10.0f");
                return true;
            });

            ut.add_test(TypeName, "Set and Get Max", [this](std::vector<std::string>& errors)
            {
                setMax(90.0f);
                if (getMax() != 90.0f)
                    errors.push_back("setMax/getMax failed — expected 90.0f");
                return true;
            });

            ut.add_test(TypeName, "Set and Get Value", [this](std::vector<std::string>& errors)
            {
                setValue(50.0f);
                if (getValue() != 50.0f)
                    errors.push_back("setValue/getValue failed — expected 50.0f");
                return true;
            });

            ut.add_test(TypeName, "Clamp Value Below Min", [this](std::vector<std::string>& errors)
            {
                setMin(10.0f);
                setMax(90.0f);
                setValue(5.0f);
                if (getValue() != 10.0f)
                    errors.push_back("Value not clamped to min (expected 10.0f)");
                return true;
            });

            ut.add_test(TypeName, "Clamp Value Above Max", [this](std::vector<std::string>& errors)
            {
                setMin(10.0f);
                setMax(90.0f);
                setValue(95.0f);
                if (getValue() != 90.0f)
                    errors.push_back("Value not clamped to max (expected 90.0f)");
                return true;
            });

            ut.add_test(TypeName, "Orientation Set Vertical", [this](std::vector<std::string>& errors)
            {
                setOrientation(Orientation::Vertical);
                if (getOrientation() != Orientation::Vertical)
                    errors.push_back("Failed to set vertical orientation");
                return true;
            });

            ut.add_test(TypeName, "Orientation Set Horizontal", [this](std::vector<std::string>& errors)
            {
                setOrientation(Orientation::Horizontal);
                if (getOrientation() != Orientation::Horizontal)
                    errors.push_back("Failed to set horizontal orientation");
                return true;
            });

            ut.add_test(TypeName, "Orientation Restore Original", [this, originalOrient](std::vector<std::string>& errors)
            {
                setOrientation(originalOrient);
                if (getOrientation() != originalOrient)
                    errors.push_back("Failed to restore original orientation");
                return true;
            });

            // --- State Restoration --------------------------------------------------
            ut.add_test(TypeName, "Restore Original State", [this, originalMin, originalMax, originalVal, originalOrient](std::vector<std::string>&)
            {
                setMin(originalMin);
                setMax(originalMax);
                setValue(originalVal);
                setOrientation(originalOrient);
                return true;
            });

            registered = true;
        }

        // ✅ Return false (one-time registration)
        return false;
    } // END -- IRangeControl::onUnitTest()





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

        // Augment the shared DisplayHandle with range-control helpers
        sol::table handleTbl;
        try { handleTbl = lua[SDOM::DisplayHandle::LuaHandleName]; } catch(...) {}
        if (!handleTbl.valid()) {
            handleTbl = SDOM::IDataObject::ensure_sol_table(lua, SDOM::DisplayHandle::LuaHandleName);
        }

        auto set_if_absent = [](sol::table& tbl, const char* name, auto&& fn) {
            sol::object cur = tbl.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                tbl.set_function(name, std::forward<decltype(fn)>(fn));
            }
        };

        auto getValue_bind = [](SDOM::DisplayHandle& self) -> float {
            auto* r = self.as<IRangeControl>();
            return r ? r->getValue() : 0.0f;
        };
        auto setValue_bind = [](SDOM::DisplayHandle& self, double v) {
            auto* r = self.as<IRangeControl>();
            if (r) r->setValue(static_cast<float>(v));
        };
        auto getMin_bind = [](SDOM::DisplayHandle& self) -> float {
            auto* r = self.as<IRangeControl>();
            return r ? r->getMin() : 0.0f;
        };
        auto setMin_bind = [](SDOM::DisplayHandle& self, double v) {
            auto* r = self.as<IRangeControl>();
            if (r) r->setMin(static_cast<float>(v));
        };
        auto getMax_bind = [](SDOM::DisplayHandle& self) -> float {
            auto* r = self.as<IRangeControl>();
            return r ? r->getMax() : 0.0f;
        };
        auto setMax_bind = [](SDOM::DisplayHandle& self, double v) {
            auto* r = self.as<IRangeControl>();
            if (r) r->setMax(static_cast<float>(v));
        };

        set_if_absent(handleTbl, "getValue", getValue_bind);
        set_if_absent(handleTbl, "setValue", setValue_bind);
        set_if_absent(handleTbl, "getMin", getMin_bind);
        set_if_absent(handleTbl, "setMin", setMin_bind);
        set_if_absent(handleTbl, "getMax", getMax_bind);
        set_if_absent(handleTbl, "setMax", setMax_bind);


    } // END: void IRangeControl::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


    // --- Cached rendering helper --- //
    bool IRangeControl::rebuildRangeTexture_(int width, int height, SDL_PixelFormat fmt)
    {
        bool needCreate = false;
        if (!cachedTexture_) {
            needCreate = true;
        } else {
            float tw = 0.0f, th = 0.0f;
            if (!SDL_GetTextureSize(cachedTexture_, &tw, &th)) {
                needCreate = true;
            } else if (static_cast<int>(tw) != width || static_cast<int>(th) != height || current_pixel_format_ != fmt) {
                needCreate = true;
            }
        }

        if (!needCreate) return true;

        if (cachedTexture_) {
            SDL_DestroyTexture(cachedTexture_);
            cachedTexture_ = nullptr;
        }

        if (width <= 0 || height <= 0) {
            current_pixel_format_ = fmt;
            current_width_ = width;
            current_height_ = height;
            return true;
        }

        cachedTexture_ = SDL_CreateTexture(getRenderer(), fmt, SDL_TEXTUREACCESS_TARGET, width, height);
        if (!cachedTexture_) {
            ERROR("IRangeControl::rebuildRangeTexture_: failed to create texture: " + std::string(SDL_GetError()));
            return false;
        }
        if (!SDL_SetTextureBlendMode(cachedTexture_, SDL_BLENDMODE_BLEND)) {
            ERROR("IRangeControl::rebuildRangeTexture_: failed to set blend mode: " + std::string(SDL_GetError()));
            return false;
        }
        if (!SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND)) {
            ERROR("IRangeControl::rebuildRangeTexture_: failed to set renderer blend mode: " + std::string(SDL_GetError()));
            return false;
        }
        // Range controls render cached textures 1:1; keep scaling nearest.
        SDL_SetTextureScaleMode(cachedTexture_, SDL_SCALEMODE_NEAREST);

        current_pixel_format_ = fmt;
        current_width_ = width;
        current_height_ = height;
        cached_renderer_ = getRenderer();
        return true;
    }

    // Cached-texture + window-resize contract
    //
    // Range controls that cache a render texture must drop and rebuild it
    // after SDL device/renderer changes. Core calls this after reconfigure
    // and on SDL resize events. Clear the cache, reset trackers, set dirty.
    void IRangeControl::onWindowResize(int /*logicalWidth*/, int /*logicalHeight*/)
    {
        // Drop reference only; avoid destroying during renderer transitions
        if (cachedTexture_) { cachedTexture_ = nullptr; }
        current_width_ = 0;
        current_height_ = 0;
        current_pixel_format_ = SDL_PIXELFORMAT_UNKNOWN;
        cached_renderer_ = nullptr;
        setDirty(true);
    }


} // END: namespace SDOM
