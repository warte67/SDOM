// GarbageCollection_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>

#include "UnitTests.hpp"


namespace SDOM
{

    bool GarbageCollection_scaffold()
    {
        // GC Scaffold: Garbage Collection Unit Test Scaffolding
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool ok = false;
        try {
            // run a simple Lua chunk that returns a boolean and read it safely
            auto result = lua.script(R"(
                return true
            )");
            ok = result.get<bool>();
        } catch (const sol::error& e) {
            // log the Lua error and mark test as failed
            std::cerr << "[GC Scaffold] Lua error: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("GC Scaffold", "Garbage Collection Unit Test Scaffolding", [=]() { return ok; });
    }



    bool GarbageCollection_UnitTests() 
    {       
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
            [&]() { return GarbageCollection_scaffold(); }    // Garbage Collection Unit Test Scaffolding

        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

} // END namespace SDOM