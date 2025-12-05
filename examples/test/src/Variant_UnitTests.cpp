// Variant_UnitTests.cpp (Lua-free, JSON/native focused)
// Variant_UnitTests.cpp (Lua-free, JSON/native focused)
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Variant.hpp>
#include <SDOM/CAPI/SDOM_CAPI_Variant.h>
#include <thread>
#include <random>
#include <cmath>
#include <unordered_map>
#include <json.hpp>

namespace SDOM
{
    namespace {

    class CVariantHandle {
    public:
        CVariantHandle() { SDOM_Variant_InitNull(&value_); }
        ~CVariantHandle() { SDOM_Variant_Destroy(&value_); }
        SDOM_Variant* get() { return &value_; }
        const SDOM_Variant* get() const { return &value_; }
    private:
        SDOM_Variant value_{};
    };

    // Helper: Variant <-> JSON (nlohmann) conversion for tests
    using json = nlohmann::json;

    Variant variantFromJson(const json& j)
    {
        if (j.is_null()) return Variant();
        if (j.is_boolean()) return Variant(j.get<bool>());
        if (j.is_number_integer() || j.is_number_unsigned()) return Variant(static_cast<int64_t>(j.get<int64_t>()));
        if (j.is_number_float()) return Variant(j.get<double>());
        if (j.is_string()) return Variant(j.get<std::string>());
        if (j.is_array()) {
            Variant v = Variant::makeArray();
            for (const auto& elem : j) v.push(variantFromJson(elem));
            return v;
        }
        if (j.is_object()) {
            Variant v = Variant::makeObject();
            for (auto it = j.begin(); it != j.end(); ++it) {
                v.set(it.key(), variantFromJson(it.value()));
            }
            return v;
        }
        return Variant::makeError("unsupported json type");
    }

    json jsonFromVariant(const Variant& v)
    {
        switch (v.type()) {
            case VariantType::Null: return nullptr;
            case VariantType::Bool: return v.toBool();
            case VariantType::Int: return v.toInt64();
            case VariantType::Real: return v.toDouble();
            case VariantType::String: return v.toString();
            case VariantType::Array: {
                json arr = json::array();
                if (auto a = v.array()) {
                    for (const auto& e : *a) {
                        if (e) arr.push_back(jsonFromVariant(*e));
                        else arr.push_back(nullptr);
                    }
                }
                return arr;
            }
            case VariantType::Object: {
                json obj = json::object();
                if (auto o = v.object()) {
                    for (const auto& kv : *o) {
                        if (kv.second) obj[kv.first] = jsonFromVariant(*kv.second);
                        else obj[kv.first] = nullptr;
                    }
                }
                return obj;
            }
            default: return nullptr;
        }
    }

    bool Variant_test_variantview_basic_access(std::vector<std::string>& errors)
    {
        Variant v = Variant::makeArray();
        v.push(Variant(1));
        v.push(Variant(2));
        Variant::VariantView view(v);
        if (!view.isArray()) errors.push_back("VariantView did not detect array");
        if (view.size() != 2) errors.push_back("VariantView size mismatch for array");
        const Variant* e0 = view.at(0);
        if (!e0 || e0->toInt64() != 1) errors.push_back("VariantView array element mismatch");

        Variant obj = Variant::makeObject();
        obj.set("a", Variant(42));
        Variant::VariantView vobj(obj);
        if (!vobj.isObject()) errors.push_back("VariantView did not detect object");
        const Variant* val = vobj.get("a");
        if (!val || val->toInt64() != 42) errors.push_back("VariantView object access mismatch");
        return true;
    }

    bool Variant_test_numeric_coercion_stress(std::vector<std::string>& errors)
    {
        struct Case { Variant v; int64_t expectInt; double expectDouble; bool expectBool; };
        std::vector<Case> cases{
            { Variant(int64_t(0)), 0, 0.0, false },
            { Variant(int64_t(1)), 1, 1.0, true },
            { Variant(int64_t(-5)), -5, -5.0, true },
            { Variant(3.14), 3, 3.14, true },
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
            if (i == cases.size()-1) {
                if (vi != 0) errors.push_back("Numeric coercion: unexpected toInt64 for non-number case");
            } else {
                if (vi != c.expectInt) errors.push_back("Numeric coercion: toInt64 mismatch");
            }
            if (std::fabs(vd - c.expectDouble) > 1e-9) errors.push_back("Numeric coercion: toDouble mismatch");
            if (vb != c.expectBool) errors.push_back("Numeric coercion: toBool mismatch");
        }

        Variant big = Variant(int64_t(std::numeric_limits<int64_t>::max()));
        if (big.toInt64(0) != std::numeric_limits<int64_t>::max()) errors.push_back("Numeric coercion: big int mismatch");
        return true;
    }

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
                case 0: { int64_t x = di(rng); v = Variant(x); expectInt = x; expectDouble = static_cast<double>(x); expectBool = (x != 0); break; }
                case 1: { double x = dr(rng); v = Variant(x); expectInt = static_cast<int64_t>(x); expectDouble = x; expectBool = (x != 0.0); break; }
                case 2: { int64_t x = di(rng); v = Variant(std::to_string(x)); expectInt = x; expectDouble = static_cast<double>(x); expectBool = false; break; }
                default: { v = Variant(std::string("str_") + std::to_string(rng()%1000)); expectInt = 0; expectDouble = 0.0; expectBool = false; break; }
            }
            if (v.toInt64(0) != expectInt) errors.push_back("Randomized coercion: toInt64 mismatch");
            if (std::fabs(v.toDouble(0.0) - expectDouble) > 1e-6) errors.push_back("Randomized coercion: toDouble mismatch");
            if (v.toBool(false) != expectBool) errors.push_back("Randomized coercion: toBool mismatch");
        }
        return true;
    }

    bool Variant_test_equality_composite_types(std::vector<std::string>& errors)
    {
        Variant a1 = Variant::makeArray(); a1.push(Variant(1)); a1.push(Variant(2));
        Variant a2 = Variant::makeArray(); a2.push(Variant(1)); a2.push(Variant(2));
        Variant a3 = Variant::makeArray(); a3.push(Variant(2)); a3.push(Variant(1));
        if (!(a1 == a2)) errors.push_back("Equality: identical arrays not equal");
        if (a1 == a3) errors.push_back("Equality: different-order arrays considered equal");

        Variant o1 = Variant::makeObject(); o1.set("a", Variant(1)); o1.set("b", Variant(2));
        Variant o2 = Variant::makeObject(); o2.set("b", Variant(2)); o2.set("a", Variant(1));
        if (!(o1 == o2)) errors.push_back("Equality: equivalent objects not equal");

        Variant vi = Variant(int64_t(42));
        Variant vf = Variant(42.0);
        if (!(vi == vf)) errors.push_back("Equality: int and double with same value not equal");

        auto sp = std::make_shared<int>(7);
        Variant d1 = Variant::makeDynamic<int>(sp);
        Variant d2 = Variant::makeDynamic<int>(sp);
        if (!(d1 == d2)) errors.push_back("Equality: dynamic variants with same pointer not equal");

        auto sp2 = std::make_shared<int>(7);
        Variant d3 = Variant::makeDynamic<int>(sp2);
        if (d1 == d3) errors.push_back("Equality: dynamic variants with different pointers considered equal");

        return true;
    }

    bool Variant_test_copy_move_and_containers(std::vector<std::string>& errors)
    {
        Variant vi = Variant(int64_t(123));
        Variant vs = Variant(std::string("hello"));

        std::vector<Variant> vec;
        vec.push_back(vi);
        vec.push_back(vs);
        if (vec.size() != 2) { errors.push_back("Container: vector size mismatch after push_back"); return true; }
        if (!(vec[0] == vi)) errors.push_back("Container: vector element 0 not equal to original int");
        if (!(vec[1] == vs)) errors.push_back("Container: vector element 1 not equal to original string");

        Variant vm = Variant::makeObject();
        vm.set("x", Variant(9));
        std::vector<Variant> vec2;
        vec2.emplace_back(std::move(vm));
        if (vec2.empty() || !vec2[0].isObject()) errors.push_back("Container: emplace_back(move) did not create object");

        std::unordered_map<Variant,int, VariantHash> map;
        Variant k1 = Variant::makeObject(); k1.set("id", Variant(1));
        Variant k2 = Variant::makeObject(); k2.set("id", Variant(1));
        map.emplace(k1, 10);
        auto it = map.find(k2);
        if (it == map.end()) errors.push_back("Container: unordered_map lookup by equivalent Variant key failed");

        auto map_copy = map;
        if (map_copy.size() != map.size()) errors.push_back("Container: unordered_map copy size mismatch");

        return true;
    }

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
        if (deep.find("c") == std::string::npos && deep.find("2") == std::string::npos)
            errors.push_back("toDebugString: deep output missing nested content");
        if (shallow.find("2") != std::string::npos)
            errors.push_back("toDebugString: shallow output unexpectedly contains deep value");
        return true;
    }

    bool Variant_test_varianthash_and_map(std::vector<std::string>& errors)
    {
        VariantHash h;
        Variant ka = Variant(int64_t(7));
        Variant kb = Variant(int64_t(7));
        if (h(ka) != h(kb)) errors.push_back("VariantHash: scalar equal values hashed differently");

        Variant arr1 = Variant::makeArray(); arr1.push(Variant(1)); arr1.push(Variant(2));
        Variant arr2 = Variant::makeArray(); arr2.push(Variant(1)); arr2.push(Variant(2));
        if (h(arr1) != h(arr2)) errors.push_back("VariantHash: equal arrays hashed differently");

        Variant o1 = Variant::makeObject(); o1.set("a", Variant(1)); o1.set("b", Variant(2));
        Variant o2 = Variant::makeObject(); o2.set("b", Variant(2)); o2.set("a", Variant(1));
        if (!(o1 == o2)) errors.push_back("VariantHash: objects expected equal but are not");
        else if (h(o1) != h(o2)) errors.push_back("VariantHash: equal objects hashed differently");

        auto sp = std::make_shared<int>(42);
        Variant d1 = Variant::makeDynamic<int>(sp);
        Variant d2 = Variant::makeDynamic<int>(sp);
        if (h(d1) != h(d2)) errors.push_back("VariantHash: dynamic variants same pointer hashed differently");

        std::unordered_map<Variant,int, VariantHash> map;
        Variant key1 = Variant::makeObject(); key1.set("id", Variant(99));
        Variant key2 = Variant::makeObject(); key2.set("id", Variant(99));
        map.emplace(key1, 1);
        map[key2] = 2;
        if (map.size() != 1) errors.push_back("Variant map: expected single bucket for equivalent keys");
        auto it = map.find(key1);
        if (it == map.end()) errors.push_back("Variant map: lookup of existing key failed");
        else if (it->second != 2) errors.push_back("Variant map: value after overwrite via equivalent key unexpected");

        std::unordered_map<Variant,int, VariantHash> map2;
        map2.emplace(d1, 10);
        auto it2 = map2.find(d2);
        if (it2 == map2.end()) errors.push_back("Variant map: lookup for dynamic key failed");
        return true;
    }

    bool Variant_test_json_roundtrip(std::vector<std::string>& errors)
    {
        // Build a representative Variant tree
        Variant root = Variant::makeObject();
        root.set("name", Variant(std::string("node")));
        root.set("count", Variant(int64_t(3)));
        Variant arr = Variant::makeArray();
        arr.push(Variant(true));
        arr.push(Variant(1.5));
        Variant nested = Variant::makeObject();
        nested.set("inner", Variant(int64_t(42)));
        arr.push(nested);
        root.set("items", arr);

        // Variant -> JSON
        auto j = jsonFromVariant(root);
        // JSON -> Variant
        Variant round = variantFromJson(j);

        if (!(root == round)) {
            errors.push_back("JSON roundtrip: Variant mismatch after serialize/deserialize");
        }

        // Check a couple of direct JSON fields to ensure shape is correct
        if (!j.is_object() || j["items"].size() != 3) {
            errors.push_back("JSON roundtrip: unexpected JSON structure");
        }
        return true;
    }

    bool Variant_test_deep_recursion_stress(std::vector<std::string>& errors)
    {
        const int depth = 120;
        Variant root = Variant::makeObject();
        Variant* cur = &root;
        for (int i = 0; i < depth; ++i) {
            Variant child = Variant::makeObject();
            child.set("__mark", Variant(i));
            cur->set("inner", child);
            Variant* next = cur->get("inner");
            if (!next) { errors.push_back("Deep recursion C++: failed to create nested node"); return true; }
            cur = next;
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

    } // namespace

    bool Variant_UnitTests()
    {
        const std::string objName = "Variant";
        UnitTests& ut = UnitTests::getInstance();

        static bool registered = false;
        if (!registered) {
            ut.add_test(objName, "Numeric coercion stress", Variant_test_numeric_coercion_stress);
            ut.add_test(objName, "Numeric coercion randomized", Variant_test_numeric_coercion_randomized);
            ut.add_test(objName, "Equality: composite types", Variant_test_equality_composite_types);
            ut.add_test(objName, "VariantView basic access", Variant_test_variantview_basic_access);
            ut.add_test(objName, "Copy/Move semantics and containers", Variant_test_copy_move_and_containers);
            ut.add_test(objName, "toDebugString shallow vs deep", Variant_test_toDebugString);
            ut.add_test(objName, "VariantHash & unordered_map usage", Variant_test_varianthash_and_map);
            ut.add_test(objName, "JSON roundtrip (Variant<->json)", Variant_test_json_roundtrip);
            ut.add_test(objName, "Deep recursion stress", Variant_test_deep_recursion_stress);
            ut.add_test(objName, "C API: roundtrip conversions", Variant_test_capi_roundtrip);
            ut.add_test(objName, "C API: copy semantics", Variant_test_capi_copy_semantics);
            ut.add_test(objName, "C API: type metadata", Variant_test_capi_type_metadata);
            ut.add_test(objName, "C API: error paths", Variant_test_capi_error_paths);
            registered = true;
        }
        return true;
    }

} // namespace SDOM
