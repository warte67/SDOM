// SDOM_Slider.cpp
#include <SDOM/SDOM.hpp>


#include <SDOM/SDOM_Slider.hpp>

namespace SDOM
{
    // --- Protected Constructors --- //
    Slider::Slider(const InitStruct& init) : IRangeControl(init)
    {
    } // END: Slider::Slider(const InitStruct& init)

    Slider::Slider(const sol::table& config) : IRangeControl(config, InitStruct())
    {
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
        SUPER::onEvent(event);

        if (event.getPhase() != Event::Phase::Target) { return; } // Only handle target phase events
        if (event.getType() == EventType::MouseButtonDown || event.getType() == EventType::Dragging)
        {
            if (!isEnabled() || !isClickable()) return;
            float mouseX = event.getMouseX();
            float mouseY = event.getMouseY();
            if (orientation_ == Orientation::Horizontal) 
            {
                if (mouseX >= getX() && mouseX <= getX() + getWidth() ) 
                {
                    float newValue = 0.0f;
                    // Match the thumb rendering formula
                    float trackStart = getX();
                    float trackWidth = getWidth();
                    float rel = (mouseX - trackStart) / trackWidth;
                    rel = std::clamp(rel, 0.0f, 1.0f);
                    newValue = min_ + rel * (max_ - min_);
                    // newValue = std::clamp(newValue, min_, max_);
                    float oldValue = value_;
                    if (newValue == oldValue) return;
                    setValue(newValue);
                    // dispatch event
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
                    float newValue = 0.0f;
                    // Match the thumb rendering formula
                    float trackStart = getY();
                    float trackHeight = getHeight();
                    float rel = (mouseY - trackStart) / trackHeight;
                    rel = std::clamp(rel, 0.0f, 1.0f);
                    newValue = min_ + rel * (max_ - min_);
                    // newValue = std::clamp(newValue, min_, max_);
                    float oldValue = value_;
                    if (newValue == oldValue) return;
                    setValue(newValue);
                    // dispatch event
                    queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
                        ev.setPayloadValue("name", getName());
                        ev.setPayloadValue("old_value", oldValue);
                        ev.setPayloadValue("new_value", newValue);
                    });
                }                
            }
        }            
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
                ss->drawSprite(66, dsstRect, trackColor, SDL_SCALEMODE_NEAREST);                

                // draw small ticks along the track
                double x_min = getX() - 3;
                double x_max = getX() + getWidth() - 4;
                double step = (x_max - x_min)/10.0;
                for (double h = x_min; h <= x_max; h += step)
                {
                    ss->drawSprite(65, // small tick mark
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
                ss->drawSprite(63, // horizontal thumb
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
                ss->drawSprite(67, dsstRect, trackColor, SDL_SCALEMODE_NEAREST);                

                // draw small ticks along the track
                double y_min = getY() - 2.5f;
                double y_max = getY() + getHeight() - 4;
                double step = (y_max - y_min)/10.0;
                for (double v = y_min; v <= y_max; v += step)
                {
                    ss->drawSprite(65, // small tick mark
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
            double thumbY = (getY() + ((value_ - min_) / (max_ - min_)) * (getHeight())) - 4;
            if (thumbColor.a > 0)
            {
                ss->drawSprite(64, // horizontal thumb
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

    } // END: void Slider::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM