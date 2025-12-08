// Variant_CAPI_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Variant.hpp>
#include <SDOM/SDOM_CoreAPI.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/CAPI/SDOM_CAPI_Variant.h>

#include <cstring>
#include <string>

#include "UnitTests.hpp"

namespace SDOM
{
    namespace
    {
        thread_local std::string s_capi_string_scratch;

        SDOM_Variant borrow_variant(Variant& v)
        {
            switch (v.type())
            {
                case VariantType::Null:  return SDOM_MakeNull();
                case VariantType::Bool:  return SDOM_MakeBool(v.toBool());
                case VariantType::Int:   return SDOM_MakeInt(v.toInt64());
                case VariantType::Real:  return SDOM_MakeFloat(v.toDouble());
                case VariantType::Array: {
                    SDOM_Variant out{}; out.type = SDOM_VARIANT_TYPE_ARRAY; out.data = reinterpret_cast<uint64_t>(&v); return out;
                }
                case VariantType::Object: {
                    SDOM_Variant out{}; out.type = SDOM_VARIANT_TYPE_OBJECT; out.data = reinterpret_cast<uint64_t>(&v); return out;
                }
                case VariantType::Dynamic: {
                    SDOM_Variant out{}; out.type = SDOM_VARIANT_TYPE_DYNAMIC; out.data = reinterpret_cast<uint64_t>(&v); return out;
                }
                case VariantType::String: {
                    s_capi_string_scratch = v.toString();
                    return SDOM_MakeCString(s_capi_string_scratch.c_str());
                }
                case VariantType::Error:
                default:
                    return SDOM_MakeNull();
            }
        }

        bool Variant_CAPI_path_success(std::vector<std::string>& errors)
        {
            Variant root = Variant::makeObject();
            Variant items = Variant::makeArray();
            items.push(Variant(int64_t(1)));
            items.push(Variant(int64_t(2)));
            items.push(Variant(int64_t(3)));
            root.set("items", items);
            root.set("x", Variant(int64_t(10)));

            SDOM_Variant root_c = borrow_variant(root);

            SDOM_ClearError();
            SDOM_Variant out{};
            if (!SDOM_Variant_GetPath(&root_c, "items[0]", &out) || !SDOM_IsInt(&out) || SDOM_AsInt(&out) != 1) {
                errors.push_back("SDOM_Variant_GetPath failed for items[0]");
            }

            if (!SDOM_Variant_GetPath(&root_c, "items[2]", &out) || !SDOM_IsInt(&out) || SDOM_AsInt(&out) != 3) {
                errors.push_back("SDOM_Variant_GetPath failed for items[2]");
            }

            if (!SDOM_Variant_GetPath(&root_c, "x", &out) || !SDOM_IsInt(&out) || SDOM_AsInt(&out) != 10) {
                errors.push_back("SDOM_Variant_GetPath failed for key 'x'");
            }

            return true;
        }

        bool Variant_CAPI_path_missing_key(std::vector<std::string>& errors)
        {
            Variant root = Variant::makeObject();
            Variant items = Variant::makeArray();
            items.push(Variant(int64_t(1)));
            items.push(Variant(int64_t(2)));
            root.set("items", items);

            SDOM_Variant root_c = borrow_variant(root);

            SDOM_ClearError();
            SDOM_Variant out = SDOM_MakeBool(true); // ensure it gets overwritten
            if (SDOM_Variant_GetPath(&root_c, "missing", &out)) {
                errors.push_back("SDOM_Variant_GetPath unexpectedly succeeded for missing key");
            } else {
                if (!SDOM_IsNull(&out)) {
                    errors.push_back("SDOM_Variant_GetPath did not null out output on failure");
                }
                const char* err = SDOM_GetError();
                if (!err || std::strstr(err, "Path not found") == nullptr) {
                    errors.push_back("Missing key error did not mention 'Path not found'");
                }
            }

            return true;
        }

        bool Variant_CAPI_path_type_mismatch(std::vector<std::string>& errors)
        {
            Variant root = Variant::makeObject();
            Variant items = Variant::makeArray();
            items.push(Variant(int64_t(1)));
            root.set("items", items);

            SDOM_Variant root_c = borrow_variant(root);

            SDOM_ClearError();
            SDOM_Variant out = SDOM_MakeNull();
            if (SDOM_Variant_GetPath(&root_c, "items.key", &out)) {
                errors.push_back("SDOM_Variant_GetPath unexpectedly succeeded for array field access");
            } else {
                const char* err = SDOM_GetError();
                if (!err || std::strstr(err, "Expected object for field access") == nullptr) {
                    errors.push_back("Type mismatch error did not mention object field access");
                }
            }

            return true;
        }

        bool Variant_CAPI_path_index_oob(std::vector<std::string>& errors)
        {
            Variant root = Variant::makeObject();
            Variant items = Variant::makeArray();
            items.push(Variant(int64_t(1)));
            items.push(Variant(int64_t(2)));
            root.set("items", items);

            SDOM_Variant root_c = borrow_variant(root);

            SDOM_ClearError();
            SDOM_Variant out = SDOM_MakeNull();
            if (SDOM_Variant_GetPath(&root_c, "items[3]", &out)) {
                errors.push_back("SDOM_Variant_GetPath unexpectedly succeeded for out-of-range index");
            } else {
                const char* err = SDOM_GetError();
                if (!err || std::strstr(err, "Index out of range") == nullptr) {
                    errors.push_back("Out-of-range error did not mention index issue");
                }
            }

            return true;
        }

        bool Variant_CAPI_path_scalar_traverse(std::vector<std::string>& errors)
        {
            Variant root(int64_t(7));
            SDOM_Variant root_c = borrow_variant(root);

            SDOM_ClearError();
            SDOM_Variant out = SDOM_MakeBool(true);
            if (SDOM_Variant_GetPath(&root_c, "child", &out)) {
                errors.push_back("SDOM_Variant_GetPath unexpectedly succeeded on scalar root");
            } else {
                const char* err = SDOM_GetError();
                if (!err || std::strstr(err, "Cannot traverse into scalar type") == nullptr) {
                    errors.push_back("Scalar traversal error did not mention scalar type");
                }
            }

            return true;
        }

    } // namespace

    bool Variant_CAPI_UnitTests()
    {
        const std::string objName = "Variant_CAPI";
        UnitTests& ut = UnitTests::getInstance();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "CAPI: object/array path success", Variant_CAPI_path_success);
            ut.add_test(objName, "CAPI: missing key", Variant_CAPI_path_missing_key);
            ut.add_test(objName, "CAPI: type mismatch", Variant_CAPI_path_type_mismatch);
            ut.add_test(objName, "CAPI: index out of range", Variant_CAPI_path_index_oob);
            ut.add_test(objName, "CAPI: scalar traversal", Variant_CAPI_path_scalar_traverse);
            registered = true;
        }

        return true;
    }

} // namespace SDOM
