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
        return UnitTests::run("IDisplayObject: test # 1", "Create generic Stage object", [factory, &dbgStr]() {
            Stage::InitStruct initData;
            initData.name = "genericStage";
            DomHandle stageHandle = factory->create("Stage", initData);
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
        return UnitTests::run("IDisplayObject: test # 2", "Set and Get Name", [factory, &dbgStr]() {
            DomHandle stageHandle = factory->getDomHandle("genericStage");
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
        return UnitTests::run("IDisplayObject: test # 3", "Set and Get Type", [factory, &dbgStr]() {
            DomHandle stageHandle = factory->getDomHandle("genericStage");
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
        return UnitTests::run("IDisplayObject: test # 4", "Destroy generic Stage object", [factory, &dbgStr]() {
            factory->destroyDisplayObject("genericStage");
            DomHandle stageHandle = factory->getDomHandle("genericStage");
            if (stageHandle) {
                dbgStr = "'genericStage' still exists after destruction!";
                return false;
            }
            return true;
        });
    }

    // Lua-based counterparts: reproduce tests 1-4 via Lua wrappers
    static bool test5_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject: test # 5", "Lua Create generic Stage object", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            // Use Factory.create via Core:createDisplayObject or equivalent Lua wrapper
            lua.script(R"(
                Core:createDisplayObject('Stage', { name = 'luaGenericStage', type = 'Stage' })
            )");
            DomHandle h = factory->getDomHandle("luaGenericStage");
            if (!h) {
                dbgStr = "Lua failed to create 'luaGenericStage'";
                return false;
            }
            return true;
        });
    }

    static bool test6_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject: test # 6", "Lua Set and Get Name", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            // Use Lua to set/get the name via DomHandle bindings
            sol::protected_function_result res = lua.script(R"(
                local h = Core:getDisplayHandle('luaGenericStage')
                if not h then return nil, 'handle missing' end
                h:setName('luaRenamedStage')
                return h:getName()
            )");
            if (!res.valid()) {
                sol::error err = res;
                dbgStr = std::string("Lua error in test6: ") + err.what();
                return false;
            }
            std::string newName = res.get<std::string>();

            // restore name from C++ side to be safe for other tests
            DomHandle h = factory->getDomHandle("luaGenericStage");
            if (h) h->setName("luaGenericStage");
            if (newName != "luaRenamedStage") {
                dbgStr = std::string("Lua setName/getName failed, returned: ") + newName;
                return false;
            }
            return true;
        });
    }

    static bool test7_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject: test # 7", "Lua Set and Get Type", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            // Use Lua to set/get the type via DomHandle bindings
            sol::protected_function_result res = lua.script(R"(
                local h = Core:getDisplayHandle('luaGenericStage')
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
            DomHandle h = factory->getDomHandle("luaGenericStage");
            if (h) h->setType("Stage");
            if (newType != "LuaCustomStage") {
                dbgStr = std::string("Lua setType/getType failed, returned: ") + newType;
                return false;
            }
            return true;
        });
    }

    static bool test8_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject: test # 8", "Lua Destroy generic Stage object", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            lua.script(std::string("Core:destroyDisplayObject('luaGenericStage')\n"));
            DomHandle h = factory->getDomHandle("luaGenericStage");
            if (h) { dbgStr = "luaGenericStage still exists after Lua destroy"; return false; }
            return true;
        });
    }

    static bool test9_IDisplayObject(Factory* factory, std::string& dbgStr) {
        return UnitTests::run("IDisplayObject: test # 9", "Lua setParent() moves blueishBox under redishBox", [factory, &dbgStr]() {
            sol::state& lua = Core::getInstance().getLua();
            (void)0;

            // Ensure both objects exist
            DomHandle blue = factory->getDomHandle("blueishBox");
            DomHandle red = factory->getDomHandle("redishBox");
            DomHandle stage = Core::getInstance().getStageHandle();
            if (!blue || !red || !stage) {
                dbgStr = "Required objects (blueishBox/redishBox/mainStage) not found";
                return false;
            }

            // From Lua: set blueishBox parent to redishBox using DomHandle method
            sol::protected_function_result res = lua.script(R"(
                local b = Core:getDisplayHandle('blueishBox')
                local r = Core:getDisplayHandle('redishBox')
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

            // Verify: getParent() of blueishBox is redishBox
            DomHandle blueParent = blue->getParent();
            if (!blueParent || blueParent.getName() != "redishBox") {
                dbgStr = "blueishBox parent is not redishBox";
                return false;
            }

            // Verify: redishBox has blueishBox as a child
            if (!red->hasChild(blue)) {
                dbgStr = "redishBox does not contain blueishBox as a child";
                return false;
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
            [&]() { return test1_IDisplayObject(factory, dbgStr); }, // Create generic Stage object
            [&]() { return test2_IDisplayObject(factory, dbgStr); }, // Set and Get Name
            [&]() { return test3_IDisplayObject(factory, dbgStr); }, // Set and Get Type
            [&]() { return test4_IDisplayObject(factory, dbgStr); }, // Destroy generic Stage object
            [&]() { return test5_IDisplayObject(factory, dbgStr); }, // Lua Create generic Stage object
            [&]() { return test6_IDisplayObject(factory, dbgStr); }, // Lua Set and Get Name
            [&]() { return test7_IDisplayObject(factory, dbgStr); }, // Lua Set and Get Type
            [&]() { return test8_IDisplayObject(factory, dbgStr); }, // Lua Destroy generic Stage object
            [&]() { return test9_IDisplayObject(factory, dbgStr); }  // Lua setParent test
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