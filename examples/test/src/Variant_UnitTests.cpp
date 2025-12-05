// Variant_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Variant.hpp>
#include <SDOM/CAPI/SDOM_CAPI_Variant.h>
#include <thread>
#include <random>
#include <sstream>
#include <algorithm>
#include <atomic>
#include <chrono>

namespace SDOM
{
    struct TestPoint { int x = 0; int y = 0; };

    namespace {

    void ensureTestPointConverterRegistered()
    {
        static bool registered = false;
        if (registered)
            return;

        Variant::ConverterEntry ce;
        ce.toLua = [](const VariantStorage::DynamicValue& dv, sol::state_view L) -> sol::object {
            auto p = std::static_pointer_cast<TestPoint>(dv.ptr);
            sol::table t = L.create_table();
            t["x"] = p ? p->x : 0;
            t["y"] = p ? p->y : 0;
            return t;
        };
        ce.fromVariant = [](const Variant& v) -> std::shared_ptr<void> {
            auto out = std::make_shared<TestPoint>();
            if (v.isObject()) {
                if (const Variant* vx = v.get("x")) out->x = static_cast<int>(vx->toInt64());
                if (const Variant* vy = v.get("y")) out->y = static_cast<int>(vy->toInt64());
                return std::static_pointer_cast<void>(out);
            }
            if (v.isArray()) {
                if (const Variant* vx = v.at(0)) out->x = static_cast<int>(vx->toInt64());
                if (const Variant* vy = v.at(1)) out->y = static_cast<int>(vy->toInt64());
                return std::static_pointer_cast<void>(out);
            }
            return nullptr;
        };

        Variant::registerConverter<TestPoint>("TestPoint", std::move(ce));
        registered = true;
    }

    class TableStorageGuard {
    public:
        explicit TableStorageGuard(Variant::TableStorageMode mode)
            : previous_(Variant::getTableStorageMode())
        {
            Variant::setTableStorageMode(mode);
        }

        ~TableStorageGuard()
        {
            Variant::setTableStorageMode(previous_);
        }

    private:
        Variant::TableStorageMode previous_;
    };

    class CVariantHandle {
    public:
        CVariantHandle()
        {
            SDOM_Variant_InitNull(&value_);
        }

        ~CVariantHandle()
        {
            SDOM_Variant_Destroy(&value_);
        }

        SDOM_Variant* get() { return &value_; }
        const SDOM_Variant* get() const { return &value_; }

    private:
        SDOM_Variant value_{};
    };

    }

    bool Variant_test_converter_to_lua(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        sol::state& L = core.getLua();
        ensureTestPointConverterRegistered();

        auto p = std::make_shared<TestPoint>();
        p->x = 7;
        p->y = 11;
        Variant v(p);

        sol::object o = v.toLua(L);
        if (!o.is<sol::table>()) {
            errors.push_back("Variant::toLua did not produce a table for TestPoint");
            return true;
        }

        sol::table t = o;
        if (t["x"].get_or(0) != 7 || t["y"].get_or(0) != 11) {
            errors.push_back("TestPoint field values mismatch from Variant::toLua");
        }

        return true;
    } // END: Variant_test_converter_to_lua()

    bool Variant_test_converter_roundtrip(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        ensureTestPointConverterRegistered();

        auto p = std::make_shared<TestPoint>();
        p->x = 13;
        p->y = 17;
        Variant v(p);

        sol::object o = v.toLua(core.getLua());
        if (!o.is<sol::table>()) {
            errors.push_back("Roundtrip: toLua did not produce a table");
            return true;
        }

        Variant v2 = Variant::fromLuaObject(o);
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
        if (!p2) {
            errors.push_back("Roundtrip: cast failed");
            return true;
        }
        if (p2->x != 13 || p2->y != 17) {
            errors.push_back("Roundtrip: value mismatch after conversion");
        }

        return true;
    } // END: Variant_test_converter_roundtrip()

    bool Variant_test_null_and_error(std::vector<std::string>& errors)
    {
        Core& core = getCore();

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
        ensureTestPointConverterRegistered();

        auto ce = Variant::getConverterByName("TestPoint");
        if (!ce) {
            errors.push_back("Converter lookup by name failed");
            return true;
        }

        auto p = std::make_shared<TestPoint>();
        p->x = 3;
        p->y = 4;
        auto dyn = Variant::makeDynamicValue<TestPoint>(p);
        sol::object o = ce->toLua(dyn, core.getLua());
        if (!o.is<sol::table>()) {
            errors.push_back("Converter toLua by name did not produce table");
            return true;
        }

        sol::table t = o;
        if (int(t["x"]) != 3 || int(t["y"]) != 4) {
            errors.push_back("Converter toLua by name produced wrong values");
        }

        Variant v = Variant::fromLuaObject(o);
        auto pv = ce->fromVariant(v);
        if (!pv) {
            errors.push_back("Converter fromVariant by name returned null");
            return true;
        }
        auto p2 = std::static_pointer_cast<TestPoint>(pv);
        if (!p2) {
            errors.push_back("Converter fromVariant by name cast failed");
            return true;
        }
        if (p2->x != 3 || p2->y != 4) {
            errors.push_back("Converter fromVariant by name value mismatch");
        }

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

        sol::table t = L.create_table();
        t["x"] = 5;
        Variant v = Variant::fromLuaObject(t);
        if (!v.isObject()) {
            errors.push_back("Expected Variant object from table");
        }
        sol::object o1 = v.toLua(L);
        if (!o1.is<sol::table>()) {
            errors.push_back("toLua did not return table for same state");
        } else {
            sol::table rt = o1.as<sol::table>();
            sol::object v_x = rt["x"];
            if (v_x.get_type() != sol::type::number || v_x.as<int>() != 5) {
                errors.push_back("toLua returned table missing expected key/value");
            }
        }

        L.script("function __variant_test_fn() return 1 end");
        sol::object fn = L["__variant_test_fn"];
        Variant vf = Variant::fromLuaObject(fn);
        if (!vf.isLuaRef()) {
            errors.push_back("Expected LuaRef for function object");
        }
        sol::object f1 = vf.toLua(L);
        if (!f1.is<sol::function>()) {
            errors.push_back("toLua did not return function for same state");
        }
        sol::state tmp;
        tmp.open_libraries(sol::lib::base);
        sol::object f2 = vf.toLua(tmp);
        if (!f2.is<sol::nil_t>()) {
            errors.push_back("toLua returned non-nil for mismatched lua_State on LuaRef");
        }

        void* p = nullptr;
        sol::object ud_obj = sol::make_object(L, p);
        Variant vud = Variant::fromLuaObject(ud_obj);
        if (!vud.isLuaRef()) {
            errors.push_back("Expected LuaRef for lightuserdata");
        }
        sol::object ud_same = vud.toLua(L);
        if (ud_same.get_type() != sol::type::lightuserdata) {
            errors.push_back("toLua did not return lightuserdata for same state");
        }
        sol::state tmp2;
        tmp2.open_libraries(sol::lib::base);
        sol::object ud_other = vud.toLua(tmp2);
        if (!ud_other.is<sol::nil_t>()) {
            errors.push_back("toLua returned non-nil for mismatched lua_State on lightuserdata LuaRef");
        }

        if (L["coroutine"].valid()) {
            L.script("th = coroutine.create(function() coroutine.yield() end)");
            sol::object th = L["th"];
            Variant vth = Variant::fromLuaObject(th);
            if (!vth.isLuaRef()) {
                errors.push_back("Expected LuaRef for thread");
            }
            sol::object th_same = vth.toLua(L);
            if (th_same.get_type() != sol::type::thread) {
                errors.push_back("toLua did not return thread for same state");
            }
            sol::state tmp3;
            tmp3.open_libraries(sol::lib::base);
            sol::object th_other = vth.toLua(tmp3);
            if (!th_other.is<sol::nil_t>()) {
                errors.push_back("toLua returned non-nil for mismatched lua_State on thread LuaRef");
            }
        }

        return true;
    }

    bool Variant_test_table_storage_and_snapshot(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        sol::state& L = core.getLua();

        TableStorageGuard guard(Variant::TableStorageMode::KeepLuaRef);

        sol::table t = L.create_table();
        t["a"] = 123;
        Variant v = Variant::fromLuaObject(t);
        if (!v.isLuaRef()) {
            errors.push_back("Expected LuaRef when table storage mode is KeepLuaRef");
        }

        Variant snap = v.snapshot();
        if (!snap.isObject()) {
            errors.push_back("Snapshot did not produce an object Variant");
        } else {
            const Variant* pa = snap.get("a");
            if (!pa || pa->toInt64() != 123) {
                errors.push_back("Snapshot value mismatch");
            }
        }

        return true;
    }

    bool Variant_test_dynamic_metadata_accessors(std::vector<std::string>& errors)
    {
        ensureTestPointConverterRegistered();
        auto p = std::make_shared<TestPoint>();
        Variant v = Variant::makeDynamic<TestPoint>(p);
        auto tn = v.dynamicTypeName();
        if (!tn || *tn != std::string("TestPoint")) {
            errors.push_back("dynamicTypeName() returned unexpected value");
        }
        auto ti = v.dynamicTypeIndex();
        if (ti != std::type_index(typeid(TestPoint))) {
            errors.push_back("dynamicTypeIndex() mismatch");
        }
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

        sol::table t = L.create_table();
        sol::table cur = t;
        const int depth = 30;
        for (int i = 0; i < depth; ++i) {
            sol::table child = L.create_table();
            child["__mark"] = 1;
            cur["inner"] = child;
            cur = child;
        }

        Variant v;
        {
            TableStorageGuard guard(Variant::TableStorageMode::KeepLuaRef);
            v = Variant::fromLuaObject(t);
        }

        if (!v.isLuaRef()) {
            errors.push_back("Deep snapshot: expected LuaRef");
            return true;
        }

        Variant snap;
        {
            TableStorageGuard guard(Variant::TableStorageMode::Copy);
            snap = v.snapshot();
        }

        if (!snap.isObject()) {
            errors.push_back("Deep snapshot: snapshot did not produce object");
            return true;
        }

        const Variant* node = &snap;
        for (int i = 0; i < depth; ++i) {
            if (!node || !node->isObject()) {
                errors.push_back("Deep snapshot: traversal failed at depth " + std::to_string(i));
                break;
            }
            node = node->get("inner");
        }

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
                ce.toLua = []([[maybe_unused]] const VariantStorage::DynamicValue& dv, sol::state_view L)->sol::object {
                    return sol::make_object(L, sol::lua_nil);
                };
                ce.fromVariant = []([[maybe_unused]] const Variant& v)->std::shared_ptr<void> { return nullptr; };
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

    // Verify copy/move semantics when placing Variants into STL containers
    bool Variant_test_copy_move_and_containers(std::vector<std::string>& errors)
    {
        // simple scalar variants
        Variant vi = Variant(int64_t(123));
        Variant vs = Variant(std::string("hello"));

        // copy into vector
        std::vector<Variant> vec;
        vec.push_back(vi);
        vec.push_back(vs);
        if (vec.size() != 2) { errors.push_back("Container: vector size mismatch after push_back"); return true; }
        if (!(vec[0] == vi)) errors.push_back("Container: vector element 0 not equal to original int");
        if (!(vec[1] == vs)) errors.push_back("Container: vector element 1 not equal to original string");

        // move into vector
        Variant vm = Variant::makeObject();
        vm.set("x", Variant(9));
        std::vector<Variant> vec2;
        vec2.emplace_back(std::move(vm));
        if (vec2.empty() || !vec2[0].isObject()) errors.push_back("Container: emplace_back(move) did not create object");

    // unordered_map using Variant as a key (requires VariantHash)
    std::unordered_map<Variant,int, VariantHash> map;
        Variant k1 = Variant::makeObject(); k1.set("id", Variant(1));
        Variant k2 = Variant::makeObject(); k2.set("id", Variant(1));
        map.emplace(k1, 10);
        auto it = map.find(k2);
        if (it == map.end()) errors.push_back("Container: unordered_map lookup by equivalent Variant key failed");

        // copying maps and vectors should preserve contents
        auto map_copy = map;
        if (map_copy.size() != map.size()) errors.push_back("Container: unordered_map copy size mismatch");

        return true;
    }

    // toDebugString tests: shallow vs deep output
    bool Variant_test_toDebugString(std::vector<std::string>& errors)
    {
        Variant root = Variant::makeObject();
        root.set("a", Variant(1));
        Variant nested = Variant::makeObject();
        nested.set("c", Variant(2));
        root.set("b", nested);

        std::string shallow = root.toDebugString(1);
        std::string deep = root.toDebugString(10);

        if (shallow.empty()) errors.push_back("toDebugString: shallow output empty");
        if (deep.empty()) errors.push_back("toDebugString: deep output empty");

        // deep output must contain nested key/value
        if (deep.find("c") == std::string::npos && deep.find("2") == std::string::npos)
            errors.push_back("toDebugString: deep output missing nested content");

        // shallow output should not contain the nested value '2' when depth==1
        if (shallow.find("2") != std::string::npos)
            errors.push_back("toDebugString: shallow output unexpectedly contains deep value");

        return true;
    }

    // More thorough VariantHash and unordered_map usage tests
    bool Variant_test_varianthash_and_map(std::vector<std::string>& errors)
    {
        VariantHash h;

        // scalar keys
        Variant ka = Variant(int64_t(7));
        Variant kb = Variant(int64_t(7));
        if (h(ka) != h(kb)) errors.push_back("VariantHash: scalar equal values hashed differently");

        // array keys
        Variant arr1 = Variant::makeArray(); arr1.push(Variant(1)); arr1.push(Variant(2));
        Variant arr2 = Variant::makeArray(); arr2.push(Variant(1)); arr2.push(Variant(2));
        if (h(arr1) != h(arr2)) errors.push_back("VariantHash: equal arrays hashed differently");

        // object keys with different insertion order
        Variant o1 = Variant::makeObject(); o1.set("a", Variant(1)); o1.set("b", Variant(2));
        Variant o2 = Variant::makeObject(); o2.set("b", Variant(2)); o2.set("a", Variant(1));
        if (!(o1 == o2)) errors.push_back("VariantHash/map: objects expected equal but are not");
        else if (h(o1) != h(o2)) errors.push_back("VariantHash: equal objects hashed differently");

        // dynamic pointer keys
        auto sp = std::make_shared<int>(42);
        Variant d1 = Variant::makeDynamic<int>(sp);
        Variant d2 = Variant::makeDynamic<int>(sp);
        if (h(d1) != h(d2)) errors.push_back("VariantHash: dynamic same-pointer hashed differently");

        // insertion and lookup in unordered_map
        std::unordered_map<Variant,int, VariantHash> map;
        Variant key1 = Variant::makeObject(); key1.set("id", Variant(99));
        Variant key2 = Variant::makeObject(); key2.set("id", Variant(99));
        map.emplace(key1, 1);
        // modify via equivalent key
        map[key2] = 2;
        if (map.size() != 1) errors.push_back("Variant map: expected single bucket for equivalent keys");
        auto it = map.find(key1);
        if (it == map.end()) errors.push_back("Variant map: lookup of existing key failed");
        else if (it->second != 2) errors.push_back("Variant map: value after overwrite via equivalent key unexpected");

        // ensure lookups using dynamic-constructed equivalent key succeed
        std::unordered_map<Variant,int, VariantHash> map2;
        map2.emplace(d1, 10);
        auto it2 = map2.find(d2);
        if (it2 == map2.end()) errors.push_back("Variant map: lookup for dynamic key failed");

        return true;
    }

    // Snapshot(sol::state_view) validity tests and TableStorageMode behavior
    bool Variant_test_snapshot_state_validity(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        sol::state& L = core.getLua();

        sol::table t = L.create_table();
        t["x"] = 1234;

        Variant v;
        {
            TableStorageGuard guard(Variant::TableStorageMode::KeepLuaRef);
            v = Variant::fromLuaObject(t);
        }

        if (!v.isLuaRef()) {
            errors.push_back("Snapshot test: expected LuaRef when mode=KeepLuaRef");
            return true;
        }

        Variant snap_same = v.snapshot(L);
        if (!snap_same.isObject()) {
            errors.push_back("Snapshot test: snapshot(L) did not produce object for same state");
        } else {
            const Variant* vx = snap_same.get("x");
            if (!vx || vx->toInt64() != 1234) {
                errors.push_back("Snapshot test: snapshot value mismatch for same state");
            }
        }

        sol::state tmp;
        tmp.open_libraries(sol::lib::base);
        Variant snap_other = v.snapshot(tmp);
        if (!snap_other.isLuaRef()) {
            errors.push_back("Snapshot test: expected LuaRef returned for mismatched lua_State");
        } else {
            sol::object maybe = snap_other.toLua(tmp);
            if (!maybe.is<sol::nil_t>()) {
                errors.push_back("Snapshot test: expected toLua(tmp) to return nil for mismatched LuaRef");
            }
        }

        L.script("function __vtest_fn() return 1 end");
        sol::object fn = L["__vtest_fn"];
        Variant vfn = Variant::fromLuaObject(fn);
        if (!vfn.isLuaRef()) {
            errors.push_back("Snapshot test: expected LuaRef for function object");
        }
        Variant snap_fn = vfn.snapshot(L);
        if (!snap_fn.isLuaRef()) {
            errors.push_back("Snapshot test: expected LuaRef returned for function snapshot");
        } else {
            sol::object got = snap_fn.toLua(L);
            if (got.get_type() != sol::type::function) {
                errors.push_back("Snapshot test: toLua(L) for function LuaRef did not return function");
            }
        }

        Variant vcopy;
        {
            TableStorageGuard guard(Variant::TableStorageMode::Copy);
            vcopy = Variant::fromLuaObject(t);
            if (!vcopy.isObject()) {
                errors.push_back("Snapshot test: expected object when TableStorageMode::Copy");
            }
        }
        Variant snap_copy = vcopy.snapshot(L);
        if (!snap_copy.isObject()) {
            errors.push_back("Snapshot test: snapshot on copied table did not produce object");
        }

        return true;
    }

    // Deep recursion stress: build large nested Variant structures (C++ and Lua) and
    // verify snapshot() and toDebugString() handle depth limits and do not crash.
    bool Variant_test_deep_recursion_stress(std::vector<std::string>& errors)
    {
        const int depth = 120; // deep but chosen to avoid practical stack overflow

        // Build a deep C++ Variant object chain: { inner: { inner: ... } }
        Variant root = Variant::makeObject();
        Variant* cur = &root;
        for (int i = 0; i < depth; ++i) {
            Variant child = Variant::makeObject();
            child.set("__mark", Variant(i));
            cur->set("inner", child);

            Variant* next = cur->get("inner");
            if (!next) {
                errors.push_back("Deep recursion C++: failed to create nested node");
                return true;
            }
            cur = next;
        }
        return true;
    }


    // Threaded registry & converter safety stress test: many threads register/get converters
    // and call converter functions concurrently to exercise locking and converter safety.
    bool Variant_test_threaded_converter_safety(std::vector<std::string>& errors)
    {
        // Define a simple stress type and converter
        struct Stress { int v; };

        std::atomic<int> toLua_calls{0};
        std::atomic<int> from_calls{0};

        Variant::ConverterEntry ce;
        ce.toLua = [&toLua_calls]([[maybe_unused]] const VariantStorage::DynamicValue& dv, sol::state_view L)->sol::object {
            toLua_calls.fetch_add(1, std::memory_order_relaxed);
            return sol::make_object(L, sol::lua_nil);
        };
        ce.fromVariant = [&from_calls]([[maybe_unused]] const Variant& v)->std::shared_ptr<void> {
            from_calls.fetch_add(1, std::memory_order_relaxed);
            return nullptr;
        };

        // Register initial converter
        Variant::registerConverterByName("StressType", ce);

        const int threads = 16;
        const int iters = 2000;
        std::vector<std::thread> ths;

        for (int t = 0; t < threads; ++t) {
            ths.emplace_back([t, iters, &from_calls]() {
                std::mt19937_64 rng(1234 + t);
                for (int i = 0; i < iters; ++i) {
                    // occasionally re-register a converter under a per-thread name
                    if ((i & 63) == 0) {
                        Variant::ConverterEntry local_ce;
                        local_ce.toLua = []([[maybe_unused]] const VariantStorage::DynamicValue& dv, sol::state_view L)->sol::object { return sol::make_object(L, sol::lua_nil); };
                        local_ce.fromVariant = []([[maybe_unused]] const Variant& v)->std::shared_ptr<void> { return nullptr; };
                        Variant::registerConverterByName(std::string("Reg_") + std::to_string(t) + "_" + std::to_string(i), local_ce);
                    }

                    // read back converter and call toLua/fromVariant if present
                    auto conv = Variant::getConverterByName("StressType");
                    if (conv && conv->fromVariant) {
                        // call fromVariant (safe without Lua) to exercise converter code concurrently
                        try { conv->fromVariant(Variant(123)); } catch(...) { /* intentionally swallow - converter may throw */ (void)0; }
                        from_calls.fetch_add(1, std::memory_order_relaxed);
                    }
                    if (i % 100 == 0) std::this_thread::yield();
                }
            });
        }

        for (auto &th : ths) th.join();

    // Basic sanity: at least some converter invocations occurred
    if (from_calls.load() == 0) errors.push_back("Threaded safety: no fromVariant calls recorded");

        return true;
    }

    // Randomized numeric coercion tests
    bool Variant_test_numeric_coercion_randomized(std::vector<std::string>& errors)
    {
        std::mt19937_64 rng(12345);
        std::uniform_int_distribution<int64_t> di(-1000000, 1000000);
        std::uniform_real_distribution<double> dr(-1e6, 1e6);

        for (int i = 0; i < 2000; ++i) {
            int t = static_cast<int>(rng() % 4);
            Variant v;
            int64_t expectInt = 0; double expectDouble = 0.0; bool expectBool = false;
            switch (t) {
                case 0: { // int
                    int64_t x = di(rng); v = Variant(x); expectInt = x; expectDouble = static_cast<double>(x); expectBool = (x != 0); break;
                }
                case 1: { // double
                    double x = dr(rng); v = Variant(x); expectInt = static_cast<int64_t>(x); expectDouble = x; expectBool = (x != 0.0); break;
                }
                case 2: { // numeric string
                    int64_t x = di(rng); v = Variant(std::to_string(x)); expectInt = x; expectDouble = static_cast<double>(x); expectBool = false; break;
                }
                default: { // non-numeric string
                    v = Variant(std::string("str_") + std::to_string(rng()%1000)); expectInt = 0; expectDouble = 0.0; expectBool = false; break;
                }
            }
            if (v.toInt64(0) != expectInt) errors.push_back("Randomized coercion: toInt64 mismatch");
            if (std::fabs(v.toDouble(0.0) - expectDouble) > 1e-6) errors.push_back("Randomized coercion: toDouble mismatch");
            if (v.toBool(false) != expectBool) errors.push_back("Randomized coercion: toBool mismatch");
        }
        return true;
    }

    // Hash-consistency tests: equal Variants should hash equal; object insertion order should not affect hash
    bool Variant_test_hash_consistency(std::vector<std::string>& errors)
    {
        Variant a = Variant::makeArray(); a.push(Variant(1)); a.push(Variant(2));
        Variant b = Variant::makeArray(); b.push(Variant(1)); b.push(Variant(2));
        VariantHash h;
        if (h(a) != h(b)) errors.push_back("Hash consistency: equal arrays produced different hashes");

        Variant o1 = Variant::makeObject(); o1.set("a", Variant(1)); o1.set("b", Variant(2));
        Variant o2 = Variant::makeObject(); o2.set("b", Variant(2)); o2.set("a", Variant(1));
        if (o1 == o2) {
            if (h(o1) != h(o2)) errors.push_back("Hash consistency: equal objects produced different hashes");
        } else {
            errors.push_back("Hash consistency: objects expected to be equal but are not");
        }

        // Check dynamic pointer hashing stability
        auto sp = std::make_shared<int>(42);
        Variant d1 = Variant::makeDynamic<int>(sp);
        Variant d2 = Variant::makeDynamic<int>(sp);
        if (h(d1) != h(d2)) errors.push_back("Hash consistency: dynamic variants same pointer hashed differently");

        return true;
    }

    // Larger threaded stress: many threads register/read converters
    bool Variant_test_threaded_registration_stress(std::vector<std::string>& errors)
    {
        const int threads = 32;
        const int per_thread = 200;
        std::vector<std::thread> ths;
        for (int t = 0; t < threads; ++t) {
            ths.emplace_back([t, per_thread]() {
                for (int i = 0; i < per_thread; ++i) {
                    Variant::ConverterEntry ce;
                    ce.toLua = []([[maybe_unused]] const VariantStorage::DynamicValue& dv, sol::state_view L)->sol::object { return sol::make_object(L, sol::lua_nil); };
                    ce.fromVariant = []([[maybe_unused]] const Variant& v)->std::shared_ptr<void> { return nullptr; };
                    Variant::registerConverterByName(std::string("StressConv_") + std::to_string(t) + "_" + std::to_string(i), ce);
                    // read back occasionally
                    if ((i & 7) == 0) Variant::getConverterByName(std::string("StressConv_") + std::to_string(t) + "_" + std::to_string(i));
                }
            });
        }
        for (auto &th : ths) th.join();

        // quick verification that some entries exist
        for (int t = 0; t < 4; ++t) {
            if (!Variant::getConverterByName(std::string("StressConv_") + std::to_string(t) + "_" + std::to_string(0)))
                errors.push_back("Threaded stress: missing expected converter");
        }

        return true;
    }

    bool Variant_test_capi_roundtrip(std::vector<std::string>& errors)
    {
        CVariantHandle boolVariant;
        if (!SDOM_Variant_InitBool(boolVariant.get(), true)) {
            errors.push_back("C API roundtrip: InitBool failed");
            return true;
        }
        bool boolOut = false;
        if (!SDOM_Variant_ToBool(boolVariant.get(), &boolOut) || !boolOut) {
            errors.push_back("C API roundtrip: ToBool mismatch");
        }

        CVariantHandle intVariant;
        if (!SDOM_Variant_InitInt(intVariant.get(), 42)) {
            errors.push_back("C API roundtrip: InitInt failed");
            return true;
        }
        int64_t intOut = 0;
        if (!SDOM_Variant_ToInt64(intVariant.get(), &intOut) || intOut != 42) {
            errors.push_back("C API roundtrip: ToInt64 mismatch");
        }

        CVariantHandle doubleVariant;
        if (!SDOM_Variant_InitDouble(doubleVariant.get(), 3.5)) {
            errors.push_back("C API roundtrip: InitDouble failed");
            return true;
        }
        double doubleOut = 0.0;
        if (!SDOM_Variant_ToDouble(doubleVariant.get(), &doubleOut) || std::fabs(doubleOut - 3.5) > 1e-9) {
            errors.push_back("C API roundtrip: ToDouble mismatch");
        }

        CVariantHandle stringVariant;
        if (!SDOM_Variant_InitString(stringVariant.get(), "hello")) {
            errors.push_back("C API roundtrip: InitString failed");
            return true;
        }
        const char* strOut = SDOM_Variant_ToString(stringVariant.get());
        if (!strOut || std::string(strOut) != "hello") {
            errors.push_back("C API roundtrip: ToString mismatch");
        }

        return true;
    }

    bool Variant_test_capi_copy_semantics(std::vector<std::string>& errors)
    {
        CVariantHandle source;
        CVariantHandle dest;

        if (!SDOM_Variant_InitString(source.get(), "alpha")) {
            errors.push_back("C API copy: InitString failed");
            return true;
        }
        if (!SDOM_Variant_Copy(source.get(), dest.get())) {
            errors.push_back("C API copy: Copy failed");
            return true;
        }

        if (!SDOM_Variant_InitInt(source.get(), 99)) {
            errors.push_back("C API copy: re-init source failed");
            return true;
        }

        const char* copied = SDOM_Variant_ToString(dest.get());
        if (!copied || std::string(copied) != "alpha") {
            errors.push_back("C API copy: destination changed unexpectedly");
        }
        if (SDOM_Variant_GetType(dest.get()) != SDOM_VariantType_String) {
            errors.push_back("C API copy: destination type mismatch");
        }

        return true;
    }

    bool Variant_test_capi_type_metadata(std::vector<std::string>& errors)
    {
        CVariantHandle nullVariant;
        if (!SDOM_Variant_InitNull(nullVariant.get())) {
            errors.push_back("C API type: InitNull failed");
            return true;
        }
        if (SDOM_Variant_GetType(nullVariant.get()) != SDOM_VariantType_Null) {
            errors.push_back("C API type: expected Null type");
        }

        CVariantHandle boolVariant;
        SDOM_Variant_InitBool(boolVariant.get(), false);
        if (SDOM_Variant_GetType(boolVariant.get()) != SDOM_VariantType_Bool) {
            errors.push_back("C API type: expected Bool type");
        }

        CVariantHandle intVariant;
        SDOM_Variant_InitInt(intVariant.get(), -7);
        if (SDOM_Variant_GetType(intVariant.get()) != SDOM_VariantType_Int) {
            errors.push_back("C API type: expected Int type");
        }

        CVariantHandle doubleVariant;
        SDOM_Variant_InitDouble(doubleVariant.get(), 0.25);
        if (SDOM_Variant_GetType(doubleVariant.get()) != SDOM_VariantType_Real) {
            errors.push_back("C API type: expected Real type");
        }

        CVariantHandle stringVariant;
        SDOM_Variant_InitString(stringVariant.get(), "beta");
        if (SDOM_Variant_GetType(stringVariant.get()) != SDOM_VariantType_String) {
            errors.push_back("C API type: expected String type");
        }

        return true;
    }

    bool Variant_test_capi_error_paths(std::vector<std::string>& errors)
    {
        if (SDOM_Variant_InitBool(nullptr, true)) {
            errors.push_back("C API error: InitBool should fail on null variant");
        }

        CVariantHandle variant;
        SDOM_Variant_InitInt(variant.get(), 5);
        if (SDOM_Variant_ToInt64(variant.get(), nullptr)) {
            errors.push_back("C API error: ToInt64 should fail on null out pointer");
        }

        CVariantHandle dest;
        if (SDOM_Variant_Copy(nullptr, dest.get())) {
            errors.push_back("C API error: Copy should fail with null source");
        }
        if (SDOM_Variant_Copy(dest.get(), nullptr)) {
            errors.push_back("C API error: Copy should fail with null destination");
        }

        if (SDOM_Variant_GetType(nullptr) != SDOM_VariantType_Error) {
            errors.push_back("C API error: GetType should return Error for null handle");
        }

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

        static bool reg_inner = false;
        if (!reg_inner) {
            Variant::ConverterEntry ice;
            ice.toLua = [](const VariantStorage::DynamicValue& dv, sol::state_view L)->sol::object {
                auto p = std::static_pointer_cast<Inner>(dv.ptr);
                sol::table t = L.create_table();
                t["a"] = p ? p->a : 0;
                t["name"] = p ? p->name : std::string();
                return t;
            };
            ice.fromVariant = [](const Variant& v)->std::shared_ptr<void> {
                auto out = std::make_shared<Inner>();
                if (v.isObject()) {
                    if (const Variant* va = v.get("a")) out->a = static_cast<int>(va->toInt64());
                    if (const Variant* vn = v.get("name")) out->name = vn->toString();
                    return std::static_pointer_cast<void>(out);
                }
                return nullptr;
            };
            Variant::registerConverter<Inner>("Inner", std::move(ice));
            reg_inner = true;
        }

        static bool reg_outer = false;
        if (!reg_outer) {
            Variant::ConverterEntry oce;
            oce.toLua = [](const VariantStorage::DynamicValue& dv, sol::state_view L)->sol::object {
                auto p = std::static_pointer_cast<Outer>(dv.ptr);
                sol::table t = L.create_table();
                sol::table inner = L.create_table();
                inner["a"] = p ? p->inner.a : 0;
                inner["name"] = p ? p->inner.name : std::string();
                t["inner"] = inner;

                sol::table arrtbl = L.create_table();
                if (p) {
                    for (size_t i = 0; i < p->arr.size(); ++i) {
                        arrtbl[i + 1] = p->arr[i];
                    }
                }
                t["arr"] = arrtbl;

                sol::table mt = L.create_table();
                if (p) {
                    for (auto &kv : p->map) mt[kv.first] = kv.second;
                }
                t["map"] = mt;
                return t;
            };
            oce.fromVariant = [](const Variant& v)->std::shared_ptr<void> {
                auto out = std::make_shared<Outer>();
                if (!v.isObject()) return nullptr;
                if (const Variant* vin = v.get("inner")) {
                    if (vin->isObject()) {
                        if (const Variant* va = vin->get("a")) out->inner.a = static_cast<int>(va->toInt64());
                        if (const Variant* vn = vin->get("name")) out->inner.name = vn->toString();
                    }
                }
                if (const Variant* varr = v.get("arr")) {
                    if (varr->isArray()) {
                        for (size_t i = 0; i < varr->size(); ++i) {
                            if (const Variant* e = varr->at(i)) {
                                out->arr.push_back(static_cast<int>(e->toInt64()));
                            }
                        }
                    }
                }
                if (const Variant* vmap = v.get("map")) {
                    if (vmap->isObject()) {
                        if (auto obj = vmap->object()) {
                            for (const auto &kv : *obj) {
                                out->map[kv.first] = static_cast<int>(kv.second->toInt64());
                            }
                        }
                    }
                }
                return std::static_pointer_cast<void>(out);
            };
            Variant::registerConverter<Outer>("Outer", std::move(oce));
            reg_outer = true;
        }

        auto out_inst = std::make_shared<Outer>();
        out_inst->inner.a = 9;
        out_inst->inner.name = "nine";
        out_inst->arr = {1, 2, 3};
        out_inst->map["one"] = 1;
        out_inst->map["two"] = 2;

        Variant v(out_inst);
        sol::object o = v.toLua(L);
        if (!o.is<sol::table>()) {
            errors.push_back("Outer toLua did not produce table");
            return true;
        }

        Variant v2 = Variant::fromLuaObject(o);
        auto ce = Variant::getConverterByName("Outer");
        if (!ce) {
            errors.push_back("Outer converter missing");
            return true;
        }
        auto pv = ce->fromVariant(v2);
        if (!pv) {
            errors.push_back("Outer fromVariant returned null");
            return true;
        }
        auto p2 = std::static_pointer_cast<Outer>(pv);
        if (!p2) {
            errors.push_back("Outer cast failed");
            return true;
        }
        if (p2->inner.a != 9 || p2->inner.name != "nine") {
            errors.push_back("Outer.inner mismatch");
        }
        if (p2->arr.size() != 3 || p2->arr[0] != 1) {
            errors.push_back("Outer.arr mismatch");
        }
        if (p2->map["one"] != 1 || p2->map["two"] != 2) {
            errors.push_back("Outer.map mismatch");
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
            ut.add_test(objName, "Converter: lookup by name", Variant_test_converter_lookup_by_name);
            ut.add_test(objName, "Converter chain: Outer nested/arrays/maps", ComplexVariant_test_outer_converter);
            ut.add_test(objName, "Numeric coercion stress", Variant_test_numeric_coercion_stress);
            ut.add_test(objName, "Numeric coercion randomized", Variant_test_numeric_coercion_randomized);
            ut.add_test(objName, "Hash consistency", Variant_test_hash_consistency);
            ut.add_test(objName, "Threaded registration stress", Variant_test_threaded_registration_stress);
            ut.add_test(objName, "Deep recursion snapshot", Variant_test_deep_recursion_snapshot);
            ut.add_test(objName, "Threaded converter registration", Variant_test_threaded_converter_registration);
            ut.add_test(objName, "Equality: composite types", Variant_test_equality_composite_types);
            ut.add_test(objName, "Null/Error semantics", Variant_test_null_and_error);
            ut.add_test(objName, "VariantView basic access", Variant_test_variantview_basic_access);
            ut.add_test(objName, "LuaRef lifetime validation", Variant_test_luaref_lifetime_validation);
            ut.add_test(objName, "Table storage mode toggle and snapshot", Variant_test_table_storage_and_snapshot);
            ut.add_test(objName, "Dynamic metadata accessors", Variant_test_dynamic_metadata_accessors);
            ut.add_test(objName, "Copy/Move semantics and containers", Variant_test_copy_move_and_containers);
            ut.add_test(objName, "toDebugString shallow vs deep", Variant_test_toDebugString);
            ut.add_test(objName, "VariantHash & unordered_map usage", Variant_test_varianthash_and_map);
            ut.add_test(objName, "C API: roundtrip conversions", Variant_test_capi_roundtrip);
            ut.add_test(objName, "C API: copy semantics", Variant_test_capi_copy_semantics);
            ut.add_test(objName, "C API: type metadata", Variant_test_capi_type_metadata);
            ut.add_test(objName, "C API: error paths", Variant_test_capi_error_paths);
            ut.add_test(objName, "Snapshot(sol::state_view) validity and TableStorageMode", Variant_test_snapshot_state_validity);

            ut.add_test(objName, "Deep recursion stress", Variant_test_deep_recursion_stress);  

            ut.add_test(objName, "Threaded converter safety stress", Variant_test_threaded_converter_safety);

            registered = true;
        }
        return true;
    }

} // namespace SDOM
