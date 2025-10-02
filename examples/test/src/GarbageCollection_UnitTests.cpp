// GarbageCollection_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>

#include "UnitTests.hpp"


namespace SDOM
{

    bool GarbageCollection_test0()
    {
        // GC Scaffold: Garbage Collection Unit Test Scaffolding
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool ok = false;
        try {
            // run a simple Lua chunk that returns a boolean and read it safely
            auto result = lua.script(R"(
                return true
            )");
            ok = result.get<bool>();
        } catch (const sol::error& e) {
            // log the Lua error and mark test as failed
            std::cerr << "[GC Scaffold] Lua error: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("GC #0", "Garbage Collection Unit Test Scaffolding", [=]() { return ok; });
    }

    // Verify AutoDestroy orphan policy immediately destroys an object when removed
    bool GarbageCollection_test1()
    {
        return UnitTests::run("GC #1", "AutoDestroy removes and destroys child immediately when not traversing", [&]() 
        {
            Core& core = getCore();
            Factory& factory = getFactory();
            // Ensure we're not in traversal so AutoDestroy will apply immediately
            core.setIsTraversing(false);

            // Create a temporary stage and box for the test
            Stage::InitStruct initData;
            initData.name = "genericStage";
            DomHandle stage = factory.create("Stage", initData);
            if (!stage) {
                std::cerr << "\nFailed to create stage for GC test" << std::endl;
                return false;
            }

            // Create a box to be added/removed
            IDisplayObject::InitStruct boxInit;
            boxInit.name = "gc_autodestroy_box";
            boxInit.type = "Box";
            boxInit.x = 100;
            boxInit.y = 100;
            boxInit.width = 50;
            boxInit.height = 50;
            DomHandle box = factory.create("Box", boxInit);
            if (!box.isValid()) {
                std::cerr << "\nFailed to create box for GC test" << std::endl;
                return false;
            }

            // Set the orphan policy to AutoDestroy
            box->setOrphanPolicy(IDisplayObject::OrphanRetentionPolicy::AutoDestroy);

            // Attach the box to the stage
            stage->addChild(box);
            if (!stage->hasChild(box)) { std::cerr << "\nFailed to add child" << std::endl; return false; }

            // Remove the child - AutoDestroy should cause immediate destruction
            stage->removeChild(box);

            // Force garbage collection
            factory.garbageCollection();  

            // After removal, factory should not have the object
            DomHandle maybe = factory.getDomHandle("gc_autodestroy_box");
            if (maybe.isValid()) 
            {
                std::cerr << "\nObject still present after AutoDestroy removal" << std::endl;
                return false;
            }
            return true;
        });
    }

    bool GarbageCollection_test2()
    {
        // GC Scaffold: Garbage Collection Unit Test Scaffolding
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool ok = false;
        try {
            // run a simple Lua chunk that returns a boolean and read it safely
            auto result = lua.script(R"(

                -- Create a stage
                Core:createDisplayObject('Stage', { name = 'genericStage', type = 'Stage' })
                local s =  Core:getDisplayObjectHandle('genericStage')
                if not s then
                    print("Failed to create or get stage")
                    return false
                end

                -- Create a box with minimal properties
                Core:createDisplayObject('Box', { name = 'gc_autodestroy_box', parent = s, type = 'Box' })
                local b = Core:getDisplayObjectHandle('gc_autodestroy_box')
                if not b then
                    print("Failed to create or get box")
                    return false
                end

                -- Add the box to the stage
                s:addChild(b)
                if not s:hasChild(b) then
                    print("Failed to add box to stage")
                    return false
                end

                -- -- Set orphan policy to AutoDestroy
                -- b:setOrphanPolicy(1)  -- AutoDestroy
                -- if b:getOrphanPolicy() ~= 1 then
                --     print("Failed to set orphan policy to AutoDestroy")
                --     return false
                -- end

                -- -- Remove the box from the stage - should be destroyed immediately
                -- s:removeChild(b)

                -- -- Force garbage collection
                -- -- Core:getFactory():garbageCollection()

                -- -- Verify the box has been destroyed
                -- local maybe = Core:getDisplayObjectHandle('gc_autodestroy_box')
                -- if maybe then
                --     print("Box still exists after AutoDestroy removal")
                --     return false
                -- end

                return false
            )");
            ok = result.get<bool>();
        } catch (const sol::error& e) {
            // log the Lua error and mark test as failed
            std::cerr << "[GC Scaffold] Lua error: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("GC #0", "Lua Garbage Collection Test", [=]() { return ok; });
    }

    bool GarbageCollection_UnitTests() 
    {       
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
            [&]() { return GarbageCollection_test0(); },    // Garbage Collection Unit Test Scaffolding
            [&]() { return GarbageCollection_test1(); },    // Verify AutoDestroy orphan policy
            [&]() { return GarbageCollection_test2(); }     // Placeholder for future GC tests
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

} // END namespace SDOM