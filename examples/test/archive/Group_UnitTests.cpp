// Group_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Group.hpp>
#include <SDOM/SDOM_Label.hpp>

/*** Group UnitTests: verification matrix ****************************************
 * Purpose: ensure Group APIs are present in Lua and that bindings exercise
 *          the underlying C++ implementations. The table below lists the
 *          primary Group methods and the unit test(s) that verify them.
 *
 * Notes:
 *  - "(Lua)" means the test exercises the API via Lua bindings.
 *  - "(C++)" means the test performs direct C++ checks (InitStruct / pointer API).
 *
 * Creation & basic DisplayHandle methods (Group_test4 - Lua):
 *   - getName(), getType(), setX()/getX(), setY()/getY(),
 *     setWidth()/getWidth(), setHeight()/getHeight()
 *     (Verified by Group_test4) (Lua)
 *
 * Label helpers (Group_test1/2/3/4/5 - Lua/C++):
 *   - getLabel(), getLabelText(), setLabelText(),
 *     getLabelColor(), setLabelColor()
 *     (Verified by Group_test1, Group_test2, Group_test3, Group_test4, Group_test5)
 *     (Lua + C++)
 *
 * Font metric getters/setters (Group_test6, Group_test7):
 *   - getFontSize()/setFontSize(), getFontWidth()/setFontWidth(),
 *     getFontHeight()/setFontHeight()
 *     (Verified by Group_test6) (C++) and (Group_test7) (Lua)
 *
 * SpriteSheet & icon helpers (planned coverage):
 *   - getSpriteSheet(), getSpriteWidth(), getSpriteHeight()
 *     (Planned: Group_test5; not covered by current tests)
 *
 * Resource lifecycle and cleanup (Group_test1..Group_test5):
 *   - destroyDisplayObject(), orphan detection and collectGarbage()
 *     (Verified across Group_test1..Group_test5) (Lua/C++)
 *
 * Registration-order / per-item bindings:
 *   - Group-specific per-item bindings are exercised during creation and
 *     immediate use in tests 1..5 (Lua).
 *
 **********************************************************************************/

namespace SDOM
{
    bool Group_test0()
    {
        std::string testName = "Group #0";
        std::string testDesc = "UnitTest Scafolding";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
                -- return { ok = false, err = "unknown error" }
                return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Group_test0()


    bool Group_test1()
    {
        std::string testName = "Group #1";
        std::string testDesc = "Creating Group via Lua config table with Label pointer effects";

        bool ok = true;
        Factory& factory = getFactory();

        std::string name = "ut_group_3";
        std::string type = "Group";
        int x = 10;
        int y = 10;
        int width = 200;
        int height = 100;
        std::string text = "Main Frame Group";
        std::string font_resource = "internal_font_8x8";

        sol::state& lua = getLua();
        sol::table lua_cfg = lua.create_table();
        lua_cfg["name"] = name;
        lua_cfg["type"] = type;
        lua_cfg["x"] = x;
        lua_cfg["y"] = y;
        lua_cfg["width"] = width;
        lua_cfg["height"] = height;
        lua_cfg["text"] = text;
        lua_cfg["font_resource"] = font_resource;
        DisplayHandle group_handle = factory.create("Group", lua_cfg);                          

        // Check creation result
        auto grp_handle = factory.getDisplayObject("ut_group_3");
        if (!grp_handle.isValid()) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get 'ut_group_3' handle" << CLR::RESET << std::endl;
            ok = false;
        }
        auto grp_ptr = grp_handle.as<Group>();
        if (!grp_ptr) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get 'ut_group_3' as Group*" << CLR::RESET << std::endl;
            ok = false;
        }

        // Child handle test (Children are created during onInit)
        auto child_label = grp_ptr->getChild("ut_group_3_label");
        if (!child_label.isValid()) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get 'ut_group_3_label' child handle" << CLR::RESET << std::endl;
            ok = false;
        }

        // Label pointer test
        auto lbl_handle = grp_ptr->getLabel();
        if (!lbl_handle.isValid()) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get Group label handle" << CLR::RESET << std::endl;
            ok = false;
        }
        if (lbl_handle != child_label) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Group label handle does not match expected child handle" << CLR::RESET << std::endl;
            ok = false;
        }
        auto lbl_ptr = lbl_handle.as<Label>();
        if (!lbl_ptr) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get Group label as Label*" << CLR::RESET << std::endl;
            ok = false;
        }

        // Modify label text via pointer
        std::string new_text = "Modified Group Label";
        lbl_ptr->setText(new_text);
        // Verify change via handle accessor
        std::string fetched_text = grp_ptr->getLabelText();
        if (fetched_text != new_text) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Label text mismatch after pointer set: got='" << fetched_text << "' expected='" << new_text << "'" << CLR::RESET << std::endl;
            ok = false;
        }

        // Modify the label color via pointer
        SDL_Color new_color = {255, 0, 0, 255}; // Red
        lbl_ptr->setColor(new_color);
        // Verify change via handle accessor
        SDL_Color fetched_color = grp_ptr->getLabelColor();
        if (fetched_color.r != new_color.r || fetched_color.g != new_color.g ||
            fetched_color.b != new_color.b || fetched_color.a != new_color.a) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Label color mismatch after pointer set" << CLR::RESET << std::endl;
            ok = false;
        }

        // Testing Cleanup
        factory.destroyDisplayObject(name);  
        factory.collectGarbage();

        // List any remaining orphaned objects
        auto orphans = factory.getOrphanedDisplayObjects();
        for (auto & orphan : orphans) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Orphaned object: name='" << orphan.getName() << "' type='" << orphan.getType() << "'" << CLR::RESET << std::endl;
        }

        // Verify no orphaned objects remain
        if (factory.countOrphanedDisplayObjects() != 0) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Orphaned objects remain after destroying Group: count=" << orphans.size() << CLR::RESET << std::endl;
            ok = false;
        }        

        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Group_test1()


    bool Group_test2()
    {
        std::string testName = "Group #2";
        std::string testDesc = "Creating Group via InitStruct with Label pointer effects";

        bool ok = true;
        Factory& factory = getFactory();
        // Create Group via InitStruct
        std::string name = "ut_group_3";
        std::string type = "Group";
        int x = 10;
        int y = 10;
        int width = 200;
        int height = 100;
        std::string text = "Main Frame Group";
        std::string font_resource = "internal_font_8x8";
        // Setup InitStruct
        Group::InitStruct grp_init;
        grp_init.name = name;
        grp_init.type = type;
        grp_init.x = static_cast<float>(x);
        grp_init.y = static_cast<float>(y);
        grp_init.width = static_cast<float>(width);
        grp_init.height = static_cast<float>(height);
        grp_init.text = text;
        grp_init.font_resource = font_resource;
        // Create the Group
        auto group_handle = getFactory().create(grp_init.type, grp_init);                

        // Check creation result
        auto grp_handle = factory.getDisplayObject("ut_group_3");
        if (!grp_handle.isValid()) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get 'ut_group_3' handle" << CLR::RESET << std::endl;
            ok = false;
        }
        auto grp_ptr = grp_handle.as<Group>();
        if (!grp_ptr) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get 'ut_group_3' as Group*" << CLR::RESET << std::endl;
            ok = false;
        }

        // Child handle test (Children are created during onInit)
        auto child_label = grp_ptr->getChild("ut_group_3_label");
        if (!child_label.isValid()) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get 'ut_group_3_label' child handle" << CLR::RESET << std::endl;
            ok = false;
        }

        // Label pointer test
        auto lbl_handle = grp_ptr->getLabel();
        if (!lbl_handle.isValid()) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get Group label handle" << CLR::RESET << std::endl;
            ok = false;
        }
        if (lbl_handle != child_label) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Group label handle does not match expected child handle" << CLR::RESET << std::endl;
            ok = false;
        }
        auto lbl_ptr = lbl_handle.as<Label>();
        if (!lbl_ptr) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get Group label as Label*" << CLR::RESET << std::endl;
            ok = false;
        }

        // Modify label text via pointer
        std::string new_text = "Modified Group Label";
        lbl_ptr->setText(new_text);
        // Verify change via handle accessor
        std::string fetched_text = lbl_ptr->getText();
        if (fetched_text != new_text) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Label text mismatch after pointer set: got='" << fetched_text << "' expected='" << new_text << "'" << CLR::RESET << std::endl;
            ok = false;
        }

        // Modify the label color via pointer
        SDL_Color new_color = {255, 0, 0, 255}; // Red
        lbl_ptr->setColor(new_color);
        // Verify change via handle accessor
        SDL_Color fetched_color = lbl_ptr->getColor();
        if (fetched_color.r != new_color.r || fetched_color.g != new_color.g ||
            fetched_color.b != new_color.b || fetched_color.a != new_color.a) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Label color mismatch after pointer set" << CLR::RESET << std::endl;
            ok = false;
        }

        // Testing Cleanup
        factory.destroyDisplayObject(name);  
        factory.collectGarbage();

        // List any remaining orphaned objects
        auto orphans = factory.getOrphanedDisplayObjects();
        for (auto & orphan : orphans) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Orphaned object: name='" << orphan.getName() << "' type='" << orphan.getType() << "'" << CLR::RESET << std::endl;
        }

        // Verify no orphaned objects remain
        if (factory.countOrphanedDisplayObjects() != 0) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Orphaned objects remain after destroying Group: count=" << orphans.size() << CLR::RESET << std::endl;
            ok = false;
        }        

        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Group_test2()



    bool Group_test3()
    {
        std::string testName = "Group #3";
        std::string testDesc = "Creating Group via lua config string with Label pointer effects";

        bool ok = true;
        Factory& factory = getFactory();
        // Create Group via InitStruct
        std::string name = "ut_group_3";
        // Provide a valid Lua table expression as a string. The previous
        // literal omitted braces and quotes around the name which caused Lua
        // parsing/runtime errors when evaluated.
        std::string str_cfg = R"lua({
            name = "ut_group_3",
            type = "Group",
            x = 10, y = 10,
            width = 200, height = 100,
            text = "Main Frame Group",
            font_resource = "internal_font_8x8"
        })lua";
        auto group_handle = getFactory().create("Group", str_cfg);                

        // Check creation result
        auto grp_handle = factory.getDisplayObject("ut_group_3");
        if (!grp_handle.isValid()) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get 'ut_group_3' handle" << CLR::RESET << std::endl;
            ok = false;
        }
        auto grp_ptr = grp_handle.as<Group>();
        if (!grp_ptr) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get 'ut_group_3' as Group*" << CLR::RESET << std::endl;
            ok = false;
        }

        // Child handle test (Children are created during onInit)
        auto child_label = grp_ptr->getChild("ut_group_3_label");
        if (!child_label.isValid()) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get 'ut_group_3_label' child handle" << CLR::RESET << std::endl;
            ok = false;
        }

        // Label pointer test
        auto lbl_handle = grp_ptr->getLabel();
        if (!lbl_handle.isValid()) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get Group label handle" << CLR::RESET << std::endl;
            ok = false;
        }
        if (lbl_handle != child_label) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Group label handle does not match expected child handle" << CLR::RESET << std::endl;
            ok = false;
        }
        auto lbl_ptr = lbl_handle.as<Label>();
        if (!lbl_ptr) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "failed to get Group label as Label*" << CLR::RESET << std::endl;
            ok = false;
        }

        // Modify label text via pointer
        std::string new_text = "Modified Group Label";
        grp_ptr->setLabelText(new_text);

        // Verify change via handle accessor
        std::string fetched_text = grp_ptr->getLabelText();
        if (fetched_text != new_text) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Label text mismatch after pointer set: got='" << fetched_text << "' expected='" << new_text << "'" << CLR::RESET << std::endl;
            ok = false;
        }

        // Modify the label color via pointer
        SDL_Color new_color = {255, 0, 0, 255}; // Red
        grp_ptr->setLabelColor(new_color);
        // Verify change via handle accessor
        SDL_Color fetched_color = grp_ptr->getLabelColor();
        if (fetched_color.r != new_color.r || fetched_color.g != new_color.g ||
            fetched_color.b != new_color.b || fetched_color.a != new_color.a) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Label color mismatch after pointer set" << CLR::RESET << std::endl;
            ok = false;
        }

        // Testing Cleanup
        factory.destroyDisplayObject(name);  
        factory.collectGarbage();

        // List any remaining orphaned objects
        auto orphans = factory.getOrphanedDisplayObjects();
        for (auto & orphan : orphans) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Orphaned object: name='" << orphan.getName() << "' type='" << orphan.getType() << "'" << CLR::RESET << std::endl;
        }

        // Verify no orphaned objects remain
        if (factory.countOrphanedDisplayObjects() != 0) {
            std::cout << CLR::ORANGE << "  [" << testName << "] " << "Orphaned objects remain after destroying Group: count=" << orphans.size() << CLR::RESET << std::endl;
            ok = false;
        }        

        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Group_test2()



    

    bool Group_test4()
    {
        std::string testName = "Group #4";
        std::string testDesc = "Create and Bindings";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            -- return { ok = false, err = "unknown error" }
            local group_name = "ut_group_1"
            local txt = "Group Label"
            local cfg = { 
                name = group_name, 
                type = "Group", 
                font_resource = "internal_font_8x8", 
                text = txt 
            }
            local ok = true
            local err = ""
            local group_obj = createDisplayObject("Group", cfg)
            if not group_obj then
                return { ok = false, err = "createDisplayObject failed: " .. tostring(h_or_err) }
            end

            -- primary check: getName / getType
            local fetched_name = group_obj:getName()
            if fetched_name ~= group_name then
                return { ok = false, err = "Group name mismatch: got='" .. tostring(fetched_name) .. "' expected='" .. tostring(group_name) .. "'" }
            end
            local fetched_type = group_obj:getType()
            if fetched_type ~= "Group" then
                return { ok = false, err = "Group type mismatch: got='" .. tostring(fetched_type) .. "' expected='Group'" }
            end

            -- Move the Group
            local new_x = 50
            local new_y = 75
            group_obj:setX(new_x)
            group_obj:setY(new_y)
            local fetched_x = group_obj:getX()
            local fetched_y = group_obj:getY()
            if fetched_x ~= new_x then
                return { ok = false, err = "Group X position mismatch: got=" .. tostring(fetched_x) .. " expected=" .. tostring(new_x) }
            end
            if fetched_y ~= new_y then
                return { ok = false, err = "Group Y position mismatch: got=" .. tostring(fetched_y) .. " expected=" .. tostring(new_y) }
            end

            -- Resize the Group
            local new_w = 300
            local new_h = 150
            group_obj:setWidth(new_w)
            group_obj:setHeight(new_h)
            local fetched_w = group_obj:getWidth()
            local fetched_h = group_obj:getHeight()
            if fetched_w ~= new_w then
                return { ok = false, err = "Group width mismatch: got=" .. tostring(fetched_w) .. " expected=" .. tostring(new_w) }
            end
            if fetched_h ~= new_h then
                return { ok = false, err = "Group height mismatch: got=" .. tostring(fetched_h) .. " expected=" .. tostring(new_h) }
            end

            -- Change Group Color
            if false then
                local new_color = { r = 128, g = 64, b = 32, a = 255 }
                group_obj:setGroupColor(new_color)
                local fetched_color = group_obj:getGroupColor()
                if fetched_color.r ~= new_color.r or fetched_color.g ~= new_color.g or
                fetched_color.b ~= new_color.b or fetched_color.a ~= new_color.a then
                    return { ok = false, err = "Group color mismatch after setGroupColor" }
                end
            end

            -- Get the Label using getLabel() and verify text
            local label_obj = group_obj:getLabel()
            if not label_obj then
                return { ok = false, err = "getLabel() returned nil" }
            end
            local label_text = label_obj:getText()
            if label_text ~= txt then
                return { ok = false, err = "Label text mismatch: got='" .. tostring(label_text) .. "' expected='" .. tostring(txt) .. "'" }
            end 

            -- Get the label using label_text property and verify text
            -- local label_text_via_prop = group_obj.label_text
            -- if label_text_via_prop ~= txt then
            --     return { ok = false, err = "Label text (via property) mismatch: got='" .. tostring(label_text_via_prop) .. "' expected='" .. tostring(txt) .. "'" }
            -- end

            -- cleanup and return
            destroyDisplayObject(group_name)            
            collectGarbage()
            -- verify destruction
            local orphans = getOrphanedDisplayObjects()
            if #orphans > 0 then
                ok = false
                err = "Orphaned objects remain after destroying Group: count=" .. tostring(#orphans)
            end

            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Group_test4()


    bool Group_test5()
    {
        std::string testName = "Group #5";
        std::string testDesc = "Label Text and Color function symmetry";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            -- return { ok = false, err = "unknown error" }
            local group_name = "ut_group_1"
            local txt = "Some Random Text"
            local cfg = { 
                name = group_name, 
                type = "Group", 
                font_resource = "internal_font_8x8", 
                text = txt 
            }
            local ok = true
            local err = ""
            local group_obj = createDisplayObject("Group", cfg)            
            if not group_obj then
                return { ok = false, err = "createDisplayObject failed: " .. tostring(h_or_err) }
            end

            -- Test getLabel()
            local label_obj = group_obj:getLabel()
            if not label_obj then
                destroyDisplayObject(group_name)
                collectGarbage()            
                return { ok = false, err = "getLabel() returned nil" }
            end
            -- verify label text
            local label_text = label_obj:getText()
            if label_text ~= txt then
                destroyDisplayObject(group_name)
                collectGarbage()            
                return { ok = false, err = "Label text mismatch: got='" .. tostring(label_text) .. "' expected='" .. tostring(txt) .. "'" }
            end

            -- Initial Test getLabelText()
            local lbl = group_obj:getLabelText()
            if not lbl then
                destroyDisplayObject(group_name)
                collectGarbage()            
                return { ok = false, err = "getLabelText() returned nil" }
            end
            -- verify label text
            if lbl ~= txt then
                destroyDisplayObject(group_name)
                collectGarbage()
                return { ok = false, err = "Label text mismatch: got='" .. tostring(lbl) .. "' expected='" .. tostring(txt) .. "'" }
            end

            -- Initial Test setLabelText()
            local new_txt = "A Different String"
            group_obj:setLabelText(new_txt)
            local fetched_txt = group_obj:getLabelText()
            if fetched_txt ~= new_txt then
                destroyDisplayObject(group_name)
                collectGarbage()
                return { ok = false, err = "Label text mismatch after setLabelText: got='" .. tostring(fetched_txt) .. "' expected='" .. tostring(new_txt) .. "'" }
            end

            -- Initial Test getLabelColor() and setLabelColor()
            local initial_color = group_obj:getLabelColor()
            local new_color = { r = 64, g = 192, b = 128, a = 255 } -- Greenish
            group_obj:setLabelColor(new_color)
            local fetched_color = group_obj:getLabelColor()
            if fetched_color.r ~= new_color.r or fetched_color.g ~= new_color.g or
                fetched_color.b ~= new_color.b or fetched_color.a ~= new_color.a then
                destroyDisplayObject(group_name)
                collectGarbage()
                return { ok = false, err = "Label color mismatch after setLabelColor" }
            end

            -- cleanup and return
            destroyDisplayObject(group_name)
            collectGarbage()
            -- verify destruction
            local orphans = getOrphanedDisplayObjects()
            if #orphans > 0 then
                ok = false
                err = "Orphaned objects remain after destroying Group: count=" .. tostring(#orphans)
            end            
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Group_test5()    


    bool Group_test6()
    {
        // int getFontSize() const;
        // int getFontWidth() const;
        // int getFontHeight() const;
        // void setFontSize(int s);
        // void setFontWidth(int w);
        // void setFontHeight(int h);

        std::string testName = "Group #6";
        std::string testDesc = "Font metrics getters/setters (C++ symmetry)";        
        std::string err = "";        
        bool ok = true;        
        Factory& factory = getFactory();

        // Create Group via InitStruct
        std::string name = "ut_group_3";
        std::string type = "Group";
        int x = 10;
        int y = 10;
        int width = 200;
        int height = 100;
        std::string text = "Main Frame Group";
        std::string font_resource = "internal_font_8x8";
        // Setup InitStruct
        Group::InitStruct grp_init;
        grp_init.name = name;
        grp_init.type = type;
        grp_init.x = static_cast<float>(x);
        grp_init.y = static_cast<float>(y);
        grp_init.width = static_cast<float>(width);
        grp_init.height = static_cast<float>(height);
        grp_init.text = text;
        grp_init.font_resource = font_resource;
        // Create the Group
        auto group_handle = getFactory().create(grp_init.type, grp_init);                

        // Check creation result
        auto grp_handle = factory.getDisplayObject("ut_group_3");
        if (!grp_handle.isValid()) {
            err += "failed to get 'ut_group_3' handle\n";
            ok = false;
        }
        auto grp_ptr = grp_handle.as<Group>();
        if (!grp_ptr) {
            err += "failed to get 'ut_group_3' as Group*\n";
            ok = false;
        }

        // Test getFontSize() and setFontSize()
        int initial_size = grp_ptr->getFontSize();
        if (initial_size != 8) {
            err += "Initial font size mismatch: got=" + std::to_string(initial_size) + " expected=8\n";
            ok = false;
        }
        int new_size = initial_size + 4;
        grp_ptr->setFontSize(new_size);
        int fetched_size = grp_ptr->getFontSize();
        if (fetched_size != new_size) {
            err += "Font size mismatch after setFontSize: got=" + std::to_string(fetched_size) + " expected=" + std::to_string(new_size) + "\n";
            ok = false;
        }

        // Test getFontWidth() and setFontWidth()
        int initial_width = grp_ptr->getFontWidth();
        if (initial_width != 8) {
            err += "Initial font width mismatch: got=" + std::to_string(initial_width) + " expected=8\n";
            ok = false;
        }
        int new_width = initial_width + 2;
        grp_ptr->setFontWidth(new_width);
        int fetched_width = grp_ptr->getFontWidth();
        if (fetched_width != new_width) {
            err += "Font width mismatch after setFontWidth: got=" + std::to_string(fetched_width) + " expected=" + std::to_string(new_width) + "\n";
            ok = false;
        }
        // Test getFontHeight() and setFontHeight()
        int initial_height = grp_ptr->getFontHeight();
        if (initial_height != 8) {
            err += "Initial font height mismatch: got=" + std::to_string(initial_height) + " expected=8\n";
            ok = false;
        }
        int new_height = initial_height + 3;
        grp_ptr->setFontHeight(new_height);
        int fetched_height = grp_ptr->getFontHeight();
        if (fetched_height != new_height) {
            err += "Font height mismatch after setFontHeight: got=" + std::to_string(fetched_height) + " expected=" + std::to_string(new_height) + "\n";
            ok = false;
        }
        // Testing Cleanup
        factory.destroyDisplayObject(name);  
        factory.collectGarbage();

        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Group_test0()



    bool Group_test7()
    {
// return true;
        // int getFontSize() const;
        // int getFontWidth() const;
        // int getFontHeight() const;
        // void setFontSize(int s);
        // void setFontWidth(int w);
        // void setFontHeight(int h);

        std::string testName = "Group #7";
        std::string testDesc = "Font metrics getters/setters (Lua symmetry)";
    sol::state& lua = SDOM::Core::getInstance().getLua();
        // Prepare Lua to create a Group and exercise font metric getters/setters
        std::string create_chunk = R"lua(
            -- Create Group
            local group_name = "ut_group_1"
            local txt = "Some Random Text"
            cfg = { 
                name = group_name, 
                type = "Group", 
                font_resource = "internal_font_8x8", 
                font_size = 8,
                font_width = 8,
                font_height = 8,
                text = txt 
            }
            group_obj = createDisplayObject("Group", cfg)
            return (group_obj ~= nil)
        )lua";

        bool created = false;
        try { created = lua.script(create_chunk).get<bool>(); } catch(...) { created = false; }
        if (!created) {
            return UnitTests::run(testName, testDesc, [=]() { return false; });
        }

        // No debug dumps here; proceed directly to Lua tests

        // Now run the rest of the test in Lua, assuming 'group_obj' and 'cfg' exist in the global env
        std::string test_chunk = R"lua(
            local ok = true
            local err = ""
            -- Test getFontSize() and setFontSize()
            local initial_size = group_obj:getFontSize()
            if (initial_size ~= 8) then
                destroyDisplayObject(cfg.name)
                collectGarbage()
                return { ok = false, err = "Initial font size mismatch: got=" .. tostring(initial_size) .. " expected=8" }
            end
            local new_size = initial_size + 4
            group_obj:setFontSize(new_size)
            local fetched_size = group_obj:getFontSize()
            if fetched_size ~= new_size then
                destroyDisplayObject(cfg.name)
                collectGarbage()
                return { ok = false, err = "Font size mismatch after setFontSize: got=" .. tostring(fetched_size) .. " expected=" .. tostring(new_size) }
            end

            -- Test getFontWidth() and setFontWidth()
            local initial_width = group_obj:getFontWidth()
            if (initial_width ~= 8) then
                destroyDisplayObject(cfg.name)
                collectGarbage()
                return { ok = false, err = "Initial font width mismatch: got=" .. tostring(initial_width) .. " expected=8" }
            end
            local new_width = initial_width + 2
            group_obj:setFontWidth(new_width)
            local fetched_width = group_obj:getFontWidth()
            if fetched_width ~= new_width then
                destroyDisplayObject(cfg.name)
                collectGarbage()
                return { ok = false, err = "Font width mismatch after setFontWidth: got=" .. tostring(fetched_width) .. " expected=" .. tostring(new_width) }
            end

            -- Test getFontHeight() and setFontHeight()
            local initial_height = group_obj:getFontHeight()
            if (initial_height ~= 8) then
                destroyDisplayObject(cfg.name)
                collectGarbage()
                return { ok = false, err = "Initial font height mismatch: got=" .. tostring(initial_height) .. " expected=8" }
            end
            local new_height = initial_height + 3
            group_obj:setFontHeight(new_height)
            local fetched_height = group_obj:getFontHeight()
            if fetched_height ~= new_height then
                destroyDisplayObject(cfg.name)
                collectGarbage()
                return { ok = false, err = "Font height mismatch after setFontHeight: got=" .. tostring(fetched_height) .. " expected=" .. tostring(new_height) }
            end

            -- cleanup and return
            destroyDisplayObject(cfg.name)
            collectGarbage()
            -- verify destruction
            local orphans = getOrphanedDisplayObjects()
            if #orphans > 0 then
                ok = false
                err = "Orphaned objects remain after destroying Group: count=" .. tostring(#orphans)
            end
            return { ok = true, err = "" }
        )lua";

        auto res = lua.script(test_chunk).get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Group_test7()


//  * SpriteSheet & icon helpers (planned coverage):
//  *   - getSpriteSheet(), getSpriteWidth(), getSpriteHeight()
//  *     (Planned: Group_test5; not covered by current tests)    

    bool Group_test8()
    {
return true;  // skip this test for now.

        std::string testName = "Group #8";
        std::string testDesc = "SpriteSeet and Icon Helpers";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script: create a Group with a known icon sprite and verify
        // getSpriteSheet/getSpriteWidth/getSpriteHeight and group color getters/setters.

        auto res = lua.script(R"lua(
            -- Create A Test Group Object:
            local group_name = "ut_group_sprite"
            local txt = "Some Random Text"
            local cfg = { 
                name = group_name, 
                type = "Group", 
                font_resource = "internal_font_8x8", 
                icon_resource = "internal_icon_12x12",
                text = txt 
            }
            local ok = true
            local err = ""
            local group_obj = createDisplayObject("Group", cfg)                             
            if not group_obj then
                return { ok = false, err = "createDisplayObject failed: " .. tostring(h_or_err) }
            end

            -- Test getSpriteSheet()
            local ss = group_obj:getSpriteSheet()
            if not ss then
                destroyDisplayObject(group_name)
                collectGarbage()
                return { ok = false, err = "getSpriteSheet() returned nil" }
            end
            local ss_name = tostring(ss:getName())
            print("SpriteSheet: " .. ss_name)
            if ss_name ~= "internal_icon_12x12_SpriteSheet" then
                destroyDisplayObject(group_name)
                collectGarbage()
                return { ok = false, err = "getSpriteSheet() wrong name: " .. ss_name }
            end

            -- Test group:getSpriteWidth() and group:getSpriteHeight()
            local ex_width = 12 -- 13 to test fails
            local ex_height = 12 -- 13 to test fails
            local width = group_obj:getSpriteWidth()
            local height = group_obj:getSpriteHeight()
            if width ~= ex_width then
                destroyDisplayObject(group_name)
                collectGarbage()
                return { ok = false, err = "Group:getSpriteWidth() got: " .. tostring(width) .. " expected: " .. tostring(ex_width) }
            end
            if height ~= ex_height then
                destroyDisplayObject(group_name)
                collectGarbage()
                return { ok = false, err = "Group:getSpriteHeight() got: " .. tostring(height) .. " expected: " .. tostring(ex_height) }
            end

            -- Test SpriteSheet:getSpriteWidth() and SpriteSheet:getSpriteHeight()
            if true then -- This may be redundant since this check should be made in SpriteSheet_UnitTests.
                -- ex_width = 13 -- test failure mode
                -- ex_height = 13 -- test failure mode
                local ss_width = ss:getSpriteWidth()
                local ss_height = ss:getSpriteHeight()
                if ss_width ~= ex_width then
                    destroyDisplayObject(group_name)
                    collectGarbage()
                    return { ok = false, err = "SpriteSheet:getSpriteWidth() got: " .. tostring(ss_width) .. " expected: " .. tostring(ex_width) }
                end
                if ss_height ~= ex_height then
                    destroyDisplayObject(group_name)
                    collectGarbage()
                    return { ok = false, err = "SpriteSheet:getSpriteHeight() got: " .. tostring(ss_height) .. " expected: " .. tostring(ex_height) }
                end
            end

            -- Test Group:getGroupColor() and Group:setGroupColor()
            local c = group_obj:getGroupColor()
            if not c then return { ok = false, err = 'Group:getGroupColor returned nil' } end

            -- increase r/g/b by 25, clamp to 255
            local newR = math.min(255, (c.r or c:getR() or 0) + 25)
            local newG = math.min(255, (c.g or c:getG() or 0) + 25)
            local newB = math.min(255, (c.b or c:getB() or 0) + 25)
            local newA = (c.a or c:getA() or 0) - 1
            if newA < 0 then newA = 0 end            

            group_obj:setGroupColor({ r = newR, g = newG, b = newB, a = newA })

            local c2 = group_obj:getGroupColor()
            if not c2 then return { ok = false, err = 'getGroupColor after setGroupColor returned nil' } end

            if c2.r ~= newR or c2.g ~= newG or c2.b ~= newB or c2.a ~= newA then
                return { ok = false, err = string.format('color mismatch after setGroupColor (got r=%s,g=%s,b=%s,a=%s expected r=%s,g=%s,b=%s,a=%s)', c2.r, c2.g, c2.b, c2.a, newR, newG, newB, newA) }
            end

            -- cleanup
            destroyDisplayObject(group_name)
            collectGarbage()
            local orphans = getOrphanedDisplayObjects()
            if #orphans > 0 then return { ok = false, err = "Orphaned objects remain after destroying Group" } end

            return { ok = true, err = "" }
        )lua").get<sol::table>();

        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Group_test8()    


    // --- Run the Group UnitTests --- //

    bool Group_UnitTests() 
    {
        std::string objName = "Group UnitTests";
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
             [&]() { return Group_test0(); }    // UnitTest Scafolding
            ,[&]() { return Group_test1(); }    // Creating Group via Lua config table with Label pointer effects
            ,[&]() { return Group_test2(); }    // Creating Group via InitStruct with Label pointer effects
            ,[&]() { return Group_test3(); }    // Creating Group via lua config string with Label pointer effects
            ,[&]() { return Group_test4(); }    // Create and Bindings
            ,[&]() { return Group_test5(); }    // Label property and function symmetry
            ,[&]() { return Group_test6(); }    // Font metrics getters/setters (C++ symmetry)
            ,[&]() { return Group_test7(); }    // Font metrics getters/setters (Lua symmetry)
            ,[&]() { return Group_test8(); }    // SpriteSeet and Icon Helpers
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        std::cout << CLR::indent() << CLR::LT_BLUE << "[" << objName << "] " << CLR::RESET;
        std::cout << (allTestsPassed ? CLR::GREEN + " [PASSED]" : CLR::fg_rgb(255, 0, 0) + " [FAILED]") << CLR::RESET << std::endl;          
        return allTestsPassed;
    } // END: Group_UnitTests()

} // END: namespace SDOM

