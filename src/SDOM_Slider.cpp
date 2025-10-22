// SDOM_Slider.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IconIndex.hpp>
#include <SDOM/SDOM_Slider.hpp>

// Per-side padding fraction of the knob sprite that is visually "transparent".
// Example: 0.25 means a 16px tile uses the middle 8px for the visible thumb,
// so we extend travel by 4px on each side (scaled with the sprite tile size).
namespace { constexpr float kKnobPaddingFraction = 0.45f; }

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
                    // Use inner rail and anchor mapping so the knob stays fully within the bounds
                    SpriteSheet* ss = getSpriteSheetPtr();
                    float tileW = ss ? ss->getSpriteWidth() : 8.0f;
                    float scale_w = ss ? (tileW / 8.0f) : 1.0f;

                    const float insetX = 2.0f * scale_w;
                    float railX = getX() + insetX;
                    float railW = std::max(0.0f, getWidth() - insetX * 2.0f);

                    // Extend travel by per-side padding so the visible inner core can reach the caps
                    float padX = tileW * kKnobPaddingFraction;
                    float leftMin = railX - padX;
                    float leftMax = railX + railW - tileW + padX;
                    float usable = std::max(0.0f, leftMax - leftMin);
                    // center the cursor in the knob while dragging, but clamp the knob inside the extended rail range
                    float desiredAnchor = mouseX - tileW * 0.5f; // knob left edge desired
                    float clampedAnchor = std::clamp(desiredAnchor, leftMin, leftMax);
                    float rel = (usable > 0.0f) ? ((clampedAnchor - leftMin) / usable) : 0.0f;
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
                    // Use inner rail and anchor mapping so the knob stays fully within the bounds
                    SpriteSheet* ss = getSpriteSheetPtr();
                    float tileH = ss ? ss->getSpriteHeight() : 8.0f;
                    float scale_h = ss ? (tileH / 8.0f) : 1.0f;

                    const float insetY = 2.0f * scale_h;
                    float railY = getY() + insetY;
                    float railH = std::max(0.0f, getHeight() - insetY * 2.0f);

                    // Extend travel by per-side padding so the visible inner core can reach the caps
                    float padY = tileH * kKnobPaddingFraction;
                    float topMin = railY - padY;
                    float topMax = railY + railH - tileH + padY;
                    float usable = std::max(0.0f, topMax - topMin);
                    // center the cursor in the knob while dragging, but clamp the knob inside the extended rail range
                    float desiredAnchor = mouseY - tileH * 0.5f; // knob top edge desired
                    float clampedAnchor = std::clamp(desiredAnchor, topMin, topMax);

                    // For vertical: bottom is min (ratio 0), top is max (ratio 1)
                    float relTopToBottom = (usable > 0.0f) ? ((clampedAnchor - topMin) / usable) : 0.0f; // 0 at top, 1 at bottom
                    relTopToBottom = std::clamp(relTopToBottom, 0.0f, 1.0f);
                    float ratio = 1.0f - relTopToBottom; // convert to 0=>bottom, 1=>top ratio

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

        SpriteSheet* ss = getSpriteSheetPtr(); 
        if (!ss)  ERROR("Slider::onRender(): No valid SpriteSheet for icon.");

        float ss_width = ss->getSpriteWidth();
        float ss_height = ss->getSpriteHeight();
        float scale_width = ss_width / 8.0f;
        float scale_height = ss_height / 8.0f;
        
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
                double x_min = getX() - 3 * scale_width;
                double x_max = getX() + getWidth() - 4 * scale_height;
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
            // Draw the thumb so its LEFT edge moves over the usable rail span
            SDL_Color thumbColor = getForegroundColor();
            float tileW = ss->getSpriteWidth();
            const float insetX = 2.0f * scale_width;
            float railX = getX() + insetX;
            float railW = std::max(0.0f, getWidth() - insetX * 2.0f);

            // Extend travel by per-side padding so the visible inner core can reach the caps
            float padX = tileW * kKnobPaddingFraction;
            double leftMin = railX - padX;
            double leftMax = railX + railW - tileW + padX;
            double usable = std::max(0.0, leftMax - leftMin);

            double range = (max_ - min_);
            double ratio = (range > 0.0) ? ((value_ - min_) / range) : 0.0;
            ratio = std::clamp(ratio, 0.0, 1.0);
            double thumbLeft = leftMin + ratio * usable;
            if (thumbColor.a > 0)
            {
                ss->drawSprite(static_cast<int>(IconIndex::Knob_Horizontal), 
                    static_cast<int>(std::lround(thumbLeft)), 
                    static_cast<int>(std::lround(getY())), 
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
                double y_min = getY() - 2.5f * scale_height;
                double y_max = getY() + getHeight() - 4 * scale_height;
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
            // Draw the thumb so its TOP edge moves over the usable rail span
            SDL_Color thumbColor = getForegroundColor();
            float tileH = ss->getSpriteHeight();
            const float insetY = 2.0f * scale_height;
            float railY = getY() + insetY;
            float railH = std::max(0.0f, getHeight() - insetY * 2.0f);

            // Extend travel by per-side padding so the visible inner core can reach the caps
            float padY = tileH * kKnobPaddingFraction;
            double topMin = railY - padY;
            double topMax = railY + railH - tileH + padY;
            double usable = std::max(0.0, topMax - topMin);

            double range = (max_ - min_);
            double ratio = (range > 0.0) ? ((value_ - min_) / range) : 0.0;
            ratio = std::clamp(ratio, 0.0, 1.0);
            // For vertical: 0 at bottom, 1 at top → anchor goes from bottom to top
            double thumbTop = topMin + (1.0 - ratio) * usable;
            if (thumbColor.a > 0)
            {
                ss->drawSprite(static_cast<int>(IconIndex::Knob_Vertical),
                    static_cast<int>(std::lround(getX())), 
                    static_cast<int>(std::lround(thumbTop)), 
                    thumbColor, 
                    SDL_SCALEMODE_NEAREST
                );                   
            }            
        }

    } // END: void Slider::onRender()

    bool Slider::onUnitTest()
    {
        // run base checks first
        if (!SUPER::onUnitTest()) return false;

        bool ok = true;

        // step must be non-negative
        if (step_ < 0.0f) {
            DEBUG_LOG("[UnitTest] Slider '" << getName() << "' has negative step: " << step_);
            ok = false;
        }

        // range sanity: min < max
        if (!(min_ < max_)) {
            DEBUG_LOG("[UnitTest] Slider '" << getName() << "' invalid range: min=" << min_ << " max=" << max_);
            ok = false;
        }

        // value must be within [min, max]
        if (value_ < min_ || value_ > max_) {
            DEBUG_LOG("[UnitTest] Slider '" << getName() << "' value out of range: value=" << value_
                      << " (min=" << min_ << " max=" << max_ << ")");
            ok = false;
        }

        // if an icon spritesheet is assigned, its sprite dimensions should be positive
        SpriteSheet* ss = getSpriteSheetPtr();
        if (ss) {
            if (ss->getSpriteWidth() <= 0 || ss->getSpriteHeight() <= 0) {
                DEBUG_LOG("[UnitTest] Slider '" << getName() << "' has invalid sprite size: w=" 
                          << ss->getSpriteWidth() << " h=" << ss->getSpriteHeight() << ")");
                ok = false;
            }
        }

        return ok;
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

        // // Augment the single shared DisplayHandle handle usertype
        // sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);


    } // END: void Slider::_registerLuaBindings(const std::string& typeName, sol::state_view lua)

} // END: namespace SDOM