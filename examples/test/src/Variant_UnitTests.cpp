// Variant_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Variant.hpp>

namespace SDOM
{
    // Simple sample dynamic type for testing
    struct TestPoint { int x = 0; int y = 0; };

    bool Variant_test_converter_to_lua(std::vector<std::string>& errors)
    {
        Core& core = getCore();

        // Register a converter for TestPoint once
        static bool registered = false;
        if (!registered) {
            Variant::ConverterEntry ce;
            ce.toLua = [](const VariantStorage::DynamicValue& dv, sol::state_view L) -> sol::object {
                auto p = std::static_pointer_cast<TestPoint>(dv.ptr);
                sol::table t = L.create_table();
                t["x"] = p ? p->x : 0;
                t["y"] = p ? p->y : 0;
                return t;
            };
            ce.fromVariant = [](const Variant& v) -> std::shared_ptr<void> {
                // Minimal: not required for this test
                return nullptr;
            };

            Variant::registerConverter<TestPoint>("TestPoint", std::move(ce));
            registered = true;
        }

        // Create a dynamic Point and wrap in Variant
        auto p = std::make_shared<TestPoint>();
        p->x = 7; p->y = 11;
        Variant v(p);

        sol::object o = v.toLua(core.getLua());
        if (!o.is<sol::table>()) {
            errors.push_back("Variant::toLua did not produce a table for TestPoint");
            return true;
        }

        sol::table t = o;
        int x = t["x"]; int y = t["y"];
        if (x != 7 || y != 11) {
            errors.push_back("TestPoint field values mismatch from Variant::toLua");
        }

        return true;
    }

    bool Variant_UnitTests()
    {
        const std::string objName = "Variant";
        UnitTests& ut = UnitTests::getInstance();

        static bool registered = false;
        if (!registered) {
            ut.add_test(objName, "Converter: TestPoint -> Lua table", Variant_test_converter_to_lua);
            registered = true;
        }

        return true;
    }

} // namespace SDOM
