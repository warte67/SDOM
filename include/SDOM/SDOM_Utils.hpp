// SDOM_Utility.hpp
// #include <SDOM/SDOM.hpp>
// #include <SDOM/SDOM_Core.hpp>
// #include <SDOM/SDOM_Factory.hpp>
// #include <SDOM/SDOM_CLR.hpp>

#pragma once
#include <cassert>



namespace SDOM
{
    // extern Core* g_core;  // temporarty global for getCore()
    // Core& getCore();
    // void setCore(Core* c);



    class Stage;
    
    void quit();
    void shutdown();
    Core& getCore();
    Factory& getFactory();
    Stage* getStage();
    DisplayHandle getStageHandle();
    SDL_Renderer* getRenderer();
    SDL_Window* getWindow();
    SDL_Texture* getTexture();

    sol::state& getLua();

    // helper function to extract SDL_Color from Lua table
    SDL_Color parseColor(const sol::object& colorObj);

    // helper function to normalize anchor point strings
    std::string normalizeAnchorString(const std::string& s);

    // helper function to validate anchor point strings
    bool validateAnchorPointString(const std::string& anchorString);

    // helper function to validate anchor point assignments in a config table
    bool validateAnchorAssignments(const sol::table& config);

    // helper function to print a message box to the terminal
    void printMessageBox(const std::string& title, const std::string& message, 
                          const std::string& file /* = "" */, int line /* = -1 */, 
                          const std::string& colorTitle /* = CLR::RED */, 
                          const std::string& colorMsg /* = CLR::WHITE */,
                          const std::string& colorBorder /* = CLR::BROWN */);

    void showWarning(const std::string& message, const std::string& file, int line);

    // Cached-texture guard: if the cached SDL_Texture is invalid for the
    // current renderer (owner != current) or its size query fails, destroy it,
    // clear both the texture and owner pointers, and return true. Callers
    // typically follow this with setDirty(true) to trigger a rebuild.
    bool drop_invalid_cached_texture(SDL_Texture*& tex,
                                     SDL_Renderer* currentRenderer,
                                     SDL_Renderer*& textureOwnerRenderer);
} // namespace SDOM
