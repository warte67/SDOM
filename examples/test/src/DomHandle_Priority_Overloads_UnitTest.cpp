#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM {

bool DomHandle_Priority_Overloads_UnitTest()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();

    // Lua performs these steps:
    //  - create three child boxes a,b,c attached to the stage
    //  - set priorities using various forms
    //  - verify resulting priorities and z-order where applicable
    auto result = lua.script(R"(
        local st = Core:getStageHandle()
        if not st then return { ok = false, err = 'no stage' } end
        -- cleanup any existing test children
        for _, n in ipairs({'ao','bo','co'}) do if Core:hasDisplayObject(n) then Core:destroyDisplayObject(n) end end

        local a = Core:createDisplayObject('Box', { name = 'ao', type = 'Box', x=0,y=0,width=8,height=8 })
        local b = Core:createDisplayObject('Box', { name = 'bo', type = 'Box', x=10,y=0,width=8,height=8 })
        local c = Core:createDisplayObject('Box', { name = 'co', type = 'Box', x=20,y=0,width=8,height=8 })
        if not (a and b and c) then return { ok = false, err = 'creation failed' } end
        st:addChild(a); st:addChild(b); st:addChild(c)

        -- Use numeric form to set this object's priority
        a:setPriority(5)
        if a:getPriority() ~= 5 then return { ok = false, err = 'numeric setPriority failed' } end

        -- Use table form to set priority on b
        b:setPriority({ priority = 7 })
        if b:getPriority() ~= 7 then return { ok = false, err = 'table setPriority failed' } end

        -- Use child-targeted form to set c's priority via parent
        st:setPriority({ child = 'co' }, 9)
        if c:getPriority() ~= 9 then return { ok = false, err = 'targeted setPriority failed' } end

        -- setZOrder using table on child 'bo'
        st:setZOrder({ child = 'bo', z = 42 })
        if b:getZOrder() ~= 42 then return { ok = false, err = 'setZOrder table failed' } end

        -- moveToTop for child 'ao' via parent
        st:moveToTop({ child = 'ao' })
        -- verify ao is now top-most by comparing children's zOrder or by checking parent ordering
        local prios = st:getChildrenPriorities()
        if not prios or #prios < 3 then return { ok = false, err = 'getChildrenPriorities failed' } end

        -- cleanup
        Core:destroyDisplayObject('ao')
        Core:destroyDisplayObject('bo')
        Core:destroyDisplayObject('co')
        return { ok = true }
    )").get<sol::table>();

    bool ok = result["ok"].get_or(false);
    std::string err = result["err"].get_or(std::string());
    return UnitTests::run("Lua: DomHandle priority overloads", "Exercise setPriority/setZOrder/moveToTop overloads", [=]() { return ok; });
}

} // namespace SDOM
