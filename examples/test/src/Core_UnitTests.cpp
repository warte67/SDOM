// Core_UnitTestss.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp> 
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM
{
    bool Core_UnitTests()
    {
        Core& core = getCore();
        // std::cout << "Core_UnitTests() called" << std::endl;
        bool allTestsPassed = true;
        bool testResult;

        // Config_Test: Window Dimensions
        const auto& expected = core.getConfig();
        testResult = UnitTests::run("Core #1", "Window Width", [&core, &expected]() {
            int actualW = 0, actualH = 0;
            SDL_GetWindowSize(core.getWindow(), &actualW, &actualH);
            return actualW == static_cast<int>(expected.windowWidth);
        });
        if (!testResult) { std::cout << CLR::indent() << "Window width does not match expected!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        testResult = UnitTests::run("Core #2", "Window Height", [&core, &expected]() {
            int actualW = 0, actualH = 0;
            SDL_GetWindowSize(core.getWindow(), &actualW, &actualH);
            return actualH == static_cast<int>(expected.windowHeight);
        });
        if (!testResult) { std::cout << CLR::indent() << "Window height does not match expected!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Config_Test: Pixel Dimensions
        testResult = UnitTests::run("Core #3", "Pixel Width", [&core, &expected]() {
            return core.getPixelWidth() == expected.pixelWidth;
        });
        if (!testResult) { std::cout << CLR::indent() << "Pixel width does not match expected!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        testResult = UnitTests::run("Core #4", "Pixel Height", [&core, &expected]() {
            return core.getPixelHeight() == expected.pixelHeight;
        });
        if (!testResult) { std::cout << CLR::indent() << "Pixel height does not match expected!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Config_Test: Aspect Ratio Preservation
        testResult = UnitTests::run("Core #5", "Preserve Aspect Ratio", [&core, &expected]() {
            return core.getPreserveAspectRatio() == expected.preserveAspectRatio;
        });
        if (!testResult) { std::cout << CLR::indent() << "PreserveAspectRatio does not match expected!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Config_Test: Texture Resize Allowance
        testResult = UnitTests::run("Core #6", "Allow Texture Resize", [&core, &expected]() {
            return core.getAllowTextureResize() == expected.allowTextureResize;
        });
        if (!testResult) { std::cout << CLR::indent() << "AllowTextureResize does not match expected!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Config_Test: Pixel Format
        testResult = UnitTests::run("Core #7", "Pixel Format", [&core, &expected]() {
            return core.getPixelFormat() == expected.pixelFormat;
        });
        if (!testResult) { std::cout << CLR::indent() << "PixelFormat does not match expected!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Config_Test: Renderer Logical Presentation
        testResult = UnitTests::run("Core #8", "Renderer Logical Presentation", [&core, &expected]() {
            return core.getRendererLogicalPresentation() == expected.rendererLogicalPresentation;
        });
        if (!testResult) { std::cout << CLR::indent() << "RendererLogicalPresentation does not match expected!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Config_Test: Window Flags
        testResult = UnitTests::run("Core #9", "Window Flags", [&core, &expected]() {
            return core.getWindowFlags() == expected.windowFlags;
        });
        if (!testResult) { std::cout << CLR::indent() << "WindowFlags does not match expected!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Config_Test: Color
        testResult = UnitTests::run("Core #10", "Border Color", [&core, &expected]() {
            SDL_Color color = core.getColor();
            return (color.r == expected.color.r && color.g == expected.color.g && color.b == expected.color.b && color.a == expected.color.a);
        });
        if (!testResult) { std::cout << CLR::indent() << "BorderColor does not match expected!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Core::Factory Existence Test
        testResult = UnitTests::run("Core #11", "Factory Existence", [&core]() {
            return (&core.getFactory() != nullptr);
        });
        if (!testResult) { std::cout << CLR::indent() << "Factory instance is not valid!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Add more Core sub-system unit tests here as needed...

        return allTestsPassed;
    }
} // namespace SDOM


// #endif // SDOM_ENABLE_UNIT_TESTS
