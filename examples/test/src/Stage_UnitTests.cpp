// Stage_UnitTests.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_UnitTests.hpp>

#include "UnitTests.hpp"

namespace SDOM
{

    bool Stage_UnitTests()
    {
        Core& core = getCore();
        Factory* factory = &core.getFactory();
        // std::cout << "Stage_UnitTests() called" << std::endl;
        bool allTestsPassed = true;
        bool testResult;
        std::string dbgStr;

        // Test: Retrieve existing stage "mainStage"
        testResult = UnitTests::run("Stage #1", "Retrieve 'mainStage'", [factory]() {
            DisplayObject stageHandle = factory->getDisplayObjectHandle("mainStage");
            return (stageHandle && dynamic_cast<Stage*>(stageHandle.get()) != nullptr);
        });
        if (!testResult) { std::cout << CLR::indent() << "Failed to retrieve 'mainStage'!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Test: Retrieve non-existing stage "nonExistentStage"
        testResult = UnitTests::run("Stage #2", "Retrieve non-existent stage", [factory]() {
            DisplayObject stageHandle = factory->getDisplayObjectHandle("nonExistentStage");
            return (stageHandle == nullptr);
        });
        if (!testResult) { std::cout << CLR::indent() << "Non-existent stage retrieval did not return nullptr!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // // Test: Set root stage to "mainStage"
        // testResult = UnitTests::run("Stage", "Set root stage to 'mainStage'", [&core]() {
        //     core.setStage("mainStage");
        //     DisplayObject rootHandle = core.getRootNode();
        //     return (rootHandle && dynamic_cast<Stage*>(rootHandle.get()) != nullptr && rootHandle->getName() == "mainStage");
        // });
        // if (!testResult) { std::cout << CLR::indent() << "Failed to set root stage to 'mainStage'!" << CLR::RESET << std::endl; }
        // allTestsPassed &= testResult;

        // Test: Set root stage to non-existing stage
        testResult = UnitTests::run("Stage #3", "Set root stage to non-existent stage", [&core]() {
            core.setStage("nonExistentStage");
            DisplayObject rootHandle = core.getRootNode();
            return (rootHandle && rootHandle->getName() == "mainStage"); // Should remain unchanged
        });
        if (!testResult) { std::cout << CLR::indent() << "Failed to set root stage to non-existent stage!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Test: Set stage to "stageTwo" using setRootNode with DisplayObject
        std::string newStage = "stageTwo";
        testResult = UnitTests::run("Stage #4", "Set root stage to '" + newStage + "' using DisplayObject", [&core, factory, &dbgStr, newStage]() {
            DisplayObject stageTwoHandle = factory->getDisplayObjectHandle(newStage);
            if (stageTwoHandle && dynamic_cast<Stage*>(stageTwoHandle.get()))
            {
                core.setRootNode(stageTwoHandle);
                DisplayObject rootHandle = core.getRootNode();
                bool nameMatch = (rootHandle && rootHandle->getName() == newStage);
                // Verify window title matches new stage name
                bool titleMatch = (core.getWindowTitle() == "Stage: " + newStage);
                if (!nameMatch)
                    dbgStr = " Expected '" + newStage + "', got '" + (rootHandle ? rootHandle->getName() : "nullptr") + "' ";
                if (!titleMatch)
                    dbgStr += " Window title mismatch: expected 'Stage: " + newStage + "', got '" + core.getWindowTitle() + "' ";
                return nameMatch && titleMatch;
            }
            return false;
        });
        if (!testResult) { std::cout << CLR::indent() << dbgStr<< CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Test: set Stage to "stageThree" using setRootNode with string name
        newStage = "stageThree";
        testResult = UnitTests::run("Stage #5", "Set root stage to '" + newStage + "' using string name", [&core, &dbgStr, newStage]() {
            core.setRootNode(newStage);
            DisplayObject rootHandle = core.getRootNode();
            if (rootHandle && dynamic_cast<Stage*>(rootHandle.get()))
            {
                bool nameMatch = (rootHandle && rootHandle->getName() == newStage);
                // Verify window title matches new stage name
                bool titleMatch = (core.getWindowTitle() == "Stage: " + newStage);
                if (!nameMatch)
                    dbgStr = " Expected '" + newStage + "', got '" + (rootHandle ? rootHandle->getName() : "nullptr") + "' ";
                if (!titleMatch)
                    dbgStr += " Window title mismatch: expected 'Stage: " + newStage + "', got '" + core.getWindowTitle() + "' ";
                return nameMatch && titleMatch;
            }
            return false;
        });
        if (!testResult) { std::cout << CLR::indent() << dbgStr<< CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Test: Set root stage to "mainStage"
        testResult = UnitTests::run("Stage #6", "Set root stage to 'mainStage'", [&core]() {
            core.setStage("mainStage");
            DisplayObject rootHandle = core.getRootNode();
            return (rootHandle && dynamic_cast<Stage*>(rootHandle.get()) != nullptr && rootHandle->getName() == "mainStage");
        });
        if (!testResult) { std::cout << CLR::indent() << "Failed to set root stage to 'mainStage'!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;


        return allTestsPassed;
    }

} // namespace SDOM