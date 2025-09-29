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

bool test1_Lua() {
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

bool test2_Lua() {
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

bool test3_Lua() {
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

bool test4_Lua() {
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

bool test5_Lua() {
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

        -- Click the Stage at 0, 0 to defocus the box
        Core:pushMouseEvent({ x = 0, y = 0, type = "down", button = 1 })
        Core:pushMouseEvent({ x = 0, y = 0, type = "up", button = 1 })
        Core:pumpEventsOnce()

        return clicked
    )").get<bool>();

    // SDOM::getCore().setKeyboardFocusedObject(SDOM::getStageHandle());

    bool ok = result;
    return UnitTests::run("Lua: test # 9", "Lua-only event handler + synthetic click", [=]() { return ok; });
}

// Split the previous step-based test10 into six individual tests for clarity
bool test10_lua()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();
    bool s = lua.script(R"(
        local orig = Core:getStageHandle()
        if not orig or orig:getName() ~= 'mainStage' then return false end
        return true
    )").get<bool>();
    return UnitTests::run("Lua: test #10", "Verify initial stage is 'mainStage'", [=]() { return s; });
}

bool test11_lua()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();
    bool s = lua.script(R"(
        Core:setRootNode('stageTwo')
        local s = Core:getStageHandle()
        if not s or s:getName() ~= 'stageTwo' then return false end
        return true
    )").get<bool>();
    return UnitTests::run("Lua: test #11", "Set root by name to 'stageTwo' and verify", [=]() { return s; });
}

bool test12_lua()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();
    bool s = lua.script(R"(
        local h = Core:getDisplayHandle('stageThree')
        if not h then return false end
        Core:setRootNode(h)
        local cur = Core:getStageHandle()
        if not cur or cur:getName() ~= 'stageThree' then return false end
        return true
    )").get<bool>();
    return UnitTests::run("Lua: test #12", "Set root by handle using 'stageThree' and verify", [=]() { return s; });
}

bool test13_lua()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();
    bool s = lua.script(R"(
        Core:setStage('mainStage')
        local cur = Core:getStageHandle()
        if not cur or cur:getName() ~= 'mainStage' then return false end
        return true
    )").get<bool>();
    return UnitTests::run("Lua: test #13", "Use alias setStage to return to 'mainStage' and verify", [=]() { return s; });
}

bool test14_lua()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();
    bool s = lua.script(R"(
        local orig = Core:getIsTraversing()
        -- toggle
        Core:setIsTraversing(not orig)
        local mid = Core:getIsTraversing()
        if mid ~= (not orig) then return false end
        -- restore
        Core:setIsTraversing(orig)
        local fin = Core:getIsTraversing()
        if fin ~= orig then return false end
        return true
    )").get<bool>();
    return UnitTests::run("Lua: test #14", "Toggle getIsTraversing via setIsTraversing and restore", [=]() { return s; });
}

bool test15_lua()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();
    bool s = lua.script(R"(
        local before = Core:countOrphanedDisplayObjects()
        -- create a temp object and attach it to the stage
        local h = Core:createDisplayObject("Box", { name = "tempOrphan", type = "Box", x = 0, y = 0, width = 8, height = 8 })
        if not h then return false end
        local st = Core:getStageHandle()
        if not st then return false end
        st:addChild(h)
        if not st:hasChild(h) then return false end
        -- ensure orphaning uses the traversing path: toggle traversing on and remove child
        local orig = Core:getIsTraversing()
        Core:setIsTraversing(true)
        st:removeChild(h)
        Core:setIsTraversing(orig)
        local mid = Core:countOrphanedDisplayObjects()
        if mid ~= (before + 1) then return false end
        local orphans = Core:getOrphanedDisplayObjects()
        local found = false
        if orphans then
            for i,o in ipairs(orphans) do
                if o and o:getName() == 'tempOrphan' then found = true break end
            end
        end
        if not found then return false end
        -- cleanup orphans
        Core:destroyOrphanedDisplayObjects()
        local fin = Core:countOrphanedDisplayObjects()
        if fin ~= before then return false end
        return true
    )").get<bool>();
    return UnitTests::run("Lua: test #15", "Orphan lifecycle: create/attach/remove/verify/cleanup", [=]() { return s; });
}

bool test16_lua()
{
    // Test TAB and SHIFT+TAB focus navigation using Lua-synthesized key events
    sol::state& lua = SDOM::Core::getInstance().getLua();

    // Ensure keyboard focus starts at the stage
    Core::getInstance().setKeyboardFocusedObject(Core::getInstance().getStageHandle());

    // Create two boxes from Lua and attach to stage
    bool created = lua.script(R"(
        local a = Core:createDisplayObject("Box", { name = "focusA", type = "Box", x = 10, y = 10, width = 16, height = 16 })
        a:setColor({ r = 128, g = 128, b = 0, a = 255 })  -- make focusA yellow to distinguish
        local b = Core:createDisplayObject("Box", { name = "focusB", type = "Box", x = 30, y = 10, width = 16, height = 16 })
        b:setColor({ r = 0, g = 128, b = 128, a = 255 })  -- make focusB cyan to distinguish
        local st = Core:getStageHandle()
        if not a or not b or not st then return false end
        st:addChild(a)
        st:addChild(b)
        return true
    )").get<bool>();
    if (!created) return UnitTests::run("Lua: test #16", "TAB focus navigation", [](){ return false; });

    // Send Tab (keycode 9) via Lua helper and pump events; verify focus order
    bool result = lua.script(R"(
        -- Helper: get name of keyboard-focused object
        local function focusedName()
            local k = Core:getKeyboardFocusedObject()
            if not k then return nil end
            return k:getName()
        end

        -- Initial focus should be the stage
        local initial = focusedName()

    -- First Tab forward: invoke handler directly so we can test traversal logic
    Core:handleTabKeyPress()
        local after1 = focusedName()

    -- Second Tab forward: invoke handler again
    Core:handleTabKeyPress()
        local after2 = focusedName()

        -- Debug print
        -- print(string.format("[test16] initial=%s after1=%s after2=%s", tostring(initial), tostring(after1), tostring(after2)))
        -- Validate expectations
        if after1 ~= 'focusB' then return false end
        if after2 ~= 'focusA' then return false end
        return true
    )").get<bool>();

    return UnitTests::run("Lua: test #16", "TAB focus navigation", [=]() { return result; });
}

bool test17_lua()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();
    bool ok = lua.script(R"(
        local st = Core:getStageHandle()
        if not st then return false end
        local a = Core:getDisplayHandle('focusA')
        local b = Core:getDisplayHandle('focusB')
        -- Remove if present
        if a and st:hasChild(a) then st:removeChild(a) end
        if b and st:hasChild(b) then st:removeChild(b) end
        -- Verify neither are children
        local stillA = false
        local stillB = false
        if a and st:hasChild(a) then stillA = true end
        if b and st:hasChild(b) then stillB = true end
        return (not stillA) and (not stillB)
    )").get<bool>();

    return UnitTests::run("Lua: test #17", "Remove focusA/focusB from stage and verify", [=]() { return ok; });
}

bool test18_lua()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();
    bool ok = lua.script(R"(
        -- Verify that there are at least two orphaned display objects and that
        -- 'focusA' and 'focusB' are present among them.
        local cnt = Core:countOrphanedDisplayObjects()
        if not cnt or cnt < 2 then return false end
        local orphans = Core:getOrphanedDisplayObjects()
        if not orphans then return false end
        local foundA = false
        local foundB = false
        for i,o in ipairs(orphans) do
            if o and o:getName() == 'focusA' then foundA = true end
            if o and o:getName() == 'focusB' then foundB = true end
        end
        if not (foundA and foundB) then return false end

        -- Destroy the named orphaned objects
        if foundA then Core:destroyDisplayObject('focusA') end
        if foundB then Core:destroyDisplayObject('focusB') end

        -- Recount and ensure they are gone
        local cnt2 = Core:countOrphanedDisplayObjects()
        local orphans2 = Core:getOrphanedDisplayObjects()
        local stillA = false
        local stillB = false
        if orphans2 then
            for i,o in ipairs(orphans2) do
                if o and o:getName() == 'focusA' then stillA = true end
                if o and o:getName() == 'focusB' then stillB = true end
            end
        end
        -- Also check factory no longer has them
        local hasA = Core:hasDisplayObject('focusA')
        local hasB = Core:hasDisplayObject('focusB')

        return (not stillA) and (not stillB) and (not hasA) and (not hasB)
    )").get<bool>();

    return UnitTests::run("Lua: test #18", "Factory has two orphaned objects and focusA/focusB are orphaned", [=]() { return ok; });
}

bool LUA_UnitTests() {
    bool allTestsPassed = true;
    std::vector<std::function<bool()>> tests = {
        [&]() { return test1_Lua(); },
        [&]() { return test2_Lua(); },
        [&]() { return test3_Lua(); },
        [&]() { return test4_Lua(); },
        [&]() { return test5_Lua(); },
        [&]() { return test6_Lua(); },
        [&]() { return test7_Lua(); },
        [&]() { return test8_Lua(); },
        [&]() { return test9_Lua(); },
        [&]() { return test10_lua(); },
        [&]() { return test11_lua(); },
        [&]() { return test12_lua(); },
        [&]() { return test13_lua(); },
        [&]() { return test14_lua(); },
        [&]() { return test15_lua(); },
        [&]() { return test16_lua(); },
        [&]() { return test17_lua(); },
        [&]() { return test18_lua(); }
    };
    for (auto& test : tests) {
        bool testResult = test();
        allTestsPassed &= testResult;
    }
    return allTestsPassed;
}

} // namespace SDOM