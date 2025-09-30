#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM {

bool DomHandle_Forwards_Reflection()
{
    sol::state& lua = SDOM::Core::getInstance().getLua();
    // Lua script: check for presence of DomHandle methods related to priority/z-order
    auto res = lua.script(R"(
        local methods = { 'getMaxPriority','getMinPriority','getPriority','setToHighestPriority','setToLowestPriority','sortChildrenByPriority','setPriority','moveToTop','getChildrenPriorities','getZOrder','setZOrder' }
        local dh = Core:getStageHandle()
        if not dh then return { ok = false, err = 'no stage handle' } end
        for i, m in ipairs(methods) do
            local f = dh[m]
            if not f then return { ok = false, err = 'missing '..m } end
            if type(f) ~= 'function' then return { ok = false, err = m..' not callable' } end
        end
        return { ok = true }
    )").get<sol::table>();

    bool ok = res["ok"].get_or(false);
    std::string err = res["err"].get_or(std::string());
    return UnitTests::run("Lua: DomHandle reflection", "Verify DomHandle forwards for priority/z-order exist and are callable", [=]() { return ok; });
}

} // namespace SDOM
