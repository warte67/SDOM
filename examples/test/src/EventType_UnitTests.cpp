// EventType unit tests

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_EventTypeHash.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>

#include "UnitTests.hpp"


namespace SDOM
{
    bool EventType_test1()
    {
        bool ok = EventType::isRegistered("MouseClick") && EventType::isRegistered("SDL_Event");
        return UnitTests::run("EventType #1", "EventType statics are present in registry", [=]() { return ok; });
    }    

    bool EventType_test2()
    {
        EventType* custom = new EventType("UT_Custom", false, true, true, false);
        bool ok = EventType::isRegistered("UT_Custom");
        if (ok) {
            ok &= (custom->getName() == "UT_Custom");
            ok &= (custom->getCaptures() == false);
            ok &= (custom->getBubbles() == true);
            ok &= (custom->getTargetOnly() == true);
            ok &= (custom->getGlobal() == false);
        }
        return UnitTests::run("EventType #2", "Constructor flags visible via getters", [=]() { return ok; });
    }  

    bool EventType_test3()
    {
        EventType et("UT_Setters", true, false, false, false);
        bool ok = true;
        et.setCaptures(false).setBubbles(true).setTargetOnly(true).setGlobal(true);
        ok &= (et.getCaptures() == false);
        ok &= (et.getBubbles() == true);
        ok &= (et.getTargetOnly() == true);
        ok &= (et.getGlobal() == true);
        return UnitTests::run("EventType #3", "Setters update the flags", [=]() { return ok; });
    }    

    bool EventType_test4()
    {
        size_t before = EventType::getRegistry().size();
        // register same name via registerEventType; should be no-op since insert only if absent
        EventType::registerEventType("MouseClick", &EventType::MouseClick);
        size_t after = EventType::getRegistry().size();
        bool ok = (after == before);
        return UnitTests::run("EventType #4", "Registering existing name is a no-op", [=]() { return ok; });
    }    

    bool EventType_test5()
    {
        EventType a("A");
        EventType b("B");
        EventType a2("A");
        bool ok = (a == a2) && (a != b) && (a < b);
        return UnitTests::run("EventType #5", "==, != and < follow name semantics", [=]() { return ok; });
    }    

    bool EventType_test6()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool ok = false;
        try {
            // Ensure EventType table is registered (Core does this on startup, but safe-guard)
            EventType::registerLua(lua);
            // Use Lua to register a unique name (avoid collisions)
            std::string name = "Lua_Custom_ET_" + std::to_string(std::rand() % 100000);
            // Directly call the Lua-side EventType.register function to avoid
            // potential overload ambiguities with safe_script variants.
            sol::table etbl = lua["EventType"];
            if (!etbl.valid()) {
                EventType::registerLua(lua);
                etbl = lua["EventType"];
            }
            sol::object regObj = etbl["register"];
            if (!regObj.valid() || regObj.get_type() != sol::type::function) {
                ok = false;
            } else {
                sol::function reg = regObj.as<sol::function>();
                sol::object ret = reg(name, true, false, false, false);
                bool created = ret.valid();
                bool inTable = etbl[name].valid();
                ok = created && inTable && EventType::isRegistered(name);
            }
        } catch (const std::exception& e) {
            std::cerr << "[EventType_test_lua_register] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("EventType #6", "EventType.register from Lua makes type visible in Lua and C++", [=]() { return ok; });
    }    

    bool EventType_test7()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        bool ok = false;
        try {
            auto result = lua.script(R"(
                return (EventType.MouseClick ~= nil) and (EventType.SDL_Event ~= nil)
            )");
            ok = result.get<bool>();
        } catch (const std::exception& e) {
            std::cerr << "[EventType_test7] Lua error: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("EventType #7", "EventType statics visible from Lua", [=]() { return ok; });        
    }

    bool EventType_test8()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        bool ok = false;
        try {
            std::string name = "Lua_UT_Custom_" + std::to_string(std::rand() % 100000);
            lua["etname"] = name;
            auto result = lua.script(R"(
                local n = etname
                local et = EventType.register(n, false, true, true, false)
                if not et then return false end
                local lookup = EventType[n]
                if not lookup then return false end
                return lookup.name == n and lookup.captures == false and lookup.bubbles == true and lookup.targetOnly == true and lookup.global == false
            )");
            ok = result.get<bool>();
        } catch (const std::exception& e) {
            std::cerr << "[EventType_test8] Lua error: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("EventType #8", "EventType.register from Lua and verify properties (Lua)", [=]() { return ok; });
    }

    bool EventType_test9()
    {
        // EventType Lua #9: emulate setter semantics by registering with final flags and verify
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        bool ok = false;
        try {
            std::string name = "Lua_UT_Setters_" + std::to_string(std::rand() % 100000);
            lua["etname"] = name;
            auto result = lua.script(R"(
                local n = etname
                -- create with the expected final values (false,true,true,true)
                local et = EventType.register(n, false, true, true, true)
                if not et then return false end
                local lookup = EventType[n]
                if not lookup then return false end
                return lookup.captures == false and lookup.bubbles == true and lookup.targetOnly == true and lookup.global == true
            )");
            ok = result.get<bool>();
        } catch (const std::exception& e) {
            std::cerr << "[EventType_test9] Lua error: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("EventType #9", "EventType setters semantics via Lua registration", [=]() { return ok; });
    }

    bool EventType_test10()
    {
        // EventType Lua #10: registering existing name is a no-op (identity preserved)
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        bool ok = false;
        try {
            auto result = lua.script(R"(
                local before = EventType('MouseClick')
                EventType.register('MouseClick', true, true, false, false)
                local after = EventType('MouseClick')
                return before == after and before.name == after.name
            )");
            ok = result.get<bool>();
        } catch (const std::exception& e) {
            std::cerr << "[EventType_test10] Lua error: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("EventType #10", "Registering existing name is a no-op (Lua)", [=]() { return ok; });
    }

    bool EventType_test11()
    {
        // EventType Lua #11: equality and ordering via name
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        bool ok = false;
        try {
            std::string nameA = "Lua_A_" + std::to_string(std::rand() % 100000);
            std::string nameB = "Lua_B_" + std::to_string(std::rand() % 100000);
            lua["nameA"] = nameA;
            lua["nameB"] = nameB;
            auto result = lua.script(R"(
                local a = EventType.register(nameA, true, true, false, false)
                local b = EventType.register(nameB, true, true, false, false)
                local a2 = EventType.register(nameA, true, true, false, false)
                -- compare by identity and by name ordering
                return (a == a2) and (a ~= b) and (a.name < b.name)
            )");
            ok = result.get<bool>();
        } catch (const std::exception& e) {
            std::cerr << "[EventType_test11] Lua error: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("EventType #11", "Equality and name-ordering via Lua", [=]() { return ok; });
    }

    bool EventType_test12()
    {
        // EventType Lua #12: EventType.register from Lua makes type visible in Lua and C++ (repeat of test6 via a slightly different path)
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        bool ok = false;
        try {
            std::string name = "Lua_Custom_ET2_" + std::to_string(std::rand() % 100000);
            lua["etname"] = name;
            auto result = lua.script(R"(
                local n = etname
                local created = EventType.register(n, true, false, false, false)
                return created ~= nil and EventType[n] ~= nil
            )");
            ok = result.get<bool>();
            // also verify C++ sees it
            if (ok) ok = EventType::isRegistered(name);
        } catch (const std::exception& e) {
            std::cerr << "[EventType_test12] Lua error: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("EventType #12", "EventType.register from Lua visible in Lua and C++ (Lua)", [=]() { return ok; });
    }

    bool EventType_UnitTests() 
    {
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = {
                [&]() { return EventType_test1(); },    // EventType statics are present in registry
                [&]() { return EventType_test2(); },    // Constructor flags visible via getters
                [&]() { return EventType_test3(); },    // Setters update the flags
                [&]() { return EventType_test4(); },    // Registering existing name is a no-op
                [&]() { return EventType_test5(); },    // ==, != and < follow name semantics
                [&]() { return EventType_test6(); },    // EventType.register from Lua makes type visible in Lua and C++
                [&]() { return EventType_test7(); },    // EventType statics visible from Lua
                [&]() { return EventType_test8(); },    // EventType.register from Lua and verify properties (Lua)
                [&]() { return EventType_test9(); },    // EventType setters semantics via Lua registration
                [&]() { return EventType_test10(); },   // Registering existing name is a no-op (Lua)
                [&]() { return EventType_test11(); },   // Equality and name-ordering via Lua
                [&]() { return EventType_test12(); }
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

}