// UnitTests.hpp
#pragma once

/******************
* This file declares all UnitTest functions for the SDOM library.
* Each function is defined in its respective UnitTest .cpp file.
* The main test runner (examples/test/main.cpp) calls these functions
* to execute the full suite of unit tests.
*
* When adding new UnitTests, declare them here and define them
* in their respective source files:
*   - /home/jay/Documents/GitHub/SDOM/examples/test/main.cpp
*   - /home/jay/Documents/GitHub/SDOM/examples/test/lua/callbacks/unittest.lua
*   - Add an appropriate UnitTest .cpp file. Use scaffold_UnitTests.cpp as a template.
*       - Implement the test functions in the new .cpp file.
*   - Prototype the new UnitTest functions here in this header.
*
* C++ Language Standard: C++23
*
******************/

namespace SDOM 
{
    bool Core_UnitTests();
    bool IDisplayObject_UnitTests();
    bool Event_UnitTests();
    
    // bool Factory_UnitTests();
    // bool Stage_UnitTests();
    // bool Box_UnitTests();
    // bool LUA_UnitTests();
    // bool DisplayHandle_UnitTests();
    // bool Event_UnitTests();
    // bool GarbageCollection_UnitTests();
    // bool SpriteSheet_UnitTests();
    // bool Label_UnitTests();
    // bool Group_UnitTests(); // new Group unit tests
    // add more unit tests here as needed ...
}