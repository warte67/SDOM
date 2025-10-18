// Event unit tests

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
    bool Event_test0()
    {
        std::string testName = "Event #0";
        std::string testDesc = "Event UnitTest Scaffolding";
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


    bool Event_test1()
    {
        std::string testName = "Event #1";
        std::string testDesc = "Payload roundtrip C++->Lua";
        sol::state& lua = getCore().getLua();
        // Prepare an event with payload for testing
        SDOM::Event ev(SDOM::EventType("UT_Event_Payload"));
        sol::state_view sv(lua);
        sol::table p = sv.create_table();
        p["answer"] = 42;
        p["name"] = "payload-test";
        ev.setPayload(p);
        // expose the event to Lua
        lua["_tmp_ev"] = &ev;
        // Lua test script
        auto res = lua.script(R"lua(
                local e = _tmp_ev
                local p = e:getPayload()
                if (type(p) == 'table') and (p.answer == 42) and (p.name == 'payload-test') then
                    -- all good
                    return { ok = true, err = "" }
                else
                    return { ok = false, err = "Payload data mismatch" }
                end
                return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        lua["_tmp_ev"] = sol::nil; // clean up temporary global
        // report and return test condition state
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test1()

    bool Event_test2()
    {
        // Event #2: open for future use
        std::string testName = "Event #2";
        std::string testDesc = "Event Open Test Placeholder";
        sol::state& lua = getCore().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
                -- return { ok = false, err = "unknown error" }
                -- Add future test logic here
                return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    }

    bool Event_test3()
    {
        std::string testName = "Event #3";
        std::string testDesc = "stopPropagation toggles propagationStopped";

        SDOM::Event ev(SDOM::EventType("UT_StopProp"));
        bool ok = !ev.isPropagationStopped();
        ev.stopPropagation();
        ok = ok && ev.isPropagationStopped();
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test3()

    bool Event_test4()
    {
        std::string testName = "Event #4";
        std::string testDesc = "SDL_Event visible in Lua via e.sdl";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        SDOM::SDL_Utils::registerLua(lua);
        // Lua test script
        SDL_Event sdl{};
        sdl.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
        sdl.button.timestamp = 12345;
        sdl.button.windowID = 1;
        sdl.button.which = 0;
        sdl.button.button = 1;
        sdl.button.clicks = 2;
        sdl.button.x = 10;
        sdl.button.y = 20;
        // create event and set SDL_Event
        SDOM::Event ev(SDOM::EventType("UT_SDL"));
        ev.setSDL_Event(sdl);
        lua["_tmp_ev"] = &ev;
        auto res = lua.script(R"lua(
            local e = _tmp_ev
            local s = e.sdl
            if not s then 
                return { ok = false, err = "e.sdl is nil" } 
            end
            if not s.button then 
                return { ok = false, err = "s.button is nil" } 
            end
            return { ok = s.button.timestamp == 12345 and s.button.x == 10 and s.button.y == 20 and s.button.clicks == 2, err = "" }
        )lua").get<sol::table>();
        // clean up temporary global
        lua["_tmp_ev"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test4()

    bool Event_test5()
    {
        std::string testName = "Event #5";
        std::string testDesc = "Elapsed time dt getter/setter via Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);

        SDOM::Event ev(SDOM::EventType("UT_DT"));
        ev.setElapsedTime(3.1415f);
        lua["_tmp_ev"] = &ev;

        auto res = lua.script(R"lua(
            local e = _tmp_ev
            if math.abs(e.dt - 3.1415) > 0.001 then 
                return { ok = false, err = "dt initial mismatch" } 
            end
            e:setElapsedTime(1.5)
            if math.abs(e:getElapsedTime() - 1.5) > 0.0001 then
                return { ok = false, err = "dt setter/getter mismatch" }
            end
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // clean up temporary global
        lua["_tmp_ev"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test5()

    bool Event_test6()
    {
        std::string testName = "Event #6";
        std::string testDesc = "Mouse properties via Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        // Lua test script
        SDOM::Event ev(SDOM::EventType("UT_MouseProps"));
        lua["_tmp_ev"] = &ev;
        auto res = lua.script(R"lua(
            local e = _tmp_ev
            e.mouse_x = 100
            e.mouse_y = 200
            e.click_count = 3
            e.button = 2
            return { ok = (e.mouse_x == 100 and e.mouse_y == 200 and e.click_count == 3 and e.button == 2), err = "" }
        )lua").get<sol::table>();
        // clean up temporary global
        lua["_tmp_ev"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test6()


    bool Event_test7()
    {
        std::string testName = "Event #7";
        std::string testDesc = "Keyboard properties via Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        // Lua test script
        SDOM::Event ev(SDOM::EventType("UT_Keyboard"));
        lua["_tmp_ev"] = &ev;
        auto res = lua.script(R"lua(
            local e = _tmp_ev
            e.scan_code = 12
            e.keycode = 65
            e.keymod = 4
            e.ascii_code = 65
            return { ok = (e.scan_code == 12 and e.keycode == 65 and e.keymod == 4 and e.ascii_code == 65), err = "" }
        )lua").get<sol::table>();
        // clean up temporary global
        lua["_tmp_ev"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test7()


    bool Event_test8()
    {
        std::string testName = "Event #8";
        std::string testDesc = "Propagation/default behavior/useCapture via Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        // Lua test script — return table { ok = bool, err = string }
        SDOM::Event ev(SDOM::EventType("UT_Prop"));
        lua["_tmp_ev"] = &ev;
        auto res = lua.script(R"lua(
            local e = _tmp_ev
            if e:isPropagationStopped() then 
                return { ok = false, err = "propagation was already stopped" } 
            end
            e:stopPropagation()
            if not e:isPropagationStopped() then 
                return { ok = false, err = "stopPropagation did not set flag" } 
            end
            e:setDisableDefaultBehavior(true)
            if not e:isDefaultBehaviorDisabled() then 
                return { ok = false, err = "setDisableDefaultBehavior did not take effect" } 
            end
            e:setUseCapture(false)
            return { ok = (e:getUseCapture() == false), err = "" }
        )lua").get<sol::table>();
        // clean up temporary global
        lua["_tmp_ev"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test8()


    bool Event_test9()
    {
        std::string testName = "Event #9";
        std::string testDesc = "Targets roundtrip via DisplayHandle and Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        // Lua test script
        DisplayHandle dh1("targetA", "TypeA");
        DisplayHandle dh2("currentB", "TypeB");
        DisplayHandle dh3("relatedC", "TypeC");
        SDOM::Event ev(SDOM::EventType("UT_Targets"));
        ev.setTarget(dh1);
        ev.setCurrentTarget(dh2);
        ev.setRelatedTarget(dh3);
        lua["_tmp_ev"] = &ev;
        auto res = lua.script(R"lua(
            local e = _tmp_ev
            local t = e.target
            local ct = e.currentTarget
            local rt = e.relatedTarget
            if not t or not ct or not rt then return { ok = false, err = "nil target" } end
            return { ok = (t:getName() == 'targetA' and ct:getName() == 'currentB' and rt:getName() == 'relatedC'), err = "" }
        )lua").get<sol::table>();
        lua["_tmp_ev"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test9()


    bool Event_test10()
    {
        std::string testName = "Event #10";
        std::string testDesc = "Payload set/get from Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        // Lua test script
        SDOM::Event ev(SDOM::EventType("UT_PayloadLua"));
        lua["_tmp_ev"] = &ev;
        auto res = lua.script(R"lua(
            local e = _tmp_ev
            e:setPayload({ x = 7, name = 'Lpayload' })
            local p = e:getPayload()
            if not p then return { ok = false, err = "no payload" } end
            return { ok = (p.x == 7 and p.name == 'Lpayload'), err = "" }
        )lua").get<sol::table>();
        lua["_tmp_ev"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test10()

    bool Event_test11()
    {
        std::string testName = "Event #11";
        std::string testDesc = "name/getName fallback and target-name behavior";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);

        SDOM::Event ev(SDOM::EventType("UT_MyType"));
        lua["_tmp_ev"] = &ev;
        auto tbl1 = lua.script(R"lua(
            local e = _tmp_ev
            return { name = e.name, getName = e:getName() }
        )lua").get<sol::table>();
        std::string before_name = tbl1["name"].get_or(std::string());
        std::string before_getname = tbl1["getName"].get_or(std::string());

        DisplayHandle dh("theTarget", "T");
        ev.setTarget(dh);
        auto tbl2 = lua.script(R"lua(
            local e = _tmp_ev
            return { name = e.name, getName = e:getName() }
        )lua").get<sol::table>();
        lua["_tmp_ev"] = sol::nil;

        std::string after_name = tbl2["name"].get_or(std::string());
        std::string after_getname = tbl2["getName"].get_or(std::string());

        bool r1 = (before_name == "UT_MyType") && (before_getname == "UT_MyType");
        bool r2 = (after_name == "theTarget") && (after_getname == "theTarget");
        if (!r1 || !r2) {
             std::cout << CLR::ORANGE << "[Event_test11] Diagnostic: before(name, getName) = ('" << before_name << "', '" << before_getname << "')" << CLR::RESET << std::endl;
            std::cout << CLR::ORANGE << "[Event_test11] Diagnostic: after(name, getName)  = ('" << after_name << "', '" << after_getname << "')" << CLR::RESET << std::endl;
        }
        bool ok = r1 && r2;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test11()

    bool Event_test12()
    {
        std::string testName = "Event #12";
        std::string testDesc = "pairs metamethod includes expected keys";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        // Lua test script
        SDOM::Event ev(SDOM::EventType("UT_Pairs"));
        ev.setElapsedTime(0.5f);
        lua["_tmp_ev"] = &ev;
        auto res = lua.script(R"lua(
            local e = _tmp_ev
            local t = {}
            for k,v in pairs(e) do t[k]=v end
            return { ok = (t.type ~= nil and t.dt ~= nil), err = "" }
        )lua").get<sol::table>();
        lua["_tmp_ev"] = sol::nil;
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Event_test12()


    bool Event_UnitTests() 
    {
        bool allTestsPassed = true;
    std::vector<std::function<bool()>> tests = {
        [&]() { return Event_test0(); },    // Event UnitTest scaffolding
        [&]() { return Event_test1(); },    // 
        [&]() { return Event_test2(); },    // payload roundtrip
        [&]() { return Event_test3(); },    // stopPropagation semantics
        [&]() { return Event_test4(); },    // SDL_Event exposure
        [&]() { return Event_test5(); },    // dt getter/setter
        [&]() { return Event_test6(); },    // mouse props
        [&]() { return Event_test7(); },    // keyboard props
        [&]() { return Event_test8(); },    // propagation/default/useCapture
        [&]() { return Event_test9(); },    // target/current/related
        [&]() { return Event_test10(); },   // payload from Lua
        [&]() { return Event_test11(); },   // name/getName behavior
        [&]() { return Event_test12(); }    // pairs metamethod
    };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

}