// IDisplayObject_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_EventManager.hpp>

#include "Box.hpp"

namespace SDOM
{
    // --- IDisplayObject Unit Test Scaffolding --- //
    bool IDisplayObject_test0(std::vector<std::string>& errors)   
    {
        // ✅ Test Verified
        // 🔄 In Progress
        // ⬜ Unchecked/Untested

        bool ok = true;

        return ok;
    } // IDisplayObject_test0(std::vector<std::string>& errors)   


    // --- Create generic Stage object --- //
    bool IDisplayObject_test1(std::vector<std::string>& errors)   
    {
        bool ok = true;
        // create a Stage object using the Init Structure
        Stage::InitStruct stageInit;
        stageInit.name = "genericStage";
        DisplayHandle stage = getCore().createDisplayObject("Stage", stageInit);
        if (!stage)
        {
            errors.push_back("Failed to create a Stage object using the Init Structure method.");
            ok = false;
        }

        // Intentionally letting the `genericStage` object remain for future tests
        return ok;
    } // IDisplayObject_test1(std::vector<std::string>& errors)   


    // --- Get and Set Name --- //
    bool IDisplayObject_test2(std::vector<std::string>& errors)   
    {
        bool ok = true;
        DisplayHandle stage = getCore().getDisplayObject("genericStage");
        if (!stage)
        {
            errors.push_back("Stage object 'genericStage' not found for name test.");
            return false;
        }
        std::string new_name = "renamedStage";
        stage->setName(new_name);
        std::string name = stage->getName();
        if (name != new_name)
        {
            errors.push_back("setName or getName failed (expected: '" + new_name + "' got '" + name + "').");
            ok = false;
        }
        // Revert name change for other tests
        stage->setName("genericStage");
        
        return ok;
    } // IDisplayObject_test2(std::vector<std::string>& errors)      
    
    
    // --- Destroy the generic Stage Object --- //
    bool IDisplayObject_test3(std::vector<std::string>& errors)   
    {
        bool ok = true;
        getCore().destroyDisplayObject("genericStage");
        DisplayHandle stage = getCore().getDisplayObject("genericStage");
        if (stage)
        {
            errors.push_back("'genericStage' still exists after destruction!");
            ok = false;
        }        
        return ok;
    } // IDisplayObject_test3(std::vector<std::string>& errors)  


    // --- Dirty/State Management --- //
    bool IDisplayObject_test4(std::vector<std::string>& errors)   
    {
        // --- Test These Dirty Flag Methods --- //
            // ✅ void cleanAll_lua(IDisplayObject* obj);
            // ✅ bool getDirty_lua(const IDisplayObject* obj);
            // ✅ void setDirty_lua(IDisplayObject* obj); 
            // ✅ bool isDirty_lua(const IDisplayObject* obj); 

        // --- Debug/Utility --- //
            // ✅ void printTree_lua(const IDisplayObject* obj);  // Tested Manually while designing these Unit Tests

        bool ok = true;
        Core& core = getCore();
        DisplayHandle stage = core.getDisplayObject("mainStage");
        if (!stage.isValid())
        {
            errors.push_back("IDisplayObject_test4: 'mainStage' object not found for dirty/state test.");
            return false;
        }

        // core.getStage()->printTree();  // debugging aid that shows a tree of DisplayObjects in the DOM tree
        stage->cleanAll();

        // Get a DisplayObject to test
        DisplayHandle blueishBox = core.getDisplayObject("blueishBox");
        if (!blueishBox.isValid()) 
        {
            errors.push_back("IDisplayObject_test4: blueishBox object not found for dirty/state test.");
            return false;
        }
        // bool initial_dirty_box = blueishBox->isDirty();


        // Get a child object to confirm recursive cleaning.
        DisplayHandle label = blueishBox->getChild("blueishBoxLabel");
        if (!label.isValid())
        {
            errors.push_back("IDisplayObject_test4: Expected 'blueishBoxLabel' child to exist.");
            return false;
        }
        // bool initial_dirty_label = label->isDirty();

        // Initially, both objects should be clean
        if (blueishBox->isDirty())
        {
            errors.push_back("IDisplayObject_test4: blueishBox should be clean initially.");
            ok = false;
        }
        if (label->isDirty())
        {
            errors.push_back("IDisplayObject_test4: blueishBoxLabel should be clean initially.");
            ok = false;
        }

        // Mark only the child as dirty
        label->setDirty(true);
        if (!label->isDirty())
        {
            errors.push_back("IDisplayObject_test4: blueishBoxLabel should be dirty after setDirty(true).");
            ok = false;
        }
        if (blueishBox->isDirty())
        {
            errors.push_back("IDisplayObject_test4: blueishBox should still be clean after child setDirty(true).");
            ok = false;
        }

        // Mark the parent as dirty
        blueishBox->setDirty(true);
        if (!blueishBox->isDirty())
        {
            errors.push_back("IDisplayObject_test4: blueishBox should be dirty after setDirty(true).");
            ok = false;
        }
        if (!label->isDirty())
        {
            errors.push_back("IDisplayObject_test4: blueishBoxLabel should still be dirty after parent setDirty(true).");
            ok = false;
        }

        // Recursively scrub away the dirtiness
        core.getStage()->cleanAll();

        // Both objects should now be clean
        if (blueishBox->isDirty())
        {
            errors.push_back("IDisplayObject_test4: blueishBox still dirty after cleanAll().");
            ok = false;
        }
        if (label->isDirty())
        {
            errors.push_back("IDisplayObject_test4: blueishBoxLabel still dirty after cleanAll().");
            ok = false;
        }

        return ok;
    } // IDisplayObject_test4(std::vector<std::string>& errors)   

    
    // --- Events and Event Listener Handling --- //
    bool IDisplayObject_test5(std::vector<std::string>& errors)   
    {
        // --- Event Handling --- //
            // ✅ void addEventListener(EventType& type, std::function<void(Event&)> listener, bool useCapture = false, int priority = 0);
            // ✅ void removeEventListener(EventType& type, std::function<void(Event&)> listener, bool useCapture = false);
            // ✅ void triggerEventListeners(Event& event, bool useCapture);
            // ✅ bool hasEventListeners(const EventType& type, bool useCapture) const;
            // ✅ void queue_event(const EventType& type, std::function<void(Event&)> init_payload);              
        bool ok = true;
        Core& core = getCore();
        DisplayHandle stage = core.getDisplayObject("mainStage");
        if (!stage.isValid()) {
            errors.push_back("EventListener_Test: 'mainStage' not found.");
            return false;
        }
        // Counter to track event invocations
        int counter = 0;
        // Lambda to call by the event listener
        auto the_none_function = [&counter, stage](SDOM::Event& event)
        {
            if (event.getTarget() == stage)
            {
                sol::state_view lua = getLua();
                if (event.getPayload().get_type() == sol::type::table && event.getPayloadValue<std::string>("info") == "test event")
                {
                    // std::string info = event.getPayloadValue<std::string>("info");
                    // std::cout << "EventListener_Test: `None` event listener invoked with payload info: " << info << std::endl;
                    counter++;
                    // std::cout << "EventListener_Test: Counter incremented to " << counter << std::endl;
                }
            }
        };
         // Initially: no listeners
        stage->addEventListener(SDOM::EventType::None, the_none_function, false);
        if (!stage->hasEventListener(SDOM::EventType::None, false)) {
            errors.push_back("EventListener_Test: Expected `None` listener to be registered.");
            ok = false;
        }
        // Queue an event and confirm listener is invoked
        stage->queue_event(SDOM::EventType::None, [&](Event& ev) {
            ev.setPayloadValue("info", "test event");
        });            
        // Test triggerEventListeners()
        Event queuedEvent(SDOM::EventType::None, stage);        
        getCore().pumpEventsOnce();
        // check the counter
        if (counter != 1)
        {
            errors.push_back("EventListener_Test: Expected listener to be invoked once, but counter is " + std::to_string(counter) + ".");
            ok = false;
        }
        // Remove the listener
        stage->removeEventListener(SDOM::EventType::None, the_none_function, false);
        // Verify that the listener is removed
        if (stage->hasEventListener(SDOM::EventType::None, false)) {
            errors.push_back("EventListener_Test: Expected `None` listener to be removed.");
            ok = false;
        }
        // return the final test result
        return ok;
    } // IDisplayObject_test5(std::vector<std::string>& errors)   


    // --- Hierarchy Management --- //
    bool IDisplayObject_test6(std::vector<std::string>& errors)   
    {
        // --- Hierarchy Management --- //
            // ✅ void addChild_lua(IDisplayObject* obj, DisplayHandle child);                
            // ✅ DisplayHandle getChild_lua(const IDisplayObject* obj, std::string name);    
            // ✅ bool removeChild_lua(IDisplayObject* obj, DisplayHandle child);             
            // ✅ bool removeChild_lua(IDisplayObject* obj, const std::string& name);         
            // ✅ bool hasChild_lua(const IDisplayObject* obj, DisplayHandle child);          
            // ✅ DisplayHandle getParent_lua(const IDisplayObject* obj);                     
            // ✅ void setParent_lua(IDisplayObject* obj, const DisplayHandle& parent);       
        // Ancestor/Descendant helpers
            // ✅ bool isAncestorOf_lua(IDisplayObject* obj, DisplayHandle descendant);       
            // ✅ bool isAncestorOf_lua(IDisplayObject* obj, const std::string& name);        
            // ✅ bool isDescendantOf_lua(IDisplayObject* obj, DisplayHandle ancestor);       
            // ✅ bool isDescendantOf_lua(IDisplayObject* obj, const std::string& name);      
        // Remove this object from its parent (convenience). Returns true if removed.
            // ✅ bool removeFromParent_lua(IDisplayObject* obj);                             
        // Recursive descendant removal: search depth-first and remove first match. Returns true if removed.
            // ✅ bool removeDescendant_lua(IDisplayObject* obj, DisplayHandle descendant);   
            // ✅ bool removeDescendant_lua(IDisplayObject* obj, const std::string& descendantName);        
        bool ok = true;
        Core& core = getCore();
        // Factory& factory = core.getFactory();
        DisplayHandle stage = core.getDisplayObject("mainStage");
        if (!stage.isValid()) {
            errors.push_back("HierarchyManagement_Test: 'mainStage' not found.");
            return false;
        }
        
        // Create a Box object
        Box::InitStruct boxInit;
        boxInit.name = "testBox";
        boxInit.x = 10;
        boxInit.y = 10;
        boxInit.width = 100;
        boxInit.height = 75;
        boxInit.color = {255, 0, 255, 255};
        DisplayHandle boxHandle = core.createDisplayObject("Box", boxInit);
        if (!boxHandle.isValid()) {
            errors.push_back("HierarchyManagement_Test: Failed to create 'testBox' object.");
            return false;
        }
        // Add the Box to the Stage
        stage->addChild(boxHandle);
        // verify that stage now has the box as a child
        if (!stage->hasChild(boxHandle))
        {
            errors.push_back("HierarchyManagement_Test: 'mainStage' should have 'testBox' as a child after addChild.");
            ok = false;
        }
        // Verify Get Child By Name
        DisplayHandle fetchedBox = stage->getChild("testBox");
        if (!fetchedBox.isValid() || fetchedBox != boxHandle)
        {
            errors.push_back("HierarchyManagement_Test: Failed to get 'testBox' by name from 'mainStage'.");
            ok = false;
        }
        // Remove Child by handle
        stage->removeChild(boxHandle);
        if (stage->hasChild(boxHandle))
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' should have been removed from 'mainStage'.");
            ok = false;
        }
        // Add it back to the stage and verify
        stage->addChild(boxHandle);
        if (!stage->hasChild(boxHandle))
        {
            errors.push_back("HierarchyManagement_Test: 'testBox` failed to return to the stage.");
            ok = false;
        }
        // Remove Child by Name
        stage->removeChild("testBox");
        if (stage->hasChild(boxHandle))
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' should have been removed from 'mainStage' by name.");
            ok = false; 
        }
        // Add it back again to see if we can find the parent
        stage->addChild(boxHandle);
        if (!stage->hasChild(boxHandle))
        {
            errors.push_back("HierarchyManagement_Test: 'testBox` failed to return to the stage (2nd time).");
            ok = false;
        }
        DisplayHandle parent = boxHandle->getParent();
        if (!parent.isValid())
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' parent not found.");
            ok = false;
        }
        else if (parent != stage)
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' parent is incorrect (expected 'mainStage').");
            ok = false;     
        }
        // Change parents to the `blueishBox`
        DisplayHandle blueishBox = core.getDisplayObject("blueishBox");
        if (!blueishBox.isValid())
        {
            errors.push_back("HierarchyManagement_Test: 'blueishBox' not found for parent change test.");
            return false;
        }
        boxHandle->setParent(blueishBox);
        if (boxHandle->getParent() != blueishBox)
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' parent change to 'blueishBox' failed.");
            ok = false;
        }
        // Verify the Stage is still an ancestor of the box by handle
        bool isAncestor = stage->isAncestorOf(boxHandle);
        if (!isAncestor)
        {
            errors.push_back("HierarchyManagement_Test: 'mainStage' should be ancestor of 'testBox' after parent change.");
            ok = false;
        }
        // Verify the Stage is still an ancestor of the box by name
        isAncestor = stage->isAncestorOf("testBox");;
        if (!isAncestor)
        {
            errors.push_back("HierarchyManagement_Test: 'mainStage' should be ancestor of 'testBox' (by name) after parent change.");
            ok = false; 
        }
        // Verify the Box is a decendant of the Stage by handle
        bool isDecendant = boxHandle->isDescendantOf(stage);
        if (!isDecendant)
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' should be descendant of 'mainStage' after parent change.");
            ok = false;
        }
        // Verify the Box is a decendant of the Stage by name
        isDecendant = boxHandle->isDescendantOf("mainStage");
        if (!isDecendant)
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' should be descendant of 'mainStage' (by name) after parent change.");
            ok = false; 
        }
        // Verify remove from parent
        bool wasRemoved = boxHandle->removeFromParent();
        if (!wasRemoved)
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' removeFromParent() reported failure.");
            ok = false; 
        }
        if (blueishBox->hasChild(boxHandle))
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' should have been removed from its parent.");
            ok = false; 
        }
        if (boxHandle->getParent().isValid())
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' parent should be invalid after removeFromParent().");
            ok = false; 
        }
        // add the box back to the blueishBox and test remove decendant by handle
        blueishBox->addChild(boxHandle);
        if (!blueishBox->hasChild(boxHandle))
        {
            errors.push_back("HierarchyManagement_Test: 'testBox` failed to return to the 'blueishBox'.");
            ok = false;
        }
        stage->removeDescendant(boxHandle);
        if (blueishBox->hasChild(boxHandle))
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' should have been removed from the 'blueishBox' by removeDescendant().");
            ok = false;
        }
        // add the box back to the blueishBox and test remove decendant by name
        blueishBox->addChild(boxHandle);
        if (!blueishBox->hasChild(boxHandle))
        {
            errors.push_back("HierarchyManagement_Test: 'testBox` failed to return to the 'blueishBox' (2nd time).");
            ok = false;
        }
        stage->removeDescendant("testBox");
        if (blueishBox->hasChild(boxHandle))
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' should have been removed from the 'blueishBox' by removeDescendant() by name.");
            ok = false;
        }
        // Finally, destroy the testBox object to clean up
        core.destroyDisplayObject(boxHandle->getName());
        // verify destruction
        DisplayHandle checkBox = core.getDisplayObject("testBox");
        if (checkBox.isValid())
        {
            errors.push_back("HierarchyManagement_Test: 'testBox' should have been destroyed but still exists.");
            ok = false;
        }
        // return the final test result
        return ok;
    } // IDisplayObject_test6(std::vector<std::string>& errors)   










    // --- Lua Integration Tests --- //

    bool IDisplayObject_LUA_Tests(std::vector<std::string>& errors)
    {
  
        UnitTests& ut = UnitTests::getInstance();
        return ut.run_lua_tests(errors, "src/IDisplayObject_UnitTests.lua");
    } // END: IDisplayObject_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool IDisplayObject_UnitTests()
    {
        UnitTests& ut = UnitTests::getInstance();
        ut.clear_tests();

        ut.add_test("Scaffold", IDisplayObject_test0);
        ut.add_test("Create Stage Object", IDisplayObject_test1);
        ut.add_test("Get and Set Name", IDisplayObject_test2);
        ut.add_test("Destroy the generic Stage Object", IDisplayObject_test3);
        ut.add_test("Dirty/State Management", IDisplayObject_test4);
        ut.add_test("Events and Event Listener Handling", IDisplayObject_test5);
        ut.add_test("Hierarchy Management", IDisplayObject_test6);


        ut.add_test("Lua Integration", IDisplayObject_LUA_Tests);

        return ut.run_all("IDisplayObject");
    } // END: IDisplayObject_UnitTests()



} // END: namespace SDOM