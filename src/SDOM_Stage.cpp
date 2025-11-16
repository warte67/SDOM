// SDOM_Stage.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_Factory.hpp>

namespace SDOM
{

    Stage::Stage(const InitStruct& init) : IDisplayObject(init)
    {
        if (init.type != TypeName) {
            ERROR("Error: Stage constructed with incorrect type: " + init.type);
        }        
        setClickable(true);
        setTabEnabled(false);
    }

    Stage::Stage(const sol::table& config) : Stage(config, InitStruct())
    {
    }

    Stage::Stage(const sol::table& config, const InitStruct& defaults) : IDisplayObject(config, defaults)
    {
        std::string type = config["type"].valid() ? config["type"].get<std::string>() : TypeName;
        if (type != TypeName) {
            ERROR("Error: Stage constructed with incorrect type: " + type);
        }
        setClickable(true);
        setTabEnabled(false);
    }


    bool Stage::onInit()
    {
        // Stage initialization logic
        Core& core = getCore();
        float w = core.getWindowWidth()  / (core.getPixelWidth()  > 0 ? core.getPixelWidth()  : 1);
        float h = core.getWindowHeight() / (core.getPixelHeight() > 0 ? core.getPixelHeight() : 1);
        setWidth(static_cast<int>(w));
        setHeight(static_cast<int>(h));

        // Ensure Stage has sensible default logical size if none provided.
        if (getWidth() <= 0 || getHeight() <= 0) {
            Core& core = Core::getInstance();
            int logicalW = 0, logicalH = 0;
            SDL_Texture* tex = core.getTexture();
            if (tex) {
                float tw = 0.0f, th = 0.0f;
                if (SDL_GetTextureSize(tex, &tw, &th)) {
                    logicalW = static_cast<int>(tw);
                    logicalH = static_cast<int>(th);
                }
            }
            // Fallback to Core config (window / pixel) if texture size unavailable
            if (logicalW <= 0 || logicalH <= 0) {
                const Core::CoreConfig& cfg = core.getConfig();
                if (cfg.pixelWidth != 0.0f) logicalW = static_cast<int>(cfg.windowWidth / cfg.pixelWidth);
                if (cfg.pixelHeight != 0.0f) logicalH = static_cast<int>(cfg.windowHeight / cfg.pixelHeight);
            }
            // Final fallback
            if (logicalW <= 0) logicalW = 640;
            if (logicalH <= 0) logicalH = 480;
            setWidth(logicalW);
            setHeight(logicalH);
        }
        return true;
    }

    void Stage::onQuit()
    {
        // Stage shutdown logic
        // std::cout << "Stage::onQuit() called for Stage: " << getName() << std::endl;
    }

    void Stage::onUpdate(float fElapsedTime)
    {
        // Stage update logic
        // std::cout << "Stage::onUpdate() called for Stage: " << getName()
        //           << " with elapsed time: " << fElapsedTime << " seconds." << std::endl;
        SUPER::onUpdate(fElapsedTime); // Call base class update handler
    }

    void Stage::onEvent(const Event& event)
    {
        // // Stage event handling logic
        // std::cout << "Stage::onEvent() called for Stage: " << getName()
        //           << " with event type: " << event.getTypeName() << std::endl;

        SUPER::onEvent(event); // Call base class event handler
    }

    void Stage::onRender()
    {
        // Stage rendering logic
        SDL_Renderer* renderer = Core::getInstance().getRenderer();
        if (!renderer)
            ERROR("Core::onRender() Error: Renderer is null.");

        // Clear the entire window to the border color
        SDL_Color color = getColor();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
    }

    bool Stage::onUnitTest(int frame)
    {
        // Run base checks first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Register tests only once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Validate stage dimensions
            ut.add_test(objName, "Stage Size Validation", [this](std::vector<std::string>& errors)
            {
                if (getWidth() <= 0 || getHeight() <= 0)
                {
                    errors.push_back("Stage '" + getName() +
                                    "' has invalid size: w=" +
                                    std::to_string(getWidth()) +
                                    " h=" + std::to_string(getHeight()));
                }
                return true; // ✅ single-frame
            });

            // 🔹 2. Validate clickability
            ut.add_test(objName, "Stage Clickability", [this](std::vector<std::string>& errors)
            {
                if (!isClickable())
                {
                    errors.push_back("Stage '" + getName() +
                                    "' should be clickable by default");
                }
                return true;
            });

            registered = true;
        }

        // ✅ Return false so Stage remains active for the global test cycle
        return false;
    } // END: Stage::onUnitTest()

    
    void Stage::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "Stage");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    


} // namespace SDOM