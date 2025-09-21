// SDOM_Stage.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_Factory.hpp>

namespace SDOM
{

    Stage::Stage(const InitStageObject& init) : IDisplayObject(init)
    {
        type_ = "Stage"; // Hardcoded type for Stage
    }

    Stage::Stage(const Json& config) : IDisplayObject(config)
    {
        type_ = "Stage"; // Hardcoded type for Stage
    }


    bool Stage::onInit()
    {
        // Stage initialization logic
        return true;
    }

    void Stage::onQuit()
    {
        // Stage shutdown logic
    }

    void Stage::onUpdate(float fElapsedTime)
    {
        // Stage update logic
    }

    void Stage::onEvent(const Event& event)
    {
        // Stage event handling logic
    }

    void Stage::onRender()
    {
        SDL_Renderer* renderer = Core::getInstance().getRenderer();
        if (!renderer)
            ERROR("Core::onRender() Error: Renderer is null.");

        // Clear the entire window to the border color
        SDL_Color color = getColor();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
    }

    bool Stage::onUnitTest()
    {
        // Stage unit testing logic
        return true;
    }

} // namespace SDOM