#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM {

bool DomHandle_Geometry_UnitTest()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();

    bool result = lua.script(R"(
        -- Create a test box
        local b = Core:createDisplayObject('Box', { name = 'geom_box', type = 'Box', x = 5, y = 6, width = 30, height = 40 })
        if not b then return false end

        -- Verify initial getters
        if b:getX() ~= 5 then return false end
        if b:getY() ~= 6 then return false end
        if b:getWidth() ~= 30 then return false end
        if b:getHeight() ~= 40 then return false end

        -- Modify via setters
        b:setX(15)
        b:setY(25)
        b:setWidth(60)
        b:setHeight(80)

        -- Verify updated values
        if b:getX() ~= 15 then return false end
        if b:getY() ~= 25 then return false end
        if b:getWidth() ~= 60 then return false end
        if b:getHeight() ~= 80 then return false end

        -- Cleanup
        Core:destroyDisplayObject('geom_box')
        return true
    )").get<bool>();

    return UnitTests::run("Lua: DomHandle geometry setters", "Exercise setX/setY/setWidth/setHeight and verify getters from Lua", [=]() { return result; });
}

} // namespace SDOM
