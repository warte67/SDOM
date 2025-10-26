// IDisplayObject_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Utils.hpp>
#include <SDOM/SDOM_Core_Lua.hpp>
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
        // ☐ Planned

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
            // ✅ bool hasBorder_lua(const IDisplayObject* obj);
            // ✅ bool hasBackground_lua(const IDisplayObject* obj);
            // ✅ void setBorder_lua(IDisplayObject* obj, bool hasBorder);      
            // ✅ void setBackground_lua(IDisplayObject* obj, bool hasBackground);            

        bool ok = true;
        Core& core = getCore();

        std::string testObjectName = "blueishBox";
        DisplayHandle testObject = core.getDisplayObject(testObjectName);
        if (!testObject.isValid())
        {
            errors.push_back("IDisplayObject_test7: Test object '" + testObjectName + "' not found.");
            return false;
        }
        // ✅ std::string getName_lua();
        std::string name = testObject->getName();
        if (name != testObjectName)
        {
            errors.push_back("IDisplayObject_test7: getName() returned '" + name + "', expected '" + testObjectName + "'.");
            ok = false;
        }
        // ✅ void setName_lua(const std::string& newName);  
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
        // ✅ std::string getType_lua();
        if (testObject.getType() != "Box")
        {
            errors.push_back("IDisplayObject_test7: getType() returned '" + testObject->getType() + "', expected 'Box'.");
            ok = false; 
        }
        // ✅ Bounds getBounds_lua();
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
        // ✅ void setBounds_lua(const sol::object& bobj);
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
        // ✅ SDL_Color getColor_lua();
        SDL_Color orig_color = testObject->getColor();
        // ✅ SDL_Color getForegroundColor_lua();
        SDL_Color orig_fg_color = testObject->getForegroundColor();
        // ✅ SDL_Color getBackgroundColor_lua();
        SDL_Color orig_bg_color = testObject->getBackgroundColor();
        // ✅ SDL_Color getBorderColor_lua();
        SDL_Color orig_border_color = testObject->getBorderColor();
        // ✅ SDL_Color getOutlineColor_lua();
        SDL_Color orig_outline_color = testObject->getOutlineColor();
        // ✅ SDL_Color getDropshadowColor_lua();
        SDL_Color orig_dropshadow_color = testObject->getDropshadowColor();
        // ✅ void setColor_lua(const sol::object& colorObj);
        SDL_Color new_color             = {0, 255, 0, 255};     testObject->setColor(new_color);
        // ✅ void setForegroundColor_lua(const sol::object& colorObj);
        SDL_Color new_fg_color          = { 255, 0, 0, 255};    testObject->setForegroundColor(new_fg_color);
        // ✅ SDL_Color getBorderColor_lua();
        SDL_Color new_bg_color          = {0, 0, 255, 255};     testObject->setBackgroundColor(new_bg_color);
        // ✅ void setBorderColor_lua(const sol::object& colorObj);
        SDL_Color new_border_color      = {255, 255, 0, 255};   testObject->setBorderColor(new_border_color);
        // ✅ void setOutlineColor_lua(const sol::object& colorObj);
        SDL_Color new_outline_color     = {0, 255, 255, 255};   testObject->setOutlineColor(new_outline_color);
        // ✅ void setDropshadowColor_lua(const sol::object& colorObj);
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
        // --- Border and Background Tests --- //
        Box* box = testObject.as<Box>();
        if (!box) {
            errors.push_back("IDisplayObject_test7: Test object is not of type Box for border/background tests.");
            ok = false;
        }
        // ✅ bool hasBorder_lua(const IDisplayObject* obj);
        bool orig_has_border = hasBorder_lua(box);
        // ✅ bool hasBackground_lua(const IDisplayObject* obj);
        bool orig_has_background = hasBackground_lua(box);
        setBorder_lua(box, orig_has_border ? false : true);  // toggle border
        if (hasBorder_lua(box) == orig_has_border) {
            errors.push_back("IDisplayObject_test7: setBorder_lua() failed to toggle border state.");
            ok = false;
        }
        setBackground_lua(box, orig_has_background ? false : true);  // toggle background
        if (hasBackground_lua(box) == orig_has_background) {
            errors.push_back("IDisplayObject_test7: setBackground_lua() failed to toggle background state.");
            ok = false;
        }
        // ✅ void setBorder_lua(IDisplayObject* obj, bool hasBorder);      
        setBorder_lua(box, orig_has_border);
        // ✅ void setBackground_lua(IDisplayObject* obj, bool hasBackground);  
        setBackground_lua(box, orig_has_background);
        // Verify restoration
        if (hasBorder_lua(box) != orig_has_border) {
            errors.push_back("IDisplayObject_test7: Failed to restore original border state.");
            ok = false;
        }
        if (hasBackground_lua(box) != orig_has_background) {
            errors.push_back("IDisplayObject_test7: Failed to restore original background state.");
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
        
        // ✅ std::vector<DisplayHandle>& getChildren(const IDisplayObject* obj);
        // ✅ int countChildren_lua(const IDisplayObject* obj);
        // ✅ int getMaxPriority_lua(const IDisplayObject* obj);              
        // ✅ int getMinPriority_lua(const IDisplayObject* obj);              
        // ✅ int getPriority_lua(const IDisplayObject* obj);                 
        // ✅ void setToHighestPriority_lua(IDisplayObject* obj);             
        // ✅ void setToLowestPriority_lua(IDisplayObject* obj);     
        // ✅ void setToHighestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);
        // ✅ void setToLowestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);
        // ✅ void sortChildrenByPriority_lua(IDisplayObject* obj);           
        // ✅ void setPriority_lua(IDisplayObject* obj, int priority);        
        // ✅ void setPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);
        // ✅ void setPriority_lua_target(IDisplayObject* obj, const sol::object& descriptor, int value);
        // ✅ std::vector<int> getChildrenPriorities_lua(const IDisplayObject* obj);      
        // ✅ void moveToTop_lua(IDisplayObject* obj);                                    
        // ✅ void moveToTop_lua_any(IDisplayObject* obj, const sol::object& descriptor);
        // ✅ void moveToBottom_lua(IDisplayObject* obj);                                 
        // ✅ void moveToBottom_lua_any(IDisplayObject* obj, const sol::object& descriptor);
        // ✅ void bringToFront_lua(IDisplayObject* obj);                                  
        // ✅ void bringToFront_lua_any(IDisplayObject* obj, const sol::object& descriptor);
        // ✅ void sendToBack_lua(IDisplayObject* obj);                                   
        // ✅ void sendToBack_lua_any(IDisplayObject* obj, const sol::object& descriptor);
        // ✅ void sendToBackAfter_lua(IDisplayObject* obj, const IDisplayObject* limitObj);
        // ✅ void sendToBackAfter_lua_any(IDisplayObject* obj, const sol::object& descriptor, const IDisplayObject* limitObj);
        // ✅ int getZOrder_lua(const IDisplayObject* obj);                               
        // ✅ void setZOrder_lua(IDisplayObject* obj, int z_order);                       
        // ✅ void setZOrder_lua_any(IDisplayObject* obj, const sol::object& descriptor);

        // Helper: get index of child in parent
        auto child_index = [](DisplayHandle parent, DisplayHandle child) {
            auto& kids = parent->getChildren();
            for (size_t i = 0; i < kids.size(); ++i)
                if (kids[i].get() == child.get()) return int(i);
            return -1;
        };

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
        DisplayHandle parent_box = make_box("parent_box", 64); stage->addChild(parent_box);
        DisplayHandle A = make_box("A", 128);    parent_box->addChild(A);
        DisplayHandle B = make_box("B", 192);    parent_box->addChild(B);
        DisplayHandle C = make_box("C", 255);    parent_box->addChild(C);
        // ✅ std::vector<int> getChildrenPriorities()
        auto priorities_before = parent_box->getChildrenPriorities();
        // std::cout << "childrencount: " << priorities_before.size() << std::endl;
        if (priorities_before.size() != 3) {
            errors.push_back("PriorityZOrder_Test: Expected three children attached to the test box.");
            ok = false;
        }        
        // ✅ void setPriority
        C->setPriority(-100); // push C to bottom
        A->setPriority(0);
        B->setPriority(100);  // pull B to top
        parent_box->sortChildrenByPriority();   
        // Read back expected order: C (lowest), A (middle), B (highest)
        auto pr_sorted = parent_box->getChildrenPriorities();
        if (child_index(parent_box, C) != 0 || child_index(parent_box, A) != 1 || child_index(parent_box, B) != 2) {
            errors.push_back("PriorityZOrder_Test: Expected sorted priorities: C > A > B.");
            ok = false;
        }
        // ✅ int getPriority(const IDisplayObject* obj); 
        if (A->getPriority() != 0) {
            errors.push_back("PriorityZOrder_Test: A priority incorrect for A (read: " + std::to_string(A->getPriority()) + ",  expected 0).");
            ok = false; 
        }
        if (B->getPriority() != 100) {
            errors.push_back("PriorityZOrder_Test: B priority incorrect for B (read: " + std::to_string(B->getPriority()) + ",  expected 100).");
            ok = false; 
        }
        if (C->getPriority() != -100) {
            errors.push_back("PriorityZOrder_Test: C priority incorrect for C (read: " + std::to_string(C->getPriority()) + ",  expected -100).");
            ok = false; 
        }
        // ✅ int getMaxPriority()
        if (parent_box->getMaxPriority() != 100) {
            errors.push_back("PriorityZOrder_Test: max priority incorrect (read: " + std::to_string(parent_box->getMaxPriority()) + ",  expected 100).");
            ok = false; 
        }
        // ✅ int getMinPriority()
        if (parent_box->getMinPriority() != -100) {
            errors.push_back("PriorityZOrder_Test: min priority incorrect (read: " + std::to_string(parent_box->getMinPriority()) + ",  expected -100).");
            ok = false; 
        }
        // ✅ void setToHighestPriority()
        C->setToHighestPriority();
        if (C->getPriority() != parent_box->getMaxPriority()) {
            errors.push_back("PriorityZOrder_Test: setToHighestPriority() failed for C.");
            ok = false;
        }   
        // ✅ void setToLowestPriority
        C->setToLowestPriority();
        if (C->getPriority() != parent_box->getMinPriority()) {
            errors.push_back("PriorityZOrder_Test: setToLowestPriority() failed for C.");
            ok = false;
        }
        // Fetch a raw pointer to the parent and setup for descriptor forms
        IDisplayObject* parentObj = parent_box.as<IDisplayObject>();
        if (!parentObj) { errors.push_back("PriorityZOrder_test: unable to fetch parent_box."); return false; }        
        sol::state_view lua = getLua();
        sol::table desc = lua.create_table();
        desc["name"] = "C";
        // ✅ void setToHighestPriority_lua_any()
        setToHighestPriority_lua_any(parentObj, desc);
        if (C->getPriority() != parent_box->getMaxPriority()) {
            errors.push_back("PriorityZOrder_Test: setToHighestPriority_lua_any(C) failed.");
            ok = false;
        }   
        // ✅ void setToLowestPriority_lua_any()
        setToLowestPriority_lua_any(parentObj, desc);
        if (C->getPriority() != parent_box->getMinPriority()) {
            errors.push_back("PriorityZOrder_Test: setToLowestPriority_lua_any(C) failed.");
            ok = false;
        }
        // ✅ void setPriority_lua_target(IDisplayObject* obj, const sol::object& descriptor, int value);
        IDisplayObject* c_obj = C.as<IDisplayObject>();
        if (!c_obj) { errors.push_back("PriorityZOrder_Test: unable to fetch C object ptr"); return false; }
        setPriority_lua_target(parentObj, desc, 4);
        if (C->getPriority() != 4)
        {
            errors.push_back("PriorityZOrder_test: setPriority_lua_target() failed to set C priorty to 4.");
            ok = false;
        }
        // ✅ void setPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);
        desc["priority"] = 200;
        setPriority_lua_any(c_obj, desc);
        if (C->getPriority() != 200)
        {
            errors.push_back("PriorityZOrder_test: setPriority_lua_any() failed to set C priority to 200.");
            ok = false;
        }
        // ✅ void moveToTop()
        A->moveToTop();
        if (A->getZOrder() < B->getZOrder()) {
            errors.push_back("PriorityZOrder_Test: moveToTop() failed to place A at the top.");
            ok = false;
        }    
        // ✅ void moveToTop_lua_any()   
        sol::table desc_c = lua.create_table();
        desc_c["name"] = "C";
        moveToTop_lua_any(parentObj, desc_c);
        // Verify: C should now be on top
        if (C->getZOrder() != parent_box->countChildren() - 1) {
            errors.push_back("PriorityZOrder_Test: moveToTop_lua_any(C) failed to place C at the top.");
            ok = false;
        }
        // ✅ setZOrder() move A to the top
        A->setZOrder(3);
        parent_box->sortByZOrder();
        auto &kids = parent_box->getChildren();
        if (kids.back().get() != A.get()) {
            errors.push_back("PriorityZOrder_Test: setZOrder(3) failed to place A at the top.");
            ok = false;
        }
        // cleanup and start a new hierarchy
        core.destroyDisplayObject("A");
        core.destroyDisplayObject("B");        
        core.destroyDisplayObject("C");        
        DisplayHandle zbox = make_box( "zbox", 32);    stage->addChild(zbox);
                      A  = make_box( "A", 64);         zbox->addChild(A);
                      B  = make_box( "B", 96);         zbox->addChild(B);
                      C  = make_box( "C", 128);        zbox->addChild(C);
        DisplayHandle D  = make_box( "D", 160);        zbox->addChild(D);
        DisplayHandle D1  = make_box( "D1", 192);        A->addChild(D1);
        DisplayHandle D2  = make_box( "D2", 224);        A->addChild(D2);
        DisplayHandle D3  = make_box( "D3", 255);        A->addChild(D3);
        // ✅ void moveToBottom_lua(IDisplayObject* obj); 
        Box* box_a = A.as<Box>();
        if (!box_a) { errors.push_back("PriorityZOrder_test: unable to fetch A box ptr"); return false; }
        moveToBottom_lua(box_a);
        if (child_index(zbox, A) != 0) {
            errors.push_back("moveToBottom: A should be first child of zbox. (was: " + std::to_string(child_index(zbox, A)) + ", expected 0)");
            ok = false;
        }
        // ✅ void moveToBottom_lua_any(IDisplayObject* obj, const sol::object& descriptor);      // descriptor form
        sol::table descA = lua.create_table(); descA["name"] = "A";
        moveToBottom_lua_any(zbox.as<IDisplayObject>(), descA);
        if (child_index(zbox, A) != 0) {
            errors.push_back("moveToBottom_lua_any: A should be first child of zbox.");
            ok = false;
        }
        // ✅ void bringToFront_lua(IDisplayObject* obj);                                  
        B->bringToFront();
        if (child_index(zbox, B) != int(zbox->countChildren()) - 1) {
            errors.push_back("bringToFront: B should be last child of zbox.");
            ok = false;
        }
        // ✅ void bringToFront_lua_any(IDisplayObject* obj, const sol::object& descriptor);      // descriptor form    
        sol::table descB = lua.create_table(); descB["name"] = "B";
        bringToFront_lua_any(zbox.as<IDisplayObject>(), descB);
        if (child_index(zbox, B) != int(zbox->countChildren()) - 1) {
            errors.push_back("bringToFront_lua_any: B should be last child of zbox.");
            ok = false;
        }
        // ✅ void sendToBack_lua(IDisplayObject* obj);    
        C->sendToBack();
        if (child_index(zbox, C) != 0) {
            errors.push_back("sendToBack: C should be first child of zbox.");
            ok = false;
        }
        // ✅ void sendToBack_lua_any(IDisplayObject* obj, const sol::object& descriptor);
        sol::table descC = lua.create_table(); descC["name"] = "C";
        sendToBack_lua_any(zbox.as<IDisplayObject>(), descC);
        if (child_index(zbox, C) != 0) {
            errors.push_back("sendToBack_lua_any: C should be first child of zbox.");
            ok = false;
        }
        // ✅ void sendToBackAfter_lua(IDisplayObject* obj, const IDisplayObject* limitObj);
        D->sendToBackAfter(B.get());
        if (child_index(zbox, D) != child_index(zbox, B) + 1) {
            errors.push_back("sendToBackAfter: D should be immediately after B in zbox.");
            ok = false;
        }
        // ✅ void sendToBackAfter_lua_any(IDisplayObject* obj, const sol::object& descriptor, const IDisplayObject* limitObj);
        sol::table descD3 = lua.create_table(); descD3["name"] = "D3";
        sendToBackAfter_lua_any(zbox.as<Box>(), descD3, C.as<Box>());
        if (child_index(zbox, D3) != child_index(zbox, C) + 1) {
            errors.push_back("sendToBackAfter_lua_any: D3 should be immediately after C in zbox.");
            ok = false;
        }
        // Cleanup
        // for (auto& kid : zbox->getChildren() )
        // {
        //     DEBUG_LOG("Kid: " + kid->getName() + " ZOrder: " + std::to_string(kid->getZOrder()));
        // }
        core.destroyDisplayObject("parent_box");
        core.destroyDisplayObject("zbox");
        core.destroyDisplayObject("A");
        core.destroyDisplayObject("B");        
        core.destroyDisplayObject("C");
        core.destroyDisplayObject("D");
        core.destroyDisplayObject("D1");
        core.destroyDisplayObject("D2");
        core.destroyDisplayObject("D3");
        if (getFactory().countOrphanedDisplayObjects() != 0)
        {
            errors.push_back("PriorityZOrder_test: Orpans Left Behind.");
            ok = false;
            for (const auto& punk : getFactory().getOrphanedDisplayObjects())
            {
                errors.push_back("  Orphan: " + punk->getName() + " (" + punk->getType() + ")");
            }
        }
        // return results
        return ok;
    } // IDisplayObject_test8(std::vector<std::string>& errors)   


    // --- Object Focus and Interactivity --- //
    bool IDisplayObject_test9(std::vector<std::string>& errors)   
    {
        // ✅ Test Verified
        // 🔄 In Progress
        // ⚠️ Failing
        // ☐ Planned

        // Focus & keyboard
        // ✅ void setKeyboardFocus_lua(IDisplayObject* obj)
        // ✅ bool isKeyboardFocused_lua(const IDisplayObject* obj)

        // Mouse hover (event injection + queries)
        // ✅ void Core::pushMouseEvent_lua(const sol::object& args)
        // ✅ void Core::pumpEventsOnce_lua()
        // ✅ void Core::pushMouseEvent(const sol::object& args)
        // ✅ void Core::pumpEventsOnce()
        // ✅ bool isMouseHovered_lua(const IDisplayObject* obj)
        // ✅ bool IDisplayObject::isMouseHovered()

        // Clickable
        // ✅ bool isClickable_lua(const IDisplayObject* obj)
        // ✅ bool IDisplayObject::isClickable()        
        // ✅ void IDisplayObject::setClickable(bool clickable)
        // ✅ void setClickable_lua(IDisplayObject* obj, bool clickable)

        // Enabled/disabled
        // ✅ bool isEnabled_lua(const IDisplayObject* obj)
        // ✅ void setEnabled_lua(IDisplayObject* obj, bool enabled)
        // ✅ void IDisplayObject::setEnabled(bool enabled)
        // ✅ bool IDisplayObject::isEnabled()

        // Hidden/visible
        // ✅ bool isHidden_lua(const IDisplayObject* obj)
        // ✅ void setHidden_lua(IDisplayObject* obj, bool hidden)
        // ✅ void IDisplayObject::setHidden(bool hidden)
        // ✅ bool IDisplayObject::isHidden()
        // ✅ bool isVisible_lua(const IDisplayObject* obj)
        // ✅ void setVisible_lua(IDisplayObject* obj, bool visible)
        // ✅ void IDisplayObject::setVisible(bool visible)
        // ✅ bool IDisplayObject::isVisible()

        // Tab Management
        // ✅ int IDisplayObject::getTabPriority()
        // ✅ int getTabPriority_lua(const IDisplayObject* obj)
        // ✅ void setTabPriority_lua(IDisplayObject* obj, int index)
        // ✅ IDisplayObject& IDisplayObject::setTabPriority(int index)
        // ✅ bool isTabEnabled_lua(const IDisplayObject* obj)
        // ✅ bool IDisplayObject::isTabEnabled()
        // ✅ void setTabEnabled_lua(IDisplayObject* obj, bool enabled)
        // ✅ IDisplayObject& IDisplayObject::setTabEnabled(bool enabled)


        bool ok = true;
        Core& core = getCore();
        DisplayHandle stage = core.getRootNode();
        if (!stage.isValid())
        {
            errors.push_back("PriorityZOrder_Test: 'mainStage' not found.");
            return false;            
        }
        DisplayHandle blueishBox = core.getDisplayObject("blueishBox");
        DisplayHandle blueishBoxLabel = core.getDisplayObject("blueishBoxLabel");
        if (!blueishBox.isValid()) {
            errors.push_back("ObjectFocusInteractivity_Test: 'blueishBox' not found.");
            return false;            
        }
        if (!blueishBoxLabel.isValid()) {
            errors.push_back("ObjectFocusInteractivity_Test: 'blueishBoxLabel' not found.");
            return false;            
        }

        // ✅ void setKeyboardFocus_lua(IDisplayObject* obj)
        DisplayHandle orig_key_focus = core.getKeyboardFocusedObject();
        setKeyboardFocus_lua(blueishBox.as<IDisplayObject>());
        DisplayHandle new_key_focus = core.getKeyboardFocusedObject();
        if (new_key_focus.get() != blueishBox.get()) {
            errors.push_back("ObjectFocusInteractivity_Test: setKeyboardFocus_lua() failed to set focus to 'blueishBox'.");
            ok = false;
        }

        // ✅ bool isKeyboardFocused_lua(const IDisplayObject* obj)
        if (!isKeyboardFocused_lua(blueishBox.as<IDisplayObject>())) {
            errors.push_back("ObjectFocusInteractivity_Test: isKeyboardFocused_lua() returned false for focused 'blueishBox'.");
            ok = false;
        }
        setKeyboardFocus_lua(orig_key_focus.as<IDisplayObject>()); // restore original focus

        // ✅ void Core::pushMouseEvent_lua(const sol::table& eventTable);
        // ✅ void Core::pumpEventsOnce_lua();
        float mx = static_cast<float>(blueishBox->getX() + blueishBox->getWidth() / 2);
        float my = static_cast<float>(blueishBox->getY() + blueishBox->getHeight() / 2);
        sol::state_view lua = getLua();
        sol::table args = lua.create_table();
        args["type"] = "MouseMove"; // or the integer value if required
        args["x"] = mx;
        args["y"] = my;
        pushMouseEvent_lua(args);
        pumpEventsOnce_lua();
        // ✅ bool isMouseHovered_lua(const IDisplayObject* obj)
        if (!isMouseHovered_lua(blueishBox.as<IDisplayObject>())) {
            errors.push_back("ObjectFocusInteractivity_Test: isMouseHovered_lua() returned false for hovered 'blueishBox'.");
            ok = false;
        }
        // ✅ bool IDisplayObject::isMouseHovered()
        args["x"] = 0.0f;
        args["y"] = 0.0f;
        core.pushMouseEvent(args);
        core.pumpEventsOnce();
        if (blueishBox->isMouseHovered()) {
            errors.push_back("ObjectFocusInteractivity_Test: isMouseHovered_lua() returned true for non-hovered 'blueishBox'.");
            ok = false;
        }
        // ✅ bool isClickable_lua(const IDisplayObject* obj)
        if (!blueishBox->isClickable()) {
            errors.push_back("ObjectFocusInteractivity_Test: isClickable_lua() returned false for 'blueishBox' (expected true).");
            ok = false; 
        }
        if (isClickable_lua(blueishBoxLabel.as<IDisplayObject>())) {
            errors.push_back("ObjectFocusInteractivity_Test: isClickable_lua() returned true for 'blueishBoxLabel' (expected false).");
            ok = false; 
        }
        // ✅ void IDisplayObject::setClickable(bool clickable)
        blueishBoxLabel->setClickable(true);
        if (!blueishBoxLabel->isClickable()) {
            errors.push_back("ObjectFocusInteractivity_Test: setClickable_lua() returned false for 'blueishBoxLabel' (expected true).");
            ok = false; 
        }
        // ✅ void setClickable_lua(IDisplayObject* obj, bool clickable)
        setClickable_lua(blueishBoxLabel.as<IDisplayObject>(), false); // restore original state
        if (blueishBoxLabel->isClickable()) {
            errors.push_back("ObjectFocusInteractivity_Test: setClickable_lua() returned true for 'blueishBoxLabel' (expected false).");
            ok = false;
        }
        // ✅ bool isEnabled_lua(const IDisplayObject* obj)
        if (!isEnabled_lua(blueishBox.as<IDisplayObject>())) {
            errors.push_back("ObjectFocusInteractivity_Test: isEnabled_lua() returned false for enabled 'blueishBox'.");
            ok = false;
        }
        // ✅ void setEnabled_lua(IDisplayObject* obj, bool enabled)
        setEnabled_lua(blueishBox.as<IDisplayObject>(), false);
        if (isEnabled_lua(blueishBox.as<IDisplayObject>())) {
            errors.push_back("ObjectFocusInteractivity_Test: isEnabled_lua() returned true for disabled 'blueishBox'.");
            ok = false;
        }
        // ✅ void setEnabled_lua(IDisplayObject* obj, bool enabled)
        blueishBox->setEnabled(true); // restore original state
        if (blueishBox->isEnabled() == false) {
            errors.push_back("ObjectFocusInteractivity_Test: Failed to restore enabled state for 'blueishBox'.");
            ok = false;
        }
        // ✅ bool isHidden_lua(const IDisplayObject* obj)
        if (isHidden_lua(blueishBoxLabel.as<IDisplayObject>())) {
            errors.push_back("ObjectFocusInteractivity_Test: isHidden_lua() returned true for visible 'blueishBoxLabel'.");
            ok = false;
        }
        // ✅ void setHidden_lua(IDisplayObject* obj, bool hidden)
        setHidden_lua(blueishBoxLabel.as<IDisplayObject>(), true);
        if (!blueishBoxLabel->isHidden()) {
            errors.push_back("ObjectFocusInteractivity_Test: isHidden_lua() returned false for hidden 'blueishBoxLabel'.");
            ok = false;
        }
        // ✅ void IDisplayObject::setHidden(bool hidden)        
        blueishBoxLabel->setHidden(false);
        // ✅ bool IDisplayObject::isHidden()
        if (blueishBoxLabel->isHidden()) {
            errors.push_back("ObjectFocusInteractivity_Test: Failed to restore hidden state for 'blueishBoxLabel'.");
            ok = false;
        }
        // ✅ void setVisible_lua(IDisplayObject* obj, bool visible)
        setVisible_lua(blueishBox.as<IDisplayObject>(), false);
        // ✅ bool isVisible_lua(const IDisplayObject* obj)
        if (isVisible_lua(blueishBox.as<IDisplayObject>())) {
            errors.push_back("ObjectFocusInteractivity_Test: isVisible_lua() returned true for isVisible 'blueishBox'.");
            ok = false;
        }
        // ✅ void IDisplayObject::setVisible(bool visible)
        blueishBox->setVisible(true);
        // ✅ bool IDisplayObject::isVisible()
        if (!blueishBox->isVisible()) {
            errors.push_back("ObjectFocusInteractivity_Test: Failed to restore visible state for 'blueishBox'.");
            ok = false;
        }

        // Save originals to restore at the end of this block
        int  orig_prio_box   = blueishBox->getTabPriority();
        bool orig_tab_box    = blueishBox->isTabEnabled();
        int  orig_prio_label = blueishBoxLabel->getTabPriority();
        bool orig_tab_label  = blueishBoxLabel->isTabEnabled();

        // ✅ bool isTabEnabled_lua(const IDisplayObject* obj)
        if (!isTabEnabled_lua(blueishBox.as<IDisplayObject>())) {
            errors.push_back("ObjectFocusInteractivity_Test: isTabEnabled_lua() returned false for enabled 'blueishBox'.");
            ok = false;
        }
        if (isTabEnabled_lua(blueishBoxLabel.as<IDisplayObject>())) {
            errors.push_back("ObjectFocusInteractivity_Test: isTabEnabled_lua() returned true for disabled 'blueishBoxLabel'.");
            ok = false;
        }
        // ✅ bool IDisplayObject::isTabEnabled()
        if (!blueishBox->isTabEnabled()) {
            errors.push_back("ObjectFocusInteractivity_Test: IDisplayObject::isTabEnabled() returned false for enabled 'blueishBox'.");
            ok = false;
        }
        if (blueishBoxLabel->isTabEnabled()) {
            errors.push_back("ObjectFocusInteractivity_Test: IDisplayObject::isTabEnabled() returned true for disabled 'blueishBoxLabel'.");
            ok = false;
        }

        // Round-trip via Lua then verify via both Lua and C++
        // ✅ void setTabPriority_lua(IDisplayObject* obj, int index)
        int lua_set_prio = orig_prio_box + 1;
        setTabPriority_lua(blueishBox.as<IDisplayObject>(), lua_set_prio);
        if (blueishBox->getTabPriority() != lua_set_prio) {
            errors.push_back("ObjectFocusInteractivity_Test: setTabPriority_lua() did not take (C++ getter mismatch).");
            ok = false;
        }
        if (getTabPriority_lua(blueishBox.as<IDisplayObject>()) != lua_set_prio) {
            errors.push_back("ObjectFocusInteractivity_Test: setTabPriority_lua() did not take (Lua getter mismatch).");
            ok = false;
        }

        // Set via C++ then verify via both C++ and Lua
        // ✅ void IDisplayObject::setTabPriority(int index)
        int cpp_set_prio = orig_prio_box + 2;
        blueishBox->setTabPriority(cpp_set_prio);
        if (blueishBox->getTabPriority() != cpp_set_prio) {
            errors.push_back("ObjectFocusInteractivity_Test: IDisplayObject::setTabPriority() did not take (C++ getter mismatch).");
            ok = false;
        }
        if (getTabPriority_lua(blueishBox.as<IDisplayObject>()) != cpp_set_prio) {
            errors.push_back("ObjectFocusInteractivity_Test: IDisplayObject::setTabPriority() did not take (Lua getter mismatch).");
            ok = false;
        }

        // Toggle off via Lua, verify both sides; then back on
        // ✅ void setTabEnabled_lua(IDisplayObject* obj, bool enabled)
        setTabEnabled_lua(blueishBox.as<IDisplayObject>(), false);
        if (isTabEnabled_lua(blueishBox.as<IDisplayObject>()) || blueishBox->isTabEnabled()) {
            errors.push_back("ObjectFocusInteractivity_Test: setTabEnabled_lua(false) did not take for 'blueishBox'.");
            ok = false;
        }
        // Re-enable; priority should remain unchanged when re-enabling (non-zero prio)
        setTabEnabled_lua(blueishBox.as<IDisplayObject>(), true);
        if (!isTabEnabled_lua(blueishBox.as<IDisplayObject>()) || !blueishBox->isTabEnabled()) {
            errors.push_back("ObjectFocusInteractivity_Test: setTabEnabled_lua(true) did not take for 'blueishBox'.");
            ok = false;
        }
        if (blueishBox->getTabPriority() != cpp_set_prio) {
            errors.push_back("ObjectFocusInteractivity_Test: tab priority changed unexpectedly after re-enabling via Lua.");
            ok = false;
        }

        // ✅ void IDisplayObject::setTabEnabled(bool enabled)
        blueishBox->setTabEnabled(false);
        if (isTabEnabled_lua(blueishBox.as<IDisplayObject>()) || blueishBox->isTabEnabled()) {
            errors.push_back("ObjectFocusInteractivity_Test: IDisplayObject::setTabEnabled(false) did not take for 'blueishBox'.");
            ok = false;
        }
        blueishBox->setTabEnabled(true);
        // ✅ void setTabEnabled_lua(IDisplayObject* obj, bool enabled)
        if (!isTabEnabled_lua(blueishBox.as<IDisplayObject>()) || !blueishBox->isTabEnabled()) {
            errors.push_back("ObjectFocusInteractivity_Test: IDisplayObject::setTabEnabled(true) did not take for 'blueishBox'.");
            ok = false;
        }
        // ✅ void IDisplayObject::setTabEnabled(bool enabled)
        if (blueishBox->getTabPriority() != cpp_set_prio) {
            errors.push_back("ObjectFocusInteractivity_Test: tab priority changed unexpectedly after re-enabling via C++.");
            ok = false;
        }

        // Optional: briefly exercise enable/disable on the non-tabbable label, then restore
        // ✅ void setTabEnabled_lua(IDisplayObject* obj, bool enabled)
        setTabEnabled_lua(blueishBoxLabel.as<IDisplayObject>(), true);
        if (!blueishBoxLabel->isTabEnabled()) {
            errors.push_back("ObjectFocusInteractivity_Test: setTabEnabled_lua(true) did not take for 'blueishBoxLabel'.");
            ok = false;
        }
        // ✅ void IDisplayObject::setTabEnabled(bool enabled)
        blueishBoxLabel->setTabEnabled(false);
        if (isTabEnabled_lua(blueishBoxLabel.as<IDisplayObject>())) {
            errors.push_back("ObjectFocusInteractivity_Test: setTabEnabled(false) did not take for 'blueishBoxLabel'.");
            ok = false;
        }

        // Restore original state for both objects to avoid leaking changes
        blueishBox->setTabPriority(orig_prio_box);
        blueishBox->setTabEnabled(orig_tab_box);
        blueishBoxLabel->setTabPriority(orig_prio_label);
        blueishBoxLabel->setTabEnabled(orig_tab_label);
        // Return Results
        return ok;
    } // END: IDisplayObject_test9(std::vector<std::string>& errors)   



    // --- IDisplayObject Unit Test Scaffolding --- //
    bool IDisplayObject_test10(std::vector<std::string>& errors)   
    {
        // ✅ Test Verified
        // 🔄 In Progress
        // ⚠️ Failing     
        // 🚫 Remove
        // ❌ Invalid
        // ☐ Planned

        // --- Geometry & Layout (C++) --- //
        // ✅ void IDisplayObject::setX(int p_x)
        // ✅ void IDisplayObject::setY(int p_y)
        // ✅ float IDisplayObject::getLeft() const
        // ✅ float IDisplayObject::getRight() const
        // ✅ float IDisplayObject::getTop() const
        // ✅ float IDisplayObject::getBottom() const
        // ✅ float IDisplayObject::getLocalLeft() const
        // ✅ float IDisplayObject::getLocalTop() const
        // ✅ IDisplayObject& IDisplayObject::setLocalLeft(float)
        // ✅ IDisplayObject& IDisplayObject::setLocalTop(float)

        // --- Geometry & Layout (Lua) --- //
        // ✅ int getX_lua(const IDisplayObject* obj)
        // ✅ int getY_lua(const IDisplayObject* obj)
        // ✅ int getWidth_lua(const IDisplayObject* obj)
        // ✅ int getHeight_lua(const IDisplayObject* obj)
        // ✅ void setX_lua(IDisplayObject* obj, int p_x)
        // ✅ void setY_lua(IDisplayObject* obj, int p_y)
        // ✅ void setWidth_lua(IDisplayObject* obj, int width)
        // ✅ void setHeight_lua(IDisplayObject* obj, int height)

        // --- Edge Anchors --- //
        // ✅ void IDisplayObject::setAnchorTop(AnchorPoint ap)
        // ✅ void IDisplayObject::setAnchorLeft(AnchorPoint ap)
        // ✅ void IDisplayObject::setAnchorBottom(AnchorPoint ap)
        // ✅ void IDisplayObject::setAnchorRight(AnchorPoint ap)
        // ✅ AnchorPoint getAnchorTop_lua(const IDisplayObject* obj)
        // ✅ AnchorPoint getAnchorLeft_lua(const IDisplayObject* obj)
        // ✅ AnchorPoint getAnchorBottom_lua(const IDisplayObject* obj)
        // ✅ AnchorPoint getAnchorRight_lua(const IDisplayObject* obj)
        // ✅ void setAnchorTop_lua(IDisplayObject* obj, AnchorPoint ap)
        // ✅ void setAnchorLeft_lua(IDisplayObject* obj, AnchorPoint ap)
        // ✅ void setAnchorBottom_lua(IDisplayObject* obj, AnchorPoint ap)
        // ✅ void setAnchorRight_lua(IDisplayObject* obj, AnchorPoint ap)

        // --- World Edge Positions (Lua) --- //
        // ✅ float getLeft_lua(const IDisplayObject* obj)
        // ✅ float getRight_lua(const IDisplayObject* obj)
        // ✅ float getTop_lua(const IDisplayObject* obj)
        // ✅ float getBottom_lua(const IDisplayObject* obj)
        // ✅ void setLeft_lua(IDisplayObject* obj, float p_left)
        // ✅ void setRight_lua(IDisplayObject* obj, float p_right)
        // ✅ void setTop_lua(IDisplayObject* obj, float p_top)
        // ✅ void setBottom_lua(IDisplayObject* obj, float p_bottom)

        // --- Local Edge Positions --- //
        // ✅ float getLocalLeft_lua(const IDisplayObject* obj)
        // ✅ float getLocalRight_lua(const IDisplayObject* obj)
        // ✅ float getLocalTop_lua(const IDisplayObject* obj)
        // ✅ float getLocalBottom_lua(const IDisplayObject* obj)
        // ✅ void setLocalLeft_lua(IDisplayObject* obj, float p_left)
        // ✅ void setLocalRight_lua(IDisplayObject* obj, float p_right)
        // ✅ void setLocalTop_lua(IDisplayObject* obj, float p_top)
        // ✅ void setLocalBottom_lua(IDisplayObject* obj, float p_bottom)



        bool ok = true;
        using AP = AnchorPoint; // assume enum with Left, Center, Right, Top, Middle, Bottom
        auto fail = [&](const std::string& msg){ errors.push_back(msg); return false; };

        auto& core = getCore();
        DisplayHandle stage = core.getRootNode();
        if (!stage.isValid()) return fail("GeomAnchors: 'stage' invalid.");

        // --- Helpers -------------------------------------------------------------

        struct RectF { float l,r,t,b; };
        auto rectf_eq = [](const RectF& a, const RectF& b, float eps=0.001f){
            return std::fabs(a.l-b.l)<=eps && std::fabs(a.r-b.r)<=eps &&
                std::fabs(a.t-b.t)<=eps && std::fabs(a.b-b.b)<=eps;
        };
        auto rectf_delta_eq = [](const RectF& a, const RectF& b, float dx, float dy, float eps=0.001f){
            return std::fabs((a.l+dx)-b.l)<=eps && std::fabs((a.r+dx)-b.r)<=eps &&
                std::fabs((a.t+dy)-b.t)<=eps && std::fabs((a.b+dy)-b.b)<=eps;
        };
        auto world_rect = [](DisplayHandle h)->RectF{
            return RectF{ h->getLeft(), h->getRight(), h->getTop(), h->getBottom() };
        };

        auto make_box = [&](const std::string& name, int x, int y, int w, int h)->DisplayHandle{
            Box::InitStruct init;
            init.name = name; init.x = x; init.y = y; init.width = w; init.height = h;
            init.color = { 16, 96, 192, 255 };
            return core.createDisplayObject("Box", init);
        };

        auto set_anchors = [](IDisplayObject* obj, AP left, AP right, AP top, AP bottom){
            obj->setAnchorLeft(left);
            obj->setAnchorRight(right);
            obj->setAnchorTop(top);
            obj->setAnchorBottom(bottom);
        };

        // --- Scene: one parent, 4 stacked children --------------------------------
        // Coordinates chosen to be simple & symmetric.
        // stage(0,0) 300x300 assumed; parent at (50,50) size 200x200
        DisplayHandle parent = make_box("geom_parent", 50, 50, 200, 200);
        if (!parent.isValid()) return fail("GeomAnchors: failed to create parent.");
        stage->addChild(parent);

        struct Node { DisplayHandle h; std::string name; };
        std::vector<Node> kids;
        kids.push_back({ make_box("ga_A", 10, 10,  60, 30), "ga_A" });  // near top-left
        kids.push_back({ make_box("ga_B", 10, 60,  80, 40), "ga_B" });  // below A
        kids.push_back({ make_box("ga_C", 10, 110, 80, 50), "ga_C" });  // below B
        kids.push_back({ make_box("ga_D", 10, 170, 60, 20), "ga_D" });  // bottom-aligned

        for (auto& n : kids) {
            if (!n.h.isValid()) return fail("GeomAnchors: child create failed for " + n.name);
            parent->addChild(n.h);
        }

        // --- 1) Verify local getters/setters ONCE (C++ & Lua) ----------------------

        // C++ locals
        auto child = kids[0].h; // A
        child->setLocalLeft(20.f);
        child->setLocalTop(15.f);
        if (child->getLocalLeft() != 20.f || child->getLocalTop() != 15.f) {
            return fail("GeomAnchors: C++ local setters/getters mismatch.");
        }

        // Lua forms mirror (callable from C++):
        setLocalLeft_lua(child.as<IDisplayObject>(), 25.f);
        setLocalTop_lua(child.as<IDisplayObject>(), 18.f);
        if (getLocalLeft_lua(child.as<IDisplayObject>()) != 25.f ||
            getLocalTop_lua(child.as<IDisplayObject>())  != 18.f) {
            return fail("GeomAnchors: Lua local setters/getters mismatch.");
        }

        // Local changes should update world edges predictably:
        auto r0 = world_rect(child);
        if (std::fabs(r0.l - (parent->getLeft() + 25.f)) > 0.001f ||
            std::fabs(r0.t - (parent->getTop()  + 18.f)) > 0.001f) {
            return fail("GeomAnchors: world edges didn’t reflect local offsets.");
        }

        // --- 2) Baseline: capture world rects for all children ---------------------

        std::vector<RectF> baseline;
        baseline.reserve(kids.size());
        for (auto& n : kids) baseline.push_back(world_rect(n.h));

        // --- 3) Anchor sweep (translation invariance) ------------------------------
        // We will iterate *individually* over each edge’s 3 anchor options while
        // keeping the other edges on neutral anchors. After changing anchors, we
        // translate the parent by (dx, dy) and assert each child’s world rect
        // moves exactly by (dx, dy). No size changes. No drift.

        const std::vector<AP> H = { AP::LEFT, AP::CENTER, AP::RIGHT };
        const std::vector<AP> V = { AP::TOP,  AP::MIDDLE, AP::BOTTOM };

        auto reset_children = [&](){
            // reset children positions to initial locals so the sweep is deterministic
            // A(10,10), B(10,60), C(10,110), D(10,170)
            kids[0].h->setX(10); kids[0].h->setY(10);
            kids[1].h->setX(10); kids[1].h->setY(60);
            kids[2].h->setX(10); kids[2].h->setY(110);
            kids[3].h->setX(10); kids[3].h->setY(170);
        };

        auto sweep_and_translate = [&](const std::string& label, auto anchorSetter){
            // 1) reset geometry
            reset_children();

            // 2) set neutral anchors: center/middle on everything
            for (auto& n : kids) {
                set_anchors(n.h.as<IDisplayObject>(), AP::CENTER, AP::CENTER, AP::MIDDLE, AP::MIDDLE);
            }

            // 3) apply the variable anchors via provided lambda (per child)
            anchorSetter();

            // 4) capture pre-translate rects
            std::vector<RectF> prior;
            prior.reserve(kids.size());
            for (auto& n : kids) prior.push_back(world_rect(n.h));

            // 5) translate parent only
            const float dx = 13.f, dy = 7.f;
            parent->setX(parent->getX() + int(dx));
            parent->setY(parent->getY() + int(dy));

            // 6) capture post-translate
            for (size_t i=0; i<kids.size(); ++i) {
                auto after = world_rect(kids[i].h);
                // Expect strict translation by (dx, dy); no resizing
                if (!rectf_delta_eq(prior[i], after, dx, dy)) {
                    std::ostringstream oss;
                    oss << "GeomAnchors[" << label << "]: child " << kids[i].name
                        << " did not preserve translation invariance.";
                    return fail(oss.str());
                }
                float w0 = prior[i].r - prior[i].l, h0 = prior[i].b - prior[i].t;
                float w1 = after.r - after.l,     h1 = after.b - after.t;
                if (std::fabs(w0 - w1) > 0.001f || std::fabs(h0 - h1) > 0.001f) {
                    std::ostringstream oss;
                    oss << "GeomAnchors[" << label << "]: child " << kids[i].name
                        << " size changed on parent translation.";
                    return fail(oss.str());
                }
            }

            // 7) restore parent position for next run
            parent->setX(parent->getX() - int(dx));
            parent->setY(parent->getY() - int(dy));
            return true;
        };

        // Sweep left edge anchors: {Left, Center, Right}
        for (auto ap : H) {
            if (!sweep_and_translate("Left="+std::to_string(int(ap)), [&](){
                for (auto& n : kids) {
                    auto* o = n.h.as<IDisplayObject>();
                    set_anchors(o, ap, AP::CENTER, AP::MIDDLE, AP::MIDDLE);
                }
            })) return false;
        }

        // Sweep right edge anchors
        for (auto ap : H) {
            if (!sweep_and_translate("Right="+std::to_string(int(ap)), [&](){
                for (auto& n : kids) {
                    auto* o = n.h.as<IDisplayObject>();
                    set_anchors(o, AP::CENTER, ap, AP::MIDDLE, AP::MIDDLE);
                }
            })) return false;
        }

        // Sweep top edge anchors
        for (auto ap : V) {
            if (!sweep_and_translate("Top="+std::to_string(int(ap)), [&](){
                for (auto& n : kids) {
                    auto* o = n.h.as<IDisplayObject>();
                    set_anchors(o, AP::CENTER, AP::CENTER, ap, AP::MIDDLE);
                }
            })) return false;
        }

        // Sweep bottom edge anchors
        for (auto ap : V) {
            if (!sweep_and_translate("Bottom="+std::to_string(int(ap)), [&](){
                for (auto& n : kids) {
                    auto* o = n.h.as<IDisplayObject>();
                    set_anchors(o, AP::CENTER, AP::CENTER, AP::MIDDLE, ap);
                }
            })) return false;
        }

        // --- 4) Changing anchors alone must NOT move anything (no parent motion) ----
        // Capture state, change anchors, confirm rects equal.
        {
            std::vector<RectF> before;
            for (auto& n : kids) before.push_back(world_rect(n.h));
            for (auto& n : kids) {
                auto* o = n.h.as<IDisplayObject>();
                set_anchors(o, AP::LEFT, AP::RIGHT, AP::TOP, AP::BOTTOM);
            }
            for (size_t i=0;i<kids.size();++i) {
                auto after = world_rect(kids[i].h);
                if (!rectf_eq(before[i], after)) {
                    return fail("GeomAnchors: changing anchors (no parent move) shifted geometry.");
                }
            }
        }

        // --- 5) Lua geometry + anchors + edges -----------------------------------------
        // Use distinct children to avoid interactions; restore state after each sub-test.

        // 5a) Geometry & Layout (Lua) — getters/setters for pos/size
        {
            auto t  = kids[1].h; // B
            auto* to = t.as<IDisplayObject>();

            int x0 = t->getX(), y0 = t->getY();
            int w0 = t->getWidth(), h0 = t->getHeight();

            // Lua getters mirror C++
            if (getX_lua(to) != x0 || getY_lua(to) != y0 ||
                getWidth_lua(to) != w0 || getHeight_lua(to) != h0)
                return fail("GeomAnchors: Lua getX/Y/Width/Height mismatch.");

            // Set via Lua, verify round-trip
            int tx = int(parent->getLeft()) + 77;
            int ty = int(parent->getTop()) + 33;
            setX_lua(to, tx);
            setY_lua(to, ty);
            if (getX_lua(to) != tx || getY_lua(to) != ty)
                return fail("GeomAnchors: setX/Y_lua did not take.");

            setWidth_lua(to, w0 + 11);
            setHeight_lua(to, h0 + 9);
            if (getWidth_lua(to) != w0 + 11 || getHeight_lua(to) != h0 + 9)
                return fail("GeomAnchors: setWidth/Height_lua did not take.");

            // Restore
            setX_lua(to, x0);
            setY_lua(to, y0);
            setWidth_lua(to, w0);
            setHeight_lua(to, h0);
        }

        // 5b) Edge Anchors (Lua) — get/set for each anchor
        {
            auto t  = kids[2].h; // C
            auto* to = t.as<IDisplayObject>();

            setAnchorLeft_lua(to,  AP::RIGHT);
            setAnchorRight_lua(to, AP::LEFT);
            setAnchorTop_lua(to,   AP::BOTTOM);
            setAnchorBottom_lua(to,AP::TOP);

            if (getAnchorLeft_lua(to)   != AP::RIGHT || to->getAnchorLeft()   != AP::RIGHT)
                return fail("GeomAnchors: set/getAnchorLeft_lua mismatch.");
            if (getAnchorRight_lua(to)  != AP::LEFT  || to->getAnchorRight()  != AP::LEFT)
                return fail("GeomAnchors: set/getAnchorRight_lua mismatch.");
            if (getAnchorTop_lua(to)    != AP::BOTTOM|| to->getAnchorTop()    != AP::BOTTOM)
                return fail("GeomAnchors: set/getAnchorTop_lua mismatch.");
            if (getAnchorBottom_lua(to) != AP::TOP   || to->getAnchorBottom() != AP::TOP)
                return fail("GeomAnchors: set/getAnchorBottom_lua mismatch.");

            // Restore neutral
            setAnchorLeft_lua(to,  AP::LEFT);
            setAnchorRight_lua(to, AP::RIGHT);
            setAnchorTop_lua(to,   AP::TOP);
            setAnchorBottom_lua(to,AP::BOTTOM);
        }

        // 5c) World edges (Lua) — get/set each edge, ensure only that edge moves
        {
            auto t  = kids[3].h; // D
            auto* to = t.as<IDisplayObject>();
            auto eps = 0.001f;

            float l0 = t->getLeft(), r0 = t->getRight();
            float t0 = t->getTop(),  b0 = t->getBottom();

            // Lua getters mirror C++
            if (std::fabs(getLeft_lua(to) - l0) > eps ||
                std::fabs(getRight_lua(to) - r0) > eps ||
                std::fabs(getTop_lua(to) - t0) > eps ||
                std::fabs(getBottom_lua(to) - b0) > eps)
                return fail("GeomAnchors: Lua getLeft/Right/Top/Bottom mismatch.");

            // setLeft: left changes, right stays
            setLeft_lua(to, l0 + 5);
            if (std::fabs(t->getLeft() - (l0 + 5)) > eps || std::fabs(t->getRight() - r0) > eps)
                return fail("GeomAnchors: setLeft_lua moved unexpected edges.");
            setLeft_lua(to, l0); // restore

            // setRight: right changes, left stays
            setRight_lua(to, r0 + 7);
            if (std::fabs(t->getRight() - (r0 + 7)) > eps || std::fabs(t->getLeft() - l0) > eps)
                return fail("GeomAnchors: setRight_lua moved unexpected edges.");
            setRight_lua(to, r0); // restore

            // setTop: top changes, bottom stays
            setTop_lua(to, t0 + 3);
            if (std::fabs(t->getTop() - (t0 + 3)) > eps || std::fabs(t->getBottom() - b0) > eps)
                return fail("GeomAnchors: setTop_lua moved unexpected edges.");
            setTop_lua(to, t0); // restore

            // setBottom: bottom changes, top stays
            setBottom_lua(to, b0 + 6);
            if (std::fabs(t->getBottom() - (b0 + 6)) > eps || std::fabs(t->getTop() - t0) > eps)
                return fail("GeomAnchors: setBottom_lua moved unexpected edges.");
            setBottom_lua(to, b0); // restore
        }

        // 5d) Local edges (Lua) — right/bottom local set/get
        {
            auto t  = kids[0].h; // A
            auto* to = t.as<IDisplayObject>();

            float lr0 = getLocalRight_lua(to);
            float lb0 = getLocalBottom_lua(to);

            setLocalRight_lua(to, lr0 + 10.f);
            if (std::fabs(getLocalRight_lua(to) - (lr0 + 10.f)) > 0.001f)
                return fail("GeomAnchors: setLocalRight_lua did not take.");
            setLocalRight_lua(to, lr0); // restore

            setLocalBottom_lua(to, lb0 + 12.f);
            if (std::fabs(getLocalBottom_lua(to) - (lb0 + 12.f)) > 0.001f)
                return fail("GeomAnchors: setLocalBottom_lua did not take.");
            setLocalBottom_lua(to, lb0); // restore
        }

        // --- Cleanup (optional: keep for visual inspection while developing) -------
        core.destroyDisplayObject("ga_A");
        core.destroyDisplayObject("ga_B");
        core.destroyDisplayObject("ga_C");
        core.destroyDisplayObject("ga_D");
        core.destroyDisplayObject("geom_parent");

        return ok;
    } // IDisplayObject_test10(std::vector<std::string>& errors)   


    // --- Orphan Retention Policy and Grace Period --- //
    bool IDisplayObject_test11(std::vector<std::string>& errors)   
    {
        // ✅ Test Verified
        // 🔄 In Progress
        // ⚠️ Failing     
        // 🚫 Remove
        // ❌ Invalid
        // ☐ Planned

        // --- 🔄 Orphan Retention Policy --- //
        // ☐ IDisplayObject::OrphanRetentionPolicy orphanPolicyFromString_lua(IDisplayObject* obj, const std::string& s)
        // ☐ std::string orphanPolicyToString_lua(IDisplayObject* obj, IDisplayObject::OrphanRetentionPolicy p)
        // ☐ void setOrphanRetentionPolicy_lua(IDisplayObject* obj, const std::string& policyStr)
        // ☐ std::string getOrphanRetentionPolicyString_lua(IDisplayObject* obj)

        // ☐ IDisplayObject::OrphanRetentionPolicy IDisplayObject::orphanPolicyFromString(const std::string& s)
        // ☐ std::string IDisplayObject::orphanPolicyToString(IDisplayObject::OrphanRetentionPolicy p)
        // ☐ void IDisplayObject::setOrphanRetentionPolicy(const std::string& policyStr)
        // ☐ std::string IDisplayObject::getOrphanRetentionPolicyString()

        // 🔄 Lua-accessible accessors for orphan grace (milliseconds)
        // ☐ int getOrphanGrace_lua(const IDisplayObject* obj)
        // ☐ void setOrphanGrace_lua(IDisplayObject* obj, std::chrono::milliseconds grace)

        // ☐ int IDisplayObject::getOrphanGrace()
        // ☐ void IDisplayObject::setOrphanGrace(std::chrono::milliseconds grace)

        bool ok = true;

        return ok;
    } // IDisplayObject_test11(std::vector<std::string>& errors)   


    // --- Lua Integration Tests --- //

    bool IDisplayObject_LUA_Tests(std::vector<std::string>& errors)
    {  
        UnitTests& ut = UnitTests::getInstance();
        return ut.run_lua_tests(errors, ut.getLuaFilename());
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
        ut.add_test("Object Focus and Interactivity", IDisplayObject_test9);
        ut.add_test("Geometry Anchors", IDisplayObject_test10);
        ut.add_test("Orphan Retention Policy and Grace Period", IDisplayObject_test11);

        ut.setLuaFilename("src/IDisplayObject_UnitTests.lua"); // Lua test script path
        ut.add_test("Lua: '" + ut.getLuaFilename() + "'", IDisplayObject_LUA_Tests, false); // Not yet implemented

        return ut.run_all("IDisplayObject");
    } // END: IDisplayObject_UnitTests()



} // END: namespace SDOM
