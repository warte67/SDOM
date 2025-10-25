// IDisplayObject_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Utils.hpp>
#include <SDOM/SDOM_IDisplayObject_Lua.hpp>

#include "Box.hpp"

namespace SDOM
{
    // --- IDisplayObject Unit Test Scaffolding --- //
    bool IDisplayObject_test0(std::vector<std::string>& errors)   
    {
        // ✅ Test Verified
        // 🔄 In Progress
        // ⚠️ Failing     
        // 🚫 Remove
        // ❌ Invalid
        // ☐ Unchecked/Untested

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


    // --- Type & Property Access --- //
    bool IDisplayObject_test7(std::vector<std::string>& errors)   
    {
        // --- Type & Property Access --- //
            // ✅ std::string getName_lua();                        
            // ✅ void setName_lua(const std::string& newName);    
            // ✅ std::string getType_lua();
            // ✅ Bounds getBounds_lua();
            // ✅ void setBounds_lua(const sol::object& bobj);
            // ✅ SDL_Color getColor_lua();
            // ✅ void setColor_lua(const sol::object& colorObj);
            // ✅ SDL_Color getForegroundColor_lua();
            // ✅ void setForegroundColor_lua(const sol::object& colorObj);
            // ✅ SDL_Color getBackgroundColor_lua();
            // ✅ void setBackgroundColor_lua(const sol::object& colorObj);
            // ✅ SDL_Color getBorderColor_lua();
            // ✅ void setBorderColor_lua(const sol::object& colorObj);
            // ✅ SDL_Color getOutlineColor_lua();
            // ✅ void setOutlineColor_lua(const sol::object& colorObj);
            // ✅ SDL_Color getDropshadowColor_lua();
            // ✅ void setDropshadowColor_lua(const sol::object& colorObj);

        bool ok = true;
        Core& core = getCore();

        std::string testObjectName = "blueishBox";
        DisplayHandle testObject = core.getDisplayObject(testObjectName);
        if (!testObject.isValid())
        {
            errors.push_back("IDisplayObject_test7: Test object '" + testObjectName + "' not found.");
            return false;
        }
        // Verify getName()
        std::string name = testObject->getName();
        if (name != testObjectName)
        {
            errors.push_back("IDisplayObject_test7: getName() returned '" + name + "', expected '" + testObjectName + "'.");
            ok = false;
        }
        // Verify setName("string")
        std::string newName = "bluishBoxRenamed";
        testObject->setName(newName);
        std::string updatedName = testObject->getName();
        if (updatedName != newName)
        {
            errors.push_back("IDisplayObject_test7: setName() failed, got '" + updatedName + "', expected '" + newName + "'.");
            ok = false; 
        }
        // Revert the name change for other tests
        testObject->setName(testObjectName);
        // Verify getType()
        if (testObject.getType() != "Box")
        {
            errors.push_back("IDisplayObject_test7: getType() returned '" + testObject->getType() + "', expected 'Box'.");
            ok = false; 
        }
        // Verify getBounds()
        Bounds bounds = testObject->getBounds();
        if ((int)bounds.left != 180 || (int)bounds.top != 70 || (int)bounds.right != 430 || (int)bounds.bottom != 295)
        {
            errors.push_back("IDisplayObject_test7: getBounds() got left: " + std::to_string(bounds.left) +
                             ", top: " + std::to_string(bounds.top) +
                             ", right: " + std::to_string(bounds.right) +
                             ", bottom: " + std::to_string(bounds.bottom) +
                             "; expected left: 180, top: 70, right: 430, bottom: 295.");
            ok = false; 
        }
        // Verify setBounds()
        Bounds new_bounds = {180, 75, 180+240, 75+215};
        testObject->setBounds(new_bounds);
        Bounds updated_bounds = testObject->getBounds();
        if (updated_bounds != new_bounds)
        {
            errors.push_back("IDisplayObject_test7: setBounds() failed, got left: " + std::to_string(updated_bounds.left) +
                             ", top: " + std::to_string(updated_bounds.top) +
                             ", right: " + std::to_string(updated_bounds.right) +
                             ", bottom: " + std::to_string(updated_bounds.bottom) +
                             "; expected left: " + std::to_string(new_bounds.left) +
                             ", top: " + std::to_string(new_bounds.top) +
                             ", right: " + std::to_string(new_bounds.right) +
                             ", bottom: " + std::to_string(new_bounds.bottom) + ".");
            ok = false; 
        }        
        // Verify Color Getters and Setters
        SDL_Color orig_color = testObject->getColor();
        SDL_Color orig_fg_color = testObject->getForegroundColor();
        SDL_Color orig_bg_color = testObject->getBackgroundColor();
        SDL_Color orig_border_color = testObject->getBorderColor();
        SDL_Color orig_outline_color = testObject->getOutlineColor();
        SDL_Color orig_dropshadow_color = testObject->getDropshadowColor();
        // New Colors
        SDL_Color new_color             = {0, 255, 0, 255};     testObject->setColor(new_color);
        SDL_Color new_fg_color          = { 255, 0, 0, 255};    testObject->setForegroundColor(new_fg_color);
        SDL_Color new_bg_color          = {0, 0, 255, 255};     testObject->setBackgroundColor(new_bg_color);
        SDL_Color new_border_color      = {255, 255, 0, 255};   testObject->setBorderColor(new_border_color);
        SDL_Color new_outline_color     = {0, 255, 255, 255};   testObject->setOutlineColor(new_outline_color);
        SDL_Color new_dropshadow_color  = {255, 0, 255, 255};   testObject->setDropshadowColor(new_dropshadow_color);
        // Verify the New Colors
        SDL_Color read_color = testObject->getColor();
        if (SDL_Utils::color_not_equal(new_color, read_color))
        {
            errors.push_back( "IDisplayObject_test7: color mismatch. " 
                "Expected " + SDL_Utils::color_to_string(new_color) +
                " but got " + SDL_Utils::color_to_string(read_color)
            );
            ok = false;
        }
        // Verify Foreground Color
        SDL_Color fg = testObject->getForegroundColor();
        if (SDL_Utils::color_not_equal(new_fg_color, fg))
        {
            errors.push_back(
                "IDisplayObject_test7: foreground_color mismatch. "
                "Expected " + SDL_Utils::color_to_string(new_fg_color) +
                " but got " + SDL_Utils::color_to_string(fg)
            );
            ok = false;
        }
        // Verify Background Color
        SDL_Color bg = testObject->getBackgroundColor();
        if (SDL_Utils::color_not_equal(new_bg_color, bg))
        {
            errors.push_back(
                "IDisplayObject_test7: background_color mismatch. "
                "Expected " + SDL_Utils::color_to_string(new_bg_color) +
                " but got " + SDL_Utils::color_to_string(bg)
            );
            ok = false;
        }
        // Verify Border Color
        SDL_Color border = testObject->getBorderColor();
        if (SDL_Utils::color_not_equal(new_border_color, border))
        {
            errors.push_back(
                "IDisplayObject_test7: border_color mismatch. "
                "Expected " + SDL_Utils::color_to_string(new_border_color) +
                " but got " + SDL_Utils::color_to_string(border)
            );
            ok = false;
        }
        // Verify Outline Color
        SDL_Color outline = testObject->getOutlineColor();
        if (SDL_Utils::color_not_equal(new_outline_color, outline))
        {
            errors.push_back(
                "IDisplayObject_test7: outline_color mismatch. "
                "Expected " + SDL_Utils::color_to_string(new_outline_color) +
                " but got " + SDL_Utils::color_to_string(outline)
            );
            ok = false;
        }
        // Verify Dropshadow Color
        SDL_Color dropshadow = testObject->getDropshadowColor();
        if (SDL_Utils::color_not_equal(new_dropshadow_color, dropshadow))
        {
            errors.push_back(
                "IDisplayObject_test7: dropshadow_color mismatch. "
                "Expected " + SDL_Utils::color_to_string(new_dropshadow_color) +
                " but got " + SDL_Utils::color_to_string(dropshadow)
            );
            ok = false;
        }
        // Restore the Original Colors
        testObject->setColor(orig_color);
        testObject->setForegroundColor(orig_fg_color);
        testObject->setBackgroundColor(orig_bg_color);
        testObject->setBorderColor(orig_border_color);
        testObject->setOutlineColor(orig_outline_color);
        testObject->setDropshadowColor(orig_dropshadow_color);
        // Return Results
        return ok;
    } // IDisplayObject_test7(std::vector<std::string>& errors)   

    // --- Priority and Z-Order --- //
    bool IDisplayObject_test8(std::vector<std::string>& errors)   
    {
        // ✅ Test Verified
        // 🔄 In Progress
        // ⚠️ Failing     
        // ☐ Unchecked/Untested

        // ☐ int getMaxPriority_lua(const IDisplayObject* obj);              
        // ☐ int getMinPriority_lua(const IDisplayObject* obj);              
        // ☐ int getPriority_lua(const IDisplayObject* obj);                 
        // ☐ void setToHighestPriority_lua(IDisplayObject* obj);             
        // ☐ void setToLowestPriority_lua(IDisplayObject* obj);     
        // ☐ void setToHighestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);        // descriptor form
        // ☐ void setToLowestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);         // descriptor form
        // ✅ void sortChildrenByPriority_lua(IDisplayObject* obj);           
        // ✅ void setPriority_lua(IDisplayObject* obj, int priority);        
        // ☐ void setPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);                 // descriptor form
        // ☐ void setPriority_lua_target(IDisplayObject* obj, const sol::object& descriptor, int value);   // descriptor form
        // ✅ std::vector<int> getChildrenPriorities_lua(const IDisplayObject* obj);      
        // ⚠️ void moveToTop_lua(IDisplayObject* obj);                                    
        // ⚠️ void moveToTop_lua_any(IDisplayObject* obj, const sol::object& descriptor);                   // descriptor form
        // ✅ int getZOrder_lua(const IDisplayObject* obj);                               
        // ☐ void setZOrder_lua(IDisplayObject* obj, int z_order);                       
        // ✅ void setZOrder_lua_any(IDisplayObject* obj, const sol::object& descriptor);                   // descriptor form
        // ☐ bool getBorder_lua(const IDisplayObject* obj);                // Rename to hasBorder() for consistency  
        // ☐ bool getBackground_lua(const IDisplayObject* obj);            // Rename to hasBackground() for  consistency                   
        // ☐ void setBorder_lua(IDisplayObject* obj, bool hasBorder);      
        // ☐ void setBackground_lua(IDisplayObject* obj, bool hasBackground);

        bool ok = true;
        Core& core = getCore();
        DisplayHandle stage = core.getRootNode();
        if (!stage.isValid())
        {
            errors.push_back("PriorityZOrder_Test: 'mainStage' not found.");
            return false;            
        }
        // Create 3 test boxes
        auto make_box = [&](const std::string& name, int x) {
            Box::InitStruct init;
            init.name = name;
            init.x = x;
            init.y = 10;
            init.width = 20;
            init.height = 20;
            init.color = {static_cast<Uint8>(x), static_cast<Uint8>(x), static_cast<Uint8>(x), 255};
            return core.createDisplayObject("Box", init);
        };
        DisplayHandle test_box = make_box("test_box", 64); stage->addChild(test_box);
        DisplayHandle A = make_box("A", 128);    test_box->addChild(A);
        DisplayHandle B = make_box("B", 192);    test_box->addChild(B);
        DisplayHandle C = make_box("C", 255);    test_box->addChild(C);

        // Confirm initial priorities are equal
        auto priorities_before = test_box->getChildrenPriorities();
        std::cout << "childrencount: " << priorities_before.size() << std::endl;
        if (priorities_before.size() != 3) {
            errors.push_back("PriorityZOrder_Test: Expected three children attached to the test box.");
            ok = false;
        }        

        // Set explicit priorities
        C->setPriority(-100); // push C to bottom
        A->setPriority(0);
        B->setPriority(100);  // pull B to top
        test_box->sortChildrenByPriority();   

        // Read back expected order: C (lowest), A (middle), B (highest)
        auto pr_sorted = test_box->getChildrenPriorities();
        if (!(pr_sorted[0] < pr_sorted[1] && pr_sorted[1] < pr_sorted[2])) {
            errors.push_back("PriorityZOrder_Test: Expected sorted priorities: C < A < B.");
            ok = false;
        }        

        // Test moveToTop()
        A->moveToTop();
        if (A->getZOrder() <= B->getZOrder()) {
            errors.push_back("PriorityZOrder_Test: moveToTop() failed to place A at the top.");
            ok = false;
        }      
        
        // Test moveToTop_lua_any() with descriptor table structure:
        sol::state_view lua = getLua();
        sol::table desc = lua.create_table();
        desc["name"] = "C";
        IDisplayObject* obj_a = A.as<IDisplayObject>(); if (!obj_a) { errors.push_back("PriorityZOrder_test: unable to fetch obj A."); return false; }
        moveToTop_lua_any(obj_a, desc);
        if (C->getZOrder() <= A->getZOrder()) {
            errors.push_back("PriorityZOrder_Test: moveToTop_lua_any(C) failed to place C at the top.");
            ok = false;
        }        

        // Cleanup
        core.destroyDisplayObject("test_box");
        core.destroyDisplayObject("A");
        core.destroyDisplayObject("B");        
        core.destroyDisplayObject("C");
        if (getFactory().countOrphanedDisplayObjects() != 0)
        {
            errors.push_back("PriorityZOrder_test: Orpans Left Behind.");
            ok = false;
        }

        // return results
        return ok;
    } // IDisplayObject_test0(std::vector<std::string>& errors)   





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
        ut.add_test("Type and Property Access", IDisplayObject_test7);
        ut.add_test("Priority and Z-Order", IDisplayObject_test8);


        ut.add_test("Lua Integration", IDisplayObject_LUA_Tests);

        return ut.run_all("IDisplayObject");
    } // END: IDisplayObject_UnitTests()



} // END: namespace SDOM