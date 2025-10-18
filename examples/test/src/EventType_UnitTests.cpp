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
        std::string testName = "EventType #0";
        std::string testDesc = "EventType UnitTest Scaffolding";
        sol::state& lua = getCore().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            -- return { ok = false, err = "unknown error" }
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test0()


    bool EventType_test2()
    {
        std::string testName = "EventType #2";
        std::string testDesc = "Constructor flags visible via getters";
        // construct on stack to avoid leaking memory
        EventType custom("UT_Custom", false, true, true, false);
        // verify registration
        bool ok = EventType::isRegistered("UT_Custom");
        if (ok) {
            ok &= (custom.getName() == "UT_Custom");
            ok &= (custom.getCaptures() == false);
            ok &= (custom.getBubbles() == true);
            ok &= (custom.getTargetOnly() == true);
            ok &= (custom.getGlobal() == false);
        }
        // report and return test condition state
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: EventType_test2()


    bool EventType_test3()
    {
        std::string testName = "EventType #3";
        std::string testDesc = "Setters update the flags";
        // construct on stack to avoid leaking memory
        EventType et("UT_Setters", true, false, false, false);
        et.setCaptures(false).setBubbles(true).setTargetOnly(true).setGlobal(true);
        // verify registration
        bool ok = (et.getCaptures() == false)
               && (et.getBubbles() == true)
               && (et.getTargetOnly() == true)
               && (et.getGlobal() == true);
        // report and return test condition state
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: EventType_test3()


    bool EventType_test4()
    {
        std::string testName = "EventType #4";
        std::string testDesc = "Registering existing name is a no-op";
        // record current registry size
        size_t before = EventType::getRegistry().size();
        // register same name via registerEventType; should be no-op since insert only if absent
        EventType::registerEventType("MouseClick", &EventType::MouseClick);
        size_t after = EventType::getRegistry().size();
        bool ok = (after == before);
        // report and return test condition state
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: EventType_test4()


    bool EventType_test5()
    {
        EventType a("A");
        EventType b("B");
        EventType a2("A");
        bool ok = (a == a2) && (a != b) && (a < b);
        return UnitTests::run("EventType #5", "==, != and < follow name semantics", [=]() { return ok; });
    } // END: EventType_test5()


    bool EventType_test6()
    {
        std::string testName = "EventType #6";
        std::string testDesc = "EventType.register from Lua makes type visible in Lua and C++";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Ensure EventType table is registered
        EventType::registerLua(lua);
        // unique name to avoid collisions
        std::string name = "Lua_Custom_ET_" + std::to_string(std::rand() % 100000);
        // locate the Lua-side register function
        sol::table etbl = lua["EventType"];
        if (!etbl.valid()) {
            EventType::registerLua(lua);
            etbl = lua["EventType"];
        }
        bool ok = false;
        std::string err;
        // call the register function
        sol::object regObj = etbl["register"];
        if (!regObj.valid() || regObj.get_type() != sol::type::function) {
            err = "EventType.register not available in Lua";
        } else {
            sol::function reg = regObj.as<sol::function>();
            sol::object ret = reg(name, true, false, false, false);
            bool created = ret.valid();
            bool inTable = etbl[name].valid();
            ok = created && inTable && EventType::isRegistered(name);
            if (!ok) err = "Lua registration did not make EventType visible";
        }
        // report and return test condition state
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: EventType_test6()
 

    bool EventType_test7()
    {
        std::string testName = "EventType #7";
        std::string testDesc = "EventType statics visible from Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        // Lua test script
        auto res = lua.script(R"lua(
            return { ok = (EventType.MouseClick ~= nil) and (EventType.SDL_Event ~= nil), err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: EventType_test7()


    bool EventType_test8()
    {
        std::string testName = "EventType #8";
        std::string testDesc = "EventType.register from Lua and verify properties (Lua)";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        // unique name to avoid collisions
        std::string name = "Lua_UT_Custom_" + std::to_string(std::rand() % 100000);
        lua["etname"] = name;
        // Lua test script should return table { ok = bool, err = string }
        auto res = lua.script(R"lua(
            local n = etname
            local et = EventType.register(n, false, true, true, false)
            if not et then return { ok = false, err = "register returned nil" } end
            local lookup = EventType[n]
            if not lookup then return { ok = false, err = "lookup missing after register" } end
            return { ok = (lookup.name == n and lookup.captures == false and lookup.bubbles == true and lookup.targetOnly == true and lookup.global == false), err = "" }
        )lua").get<sol::table>();
        // clean up temporary global
        lua["etname"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: EventType_test8()


    bool EventType_test9()
    {
        std::string testName = "EventType #9";
        std::string testDesc = "EventType setters semantics via Lua registration";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        // unique name to avoid collisions
        std::string name = "Lua_UT_Setters_" + std::to_string(std::rand() % 100000);
        lua["etname"] = name;
        // Lua script should return table { ok = bool, err = string }
        auto res = lua.script(R"lua(
            local n = etname
            -- create with the expected final values (false,true,true,true)
            local et = EventType.register(n, false, true, true, true)
            if not et then return { ok = false, err = "register returned nil" } end
            local lookup = EventType[n]
            if not lookup then return { ok = false, err = "lookup missing after register" } end
            return { ok = (lookup.captures == false and lookup.bubbles == true and lookup.targetOnly == true and lookup.global == true), err = "" }
        )lua").get<sol::table>();
        // clean up temporary global
        lua["etname"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: EventType_test9()


    bool EventType_test10()
    {
        std::string testName = "EventType #10";
        std::string testDesc = "Registering existing name is a no-op (Lua)";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        // Lua test script should return table { ok = bool, err = string }
        auto res = lua.script(R"lua(
            local before = EventType('MouseClick')
            EventType.register('MouseClick', true, true, false, false)
            local after = EventType('MouseClick')
            if before == after and before.name == after.name then
                return { ok = true, err = "" }
            else
                return { ok = false, err = "identity changed after register" }
            end
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: EventType_test10()



    bool EventType_test11()
    {
        std::string testName = "EventType #11";
        std::string testDesc = "Equality and name-ordering via Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        // unique names to avoid collisions
        std::string nameA = "Lua_A_" + std::to_string(std::rand() % 100000);
        std::string nameB = "Lua_B_" + std::to_string(std::rand() % 100000);
        lua["nameA"] = nameA;
        lua["nameB"] = nameB;
        // Lua test script returns table { ok = bool, err = string }
        auto res = lua.script(R"lua(
            local a = EventType.register(nameA, true, true, false, false)
            local b = EventType.register(nameB, true, true, false, false)
            local a2 = EventType.register(nameA, true, true, false, false)
            return { ok = ((a == a2) and (a ~= b) and (a.name < b.name)), err = "" }
        )lua").get<sol::table>();
        // clean up temporary globals
        lua["nameA"] = sol::nil;
        lua["nameB"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: EventType_test11()


    bool EventType_test12()
    {
        std::string testName = "EventType #12";
        std::string testDesc = "EventType.register from Lua visible in Lua and C++ (Lua)";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        EventType::registerLua(lua);
        // unique name to avoid collisions
        std::string name = "Lua_Custom_ET2_" + std::to_string(std::rand() % 100000);
        lua["etname"] = name;
        // Lua script should return table { ok = bool, err = string }
        auto res = lua.script(R"lua(
            local n = etname
            local created = EventType.register(n, true, false, false, false)
            local ok = (created ~= nil and EventType[n] ~= nil)
            return { ok = ok, err = "" }
        )lua").get<sol::table>();
        // clean up temporary global
        lua["etname"] = sol::nil;
        // report and verify C++ visibility as well
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (ok) ok = EventType::isRegistered(name);
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: EventType_test12()


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