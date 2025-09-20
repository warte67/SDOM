/**
 * @file main.cpp
 * @author Jay Faries (https://github.com/warte67/SDOM)
 * @brief Main entry point for the SDOM example test application.

 * @date 2025-09-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include <SDOM/SDOM.hpp>
 #include <SDOM/SDOM_Core.hpp>

 using namespace SDOM;


int main() 
{
    std::cout << CLR::YELLOW << "Hello, SDOM!" << CLR::RESET << std::endl;

    Core& core = getCore();
    core.configureFromJson(R"({
        "core": {
            "windowWidth": 1200,
            "windowHeight": 800,
            "pixelWidth": 2,
            "pixelHeight": 2,
            "allowTextureResize": true,
            "preserveAspectRatio": true,
            "rendererLogicalPresentation": "SDL_LOGICAL_PRESENTATION_LETTERBOX",
            "windowFlags": "SDL_WINDOW_RESIZABLE",
            "pixelFormat": "SDL_PIXELFORMAT_RGBA8888",
            "color": { "r": 32, "g": 0, "b": 8, "a": 255 }
        }
    })");

    // core.registerOnInit([]() {
    //     std::cout << CLR::GREEN << "Custom OnInit called!" << CLR::RESET << std::endl;
    //     return true; // Initialization successful
    // });

    // core.registerOnEvent([](const Core::Event& event) {
    //     // Handle events here
    //     std::cout << CLR::CYAN << "Event received!" << CLR::RESET << std::endl;
    // });

    // core.registerOnQuit([]() {
    //     std::cout << CLR::PURPLE << "Custom OnQuit called!" << CLR::RESET << std::endl;
    // });

    // core.registerOnRender([]() {
    //     // Custom rendering logic here
    //     std::cout << CLR::BLUE << "Custom Render called!" << CLR::RESET << std::endl;
    // });

    // core.registerOnUnitTest([]() {
    //     std::cout << CLR::GREEN << "Custom Unit Test called!" << CLR::RESET << std::endl;
    //     return true; // All tests passed
    // });

    // core.registerOnUpdate([](float fElapsedTime) {
    //     // Update logic here
    //     std::cout << CLR::YELLOW << "Custom Update called! Elapsed time: " << fElapsedTime << " seconds" << CLR::RESET << std::endl;
    // });

    core.registerOnWindowResize([](int newWidth, int newHeight) {
        std::cout << "Window Resized--Width: " << newWidth << "  Height: " << newHeight << std::endl;
    });

    core.run();

    return 0;
}