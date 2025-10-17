// SDOM_ScrollBar.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>


#include <SDOM/SDOM_ScrollBar.hpp>

namespace SDOM
{
    // --- Protected Constructors --- //
    ScrollBar::ScrollBar(const InitStruct& init) : IRangeControl(init), 
        step_(init.step), page_size_(init.page_size), content_size_(init.content_size), min_thumb_length_(init.min_thumb_length)
    {
    } // END: ScrollBar::ScrollBar(const InitStruct& init)

    ScrollBar::ScrollBar(const sol::table& config) : IRangeControl(config, ScrollBar::InitStruct())
    {
        // Initialize members from derived InitStruct defaults so Lua can omit keys
        InitStruct init_defaults;
        step_ = init_defaults.step;
        page_size_ = init_defaults.page_size;
        content_size_ = init_defaults.content_size;
        min_thumb_length_ = init_defaults.min_thumb_length;

        // Read step from Lua if present
        if (config["step"].valid()) {
            try { step_ = static_cast<float>(config.get_or("step", static_cast<double>(step_))); }
            catch(...) { /* ignore */ }
        }
        // Read page_size from Lua if present
        if (config["page_size"].valid()) {
            try { page_size_ = static_cast<float>(config.get_or("page_size", static_cast<double>(page_size_))); }
            catch(...) { /* ignore */ }
        }
        // Read content_size from Lua if present
        if (config["content_size"].valid()) {
            try { content_size_ = static_cast<float>(config.get_or("content_size", static_cast<double>(content_size_))); }
            catch(...) { /* ignore */ }
        }        
        // Read min_thumb_length from Lua if present
        if (config["min_thumb_length"].valid()) {
            try { min_thumb_length_ = static_cast<float>(config.get_or("min_thumb_length", static_cast<double>(min_thumb_length_))); }
            catch(...) { /* ignore */ }
        }
    } // END: ScrollBar::ScrollBar(const sol::table& config)

    // --- Virtual Methods --- //

    bool ScrollBar::onInit()
    {
        bool ret = SUPER::onInit(); 

        // create the decrease button
        if (!button_decrease_)
        {
            ArrowButton::InitStruct init;
            init.name = getName() + "_arrowbutton_min";
            init.type = "ArrowButton";
            init.icon_resource = icon_resource_;
            init.x = getX();
            init.y = getY();
            init.width = 8;
            init.height = 8;   
            init.isClickable = true;
            init.tabEnabled = false;
            init.color = getColor(); 
            if (orientation_ == Orientation::Horizontal)
                init.direction = ArrowButton::ArrowDirection::Left;
            else
                init.direction = ArrowButton::ArrowDirection::Up;
            button_decrease_ = getFactory().create("ArrowButton", init);
            addChild(button_decrease_);
        }

        // create the increase button
        if (!button_increase_)
        {
            ArrowButton::InitStruct init;
            init.name = getName() + "_arrowbutton_max";
            init.type = "ArrowButton";
            init.icon_resource = icon_resource_;
            if (orientation_ == Orientation::Horizontal)
            {
                init.x = getX() + getWidth() - 8;
                init.y = getY();
                init.direction = ArrowButton::ArrowDirection::Right;                
            }
            else
            {
                init.x = getX();
                init.y = getY() + getHeight() - 8;
                init.direction = ArrowButton::ArrowDirection::Down;
            }
            init.width = 8;
            init.height = 8;   
            init.isClickable = true;
            init.tabEnabled = false;
            init.color = getColor(); 
            button_increase_ = getFactory().create("ArrowButton", init);
            addChild(button_increase_);
        }

        button_decrease_ptr_ = button_decrease_.as<ArrowButton>();
        if (!button_decrease_ptr_) {
            ERROR("ScrollBar::onInit(): 'buttonDecrease' child not found or not an ArrowButton.");
            return false;
        }
        button_increase_ptr_ = button_increase_.as<ArrowButton>();
        if (!button_increase_ptr_) {
            ERROR("ScrollBar::onInit(): 'buttonIncrease' child not found or not an ArrowButton.");
            return false;
        }

        // Wire arrow button clicks to increment/decrement the scrollbar value
        if (button_decrease_ptr_ || button_increase_ptr_)
        {
            // Common helpers to perform the change and dispatch ValueChanged
            auto doChange = [this](float newValue, float oldValue) {
                if (newValue == oldValue) return;
                setValue(newValue);
                queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& e) {
                    e.setPayloadValue("name", getName());
                    e.setPayloadValue("old_value", oldValue);
                    e.setPayloadValue("new_value", newValue);
                });
            };

            // decrease handler (left or top)
            auto decreaseHandler = [this, doChange](Event& ev) {
                if (!isEnabled()) return;
                float oldValue = getValue();
                float delta = (step_ > 0.0f) ? step_ : 1.0f;
                float newValue;
                if (orientation_ == Orientation::Horizontal) {
                    newValue = std::clamp(getValue() - delta, getMin(), getMax());
                } else {
                    // top arrow increases value
                    newValue = std::clamp(getValue() + delta, getMin(), getMax());
                }
                doChange(newValue, oldValue);
            };

            // increase handler (right or bottom)
            auto increaseHandler = [this, doChange](Event& ev) {
                if (!isEnabled()) return;
                float oldValue = getValue();
                float delta = (step_ > 0.0f) ? step_ : 1.0f;
                float newValue;
                if (orientation_ == Orientation::Horizontal) {
                    newValue = std::clamp(getValue() + delta, getMin(), getMax());
                } else {
                    // bottom arrow decreases value
                    newValue = std::clamp(getValue() - delta, getMin(), getMax());
                }
                doChange(newValue, oldValue);
            };

            if (button_decrease_ptr_) {
                button_decrease_ptr_->addEventListener(EventType::MouseClick, decreaseHandler);
                button_decrease_ptr_->addEventListener(EventType::MouseDoubleClick, decreaseHandler);
            }
            if (button_increase_ptr_) {
                button_increase_ptr_->addEventListener(EventType::MouseClick, increaseHandler);
                button_increase_ptr_->addEventListener(EventType::MouseDoubleClick, increaseHandler);
            }
        }

        return ret;
    } // END: bool ScrollBar::onInit()

    void ScrollBar::onQuit()
    {
        SUPER::onQuit();
    } // END: void ScrollBar::onQuit()

    void ScrollBar::onEvent(const Event& event)
    {
        // First, call base handler
        SUPER::onEvent(event);

        // Helper to snap to discrete steps
        auto snapToStep = [&](float v) -> float {
            if (step_ > 0.0f)
                return getMin() + std::round((v - getMin()) / step_) * step_;
            return v;
        };

        // Only handle target phase events here
        if (event.getPhase() != Event::Phase::Target) { return; }

        // Mouse click or drag
        if (event.getType() == EventType::MouseButtonDown || event.getType() == EventType::Dragging)
        {
            if (!isEnabled() || !isClickable()) return;
            float mouseX = event.getMouseX();
            float mouseY = event.getMouseY();

            if (orientation_ == Orientation::Horizontal)
            {
                // Track spans from x+11 to x+width-11
                float trackStart = static_cast<float>(getX() + 11);
                float trackWidth = static_cast<float>(getWidth() - 22);
                if (mouseX >= trackStart && mouseX <= trackStart + trackWidth)
                {
                    float rel = (mouseX - trackStart) / std::max(trackWidth, 1.0f);
                    rel = std::clamp(rel, 0.0f, 1.0f);
                    float newValue = getMin() + rel * (getMax() - getMin());
                    newValue = snapToStep(newValue);
                    float oldValue = getValue();
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
                // Track spans from y+11 to y+height-11
                float trackStart = static_cast<float>(getY() + 11);
                float trackHeight = static_cast<float>(getHeight() - 22);
                if (mouseY >= trackStart && mouseY <= trackStart + trackHeight)
                {
                    float rel = (mouseY - trackStart) / std::max(trackHeight, 1.0f);
                    rel = std::clamp(rel, 0.0f, 1.0f);
                    // Invert so top corresponds to max
                    float ratio = 1.0f - rel;
                    float newValue = getMin() + ratio * (getMax() - getMin());
                    newValue = snapToStep(newValue);
                    float oldValue = getValue();
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

        // MouseWheel event to adjust the scrollbar value
        if (event.getType() == EventType::MouseWheel)
        {
            SDL_Keymod modState = SDL_GetModState();
            float wheel_delta = event.getWheelY() * ((step_ > 0.0f) ? step_ : 1.0f);
            if (modState & SDL_KMOD_SHIFT)
                wheel_delta *= 10.0f; // larger step with SHIFT key

            float oldValue = getValue();
            float newValue = snapToStep(std::clamp(getValue() + wheel_delta, getMin(), getMax()));
            if (newValue == oldValue) return;
            setValue(newValue);
            queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
                ev.setPayloadValue("name", getName());
                ev.setPayloadValue("old_value", oldValue);
                ev.setPayloadValue("new_value", newValue);
            });
        } // END: MouseWheel event

        // Key Events to adjust the scrollbar value (if key focused)
        if (event.getType() == EventType::KeyDown)
        {
            SDL_Scancode scancode = event.getScanCode();
            float oldValue = getValue();
            float newValue = oldValue;
            if (orientation_ == Orientation::Horizontal)
            {
                // LEFT and RIGHT arrows
                if (scancode == SDL_SCANCODE_LEFT)
                    newValue = getValue() - ((step_ > 0.0f) ? step_ : 1.0f);
                else if (scancode == SDL_SCANCODE_RIGHT)
                    newValue = getValue() + ((step_ > 0.0f) ? step_ : 1.0f);
                // PgUP and PgDN keys
                if (scancode == SDL_SCANCODE_PAGEUP)
                    newValue = getValue() + ((step_ > 0.0f) ? step_*10.0f : 10.0f);
                else if (scancode == SDL_SCANCODE_PAGEDOWN)
                    newValue = getValue() - ((step_ > 0.0f) ? step_*10.0f : 10.0f);
                // HOME and END keys
                if (scancode == SDL_SCANCODE_HOME)
                    newValue = getMin();
                else if (scancode == SDL_SCANCODE_END)
                    newValue = getMax();
            }
            else // vertical
            {
                // UP should increase value (move thumb up), DOWN should decrease
                if (scancode == SDL_SCANCODE_DOWN)
                    newValue = getValue() - ((step_ > 0.0f) ? step_ : 1.0f);
                else if (scancode == SDL_SCANCODE_UP)
                    newValue = getValue() + ((step_ > 0.0f) ? step_ : 1.0f);
                // PgUP and PgDN keys: PageUp increases, PageDown decreases
                if (scancode == SDL_SCANCODE_PAGEUP)
                    newValue = getValue() + ((step_ > 0.0f) ? step_*10.0f : 10.0f);
                else if (scancode == SDL_SCANCODE_PAGEDOWN)
                    newValue = getValue() - ((step_ > 0.0f) ? step_*10.0f : 10.0f);
                // HOME and END keys
                if (scancode == SDL_SCANCODE_HOME)
                    newValue = getMin();
                else if (scancode == SDL_SCANCODE_END)
                    newValue = getMax();
            }

            newValue = snapToStep(std::clamp(newValue, getMin(), getMax()));
            if (newValue != oldValue)
            {
                setValue(newValue);
                queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
                    ev.setPayloadValue("name", getName());
                    ev.setPayloadValue("old_value", oldValue);
                    ev.setPayloadValue("new_value", newValue);
                });
            }
        } // END: Key Events to adjust the scrollbar value
    } // END: void ScrollBar::onEvent(const Event& event)

    void ScrollBar::onUpdate(float fElapsedTime)
    {
        SUPER::onUpdate(fElapsedTime);
    } // END: void ScrollBar::onUpdate(float fElapsedTime)

    void ScrollBar::onRender()
    {
        // SUPER::onRender();
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

        // Render the ScrollBar Thumb
        SDL_FRect thumbRect = { 0, 0, 0, 0 };
        float trackLength = (orientation_ == Orientation::Horizontal)
            ? static_cast<float>(getWidth() - 22)
            : static_cast<float>(getHeight() - 22);

        // 1. Thumb length (proportional to page/content), clamp to [min_thumb_length_, trackLength]
        float denom = std::max(content_size_, 1.0f);
        float thumbProportion = page_size_ / denom;
        float thumbLength = trackLength * thumbProportion;
        if (thumbLength < min_thumb_length_) thumbLength = min_thumb_length_;
        if (thumbLength > trackLength) thumbLength = trackLength;

        // 2. Usable track for thumb movement (non-negative)
        float usableTrack = trackLength - thumbLength;
        if (usableTrack < 0.0f) usableTrack = 0.0f;

        float valueRange = std::max(getMax() - getMin(), 1.0f);

        // 3. Thumb position (relative to value)
        float valueRel = (getValue() - getMin()) / valueRange;
        valueRel = std::clamp(valueRel, 0.0f, 1.0f);

        // For vertical orientation we want min at the bottom and max at the top,
        // so invert the relative position when computing the thumb Y coordinate.
        float posRel = (orientation_ == Orientation::Horizontal) ? valueRel : (1.0f - valueRel);

        int spriteIndex = 0;
        if (orientation_ == Orientation::Horizontal) {
            thumbRect.x = getX() + 11 + usableTrack * posRel;
            thumbRect.y = static_cast<float>(getY());
            thumbRect.w = thumbLength;
            thumbRect.h = static_cast<float>(getHeight());
            spriteIndex = 75; // Thumb HProgress
        } else {
            thumbRect.x = static_cast<float>(getX());
            thumbRect.y = getY() + 11 + usableTrack * posRel;
            thumbRect.w = static_cast<float>(getWidth());
            thumbRect.h = thumbLength;
            spriteIndex = 79; // Thumb VProgress
        }
        // Debug output for scrollbar computation
#ifdef SCROLLBAR_DEBUG
        fprintf(stderr, "ScrollBar: track=%.2f page=%.2f content=%.2f prop=%.4f thumb=%.2f usable=%.2f valrel=%.4f posrel=%.4f\n",
            trackLength, page_size_, content_size_, thumbProportion, thumbLength, usableTrack, valueRel, posRel);
#endif
        ss->drawSprite(spriteIndex, thumbRect, getForegroundColor(), SDL_SCALEMODE_NEAREST);


        // Render the ScrollBar Track
        if (orientation_ == Orientation::Horizontal)
        {
            // Horizontal Track
            SDL_Color trackColor = getColor();
            if (trackColor.a > 0)
            {
                // min end
                ss->drawSprite(73, // Left HProgress
                    static_cast<int>(getX() + 3), 
                    static_cast<int>(getY()), 
                    trackColor, 
                    SDL_SCALEMODE_NEAREST
                );     
                // middle
                SDL_FRect midRect = {
                    static_cast<float>(getX() + 11), 
                    static_cast<float>(getY()), 
                    static_cast<float>(getWidth() - 22), 
                    static_cast<float>(getHeight())
                };
                ss->drawSprite(74, midRect, trackColor, SDL_SCALEMODE_NEAREST);                

                // max end
                ss->drawSprite(76, // Right HProgress
                    static_cast<int>(getX() + getWidth() - 11), 
                    static_cast<int>(getY()), 
                    trackColor, 
                    SDL_SCALEMODE_NEAREST
                );     
            }
        }
        else
        {
            // Vertical Track
            SDL_Color trackColor = getColor();
            if (trackColor.a > 0)
            {
                // min end
                ss->drawSprite(77, // Top VProgress
                    static_cast<int>(getX()),
                    static_cast<int>(getY() + 3),
                    trackColor,
                    SDL_SCALEMODE_NEAREST
                );     
                // middle
                SDL_FRect midRect = {
                    static_cast<float>(getX()), 
                    static_cast<float>(getY() + 11), 
                    static_cast<float>(getWidth() ), 
                    static_cast<float>(getHeight() - 22)
                };
                ss->drawSprite(78, midRect, trackColor, SDL_SCALEMODE_NEAREST);                

                // max end
                ss->drawSprite(80, // Bottom VProgress
                    static_cast<int>(getX()), 
                    static_cast<int>(getY() + getHeight() - 11), 
                    trackColor, 
                    SDL_SCALEMODE_NEAREST
                );     
            }
        }

    } // END: void ScrollBar::onRender()

    bool ScrollBar::onUnitTest()
    {
        return SUPER::onUnitTest();
    } // END: bool ScrollBar::onUnitTest()




    // --- Public Accessors --- //




    // --- Public Mutators --- //



    
    // --- Protected Virtual Methods --- //        

    void ScrollBar::_onValueChanged(float oldValue, float newValue)
    {
        SUPER::_onValueChanged(oldValue, newValue);
        // // dispatch event
        // queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
        //     ev.setPayloadValue("name", getName());
        //     ev.setPayloadValue("old_value", oldValue);
        //     ev.setPayloadValue("new_value", newValue);
        // });
    } // END: void ScrollBar::_onValueChanged(float oldValue, float newValue)




    // --- Lua Registration --- //

    void ScrollBar::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
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
                    auto* s = h.as<ScrollBar>();
                    return s ? s->getStep() : 0.0f;
                },
                [](SDOM::DisplayHandle h, double v) {
                    auto* s = h.as<ScrollBar>();
                    if (s) s->setStep(static_cast<float>(v));
                }
            ));
        }

        // Optionally, expose getStep/setStep as methods
        if (absent("getStep")) {
            handle.set("getStep", [](SDOM::DisplayHandle h) -> float {
                auto* s = h.as<ScrollBar>();
                return s ? s->getStep() : 0.0f;
            });
        }
        if (absent("setStep")) {
            handle.set("setStep", [](SDOM::DisplayHandle h, double v) {
                auto* s = h.as<ScrollBar>();
                if (s) s->setStep(static_cast<float>(v));
            });
        }

    } // END: void ScrollBar::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM