#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM {

bool DomHandle_Tab_UnitTest()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();

    bool result = lua.script(R"(
        -- Create a test box with tab settings
        local b = Core:createDisplayObject('Box', { name = 'tab_box', type = 'Box', x = 10, y = 10, width = 20, height = 20, tabPriority = 5, tabEnabled = true })
        if not b then return false end

    -- Verify initial tab priority and enabled
    if b:getTabPriority() ~= 5 then return false end
    if not b:isTabEnabled() then return false end

        -- Change tab priority and enabled state
    b:setTabPriority(2)
    if b:getTabPriority() ~= 2 then return false end

    b:setTabEnabled(false)
    if b:isTabEnabled() then return false end

        -- Restore and cleanup
        b:setTabPriority(5)
        b:setTabEnabled(true)
        Core:destroyDisplayObject('tab_box')
        return true
    )").get<bool>();

    return UnitTests::run("Lua: DomHandle tab wrappers", "Exercise get/set TabPriority and is/setTabEnabled from Lua", [=]() { return result; });
}

} // namespace SDOM
