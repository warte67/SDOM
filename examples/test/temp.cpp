SDOM_Init(/*SDL_InitFlags*/);  // this also calls SDL_Init()

bool is_running = true;
while (is_running)
{
    while (SDOM_PollEvent(SDOM::Event& evnt))
    {
        SDOM::EventType type = evnt.getType();
        if (type == EventType::OnUpdate)
        {
            // handle update phase event
        }
        if (type == EventType::OnPreRender)
        {
            // handle all rendering that occurs before the children
        }
        if (type == EventType::OnRender)
        {
            // handle all post children rendering (overlay)
        }
        if (type == EventType::Quit)
        {
            is_running = false;
        }
        SDOM_ProcessFrame();  // Process a single DOM frame
    }
}

SDOM_Quit();

///////////////////////

#include <SDOM/SDOM.hpp>

void fnOnUpdate(float delta_time);
void fnOnRender();
void onQuit();

int main()
{
    SDOM_Init(/*SDL_InitFlags*/);  // this also calls SDL_Init()

    SDOM_RegisterOnUpdate(fnOnUpdate);
    SDOM_RegisterOnRender(fnOnRender);
    SDOM_RegisterOnQuit(fnOnQuit);

    SDOM::CoreConfig core_config;
    core_config.window_width = 800.0f;
    core_config.window_height = 600.0f;
    core_config.pixel_width = 2.0f;
    core_config.pixel_height = 2.0f;
    core_config.preserve_aspect_ratio = true;
    core_config.allow_texture_resize = false;
    core_config.renderer_logical_presentation = SDL_LOGICAL_PRESENTATION_LETTERBOX;
    core_config.window_flags = SDL_WINDOW_RESIZABLE;
    core_config.pixel_format = SDL_PIXELFORMAT_RGBA8888;
    core_config.color = { 32, 32, 32, 255 }; // border color

    SDOM_Run(core_config);
}

void fnOnUpdate(float delta_time)
{
    // process each frame
}
void fnOnRender()
{
    // render each frame
}
void onQuit()
{
    // close out and shutdown custom stuff
}
