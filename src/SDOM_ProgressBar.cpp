// SDOM_ProgressBar.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
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
        SpriteSheet* ss = getSpriteSheetPtr();
        if (!ss) { ERROR("ProgressBar::onRender(): No valid SpriteSheet for icon."); return; }

        SDL_Renderer* renderer = getRenderer();
        if (!renderer) { ERROR("ProgressBar::onRender(): renderer is null"); return; }

        // Invalidate cached texture when renderer changes or texture becomes invalid.
        if (cachedTexture_)
        {
            if (SDOM::drop_invalid_cached_texture(cachedTexture_, renderer, cached_renderer_))
                setDirty(true);
        }

        if (isDirty())
        {
            if (!rebuildRangeTexture_(getWidth(), getHeight(), getCore().getPixelFormat()))
            {
                ERROR("ProgressBar::onRender(): failed to rebuild cache texture");
                return;
            }

            if (cachedTexture_)
            {
                SDL_Texture* prev = SDL_GetRenderTarget(renderer);
                if (!SDL_SetRenderTarget(renderer, cachedTexture_))
                {
                    ERROR("ProgressBar::onRender(): unable to set target");
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

                if (orientation_ == Orientation::Horizontal)
                {
                    SDL_Color barColor = getColor();
                    SDL_Color frameColor = getForegroundColor();

                    const float insetX = 2.0f * scale_width;
                    const float leftEndX = 6.0f * scale_width;
                    const float rightEndX = 2.0f * scale_width;

                    ss->drawSprite(static_cast<int>(IconIndex::HProgress_Left),
                        static_cast<int>(std::lround(0.0f - leftEndX)),
                        0,
                        frameColor,
                        SDL_SCALEMODE_NEAREST
                    );

                    float range = (max_ - min_);
                    float ratio = 0.0f;
                    if (range > 0.0f) ratio = (value_ - min_) / range;
                    ratio = std::max(0.0f, std::min(1.0f, ratio));

                    SDL_FRect frameRect = {
                        insetX,
                        0.0f,
                        std::max(0.0f, static_cast<float>(getWidth()) - insetX * 2.0f),
                        static_cast<float>(getHeight())
                    };
                    float innerWidth = frameRect.w;
                    float fillW = innerWidth * ratio;
                    SDL_FRect barDstRect = { frameRect.x, frameRect.y, fillW, frameRect.h };
                    ss->drawSprite(static_cast<int>(IconIndex::HProgress_Thumb), barDstRect, barColor, SDL_SCALEMODE_NEAREST);
                    ss->drawSprite(static_cast<int>(IconIndex::HProgress_Rail), frameRect, frameColor, SDL_SCALEMODE_NEAREST);

                    ss->drawSprite(static_cast<int>(IconIndex::HProgress_Right),
                        static_cast<int>(std::lround(static_cast<float>(getWidth()) - rightEndX)),
                        0,
                        frameColor,
                        SDL_SCALEMODE_NEAREST
                    );
                }
                else
                {
                    SDL_Color barColor = getColor();
                    SDL_Color frameColor = getForegroundColor();

                    const float insetY = 2.0f * scale_height;
                    const float topEndY = 6.0f * scale_height;
                    const float bottomEndY = 2.0f * scale_height;

                    ss->drawSprite(77,
                        0,
                        static_cast<int>(std::lround(0.0f - topEndY)),
                        frameColor,
                        SDL_SCALEMODE_NEAREST
                    );

                    float range = (max_ - min_);
                    float ratio = 0.0f;
                    if (range > 0.0f) ratio = (value_ - min_) / range;
                    ratio = std::max(0.0f, std::min(1.0f, ratio));

                    SDL_FRect frameRect = {
                        0.0f,
                        insetY,
                        static_cast<float>(getWidth()),
                        std::max(0.0f, static_cast<float>(getHeight()) - insetY * 2.0f)
                    };
                    float innerHeight = frameRect.h;
                    float fillH = innerHeight * ratio;
                    SDL_FRect barDstRect = {
                        frameRect.x,
                        frameRect.y + (innerHeight - fillH),
                        frameRect.w,
                        fillH
                    };
                    ss->drawSprite(static_cast<int>(IconIndex::VProgress_Thumb), barDstRect, barColor, SDL_SCALEMODE_NEAREST);
                    ss->drawSprite(static_cast<int>(IconIndex::VProgress_Rail), frameRect, frameColor, SDL_SCALEMODE_NEAREST);

                    ss->drawSprite(static_cast<int>(IconIndex::VProgress_Bottom),
                        0,
                        static_cast<int>(std::lround(static_cast<float>(getHeight()) - bottomEndY)),
                        frameColor,
                        SDL_SCALEMODE_NEAREST
                    );
                }

                SDL_SetRenderTarget(renderer, prev);
                // Track which renderer created this cache
                cached_renderer_ = renderer;
            }

            setDirty(false);
        }

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
                ERROR("ProgressBar::onRender(): failed to render cache: " + std::string(SDL_GetError()));
            }
        }
    } // END: void ProgressBar::onRender()

    
    bool ProgressBar::onUnitTest(int frame)
    {
        // Run base class tests first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Register tests only once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Range sanity check: min < max
            ut.add_test(objName, "ProgressBar Range Sanity", [this](std::vector<std::string>& errors)
            {
                if (!(min_ < max_))
                    errors.push_back("ProgressBar '" + getName() + "' invalid range: min=" +
                                    std::to_string(min_) + " max=" + std::to_string(max_));
                return true; // ✅ single-frame test
            });

            // 🔹 2. Value within range
            ut.add_test(objName, "ProgressBar Value In Range", [this](std::vector<std::string>& errors)
            {
                if (value_ < min_ || value_ > max_)
                    errors.push_back("ProgressBar '" + getName() + "' value out of range: value=" +
                                    std::to_string(value_) + " (min=" +
                                    std::to_string(min_) + " max=" + std::to_string(max_) + ")");
                return true; // ✅ single-frame test
            });

            // 🔹 3. SpriteSheet dimensions valid (if assigned)
            ut.add_test(objName, "ProgressBar SpriteSheet Dimensions", [this](std::vector<std::string>& errors)
            {
                SpriteSheet* ss = getSpriteSheetPtr();
                if (ss)
                {
                    if (ss->getSpriteWidth() <= 0 || ss->getSpriteHeight() <= 0)
                    {
                        errors.push_back("ProgressBar '" + getName() +
                                        "' has invalid sprite size: w=" +
                                        std::to_string(ss->getSpriteWidth()) +
                                        " h=" + std::to_string(ss->getSpriteHeight()));
                    }
                }
                return true; // ✅ single-frame test
            });

            registered = true;
        }

        // ✅ Return false so the object remains active in multi-frame systems
        return false;
    } // END: ProgressBar::onUnitTest()






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

        // Go-by for future bindings on DisplayHandle:
        //   sol::table handle = SDOM::IDataObject::ensure_sol_table(lua, SDOM::DisplayHandle::LuaHandleName);

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




    
    void ProgressBar::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "ProgressBar");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    


} // END: namespace SDOM
