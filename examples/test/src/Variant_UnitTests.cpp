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
                // Convert a Variant (object or array) back into a TestPoint instance.
                auto out = std::make_shared<TestPoint>();
                if (v.isObject()) {
                    const Variant* vx = v.get("x");
                    const Variant* vy = v.get("y");
                    if (vx) out->x = static_cast<int>(vx->toInt64());
                    if (vy) out->y = static_cast<int>(vy->toInt64());
                    return std::static_pointer_cast<void>(out);
                }
                if (v.isArray()) {
                    const Variant* vx = v.at(0);
                    const Variant* vy = v.at(1);
                    if (vx) out->x = static_cast<int>(vx->toInt64());
                    if (vy) out->y = static_cast<int>(vy->toInt64());
                    return std::static_pointer_cast<void>(out);
                }
                // Unsupported shape
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

    bool Variant_test_converter_roundtrip(std::vector<std::string>& errors)
    {
        Core& core = getCore();

        // create a TestPoint, convert to Variant (dynamic), to Lua, back to Variant, then back to C++
        auto p = std::make_shared<TestPoint>();
        p->x = 13; p->y = 17;
        Variant v(p);

        sol::object o = v.toLua(core.getLua());
        if (!o.is<sol::table>()) {
            errors.push_back("Roundtrip: toLua did not produce a table");
            return true;
        }

        // Convert Lua table back to Variant
        Variant v2 = Variant::fromLuaObject(o);

        // Lookup converter and attempt fromVariant
        auto ce = Variant::getConverter(std::type_index(typeid(TestPoint)));
        if (!ce) {
            errors.push_back("Roundtrip: converter missing for TestPoint");
            return true;
        }

        std::shared_ptr<void> pv = ce->fromVariant(v2);
        if (!pv) {
            errors.push_back("Roundtrip: fromVariant returned null");
            return true;
        }

        auto p2 = std::static_pointer_cast<TestPoint>(pv);
        if (!p2) { errors.push_back("Roundtrip: cast failed"); return true; }
        if (p2->x != 13 || p2->y != 17) errors.push_back("Roundtrip: value mismatch after conversion");

        return true;
    }

    bool Variant_test_null_and_error(std::vector<std::string>& errors)
    {
        Core& core = getCore();

        // Default-constructed Variant should be null
        Variant v;
        if (!v.isNull()) {
            errors.push_back("Default-constructed Variant is not null");
        }
        if (v.type() != VariantType::Null) {
            errors.push_back("Variant::type() for default Variant is not Null");
        }
        sol::object o = v.toLua(core.getLua());
        if (!o.is<sol::nil_t>()) {
            errors.push_back("Variant::toLua for null Variant did not return nil");
        }

        // makeError should set error flag and message and report Error type
        Variant e = Variant::makeError("boom");
        if (!e.hasError()) {
            errors.push_back("Variant::makeError did not set error flag");
        }
        if (e.errorMessage() != std::string("boom")) {
            errors.push_back("Variant::makeError errorMessage mismatch");
        }
        if (e.type() != VariantType::Error) {
            errors.push_back("Variant::type() for error Variant is not Error");
        }
        sol::object eo = e.toLua(core.getLua());
        if (!eo.is<sol::nil_t>()) {
            errors.push_back("Variant::toLua for error Variant did not return nil");
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
            ut.add_test(objName, "Converter: TestPoint roundtrip", Variant_test_converter_roundtrip);
            ut.add_test(objName, "Converter: lookup by name", [](std::vector<std::string>& errors)->bool{
                Core& core = getCore();
                sol::state& L = core.getLua();

                // Ensure converter was registered by previous test registration
                auto ce = Variant::getConverterByName("TestPoint");
                if (!ce) { errors.push_back("Converter lookup by name failed"); return true; }

                // Build a DynamicValue representing a TestPoint
                auto p = std::make_shared<TestPoint>(); p->x = 3; p->y = 4;
                auto dv = Variant::makeDynamicValue<TestPoint>(p);

                sol::object o = ce->toLua(dv, L);
                if (!o.is<sol::table>()) { errors.push_back("Converter toLua by name did not produce table"); return true; }
                sol::table t = o;
                int xx = t["x"]; int yy = t["y"];
                if (xx != 3 || yy != 4) errors.push_back("Converter toLua by name produced wrong values");

                // Now convert back via fromVariant
                Variant v = Variant::fromLuaObject(o);
                auto pv = ce->fromVariant(v);
                if (!pv) { errors.push_back("Converter fromVariant by name returned null"); return true; }
                auto p2 = std::static_pointer_cast<TestPoint>(pv);
                if (!p2) { errors.push_back("Converter fromVariant by name cast failed"); return true; }
                if (p2->x != 3 || p2->y != 4) errors.push_back("Converter fromVariant by name value mismatch");

                return true;
            });
            ut.add_test(objName, "Null/Error semantics", Variant_test_null_and_error);
            ut.add_test(objName, "VariantView basic access", [](std::vector<std::string>& errors)->bool{
                // Array view
                Variant v = Variant::makeArray();
                v.push(Variant(1));
                v.push(Variant(2));
                Variant::VariantView view(v);
                if (!view.isArray()) { errors.push_back("VariantView did not detect array"); }
                if (view.size() != 2) { errors.push_back("VariantView size mismatch for array"); }
                const Variant* e0 = view.at(0);
                if (!e0 || e0->toInt64() != 1) errors.push_back("VariantView array element mismatch");

                // Object view
                Variant obj = Variant::makeObject();
                obj.set("a", Variant(42));
                Variant::VariantView vobj(obj);
                if (!vobj.isObject()) { errors.push_back("VariantView did not detect object"); }
                const Variant* val = vobj.get("a");
                if (!val || val->toInt64() != 42) errors.push_back("VariantView object access mismatch");

                return true;
            });

            ut.add_test(objName, "LuaRef lifetime validation", [](std::vector<std::string>& errors)->bool{
                Core& core = getCore();
                sol::state& L = core.getLua();
                // Tables are converted into Variant objects/arrays, not opaque Lua refs
                sol::table t = L.create_table();
                t["x"] = 5;
                Variant v = Variant::fromLuaObject(t);
                if (!v.isObject()) { errors.push_back("Expected Variant object from table"); }
                // toLua with same state should return a table with the value
                sol::object o1 = v.toLua(L);
                if (!o1.is<sol::table>()) errors.push_back("toLua did not return table for same state");
                if (o1.is<sol::table>()) {
                    sol::table rt = o1.as<sol::table>();
                    sol::object v_x = rt["x"];
                    if (v_x.get_type() != sol::type::number || v_x.as<int>() != 5)
                        errors.push_back("toLua returned table missing expected key/value");
                }

                // But opaque values (functions/userdata) are stored as LuaRef and must be validated against state
                // create a named function in the Lua state and retrieve it safely
                L.script("function __variant_test_fn() return 1 end");
                sol::object fn = L["__variant_test_fn"];
                Variant vf = Variant::fromLuaObject(fn);
                if (!vf.isLuaRef()) { errors.push_back("Expected LuaRef for function object"); }
                // toLua with same state returns the function
                sol::object f1 = vf.toLua(L);
                if (!f1.is<sol::function>()) errors.push_back("toLua did not return function for same state");
                // different lua_State should yield nil for stored LuaRef
                sol::state tmp; tmp.open_libraries(sol::lib::base);
                sol::object f2 = vf.toLua(tmp);
                if (!f2.is<sol::nil_t>()) errors.push_back("toLua returned non-nil for mismatched lua_State on LuaRef");

                // lightuserdata: create a lightuserdata value and verify LuaRef storage
                void* p = nullptr;
                sol::object ud_obj = sol::make_object(L, p);
                Variant vud = Variant::fromLuaObject(ud_obj);
                if (!vud.isLuaRef()) errors.push_back("Expected LuaRef for lightuserdata");
                sol::object ud_same = vud.toLua(L);
                if (ud_same.get_type() != sol::type::lightuserdata) errors.push_back("toLua did not return lightuserdata for same state");
                sol::state tmp2; tmp2.open_libraries(sol::lib::base);
                sol::object ud_other = vud.toLua(tmp2);
                if (!ud_other.is<sol::nil_t>()) errors.push_back("toLua returned non-nil for mismatched lua_State on lightuserdata LuaRef");

                // thread/coroutine: create via coroutine.create and verify LuaRef behavior
                // The coroutine library may not be available in the embedded Lua state; only run this test when present.
                if (L["coroutine"].valid()) {
                    L.script("th = coroutine.create(function() coroutine.yield() end)");
                    sol::object th = L["th"];
                    Variant vth = Variant::fromLuaObject(th);
                    if (!vth.isLuaRef()) errors.push_back("Expected LuaRef for thread");
                    sol::object th_same = vth.toLua(L);
                    if (th_same.get_type() != sol::type::thread) errors.push_back("toLua did not return thread for same state");
                    sol::state tmp3; tmp3.open_libraries(sol::lib::base);
                    sol::object th_other = vth.toLua(tmp3);
                    if (!th_other.is<sol::nil_t>()) errors.push_back("toLua returned non-nil for mismatched lua_State on thread LuaRef");
                }

                return true;
            });

            ut.add_test(objName, "Table storage mode toggle and snapshot", [](std::vector<std::string>& errors)->bool{
                Core& core = getCore();
                sol::state& L = core.getLua();

                // remember and restore previous mode
                auto prev = Variant::getTableStorageMode();
                Variant::setTableStorageMode(Variant::TableStorageMode::KeepLuaRef);

                sol::table t = L.create_table();
                t["a"] = 123;
                Variant v = Variant::fromLuaObject(t);
                if (!v.isLuaRef()) errors.push_back("Expected LuaRef when table storage mode is KeepLuaRef");

                // snapshot should produce an object copy of the table
                Variant snap = v.snapshot();
                if (!snap.isObject()) errors.push_back("Snapshot did not produce an object Variant");
                const Variant* pa = snap.get("a");
                if (!pa || pa->toInt64() != 123) errors.push_back("Snapshot value mismatch");

                // restore
                Variant::setTableStorageMode(prev);
                return true;
            });
            registered = true;
        }

        return true;
    }

} // namespace SDOM
