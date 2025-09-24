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

    bool IDisplayObject_UnitTests()
    {
        Core& core = getCore();
        Factory* factory = &core.getFactory();
        std::cout << "IDisplayObject_UnitTests() called" << std::endl;
        bool allTestsPassed = true;
        bool testResult;
        std::string dbgStr;

        // Test: Create a generic Stage object via InitStruct
        testResult = UnitTests::run("IDisplayObject", "Create generic Stage object", [factory, &dbgStr]() {
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
        if (!testResult) { std::cout << CLR::indent() << dbgStr << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Test:setName and getName
        testResult = UnitTests::run("IDisplayObject", "Set and Get Name", [factory, &dbgStr]() {
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
        if (!testResult) { std::cout << CLR::indent() << dbgStr << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // Test: setType and getType
        testResult = UnitTests::run("IDisplayObject", "Set and Get Type", [factory, &dbgStr]() {
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
        if (!testResult) { std::cout << CLR::indent() << dbgStr << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;





        // Test: Remove the generic stage object
        testResult = UnitTests::run("IDisplayObject", "Remove generic Stage object", [factory, &dbgStr]() {
            factory->removeResource("genericStage");
            DomHandle stageHandle = factory->getDomHandle("genericStage");
            if (stageHandle) {
                dbgStr = "'genericStage' still exists after removal!";
                return false;
            }
            return true;
        });
        if (!testResult) { std::cout << CLR::indent() << dbgStr << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        return allTestsPassed;
    } // END bool IDisplayObject_UnitTests()
} // END namespace SDOM