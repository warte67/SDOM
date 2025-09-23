// SDOM.coo

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>

namespace SDOM
{
    void quit() { Core::getInstance().onQuit(); }
    void shutdown() { Core::getInstance().quit(); } // alias for quit()
    Core& getCore() { return Core::getInstance(); }
    Factory& getFactory() { return getCore().getFactory(); }
    Stage* getStage() { return getCore().getStage(); }
    SDL_Renderer* getRenderer() { return Core::getInstance().getRenderer(); }
    SDL_Window* getWindow() { return Core::getInstance().getWindow(); }
    SDL_Texture* getTexture() { return Core::getInstance().getTexture(); }

} // END namespace SDOM