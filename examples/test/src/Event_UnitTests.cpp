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

    bool Event_test2()
    {
        // Event #2: payload roundtrip (C++ set, Lua read)
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        bool ok = false;
        try {
            SDOM::Event ev(SDOM::EventType("UT_Event_Payload"));
            sol::state_view sv(lua);
            sol::table p = sv.create_table();
            p["answer"] = 42;
            p["name"] = "payload-test";
            ev.setPayload(p);
            // expose the event to Lua and read payload there
            lua["_tmp_ev"] = &ev;
            auto result = lua.script(R"(
                local e = _tmp_ev
                local p = e:getPayload()
                return (type(p) == 'table') and (p.answer == 42) and (p.name == 'payload-test')
            )");
            ok = result.get<bool>();
            lua["_tmp_ev"] = sol::nil;
        } catch (const std::exception& e) {
            std::cerr << "[Event_test2] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #2", "Payload roundtrip C++->Lua", [=]() { return ok; });
    }

    bool Event_test3()
    {
        // Event #3: stopPropagation semantics
        bool ok = false;
        try {
            SDOM::Event ev(SDOM::EventType("UT_StopProp"));
            ok = !ev.isPropagationStopped();
            ev.stopPropagation();
            ok = ok && ev.isPropagationStopped();
        } catch (const std::exception& e) {
            std::cerr << "[Event_test3] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #3", "stopPropagation toggles propagationStopped", [=]() { return ok; });
    }

    bool Event_test4()
    {
        // Event #4: SDL_Event exposure to Lua via SDL_Utils::eventToLuaTable
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        SDOM::SDL_Utils::registerLua(lua);
        bool ok = false;
        try {
            SDL_Event sdl{};
            sdl.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
            sdl.button.timestamp = 12345;
            sdl.button.windowID = 1;
            sdl.button.which = 0;
            sdl.button.button = 1;
            sdl.button.clicks = 2;
            sdl.button.x = 10;
            sdl.button.y = 20;

            SDOM::Event ev(SDOM::EventType("UT_SDL"));
            ev.setSDL_Event(sdl);
            lua["_tmp_ev"] = &ev;
            auto result = lua.script(R"(
                local e = _tmp_ev
                local s = e.sdl
                if not s then return false end
                if not s.button then return false end
                return s.button.timestamp == 12345 and s.button.x == 10 and s.button.y == 20 and s.button.clicks == 2
            )");
            ok = result.get<bool>();
            lua["_tmp_ev"] = sol::nil;
        } catch (const std::exception& e) {
            std::cerr << "[Event_test4] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #4", "SDL_Event visible in Lua via e.sdl", [=]() { return ok; });
    }

    bool Event_test5()
    {
        // Event #5: elapsed time (dt) via property and getters/setters
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        bool ok = false;
        try {
            SDOM::Event ev(SDOM::EventType("UT_DT"));
            ev.setElapsedTime(3.1415f);
            lua["_tmp_ev"] = &ev;
            auto result = lua.script(R"(
                local e = _tmp_ev
                if math.abs(e.dt - 3.1415) > 0.001 then return false end
                e:setElapsedTime(1.5)
                return math.abs(e:getElapsedTime() - 1.5) < 0.0001
            )");
            ok = result.get<bool>();
            lua["_tmp_ev"] = sol::nil;
        } catch (const std::exception& e) {
            std::cerr << "[Event_test5] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #5", "Elapsed time dt getter/setter via Lua", [=]() { return ok; });
    }

    bool Event_test6()
    {
        // Event #6: mouse properties via lower-camelcase properties and methods
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        bool ok = false;
        try {
            SDOM::Event ev(SDOM::EventType("UT_MouseProps"));
            lua["_tmp_ev"] = &ev;
            auto result = lua.script(R"(
                local e = _tmp_ev
                e.mouse_x = 100
                e.mouse_y = 200
                e.click_count = 3
                e.button = 2
                return e.mouse_x == 100 and e.mouse_y == 200 and e.click_count == 3 and e.button == 2
            )");
            ok = result.get<bool>();
            lua["_tmp_ev"] = sol::nil;
        } catch (const std::exception& e) {
            std::cerr << "[Event_test6] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #6", "Mouse properties via Lua", [=]() { return ok; });
    }

    bool Event_test7()
    {
        // Event #7: keyboard properties
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        bool ok = false;
        try {
            SDOM::Event ev(SDOM::EventType("UT_Keyboard"));
            lua["_tmp_ev"] = &ev;
            auto result = lua.script(R"(
                local e = _tmp_ev
                e.scan_code = 12
                e.keycode = 65
                e.keymod = 4
                e.ascii_code = 65
                return e.scan_code == 12 and e.keycode == 65 and e.keymod == 4 and e.ascii_code == 65
            )");
            ok = result.get<bool>();
            lua["_tmp_ev"] = sol::nil;
        } catch (const std::exception& e) {
            std::cerr << "[Event_test7] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #7", "Keyboard properties via Lua", [=]() { return ok; });
    }

    bool Event_test8()
    {
        // Event #8: propagation/defaultBehavior/useCapture
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        bool ok = false;
        try {
            SDOM::Event ev(SDOM::EventType("UT_Prop"));
            lua["_tmp_ev"] = &ev;
            auto result = lua.script(R"(
                local e = _tmp_ev
                if e:isPropagationStopped() then return false end
                e:stopPropagation()
                if not e:isPropagationStopped() then return false end
                e:setDisableDefaultBehavior(true)
                if not e:isDefaultBehaviorDisabled() then return false end
                e:setUseCapture(false)
                return e:getUseCapture() == false
            )");
            ok = result.get<bool>();
            lua["_tmp_ev"] = sol::nil;
        } catch (const std::exception& e) {
            std::cerr << "[Event_test8] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #8", "Propagation/default behavior/useCapture via Lua", [=]() { return ok; });
    }

    bool Event_test9()
    {
        // Event #9: target/currentTarget/relatedTarget roundtrip (DomHandle names)
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        bool ok = false;
        try {
            DomHandle dh1("targetA", "TypeA");
            DomHandle dh2("currentB", "TypeB");
            DomHandle dh3("relatedC", "TypeC");
            SDOM::Event ev(SDOM::EventType("UT_Targets"));
            ev.setTarget(dh1);
            ev.setCurrentTarget(dh2);
            ev.setRelatedTarget(dh3);
            lua["_tmp_ev"] = &ev;
            auto result = lua.script(R"(
                local e = _tmp_ev
                local t = e.target
                local ct = e.currentTarget
                local rt = e.relatedTarget
                if not t or not ct or not rt then return false end
                return t:getName() == 'targetA' and ct:getName() == 'currentB' and rt:getName() == 'relatedC'
            )");
            ok = result.get<bool>();
            lua["_tmp_ev"] = sol::nil;
        } catch (const std::exception& e) {
            std::cerr << "[Event_test9] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #9", "Targets roundtrip via DomHandle and Lua", [=]() { return ok; });
    }

    bool Event_test10()
    {
        // Event #10: payload manipulation from Lua
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        bool ok = false;
        try {
            SDOM::Event ev(SDOM::EventType("UT_PayloadLua"));
            lua["_tmp_ev"] = &ev;
            auto result = lua.script(R"(
                local e = _tmp_ev
                e:setPayload({ x = 7, name = 'Lpayload' })
                local p = e:getPayload()
                return p.x == 7 and p.name == 'Lpayload'
            )");
            ok = result.get<bool>();
            lua["_tmp_ev"] = sol::nil;
        } catch (const std::exception& e) {
            std::cerr << "[Event_test10] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #10", "Payload set/get from Lua", [=]() { return ok; });
    }

    bool Event_test11()
    {
        // Event #11: name/getName - falls back to type when no target, uses target name when present
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        bool ok = false;
        try {
            SDOM::Event ev(SDOM::EventType("UT_MyType"));
            // no target -> name should be type name
            lua["_tmp_ev"] = &ev;
            // capture both name and getName for diagnostics
            auto tbl1 = lua.script(R"(
                local e = _tmp_ev
                return { name = e.name, getName = e:getName() }
            )");
            sol::table t1 = tbl1;
            std::string before_name = t1["name"].get_or(std::string());
            std::string before_getname = t1["getName"].get_or(std::string());
            bool r1 = (before_name == "UT_MyType") && (before_getname == "UT_MyType");
            // set a target and re-check
            DomHandle dh("theTarget", "T");
            ev.setTarget(dh);
            auto tbl2 = lua.script(R"(
                local e = _tmp_ev
                return { name = e.name, getName = e:getName() }
            )");
            sol::table t2 = tbl2;
            std::string after_name = t2["name"].get_or(std::string());
            std::string after_getname = t2["getName"].get_or(std::string());
            bool r2 = (after_name == "theTarget") && (after_getname == "theTarget");
            if (!r1 || !r2) {
                std::cerr << "[Event_test11] Diagnostic: before(name, getName) = ('" << before_name << "', '" << before_getname << "')\n";
                std::cerr << "[Event_test11] Diagnostic: after(name, getName)  = ('" << after_name << "', '" << after_getname << "')\n";
            }
            ok = r1 && r2;
            lua["_tmp_ev"] = sol::nil;
        } catch (const std::exception& e) {
            std::cerr << "[Event_test11] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #11", "name/getName fallback and target-name behavior", [=]() { return ok; });
    }

    bool Event_test12()
    {
        // Event #12: pairs meta-function includes type and dt
        sol::state& lua = SDOM::Core::getInstance().getLua();
        SDOM::Event::registerLua(lua);
        bool ok = false;
        try {
            SDOM::Event ev(SDOM::EventType("UT_Pairs"));
            ev.setElapsedTime(0.5f);
            lua["_tmp_ev"] = &ev;
            auto result = lua.script(R"(
                local e = _tmp_ev
                local t = {}
                for k,v in pairs(e) do t[k]=v end
                return t.type ~= nil and t.dt ~= nil
            )");
            ok = result.get<bool>();
            lua["_tmp_ev"] = sol::nil;
        } catch (const std::exception& e) {
            std::cerr << "[Event_test12] Exception: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("Event #12", "pairs metamethod includes expected keys", [=]() { return ok; });
    }


    bool Event_UnitTests() 
    {
        bool allTestsPassed = true;
    std::vector<std::function<bool()>> tests = {
        [&]() { return Event_test1(); },    // scaffolding (pseudo-test)
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