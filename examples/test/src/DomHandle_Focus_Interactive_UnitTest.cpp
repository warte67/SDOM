#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM {

// Test Lua-level focus and interactivity wrappers
bool DomHandle_Focus_Interactive_UnitTest()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();

    // Create a small test object via Factory accessible from Lua
    bool result = lua.script(R"(
        -- create a test box
            local b = Core:createDisplayObject('Box', { name = 'li_box', type = 'Box', x = 10, y = 10, width = 20, height = 20, isClickable = true, isEnabled = true, isHidden = false })
        -- Ensure the newly-created object is attached to the stage so hit-testing finds it
            local stage = Core:getStageHandle()
            if stage and b then stage:addChild(b) end
                if not b then return false end

        -- 1) Test setClickable / isClickable
            if not b:isClickable() then print('DEBUG: clickable failed at initial check') return false end
            b:setClickable(false)
            if b:isClickable() then print('DEBUG: clickable failed after setClickable(false)') return false end
            b:setClickable(true)
            if not b:isClickable() then print('DEBUG: clickable failed after setClickable(true)') return false end
        -- clickable passed

        -- 2) Test setEnabled / isEnabled
            if not b:isEnabled() then print('DEBUG: enabled failed at initial check') return false end
            b:setEnabled(false)
            if b:isEnabled() then print('DEBUG: enabled failed after setEnabled(false)') return false end
            b:setEnabled(true)
            if not b:isEnabled() then print('DEBUG: enabled failed after setEnabled(true)') return false end
        -- enabled passed

        -- 3) Test setVisible / isVisible and setHidden / isHidden
            if not b:isVisible() then print('DEBUG: visible failed at initial check') return false end
            b:setVisible(false)
            if b:isVisible() then print('DEBUG: visible failed after setVisible(false)') return false end
            b:setVisible(true)
            if not b:isVisible() then print('DEBUG: visible failed after setVisible(true)') return false end
            b:setHidden(true)
            if not b:isHidden() then print('DEBUG: hidden failed after setHidden(true)') return false end
            b:setHidden(false)
            if b:isHidden() then print('DEBUG: hidden failed after setHidden(false)') return false end
        -- visible/hidden passed

        -- 4) Test keyboard focus: setKeyboardFocus and Core:getKeyboardFocusedObject
            -- Ensure focus starts at stage
            Core:setKeyboardFocusedObject(Core:getStageHandle())
            b:setKeyboardFocus()
            local k = Core:getKeyboardFocusedObject()
            if not k then print('DEBUG: keyboard focus failed: no focused object') return false end
            if k:getName() ~= 'li_box' then print('DEBUG: keyboard focus failed: focused object name=' .. tostring(k:getName())) return false end
        -- keyboard focus passed

        -- 5) Test mouse hover: push a motion event to center of box and pump
            local cx = b:getX() + b:getWidth()/2
            local cy = b:getY() + b:getHeight()/2
            Core:pushMouseEvent({ x = cx, y = cy, type = 'motion' })
            Core:pumpEventsOnce()
            local m = Core:getMouseHoveredObject()
        if not m then return false end
        if m:getName() ~= 'li_box' then return false end

        -- Cleanup: destroy the test object
            Core:destroyDisplayObject('li_box')
        return true
    )").get<bool>();

    return UnitTests::run("Lua: DomHandle focus/interactive", "Exercise focus and interactive wrappers from Lua", [=]() { return result; });
}

} // namespace SDOM
