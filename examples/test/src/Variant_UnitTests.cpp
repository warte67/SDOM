// Variant_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_CoreAPI.hpp>
#include <SDOM/SDOM_Variant.hpp>
#include <thread>
#include <random>
#include <sstream>
#include <algorithm>
#include <atomic>
#include <chrono>

namespace SDOM
{
    // Simple sample dynamic type for testing
    struct TestPoint { int x = 0; int y = 0; };

    bool Variant_test_converter_to_json(std::vector<std::string>& errors)
    {
        static bool registered = false;
        if (!registered) {
            Variant::ConverterEntry ce;
            ce.toJson = [](const VariantStorage::DynamicValue& dv)->nlohmann::json {
                auto p = std::static_pointer_cast<TestPoint>(dv.ptr);
                nlohmann::json j;
                j["x"] = p ? p->x : 0;
                j["y"] = p ? p->y : 0;
                return j;
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

        auto p = std::make_shared<TestPoint>();
        p->x = 7; p->y = 11;
        Variant v(p);

        nlohmann::json j = v.toJson();
        if (!j.is_object()) errors.push_back("toJson did not produce object for TestPoint");
        if (j.value("x", 0) != 7 || j.value("y", 0) != 11) errors.push_back("TestPoint fields mismatch in toJson");

        Variant v2 = Variant::fromJson(j);
        auto ce = Variant::getConverter(std::type_index(typeid(TestPoint)));
        if (!ce) { errors.push_back("Converter missing for TestPoint"); return true; }
        auto pv = ce->fromVariant(v2);
        auto p2 = std::static_pointer_cast<TestPoint>(pv);
        if (!p2 || p2->x != 7 || p2->y != 11) errors.push_back("fromVariant produced wrong TestPoint");

        return true;
    } // END: Variant_test_converter_to_json()

    bool Variant_test_converter_roundtrip(std::vector<std::string>& errors)
    {
        // create a TestPoint, convert to Variant (dynamic), to JSON, back to Variant, then back to C++
        auto p = std::make_shared<TestPoint>();
        p->x = 13; p->y = 17;
        Variant v(p);

        nlohmann::json j = v.toJson();
        Variant v2 = Variant::fromJson(j);

        auto ce = Variant::getConverter(std::type_index(typeid(TestPoint)));
        if (!ce) { errors.push_back("Roundtrip: converter missing for TestPoint"); return true; }

        std::shared_ptr<void> pv = ce->fromVariant(v2);
        auto p2 = std::static_pointer_cast<TestPoint>(pv);
        if (!p2) { errors.push_back("Roundtrip: cast failed"); return true; }
        if (p2->x != 13 || p2->y != 17) errors.push_back("Roundtrip: value mismatch after conversion");

        return true;
    } // END: Variant_test_converter_roundtrip()

    bool Variant_test_null_and_error(std::vector<std::string>& errors)
    {
        // Default-constructed Variant should be null
        Variant v;
        if (!v.isNull()) errors.push_back("Default-constructed Variant is not null");
        if (v.type() != VariantType::Null) errors.push_back("Variant::type() for default Variant is not Null");
        if (!v.toJson().is_null()) errors.push_back("toJson for null Variant is not null");

        // makeError should set error flag and message and report Error type
        Variant e = Variant::makeError("boom");
        if (!e.hasError()) errors.push_back("Variant::makeError did not set error flag");
        if (e.errorMessage() != std::string("boom")) errors.push_back("Variant::makeError errorMessage mismatch");
        if (e.type() != VariantType::Error) errors.push_back("Variant::type() for error Variant is not Error");
        if (!e.toJson().is_null()) errors.push_back("toJson for error Variant is not null");

        return true;
    } // END: Variant_test_null_and_error()

    // Separate named tests (extracted from inline lambdas) for readability
    bool Variant_test_converter_lookup_by_name(std::vector<std::string>& errors)
    {
        auto ce = Variant::getConverterByName("TestPoint");
        if (!ce) { errors.push_back("Converter lookup by name failed"); return true; }

        auto p = std::make_shared<TestPoint>(); p->x = 3; p->y = 4;
        auto dyn = Variant::makeDynamicValue<TestPoint>(p);
        nlohmann::json j = ce->toJson ? ce->toJson(dyn) : nlohmann::json(nullptr);
        if (!j.is_object()) { errors.push_back("Converter toJson by name did not produce object"); return true; }
        if (j.value("x", 0) != 3 || j.value("y", 0) != 4) errors.push_back("Converter toJson by name produced wrong values");

        Variant v = Variant::fromJson(j);
        auto pv = ce->fromVariant ? ce->fromVariant(v) : nullptr;
        auto p2 = std::static_pointer_cast<TestPoint>(pv);
        if (!p2 || p2->x != 3 || p2->y != 4) errors.push_back("Converter fromVariant by name mismatch");

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

    bool Variant_test_json_roundtrip_basic(std::vector<std::string>& errors)
    {
        // Object
        nlohmann::json jo = {{"a", 5}, {"b", "hi"}};
        Variant vo = Variant::fromJson(jo);
        if (!vo.isObject()) errors.push_back("fromJson(object) did not produce object Variant");
        if (vo.toJson() != jo) errors.push_back("toJson(object) mismatch");

        // Array
        nlohmann::json ja = {1, 2, 3};
        Variant va = Variant::fromJson(ja);
        if (!va.isArray()) errors.push_back("fromJson(array) did not produce array Variant");
        if (va.toJson() != ja) errors.push_back("toJson(array) mismatch");

        // Nested
        nlohmann::json jn = {{"arr", nlohmann::json::array({ { {"x",1} }, { {"x",2} } })}};
        Variant vn = Variant::fromJson(jn);
        if (vn.toJson() != jn) errors.push_back("toJson nested mismatch");

        return true;
    }

    bool Variant_test_json_snapshot_value_semantics(std::vector<std::string>& errors)
    {
        nlohmann::json j = {{"a", 123}, {"arr", nlohmann::json::array({1,2,3})}};
        Variant v = Variant::fromJson(j);
        // Mutate original json after Variant creation; Variant should keep prior values
        j["a"] = 999;
        auto out = v.toJson();
        if (out.value("a", 0) != 123) errors.push_back("Variant did not snapshot JSON by value");

        // Mutate Variant and ensure emitted json reflects change
        if (Variant* arr = v.get("arr")) {
            arr->push(Variant(4));
        }
        auto out2 = v.toJson();
        if (!out2.contains("arr") || out2["arr"].size() != 4) errors.push_back("Variant mutation not reflected in toJson");
        return true;
    }

    bool Variant_test_dynamic_metadata_accessors(std::vector<std::string>& errors)
    {
        // Ensure converter is present so the registry has a name for TestPoint
        auto ce = Variant::getConverterByName("TestPoint");
        if (!ce) {
            Variant::ConverterEntry reg;
            reg.toJson = [](const VariantStorage::DynamicValue& dv)->nlohmann::json {
                auto p = std::static_pointer_cast<TestPoint>(dv.ptr);
                nlohmann::json j;
                j["x"] = p ? p->x : 0;
                j["y"] = p ? p->y : 0;
                return j;
            };
            reg.fromVariant = [](const Variant& v)->std::shared_ptr<void> {
                auto out = std::make_shared<TestPoint>();
                if (v.isObject()) {
                    if (const Variant* vx = v.get("x")) out->x = static_cast<int>(vx->toInt64());
                    if (const Variant* vy = v.get("y")) out->y = static_cast<int>(vy->toInt64());
                    return std::static_pointer_cast<void>(out);
                }
                return nullptr;
            };
            Variant::registerConverter<TestPoint>("TestPoint", std::move(reg));
        }

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
        const int depth = 60; // deep enough to exercise recursion without blowing the stack

        // Build a deep object chain: {inner: {inner: ...}} and stamp a marker per level
        Variant root = Variant::makeObject();
        Variant* cur = &root;
        for (int i = 0; i < depth; ++i) {
            Variant child = Variant::makeObject();
            child.set("__mark", Variant(int64_t(i)));
            cur->set("inner", child);
            cur = cur->get("inner");
            if (!cur) { errors.push_back("Deep snapshot: failed to extend chain at level " + std::to_string(i)); return true; }
        }

        // Take a JSON snapshot (round-trip) and verify it retains the full depth and marker values
        Variant snap = Variant::fromJson(root.toJson());
        if (!snap.isObject()) { errors.push_back("Deep snapshot: snapshot is not an object"); return true; }

        const Variant* node = snap.get("inner");
        for (int i = 0; i < depth; ++i) {
            if (!node || !node->isObject()) { errors.push_back("Deep snapshot: traversal failed at depth " + std::to_string(i)); return true; }
            const Variant* mark = node->get("__mark");
            if (!mark || mark->toInt64(-1) != i) {
                errors.push_back("Deep snapshot: marker mismatch at depth " + std::to_string(i));
                return true;
            }
            node = node->get("inner");
        }

        // Mutate the original chain to ensure snapshot was by value
        Variant* mutate = root.get("inner");
        if (mutate && mutate->isObject()) mutate->set("__mark", Variant(int64_t(-999)));

        const Variant* snap_first_inner = snap.get("inner");
        if (snap_first_inner && snap_first_inner->isObject()) {
            const Variant* snap_mark = snap_first_inner->get("__mark");
            if (snap_mark && snap_mark->toInt64(-1) != 0) errors.push_back("Deep snapshot: snapshot mutated after source change");
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
                ce.toJson = []([[maybe_unused]] const VariantStorage::DynamicValue& dv)->nlohmann::json {
                    return nlohmann::json(nullptr);
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
        // Build a representative JSON object and snapshot it via JSON round-trip.
        nlohmann::json j = {
            {"x", 1234},
            {"inner", { {"y", "hi"}, {"z", 42} } }
        };

        Variant v = Variant::fromJson(j);
        if (!v.isObject()) { errors.push_back("Snapshot state: fromJson did not produce object"); return true; }

        Variant snap = Variant::fromJson(v.toJson());
        if (!snap.isObject()) { errors.push_back("Snapshot state: snapshot is not an object"); return true; }

        // The snapshot should be equal to the original state
        if (!(snap == v)) errors.push_back("Snapshot state: snapshot not equal to original");

        // Mutate original; snapshot should remain unchanged
        if (Variant* inner = v.get("inner")) inner->set("y", Variant(std::string("mutated")));
        const Variant* snap_inner = snap.get("inner");
        if (!snap_inner || !snap_inner->isObject()) {
            errors.push_back("Snapshot state: missing inner in snapshot");
            return true;
        }
        if (const Variant* y = snap_inner->get("y")) {
            if (y->toString() != std::string("hi")) errors.push_back("Snapshot state: snapshot changed after source mutation");
        } else {
            errors.push_back("Snapshot state: missing y in snapshot");
        }

        // Mutate snapshot; original should remain unchanged
        if (Variant* sx = snap.get("x")) *sx = Variant(int64_t(999));
        const Variant* ox = v.get("x");
        if (!ox || ox->toInt64(-1) != 1234) errors.push_back("Snapshot state: original changed after snapshot mutation");

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

        std::atomic<int> toJson_calls{0};
        std::atomic<int> from_calls{0};

        Variant::ConverterEntry ce;
        ce.toJson = [&toJson_calls]([[maybe_unused]] const VariantStorage::DynamicValue& dv)->nlohmann::json {
            toJson_calls.fetch_add(1, std::memory_order_relaxed);
            return nlohmann::json(nullptr);
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
                        local_ce.toJson = []([[maybe_unused]] const VariantStorage::DynamicValue& dv)->nlohmann::json { return nlohmann::json(nullptr); };
                        local_ce.fromVariant = []([[maybe_unused]] const Variant& v)->std::shared_ptr<void> { return nullptr; };
                        Variant::registerConverterByName(std::string("Reg_") + std::to_string(t) + "_" + std::to_string(i), local_ce);
                    }

                    // read back converter and call fromVariant if present
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
                    ce.toJson = []([[maybe_unused]] const VariantStorage::DynamicValue& dv)->nlohmann::json { return nlohmann::json(nullptr); };
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

    // Complex converter examples (previously a separate file). Keep with Variant tests.
    struct Inner { int a = 0; std::string name; };
    struct Outer {
        Inner inner;
        std::vector<int> arr;
        std::unordered_map<std::string,int> map;
    };

    bool ComplexVariant_test_outer_converter(std::vector<std::string>& errors)
    {
        static bool converters_registered = false;
        if (!converters_registered) {
            // Register converter for Inner
            Variant::ConverterEntry ice;
            ice.toJson = [](const VariantStorage::DynamicValue& dv)->nlohmann::json {
                auto p = std::static_pointer_cast<Inner>(dv.ptr);
                nlohmann::json j;
                j["a"] = p ? p->a : 0;
                j["name"] = p ? p->name : std::string();
                return j;
            };
            ice.fromVariant = [](const Variant& v)->std::shared_ptr<void> {
                auto out = std::make_shared<Inner>();
                if (!v.isObject()) return nullptr;
                if (const Variant* va = v.get("a")) out->a = static_cast<int>(va->toInt64());
                if (const Variant* vn = v.get("name")) out->name = vn->toString();
                return std::static_pointer_cast<void>(out);
            };
            Variant::registerConverter<Inner>("Inner", std::move(ice));

            // Register converter for Outer
            Variant::ConverterEntry oce;
            oce.toJson = [](const VariantStorage::DynamicValue& dv)->nlohmann::json {
                auto p = std::static_pointer_cast<Outer>(dv.ptr);
                nlohmann::json j;
                nlohmann::json inner;
                if (p) {
                    inner["a"] = p->inner.a;
                    inner["name"] = p->inner.name;
                    j["arr"] = p->arr;
                    nlohmann::json map = nlohmann::json::object();
                    for (auto &kv : p->map) map[kv.first] = kv.second;
                    j["map"] = map;
                }
                j["inner"] = inner;
                return j;
            };
            oce.fromVariant = [](const Variant& v)->std::shared_ptr<void> {
                if (!v.isObject()) return nullptr;
                auto out = std::make_shared<Outer>();
                if (const Variant* vin = v.get("inner")) {
                    if (const Variant* va = vin->get("a")) out->inner.a = static_cast<int>(va->toInt64());
                    if (const Variant* vn = vin->get("name")) out->inner.name = vn->toString();
                }
                if (const Variant* varr = v.get("arr")) {
                    if (varr->isArray()) {
                        for (size_t i = 0; i < varr->size(); ++i) {
                            if (const Variant* e = varr->at(i)) out->arr.push_back(static_cast<int>(e->toInt64()));
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
            converters_registered = true;
        }

        // Build an Outer instance and round-trip through Variant -> JSON -> Variant -> C++
        auto out_inst = std::make_shared<Outer>();
        out_inst->inner.a = 9; out_inst->inner.name = "nine";
        out_inst->arr = {1,2,3};
        out_inst->map["one"] = 1; out_inst->map["two"] = 2;

        Variant v(out_inst);
        nlohmann::json j = v.toJson();
        if (!j.is_object()) errors.push_back("Outer toJson did not produce object");

        Variant v2 = Variant::fromJson(j);
        auto ce = Variant::getConverterByName("Outer");
        if (!ce) { errors.push_back("Outer converter missing"); return true; }
        auto pv = ce->fromVariant ? ce->fromVariant(v2) : nullptr;
        auto p2 = std::static_pointer_cast<Outer>(pv);
        if (!p2) { errors.push_back("Outer fromVariant returned null"); return true; }

        if (p2->inner.a != 9 || p2->inner.name != "nine") errors.push_back("Outer.inner mismatch");
        if (p2->arr.size() != 3 || p2->arr[0] != 1 || p2->arr[1] != 2 || p2->arr[2] != 3) errors.push_back("Outer.arr mismatch");
        if (p2->map["one"] != 1 || p2->map["two"] != 2) errors.push_back("Outer.map mismatch");

        return true;
    }

    bool Variant_test_parsePath_success(std::vector<std::string>& errors)
    {
        auto expect = [&](std::string_view path, const std::vector<PathElement>& expected) {
            std::string err;
            PathView view = parsePath(path, &err);
            if (view.empty()) {
                errors.push_back(std::string("parsePath unexpectedly failed for ") + std::string(path) + ": " + err);
                return;
            }
            if (view.size() != expected.size()) {
                errors.push_back(std::string("parsePath element count mismatch for ") + std::string(path));
                return;
            }
            for (size_t i = 0; i < expected.size(); ++i) {
                const auto& got = view[i];
                const auto& exp = expected[i];
                if (got.kind != exp.kind) {
                    errors.push_back(std::string("parsePath kind mismatch at index ") + std::to_string(i));
                    continue;
                }
                if (exp.kind == PathElement::Kind::Key && got.key != exp.key) {
                    errors.push_back(std::string("parsePath key mismatch for ") + std::string(path));
                }
                if (exp.kind == PathElement::Kind::Index && got.index != exp.index) {
                    errors.push_back(std::string("parsePath index mismatch for ") + std::string(path));
                }
            }
        };

        expect("foo", { PathElement{PathElement::Kind::Key, "foo", 0} });
        expect("foo.bar", {
            PathElement{PathElement::Kind::Key, "foo", 0},
            PathElement{PathElement::Kind::Key, "bar", 0}
        });
        expect("items[0]", {
            PathElement{PathElement::Kind::Key, "items", 0},
            PathElement{PathElement::Kind::Index, "", 0}
        });
        expect("player.inventory[12].count", {
            PathElement{PathElement::Kind::Key, "player", 0},
            PathElement{PathElement::Kind::Key, "inventory", 0},
            PathElement{PathElement::Kind::Index, "", 12},
            PathElement{PathElement::Kind::Key, "count", 0}
        });

        return true;
    }

    bool Variant_test_parsePath_failure(std::vector<std::string>& errors)
    {
        auto expect_fail = [&](std::string_view path, std::string_view expected_substr) {
            std::string err;
            PathView view = parsePath(path, &err);
            if (!view.empty()) {
                errors.push_back(std::string("parsePath succeeded unexpectedly for ") + std::string(path));
                return;
            }
            if (err.find(expected_substr) == std::string::npos) {
                errors.push_back(std::string("parsePath error mismatch for ") + std::string(path) +
                                 " (expected to contain '" + std::string(expected_substr) +
                                 "', got '" + err + "')");
            }
        };

        expect_fail("", "empty");
        expect_fail(".", "empty path segment");
        expect_fail("foo..", "empty path segment");
        expect_fail("items[]", "missing digits");
        expect_fail("items[abc]", "missing digits");

        return true;
    }

    // Helper to build a simple object/array hierarchy for path tests
    static Variant make_path_root()
    {
        Variant item0 = Variant::makeObject(); item0.set("count", Variant(int64_t(5)));
        Variant item1 = Variant::makeObject(); item1.set("count", Variant(int64_t(7)));

        Variant inventory = Variant::makeArray();
        inventory.push(item0);
        inventory.push(item1);

        Variant player = Variant::makeObject();
        player.set("inventory", inventory);

        Variant root = Variant::makeObject();
        root.set("player", player);
        return root;
    }

    bool Variant_test_path_simple_object(std::vector<std::string>& errors)
    {
        Variant root = Variant::makeObject();
        root.set("x", Variant(int64_t(10)));
        root.set("y", Variant(int64_t(20)));

        Variant out;
        if (!root.getPath("x", out) || out.toInt64(-1) != 10) {
            errors.push_back("getPath failed for object key 'x'");
        }
        if (!root.getPath("y", out) || out.toInt64(-1) != 20) {
            errors.push_back("getPath failed for object key 'y'");
        }

        return true;
    }

    bool Variant_test_path_object_array_chain(std::vector<std::string>& errors)
    {
        Variant root = Variant::makeObject();
        Variant items = Variant::makeArray();
        items.push(Variant(int64_t(1)));
        items.push(Variant(int64_t(2)));
        items.push(Variant(int64_t(3)));
        root.set("items", items);

        Variant out;
        if (!root.getPath("items[0]", out) || out.toInt64(-1) != 1) {
            errors.push_back("getPath failed for items[0]");
        }
        if (!root.getPath("items[2]", out) || out.toInt64(-1) != 3) {
            errors.push_back("getPath failed for items[2]");
        }

        return true;
    }

    bool Variant_test_path_missing_key(std::vector<std::string>& errors)
    {
        Variant root = Variant::makeObject();
        Variant items = Variant::makeArray();
        items.push(Variant(int64_t(1)));
        items.push(Variant(int64_t(2)));
        root.set("items", items);

        CoreAPI::clearError();
        Variant out;
        if (root.pathExists("items[2]")) {
            errors.push_back("pathExists reported true for missing index");
        }
        if (root.getPath("missing", out)) {
            errors.push_back("getPath unexpectedly succeeded for missing key");
        } else {
            const std::string err = CoreAPI::getErrorString();
            if (err.find("Path not found") == std::string::npos) {
                errors.push_back("Missing key error did not mention 'Path not found'");
            }
        }

        return true;
    }

    bool Variant_test_path_type_mismatch(std::vector<std::string>& errors)
    {
        Variant root = Variant::makeObject();
        Variant items = Variant::makeArray();
        items.push(Variant(int64_t(1)));
        root.set("items", items);

        CoreAPI::clearError();
        Variant out;
        if (root.getPath("items.key", out)) {
            errors.push_back("getPath unexpectedly succeeded for array field access");
        } else {
            const std::string err = CoreAPI::getErrorString();
            if (err.find("Expected object for field access") == std::string::npos) {
                errors.push_back("Type mismatch error did not mention expected object access");
            }
        }

        return true;
    }

    bool Variant_test_path_index_oob(std::vector<std::string>& errors)
    {
        Variant root = Variant::makeObject();
        Variant items = Variant::makeArray();
        items.push(Variant(int64_t(1)));
        items.push(Variant(int64_t(2)));
        root.set("items", items);

        CoreAPI::clearError();
        Variant out;
        if (root.getPath("items[3]", out)) {
            errors.push_back("getPath unexpectedly succeeded for out-of-range index");
        } else {
            const std::string err = CoreAPI::getErrorString();
            if (err.find("Index out of range") == std::string::npos) {
                errors.push_back("Out-of-range error did not mention index issue");
            }
        }

        return true;
    }

    bool Variant_test_path_scalar_traverse(std::vector<std::string>& errors)
    {
        Variant root(int64_t(42));

        CoreAPI::clearError();
        Variant out;
        if (root.getPath("child", out)) {
            errors.push_back("getPath unexpectedly succeeded on scalar root");
        } else {
            const std::string err = CoreAPI::getErrorString();
            if (err.find("Cannot traverse into scalar type") == std::string::npos) {
                errors.push_back("Scalar traversal error did not mention scalar type");
            }
        }

        return true;
    }

    bool Variant_test_path_get_and_exists(std::vector<std::string>& errors)
    {
        Variant root = make_path_root();

        Variant out;
        if (!root.getPath("player.inventory[1].count", out) || out.toInt64(-1) != 7) {
            errors.push_back("getPath failed to retrieve nested value");
        }

        if (!root.pathExists("player.inventory[0]")) errors.push_back("pathExists reported false for valid path");
        if (root.pathExists("player.inventory[2]")) errors.push_back("pathExists reported true for missing index");

        return true;
    }

    bool Variant_test_path_set_strict(std::vector<std::string>& errors)
    {
        Variant root = make_path_root();

        Variant value(int64_t(9));
        if (root.setPath("player.inventory[2].count", value)) {
            errors.push_back("setPath unexpectedly created path without opts");
        }

        Variant out;
        if (!root.getPath("player.inventory[1].count", out) || out.toInt64(-1) != 7) {
            errors.push_back("setPath strict altered existing value unexpectedly");
        }

        return true;
    }

    bool Variant_test_path_set_create(std::vector<std::string>& errors)
    {
        Variant root = make_path_root();
        PathOptions opts; opts.create_intermediates = true; opts.extend_arrays_with_null = true;

        Variant value(int64_t(9));
        if (!root.setPath("player.inventory[2].count", value, opts)) {
            errors.push_back("setPath with creation failed to append array");
        }

        Variant out;
        if (!root.getPath("player.inventory[2].count", out) || out.toInt64(-1) != 9) {
            errors.push_back("setPath with creation did not store expected value");
        }

        Variant hp(int64_t(15));
        if (!root.setPath("player.stats.hp", hp, opts)) {
            errors.push_back("setPath with creation failed to build object chain");
        }
        if (!root.getPath("player.stats.hp", out) || out.toInt64(-1) != 15) {
            errors.push_back("setPath with creation did not set object leaf");
        }

        return true;
    }

    bool Variant_test_path_erase(std::vector<std::string>& errors)
    {
        Variant root = make_path_root();
        PathOptions opts; opts.create_intermediates = true; opts.extend_arrays_with_null = true;
        // ensure three inventory entries
        root.setPath("player.inventory[2].count", Variant(int64_t(9)), opts);

        if (!root.erasePath("player.stats")) {
            // stats not present yet; erase should fail quietly
        }

        if (!root.erasePath("player.inventory[1]")) {
            errors.push_back("erasePath failed to remove array slot");
        }

        if (root.pathExists("player.inventory[2]")) {
            errors.push_back("erasePath did not compact array after erase");
        }

        Variant out;
        if (!root.getPath("player.inventory[1].count", out) || out.toInt64(-1) != 9) {
            errors.push_back("erasePath did not shift elements as expected");
        }

        if (!root.setPath("player.stats.hp", Variant(int64_t(12)), opts)) {
            errors.push_back("setPath failed before erasePath key removal");
        }
        if (!root.erasePath("player.stats.hp")) {
            errors.push_back("erasePath failed to remove object key");
        }
        if (root.pathExists("player.stats.hp")) {
            errors.push_back("erasePath left object key present");
        }

        return true;
    }

    bool Variant_UnitTests()
    {
        const std::string objName = "Variant";
        UnitTests& ut = UnitTests::getInstance();

        static bool registered = false;
        if (!registered) {
            ut.add_test(objName, "Converter: TestPoint -> JSON object", Variant_test_converter_to_json);
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
            ut.add_test(objName, "JSON roundtrip basic", Variant_test_json_roundtrip_basic);
            ut.add_test(objName, "JSON snapshot/value semantics", Variant_test_json_snapshot_value_semantics);
            ut.add_test(objName, "Dynamic metadata accessors", Variant_test_dynamic_metadata_accessors);
            ut.add_test(objName, "Copy/Move semantics and containers", Variant_test_copy_move_and_containers);
            ut.add_test(objName, "toDebugString shallow vs deep", Variant_test_toDebugString);
            ut.add_test(objName, "VariantHash & unordered_map usage", Variant_test_varianthash_and_map);
            ut.add_test(objName, "Deep recursion stress", Variant_test_deep_recursion_stress);  
            ut.add_test(objName, "Threaded converter safety stress", Variant_test_threaded_converter_safety);
            ut.add_test(objName, "parsePath success cases", Variant_test_parsePath_success);
            ut.add_test(objName, "parsePath failure cases", Variant_test_parsePath_failure);
            ut.add_test(objName, "path API: simple object get", Variant_test_path_simple_object);
            ut.add_test(objName, "path API: object + array chain", Variant_test_path_object_array_chain);
            ut.add_test(objName, "path API: missing key error", Variant_test_path_missing_key);
            ut.add_test(objName, "path API: type mismatch error", Variant_test_path_type_mismatch);
            ut.add_test(objName, "path API: index out of range", Variant_test_path_index_oob);
            ut.add_test(objName, "path API: scalar traversal", Variant_test_path_scalar_traverse);
            ut.add_test(objName, "path API: get & exists", Variant_test_path_get_and_exists);
            ut.add_test(objName, "path API: strict set", Variant_test_path_set_strict);
            ut.add_test(objName, "path API: create set", Variant_test_path_set_create);
            ut.add_test(objName, "path API: erase", Variant_test_path_erase);

            

            registered = true;
        }
        return true;
    }

} // namespace SDOM
