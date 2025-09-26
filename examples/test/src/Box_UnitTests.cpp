// Box_UnitTestss.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp> 
#include <SDOM/SDOM_UnitTests.hpp>
#include "Box.hpp"
#include "UnitTests.hpp"

namespace SDOM
{
    DomHandle testBox1;
    DomHandle testBox2;

    bool test1(DomHandle& testBox1) {
        return UnitTests::run("Box: Test #1", "Factory refuses to create with invalid type 'Box1'", [&testBox1]() {
            Box::InitStruct init;
            init.name = "testBox1";
            init.x = 50;
            init.y = 50;
            init.width = 100;
            init.height = 100;
            init.color = {255, 0, 0, 255};
            init.anchorTop = AnchorPoint::TOP_LEFT;
            init.anchorLeft = AnchorPoint::TOP_LEFT;
            init.anchorBottom = AnchorPoint::TOP_LEFT;
            init.anchorRight = AnchorPoint::TOP_LEFT;
            init.z_order = 1;
            init.priority = 1;
            init.isClickable = true;
            init.isEnabled = true;
            init.isHidden = false;
            init.tabPriority = 1;
            init.tabEnabled = true;
            testBox1 = SDOM::getCore().getFactory().create("Box1", init);
            return (testBox1 == nullptr);
        });
    }

    bool test2(DomHandle& testBox2) {
        return UnitTests::run("Box: Test #2", "Factory creates valid Box instance", [&testBox2]() {
            Box::InitStruct init;
            init.name = "testBox2";
            init.x = 325;
            init.y = 150;
            init.width = 200;
            init.height = 200;
            init.color = {64, 0, 32, 255};
            init.anchorTop = AnchorPoint::TOP_LEFT;
            init.anchorLeft = AnchorPoint::TOP_LEFT;
            init.anchorBottom = AnchorPoint::TOP_LEFT;
            init.anchorRight = AnchorPoint::TOP_LEFT;
            init.z_order = 2;
            init.priority = 2;
            init.isClickable = true;
            init.isEnabled = true;
            init.isHidden = false;
            init.tabPriority = 2;
            init.tabEnabled = true;
            testBox2 = SDOM::getCore().getFactory().create("Box", init);
            return (testBox2 != nullptr && testBox2->getType() == Box::TypeName);
        });
    }

    bool test3(DomHandle& testBox2) {
        return UnitTests::run("Box: Test #3", "Add Box as child to Stage", [&testBox2]() {
            DomHandle stage = SDOM::getCore().getStageHandle();
            if (!stage || !testBox2) return false;
            stage->addChild(testBox2);
            return stage->hasChild(testBox2);
        });
    }

    bool test4(DomHandle& testBox2) {
        return UnitTests::run("Box: Test #4", "Remove Box from Stage", [&testBox2]() {
            DomHandle stage = SDOM::getCore().getStageHandle();
            if (!stage || !testBox2) return false;
            stage->removeChild(testBox2);
            return !stage->hasChild(testBox2);
        });
    }

    bool test5(DomHandle& testBox2) {
        return UnitTests::run("Box: Test #5", "Verify Box parent cleared after removal", [&testBox2]() {
            DomHandle stage = SDOM::getCore().getStageHandle();
            if (!stage || !testBox2) return false;
            return (!testBox2->parent_ || testBox2->parent_ != stage);
        });
    }

    bool test6() {
        return UnitTests::run("Box: Test #6", "Create Box from Lua and add to Stage", []() {
            sol::state lua;
            lua.open_libraries(sol::lib::base);
            lua.script(R"(
                boxConfig = {
                    name = "luaBox",
                    type = "Box",
                    x = 400,
                    y = 200,
                    width = 120,
                    height = 80,
                    color = {255, 128, 0, 255},
                    anchorTop = "top|right",
                    anchorLeft = "bottom | left",
                    anchorBottom = "middle& RIGHT",
                    anchorRight = "top+MIDDLE",
                    z_order = 3,
                    priority = 3,
                    isClickable = true,
                    isEnabled = true,
                    isHidden = false,
                    tabPriority = 3,
                    tabEnabled = true
                }
            )");
            sol::table config = lua["boxConfig"];
            if (!SDOM::validateAnchorAssignments(config)) {
                return false;
            }
            DomHandle testBoxLua = SDOM::getCore().getFactory().create("Box", config);
            DomHandle stage = SDOM::getCore().getStageHandle();
            if (!testBoxLua || testBoxLua->getType() != Box::TypeName || !stage) return false;
            stage->addChild(testBoxLua);
            bool result = stage->hasChild(testBoxLua);
            stage->removeChild(testBoxLua); // Clean up after test
            return result;
        });
    }

    bool test7() {
        return UnitTests::run("Box: Test #7", "Create Box from Lua with minimal properties", []() {
            sol::state lua;
            lua.open_libraries(sol::lib::base);
            lua.script(R"(
                boxConfig = {
                    name = "minimalBox",
                    type = "Box",
                    x = 100,
                    y = 100,
                    width = 50,
                    height = 50
                    -- No color, no anchors, no optional properties
                }
            )");
            sol::table config = lua["boxConfig"];
            if (!SDOM::validateAnchorAssignments(config)) {
                return false;
            }
            DomHandle testBoxLua = SDOM::getCore().getFactory().create("Box", config);
            DomHandle stage = SDOM::getCore().getStageHandle();
            if (!testBoxLua || testBoxLua->getType() != Box::TypeName || !stage) return false;
            stage->addChild(testBoxLua);
            bool result = stage->hasChild(testBoxLua);
            stage->removeChild(testBoxLua); // Clean up after test
            return result;
        });
    }

    bool test8() {
        return UnitTests::run("Box: Test #8", "Create Box from Lua with less than minimal properties", []() {
            sol::state lua;
            lua.open_libraries(sol::lib::base);
            lua.script(R"(
                boxConfig = {
                    -- name = "badBox",   -- Intentionally omitted
                    type = "Box",
                    x = 10,
                    y = 10,
                    width = 20,
                    height = 20
                }
            )");
            sol::table config = lua["boxConfig"];
            if (!SDOM::validateAnchorAssignments(config)) {
                return false;
            }
            try {
                DomHandle testBoxLua = SDOM::getCore().getFactory().create("Box", config);
                // If no exception, test fails
                return false;
            } catch (const SDOM::Exception& e) {
                // Exception thrown as expected, test passes
                return true;
            } catch (...) {
                // Any other exception, still treat as pass (or handle as needed)
                return true;
            }
        });
    }

    bool testHierarchyAnchors() 
    {
        return UnitTests::run("Box: Test #9", "Anchor hierarchy world coordinate tests", []() 
        {
            auto makeBox = [](const std::string& name, int x, int y, int w, int h, SDL_Color color) 
            {
                Box::InitStruct init;
                init.name = name;
                init.x = x;
                init.y = y;
                init.width = w;
                init.height = h;
                init.color = color;
                auto box = SDOM::getCore().getFactory().create("Box", init);
                return box;
            };

            // NOTE: For now the boxes should all fit within their parent
            DomHandle greenBox  = makeBox("greenBox", 20, 30, 50, 60, { 0, 128, 0, 255 });  
            DomHandle redBox    = makeBox("redBox",   5, 5, 40, 50, { 200, 0, 0, 255 });
            DomHandle orangeBox = makeBox("orangeBox",5, 5, 30, 40, { 200, 160, 0, 255 });
            DomHandle blueBox   = makeBox("blueBox",  5, 5, 20, 30,  { 0, 0, 200, 255 });

            DomHandle stage = SDOM::getCore().getStageHandle();
            if (!stage || !greenBox || !redBox || !orangeBox || !blueBox) return false;

            stage->addChild(greenBox);
            greenBox->addChild(redBox);
            redBox->addChild(orangeBox);
            orangeBox->addChild(blueBox);

            std::vector<SDOM::AnchorPoint> anchorPoints = {
                SDOM::AnchorPoint::TOP_LEFT, SDOM::AnchorPoint::TOP_CENTER, SDOM::AnchorPoint::TOP_RIGHT,
                SDOM::AnchorPoint::MIDDLE_LEFT, SDOM::AnchorPoint::MIDDLE_CENTER, SDOM::AnchorPoint::MIDDLE_RIGHT,
                SDOM::AnchorPoint::BOTTOM_LEFT, SDOM::AnchorPoint::BOTTOM_CENTER, SDOM::AnchorPoint::BOTTOM_RIGHT
            };            

            Bounds expectedGreenBounds  = greenBox->getBounds();
            Bounds expectedRedBounds    = redBox->getBounds();
            Bounds expectedOrangeBounds = orangeBox->getBounds();
            Bounds expectedBlueBounds   = blueBox->getBounds();

            auto verifyBounds = [](const Bounds& expected, const DomHandle& obj) -> bool {
                Bounds actual = obj->getBounds();
                bool match = expected == actual;
                if (!match) {
                    std::cout << "\nBounds mismatch for '" << obj->getName() << "':\n";
                    std::cout << anchorPointToString_.at(obj->getAnchorLeft()) << " | "
                              << anchorPointToString_.at(obj->getAnchorTop()) << " | "
                              << anchorPointToString_.at(obj->getAnchorRight()) << " | "
                              << anchorPointToString_.at(obj->getAnchorBottom()) << "\n";
                    std::cout << "  Expected: left=" << expected.left << ", top=" << expected.top
                            << ", right=" << expected.right << ", bottom=" << expected.bottom << "\n";
                    std::cout << "  Actual:   left=" << actual.left << ", top=" << actual.top
                            << ", right=" << actual.right << ", bottom=" << actual.bottom << "\n";
                }
                return match;
            };
            auto moveExpected = [&](int dx, int dy) {
                expectedGreenBounds.left   += dx;
                expectedGreenBounds.right  += dx;
                expectedGreenBounds.top    += dy;
                expectedGreenBounds.bottom += dy;

                expectedRedBounds.left     += dx;
                expectedRedBounds.right    += dx;
                expectedRedBounds.top      += dy;
                expectedRedBounds.bottom   += dy;

                expectedOrangeBounds.left  += dx;
                expectedOrangeBounds.right += dx;
                expectedOrangeBounds.top   += dy;
                expectedOrangeBounds.bottom+= dy;

                expectedBlueBounds.left    += dx;
                expectedBlueBounds.right   += dx;
                expectedBlueBounds.top     += dy;
                expectedBlueBounds.bottom  += dy;
            };
            auto moveBoxTest = [&](DomHandle box, int dx, int dy) -> bool {
                int width = box->getWidth();
                int height = box->getHeight();
                box->setX(box->getX() + dx);
                box->setY(box->getY() + dy);
                box->setWidth(width);   // Force re-evaluation of anchors
                box->setHeight(height); // Force re-evaluation of anchors
                moveExpected(dx, dy);

                bool greenOk   = verifyBounds(expectedGreenBounds, greenBox);
                bool redOk     = verifyBounds(expectedRedBounds, redBox);
                bool orangeOk  = verifyBounds(expectedOrangeBounds, orangeBox);
                bool blueOk    = verifyBounds(expectedBlueBounds, blueBox);

                return greenOk && redOk && orangeOk && blueOk;
            };          

            std::vector<DomHandle> boxes = { greenBox, redBox, orangeBox, blueBox };

            std::vector<std::function<void(DomHandle, SDOM::AnchorPoint)>> anchorSetters = {
                [](DomHandle obj, SDOM::AnchorPoint ap) { obj->setAnchorLeft(ap); },
                [](DomHandle obj, SDOM::AnchorPoint ap) { obj->setAnchorTop(ap); },
                [](DomHandle obj, SDOM::AnchorPoint ap) { obj->setAnchorRight(ap); },
                [](DomHandle obj, SDOM::AnchorPoint ap) { obj->setAnchorBottom(ap); }
            };

            bool allAnchorsOk = true;         
            const int delta = 2;
            for (size_t setterIdx = 0; setterIdx < anchorSetters.size(); ++setterIdx) {
                auto& setter = anchorSetters[setterIdx];
                for (auto& box : boxes) {
                    for (const auto& anchor : anchorPoints) {
                        // Reset all anchors on all boxes to TOP_LEFT before each test
                        for (auto& b : boxes) {
                            b->setAnchorLeft(SDOM::AnchorPoint::TOP_LEFT);
                            b->setAnchorTop(SDOM::AnchorPoint::TOP_LEFT);
                            b->setAnchorRight(SDOM::AnchorPoint::TOP_LEFT);
                            b->setAnchorBottom(SDOM::AnchorPoint::TOP_LEFT);
                        }
                        // Set the current anchor on the current box for the current edge
                        setter(box, anchor);

                        // Move ONLY the parent (greenBox)
                        greenBox->setX(greenBox->getX() + delta);
                        greenBox->setY(greenBox->getY() + delta);
                        moveExpected(delta, delta);

                        // std::cout << "Testing " << box->getName() << " with anchors: "
                        //         << anchorPointToString_.at(box->getAnchorLeft()) << " | "
                        //         << anchorPointToString_.at(box->getAnchorTop()) << " | "
                        //         << anchorPointToString_.at(box->getAnchorRight()) << " | "
                        //         << anchorPointToString_.at(box->getAnchorBottom()) << " ... ";

                        bool ret = moveBoxTest(greenBox, 0, 0); // Only move greenBox, but test all bounds
                        // if (ret) {
                        //     std::cout << "passed";
                        // } else {
                        //     std::cout << "FAILED";
                        // }
                        allAnchorsOk &= ret;
                        // std::cout << std::endl;
                    }
                }
            }
            return allAnchorsOk;
        });
    }

    // ----- Run all Box unit tests -----

    bool Box_UnitTests()
    {
        bool allTestsPassed = true;

        // Vector of test functions
        std::vector<std::function<bool()>> tests = {
            [&]() { return test1(testBox1); },
            [&]() { return test2(testBox2); },
            [&]() { return test3(testBox2); },
            [&]() { return test4(testBox2); },
            [&]() { return test5(testBox2); },
            [&]() { return test6(); },
            [&]() { return test7(); },
            [&]() { return test8(); },
            [&]() { return testHierarchyAnchors(); }

        };
        // Run each test and accumulate results
        for (auto& test : tests) {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        // Report test results
        return allTestsPassed;
    }


} // namespace SDOM
