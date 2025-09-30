#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM {

bool DomHandle_SortAndHighestLowest_UnitTest()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();

    auto res = lua.script(R"(
        local st = Core:getStageHandle()
        if not st then return { ok = false, err = 'no stage' } end
        -- cleanup
        for _, n in ipairs({'s1','s2','s3'}) do if Core:hasDisplayObject(n) then Core:destroyDisplayObject(n) end end
        local a = Core:createDisplayObject('Box', { name = 's1', type = 'Box', x=0,y=0,width=8,height=8 })
        local b = Core:createDisplayObject('Box', { name = 's2', type = 'Box', x=10,y=0,width=8,height=8 })
        local c = Core:createDisplayObject('Box', { name = 's3', type = 'Box', x=20,y=0,width=8,height=8 })
        st:addChild(a); st:addChild(b); st:addChild(c)
        -- assign mixed priorities
        a:setPriority(10)
        b:setPriority(5)
        c:setPriority(7)
        -- sort children
        st:sortChildrenByPriority()
        local pr = st:getChildrenPriorities()
        -- Filter out zero entries (may be other children on the stage producing zeros).
        local nonzero = {}
        for i,v in ipairs(pr) do if v and v ~= 0 then table.insert(nonzero, v) end end
        if #nonzero < 3 then return { ok = false, err = 'not enough nonzero children priorities' } end
        -- after sort ascending the last three should be 5,7,10
        local n = #nonzero
        if not (nonzero[n-2] == 5 and nonzero[n-1] == 7 and nonzero[n] == 10) then
            local s = ''
            for i,v in ipairs(nonzero) do s = s .. tostring(v) .. ',' end
            return { ok = false, err = 'initial sort failed, nonzero=' .. s }
        end

        -- setToHighest on child 's2' via parent
        st:setToHighestPriority({ child = 's2' })
        -- now priorities should be something like [7,10,11] or similar depending on implementation, but ensure s2 is highest
    st:sortChildrenByPriority()
    local pr2 = st:getChildrenPriorities()
    -- compute max among nonzero entries
    local maxv = nil
    for i,v in ipairs(pr2) do if v and v ~= 0 then if not maxv or v > maxv then maxv = v end end end
    if not maxv then return { ok = false, err = 'no nonzero priorities after setToHighest' } end
    if not (maxv == Core:getDisplayObjectHandle('s2'):getPriority()) then return { ok = false, err = 'setToHighest failed' } end

        -- setToLowest on child 's3' via parent
        st:setToLowestPriority({ child = 's3' })
    st:sortChildrenByPriority()
    local pr3 = st:getChildrenPriorities()
    -- compute min among nonzero entries
    local minv = nil
    for i,v in ipairs(pr3) do if v and v ~= 0 then if not minv or v < minv then minv = v end end end
    if not minv then return { ok = false, err = 'no nonzero priorities after setToLowest' } end
    if not (minv == Core:getDisplayObjectHandle('s3'):getPriority()) then return { ok = false, err = 'setToLowest failed' } end

        -- cleanup
        Core:destroyDisplayObject('s1')
        Core:destroyDisplayObject('s2')
        Core:destroyDisplayObject('s3')
        return { ok = true }
    )").get<sol::table>();

    bool ok = res["ok"].get_or(false);
    std::string err = res["err"].get_or(std::string());
    if (!ok) {
        std::cout << "[Debug] DomHandle_SortAndHighestLowest_UnitTest Lua error: " << err << std::endl;
    }
    return UnitTests::run("Lua: sort/highest/lowest overloads", "Verify sortChildrenByPriority and setToHighest/Lowest with child specs", [=]() { return ok; });
}

} // namespace SDOM
