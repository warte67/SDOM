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

        SpriteSheet* ss = getIconSpriteSheet();
        if (!ss) ERROR("Slider::onRender(): No valid SpriteSheet for icon.");

        float ss_width = ss->getSpriteWidth();
        float ss_height = ss->getSpriteHeight();
        float scale_width = ss_width / 8.0f;
        float scale_height = ss_height / 8.0f;

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

        if (orientation_ == Orientation::Horizontal)
        {
            SDL_Color barColor = getColor();
            SDL_Color frameColor = getForegroundColor();

            // scaled insets (use same scale for frame & fill)
            const float insetX = 2.0f * scale_width;
            const float leftEndX = 6.0f * scale_width;   // where left cap is drawn relative to getX()
            const float rightEndX = 2.0f * scale_width;  // where right cap is drawn relative to right edge

            // draw left cap (pixel-snapped)
            ss->drawSprite(static_cast<int>(IconIndex::HProgress_Left),
                static_cast<int>(std::lround(getX() - leftEndX)),
                static_cast<int>(std::lround(getY())),
                frameColor,
                SDL_SCALEMODE_NEAREST
            );

            // safe normalized ratio in [0,1]
            float range = (max_ - min_);
            float ratio = 0.0f;
            if (range > 0.0f) ratio = (value_ - min_) / range;
            ratio = std::max(0.0f, std::min(1.0f, ratio));

            // rail/frame rect uses the same scaled insets
            SDL_FRect frameRect = {
                static_cast<float>(getX() + insetX),
                static_cast<float>(getY()),
                static_cast<float>(std::max(0.0f, static_cast<float>(getWidth()) - insetX * 2.0f)),
                static_cast<float>(getHeight())
            };
            // progress "meat" inside the rail/frame
            float innerWidth = frameRect.w; // already accounts for insetX*2
            float fillW = innerWidth * ratio;
            SDL_FRect barDstRect = {
                static_cast<float>(frameRect.x),
                static_cast<float>(frameRect.y),
                fillW,
                static_cast<float>(frameRect.h)
            };
            ss->drawSprite(static_cast<int>(IconIndex::HProgress_Thumb), barDstRect, barColor, SDL_SCALEMODE_NEAREST);
            ss->drawSprite(static_cast<int>(IconIndex::HProgress_Rail), frameRect, frameColor, SDL_SCALEMODE_NEAREST);


            // draw right cap (pixel-snapped)
            ss->drawSprite(static_cast<int>(IconIndex::HProgress_Right),
                static_cast<int>(std::lround(getX() + getWidth() - rightEndX)),
                static_cast<int>(std::lround(getY())),
                frameColor,
                SDL_SCALEMODE_NEAREST
            );
        }
        else
        {
            SDL_Color barColor = getColor();
            SDL_Color frameColor = getForegroundColor();

            // scaled insets (vertical)
            const float insetY = 2.0f * scale_height;
            const float topEndY = 6.0f * scale_height;
            const float bottomEndY = 2.0f * scale_height;

            // top cap (uses literal index for top if necessary)
            ss->drawSprite(77, // Top VProgress
                static_cast<int>(std::lround(getX())),
                static_cast<int>(std::lround(getY() - topEndY)),
                frameColor,
                SDL_SCALEMODE_NEAREST
            );

            // safe normalized ratio
            float range = (max_ - min_);
            float ratio = 0.0f;
            if (range > 0.0f) ratio = (value_ - min_) / range;
            ratio = std::max(0.0f, std::min(1.0f, ratio));

            // rail/frame rect uses scaled insets
            SDL_FRect frameRect = {
                static_cast<float>(getX()),
                static_cast<float>(getY() + insetY),
                static_cast<float>(getWidth()),
                static_cast<float>(std::max(0.0f, static_cast<float>(getHeight()) - insetY * 2.0f))
            };
            // progress "meat" inside the rail/frame (grows from bottom)
            float innerHeight = frameRect.h;
            float fillH = innerHeight * ratio;
            SDL_FRect barDstRect = {
                static_cast<float>(frameRect.x),
                static_cast<float>(frameRect.y + (innerHeight - fillH)),
                static_cast<float>(frameRect.w),
                fillH
            };
            ss->drawSprite(static_cast<int>(IconIndex::VProgress_Thumb), barDstRect, barColor, SDL_SCALEMODE_NEAREST);
            ss->drawSprite(static_cast<int>(IconIndex::VProgress_Rail), frameRect, frameColor, SDL_SCALEMODE_NEAREST);

            // bottom cap
            ss->drawSprite(static_cast<int>(IconIndex::VProgress_Bottom),
                static_cast<int>(std::lround(getX())),
                static_cast<int>(std::lround(getY() + getHeight() - bottomEndY)),
                frameColor,
                SDL_SCALEMODE_NEAREST
            );
        }
    } // END: void ProgressBar::onRender()

    bool ProgressBar::onUnitTest()
    {
        // run base checks first
        if (!SUPER::onUnitTest()) return false;

        bool ok = true;

        // range sanity: min < max
        if (!(min_ < max_)) {
            DEBUG_LOG("[UnitTest] ProgressBar '" << getName() << "' invalid range: min=" << min_ << " max=" << max_);
            ok = false;
        }

        // value must be within [min, max]
        if (value_ < min_ || value_ > max_) {
            DEBUG_LOG("[UnitTest] ProgressBar '" << getName() << "' value out of range: value=" << value_
                      << " (min=" << min_ << " max=" << max_ << ")");
            ok = false;
        }

        // if an icon spritesheet is assigned, its sprite dimensions should be positive
        SpriteSheet* ss = getIconSpriteSheet();
        if (ss) {
            if (ss->getSpriteWidth() <= 0 || ss->getSpriteHeight() <= 0) {
                DEBUG_LOG("[UnitTest] ProgressBar '" << getName() << "' has invalid sprite size: w="
                          << ss->getSpriteWidth() << " h=" << ss->getSpriteHeight() << ")");
                ok = false;
            }
        }

        return ok;
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