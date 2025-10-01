/**
 * @file main.cpp
 * @author Jay Faries (https://github.com/warte67/SDOM)
 * @brief Main entry point for the SDOM example test application.

 * @date 2025-09-19
 * 
 * @copyright Copyright (c) 2025
 * 
 * 
 * 
 * Note:  sudo apt install liblua5.3-dev  or  sudo apt install liblua5.4-dev
 * 
//  */

// #define SOL_ALL_AUTOMAGIC 0
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
 
// For development, define SDOM_USE_INDIVIDUAL_HEADERS and include only 
// the SDOM headers you need. This speeds up incremental builds and makes 
// dependencies explicit.

#define SDOM_USE_INDIVIDUAL_HEADERS
#include <SDOM/SDOM.hpp>

#ifdef SDOM_USE_INDIVIDUAL_HEADERS
    #include <SDOM/SDOM_Core.hpp>
    #include <SDOM/SDOM_Factory.hpp>
    #include <SDOM/SDOM_Event.hpp>
    #include <SDOM/SDOM_IUnitTest.hpp>
    #include <SDOM/SDOM_IDataObject.hpp>
#endif

#include "Box.hpp"
#include "UnitTests.hpp"

using namespace SDOM;


int main() 
{
    std::cout << CLR::YELLOW << "Hello, SDOM!" << CLR::RESET << std::endl;

    // Fetch the Core singleton
    Core& core = getCore();
    // Use Core's Lua state
    sol::state& lua = core.getLua(); 

    // register any custom DisplayObject types before configuring the Core
    core.getFactory().registerDomType("Box", TypeCreators{
        Box::CreateFromLua,
        Box::CreateFromInitStruct
    });    

    // Expose C++ unit test functions to Lua so scripts can call them to run tests
    // (these are defined in UnitTests.hpp)
    lua["Core_UnitTests"] = &SDOM::Core_UnitTests;
    lua["Factory_UnitTests"] = &SDOM::Factory_UnitTests;
    lua["IDisplayObject_UnitTests"] = &SDOM::IDisplayObject_UnitTests;
    lua["Stage_UnitTests"] = &SDOM::Stage_UnitTests;
    lua["Box_UnitTests"] = &SDOM::Box_UnitTests;
    lua["LUA_UnitTests"] = &SDOM::LUA_UnitTests;
    lua["DomHandle_UnitTests"] = &SDOM::DomHandle_UnitTests;
    lua["EventType_UnitTests"] = &SDOM::EventType_UnitTests;
    lua["Event_UnitTests"] = &SDOM::Event_UnitTests;

    // Configure the Core from a Lua config file
    core.run("lua/config.lua");
    return 0;
} // END main()