// FrontEnd_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>
#include <SDOM/SDOM_IRangeControl.hpp>
#include <SDOM/SDOM_Group.hpp>
#include <SDOM/SDOM_CheckButton.hpp>
#include <SDOM/SDOM_RadioButton.hpp>
#include <SDOM/SDOM_TristateButton.hpp>
#include <SDOM/SDOM_Button.hpp>
#include <SDOM/SDOM_ArrowButton.hpp>
#include <SDOM/SDOM_Slider.hpp>
#include <SDOM/SDOM_ProgressBar.hpp>
#include <SDOM/SDOM_ScrollBar.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <array>
#include <sstream>
#include <cmath>
#include <vector>

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

    SDOM::DisplayHandle getChildOrReport(const SDOM::DisplayHandle& parent,
                                         const std::string& childName,
                                         std::vector<std::string>& errors,
                                         const std::string& context)
    {
        if (!parent.isValid())
        {
            errors.push_back(context + ": parent handle is invalid (needed child '" + childName + "')");
            return {};
        }

        SDOM::DisplayHandle child = parent->getChild(childName);
        if (!child.isValid())
        {
            errors.push_back(context + ": missing child '" + childName + "'");
        }
        return child;
    }

    void expectColorEq(const SDL_Color& actual,
                       const SDL_Color& expected,
                       const std::string& context,
                       std::vector<std::string>& errors)
    {
        if (actual.r == expected.r && actual.g == expected.g &&
            actual.b == expected.b && actual.a == expected.a)
        {
            return;
        }

        std::ostringstream oss;
        oss << context << ": color mismatch (expected RGBA("
            << static_cast<int>(expected.r) << ','
            << static_cast<int>(expected.g) << ','
            << static_cast<int>(expected.b) << ','
            << static_cast<int>(expected.a) << ") got RGBA("
            << static_cast<int>(actual.r) << ','
            << static_cast<int>(actual.g) << ','
            << static_cast<int>(actual.b) << ','
            << static_cast<int>(actual.a) << ")";
        errors.push_back(oss.str());
    }

    template <typename T>
    T* castOrReport(const SDOM::DisplayHandle& handle,
                    const std::string& expectedType,
                    const std::string& context,
                    std::vector<std::string>& errors)
    {
        if (!handle.isValid())
            return nullptr;

        T* ptr = dynamic_cast<T*>(handle.get());
        if (!ptr)
        {
            errors.push_back(context + ": '" + handle.getName() + "' is not a " + expectedType);
        }
        return ptr;
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
            errors.push_back("Core root node is invalid");
            return true;
        }

        if (false)
        {
            INFO("Inspecting main stage '" << root.getName()
                << "' [" << root.getType() << "] via MAIN_VARIANT 2 setup");
            std::size_t visited = 0;
            logDisplayTree(root, 0, visited);
            INFO("enumerated " << visited << " display object(s) under main stage.");
        }

        // Check for size and color
        if (root->getWidth() != 600 || root->getHeight() != 400)
        {
            errors.push_back("Main stage size is invalid");
        }

        if (root->getColor().r != 32 || 
            root->getColor().g != 8|| 
            root->getColor().b != 4 || 
            root->getColor().a != 255)
        {
            errors.push_back("Main stage color is invalid");
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
        if (frameColor.r != 64 || frameColor.g != 32 || frameColor.b != 32 || frameColor.a != 255)
            { errors.push_back("rightMainFrame color mismatch (expected RGBA(64,32,32,255))"); }

        SDOM::IPanelObject* panel = dynamic_cast<SDOM::IPanelObject*>(frameObject);
        if (!panel) 
            { errors.push_back("rightMainFrame is not an IPanelObject instance"); return true; }

        const std::string iconResource = panel->getIconResourceName();
        if (iconResource.find("internal_icon_16x16") == std::string::npos) 
            { errors.push_back("icon_resource mismatch (expected internal_icon_16x16*, got '" + iconResource + "')"); }
        AssetHandle iconAsset = panel->getSpriteSheet();
        if (!iconAsset.isValid())   
            { errors.push_back("icon_resource handle is invalid"); }
        else if (iconAsset->getType() != SpriteSheet::TypeName) 
            { errors.push_back("icon_resource asset is not a SpriteSheet (type='" + iconAsset->getType() + "')"); }

        if (panel->getIconWidth() != 16 || panel->getIconHeight() != 16) { 
            errors.push_back("icon dimensions mismatch (expected 16x16, got "
                             + std::to_string(panel->getIconWidth()) + "x" + std::to_string(panel->getIconHeight()) + ")"); }

        const std::string fontResource = panel->getFontResourceName();
        if (fontResource != "internal_font_8x8")
            { errors.push_back("font_resource mismatch (expected internal_font_8x8, got '" + fontResource + "')"); }
        AssetHandle fontAsset = panel->getFontAsset();
        if (!fontAsset.isValid()) { errors.push_back("font_resource handle is invalid"); }
        if (panel->getFontWidth() != 8 || panel->getFontHeight() != 8) { 
            errors.push_back("font dimensions mismatch (expected 8x8, got "
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


    // CheckButtons on rightMainFrame
    bool FrontEnd_test4([[maybe_unused]] std::vector<std::string>& errors)
    {
        Core& core = getCore();
        DisplayHandle root = core.getRootNode();
        if (!root.isValid())
        {
            errors.push_back("FrontEnd_test4: Core root node is invalid");
            return true;
        }

        DisplayHandle rightMainFrame = getChildOrReport(root, "rightMainFrame", errors, "FrontEnd_test4");
        if (!rightMainFrame.isValid())
            return true;

        const SDL_Color expectedLabelColor{255, 255, 255, 255};
        struct CheckExpectation
        {
            const char* name;
            const char* fontResource;
        };

        const std::array<CheckExpectation, 3> expectations{ {
            {"mainFrame_CheckButton_1", "internal_ttf"},
            {"mainFrame_CheckButton_2", "VarelaRound16"},
            {"mainFrame_CheckButton_3", "internal_font_8x8"}
        } };

        for (const CheckExpectation& exp : expectations)
        {
            DisplayHandle child = getChildOrReport(rightMainFrame, exp.name, errors, "FrontEnd_test4");
            if (!child.isValid())
                continue;

            CheckButton* cb = castOrReport<CheckButton>(child, "CheckButton", "FrontEnd_test4", errors);
            if (!cb)
                continue;

            if (cb->getFontResource() != exp.fontResource)
            {
                errors.push_back(std::string("FrontEnd_test4: ") + exp.name + " font mismatch (expected " + exp.fontResource + ", got '" + cb->getFontResource() + "')");
            }

            if (cb->getIconResource() != "internal_icon_12x12")
            {
                errors.push_back(std::string("FrontEnd_test4: ") + exp.name + " icon mismatch (expected internal_icon_12x12, got '" + cb->getIconResource() + "')");
            }

            expectColorEq(cb->getLabelColor(), expectedLabelColor,
                          std::string("FrontEnd_test4: ") + exp.name + " label color", errors);
        }

        return true;
    } // END: FrontEnd_test4(std::vector<std::string>& errors)


    // RadioButtons + Tristate coverage
    bool FrontEnd_test5([[maybe_unused]] std::vector<std::string>& errors)
    {
        Core& core = getCore();
        DisplayHandle root = core.getRootNode();
        if (!root.isValid())
        {
            errors.push_back("FrontEnd_test5: Core root node is invalid");
            return true;
        }

        DisplayHandle rightMainFrame = getChildOrReport(root, "rightMainFrame", errors, "FrontEnd_test5");
        if (!rightMainFrame.isValid())
            return true;

        DisplayHandle mainGroup = getChildOrReport(rightMainFrame, "mainFrameGroup", errors, "FrontEnd_test5");
        if (!mainGroup.isValid())
            return true;

        struct RadioExpectation
        {
            const char* name;
            const char* fontResource;
            const char* iconResource;
            SDL_Color labelColor;
        };

        auto validateRadios = [&](const DisplayHandle& parent,
                                  const std::string& context,
                                  const std::vector<RadioExpectation>& list)
        {
            for (const RadioExpectation& exp : list)
            {
                DisplayHandle child = getChildOrReport(parent, exp.name, errors, context);
                if (!child.isValid())
                    continue;

                RadioButton* rb = castOrReport<RadioButton>(child, "RadioButton", context, errors);
                if (!rb)
                    continue;

                if (rb->getFontResource() != exp.fontResource)
                {
                    errors.push_back(context + ": " + exp.name + " font mismatch (expected " + exp.fontResource + ", got '" + rb->getFontResource() + "')");
                }

                if (rb->getIconResource() != exp.iconResource)
                {
                    errors.push_back(context + ": " + exp.name + " icon mismatch (expected " + exp.iconResource + ", got '" + rb->getIconResource() + "')");
                }

                expectColorEq(rb->getLabelColor(), exp.labelColor,
                              context + ": " + exp.name + " label color", errors);
            }
        };

        validateRadios(mainGroup, "FrontEnd_test5 (mainFrameGroup)",
            {
                {"mainFrameGroup_radioButton_1", "internal_ttf",   "internal_icon_16x16", SDL_Color{255, 255, 255, 255}},
                {"mainFrameGroup_radioButton_2", "VarelaRound16", "internal_icon_12x12", SDL_Color{255, 255, 255, 255}},
                {"mainFrameGroup_radioButton_3", "internal_font_8x8", "internal_icon_8x8", SDL_Color{255, 255, 255, 255}}
            });

        validateRadios(rightMainFrame, "FrontEnd_test5 (rightMainFrame)",
            {
                {"mainFrame_RadioButton_1", "internal_ttf",   "internal_icon_12x12", SDL_Color{255, 255, 255, 255}},
                {"mainFrame_RadioButton_2", "VarelaRound16", "internal_icon_12x12", SDL_Color{255, 255, 255, 255}},
                {"mainFrame_RadioButton_3", "internal_font_8x8", "internal_icon_12x12", SDL_Color{255, 255, 255, 255}}
            });

        DisplayHandle tristateHandle = getChildOrReport(rightMainFrame, "mainFrame_tristate_1", errors, "FrontEnd_test5");
        if (tristateHandle.isValid())
        {
            TristateButton* tristate = castOrReport<TristateButton>(tristateHandle, "TristateButton", "FrontEnd_test5", errors);
            if (tristate)
            {
                if (tristate->getFontResource() != "VarelaRound16")
                {
                    errors.push_back("FrontEnd_test5: Tristate button font mismatch (expected VarelaRound16, got '" + tristate->getFontResource() + "')");
                }

                if (tristate->getIconResource() != "internal_icon_12x12")
                {
                    errors.push_back("FrontEnd_test5: Tristate button icon mismatch (expected internal_icon_12x12, got '" + tristate->getIconResource() + "')");
                }

                if (tristate->getState() != ButtonState::Inactive)
                {
                    errors.push_back("FrontEnd_test5: Tristate button should start inactive");
                }
            }
        }

        return true;
    } // END: FrontEnd_test5(std::vector<std::string>& errors)


    // Range controls (sliders/progress/scrollbars)
    bool FrontEnd_test6([[maybe_unused]] std::vector<std::string>& errors)
    {
        Core& core = getCore();
        DisplayHandle root = core.getRootNode();
        if (!root.isValid())
        {
            errors.push_back("FrontEnd_test6: Core root node is invalid");
            return true;
        }

        DisplayHandle rightMainFrame = getChildOrReport(root, "rightMainFrame", errors, "FrontEnd_test6");
        if (!rightMainFrame.isValid())
            return true;

        auto expectOrientation = [&](const std::string& ctx, IRangeControl* control, Orientation expected)
        {
            if (!control)
                return;
            if (control->getOrientation() != expected)
            {
                const std::string got = orientation_to_string.at(control->getOrientation());
                const std::string want = orientation_to_string.at(expected);
                errors.push_back(ctx + ": orientation mismatch (expected " + want + ", got " + got + ")");
            }
        };

        auto expectFloat = [&](const std::string& ctx, float actual, float expected)
        {
            if (std::fabs(actual - expected) > 0.01f)
            {
                errors.push_back(ctx + ": value mismatch (expected " + std::to_string(expected) + ", got " + std::to_string(actual) + ")");
            }
        };

        const std::string ctx = "FrontEnd_test6";

        // Horizontal slider
        {
            DisplayHandle handle = getChildOrReport(rightMainFrame, "mainFrame_hslider_1", errors, ctx);
            Slider* slider = castOrReport<Slider>(handle, "Slider", ctx, errors);
            if (slider)
            {
                expectOrientation(ctx + " mainFrame_hslider_1", slider, Orientation::Horizontal);
                expectFloat(ctx + " mainFrame_hslider_1 step", slider->getStep(), 2.5f);
                if (slider->getIconResource() != "internal_icon_16x16")
                    errors.push_back(ctx + ": mainFrame_hslider_1 icon mismatch (expected internal_icon_16x16)");
            }
        }

        // Horizontal progress bar
        {
            DisplayHandle handle = getChildOrReport(rightMainFrame, "mainFrame_hprogress_1", errors, ctx);
            ProgressBar* progress = castOrReport<ProgressBar>(handle, "ProgressBar", ctx, errors);
            if (progress)
            {
                expectOrientation(ctx + " mainFrame_hprogress_1", progress, Orientation::Horizontal);
                expectFloat(ctx + " mainFrame_hprogress_1 value", progress->getValue(), 50.0f);
            }
        }

        // Horizontal scrollbar
        {
            DisplayHandle handle = getChildOrReport(rightMainFrame, "mainFrame_hscrollbar_1", errors, ctx);
            ScrollBar* scroll = castOrReport<ScrollBar>(handle, "ScrollBar", ctx, errors);
            if (scroll)
            {
                expectOrientation(ctx + " mainFrame_hscrollbar_1", scroll, Orientation::Horizontal);
                expectFloat(ctx + " mainFrame_hscrollbar_1 step", scroll->getStep(), 5.0f);
            }
        }

        // Vertical slider
        {
            DisplayHandle handle = getChildOrReport(rightMainFrame, "mainFrame_vslider_1", errors, ctx);
            Slider* slider = castOrReport<Slider>(handle, "Slider", ctx, errors);
            if (slider)
            {
                expectOrientation(ctx + " mainFrame_vslider_1", slider, Orientation::Vertical);
                expectFloat(ctx + " mainFrame_vslider_1 step", slider->getStep(), 2.5f);
            }
        }

        // Vertical progress bar
        {
            DisplayHandle handle = getChildOrReport(rightMainFrame, "mainFrame_vprogress_1", errors, ctx);
            ProgressBar* progress = castOrReport<ProgressBar>(handle, "ProgressBar", ctx, errors);
            if (progress)
            {
                expectOrientation(ctx + " mainFrame_vprogress_1", progress, Orientation::Vertical);
                expectFloat(ctx + " mainFrame_vprogress_1 value", progress->getValue(), 25.0f);
            }
        }

        // Vertical scrollbar
        {
            DisplayHandle handle = getChildOrReport(rightMainFrame, "mainFrame_vscrollbar_1", errors, ctx);
            ScrollBar* scroll = castOrReport<ScrollBar>(handle, "ScrollBar", ctx, errors);
            if (scroll)
            {
                expectOrientation(ctx + " mainFrame_vscrollbar_1", scroll, Orientation::Vertical);
                expectFloat(ctx + " mainFrame_vscrollbar_1 step", scroll->getStep(), 5.0f);
                expectFloat(ctx + " mainFrame_vscrollbar_1 value", scroll->getValue(), 50.0f);
            }
        }

        return true;
    } // END: FrontEnd_test6(std::vector<std::string>& errors)


    // Stage-level buttons & icons
    bool FrontEnd_test7([[maybe_unused]] std::vector<std::string>& errors)
    {
        Core& core = getCore();
        DisplayHandle root = core.getRootNode();
        if (!root.isValid())
        {
            errors.push_back("FrontEnd_test7: Core root node is invalid");
            return true;
        }

        // Main stage button
        DisplayHandle stageButtonHandle = getChildOrReport(root, "main_stage_button", errors, "FrontEnd_test7");
        if (stageButtonHandle.isValid())
        {
            Button* button = castOrReport<Button>(stageButtonHandle, "Button", "FrontEnd_test7", errors);
            if (button)
            {
                if (IPanelObject* panel = dynamic_cast<IPanelObject*>(button))
                {
                    if (panel->getFontResourceName() != "VarelaRound16")
                    {
                        errors.push_back("FrontEnd_test7: main_stage_button font mismatch (expected VarelaRound16, got '" + panel->getFontResourceName() + "')");
                    }
                }

                expectColorEq(button->getLabelColor(), SDL_Color{255, 255, 255, 255},
                              "FrontEnd_test7: main_stage_button label color", errors);

                expectColorEq(button->getColor(), SDL_Color{96, 32, 16, 255},
                              "FrontEnd_test7: main_stage_button color", errors);
            }
        }

        // Hamburger icon button
        DisplayHandle iconHandle = getChildOrReport(root, "main_stage_hamburger_iconbutton", errors, "FrontEnd_test7");
        if (iconHandle.isValid())
        {
            IconButton* iconBtn = castOrReport<IconButton>(iconHandle, "IconButton", "FrontEnd_test7", errors);
            if (iconBtn)
            {
                if (!iconBtn->getSpriteSheetHandle().isValid())
                {
                    errors.push_back("FrontEnd_test7: hamburger icon missing sprite sheet asset");
                }

                if (iconBtn->getIconIndex() != IconIndex::Hamburger)
                {
                    errors.push_back("FrontEnd_test7: hamburger icon index mismatch (expected Hamburger)");
                }
            }
        }

        // Arrow button on stage
        DisplayHandle arrowHandle = getChildOrReport(root, "main_stage_arrowbutton", errors, "FrontEnd_test7");
        if (arrowHandle.isValid())
        {
            ArrowButton* arrow = castOrReport<ArrowButton>(arrowHandle, "ArrowButton", "FrontEnd_test7", errors);
            if (arrow)
            {
                if (arrow->getDirection() != ArrowButton::ArrowDirection::Up)
                {
                    errors.push_back("FrontEnd_test7: arrow button expected Up direction");
                }

                expectColorEq(arrow->getColor(), SDL_Color{255, 255, 0, 255},
                              "FrontEnd_test7: arrow button color", errors);
            }
        }

        return true;
    } // END: FrontEnd_test7(std::vector<std::string>& errors)


    // Decorative labels & fonts
    bool FrontEnd_test8([[maybe_unused]] std::vector<std::string>& errors)
    {
        Core& core = getCore();
        DisplayHandle root = core.getRootNode();
        if (!root.isValid())
        {
            errors.push_back("FrontEnd_test8: Core root node is invalid");
            return true;
        }

        auto validateLabelFont = [&](const DisplayHandle& parent,
                                     const std::string& labelName,
                                     const std::string& ctx,
                                     const std::string& expectedFont)
        {
            DisplayHandle labelHandle = getChildOrReport(parent, labelName, errors, ctx);
            if (!labelHandle.isValid())
                return static_cast<Label*>(nullptr);

            return castOrReport<Label>(labelHandle, "Label", ctx, errors);
        };

        // Redish box label: 32pt VarelaRound
        DisplayHandle redishBox = getChildOrReport(root, "redishBox", errors, "FrontEnd_test8");
        if (redishBox.isValid())
        {
            if (Label* label = validateLabelFont(redishBox, "redishBoxLabel", "FrontEnd_test8 redish", "VarelaRound32"))
            {
                AssetHandle font = label->getFont();
                if (!font.isValid())
                    errors.push_back("FrontEnd_test8: redish label font asset invalid");
                else if (font.getName() != "VarelaRound32")
                    errors.push_back("FrontEnd_test8: redish label font asset mismatch (expected VarelaRound32, got '" + font.getName() + "')");

                if (label->getFontSize() != 32)
                    errors.push_back("FrontEnd_test8: redish label font size mismatch (expected 32, got " + std::to_string(label->getFontSize()) + ")");

                if (!label->getWordwrap())
                    errors.push_back("FrontEnd_test8: redish label should have wordwrap enabled (got false)");
            }
        }

        // Greenish box label: bitmap font with constrained width
        DisplayHandle greenishBox = getChildOrReport(root, "greenishBox", errors, "FrontEnd_test8");
        if (greenishBox.isValid())
        {
            if (Label* label = validateLabelFont(greenishBox, "greenishBoxLabel", "FrontEnd_test8 greenish", "internal_font_8x8"))
            {
                if (!label->getWordwrap())
                    errors.push_back("FrontEnd_test8: greenish label should wrap text (got false)");
                if (label->getAutoResize())
                    errors.push_back("FrontEnd_test8: greenish label auto_resize should be false");
                if (label->getMaxWidth() != 400)
                    errors.push_back("FrontEnd_test8: greenish label max_width mismatch (expected 400, got " + std::to_string(label->getMaxWidth()) + ")");
            }
        }

        // Blueish box label: internal_ttf, capped width 500
        DisplayHandle blueishBox = getChildOrReport(root, "blueishBox", errors, "FrontEnd_test8");
        if (blueishBox.isValid())
        {
            if (Label* label = validateLabelFont(blueishBox, "blueishBoxLabel", "FrontEnd_test8 blueish", "internal_ttf"))
            {
                if (!label->getWordwrap())
                    errors.push_back("FrontEnd_test8: blueish label should wrap text (got false)");
                if (label->getMaxWidth() != 500)
                    errors.push_back("FrontEnd_test8: blueish label max_width mismatch (expected 500)");
            }
        }

        // Orangish box label: bitmap 8x12 font
        DisplayHandle orangishBox = getChildOrReport(root, "orangishBox", errors, "FrontEnd_test8");
        if (orangishBox.isValid())
        {
            if (Label* label = validateLabelFont(orangishBox, "orangeishBoxLabel", "FrontEnd_test8 orangish", "internal_font_8x12"))
            {
                if (!label->getWordwrap())
                    errors.push_back("FrontEnd_test8: orangish label should wrap text (got false)");
            }
        }

        return true;
    } // END: FrontEnd_test8(std::vector<std::string>& errors)



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
            ut.add_test(objName, "Frame CheckButtons", FrontEnd_test4);
            ut.add_test(objName, "Radio + Tristate", FrontEnd_test5);
            ut.add_test(objName, "Range Controls", FrontEnd_test6);
            ut.add_test(objName, "Stage Buttons", FrontEnd_test7);
            ut.add_test(objName, "Decorative Labels", FrontEnd_test8);

            // ut.add_test(objName, "Lua: src/FrontEnd_UnitTests.lua", FrontEnd_LUA_Tests, false);

            registered = true;
        }

        // return ut.run_all(objName, "FrontEnd");
        return true;
    } // END: FrontEnd_UnitTests()



} // END: namespace SDOM
