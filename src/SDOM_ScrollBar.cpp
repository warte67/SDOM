// SDOM_ScrollBar.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
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
            // determine icon tile size from this ScrollBar's spritesheet if available
            SpriteSheet* ss_local = getSpriteSheetPtr();
            int iconW = 8;
            int iconH = 8;
            if (ss_local) { iconW = ss_local->getSpriteWidth(); iconH = ss_local->getSpriteHeight(); }
            init.x = getX();
            init.y = getY();
            init.width = iconW;
            init.height = iconH;
            // ensure the ArrowButton uses the same icon tile size
            init.icon_width = iconW;
            init.icon_height = iconH;
            init.isClickable = true;
            init.tabEnabled = false;
            init.color = getColor(); 
            if (orientation_ == Orientation::Horizontal)
                init.direction = ArrowButton::ArrowDirection::Left;
            else
                init.direction = ArrowButton::ArrowDirection::Up;
            button_decrease_ = getFactory().createDisplayObject("ArrowButton", init);
            addChild(button_decrease_);
        }

        // create the increase button
        if (!button_increase_)
        {
            ArrowButton::InitStruct init;
            init.name = getName() + "_arrowbutton_max";
            init.type = "ArrowButton";
            init.icon_resource = icon_resource_;
            // determine icon tile size from this ScrollBar's spritesheet if available
            SpriteSheet* ss_local = getSpriteSheetPtr();
            int iconW = 8;
            int iconH = 8;
            if (ss_local) { iconW = ss_local->getSpriteWidth(); iconH = ss_local->getSpriteHeight(); }
            if (orientation_ == Orientation::Horizontal)
            {
                init.x = getX() + getWidth() - iconW;
                init.y = getY();
                init.direction = ArrowButton::ArrowDirection::Right;                
            }
            else
            {
                init.x = getX();
                init.y = getY() + getHeight() - iconH;
                init.direction = ArrowButton::ArrowDirection::Down;
            }
            init.width = iconW;
            init.height = iconH;   
            // ensure the ArrowButton uses the same icon tile size
            init.icon_width = iconW;
            init.icon_height = iconH;
            init.isClickable = true;
            init.tabEnabled = false;
            init.color = getColor(); 
            button_increase_ = getFactory().createDisplayObject("ArrowButton", init);
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
                // Compute scaled track and thumb metrics the same way onRender does so
                // mouse mapping aligns with the visible thumb movement.
                float scaleW = 1.0f;
                SpriteSheet* ss_local = getSpriteSheetPtr();
                if (ss_local) scaleW = static_cast<float>(ss_local->getSpriteWidth()) / 8.0f;
                float trackStart = static_cast<float>(getX() + 11.0f * scaleW);
                float trackWidth = static_cast<float>(getWidth() - 22.0f * scaleW);

                // compute thumb length proportional to page/content (same as onRender)
                float denom = std::max(content_size_, 1.0f);
                float thumbProportion = page_size_ / denom;
                float trackLength = trackWidth; // same semantic as onRender's trackLength
                float thumbLength = trackLength * thumbProportion;
                if (thumbLength < min_thumb_length_) thumbLength = min_thumb_length_;
                if (thumbLength > trackLength) thumbLength = trackLength;

                float usableTrack = trackLength - thumbLength;
                if (usableTrack < 0.0f) usableTrack = 0.0f;

                if (mouseX >= trackStart && mouseX <= trackStart + trackWidth)
                {
                    // Center the thumb under the cursor by offsetting the mouse
                    // coordinate by half the thumb length before mapping into the
                    // usable track. This keeps the cursor visually centered in the
                    // thumb while dragging.
                    float mousePosForThumb = mouseX - (thumbLength * 0.5f);
                    float rel = (mousePosForThumb - trackStart) / std::max(usableTrack, 1.0f);
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
                // Track spans from y+11*scale to y+height-11*scale. Compute scale the
                // same way as onRender: derive it from the icon spritesheet tile size.
                float scaleH = 1.0f;
                SpriteSheet* ss_local = getSpriteSheetPtr();
                if (ss_local) scaleH = static_cast<float>(ss_local->getSpriteHeight()) / 8.0f;
                float trackStart = static_cast<float>(getY() + 11.0f * scaleH);
                float trackHeight = static_cast<float>(getHeight() - 22.0f * scaleH);
                if (mouseY >= trackStart && mouseY <= trackStart + trackHeight)
                {
                    // Vertical mapping: compute thumbLength/usableTrack same as onRender
                    float denom = std::max(content_size_, 1.0f);
                    float thumbProportion = page_size_ / denom;
                    float trackLength = trackHeight;
                    float thumbLength = trackLength * thumbProportion;
                    if (thumbLength < min_thumb_length_) thumbLength = min_thumb_length_;
                    if (thumbLength > trackLength) thumbLength = trackLength;
                    float usableTrack = trackLength - thumbLength;
                    if (usableTrack < 0.0f) usableTrack = 0.0f;

                    // Center the thumb under the cursor vertically by offsetting
                    // the mouse coordinate by half the thumb length.
                    float mousePosForThumb = mouseY - (thumbLength * 0.5f);
                    float rel = (mousePosForThumb - trackStart) / std::max(usableTrack, 1.0f);
                    rel = std::clamp(rel, 0.0f, 1.0f);
                    // try {
                    //     DEBUG_LOG(std::string("[ScrollBarDiag] '") + getName() + " V mouseY=" + std::to_string(mouseY)
                    //               + " mousePosForThumb=" + std::to_string(mousePosForThumb)
                    //               + " trackStart=" + std::to_string(trackStart)
                    //               + " usableTrack=" + std::to_string(usableTrack)
                    //               + " thumbLength=" + std::to_string(thumbLength)
                    //               + " rel=" + std::to_string(rel));
                    // } catch(...) {}
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
        SpriteSheet* ss = getSpriteSheetPtr(); 
        if (!ss)  { ERROR("ScrollBar::onRender(): No valid SpriteSheet for icon."); return; }

        SDL_Renderer* renderer = getRenderer();
        if (!renderer) { ERROR("ScrollBar::onRender(): renderer is null"); return; }

        // Invalidate cache when renderer changes or texture becomes invalid
        // Skip per-frame validity query; rely on onWindowResize() and the
        // renderer-change guard when drawing to handle invalidation.

        if (isDirty())
        {
            if (!rebuildRangeTexture_(getWidth(), getHeight(), getCore().getPixelFormat()))
            {
                ERROR("ScrollBar::onRender(): failed to rebuild cache texture");
                return;
            }

            if (cachedTexture_)
            {
                SDL_Texture* prev = SDL_GetRenderTarget(renderer);
                if (!SDL_SetRenderTarget(renderer, cachedTexture_))
                {
                    ERROR("ScrollBar::onRender(): unable to set target");
                    return;
                }

                // Clear to transparent
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                SDL_RenderClear(renderer);

                float ss_width = ss->getSpriteWidth();
                float ss_height = ss->getSpriteHeight();
                float scale_width = ss_width / 8.0f;
                float scale_height = ss_height / 8.0f;

                SDL_FRect localRect = {
                    0.0f, 0.0f,
                    static_cast<float>(getWidth()),
                    static_cast<float>(getHeight())
                };

                // Background
                SDL_Color bgColor = getBackgroundColor();
                if (bgColor.a > 0)
                {
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
                    SDL_RenderFillRect(renderer, &localRect);
                }
                // Border
                SDL_Color borderColor = getBorderColor();
                if (borderColor.a > 0)
                {
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
                    SDL_RenderRect(renderer, &localRect);
                }

                // Compute thumb geometry
                SDL_FRect thumbRect = { 0, 0, 0, 0 };
                float trackLength = (orientation_ == Orientation::Horizontal)
                    ? static_cast<float>(getWidth() - 22 * scale_width)
                    : static_cast<float>(getHeight() - 22 * scale_height);

                float denom = std::max(content_size_, 1.0f);
                float thumbProportion = page_size_ / denom;
                float thumbLength = trackLength * thumbProportion;
                if (thumbLength < min_thumb_length_) thumbLength = min_thumb_length_;
                if (thumbLength > trackLength) thumbLength = trackLength;

                float usableTrack = trackLength - thumbLength;
                if (usableTrack < 0.0f) usableTrack = 0.0f;

                float valueRange = std::max(getMax() - getMin(), 1.0f);
                float valueRel = (getValue() - getMin()) / valueRange;
                valueRel = std::clamp(valueRel, 0.0f, 1.0f);
                float posRel = (orientation_ == Orientation::Horizontal) ? valueRel : (1.0f - valueRel);

                int spriteIndex = 0;
                if (orientation_ == Orientation::Horizontal) {
                    thumbRect.x = (11 * scale_width) + usableTrack * posRel;
                    thumbRect.y = 0.0f;
                    thumbRect.w = thumbLength;
                    thumbRect.h = static_cast<float>(getHeight());
                    spriteIndex = static_cast<int>(IconIndex::HProgress_Thumb);
                } else {
                    thumbRect.x = 0.0f;
                    thumbRect.y = (11 * scale_height) + usableTrack * posRel;
                    thumbRect.w = static_cast<float>(getWidth());
                    thumbRect.h = thumbLength;
                    spriteIndex = static_cast<int>(IconIndex::VProgress_Thumb);
                }
                ss->drawSprite(spriteIndex, thumbRect, getForegroundColor(), SDL_SCALEMODE_NEAREST);

                // Track
                SDL_Color trackColor = getColor();
                if (trackColor.a > 0)
                {
                    if (orientation_ == Orientation::Horizontal)
                    {
                        ss->drawSprite(static_cast<int>(IconIndex::HProgress_Left),
                            static_cast<int>(std::lround(3 * scale_width)),
                            0,
                            trackColor,
                            SDL_SCALEMODE_NEAREST
                        );
                        SDL_FRect midRect = {
                            static_cast<float>(11 * scale_width),
                            0.0f,
                            static_cast<float>(getWidth() - 22 * scale_width),
                            static_cast<float>(getHeight())
                        };
                        ss->drawSprite(static_cast<int>(IconIndex::HProgress_Rail), midRect, trackColor, SDL_SCALEMODE_NEAREST);
                        ss->drawSprite(static_cast<int>(IconIndex::HProgress_Right),
                            static_cast<int>(std::lround(static_cast<float>(getWidth()) - 11 * scale_width)),
                            0,
                            trackColor,
                            SDL_SCALEMODE_NEAREST
                        );
                    }
                    else
                    {
                        ss->drawSprite(static_cast<int>(IconIndex::VProgress_Top),
                            0,
                            static_cast<int>(std::lround(3 * scale_height)),
                            trackColor,
                            SDL_SCALEMODE_NEAREST
                        );
                        SDL_FRect midRect = {
                            0.0f,
                            static_cast<float>(11 * scale_height),
                            static_cast<float>(getWidth()),
                            static_cast<float>(getHeight() - 22 * scale_height)
                        };
                        ss->drawSprite(static_cast<int>(IconIndex::VProgress_Rail), midRect, trackColor, SDL_SCALEMODE_NEAREST);
                        ss->drawSprite(static_cast<int>(IconIndex::VProgress_Bottom),
                            0,
                            static_cast<int>(std::lround(static_cast<float>(getHeight()) - 11 * scale_height)),
                            trackColor,
                            SDL_SCALEMODE_NEAREST
                        );
                    }
                }

                SDL_SetRenderTarget(renderer, prev);
                // Track which renderer created this cache
                cached_renderer_ = renderer;
            }

            setDirty(false);
        }

        // Present the cached texture
        if (cachedTexture_)
        {
            SDL_FRect dst = {
                static_cast<float>(getX()),
                static_cast<float>(getY()),
                static_cast<float>(getWidth()),
                static_cast<float>(getHeight())
            };
            if (!SDL_RenderTexture(renderer, cachedTexture_, nullptr, &dst))
            {
                ERROR("ScrollBar::onRender(): failed to render cache: " + std::string(SDL_GetError()));
            }
        }
    } // END: void ScrollBar::onRender()
        

    bool ScrollBar::onUnitTest(int frame)
    {
        // Run base class tests first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Register all tests only once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Range sanity: min < max
            ut.add_test(objName, "ScrollBar Range Sanity", [this](std::vector<std::string>& errors)
            {
                if (!(getMin() < getMax()))
                    errors.push_back("ScrollBar '" + getName() + "' invalid range: min=" +
                                    std::to_string(getMin()) + " max=" + std::to_string(getMax()));
                return true;
            });

            // 🔹 2. Value within range
            ut.add_test(objName, "ScrollBar Value In Range", [this](std::vector<std::string>& errors)
            {
                if (getValue() < getMin() || getValue() > getMax())
                    errors.push_back("ScrollBar '" + getName() + "' value out of range: value=" +
                                    std::to_string(getValue()) + " (min=" +
                                    std::to_string(getMin()) + " max=" + std::to_string(getMax()) + ")");
                return true;
            });

            // 🔹 3. Page/content/thumb invariants
            ut.add_test(objName, "ScrollBar Page and Content Invariants", [this](std::vector<std::string>& errors)
            {
                if (page_size_ < 0.0f)
                    errors.push_back("ScrollBar '" + getName() + "' has negative page_size: " + std::to_string(page_size_));
                if (content_size_ <= 0.0f)
                    errors.push_back("ScrollBar '" + getName() + "' has non-positive content_size: " + std::to_string(content_size_));
                if (min_thumb_length_ < 0.0f)
                    errors.push_back("ScrollBar '" + getName() + "' has negative min_thumb_length: " + std::to_string(min_thumb_length_));
                return true;
            });

            // 🔹 4. Computed thumb length validity
            ut.add_test(objName, "ScrollBar Thumb Length Computation", [this](std::vector<std::string>& errors)
            {
                float trackLength = (orientation_ == Orientation::Horizontal)
                    ? static_cast<float>(getWidth() - 22)
                    : static_cast<float>(getHeight() - 22);
                float denom = std::max(content_size_, 1.0f);
                float thumbProportion = page_size_ / denom;
                float thumbLength = trackLength * thumbProportion;
                if (thumbLength < min_thumb_length_) thumbLength = min_thumb_length_;
                if (thumbLength > trackLength) thumbLength = trackLength;

                if (!(thumbLength >= 0.0f && thumbLength <= trackLength))
                    errors.push_back("ScrollBar '" + getName() + "' computed thumb length invalid: thumb=" +
                                    std::to_string(thumbLength) + " track=" + std::to_string(trackLength));
                return true;
            });

            // 🔹 5. SpriteSheet validity
            ut.add_test(objName, "ScrollBar SpriteSheet Dimensions", [this](std::vector<std::string>& errors)
            {
                SpriteSheet* ss = getSpriteSheetPtr();
                if (ss)
                {
                    if (ss->getSpriteWidth() <= 0 || ss->getSpriteHeight() <= 0)
                    {
                        errors.push_back("ScrollBar '" + getName() + "' has invalid sprite size: w=" +
                                        std::to_string(ss->getSpriteWidth()) + " h=" +
                                        std::to_string(ss->getSpriteHeight()));
                    }
                }
                return true;
            });

            registered = true;
        }

        // ✅ Return false so the test remains active in the UnitTest cycle
        return false;
    } // END: ScrollBar::onUnitTest()






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

        // Go-by for future bindings on DisplayHandle:
        //   sol::table handle = SDOM::IDataObject::ensure_sol_table(lua, SDOM::DisplayHandle::LuaHandleName);


    } // END: void ScrollBar::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    

    
    void ScrollBar::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "ScrollBar");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    

} // END: namespace SDOM
