// SDOM_Slider.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IconIndex.hpp>
#include <SDOM/SDOM_Slider.hpp>

namespace SDOM
{
    // --- Protected Constructors --- //
    Slider::Slider(const InitStruct& init) : IRangeControl(init)
    {
        step_ = init.step;
    } // END: Slider::Slider(const InitStruct& init)

    Slider::Slider(const sol::table& config) : IRangeControl(config, InitStruct())
    {
        // Parse step from Lua config
        step_ = lua_value_case_insensitive<float>(config, "step", 0.0f);

        // Now that we forwarded Slider::InitStruct into the base parsing, no further
        // default-fixups are necessary here.
    } // END: Slider::Slider(const sol::table& config)




    // --- Virtual Methods --- //

    bool Slider::onInit()
    {
        // Call the base class onInit first to load the SpriteSheet
        return SUPER::onInit(); 
        // additional initialization as needed
        // ...
    } // END: bool Slider::onInit()

    void Slider::onQuit()
    {
        SUPER::onQuit();
    } // END: void Slider::onQuit()

    void Slider::onEvent(const Event& event)
    {
        // First, call the base class event handler
        SUPER::onEvent(event);
        // Then handle Slider-specific events
        auto snapToStep = [&](float v) -> float {
            if (step_ > 0.0f)
                return min_ + std::round((v - min_) / step_) * step_;
            return v;
        };
        // Early return if not a target phase event
        if (event.getPhase() != Event::Phase::Target) { return; } // Only handle target phase events

        // Mouse click or drag
        if (event.getType() == EventType::MouseButtonDown || event.getType() == EventType::Dragging)
        {
            if (!isEnabled() || !isClickable()) return;
            float mouseX = event.getMouseX();
            float mouseY = event.getMouseY();
            if (orientation_ == Orientation::Horizontal) 
            {
                if (mouseX >= getX() && mouseX <= getX() + getWidth() ) 
                {
                    float trackStart = getX();
                    float trackWidth = getWidth();
                    float rel = (mouseX - trackStart) / trackWidth;
                    rel = std::clamp(rel, 0.0f, 1.0f);
                    float newValue = min_ + rel * (max_ - min_);
                    newValue = snapToStep(newValue);
                    float oldValue = value_;
                    if (newValue == oldValue) return;
                    setValue(newValue);
                    queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
                        ev.setPayloadValue("name", getName());
                        ev.setPayloadValue("old_value", oldValue);
                        ev.setPayloadValue("new_value", newValue);
                    });
                }
            }
            else // vertical
            {
                if (mouseY >= getY() && mouseY <= getY() + getHeight() ) 
                {
                    float trackStart = getY();
                    float trackHeight = getHeight();
                    float rel = (mouseY - trackStart) / trackHeight;
                    rel = std::clamp(rel, 0.0f, 1.0f);
                    // Invert rel so that 0 is bottom and 1 is top for value mapping
                    float ratio = 1.0f - rel;
                    float newValue = min_ + ratio * (max_ - min_);
                    newValue = snapToStep(newValue);
                    float oldValue = value_;
                    if (newValue == oldValue) return;
                    setValue(newValue);
                    queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
                        ev.setPayloadValue("name", getName());
                        ev.setPayloadValue("old_value", oldValue);
                        ev.setPayloadValue("new_value", newValue);
                    });
                }                
            }
        } // END: Mouse click or drag

        // MouseWheel event to adjust the slider value
        if (event.getType() == EventType::MouseWheel)
        {
            SDL_Keymod modState = SDL_GetModState();
            float wheel_delta = event.getWheelY() * ((step_ > 0.0f) ? step_ : 1.0f);
            if (modState & SDL_KMOD_SHIFT)
                wheel_delta *= 10.0f; // larger step with SHIFT key
            // For vertical sliders, do not invert wheel delta: positive wheel (up) should increase value (move thumb up)
            float oldValue = value_;
            float newValue = snapToStep(std::clamp(value_ + wheel_delta, min_, max_));
            if (newValue == oldValue) return;
            setValue(newValue);
            queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
                ev.setPayloadValue("name", getName());
                ev.setPayloadValue("old_value", oldValue);
                ev.setPayloadValue("new_value", newValue);
            });
        } // END: MouseWheel event

        // Key Events to adjust the slider value (if key focused)
        if (event.getType() == EventType::KeyDown)
        {
            SDL_Scancode scancode = event.getScanCode();
            float oldValue = value_;
            float newValue = oldValue;
            if (orientation_ == Orientation::Horizontal)
            {
                // LEFT and RIGHT arrows
                if (scancode == SDL_SCANCODE_LEFT)
                    newValue = value_ - ((step_ > 0.0f) ? step_ : 1.0f);
                else if (scancode == SDL_SCANCODE_RIGHT)
                    newValue = value_ + ((step_ > 0.0f) ? step_ : 1.0f);
                // PgUP and PgDN keys
                if (scancode == SDL_SCANCODE_PAGEUP)
                    newValue = value_ + ((step_ > 0.0f) ? step_*10.0f : 10.0f);
                else if (scancode == SDL_SCANCODE_PAGEDOWN)
                    newValue = value_ - ((step_ > 0.0f) ? step_*10.0f : 10.0f);
                // HOME and END keys
                if (scancode == SDL_SCANCODE_HOME)
                    newValue = min_;
                else if (scancode == SDL_SCANCODE_END)
                    newValue = max_;
            }
            else // vertical
            {
                // UP should increase value (move thumb up), DOWN should decrease
                if (scancode == SDL_SCANCODE_DOWN)
                    newValue = value_ - ((step_ > 0.0f) ? step_ : 1.0f);
                else if (scancode == SDL_SCANCODE_UP)
                    newValue = value_ + ((step_ > 0.0f) ? step_ : 1.0f);
                // PgUP and PgDN keys: PageUp increases, PageDown decreases
                if (scancode == SDL_SCANCODE_PAGEUP)
                    newValue = value_ + ((step_ > 0.0f) ? step_*10.0f : 10.0f);
                else if (scancode == SDL_SCANCODE_PAGEDOWN)
                    newValue = value_ - ((step_ > 0.0f) ? step_*10.0f : 10.0f);
                // HOME and END keys
                if (scancode == SDL_SCANCODE_HOME)
                    newValue = min_;
                else if (scancode == SDL_SCANCODE_END)
                    newValue = max_;
            }
            newValue = snapToStep(std::clamp(newValue, min_, max_));
            if (newValue != oldValue)
            {
                setValue(newValue);
                queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
                    ev.setPayloadValue("name", getName());
                    ev.setPayloadValue("old_value", oldValue);
                    ev.setPayloadValue("new_value", newValue);
                });
            }
        } // END: Key Events to adjust the slider value
    } // END: void Slider::onEvent(const Event& event)

    void Slider::onUpdate(float fElapsedTime)
    {
        SUPER::onUpdate(fElapsedTime);
    } // END: void Slider::onUpdate(float fElapsedTime)

    void Slider::onRender()
    {
        // SUPER::onRender();  // Nothing for the SUPER to render

        // --- Render the Slider Control --- //

        SpriteSheet* ss = getIconSpriteSheet(); 
        if (!ss)  ERROR("Slider::onRender(): No valid SpriteSheet for icon.");

        SDL_FRect dsstRect = { 
            static_cast<float>(getX()), 
            static_cast<float>(getY()),
            static_cast<float>(getWidth()), 
            static_cast<float>(getHeight()) 
        };

        // Render Background Color
        SDL_Color bgColor = getBackgroundColor();
        if (bgColor.a > 0)
        {
            SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(getRenderer(), bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_RenderFillRect(getRenderer(), &dsstRect);
        }

        // Render Border Color
        SDL_Color borderColor = getBorderColor();
        if (borderColor.a > 0)
        {
            SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(getRenderer(), borderColor.r, borderColor.g, borderColor.b, borderColor.a);
            SDL_RenderRect(getRenderer(), &dsstRect);
        }

        // horizontal slider
        if (orientation_ == Orientation::Horizontal)
        {
            // Draw the track
            SDL_Color trackColor = getColor();
            if (trackColor.a > 0)
            {
                // draw the track as a stretched sprite
                ss->drawSprite(static_cast<int>(IconIndex::HSlider_Rail), dsstRect, trackColor, SDL_SCALEMODE_NEAREST);

                // draw small ticks along the track
                double x_min = getX() - 3;
                double x_max = getX() + getWidth() - 4;
                double step = (x_max - x_min)/10.0;
                for (double h = x_min; h <= x_max; h += step)
                {
                    ss->drawSprite(static_cast<int>(IconIndex::Slider_Tick), 
                        static_cast<int>(h), 
                        static_cast<int>(getY()), 
                        trackColor, 
                        SDL_SCALEMODE_NEAREST
                    );            
                }        
            }
            // Draw the thumb
            SDL_Color thumbColor = getForegroundColor();
            // double thumbX = ( getX() + ((value_ - min_) / (max_ - min_)) * (getWidth() + 8) ) - 3;
            double thumbX = (getX() + ((value_ - min_) / (max_ - min_)) * (getWidth())) - 5;
            if (thumbColor.a > 0)
            {
                ss->drawSprite(static_cast<int>(IconIndex::Knob_Horizontal), 
                    static_cast<int>(thumbX), 
                    static_cast<int>(getY()), 
                    thumbColor, 
                    SDL_SCALEMODE_NEAREST
                );                   
            }
        }
        else // vertical
        {
            // Draw the track
            SDL_Color trackColor = getColor();
            if (trackColor.a > 0)
            {
                // draw the track as a stretched sprite
                ss->drawSprite(static_cast<int>(IconIndex::VSlider_Rail), dsstRect, trackColor, SDL_SCALEMODE_NEAREST);

                // draw small ticks along the track
                double y_min = getY() - 2.5f;
                double y_max = getY() + getHeight() - 4;
                double step = (y_max - y_min)/10.0;
                for (double v = y_min; v <= y_max; v += step)
                {
                    ss->drawSprite(static_cast<int>(IconIndex::Slider_Tick),
                        static_cast<int>(getX()), 
                        static_cast<int>(v), 
                        trackColor, 
                        SDL_SCALEMODE_NEAREST
                    );            
                }        
            }
            // Draw the thumb
            SDL_Color thumbColor = getForegroundColor();
            // double thumbX = (getX() + ((value_ - min_) / (max_ - min_)) * (getWidth())) - 5;
            // Compute thumb Y so that min_ is at bottom and max_ at top
            double range = (max_ - min_);
            double ratio = 0.0;
            if (range > 0.0) ratio = (value_ - min_) / range;
            ratio = std::clamp(ratio, 0.0, 1.0);
            // invert so 0 => bottom, 1 => top
            double inv = 1.0 - ratio;
            double thumbY = (getY() + inv * (getHeight())) - 4;
            if (thumbColor.a > 0)
            {
                ss->drawSprite(static_cast<int>(IconIndex::Knob_Vertical),
                    static_cast<int>(getX()), 
                    static_cast<int>(thumbY), 
                    thumbColor, 
                    SDL_SCALEMODE_NEAREST
                );                   
            }            
        }

    } // END: void Slider::onRender()

    bool Slider::onUnitTest()
    {
        return SUPER::onUnitTest();
    } // END: bool Slider::onUnitTest()




    // --- Public Accessors --- //




    // --- Public Mutators --- //



    
    // --- Protected Virtual Methods --- //        

    void Slider::_onValueChanged(float oldValue, float newValue)
    {
        SUPER::_onValueChanged(oldValue, newValue);
        // // dispatch event
        // queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
        //     ev.setPayloadValue("name", getName());
        //     ev.setPayloadValue("old_value", oldValue);
        //     ev.setPayloadValue("new_value", newValue);
        // });
    } // END: void Slider::_onValueChanged(float oldValue, float newValue)




    // --- Lua Registration --- //

    void Slider::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
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

        // Augment the single shared DisplayHandle handle usertype
        sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);

        // Helper to check if a property/command is already registered
        auto absent = [&](const char* name) -> bool 
        {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        // Expose step as a property (get/set)
        if (absent("step")) {
            handle.set("step", sol::property(
                [](SDOM::DisplayHandle h) -> float {
                    auto* s = h.as<Slider>();
                    return s ? s->getStep() : 0.0f;
                },
                [](SDOM::DisplayHandle h, double v) {
                    auto* s = h.as<Slider>();
                    if (s) s->setStep(static_cast<float>(v));
                }
            ));
        }

        // Optionally, expose getStep/setStep as methods
        if (absent("getStep")) {
            handle.set("getStep", [](SDOM::DisplayHandle h) -> float {
                auto* s = h.as<Slider>();
                return s ? s->getStep() : 0.0f;
            });
        }
        if (absent("setStep")) {
            handle.set("setStep", [](SDOM::DisplayHandle h, double v) {
                auto* s = h.as<Slider>();
                if (s) s->setStep(static_cast<float>(v));
            });
        }
    } // END: void Slider::_registerLuaBindings(const std::string& typeName, sol::state_view lua)

} // END: namespace SDOM