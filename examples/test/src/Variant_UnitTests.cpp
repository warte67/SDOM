// Variant_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Variant.hpp>
#include <thread>

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
    } // END: Variant_test_converter_to_lua()

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
    } // END: Variant_test_converter_roundtrip()

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
    } // END: Variant_test_null_and_error()

    // Separate named tests (extracted from inline lambdas) for readability
    bool Variant_test_converter_lookup_by_name(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        sol::state& L = core.getLua();

        // Ensure converter was registered by previous test registration
        auto ce = Variant::getConverterByName("TestPoint");
        if (!ce) { errors.push_back("Converter lookup by name failed"); return true; }

        // Build a DynamicValue representing a TestPoint and convert via converter
        auto p = std::make_shared<TestPoint>(); p->x = 3; p->y = 4;
        auto dyn = Variant::makeDynamicValue<TestPoint>(p);
        sol::object o = ce->toLua(dyn, L);
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
    }

    bool Variant_test_variantview_basic_access(std::vector<std::string>& errors)
    {
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
    }

    bool Variant_test_luaref_lifetime_validation(std::vector<std::string>& errors)
    {
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
    }

    bool Variant_test_table_storage_and_snapshot(std::vector<std::string>& errors)
    {
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
    }

    bool Variant_test_dynamic_metadata_accessors(std::vector<std::string>& errors)
    {
        // Ensure TestPoint converter was registered
        auto ce = Variant::getConverterByName("TestPoint");
        if (!ce) { errors.push_back("Dynamic metadata: converter missing"); return true; }
        auto p = std::make_shared<TestPoint>();
        Variant v = Variant::makeDynamic<TestPoint>(p);
        auto tn = v.dynamicTypeName();
        if (!tn || *tn != std::string("TestPoint")) errors.push_back("dynamicTypeName() returned unexpected value");
        auto ti = v.dynamicTypeIndex();
        if (ti != std::type_index(typeid(TestPoint))) errors.push_back("dynamicTypeIndex() mismatch");
        return true;
    }

    // Additional tests requested: numeric coercion stress, deep snapshot recursion,
    // threaded converter registration, and composite equality cases.

    bool Variant_test_numeric_coercion_stress(std::vector<std::string>& errors)
    {
        // Basic deterministic cases covering ints, reals, bools and numeric strings
        struct Case { Variant v; int64_t expectInt; double expectDouble; bool expectBool; };
        std::vector<Case> cases{
            { Variant(int64_t(0)), 0, 0.0, false },
            { Variant(int64_t(1)), 1, 1.0, true },
            { Variant(int64_t(-5)), -5, -5.0, true },
            { Variant(3.14), 3, 3.14, true },
            // string values: toBool only recognizes "true" or "1" as true
            { Variant("42"), 42, 42.0, false },
            { Variant("-7"), -7, -7.0, false },
            { Variant("3.5"), 3, 3.5, false },
            { Variant("not_a_number"), 0, 0.0, false }
        };

        for (size_t i = 0; i < cases.size(); ++i) {
            const auto &c = cases[i];
            int64_t vi = c.v.toInt64(0);
            double vd = c.v.toDouble(0.0);
            bool vb = c.v.toBool(false);
            // For the string non-number case we expect defaults (we use 0 here)
            if (i == cases.size()-1) {
                if (vi != 0) errors.push_back("Numeric coercion: unexpected toInt64 for non-number case");
            } else {
                if (vi != c.expectInt) errors.push_back("Numeric coercion: toInt64 mismatch");
            }
            // allow small rounding differences for double
            if (std::fabs(vd - c.expectDouble) > 1e-9) errors.push_back("Numeric coercion: toDouble mismatch");
            if (vb != c.expectBool) errors.push_back("Numeric coercion: toBool mismatch");
        }

        // Edge cases: large int near limits
        Variant big = Variant(int64_t(std::numeric_limits<int64_t>::max()));
        if (big.toInt64(0) != std::numeric_limits<int64_t>::max()) errors.push_back("Numeric coercion: big int mismatch");

        return true;
    }

    bool Variant_test_deep_recursion_snapshot(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        sol::state& L = core.getLua();

        // Temporarily keep LuaRef mode to create a LuaRef Variant
        auto prev = Variant::getTableStorageMode();
        Variant::setTableStorageMode(Variant::TableStorageMode::KeepLuaRef);

        sol::table t = L.create_table();
        sol::table cur = t;
    const int depth = 30; // deep but safe for stack across environments
        for (int i = 0; i < depth; ++i) {
            sol::table child = L.create_table();
            // ensure child is treated as an object (non-empty) by fromLuaTable_
            child["__mark"] = 1;
            cur["inner"] = child;
            cur = child;
        }

    Variant v = Variant::fromLuaObject(t);
    if (!v.isLuaRef()) { errors.push_back("Deep snapshot: expected LuaRef"); Variant::setTableStorageMode(prev); return true; }

    // Ensure nested tables are deep-copied by snapshot(): temporarily switch to Copy
    Variant::setTableStorageMode(Variant::TableStorageMode::Copy);
    Variant snap = v.snapshot();
        if (!snap.isObject()) { errors.push_back("Deep snapshot: snapshot did not produce object"); Variant::setTableStorageMode(prev); return true; }

        const Variant* node = &snap;
        for (int i = 0; i < depth; ++i) {
            if (!node || !node->isObject()) { errors.push_back("Deep snapshot: traversal failed at depth " + std::to_string(i)); break; }
            node = node->get("inner");
        }

    Variant::setTableStorageMode(prev);
        return true;
    }

    bool Variant_test_threaded_converter_registration(std::vector<std::string>& errors)
    {
        // Spawn several threads which register converters by name to exercise the registry mutex
        const int threads = 6;
        std::vector<std::thread> ths;
        for (int i = 0; i < threads; ++i) {
            ths.emplace_back([i]() {
                Variant::ConverterEntry ce;
                ce.toLua = [](const VariantStorage::DynamicValue& dv, sol::state_view L)->sol::object {
                    return sol::make_object(L, sol::lua_nil);
                };
                ce.fromVariant = [](const Variant& v)->std::shared_ptr<void> { return nullptr; };
                Variant::registerConverterByName(std::string("ThreadConv_") + std::to_string(i), std::move(ce));
            });
        }
        for (auto &t : ths) t.join();

        // Verify registration
        for (int i = 0; i < threads; ++i) {
            auto name = std::string("ThreadConv_") + std::to_string(i);
            if (!Variant::getConverterByName(name)) errors.push_back("Threaded registration: missing converter " + name);
        }

        return true;
    }

    bool Variant_test_equality_composite_types(std::vector<std::string>& errors)
    {
        // Arrays equality (order-sensitive)
        Variant a1 = Variant::makeArray(); a1.push(Variant(1)); a1.push(Variant(2));
        Variant a2 = Variant::makeArray(); a2.push(Variant(1)); a2.push(Variant(2));
        Variant a3 = Variant::makeArray(); a3.push(Variant(2)); a3.push(Variant(1));
        if (!(a1 == a2)) errors.push_back("Equality: identical arrays not equal");
        if (a1 == a3) errors.push_back("Equality: different-order arrays considered equal");

        // Objects equality (order-insensitive)
        Variant o1 = Variant::makeObject(); o1.set("a", Variant(1)); o1.set("b", Variant(2));
        Variant o2 = Variant::makeObject(); o2.set("b", Variant(2)); o2.set("a", Variant(1));
        if (!(o1 == o2)) errors.push_back("Equality: equivalent objects not equal");

        // Numeric cross-type equality
        Variant vi = Variant(int64_t(42));
        Variant vf = Variant(42.0);
        if (!(vi == vf)) errors.push_back("Equality: int and double with same value not equal");

        // Dynamic equality (pointer identity + type)
        auto sp = std::make_shared<int>(7);
        Variant d1 = Variant::makeDynamic<int>(sp);
        Variant d2 = Variant::makeDynamic<int>(sp);
        if (!(d1 == d2)) errors.push_back("Equality: dynamic variants with same pointer not equal");

        auto sp2 = std::make_shared<int>(7);
        Variant d3 = Variant::makeDynamic<int>(sp2);
        if (d1 == d3) errors.push_back("Equality: dynamic variants with different pointers considered equal");

        return true;
    }

    // Complex converter examples (previously a separate file). Keep with Variant tests.
    struct Inner { int a = 0; std::string name; };
    struct Outer {
        Inner inner;
        std::vector<int> arr;
        std::unordered_map<std::string,int> map;
    };

    bool ComplexVariant_test_outer_converter(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        sol::state& L = core.getLua();

        // Register converter for Inner
        static bool reg_inner = false;
        if (!reg_inner) {
            Variant::ConverterEntry ice;
            ice.toLua = [](const VariantStorage::DynamicValue& dv, sol::state_view L)->sol::object {
                auto p = std::static_pointer_cast<Inner>(dv.ptr);
                sol::table t = L.create_table();
                t["a"] = p->a;
                t["name"] = p->name;
                return t;
            };
            ice.fromVariant = [](const Variant& v)->std::shared_ptr<void> {
                auto out = std::make_shared<Inner>();
                if (v.isObject()) {
                    const Variant* va = v.get("a");
                    const Variant* vn = v.get("name");
                    if (va) out->a = static_cast<int>(va->toInt64());
                    if (vn) out->name = vn->toString();
                    return std::static_pointer_cast<void>(out);
                }
                return nullptr;
            };
            Variant::registerConverter<Inner>("Inner", std::move(ice));
            reg_inner = true;
        }

        // Register converter for Outer
        static bool reg_outer = false;
        if (!reg_outer) {
            Variant::ConverterEntry oce;
            oce.toLua = [](const VariantStorage::DynamicValue& dv, sol::state_view L)->sol::object {
                auto p = std::static_pointer_cast<Outer>(dv.ptr);
                sol::table t = L.create_table();
                // inner
                sol::table inner = L.create_table();
                inner["a"] = p->inner.a;
                inner["name"] = p->inner.name;
                t["inner"] = inner;
                // arr
                sol::table arrtbl = L.create_table();
                for (size_t i=0;i<p->arr.size();++i) arrtbl[i+1] = p->arr[i];
                t["arr"] = arrtbl;
                // map
                sol::table mt = L.create_table();
                for (auto &kv : p->map) mt[kv.first] = kv.second;
                t["map"] = mt;
                return t;
            };
            oce.fromVariant = [](const Variant& v)->std::shared_ptr<void> {
                auto out = std::make_shared<Outer>();
                if (!v.isObject()) return nullptr;
                const Variant* vin = v.get("inner");
                if (vin && vin->isObject()) {
                    const Variant* va = vin->get("a");
                    const Variant* vn = vin->get("name");
                    if (va) out->inner.a = static_cast<int>(va->toInt64());
                    if (vn) out->inner.name = vn->toString();
                }
                const Variant* varr = v.get("arr");
                if (varr && varr->isArray()) {
                    for (size_t i=0;i<varr->size();++i) {
                        const Variant* e = varr->at(i);
                        if (e) out->arr.push_back(static_cast<int>(e->toInt64()));
                    }
                }
                const Variant* vmap = v.get("map");
                if (vmap && vmap->isObject()) {
                    // iterate entries (object() returns a pointer to the underlying map)
                    if (auto obj = vmap->object()) {
                        for (const auto &kv : *obj) {
                            out->map[kv.first] = static_cast<int>(kv.second.toInt64());
                        }
                    }
                }
                return std::static_pointer_cast<void>(out);
            };
            Variant::registerConverter<Outer>("Outer", std::move(oce));
            reg_outer = true;
        }

        // Build an Outer instance and round-trip
        auto out_inst = std::make_shared<Outer>();
        out_inst->inner.a = 9; out_inst->inner.name = "nine";
        out_inst->arr = {1,2,3};
        out_inst->map["one"] = 1; out_inst->map["two"] = 2;

        Variant v(out_inst);
        sol::object o = v.toLua(L);
        if (!o.is<sol::table>()) { errors.push_back("Outer toLua did not produce table"); return true; }

        Variant v2 = Variant::fromLuaObject(o);
        auto ce = Variant::getConverterByName("Outer");
        if (!ce) { errors.push_back("Outer converter missing"); return true; }
        auto pv = ce->fromVariant(v2);
        if (!pv) { errors.push_back("Outer fromVariant returned null"); return true; }
        auto p2 = std::static_pointer_cast<Outer>(pv);
        if (!p2) { errors.push_back("Outer cast failed"); return true; }
        if (p2->inner.a != 9 || p2->inner.name != "nine") errors.push_back("Outer.inner mismatch");
        if (p2->arr.size() != 3 || p2->arr[0] != 1) errors.push_back("Outer.arr mismatch");
        if (p2->map["one"] != 1) errors.push_back("Outer.map mismatch");

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
            ut.add_test(objName, "Converter: lookup by name", Variant_test_converter_lookup_by_name);
            ut.add_test(objName, "Converter chain: Outer nested/arrays/maps", ComplexVariant_test_outer_converter);
            ut.add_test(objName, "Numeric coercion stress", Variant_test_numeric_coercion_stress);
            ut.add_test(objName, "Deep recursion snapshot", Variant_test_deep_recursion_snapshot);
            ut.add_test(objName, "Threaded converter registration", Variant_test_threaded_converter_registration);
            ut.add_test(objName, "Equality: composite types", Variant_test_equality_composite_types);
            ut.add_test(objName, "Null/Error semantics", Variant_test_null_and_error);
            ut.add_test(objName, "VariantView basic access", Variant_test_variantview_basic_access);
            ut.add_test(objName, "LuaRef lifetime validation", Variant_test_luaref_lifetime_validation);
            ut.add_test(objName, "Table storage mode toggle and snapshot", Variant_test_table_storage_and_snapshot);
            registered = true;
        }

        // Small test: dynamicTypeName accessor
        ut.add_test(objName, "Dynamic metadata accessors", Variant_test_dynamic_metadata_accessors);

        return true;
    }

} // namespace SDOM
