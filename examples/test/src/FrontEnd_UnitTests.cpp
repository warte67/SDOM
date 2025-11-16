// FrontEnd_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>
#include <SDOM/SDOM_Group.hpp>

namespace
{
    void logDisplayTree(const SDOM::DisplayHandle& handle, int depth, std::size_t& visited)
    {
        const std::string indent(depth * 2, ' ');
        if (!handle.isValid())
        {
            INFO(indent << "- <invalid handle>");
            return;
        }

        SDOM::IDisplayObject* obj = handle.get();
        ++visited;

        INFO(indent << "- " << handle.getName() << " [" << handle.getType() << "] ptr=" << obj
                    << " children=" << obj->countChildren());

        INFO(indent << "  pos=(" << obj->getX() << ", " << obj->getY()
                    << ") size=(" << obj->getWidth() << " x " << obj->getHeight() << ")"
                    << " enabled=" << (obj->isEnabled() ? "true" : "false")
                    << " hidden=" << (obj->isHidden() ? "true" : "false")
                    << " clickable=" << (obj->isClickable() ? "true" : "false"));

        const SDL_Color color = obj->getColor();
        INFO(indent << "  color RGBA(" << static_cast<int>(color.r) << ", "
                    << static_cast<int>(color.g) << ", "
                    << static_cast<int>(color.b) << ", "
                    << static_cast<int>(color.a) << ")");

        for (const SDOM::DisplayHandle& child : obj->getChildren())
            logDisplayTree(child, depth + 1, visited);
    }
}

namespace SDOM
{
    // --- Individual FrontEnd Unit Tests --- //

    // ============================================================================
    //  Test 0: FrontEnd Template
    // ----------------------------------------------------------------------------
    //  This template serves as a reference pattern for writing SDOM unit tests.
    //
    //  Status Legend:
    //   ✅ Test Verified     - Stable, validated, and passing
    //   🔄 In Progress       - Currently being implemented or debugged
    //   ⚠️  Failing          - Currently failing; requires investigation
    //   🚫 Remove            - Deprecated or replaced
    //   ❌ Invalid           - No longer applicable or test case obsolete
    //   ☐ Planned            - Placeholder for future implementation
    //
    //  Usage Notes:
    //   • To signal a test failure, push a descriptive message to `errors`.
    //   • Each test should return `true` once it has finished running.
    //   • Multi-frame tests may return `false` until all assertions pass.
    //   • Keep tests self-contained and deterministic.
    //
    // ============================================================================
    bool FrontEnd_test0([[maybe_unused]] std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        // ok = false;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        // To fetch the current frame:
        // int current_frame = UnitTests::getInstance().get_frame_counter();

        // Use State Machines when re-entrant tests are required to test SDOM main loop

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

    } // END: FrontEnd_test0(std::vector<std::string>& errors)


    // Main Stage
    bool FrontEnd_test1([[maybe_unused]] std::vector<std::string>& errors)
    {
        Core& core = getCore();
        DisplayHandle root = core.getRootNode();

        if (!root.isValid())
        {
            errors.push_back("FrontEnd_test1: Core root node is invalid");
            return true;
        }

        if (false)
        {
            INFO("FrontEnd_test1: Inspecting main stage '" << root.getName()
                << "' [" << root.getType() << "] via MAIN_VARIANT 2 setup");
            std::size_t visited = 0;
            logDisplayTree(root, 0, visited);
            INFO("FrontEnd_test1: enumerated " << visited << " display object(s) under main stage.");
        }

        // Check for size and color
        if (root->getWidth() != 600 || root->getHeight() != 400)
        {
            errors.push_back("FrontEnd_test1: Main stage size is invalid");
        }

        if (root->getColor().r != 16 || 
            root->getColor().g != 32|| 
            root->getColor().b != 8 || 
            root->getColor().a != 255)
        {
            errors.push_back("FrontEnd_test1: Main stage color is invalid");
        }

        return true; // ✅ finished this frame
    } // END: FrontEnd_test1(std::vector<std::string>& errors)


    // Right Main Frame
    bool FrontEnd_test2([[maybe_unused]] std::vector<std::string>& errors)
    {
        Core& core = getCore();
        DisplayHandle root = core.getRootNode();
        if (!root.isValid()) 
            { errors.push_back("Core root node is invalid"); return true; }

        DisplayHandle rightMainFrame = root->getChild("rightMainFrame");
        if (!rightMainFrame.isValid()) 
            { errors.push_back("FrightMainFrame child not found under root stage"); return true; }

        IDisplayObject* frameObject = rightMainFrame.get();
        if (!frameObject) 
            { errors.push_back("rightMainFrame DisplayHandle resolved to nullptr"); return true; }

        const std::string actualType = frameObject->getType();
        if (actualType != "Frame") 
            { errors.push_back("rightMainFrame type mismatch (expected Frame, got '" + actualType + "')"); }

        if (frameObject->getX() != 300)      { errors.push_back("rightMainFrame x mismatch (expected 300, got " + std::to_string(frameObject->getX()) + ")"); }
        if (frameObject->getY() != 5)        { errors.push_back("rightMainFrame y mismatch (expected 5, got " + std::to_string(frameObject->getY()) + ")"); }
        if (frameObject->getWidth() != 295)  { errors.push_back("rightMainFrame width mismatch (expected 295, got " + std::to_string (frameObject->getWidth()) + ")"); }
        if (frameObject->getHeight() != 390) { errors.push_back("rightMainFrame height mismatch (expected 390, got " + std::to_string(frameObject->getHeight()) + ")"); }

        const SDL_Color frameColor = frameObject->getColor();
        if (frameColor.r != 32 || frameColor.g != 64 || frameColor.b != 16 || frameColor.a != 255)
            { errors.push_back("FrontEnd_test2: rightMainFrame color mismatch (expected RGBA(32,64,16,255))"); }

        SDOM::IPanelObject* panel = dynamic_cast<SDOM::IPanelObject*>(frameObject);
        if (!panel) 
            { errors.push_back("FrontEnd_test2: rightMainFrame is not an IPanelObject instance"); return true; }

        const std::string iconResource = panel->getIconResourceName();
        if (iconResource.find("internal_icon_16x16") == std::string::npos) 
            { errors.push_back("FrontEnd_test2: icon_resource mismatch (expected internal_icon_16x16*, got '" + iconResource + "')"); }
        AssetHandle iconAsset = panel->getSpriteSheet();
        if (!iconAsset.isValid())   
            { errors.push_back("FrontEnd_test2: icon_resource handle is invalid"); }
        else if (iconAsset->getType() != SpriteSheet::TypeName) 
            { errors.push_back("FrontEnd_test2: icon_resource asset is not a SpriteSheet (type='" + iconAsset->getType() + "')"); }

        if (panel->getIconWidth() != 16 || panel->getIconHeight() != 16) { 
            errors.push_back("FrontEnd_test2: icon dimensions mismatch (expected 16x16, got "
                             + std::to_string(panel->getIconWidth()) + "x" + std::to_string(panel->getIconHeight()) + ")"); }

        const std::string fontResource = panel->getFontResourceName();
        if (fontResource != "internal_font_8x8")
            { errors.push_back("FrontEnd_test2: font_resource mismatch (expected internal_font_8x8, got '" + fontResource + "')"); }
        AssetHandle fontAsset = panel->getFontAsset();
        if (!fontAsset.isValid()) { errors.push_back("FrontEnd_test2: font_resource handle is invalid"); }
        if (panel->getFontWidth() != 8 || panel->getFontHeight() != 8) { 
            errors.push_back("FrontEnd_test2: font dimensions mismatch (expected 8x8, got "
                             + std::to_string(panel->getFontWidth()) + "x" + std::to_string(panel->getFontHeight()) + ")"); }

        return true; // ✅ finished this frame

    } // END: FrontEnd_test2(std::vector<std::string>& errors)


    // Main Frame Group
    bool FrontEnd_test3([[maybe_unused]] std::vector<std::string>& errors)
    {
        Core& core = getCore();
        DisplayHandle root = core.getRootNode();
        if (!root.isValid())
            { errors.push_back("Core root node is invalid"); return true; }

        DisplayHandle rightMainFrame = root->getChild("rightMainFrame");
        if (!rightMainFrame.isValid())
            { errors.push_back("FrontEnd_test3: rightMainFrame not found under root stage"); return true; }

        DisplayHandle mainGroup = rightMainFrame->getChild("mainFrameGroup");
        if (!mainGroup.isValid())
            { errors.push_back("FrontEnd_test3: mainFrameGroup child not found under rightMainFrame"); return true; }

        IDisplayObject* groupObject = mainGroup.get();
        if (!groupObject)
            { errors.push_back("FrontEnd_test3: mainFrameGroup DisplayHandle resolved to nullptr"); return true; }

        const std::string groupType = groupObject->getType();
        if (groupType != "Group")
            { errors.push_back("FrontEnd_test3: mainFrameGroup type mismatch (expected Group, got '" + groupType + "')"); }

        if (groupObject->getX() != 450)
            { errors.push_back("FrontEnd_test3: mainFrameGroup x mismatch (expected 450, got " + std::to_string(groupObject->getX()) + ")"); }
        if (groupObject->getY() != 10)
            { errors.push_back("FrontEnd_test3: mainFrameGroup y mismatch (expected 10, got " + std::to_string(groupObject->getY()) + ")"); }
        if (groupObject->getWidth() != 140)
            { errors.push_back("FrontEnd_test3: mainFrameGroup width mismatch (expected 140, got " + std::to_string(groupObject->getWidth()) + ")"); }
        if (groupObject->getHeight() != 70)
            { errors.push_back("FrontEnd_test3: mainFrameGroup height mismatch (expected 70, got " + std::to_string(groupObject->getHeight()) + ")"); }

        // const SDL_Color groupColor = groupObject->getColor();
        // if (groupColor.r != 255 || groupColor.g != 255 || groupColor.b != 255 || groupColor.a != 96)
        //     { errors.push_back("FrontEnd_test3: mainFrameGroup color mismatch (expected RGBA(255,255,255,96))"); }

        IPanelObject* panel = dynamic_cast<IPanelObject*>(groupObject);
        if (!panel)
            { errors.push_back("FrontEnd_test3: mainFrameGroup is not an IPanelObject instance"); return true; }

        const std::string iconResource = panel->getIconResourceName();
        if (iconResource.find("internal_icon_16x16") == std::string::npos)
            { errors.push_back("FrontEnd_test3: icon_resource mismatch (expected internal_icon_16x16*, got '" + iconResource + "')"); }

        AssetHandle iconAsset = panel->getSpriteSheet();
        if (!iconAsset.isValid())
            { errors.push_back("FrontEnd_test3: icon_resource handle is invalid"); }
        else if (iconAsset->getType() != SpriteSheet::TypeName)
            { errors.push_back("FrontEnd_test3: icon_resource asset is not a SpriteSheet (type='" + iconAsset->getType() + "')"); }

        if (panel->getIconWidth() != 16 || panel->getIconHeight() != 16)
            { errors.push_back("FrontEnd_test3: icon dimensions mismatch (expected 16x16, got "
                               + std::to_string(panel->getIconWidth()) + "x" + std::to_string(panel->getIconHeight()) + ")"); }

        const std::string fontResource = panel->getFontResourceName();
        if (fontResource != "internal_ttf")
            { errors.push_back("FrontEnd_test3: font_resource mismatch (expected internal_ttf, got '" + fontResource + "')"); }

        AssetHandle fontAsset = panel->getFontAsset();
        if (!fontAsset.isValid())
            { errors.push_back("FrontEnd_test3: font_resource handle is invalid"); }
        else if (fontAsset->getType() != "TruetypeFont")
            { errors.push_back("FrontEnd_test3: font_resource asset is not a TruetypeFont (type='" + fontAsset->getType() + "')"); }

        Group* group = dynamic_cast<Group*>(groupObject);
        if (!group)
            { errors.push_back("FrontEnd_test3: mainFrameGroup did not resolve to Group pointer"); return true; }

        DisplayHandle label = group->getLabel();
        if (!label.isValid())
            { errors.push_back("FrontEnd_test3: auto-generated label handle is invalid"); return true; }

        IDisplayObject* labelObject = label.get();
        if (!labelObject)
            { errors.push_back("FrontEnd_test3: auto-generated label resolved to nullptr"); return true; }

        if (label.getName() != "mainFrameGroup_label")
            { errors.push_back("FrontEnd_test3: label name mismatch (expected mainFrameGroup_label, got '" + label.getName() + "')"); }

        const std::string labelType = labelObject->getType();
        if (labelType != "Label")
            { errors.push_back("FrontEnd_test3: label type mismatch (expected Label, got '" + labelType + "')"); }

        const std::string labelText = group->getLabelText();
        if (labelText != "Main Group")
            { errors.push_back("FrontEnd_test3: label text mismatch (expected 'Main Group', got '" + labelText + "')"); }

        // const SDL_Color labelColor = group->getLabelColor();
        // if (labelColor.r != 224 || labelColor.g != 192 || labelColor.b != 192 || labelColor.a != 255)
        //     { errors.push_back("FrontEnd_test3: label color mismatch (expected RGBA(224,192,192,255))"); }

        return true; // ✅ finished this frame

    } // END: FrontEnd_test3(std::vector<std::string>& errors)



    // --- Lua Integration Tests --- //

    bool FrontEnd_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/FrontEnd_UnitTests.lua");
    } // END: FrontEnd_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool FrontEnd_UnitTests()
    {
        const std::string objName = "FrontEnd";
        UnitTests& ut = UnitTests::getInstance();
        // ut.clear_tests();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test FrontEnd", FrontEnd_test0);
            ut.add_test(objName, "Main Stage", FrontEnd_test1);
            ut.add_test(objName, "Right Main Frame", FrontEnd_test2);
            ut.add_test(objName, "Main Frame Group", FrontEnd_test3);

            // ut.add_test(objName, "Lua: src/FrontEnd_UnitTests.lua", FrontEnd_LUA_Tests, false);

            registered = true;
        }

        // return ut.run_all(objName, "FrontEnd");
        return true;
    } // END: FrontEnd_UnitTests()



} // END: namespace SDOM
