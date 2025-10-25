// IDisplayObject_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_Stage.hpp>

namespace SDOM
{
    // --- IDisplayObject Unit Test Scaffolding --- //
    bool IDisplayObject_test0(std::vector<std::string>& errors)   
    {
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
            // void cleanAll_lua(IDisplayObject* obj);
            // bool getDirty_lua(const IDisplayObject* obj);
            // void setDirty_lua(IDisplayObject* obj); 
            // bool isDirty_lua(const IDisplayObject* obj); 

        // --- Debug/Utility --- //
            // void printTree_lua(const IDisplayObject* obj);  // Tested Manually while designing these Unit Tests

        bool ok = true;
        // Core& core = getCore();
        // DisplayHandle stage = core.getDisplayObject("mainStage");
        // if (!stage.isValid())
        // {
        //     errors.push_back("IDisplayObject_test4: 'mainStage' object not found for dirty/state test.");
        //     return false;
        // }

        // core.getStage()->printTree();  // debugging aid that shows a tree of DisplayObjects in the DOM tree
        // stage->cleanAll();

        // // Get a DisplayObject to test
        // DisplayHandle blueishBox = core.getDisplayObject("blueishBox");
        // if (!blueishBox.isValid()) 
        // {
        //     errors.push_back("IDisplayObject_test4: blueishBox object not found for dirty/state test.");
        //     return false;
        // }
        // // bool initial_dirty_box = blueishBox->isDirty();


        // // Get a child object to confirm recursive cleaning.
        // DisplayHandle label = blueishBox->getChild("blueishBoxLabel");
        // if (!label.isValid())
        // {
        //     errors.push_back("IDisplayObject_test4: Expected 'blueishBoxLabel' child to exist.");
        //     return false;
        // }
        // // bool initial_dirty_label = label->isDirty();

        // // Initially, both objects should be clean
        // if (blueishBox->isDirty())
        // {
        //     errors.push_back("IDisplayObject_test4: blueishBox should be clean initially.");
        //     ok = false;
        // }
        // if (label->isDirty())
        // {
        //     errors.push_back("IDisplayObject_test4: blueishBoxLabel should be clean initially.");
        //     ok = false;
        // }

        // // Mark only the child as dirty
        // label->setDirty(true);
        // if (!label->isDirty())
        // {
        //     errors.push_back("IDisplayObject_test4: blueishBoxLabel should be dirty after setDirty(true).");
        //     ok = false;
        // }
        // if (blueishBox->isDirty())
        // {
        //     errors.push_back("IDisplayObject_test4: blueishBox should still be clean after child setDirty(true).");
        //     ok = false;
        // }

        // // Mark the parent as dirty
        // blueishBox->setDirty(true);
        // if (!blueishBox->isDirty())
        // {
        //     errors.push_back("IDisplayObject_test4: blueishBox should be dirty after setDirty(true).");
        //     ok = false;
        // }
        // if (!label->isDirty())
        // {
        //     errors.push_back("IDisplayObject_test4: blueishBoxLabel should still be dirty after parent setDirty(true).");
        //     ok = false;
        // }

        // // Recursively scrub away the dirtiness
        // core.getStage()->cleanAll();

        // // Both objects should now be clean
        // if (blueishBox->isDirty())
        // {
        //     errors.push_back("IDisplayObject_test4: blueishBox still dirty after cleanAll().");
        //     ok = false;
        // }
        // if (label->isDirty())
        // {
        //     errors.push_back("IDisplayObject_test4: blueishBoxLabel still dirty after cleanAll().");
        //     ok = false;
        // }

        return ok;
    } // IDisplayObject_test4(std::vector<std::string>& errors)   










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




        ut.add_test("Lua Integration", IDisplayObject_LUA_Tests);

        return ut.run_all("IDisplayObject");
    } // END: IDisplayObject_UnitTests()



} // END: namespace SDOM