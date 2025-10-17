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
            init.tabEnabled = true;
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
            init.tabEnabled = true;
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

        return ret;
    } // END: bool ScrollBar::onInit()

    void ScrollBar::onQuit()
    {
        SUPER::onQuit();
    } // END: void ScrollBar::onQuit()

    void ScrollBar::onEvent(const Event& event)
    {
        SUPER::onEvent(event);
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

        // 1. Thumb length (proportional, clamped)
        float thumbProportion = page_size_ / std::max(content_size_, 1.0f);
        float thumbLength = std::max(trackLength * thumbProportion, min_thumb_length_);

        // 2. Usable track for thumb movement
        float usableTrack = trackLength - thumbLength;
        float valueRange = std::max(getMax() - getMin(), 1.0f);

        // 3. Thumb position (relative to value)
        float valueRel = (getValue() - getMin()) / valueRange;
        valueRel = std::clamp(valueRel, 0.0f, 1.0f);

        int spriteIndex = 0;
        if (orientation_ == Orientation::Horizontal) {
            thumbRect.x = getX() + 11 + usableTrack * valueRel;
            thumbRect.y = static_cast<float>(getY());
            thumbRect.w = thumbLength;
            thumbRect.h = static_cast<float>(getHeight());
            spriteIndex = 75; // Thumb HProgress
        } else {
            thumbRect.x = static_cast<float>(getX());
            thumbRect.y = getY() + 11 + usableTrack * valueRel;
            thumbRect.w = static_cast<float>(getWidth());
            thumbRect.h = thumbLength;
            spriteIndex = 79; // Thumb VProgress
        }
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