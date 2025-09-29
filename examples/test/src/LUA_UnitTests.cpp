// Lua_UnitTests.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"
// Include Box so test helpers (reset/getTestClickCount) are visible
#include "Box.hpp"
// Lua_UnitTests.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM {

DomHandle myBoxHandle;

bool test1() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    sol::object handle_obj = lua.script(R"(
        Core:createDisplayObject("Box", { 
            name = "myBox", 
            type = "Box", 
            x = 10, y = 20,
            width = 200, height = 100,
            color = { r = 96, g = 0, b = 96, a = 255 }
        })
        return Core:getDisplayHandle("myBox")
    )");
    myBoxHandle = handle_obj.as<DomHandle>();
    bool box_exists = myBoxHandle.isValid();
    return UnitTests::run("Lua: test # 1", "Create Box and verify existence", [box_exists]() { return box_exists; });
}

bool test2() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    lua["myBoxHandle"] = myBoxHandle;
    bool added = lua.script(R"(
        local obj = myBoxHandle
        local stage = Core:getStageHandle()
        if not stage then return false end
        stage:addChild(obj)
        return stage:hasChild(obj)
    )").get<bool>();
    return UnitTests::run("Lua: test # 2", "Add the Box to Stage from Lua", [added]() { return added; });
}

bool test3() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    bool destroyed = lua.script(R"(
        local boxHandle = Core:getDisplayHandle("myBox")
        local valid = boxHandle:isValid()
        if valid then
            Core:destroyDisplayObject("myBox")
            local inFactory = Core:hasDisplayObject("myBox")
            local stage = Core:getStageHandle()
            local inStage = false
            if stage then
                inStage = stage:get():hasChild(boxHandle)
            end
            return not inFactory and not inStage
        end
        return false
    )").get<bool>();
    return UnitTests::run("Lua: test # 3", "Destroy Box and verify removal", [destroyed]() { return destroyed; });
}

bool test4() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    bool ok = lua.script(R"(
        local h = Core:createDisplayObject("Box", { name = "tempBox", type = "Box", x = 1, y = 1, width = 16, height = 16 })
        if not h then return false end
        local stage = Core:getStageHandle()
        if not stage then return false end
        stage:addChild(h)
        local wasAdded = stage:hasChild(h)
        Core:destroyDisplayObject("tempBox")
        return wasAdded
    )").get<bool>();
    return UnitTests::run("Lua: test # 4", "Lua binding roundtrip add/hasChild", [ok]() { return ok; });
}

bool test5() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    auto pfr = lua.script(R"(
        local names = nil
        if Core.getPropertyNamesForType then
            names = Core:getPropertyNamesForType("blueishBox")
        elseif Core.getFactory and Core.getFactory().getPropertyNamesForType then
            names = Core:getFactory():getPropertyNamesForType("blueishBox")
        elseif Factory and Factory.getPropertyNamesForType then
            names = Factory:getPropertyNamesForType("blueishBox")
        end
        if names then
            local count = #names
            -- print(string.format("blueishBox property count: %d", count))
            if count > 2 then return true end
        end
        return false
    )");
    bool ok = pfr.get<bool>();
    return UnitTests::run("Lua: test # 5", "Verify getPropertyNamesForType(blueishBox)", [=]() { return ok; });
}

bool test6_Lua() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    auto pfr = lua.script(R"(
        local names = nil
        if Core.getFunctionNamesForType then
            names = Core:getFunctionNamesForType("blueishBox")
        elseif Core.getFactory and Core.getFactory().getFunctionNamesForType then
            names = Core:getFactory():getFunctionNamesForType("blueishBox")
        elseif Factory and Factory.getFunctionNamesForType then
            names = Factory:getFunctionNamesForType("blueishBox")
        end
        if names then
            local count = #names
            -- print(string.format("blueishBox function count: %d", count))
            if count > 0 then
                for i, n in ipairs(names) do
                    -- print(string.format("  %d: %s", i, n))
                end
                return true
            end
        end
        return false
    )");
    bool ok = pfr.get<bool>();
    return UnitTests::run("Lua: test # 6", "Verify getFunctionNamesForType(blueishBox)", [=]() { return ok; });
}

bool test7_Lua() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    auto pfr = lua.script(R"(
        local names = nil
        if Core.getCommandNamesForType then
            names = Core:getCommandNamesForType("blueishBox")
        elseif Core.getFactory and Core.getFactory().getCommandNamesForType then
            names = Core:getFactory():getCommandNamesForType("blueishBox")
        elseif Factory and Factory.getCommandNamesForType then
            names = Factory:getCommandNamesForType("blueishBox")
        end
        if names then
            local count = #names
            -- print(string.format("blueishBox command count: %d", count))
            -- Consider passing if zero or more; require >= 0 always true, but check API returns a table
            return true
        end
        return false
    )");
    bool ok = pfr.get<bool>();
    return UnitTests::run("Lua: test # 7", "Verify getCommandNamesForType(blueishBox)", [=]() { return ok; });
}

bool test8_Lua() {
    // This test synthesizes an SDL mouse down + up sequence at the center of blueishBox
    // and pumps events once to cause the EventManager to generate a MouseClick which
    // should be caught by Box's listener and increment its test counter.
    using namespace SDOM;
    // reset counter
    Box::resetTestClickCount();

    // Determine blueishBox center from the object in the factory
    DomHandle box = Core::getInstance().getDisplayHandle("blueishBox");
    if (!box) return UnitTests::run("Lua: test # 8", "Verify synthetic MouseClick triggers Box listener", [](){ return false; });

    int cx = box->getX() + box->getWidth() / 2;
    int cy = box->getY() + box->getHeight() / 2;

    // Build SDL mouse down
    SDL_Event down{};
    down.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    down.button.windowID = Core::getInstance().getWindow() ? SDL_GetWindowID(Core::getInstance().getWindow()) : 0;
    down.button.button = SDL_BUTTON_LEFT;
    down.button.clicks = 1;
    // Convert from stage/render coords to window coords so SDL_ConvertEventToRenderCoordinates
    // inside EventManager will map them back to the intended render coordinates.
    int winX = static_cast<int>(cx * Core::getInstance().getConfig().pixelWidth);
    int winY = static_cast<int>(cy * Core::getInstance().getConfig().pixelHeight);
    // Synthesize coordinates in both the button and motion unions to be robust
    down.button.x = winX;
    down.button.y = winY;
    down.motion.x = winX;
    down.motion.y = winY;

    // Build SDL mouse up
    SDL_Event up{};
    up.type = SDL_EVENT_MOUSE_BUTTON_UP;
    up.button.windowID = down.button.windowID;
    up.button.button = SDL_BUTTON_LEFT;
    up.button.clicks = 1;
    // Use the same scaled window coordinates for the up event
    up.button.x = winX;
    up.button.y = winY;
    up.motion.x = winX;
    up.motion.y = winY;

    // Push events and pump
    SDL_PushEvent(&down);
    SDL_PushEvent(&up);

    // process queued events once
    Core::getInstance().pumpEventsOnce();

    int count = Box::getTestClickCount();
    bool ok = (count > 0);
    return UnitTests::run("Lua: test # 8", "Verify synthetic MouseClick triggers Box listener", [=]() { return ok; });
} //test8_lua()

bool test9_Lua() {
    // This test will register an event listener purely from Lua and synthesize
    // a mouse click from Lua using Core.pushMouseEvent / Core.pumpEventsOnce.
    sol::state& lua = SDOM::Core::getInstance().getLua();

    // Ensure Box test counter reset
    Box::resetTestClickCount();

    // Ensure blueishBox exists and is on the stage
    DomHandle box = Core::getInstance().getDisplayHandle("blueishBox");
    if (!box) return UnitTests::run("Lua: test # 9", "Lua-only event handler + synthetic click", [](){ return false; });

    // Build and run Lua script that attaches a listener and synthesizes a click
    auto result = lua.script(R"(
        local clicked = false
        local bh = Core:getDisplayHandle("blueishBox")
        if not bh then return false end
        -- register listener on the box
        bh:addEventListener({ type = EventType.MouseClick, listener = function(e) clicked = true end })

    -- compute center in stage coords (use DomHandle forwarded methods)
    local x = bh:getX() + bh:getWidth() / 2
    local y = bh:getY() + bh:getHeight() / 2

        -- push down/up and pump from Lua
        Core:pushMouseEvent({ x = x, y = y, type = "down", button = 1 })
        Core:pushMouseEvent({ x = x, y = y, type = "up", button = 1 })
        Core:pumpEventsOnce()

        return clicked
    )").get<bool>();

    bool ok = result;
    return UnitTests::run("Lua: test # 9", "Lua-only event handler + synthetic click", [=]() { return ok; });
}

bool LUA_UnitTests() {
    bool allTestsPassed = true;
    std::vector<std::function<bool()>> tests = {
        [&]() { return test1(); },
        [&]() { return test2(); },
        [&]() { return test3(); },
        [&]() { return test4(); },
        [&]() { return test5(); },
        [&]() { return test6_Lua(); },
        [&]() { return test7_Lua(); },
        [&]() { return test8_Lua(); },
        [&]() { return test9_Lua(); }
    };
    for (auto& test : tests) {
        bool testResult = test();
        allTestsPassed &= testResult;
    }
    return allTestsPassed;
}

} // namespace SDOM