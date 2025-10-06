// GarbageCollection_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_DisplayObject.hpp>

#include "Box.hpp"
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

    // Create then destroy a display object via the Factory and verify it no longer exists
    bool GarbageCollection_test1()
    {
        return UnitTests::run("GC #1", "Factory create/destroy removes object", [&]() {
            Factory& factory = Core::getInstance().getFactory();
            sol::state lua;
            lua.open_libraries(sol::lib::base);
            lua.script(R"(
                cfg = { type = "Box", name = "gcBox1" }
            )");
            sol::table cfg = lua["cfg"];
            DisplayObject d;
            try { d = factory.create("Box", cfg); } catch (...) { d = nullptr; }
            bool created = (d != nullptr);
            if (!created) {
                // cleanup attempt and fail
                factory.destroyDisplayObject("gcBox1");
                return false;
            }
            // now destroy and ensure it is gone
            factory.destroyDisplayObject("gcBox1");
            DisplayObject check = factory.getDisplayObject("gcBox1");
            return (check == nullptr);
        });
    }    

    // Orphan detection: create a stage and a child, destroy the parent, ensure child is reported orphan and can be destroyed
    bool GarbageCollection_test2()
    {
        return UnitTests::run("GC #2", "Orphan detection and destruction", [&]() 
        {
            // setup
            Factory& factory = Core::getInstance().getFactory();

            // create a stage
            Stage::InitStruct stageInit; // stageInit.type was preinitialized to "Stage" by Stage::InitStruct
            stageInit.name = "gcStage";
            DisplayObject stage = factory.create("Stage", stageInit);
            if (!stage) 
            {
                // cleanup and fail
                DEBUG_LOG("Failed to create Stage in GC_test2");
                factory.destroyDisplayObject("gcStage");
                return false;
            }  

            // create a box         
            Box::InitStruct boxInit;  // boxInit.type was preinitialized to "Box" by Box::InitStruct
            boxInit.name = "gcOrphan";
            DisplayObject child = factory.create("Box", boxInit);
            if (!child) 
            {
                // cleanup and fail
                DEBUG_LOG("Failed to create Box in GC_test2");
                factory.destroyDisplayObject("gcOrphan");
                return false;
            }

            // add the child to the stage and verify
            stage->addChild(child);
            if (stage->getChild("gcOrphan") == nullptr) 
            {
                // cleanup and fail
                DEBUG_LOG("Failed to add Box as child to Stage in GC_test2");
                factory.destroyDisplayObject("gcOrphan");
                factory.destroyDisplayObject("gcStage");
                return false;
            }

            // remove the child from the stage and verify
            stage->removeChild(child);
            if (stage->hasChild(child)) 
            {
                // cleanup and fail
                DEBUG_LOG("Failed to remove Box as child from Stage in GC_test2");
                factory.destroyDisplayObject("gcOrphan");
                factory.destroyDisplayObject("gcStage");
                return false;
            }

            // there should be an orphan now
            if (factory.countOrphanedDisplayObjects() == 0)
            {
                DEBUG_LOG("No orphan found in GC_test2");
                factory.destroyDisplayObject("gcOrphan");
                factory.destroyDisplayObject("gcStage");
                return false;
            }

            // there should be an orphan now - make sure 'gcOrphan' is in the list
            if (factory.countOrphanedDisplayObjects() > 0) 
            {
                auto orphans = factory.getOrphanedDisplayObjects();
                bool found = false;
                for (const auto& orphan : orphans) 
                {
                    if (!orphan) continue;
                    if (orphan->getName() == "gcOrphan") { found = true; break; }
                }
                if (!found) 
                {
                    DEBUG_LOG("Orphan 'gcOrphan' not found in GC_test2");
                    factory.destroyDisplayObject("gcOrphan");
                    factory.destroyDisplayObject("gcStage");
                    return false;
                }
            }  
            
            // run garbage collection (RetainUntilManual)
            child->setOrphanRetentionPolicy(IDisplayObject::OrphanRetentionPolicy::RetainUntilManual);
            factory.collectGarbage(); // should NOT remove the orphan (RetainUntilManual)

            // there should still be an orphan - make sure 'gcOrphan' is in the list
            if (factory.countOrphanedDisplayObjects() > 0) 
            {
                auto orphans = factory.getOrphanedDisplayObjects();
                bool found = false;
                for (const auto& orphan : orphans) 
                {
                    if (!orphan) continue;
                    if (orphan->getName() == "gcOrphan") { found = true; break; }
                }
                if (!found) 
                {
                    DEBUG_LOG("Orphan 'gcOrphan' should still be in GC_test2");
                    factory.destroyDisplayObject("gcOrphan");
                    factory.destroyDisplayObject("gcStage");
                    return false;
                }
            }  

            // run garbage collection (AutoDestroy)
            child->setOrphanRetentionPolicy(IDisplayObject::OrphanRetentionPolicy::AutoDestroy);
            factory.collectGarbage(); // should remove the orphan (AutoDestroy)

            // 'gcOrphan' should no longer be in the orphan list
            if (factory.countOrphanedDisplayObjects() > 0) 
            {
                auto orphans = factory.getOrphanedDisplayObjects();
                bool found = false;
                for (const auto& orphan : orphans) 
                {
                    if (!orphan) continue;
                    if (orphan->getName() == "gcOrphan") { found = true; break; }
                }
                if (found) 
                {
                    DEBUG_LOG("Orphan 'gcOrphan' was not removed by garbage collection in GC_test2");
                    factory.destroyDisplayObject("gcOrphan");
                    factory.destroyDisplayObject("gcStage");
                    return false;
                }
            }          
            
            // make sure the factory no longer includes 'gcOrphan'
            DisplayObject remains = factory.getDisplayObject("gcOrphan");
            if (remains.isValid())
            {
                DEBUG_LOG("Factory still has 'gcOrphan' after garbage collection in GC_test2");
                factory.destroyDisplayObject("gcOrphan");
                factory.destroyDisplayObject("gcStage");
                return false;
            }

            // cleanup
            factory.destroyDisplayObject("gcStage");
            return true;
        });
    }

    // LUA Orphan detection: create a stage and a child, destroy the parent, ensure child is reported orphan and can be destroyed
    bool GarbageCollection_test3()
    {
        return UnitTests::run("GC #3", "LUA Orphan detection and destruction", [&]() {
            sol::state& lua = Core::getInstance().getLua();
            bool ok = false;
            try {
                // provide a timeout value to Lua: ORPHAN_GRACE_PERIOD + 500 ms
                lua["gc_timeout_ms"] = ORPHAN_GRACE_PERIOD + 500;

                auto result = lua.script(R"(
                    -- LUA GC test (mirrors GarbageCollection_test2 but executed from Lua)
                    local function cleanup()
                        pcall(function() Core:destroyDisplayObject("gcOrphanLua") end)
                        pcall(function() Core:destroyDisplayObject("gcStageLua") end)
                    end

                    -- create stage and child
                    local s = Core:createDisplayObject("Stage", { name = "gcStageLua", type = "Stage" })
                    if not s then cleanup()
                        print("Failed to create Stage")
                        return false 
                    end
                    local c = Core:createDisplayObject("Box", { name = "gcOrphanLua", type = "Box" })
                    if not c then 
                        print("Failed to create Box")
                        cleanup() 
                        return false 
                    end

                    -- attach/detach
                    s:addChild(c)
                    if not s:hasChild(c) then 
                        print("Failed to add Box as child to Stage")
                        cleanup()
                        return false 
                    end

                    s:removeChild(c)
                    if s:hasChild(c) then 
                        print("Failed to remove Box as child from Stage")
                        cleanup()
                        return false 
                    end

                    -- orphan should be present
                    if Core:countOrphanedDisplayObjects() == 0 then 
                        print("No orphan found")
                        cleanup() 
                        return false 
                    end

                    -- ensure the orphan list contains our orphan (handle or name)
                    local orphans = Core:getOrphanedDisplayObjects()
                    local found = false
                    for i = 1, #orphans do
                        local o = orphans[i]
                        local name = nil
                        if type(o) == "string" then
                            name = o
                        else
                            -- assume userdata with getName()
                            if type(o.getName) == "function" then
                                name = o:getName()
                            elseif type(o.get_name) == "function" then
                                name = o:get_name() -- defensive fallback
                            end
                        end
                        if name == "gcOrphanLua" then found = true; break end
                    end
                    if not found then 
                        print("Orphan 'gcOrphan' not found")
                        cleanup()
                        return false 
                    end

                    -- RetainUntilManual: orphan should remain after collect
                    -- Use string form ("manual") via Lua binding helper instead of undefined IDisplayObject table
                    c:setOrphanRetentionPolicy("manual")
                    Core:collectGarbage()
                    if Core:countOrphanedDisplayObjects() == 0 then 
                        print("Orphan 'gcOrphan' should still be present")
                        cleanup()
                        return false 
                    end

                    -- AutoDestroy: orphan should be removed after collect
                    -- Use string form ("auto") via Lua binding helper
                    c:setOrphanRetentionPolicy("auto")
                    Core:collectGarbage()

                    -- verify orphan no longer present
                    local stillFound = false
                    orphans = Core:getOrphanedDisplayObjects()
                    for i = 1, #orphans do
                        local o = orphans[i]
                        local name = nil
                        if type(o) == "string" then
                            name = o
                        else
                            if type(o.getName) == "function" then
                                name = o:getName()
                            elseif type(o.get_name) == "function" then
                                name = o:get_name()
                            end
                        end
                        if name == "gcOrphanLua" then stillFound = true; break end
                    end
                    if stillFound then 
                        print("Orphan 'gcOrphan' was not removed by garbage collection")
                        cleanup()
                        return false 
                    end

                    -- ensure factory no longer returns the object (use hasDisplayObject)
                    if Core:hasDisplayObject("gcOrphanLua") then
                        print("Factory still has 'gcOrphan' after garbage collection")
                        cleanup()
                        return false
                    end

                    -- cleanup stage and succeed
                    cleanup()
                    return true
                )");
                ok = result.get<bool>();
            } catch (const sol::error& e) {
                DEBUG_LOG("Lua GC test3 error: ", e.what());
                ok = false;
            }
            return ok;
        });
    }     

    // LUA Orphan grace-period auto-collect: create a child, detach it with GracePeriod policy,
    // wait for ~2 seconds, collect, and verify it was destroyed.
    bool GarbageCollection_test4()
    {
        return UnitTests::run("GC #4", "Orphan grace period auto-collect after delay", [&]() {
            sol::state& lua = Core::getInstance().getLua();
            bool ok = false;
            try {
                auto result = lua.script(R"(
                    local function cleanup()
                        pcall(function() Core:destroyDisplayObject("gcGraceBox") end)
                        pcall(function() Core:destroyDisplayObject("gcStageGrace") end)
                    end

                    -- create stage and child
                    local s = Core:createDisplayObject('Stage', { name = 'gcStageGrace', type = 'Stage' })
                    if not s then cleanup(); return false end
                    local c = Core:createDisplayObject('Box', { name = 'gcGraceBox', type = 'Box' })
                    if not c then cleanup(); return false end

                    -- attach child and set policy
                    s:addChild(c)
                    if not s:hasChild(c) then cleanup(); return false end
                    c:setOrphanRetentionPolicy('grace') -- use string helper; maps to GracePeriod

                    -- temporarily reduce grace period for faster test: save original and set to 500ms
                    local orig_grace = nil
                    if type(c.getOrphanGrace) == 'function' and type(c.setOrphanGrace) == 'function' then
                        orig_grace = c:getOrphanGrace()
                        c:setOrphanGrace(500)
                    end

                    -- detach child; should now be orphaned
                    s:removeChild(c)
                    if s:hasChild(c) then cleanup(); return false end
                    if Core:countOrphanedDisplayObjects() == 0 then cleanup(); return false end

                    -- verify orphan list contains our child
                    local function list_contains(name)
                        local orphans = Core:getOrphanedDisplayObjects()
                        for i = 1, #orphans do
                            local o = orphans[i]
                            local oname = nil
                            if type(o) == 'string' then oname = o else
                                if type(o.getName) == 'function' then oname = o:getName()
                                elseif type(o.get_name) == 'function' then oname = o:get_name() end
                            end
                            if oname == name then return true end
                        end
                        return false
                    end
                    if not list_contains('gcGraceBox') then cleanup(); return false end

                    -- wait until the garbage has been collected or the timeout is reached
                    local function wait_for_collect(name, timeout_ms)
                        local elapsed = 0
                        local step = 100 -- ms per iteration
                        while elapsed <= timeout_ms do
                            Core:collectGarbage()
                            if not Core:hasDisplayObject(name) then return true end
                            SDL:Delay(step)
                            elapsed = elapsed + step
                        end
                        return false
                    end

                    if not wait_for_collect('gcGraceBox', gc_timeout_ms) then
                        -- restore original grace before failing (only if object still exists)
                        if orig_grace and Core:hasDisplayObject('gcGraceBox') then pcall(function() c:setOrphanGrace(orig_grace) end) end
                        cleanup(); return false
                    end

                    -- restore original grace period if the object still exists
                    if orig_grace and Core:hasDisplayObject('gcGraceBox') then pcall(function() c:setOrphanGrace(orig_grace) end) end

                    -- verify orphan is no longer listed and object no longer exists
                    if list_contains('gcGraceBox') then cleanup(); return false end
                    if Core:hasDisplayObject('gcGraceBox') then cleanup(); return false end

                    -- cleanup stage and succeed
                    cleanup()
                    return true
                )");
                ok = result.get<bool>();
            } catch (const sol::error& e) {
                DEBUG_LOG("Lua GC test4 error: ", e.what());
                ok = false;
            }
            return ok;
        });
    }

    bool GarbageCollection_UnitTests() 
    {       
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
            [&]() { return GarbageCollection_test0(); },   // Garbage Collection Unit Test Scaffolding
            [&]() { return GarbageCollection_test1(); },   // Factory create/destroy removes object
            [&]() { return GarbageCollection_test2(); },   // Orphan detection and destruction
            [&]() { return GarbageCollection_test3(); },   // Lua orphan detection and destruction
            [&]() { return GarbageCollection_test4(); }    // Lua grace-period auto-collect after delay

        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

} // END namespace SDOM./prog

