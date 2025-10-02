// Event unit tests

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_EventTypeHash.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"


namespace SDOM
{
    bool Event_test1()
    {
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
            std::cerr << "[Event_test1] Lua error: " << e.what() << std::endl;
            ok = false;
        }

        return UnitTests::run("Event #1", "Scaffolding Pseudo-Test Description", [=]() { return ok; });
    }   


    bool Event_UnitTests() 
    {
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = {
                [&]() { return Event_test1(); }
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

}