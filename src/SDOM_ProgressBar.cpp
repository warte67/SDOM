// SDOM_ProgressBar.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IconIndex.hpp>
#include <SDOM/SDOM_ProgressBar.hpp>

namespace SDOM
{
    // --- Protected Constructors --- //
    ProgressBar::ProgressBar(const InitStruct& init) : IRangeControl(init)
    {
    } // END: ProgressBar::ProgressBar(const InitStruct& init)

    ProgressBar::ProgressBar(const sol::table& config) : IRangeControl(config, ProgressBar::InitStruct())
    {
    } // END: ProgressBar::ProgressBar(const sol::table& config)

    // --- Virtual Methods --- //

    bool ProgressBar::onInit()
    {
        // Call the base class onInit first to load the SpriteSheet
        return SUPER::onInit(); 
        // additional initialization as needed
        // ...
    } // END: bool ProgressBar::onInit()

    void ProgressBar::onQuit()
    {
        SUPER::onQuit();
    } // END: void ProgressBar::onQuit()

    void ProgressBar::onEvent(const Event& event)
    {
        SUPER::onEvent(event);
    } // END: void ProgressBar::onEvent(const Event& event)

    void ProgressBar::onUpdate(float fElapsedTime)
    {
        SUPER::onUpdate(fElapsedTime);
    } // END: void ProgressBar::onUpdate(float fElapsedTime)

    void ProgressBar::onRender()
    {
        // SUPER::onRender();

        // --- Render the Slider Control --- //

        SpriteSheet* ss = getIconSpriteSheet(); 
        if (!ss)  ERROR("Slider::onRender(): No valid SpriteSheet for icon.");

        SDL_FRect dstRect = { 
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
            SDL_RenderFillRect(getRenderer(), &dstRect);
        }

        // Render Border Color
        SDL_Color borderColor = getBorderColor();
        if (borderColor.a > 0)
        {
            SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(getRenderer(), borderColor.r, borderColor.g, borderColor.b, borderColor.a);
            SDL_RenderRect(getRenderer(), &dstRect);
        }

        // draw the progress bar min end
        if (orientation_ == Orientation::Horizontal)
        {
            // Horizontal Progress Bar
            SDL_Color barColor = getColor();
            SDL_Color frameColor = getForegroundColor();
            // left end
            ss->drawSprite(static_cast<int>(IconIndex::HProgress_Left),
                static_cast<int>(getX() - 6), 
                static_cast<int>(getY()), 
                frameColor, 
                SDL_SCALEMODE_NEAREST
            );  
            // Calculate a safe normalized ratio in [0,1]. Guard against zero range.
            float range = (max_ - min_);
            float ratio = 0.0f;
            if (range > 0.0f) ratio = (value_ - min_) / range;
            ratio = std::max(0.0f, std::min(1.0f, ratio));

            // Use an inset inside the frame for the progress "meat" so borders are preserved.
            const float inset = 2.0f; // matches offsets used for frame drawing
            float innerWidth = std::max(0.0f, static_cast<float>(getWidth()) - inset*2.0f);
            float w = innerWidth * ratio;
            SDL_FRect barDstRect = {
                static_cast<float>(getX() + inset),
                static_cast<float>(getY()),
                w,
                static_cast<float>(getHeight())
            };
            // draw the middle progress bar meat
            ss->drawSprite(static_cast<int>(IconIndex::HProgress_Thumb), barDstRect, barColor, SDL_SCALEMODE_NEAREST);  
            SDL_FRect frameRect = {
                static_cast<float>(getX()+2), 
                static_cast<float>(getY()), 
                static_cast<float>(getWidth() - 4),
                static_cast<float>(getHeight())
            };   
            ss->drawSprite(static_cast<int>(IconIndex::HProgress_Rail), frameRect, frameColor, SDL_SCALEMODE_NEAREST);
            // right end
            ss->drawSprite(static_cast<int>(IconIndex::HProgress_Right), 
                static_cast<int>(getX() + getWidth() - 2), 
                static_cast<int>(getY()), 
                frameColor, 
                SDL_SCALEMODE_NEAREST
            );
        }
        else
        {
            // Vertical Progress Bar
            SDL_Color barColor = getColor();
            SDL_Color frameColor = getForegroundColor();
            // top end
            ss->drawSprite(77, // Top VProgress
                static_cast<int>(getX()), 
                static_cast<int>(getY() - 6), 
                frameColor, 
                SDL_SCALEMODE_NEAREST
            );  
            // stretch the progress bar according to value
            // Vertical: safe normalized ratio
            float range = (max_ - min_);
            float ratio = 0.0f;
            if (range > 0.0f) ratio = (value_ - min_) / range;
            ratio = std::max(0.0f, std::min(1.0f, ratio));

            const float inset = 2.0f;
            float innerHeight = std::max(0.0f, static_cast<float>(getHeight()) - inset*2.0f);
            float h = innerHeight * ratio;
            SDL_FRect barDstRect = {
                static_cast<float>(getX()),
                static_cast<float>(getY() + inset + (innerHeight - h)),
                static_cast<float>(getWidth()),
                h
            };
            // draw the middle progress bar meat
            ss->drawSprite(static_cast<int>(IconIndex::VProgress_Thumb), barDstRect, barColor, SDL_SCALEMODE_NEAREST);  
            SDL_FRect frameRect = {
                static_cast<float>(getX()), 
                static_cast<float>(getY()+2), 
                static_cast<float>(getWidth()),
                static_cast<float>(getHeight() - 4)
            };   
            ss->drawSprite(static_cast<int>(IconIndex::VProgress_Rail), frameRect, frameColor, SDL_SCALEMODE_NEAREST);
            // bottom end
            ss->drawSprite(static_cast<int>(IconIndex::VProgress_Bottom), 
                static_cast<int>(getX()), 
                static_cast<int>(getY() + getHeight() - 2), 
                frameColor, 
                SDL_SCALEMODE_NEAREST
            );
        }

    } // END: void ProgressBar::onRender()

    bool ProgressBar::onUnitTest()
    {
        return SUPER::onUnitTest();
    } // END: bool ProgressBar::onUnitTest()




    // --- Public Accessors --- //




    // --- Public Mutators --- //



    
    // --- Protected Virtual Methods --- //        

    void ProgressBar::_onValueChanged(float oldValue, float newValue)
    {
        SUPER::_onValueChanged(oldValue, newValue);
        // // dispatch event
        // queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
        //     ev.setPayloadValue("name", getName());
        //     ev.setPayloadValue("old_value", oldValue);
        //     ev.setPayloadValue("new_value", newValue);
        // });
    } // END: void ProgressBar::_onValueChanged(float oldValue, float newValue)




    // --- Lua Registration --- //

    void ProgressBar::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
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

    } // END: void ProgressBar::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM