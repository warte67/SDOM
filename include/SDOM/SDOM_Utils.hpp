// SDOM_Utility.hpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_CLR.hpp>

#pragma once

namespace SDOM
{
    void quit();
    void shutdown();
    Core& getCore();
    Factory& getFactory();
    Stage* getStage();
    DomHandle getStageHandle();
    SDL_Renderer* getRenderer();
    SDL_Window* getWindow();
    SDL_Texture* getTexture();

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
} // namespace SDOM