// Factory_UnitTests.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_DisplayObject.hpp>
#include "UnitTests.hpp"

namespace SDOM
{
    bool Factory_UnitTests()
    {
        // std::cout << "Factory_UnitTests() called" << std::endl; 
        
        Core& core = getCore();
        Factory& factory = core.getFactory();

        bool allTestsPassed = true;
        bool result; 
        std::string debugRet;
        std::vector<std::string> originalNames = factory.listDisplayObjectNames();

        // Test display object creation
        DisplayObject handle;
        result = UnitTests::run("Factory #1", "Display Object Creation", [&factory, &handle]() {
            sol::state lua;
            lua.open_libraries(sol::lib::base);
            lua.script(R"(
                config = {
                    type = "Stage",
                    name = "testStage",
                    color = { r = 7, g = 15, b = 31, a = 63 }
                }
            )");
            sol::table config = lua["config"];
            handle = factory.create("Stage", config);
            return handle != nullptr;
        });
        if (!result) { std::cout << CLR::indent() << "Resource creation test failed!" << CLR::RESET << std::endl; }
        allTestsPassed &= result;


        // Test resource retrieval
        result = UnitTests::run("Factory #2", "Resource Retrieval", [&factory, &debugRet, &handle]() 
        {
            DisplayObject testHandle = factory.getDisplayObjectHandle("testStage");
            if (testHandle != handle) 
            {
                debugRet = "Expected handle: " + handle.str() + ", but got: " + testHandle.str();
                return false;
            }
            return true;
        });
        if (!result) { std::cout << CLR::indent() << debugRet << CLR::RESET << std::endl; }
        allTestsPassed &= result;

        // Test resource properties
        result = UnitTests::run("Factory #3", "Verify Resource Handle", [&factory, &debugRet, &handle]() 
        {
            if (!handle) 
            {
                debugRet = "Handle is null";
                return false;
            }
            IDisplayObject* obj = handle.get();
            if (!obj) 
            {
                debugRet = "Object is null";
                return false;
            }
            return true;
        });
        if (!result) { std::cout << CLR::indent() << debugRet << CLR::RESET << std::endl; }
        allTestsPassed &= result;

        // Test handle type
        result = UnitTests::run("Factory #4", "Resource Type", [&factory, &handle, &debugRet]() 
        {
            if (!handle) 
            {
                debugRet = "Handle is null";
                return false;
            }
            IDisplayObject* obj = handle.get();
            if (!obj)
            {
                debugRet = "Object is null";
                return false;
            }
            if (obj->getType() != "Stage")
            {
                debugRet = "Object (" + obj->getType() + ") is not of type 'Stage'";
                return false;
            }
            return obj->getType() == "Stage";
        });
        if (!result) { std::cout << CLR::indent() << debugRet << CLR::RESET << std::endl; }
        allTestsPassed &= result;

        // Test handle name
        result = UnitTests::run("Factory #5", "Resource Name", [&factory, &handle, &debugRet]() 
        {
            if (!handle) 
            {
                debugRet = "Handle is null";
                return false;
            }
            IDisplayObject* obj = handle.get();
            if (!obj)
            {
                debugRet = "Object is null";
                return false;
            }
            if (obj->getName() != "testStage")
            {
                debugRet = "Object name (" + obj->getName() + ") does not match 'testStage'";
                return false;
            }
            return obj->getName() == "testStage";
        });
        if (!result) { std::cout << CLR::indent() << debugRet << CLR::RESET << std::endl; }
        allTestsPassed &= result;

        // Test destroy resource
        result = UnitTests::run("Factory #6", "Resource Destruction", [&factory, &handle]() 
        {
            factory.destroyDisplayObject("testStage");
            DisplayObject testHandle = factory.getDisplayObjectHandle("testStage");
            return testHandle == nullptr;
        });
        if (!result) { std::cout << CLR::indent() << "Resource destruction test failed!" << CLR::RESET << std::endl; }
        allTestsPassed &= result;

        // duplicate resource creation test
        result = UnitTests::run("Factory #7", "Duplicate Resource Creation", [&factory]() 
        {
            sol::state lua;
            lua.open_libraries(sol::lib::base);
            lua.script(R"(
                config = {
                    type = "Stage",
                    name = "testStage",
                    color = { r = 1, g = 2, b = 3, a = 4 }
                }
            )");
            sol::table config = lua["config"];
            DisplayObject first = factory.create("Stage", config);
            DisplayObject second = factory.create("Stage", config);
            // Define expected behavior: should not allow duplicate, or should overwrite
            bool ret = first != nullptr && second != nullptr;
            // clean up
            factory.destroyDisplayObject("testStage");
            return ret;
        });
        if (!result) { std::cout << CLR::indent() << "Duplicate resource creation test failed!" << CLR::RESET << std::endl; }
        allTestsPassed &= result;


        // TEST: Resource Overwrite Behavior
        result = UnitTests::run("Factory #8", "Resource Overwrite Behavior", [&factory, &debugRet]() 
        {
            sol::state lua;
            lua.open_libraries(sol::lib::base);

            // Create initial resource
            lua.script(R"(
                config1 = {
                    type = "Stage",
                    name = "overwriteTest",
                    color = { r = 10, g = 20, b = 30, a = 40 }
                }
            )");
            sol::table config1 = lua["config1"];
            DisplayObject first = factory.create("Stage", config1);

            // Overwrite with new config
            lua.script(R"(
                config2 = {
                    type = "Stage",
                    name = "overwriteTest",
                    color = { r = 15, g = 25, b = 35, a = 45 }
                }
            )");
            sol::table config2 = lua["config2"];
            DisplayObject second = factory.create("Stage", config2);

            // Retrieve and check if the resource was overwritten
            DisplayObject retrieved = factory.getDisplayObjectHandle("overwriteTest");
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(retrieved.get());
            if (!obj) {
                debugRet = "Resource 'overwriteTest' not found after overwrite.";
                return false;
            }

            // Check if color matches the second config (overwritten)
            SDL_Color color = obj->getColor();
            if (color.r != 15 || color.g != 25 || color.b != 35 || color.a != 45) {
                debugRet = "Resource 'overwriteTest' color was not overwritten correctly.";
                return false;
            }

            // Clean up
            factory.destroyDisplayObject("overwriteTest");
            return true;
        });
        if (!result) { std::cout << CLR::indent() << debugRet << CLR::RESET << std::endl; }
        allTestsPassed &= result;



        // Last Test: verify original resources remain
        std::vector<std::string> currentNames = factory.listDisplayObjectNames();
        result = UnitTests::run("Factory #9", "Original Resources Intact", [&debugRet, &originalNames, &currentNames]() 
        {
            if (originalNames.size() != currentNames.size()) 
            {
                debugRet = "Original and current resource counts differ.\n";
                debugRet += CLR::indent() + "Original count: " + std::to_string(originalNames.size()) + ", Current count: " + std::to_string(currentNames.size()) + "\n";
                debugRet += CLR::indent() + "Original names: ";
                for (const auto& name : originalNames) debugRet += name + " ";
                debugRet += "\n" + CLR::indent() + "Current names: ";
                for (const auto& name : currentNames) debugRet += name + " ";
                return false;
            }
            bool ret = true;
            for (const auto& name : originalNames) 
            {
                if (std::find(currentNames.begin(), currentNames.end(), name) == currentNames.end()) 
                {
                    debugRet += "Original resource '" + name + "' is missing from current resources.\n";
                    ret = false;
                }
            }
            for (const auto& name : currentNames) 
            {
                if (std::find(originalNames.begin(), originalNames.end(), name) == originalNames.end()) 
                {
                    debugRet += "Extra resource '" + name + "' found in current resources.\n";
                    ret = false;
                }
            }
            return ret;
        });
        if (!result) { std::cout << CLR::indent() << debugRet << CLR::RESET << std::endl; }
        allTestsPassed &= result;


        return allTestsPassed;                
    } // END Factory_UnitTests()
} // namespace SDOM
