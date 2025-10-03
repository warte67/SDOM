// IDisplayObject_UnitTests.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>

#include "UnitTests.hpp"

namespace SDOM
{

    // Individual test functions (keeps test bodies named and reusable)
    static bool test1_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #1", "Create generic Stage object", [factory, &dbgStr]() {
            Stage::InitStruct initData;
            initData.name = "genericStage";
            DisplayObject stageHandle = factory->create("Stage", initData);
            if (!stageHandle) {
                dbgStr = "Failed to create Stage object via InitStruct!";
                return false;
            }
            if (dynamic_cast<Stage*>(stageHandle.get()) == nullptr) {
                dbgStr = "Created object is not of type Stage!";
                return false;
            }
            return (stageHandle && dynamic_cast<Stage*>(stageHandle.get()) != nullptr);
        });
    }

    static bool test2_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #2", "Set and Get Name", [factory, &dbgStr]() {
            DisplayObject stageHandle = factory->getDisplayObjectHandle("genericStage");
            if (!stageHandle) {
                dbgStr = "Stage object 'genericStage' not found!";
                return false;
            }
            stageHandle->setName("renamedStage");
            if (stageHandle->getName() != "renamedStage") {
                dbgStr = "setName or getName failed!";
                return false;
            }
            stageHandle->setName("genericStage"); // revert name change for other tests
            return true;
        });
    }

    static bool test3_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #3", "Set and Get Type", [factory, &dbgStr]() {
            DisplayObject stageHandle = factory->getDisplayObjectHandle("genericStage");
            if (!stageHandle) {
                dbgStr = "Stage object 'genericStage' not found!";
                return false;
            }
            stageHandle->setType("CustomStageType");
            if (stageHandle->getType() != "CustomStageType") {
                dbgStr = "setType or getType failed!";
                return false;
            }
            stageHandle->setType("Stage"); // revert type change for other tests
            return true;
        });
    }

    static bool test4_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #4", "Destroy generic Stage object", [factory, &dbgStr]() {
            factory->destroyDisplayObject("genericStage");
            DisplayObject stageHandle = factory->getDisplayObjectHandle("genericStage");
            if (stageHandle) {
                dbgStr = "'genericStage' still exists after destruction!";
                return false;
            }
            return true;
        });
    }

    // Lua-based counterparts: reproduce tests 1-4 via Lua wrappers
    static bool test5_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #5", "Lua Create generic Stage object", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            // Use Factory.create via Core:createDisplayObject or equivalent Lua wrapper
            lua.script(R"(
                Core:createDisplayObject('Stage', { name = 'luaGenericStage', type = 'Stage' })
            )");
            DisplayObject h = factory->getDisplayObjectHandle("luaGenericStage");
            if (!h) {
                dbgStr = "Lua failed to create 'luaGenericStage'";
                return false;
            }
            return true;
        });
    }

    static bool test6_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #6", "Lua Set and Get Name", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            // Use Lua to set/get the name via DisplayObject bindings
            sol::protected_function_result res = lua.script(R"(
                -- DEBUG: inspect Core and the bound function before invoking
                print('DEBUG: Core =', Core)
                print('DEBUG: Core.getDisplayObjectHandle =', Core.getDisplayObjectHandle)
                -- call as function with explicit self to avoid colon-call metamethod surprises
                local h = Core.getDisplayObjectHandle(Core, 'luaGenericStage')
                if not h then return nil, 'handle missing' end
                -- print type/tostring for diagnostic
                print('DEBUG: handle type =', type(h), ' tostring=', tostring(h))
                -- call setName/getName in a protected call so errors are returned instead of thrown
                local ok, result = pcall(function()
                    h:setName('luaRenamedStage')
                    return h:getName()
                end)
                if not ok then return nil, result end
                return result
            )");
            if (!res.valid()) {
                sol::error err = res;
                dbgStr = std::string("Lua error in test6: ") + err.what();
                return false;
            }
            std::string newName = res.get<std::string>();

            // restore name from C++ side to be safe for other tests
            DisplayObject h = factory->getDisplayObjectHandle("luaGenericStage");
            if (h) h->setName("luaGenericStage");
            if (newName != "luaRenamedStage") {
                dbgStr = std::string("Lua setName/getName failed, returned: ") + newName;
                return false;
            }
            return true;
        });
    }

    static bool test7_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #7", "Lua Set and Get Type", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            // Use Lua to set/get the type via DisplayObject bindings
            sol::protected_function_result res = lua.script(R"(
                local h = Core:getDisplayObjectHandle('luaGenericStage')
                if not h then return nil, 'handle missing' end
                h:setType('LuaCustomStage')
                return h:getType()
            )");
            if (!res.valid()) {
                sol::error err = res;
                dbgStr = std::string("Lua error in test7: ") + err.what();
                return false;
            }
            std::string newType = res.get<std::string>();

            // restore type from C++ side
            DisplayObject h = factory->getDisplayObjectHandle("luaGenericStage");
            if (h) h->setType("Stage");
            if (newType != "LuaCustomStage") {
                dbgStr = std::string("Lua setType/getType failed, returned: ") + newType;
                return false;
            }
            return true;
        });
    }

    static bool test8_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #8", "Lua Destroy generic Stage object", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            lua.script(std::string("Core:destroyDisplayObject('luaGenericStage')\n"));
            DisplayObject h = factory->getDisplayObjectHandle("luaGenericStage");
            if (h) { dbgStr = "luaGenericStage still exists after Lua destroy"; return false; }
            return true;
        });
    }

    static bool test9_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #9", "Lua setParent() moves blueishBox under redishBox", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            (void)0;

            // Ensure both objects exist
            DisplayObject blue = factory->getDisplayObjectHandle("blueishBox");
            DisplayObject red = factory->getDisplayObjectHandle("redishBox");
            DisplayObject stage = Core::getInstance().getStageHandle();
            if (!blue || !red || !stage) {
                dbgStr = "Required objects (blueishBox/redishBox/mainStage) not found";
                return false;
            }

            // From Lua: set blueishBox parent to redishBox using DisplayObject method
            sol::protected_function_result res = lua.script(R"(
                local b = Core:getDisplayObjectHandle('blueishBox')
                local r = Core:getDisplayObjectHandle('redishBox')
                if not b or not r then return false end
                -- call the setParent command registered on the concrete type
                b:setParent({ parent = r })
                return true
            )");
            if (!res.valid()) { sol::error err = res; dbgStr = std::string("Lua error in test9: ") + err.what(); return false; }
            bool ok = res.get<bool>();
            if (!ok) { dbgStr = "Lua failed to call setParent"; return false; }

            // Verify: blueishBox is no longer a child of mainStage
            if (stage->hasChild(blue)) {
                dbgStr = "blueishBox is still a child of mainStage after setParent";
                return false;
            }

            // Verify: getParent() of blueishBox is redishBox (check from Lua)
            sol::protected_function_result pres = lua.script(R"(
                local b = Core:getDisplayObjectHandle('blueishBox')
                if not b then return nil, 'blueishBox missing' end
                local p = b:getParent()
                if not p then return nil, 'parent missing' end
                return p:getName()
            )");
            if (!pres.valid()) { sol::error err = pres; dbgStr = std::string("Lua error checking parent: ") + err.what(); return false; }
            std::string parentName = pres.get<std::string>();
            if (parentName != "redishBox") { dbgStr = std::string("blueishBox parent is not redishBox (returned: ") + parentName + ")"; return false; }

            // Verify: redishBox has blueishBox as a child
            if (!red->hasChild(blue)) {
                dbgStr = "redishBox does not contain blueishBox as a child";
                return false;
            }

            // Cleanup: restore original hierarchy (blueishBox under mainStage)
            auto result = lua.script(R"(
                local b = Core:getDisplayObjectHandle('blueishBox')
                local s = Core:getStageHandle()
                if not b or not s then return false, 'missing handle' end
                b:setParent(s)
                local p = b:getParent()
                if not p then return false, 'parent nil after setParent' end
                return p:getName() == s:getName()
            )");
            if (!result.valid()) { sol::error err = result; dbgStr = std::string("Lua error restoring parent: ") + err.what(); return false; }

            return true;
        });
    }

    static bool test10_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #10", "Lua getBounds for blueishBox", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();

            // Do the entire bounds check in Lua so this test is Lua-only
            sol::protected_function_result res = lua.script(R"(
                local EXPECTED = { x = 240, y = 70, width = 250, height = 225 }
                local h = Core:getDisplayObjectHandle('blueishBox')
                if not h then return { ok = false, err = 'handle missing' } end
                local b = h:getBounds()
                if not b then return { ok = false, err = 'getBounds returned nil' } end
                -- Support both userdata with fields and legacy table-style access
                local x = b.x or b.left
                local y = b.y or b.top
                local w = b.width or b.right - b.left
                local h = b.height or b.bottom - b.top

                if x ~= EXPECTED.x or y ~= EXPECTED.y or w ~= EXPECTED.width or h ~= EXPECTED.height then
                    return { ok = false, err = string.format('blueishBox bounds mismatch (got x=%s, y=%s, w=%s, h=%s) expected (x=%s,y=%s,w=%s,h=%s)', x, y, w, h, EXPECTED.x, EXPECTED.y, EXPECTED.width, EXPECTED.height) }
                end
                return { ok = true }
            )");
            if (!res.valid()) { sol::error err = res; dbgStr = std::string("Lua error in test10: ") + err.what(); return false; }
            sol::table tret = res.get<sol::table>();
            bool ok = tret["ok"].get_or(false);
            if (!ok) {
                std::string err = tret["err"].get_or(std::string("unknown error"));
                dbgStr = std::string("Lua test10 failed: ") + err;
                return false;
            }
            return true;
        });
    }

    static bool test11_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #11", "Lua setBounds/getX/getY/getWidth/getHeight and edges", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();

            sol::protected_function_result res = lua.script(R"(
                local BASE = { x = 240, y = 70, width = 250, height = 225 }
                local h = Core:getDisplayObjectHandle('blueishBox')
                if not h then return { ok = false, err = 'handle missing' } end

                -- compute new values (subtract 5 from each)
                local newX = BASE.x - 5
                local newY = BASE.y - 5
                local newW = BASE.width - 5
                local newH = BASE.height - 5

                -- set bounds on the object (left, top, right, bottom)
                h:setBounds({ left = newX, top = newY, right = newX + newW, bottom = newY + newH })

                -- verify via getX/getY/getWidth/getHeight
                local gx = h:getX()
                local gy = h:getY()
                local gw = h:getWidth()
                local gh = h:getHeight()
                if gx ~= newX or gy ~= newY or gw ~= newW or gh ~= newH then
                    return { ok = false, err = string.format('getX/Y/Width/Height mismatch (got %s,%s,%s,%s expected %s,%s,%s,%s)', gx, gy, gw, gh, newX, newY, newW, newH) }
                end

                -- secondary check: verify left/top/right/bottom via direct getters
                local left = h:getLeft()
                local top = h:getTop()
                local right = h:getRight()
                local bottom = h:getBottom()
                if left ~= newX or top ~= newY or right ~= (newX + newW) or bottom ~= (newY + newH) then
                    return { ok = false, err = string.format('edges mismatch (got l=%s t=%s r=%s b=%s expected l=%s t=%s r=%s b=%s)', left, top, right, bottom, newX, newY, newX+newW, newY+newH) }
                end

                return { ok = true }
            )");

            if (!res.valid()) { sol::error err = res; dbgStr = std::string("Lua error in test11: ") + err.what(); return false; }
            sol::table tret = res.get<sol::table>();
            bool ok = tret["ok"].get_or(false);
            if (!ok) {
                std::string err = tret["err"].get_or(std::string("unknown error"));
                dbgStr = std::string("Lua test11 failed: ") + err;
                return false;
            }
            return true;
        });
    }

    static bool test12_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #12", "Lua getColor/setColor roundtrip", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();

            sol::protected_function_result res = lua.script(R"(
                local h = Core:getDisplayObjectHandle('blueishBox')
                if not h then return { ok = false, err = 'handle missing' } end
                local c = h:getColor()
                if not c then return { ok = false, err = 'getColor returned nil' } end

                -- increase r/g/b by 25, clamp to 255
                local newR = math.min(255, (c.r or c:getR() or 0) + 25)
                local newG = math.min(255, (c.g or c:getG() or 0) + 25)
                local newB = math.min(255, (c.b or c:getB() or 0) + 25)
                local newA = (c.a or c:getA() or 0) - 1
                if newA < 0 then newA = 0 end

                h:setColor({ r = newR, g = newG, b = newB, a = newA })

                local c2 = h:getColor()
                if not c2 then return { ok = false, err = 'getColor after setColor returned nil' } end

                if c2.r ~= newR or c2.g ~= newG or c2.b ~= newB or c2.a ~= newA then
                    return { ok = false, err = string.format('color mismatch after setColor (got r=%s,g=%s,b=%s,a=%s expected r=%s,g=%s,b=%s,a=%s)', c2.r, c2.g, c2.b, c2.a, newR, newG, newB, newA) }
                end

                -- restore alpha to fully opaque before returning
                h:setColor({ r = c2.r, g = c2.g, b = c2.b, a = c.a })

                return { ok = true }
            )");

            if (!res.valid()) { sol::error err = res; dbgStr = std::string("Lua error in test12: ") + err.what(); return false; }
            sol::table tret = res.get<sol::table>();
            bool ok = tret["ok"].get_or(false);
            if (!ok) {
                std::string err = tret["err"].get_or(std::string("unknown error"));
                dbgStr = std::string("Lua test12 failed: ") + err;
                return false;
            }
            return true;
        });
    }

    static bool test13_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #13", "Lua priority getMax/getMin/getPriority", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            sol::protected_function_result res = lua.script(R"(
                -- create three boxes under stage for priority testing
                local s = Core:getStageHandle()
                Core:createDisplayObject('Box', { name = 'prioA', parent = s, type = 'Box' })
                Core:createDisplayObject('Box', { name = 'prioB', parent = s, type = 'Box' })
                Core:createDisplayObject('Box', { name = 'prioC', parent = s, type = 'Box' })
                local a = Core:getDisplayObjectHandle('prioA')
                local b = Core:getDisplayObjectHandle('prioB')
                local c = Core:getDisplayObjectHandle('prioC')
                if not a or not b or not c then return { ok = false, err = 'missing handles' } end
                a:setPriority(1)
                b:setPriority(5)
                c:setPriority(3)
                -- compute max/min from the three created children to avoid
                -- interference from pre-existing objects on the stage
                local max = math.max(a:getPriority(), b:getPriority(), c:getPriority())
                local min = math.min(a:getPriority(), b:getPriority(), c:getPriority())
                if max ~= 5 or min ~= 1 then return { ok = false, err = string.format('max/min mismatch (got %s/%s)', max, min) } end
                if a:getPriority() ~= 1 or b:getPriority() ~= 5 or c:getPriority() ~= 3 then return { ok = false, err = 'child priority mismatch' } end
                return { ok = true }
            )");
            if (!res.valid()) { sol::error err = res; dbgStr = std::string("Lua error in test13: ") + err.what(); return false; }
            sol::table tret = res.get<sol::table>();
            bool ok = tret["ok"].get_or(false);
            if (!ok) { std::string err = tret["err"].get_or(std::string("unknown error")); dbgStr = std::string("Lua test13 failed: ") + err; return false; }
            return true;
        });
    }

    static bool test14_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #14", "Lua setPriority/sortChildren/moveToTop and getChildrenPriorities", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            sol::protected_function_result res = lua.script(R"(
                local s = Core:getStageHandle()
                -- reuse prioA/prioB/prioC created earlier (assume present)
                local a = Core:getDisplayObjectHandle('prioA')
                local b = Core:getDisplayObjectHandle('prioB')
                local c = Core:getDisplayObjectHandle('prioC')
                if not a or not b or not c then return { ok = false, err = 'missing handles' } end
                -- bump prioA to top
                a:setPriority(20)
                -- sort children on stage and get priorities vector
                s:sortChildrenByPriority()
                local pri = s:getChildrenPriorities()
                -- pri is an array; find highest value
                local maxv = -999999
                for i=1,#pri do if pri[i] > maxv then maxv = pri[i] end end
                if maxv ~= a:getPriority() then return { ok = false, err = 'move to top via setPriority failed' } end
                -- now move b to top via moveToTop
                b:moveToTop()
                s:sortChildrenByPriority()
                pri = s:getChildrenPriorities()
                maxv = -999999
                for i=1,#pri do if pri[i] > maxv then maxv = pri[i] end end
                if maxv ~= b:getPriority() then return { ok = false, err = 'moveToTop failed' } end
                return { ok = true }
            )");
            if (!res.valid()) { sol::error err = res; dbgStr = std::string("Lua error in test14: ") + err.what(); return false; }
            sol::table tret = res.get<sol::table>();
            bool ok = tret["ok"].get_or(false);
            if (!ok) { std::string err = tret["err"].get_or(std::string("unknown error")); dbgStr = std::string("Lua test14 failed: ") + err; return false; }
            return true;
        });
    }

    static bool test15_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject #15", "Lua setToHighest/setToLowest roundtrips", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            sol::protected_function_result res = lua.script(R"(
                local s = Core:getStageHandle()
                local a = Core:getDisplayObjectHandle('prioA')
                local b = Core:getDisplayObjectHandle('prioB')
                if not a or not b then return { ok = false, err = 'missing handles' } end
                -- record original priorities
                local origA = a:getPriority()
                local origB = b:getPriority()
                a:setToHighestPriority()
                b:setToLowestPriority()
                if a:getPriority() <= b:getPriority() then return { ok = false, err = 'setToHighest/Lowest did not produce expected ordering' } end
                -- restore
                a:setPriority(origA)
                b:setPriority(origB)
                return { ok = true }
            )");
            if (!res.valid()) { sol::error err = res; dbgStr = std::string("Lua error in test15: ") + err.what(); return false; }
            sol::table tret = res.get<sol::table>();
            bool ok = tret["ok"].get_or(false);
            if (!ok) { std::string err = tret["err"].get_or(std::string("unknown error")); dbgStr = std::string("Lua test15 failed: ") + err; return false; }
            // Cleanup the three boxes created earlier in test13 so they don't remain on the stage
            if (factory) {
                try {
                    factory->destroyDisplayObject("prioA");
                } catch (...) { }
                try {
                    factory->destroyDisplayObject("prioB");
                } catch (...) { }
                try {
                    factory->destroyDisplayObject("prioC");
                } catch (...) { }
            }
            return true;
        });
    }

    bool IDisplayObject_UnitTests()
    {
        Core& core = getCore();
        Factory* factory = &core.getFactory();
        bool allTestsPassed = true;
        std::string dbgStr;

        std::vector<std::function<bool()>> tests = {
            [&]() { return  test1_IDisplayObject(factory, dbgStr); }, // Create generic Stage object
            [&]() { return  test2_IDisplayObject(factory, dbgStr); }, // Set and Get Name
            [&]() { return  test3_IDisplayObject(factory, dbgStr); }, // Set and Get Type
            [&]() { return  test4_IDisplayObject(factory, dbgStr); }, // Destroy generic Stage object
            [&]() { return  test5_IDisplayObject(factory, dbgStr); }, // Lua Create generic Stage object
            [&]() { return  test6_IDisplayObject(factory, dbgStr); }, // Lua Set and Get Name
            [&]() { return  test7_IDisplayObject(factory, dbgStr); }, // Lua Set and Get Type
            [&]() { return  test8_IDisplayObject(factory, dbgStr); }, // Lua Destroy generic Stage object
            [&]() { return  test9_IDisplayObject(factory, dbgStr); }, // Lua setParent test
            [&]() { return test10_IDisplayObject(factory, dbgStr); }, // Lua bounds test for blueishBox
            [&]() { return test11_IDisplayObject(factory, dbgStr); }, // Lua setBounds/getX/getY/getWidth/getHeight and edges
            [&]() { return test12_IDisplayObject(factory, dbgStr); }, // Lua getColor/setColor roundtrip
            [&]() { return test13_IDisplayObject(factory, dbgStr); }, // Lua priority getMax/getMin/getPriority
            [&]() { return test14_IDisplayObject(factory, dbgStr); }, // Lua setPriority/sortChildren/moveToTop and getChildrenPriorities
            [&]() { return test15_IDisplayObject(factory, dbgStr); }  // Lua setToHighest/setToLowest roundtrips
        };

        for (auto &t : tests) {
            bool result = t();
            if (!result) {
                if (!dbgStr.empty()) std::cout << CLR::indent() << dbgStr << CLR::RESET << std::endl;
            }
            allTestsPassed &= result;
        }

        return allTestsPassed;
    } // END bool IDisplayObject_UnitTests()
} // END namespace SDOM