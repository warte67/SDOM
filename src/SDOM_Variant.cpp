
#include <SDOM/SDOM_Variant.hpp>

#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <SDOM/SDOM_Factory.hpp>

#include <cmath>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <limits>
#include <mutex>
#include <json.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_CoreAPI.hpp>
#include <SDOM/CAPI/SDOM_CAPI_Handles.h>
#include <SDOM/CAPI/SDOM_CAPI_Variant.h>

namespace {

double bitsToDouble(std::uint64_t bits) {
    double d = 0.0;
    std::memcpy(&d, &bits, sizeof(double));
    return d;
}

std::uint64_t doubleToBits(double d) {
    std::uint64_t bits = 0;
    std::memcpy(&bits, &d, sizeof(double));
    return bits;
}

struct DisplayHandleScratch {
    std::string name;
    std::string type;
};

struct AssetHandleScratch {
    std::string name;
    std::string type;
};

thread_local DisplayHandleScratch s_variantDisplayHandleScratch;
thread_local AssetHandleScratch s_variantAssetHandleScratch;
thread_local SDOM_DisplayHandle s_variantDisplayHandleOut;
thread_local SDOM_AssetHandle s_variantAssetHandleOut;

// Variant marshaling helpers (scalar-only ABI view for now).
thread_local std::string s_variantStringScratch;
thread_local SDOM::Variant s_variantValueScratch;

// Traversal helper that stays pure by default; caller may opt-in to an
// error string when needed (e.g., string-based getPath entry point).
bool getPathImpl(const SDOM::Variant& root,
                 const SDOM::PathView& path,
                 SDOM::Variant& out,
                 std::string* errMessage)
{
    auto setErr = [&](const char* msg) {
        if (errMessage && msg) *errMessage = msg;
    };

    if (path.empty()) {
        setErr("Path is empty");
        return false;
    }

    const auto isScalar = [](const SDOM::Variant& v) {
        const auto t = v.type();
        return t == SDOM::VariantType::Null ||
               t == SDOM::VariantType::Bool ||
               t == SDOM::VariantType::Int ||
               t == SDOM::VariantType::Real ||
               t == SDOM::VariantType::String ||
               t == SDOM::VariantType::Dynamic;
    };

    // Prevent multi-segment traversal into a scalar root before iterating.
    if (isScalar(root) && path.size() > 1) {
        setErr("Cannot traverse into scalar type");
        return false;
    }

    const SDOM::Variant* cur = &root;
    const auto& elems = path.elements();

    for (std::size_t i = 0; i < elems.size(); ++i) {
        const bool last = (i + 1 == elems.size());
        const auto& elem = elems[i];

        if (elem.kind == SDOM::PathElement::Kind::Key) {
            if (!cur->isObject()) {
                if (cur->isArray()) {
                    setErr("Expected object for field access");
                } else {
                    setErr("Cannot traverse into scalar type");
                }
                return false;
            }
            const auto* obj = cur->object();
            if (!obj) {
                setErr("Path not found");
                return false;
            }
            auto it = obj->find(elem.key);
            if (it == obj->end() || !it->second) {
                setErr("Path not found");
                return false;
            }
            cur = it->second.get();
        } else {
            if (!cur->isArray()) {
                if (cur->isObject()) {
                    setErr("Expected array for index access");
                } else {
                    setErr("Cannot traverse into scalar type");
                }
                return false;
            }
            const auto* arr = cur->array();
            if (!arr) {
                setErr("Path not found");
                return false;
            }
            if (elem.index < 0 || static_cast<size_t>(elem.index) >= arr->size()) {
                setErr("Index out of range");
                return false;
            }
            const auto& ptr = (*arr)[static_cast<size_t>(elem.index)];
            if (!ptr) {
                setErr("Path not found");
                return false;
            }
            cur = ptr.get();
        }

        // If we have more segments after landing on a scalar, stop early with
        // the scalar-specific error to avoid mismatched messaging.
        if (!last && isScalar(*cur)) {
            setErr("Cannot traverse into scalar type");
            return false;
        }
    }

    out = *cur;
    return true;
}

bool importVariantFromC(const SDOM_Variant* in, SDOM::Variant& out)
{
    out = SDOM::Variant{};
    if (!in) return false;

    switch (static_cast<SDOM_VariantType>(in->type)) {
        case SDOM_VARIANT_TYPE_NULL:
            out = SDOM::Variant{};
            return true;
        case SDOM_VARIANT_TYPE_BOOL:
            out = SDOM::Variant(static_cast<bool>(in->data != 0));
            return true;
        case SDOM_VARIANT_TYPE_INT:
            out = SDOM::Variant(static_cast<std::int64_t>(in->data));
            return true;
        case SDOM_VARIANT_TYPE_FLOAT: {
            const double f = bitsToDouble(static_cast<std::uint64_t>(in->data));
            out = SDOM::Variant(f);
            return true;
        }
        case SDOM_VARIANT_TYPE_STRING: {
            const char* s = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(in->data));
            out = SDOM::Variant(s ? std::string{s} : std::string{});
            return true;
        }
        case SDOM_VARIANT_TYPE_DISPLAY_HANDLE: {
            const uint64_t id = static_cast<uint64_t>(in->data);
            SDOM::DisplayHandle h;
            if (id != 0 && SDOM::DisplayHandle::factory_) {
                h = SDOM::DisplayHandle::factory_->resolveDisplayHandleById(id);
            }
            if (!h.getId()) h.setId(id);
            out = SDOM::Variant(std::move(h));
            return true;
        }
        case SDOM_VARIANT_TYPE_ASSET_HANDLE: {
            const uint64_t id = static_cast<uint64_t>(in->data);
            SDOM::AssetHandle h;
            if (id != 0 && SDOM::AssetHandle::factory_) {
                h = SDOM::AssetHandle::factory_->resolveAssetHandleById(id);
            }
            if (!h.getId()) h.setId(id);
            out = SDOM::Variant(std::move(h));
            return true;
        }
        case SDOM_VARIANT_TYPE_ARRAY:
        case SDOM_VARIANT_TYPE_OBJECT:
        case SDOM_VARIANT_TYPE_DYNAMIC: {
            const auto* ptr = reinterpret_cast<const SDOM::Variant*>(static_cast<std::uintptr_t>(in->data));
            if (!ptr) {
                SDOM::CoreAPI::setErrorMessage("Unsupported Variant type for ABI import");
                return false;
            }
            out = *ptr; // borrow/copy the pointed-to variant
            return true;
        }
        default:
            SDOM::CoreAPI::setErrorMessage("Unsupported Variant type for ABI import");
            return false;
    }
}

// Import variant without touching global error state (used for query-only flows
// like PathExists where callers expect no side effects on errors).
bool importVariantFromCNoError(const SDOM_Variant* in, SDOM::Variant& out)
{
    out = SDOM::Variant{};
    if (!in) return false;

    switch (static_cast<SDOM_VariantType>(in->type)) {
        case SDOM_VARIANT_TYPE_NULL:
            out = SDOM::Variant{};
            return true;
        case SDOM_VARIANT_TYPE_BOOL:
            out = SDOM::Variant(static_cast<bool>(in->data != 0));
            return true;
        case SDOM_VARIANT_TYPE_INT:
            out = SDOM::Variant(static_cast<std::int64_t>(in->data));
            return true;
        case SDOM_VARIANT_TYPE_FLOAT: {
            const double f = bitsToDouble(static_cast<std::uint64_t>(in->data));
            out = SDOM::Variant(f);
            return true;
        }
        case SDOM_VARIANT_TYPE_STRING: {
            const char* s = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(in->data));
            out = SDOM::Variant(s ? std::string{s} : std::string{});
            return true;
        }
        case SDOM_VARIANT_TYPE_DISPLAY_HANDLE: {
            const uint64_t id = static_cast<uint64_t>(in->data);
            SDOM::DisplayHandle h;
            if (id != 0 && SDOM::DisplayHandle::factory_) {
                h = SDOM::DisplayHandle::factory_->resolveDisplayHandleById(id);
            }
            if (!h.getId()) h.setId(id);
            out = SDOM::Variant(std::move(h));
            return true;
        }
        case SDOM_VARIANT_TYPE_ASSET_HANDLE: {
            const uint64_t id = static_cast<uint64_t>(in->data);
            SDOM::AssetHandle h;
            if (id != 0 && SDOM::AssetHandle::factory_) {
                h = SDOM::AssetHandle::factory_->resolveAssetHandleById(id);
            }
            if (!h.getId()) h.setId(id);
            out = SDOM::Variant(std::move(h));
            return true;
        }
        case SDOM_VARIANT_TYPE_ARRAY:
        case SDOM_VARIANT_TYPE_OBJECT:
        case SDOM_VARIANT_TYPE_DYNAMIC: {
            const auto* ptr = reinterpret_cast<const SDOM::Variant*>(static_cast<std::uintptr_t>(in->data));
            if (!ptr) return false;
            out = *ptr;
            return true;
        }
        default:
            return false;
    }
}

SDOM_Variant exportVariantToC(const SDOM::Variant& v)
{
    SDOM_Variant out{};
    switch (v.type()) {
        case SDOM::VariantType::Null:
            out.type = SDOM_VARIANT_TYPE_NULL;
            break;
        case SDOM::VariantType::Bool:
            out.type = SDOM_VARIANT_TYPE_BOOL;
            out.data = static_cast<std::uint64_t>(v.toBool());
            break;
        case SDOM::VariantType::Int:
            out.type = SDOM_VARIANT_TYPE_INT;
            out.data = static_cast<std::uint64_t>(v.toInt64());
            break;
        case SDOM::VariantType::Real:
            out.type = SDOM_VARIANT_TYPE_FLOAT;
            out.data = doubleToBits(static_cast<double>(v.toDouble()));
            break;
        case SDOM::VariantType::String:
            out.type = SDOM_VARIANT_TYPE_STRING;
            s_variantStringScratch = v.toString();
            out.data = reinterpret_cast<std::uint64_t>(s_variantStringScratch.c_str());
            break;
        case SDOM::VariantType::DisplayHandle: {
            out.type = SDOM_VARIANT_TYPE_DISPLAY_HANDLE;
            const auto* dh = v.displayHandle();
            out.data = dh ? dh->getId() : 0;
            break;
        }
        case SDOM::VariantType::AssetHandle: {
            out.type = SDOM_VARIANT_TYPE_ASSET_HANDLE;
            const auto* ah = v.assetHandle();
            out.data = ah ? ah->getId() : 0;
            break;
        }
        case SDOM::VariantType::Array:
            out.type = SDOM_VARIANT_TYPE_ARRAY;
            s_variantValueScratch = v;
            out.data = reinterpret_cast<std::uint64_t>(&s_variantValueScratch);
            break;
        case SDOM::VariantType::Object:
            out.type = SDOM_VARIANT_TYPE_OBJECT;
            s_variantValueScratch = v;
            out.data = reinterpret_cast<std::uint64_t>(&s_variantValueScratch);
            break;
        case SDOM::VariantType::Dynamic:
            out.type = SDOM_VARIANT_TYPE_DYNAMIC;
            s_variantValueScratch = v;
            out.data = reinterpret_cast<std::uint64_t>(&s_variantValueScratch);
            break;
        default:
            SDOM::CoreAPI::setErrorMessage("Unsupported Variant type for ABI export");
            return out; // defaults to zeroed/null
    }
    return out;
}

} // namespace

extern "C" {

// Basic Variant C API surface (scalar-only ABI view) ----------------------

uint8_t SDOM_Type(const SDOM_Variant* v) {
    if (!v) return 0;
    return v->type;
}

bool SDOM_IsNull(const SDOM_Variant* v)   { return SDOM_Type(v) == SDOM_VARIANT_TYPE_NULL; }
bool SDOM_IsBool(const SDOM_Variant* v)   { return SDOM_Type(v) == SDOM_VARIANT_TYPE_BOOL; }
bool SDOM_IsInt(const SDOM_Variant* v)    { return SDOM_Type(v) == SDOM_VARIANT_TYPE_INT; }
bool SDOM_IsFloat(const SDOM_Variant* v)  { return SDOM_Type(v) == SDOM_VARIANT_TYPE_FLOAT; }
bool SDOM_IsString(const SDOM_Variant* v) { return SDOM_Type(v) == SDOM_VARIANT_TYPE_STRING; }
bool SDOM_IsDisplayHandle(const SDOM_Variant* v) { return SDOM_Type(v) == SDOM_VARIANT_TYPE_DISPLAY_HANDLE; }
bool SDOM_IsAssetHandle(const SDOM_Variant* v)   { return SDOM_Type(v) == SDOM_VARIANT_TYPE_ASSET_HANDLE; }

SDOM_Variant SDOM_MakeNull(void) {
    SDOM_Variant v { SDOM_VARIANT_TYPE_NULL, {0,0,0}, 0, 0 };
    return v;
}

SDOM_Variant SDOM_MakeBool(bool b) {
    SDOM_Variant v { SDOM_VARIANT_TYPE_BOOL, {0,0,0}, 0, static_cast<uint64_t>(b) };
    return v;
}

SDOM_Variant SDOM_MakeInt(int64_t i) {
    SDOM_Variant v { SDOM_VARIANT_TYPE_INT, {0,0,0}, 0, static_cast<uint64_t>(i) };
    return v;
}

SDOM_Variant SDOM_MakeFloat(double f) {
    uint64_t d = doubleToBits(f);
    SDOM_Variant v { SDOM_VARIANT_TYPE_FLOAT, {0,0,0}, 0, d };
    return v;
}

SDOM_Variant SDOM_MakeCString(const char* utf8) {
    SDOM_Variant v { SDOM_VARIANT_TYPE_STRING, {0,0,0}, 0, reinterpret_cast<uint64_t>(utf8) };
    return v;
}

static bool SDOM_Handle_IsDisplay_Impl(const SDOM_Variant* handle)
{
    return handle && static_cast<SDOM_VariantType>(handle->type) == SDOM_VARIANT_TYPE_DISPLAY_HANDLE;
}

static bool SDOM_Handle_IsAsset_Impl(const SDOM_Variant* handle)
{
    return handle && static_cast<SDOM_VariantType>(handle->type) == SDOM_VARIANT_TYPE_ASSET_HANDLE;
}

static uint64_t SDOM_Handle_ObjectId_Impl(const SDOM_Variant* handle)
{
    if (!handle) {
        return 0;
    }

    if (SDOM_Handle_IsDisplay_Impl(handle) || SDOM_Handle_IsAsset_Impl(handle)) {
        return static_cast<uint64_t>(handle->data);
    }

    return 0;
}

static bool SDOM_Handle_IsValid_Impl(const SDOM_Variant* handle)
{
    if (!handle) {
        SDOM::CoreAPI::setErrorMessage("SDOM_Handle_IsValid: subject 'handle' is null");
        return false;
    }

    const uint64_t id = SDOM_Handle_ObjectId_Impl(handle);
    if (id == 0) {
        return false;
    }

    if (SDOM_Handle_IsDisplay_Impl(handle)) {
        if (SDOM::DisplayHandle::factory_) {
            return SDOM::DisplayHandle::factory_->resolveDisplayHandleById(id).isValid();
        }
        SDOM::CoreAPI::setErrorMessage("SDOM_Handle_IsValid: no display factory registered");
        return false;
    }

    if (SDOM_Handle_IsAsset_Impl(handle)) {
        if (SDOM::AssetHandle::factory_) {
            return SDOM::AssetHandle::factory_->resolveAssetHandleById(id).isValid();
        }
        SDOM::CoreAPI::setErrorMessage("SDOM_Handle_IsValid: no asset factory registered");
        return false;
    }

    SDOM::CoreAPI::setErrorMessage("SDOM_Handle_IsValid: variant is not a handle");
    return false;
}

SDOM_Variant SDOM_MakeDisplayHandle(const SDOM_DisplayHandle* handle) {
    SDOM_Variant v { SDOM_VARIANT_TYPE_DISPLAY_HANDLE, {0,0,0}, 0, 0 };
    if (!handle) return v;
    v.data = handle->object_id;
    return v;
}

SDOM_Variant SDOM_MakeAssetHandle(const SDOM_AssetHandle* handle) {
    SDOM_Variant v { SDOM_VARIANT_TYPE_ASSET_HANDLE, {0,0,0}, 0, 0 };
    if (!handle) return v;
    v.data = handle->object_id;
    return v;
}

bool SDOM_AsBool(const SDOM_Variant* v) {
    return SDOM_IsBool(v) ? static_cast<bool>(v->data) : false;
}

int64_t SDOM_AsInt(const SDOM_Variant* v) {
    return SDOM_IsInt(v) ? static_cast<int64_t>(v->data) : 0;
}

// Path helpers -------------------------------------------------------------

bool SDOM_Variant_GetPath(const SDOM_Variant* root, const char* path, SDOM_Variant* out_value) {
    if (!out_value) return false;
    *out_value = SDOM_MakeNull();
    if (!root || !path) return false;

    SDOM::Variant v_root;
    if (!importVariantFromC(root, v_root)) {
        return false; // import already set Core error
    }

    SDOM::Variant result;
    if (!v_root.getPath(path, result)) {
        return false; // getPath sets Core error on parse/semantic failure
    }

    *out_value = exportVariantToC(result);
    return true;
}

bool SDOM_Variant_SetPath(SDOM_Variant* root, const char* path, const SDOM_Variant* value) {
    if (!root || !path || !value) return false;

    SDOM::Variant v_root;
    if (!importVariantFromC(root, v_root)) return false;

    SDOM::Variant v_value;
    if (!importVariantFromC(value, v_value)) return false;

    if (!v_root.setPath(path, v_value)) {
        return false; // setPath sets Core error on failure
    }

    *root = exportVariantToC(v_root);
    return true;
}

bool SDOM_Variant_PathExists(const SDOM_Variant* root, const char* path) {
    if (!root || !path) return false;

    SDOM::Variant v_root;
    if (!importVariantFromCNoError(root, v_root)) return false;

    return v_root.pathExists(path);
}

bool SDOM_Variant_ErasePath(SDOM_Variant* root, const char* path) {
    if (!root || !path) return false;

    SDOM::Variant v_root;
    if (!importVariantFromC(root, v_root)) return false;

    if (!v_root.erasePath(path)) return false;

    *root = exportVariantToC(v_root);
    return true;
}

} // extern "C"

// Nothing to implement out-of-line here; registry helpers are inline in header.

namespace SDOM {

void Variant::registerBindings(DataRegistry& registry) {
    // Ensure a TypeInfo entry for the C++ Variant type so functions can attach.
    if (!registry.lookupType("Variant")) {
        SDOM::TypeInfo ti;
        ti.name        = "Variant";
        ti.kind        = SDOM::EntryKind::Object;
        ti.cpp_type_id = "SDOM::Variant";
        ti.file_stem   = "Variant";
        ti.export_name = ""; // no direct C handle yet
        ti.subject_kind = "Variant";
        ti.subject_uses_handle = false;
        ti.has_handle_override = true;
        ti.doc = "Reflectable SDOM::Variant value type (no ABI surface yet).";
        registry.registerType(ti);
    }

    if (!registry.lookupType("SDOM_Variant")) {
        SDOM::TypeInfo ti;
        ti.name        = "SDOM_Variant";
        ti.kind        = SDOM::EntryKind::Struct;
        ti.cpp_type_id = "SDOM::Variant";
        ti.file_stem   = "Variant";
        ti.export_name = "SDOM_Variant";
        ti.category    = "Variant";
        ti.doc         = "Provisional ABI view of SDOM::Variant; layout may change.";

        auto addField = [&](const std::string& fieldName,
                            const std::string& cppType,
                            const std::string& doc) {
            SDOM::PropertyInfo p;
            p.name      = fieldName;
            p.cpp_type  = cppType;
            p.read_only = false;
            p.doc       = doc;
            ti.properties.push_back(std::move(p));
        };

        addField("type", "uint8_t", "Variant kind tag; mirrors SDOM_VariantType." );
        addField("reserved", "uint8_t[3]", "Reserved bytes; must be zero when passed across the ABI." );
        addField("flags", "uint32_t", "Future use bitmask; currently must be zero." );
        addField("data", "uint64_t", "Inline payload or handle depending on type/flags; provisional semantics." );

        registry.registerType(ti);
    }

    if (!registry.lookupType("Handle")) {
        SDOM::TypeInfo ti;
        ti.name        = "Handle";
        ti.kind        = SDOM::EntryKind::Function;
        ti.cpp_type_id = "SDOM::Variant";
        ti.file_stem   = "Handles";
        ti.export_name = "";
        ti.category    = "Handles";
        ti.subject_kind = "Core";
        ti.doc         = "Variant-backed handle helpers (no struct ABI).";
        registry.registerType(ti);
    }

    // --- Register Variant C API Functions ---
    // These attach behavior to the Variant type for C ABI bindings.
    auto registerFn = [&](const std::string& name, const std::string& ret, const std::string& sig) {
        SDOM::FunctionInfo fi;
        fi.name         = name;
        fi.c_name       = name;
        fi.c_signature  = sig;
        fi.return_type  = ret;
        fi.exported     = true;
        registry.registerFunction("Variant", fi);
    };

    registerFn("SDOM_Type", "uint8_t", "uint8_t SDOM_Type(const SDOM_Variant* v)");
    registerFn("SDOM_IsNull", "bool", "bool SDOM_IsNull(const SDOM_Variant* v)");
    registerFn("SDOM_IsBool", "bool", "bool SDOM_IsBool(const SDOM_Variant* v)");
    registerFn("SDOM_IsInt", "bool", "bool SDOM_IsInt(const SDOM_Variant* v)");
    registerFn("SDOM_IsFloat", "bool", "bool SDOM_IsFloat(const SDOM_Variant* v)");
    registerFn("SDOM_IsString", "bool", "bool SDOM_IsString(const SDOM_Variant* v)");
    registerFn("SDOM_IsDisplayHandle", "bool", "bool SDOM_IsDisplayHandle(const SDOM_Variant* v)");
    registerFn("SDOM_IsAssetHandle", "bool", "bool SDOM_IsAssetHandle(const SDOM_Variant* v)");

    registerFn("SDOM_MakeNull", "SDOM_Variant", "SDOM_Variant SDOM_MakeNull(void)");
    registerFn("SDOM_MakeBool", "SDOM_Variant", "SDOM_Variant SDOM_MakeBool(bool b)");
    registerFn("SDOM_MakeInt", "SDOM_Variant", "SDOM_Variant SDOM_MakeInt(int64_t i)");
    registerFn("SDOM_MakeFloat", "SDOM_Variant", "SDOM_Variant SDOM_MakeFloat(double f)");
    registerFn("SDOM_MakeCString", "SDOM_Variant", "SDOM_Variant SDOM_MakeCString(const char* utf8)");
    registerFn("SDOM_AsBool", "bool", "bool SDOM_AsBool(const SDOM_Variant* v)");
    registerFn("SDOM_AsInt", "int64_t", "int64_t SDOM_AsInt(const SDOM_Variant* v)");
    registerFn("SDOM_AsFloat", "double", "double SDOM_AsFloat(const SDOM_Variant* v)");
    registerFn("SDOM_AsString", "const char*", "const char* SDOM_AsString(const SDOM_Variant* v)");

    registerFn("SDOM_Variant_GetPath", "bool", "bool SDOM_Variant_GetPath(const SDOM_Variant* root, const char* path, SDOM_Variant* out_value)");
    registerFn("SDOM_Variant_SetPath", "bool", "bool SDOM_Variant_SetPath(SDOM_Variant* root, const char* path, const SDOM_Variant* value)");
    registerFn("SDOM_Variant_PathExists", "bool", "bool SDOM_Variant_PathExists(const SDOM_Variant* root, const char* path)");
    registerFn("SDOM_Variant_ErasePath", "bool", "bool SDOM_Variant_ErasePath(SDOM_Variant* root, const char* path)");

    auto registerHandleFn = [&](const std::string& name,
                                const std::string& ret,
                                const std::string& sig,
                                const std::string& doc,
                                auto* fnPtr) {
        SDOM::FunctionInfo fi;
        fi.name        = name;
        fi.c_name      = name;
        fi.c_signature = sig;
        fi.return_type = ret;
        fi.doc         = doc;
        fi.exported    = true;
        registry.registerFunction("Handle", fi, fnPtr);
    };

    registerHandleFn("SDOM_Handle_IsDisplay",
                     "bool",
                     "bool SDOM_Handle_IsDisplay(const SDOM_Variant* handle)",
                     "Returns true when the variant tags a display handle.",
                     &SDOM_Handle_IsDisplay_Impl);

    registerHandleFn("SDOM_Handle_IsAsset",
                     "bool",
                     "bool SDOM_Handle_IsAsset(const SDOM_Variant* handle)",
                     "Returns true when the variant tags an asset handle.",
                     &SDOM_Handle_IsAsset_Impl);

    registerHandleFn("SDOM_Handle_IsValid",
                     "bool",
                     "bool SDOM_Handle_IsValid(const SDOM_Variant* handle)",
                     "Resolves the tagged handle and reports whether it maps to a live object.",
                     &SDOM_Handle_IsValid_Impl);

    registerHandleFn("SDOM_Handle_ObjectId",
                     "uint64_t",
                     "uint64_t SDOM_Handle_ObjectId(const SDOM_Variant* handle)",
                     "Returns the underlying object identifier for display or asset handles; returns 0 otherwise.",
                     &SDOM_Handle_ObjectId_Impl);
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructors
// ─────────────────────────────────────────────────────────────────────────────
Variant::Variant()
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = std::monostate{};
}

// Variant(nullptr) overload removed — default constructor represents null

Variant::Variant(bool b)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = b;
}

Variant::Variant(int32_t i)
: Variant(static_cast<int64_t>(i)) {}

Variant::Variant(int64_t i)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = i;
}

Variant::Variant(double d)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = d;
}

Variant::Variant(const char* s)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = std::string{s ? s : ""};
}

Variant::Variant(std::string s)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = std::move(s);
}

Variant::Variant(const std::vector<Variant>& arr)
: storage_(std::make_shared<VariantStorage>()) {
    // convert by-value vector<Variant> to vector<shared_ptr<Variant>>
    VariantStorage::Array a;
    a.reserve(arr.size());
    for (const auto &e : arr) a.push_back(std::make_shared<Variant>(e));
    storage_->data = std::move(a);
}

Variant::Variant(std::vector<Variant>&& arr)
: storage_(std::make_shared<VariantStorage>()) {
    VariantStorage::Array a;
    a.reserve(arr.size());
    for (auto &e : arr) a.push_back(std::make_shared<Variant>(std::move(e)));
    storage_->data = std::move(a);
}

Variant::Variant(const std::unordered_map<std::string, Variant>& obj)
: storage_(std::make_shared<VariantStorage>()) {
    VariantStorage::Object o;
    o.reserve(obj.size());
    for (const auto &kv : obj) o.insert_or_assign(kv.first, std::make_shared<Variant>(kv.second));
    storage_->data = std::move(o);
}

Variant::Variant(std::unordered_map<std::string, Variant>&& obj)
: storage_(std::make_shared<VariantStorage>()) {
    VariantStorage::Object o;
    o.reserve(obj.size());
    for (auto &kv : obj) o.insert_or_assign(std::move(kv.first), std::make_shared<Variant>(std::move(kv.second)));
    storage_->data = std::move(o);
}

Variant::Variant(const nlohmann::json& j)
: storage_(std::make_shared<VariantStorage>()) {
    *this = Variant::fromJson(j);
}

Variant::Variant(const DisplayHandle& handle)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = handle;
}

Variant::Variant(DisplayHandle&& handle)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = std::move(handle);
}

Variant::Variant(const AssetHandle& handle)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = handle;
}

Variant::Variant(AssetHandle&& handle)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = std::move(handle);
}

// (templated dynamic constructor is defined in the header)

// Factories
Variant Variant::makeArray() {
    Variant v;
    v.storage_->data = VariantStorage::Array{};
    return v;
}

Variant Variant::makeObject() {
    Variant v;
    v.storage_->data = VariantStorage::Object{};
    return v;
}

// Error helpers
Variant Variant::makeError(const std::string& msg) {
    Variant v;
    v.storage_->data = std::monostate{};
    v.errorFlag_ = true;
    v.errorMsg_  = msg;
    return v;
}

// Type info
VariantType Variant::type() const noexcept {
    if (errorFlag_) return VariantType::Error;

    const auto& d = storage_->data;
    if (std::holds_alternative<std::monostate>(d))                 return VariantType::Null;
    if (std::holds_alternative<bool>(d))                           return VariantType::Bool;
    if (std::holds_alternative<int64_t>(d))                        return VariantType::Int;
    if (std::holds_alternative<double>(d))                         return VariantType::Real;
    if (std::holds_alternative<std::string>(d))                    return VariantType::String;
    if (std::holds_alternative<VariantStorage::Array>(d))          return VariantType::Array;
    if (std::holds_alternative<VariantStorage::Object>(d))         return VariantType::Object;
    if (std::holds_alternative<DisplayHandle>(d))                  return VariantType::DisplayHandle;
    if (std::holds_alternative<AssetHandle>(d))                    return VariantType::AssetHandle;
    if (std::holds_alternative<VariantStorage::DynamicValue>(d))     return VariantType::Dynamic;
    return VariantType::Error;
}

// Conversions
bool Variant::toBool(bool def) const noexcept {
    const auto& d = storage_->data;
    if (auto p = std::get_if<bool>(&d)) return *p;
    if (auto pi = std::get_if<int64_t>(&d)) return *pi != 0;
    if (auto pr = std::get_if<double>(&d))  return *pr != 0.0;
    if (auto ps = std::get_if<std::string>(&d)) return (*ps == "true") || (*ps == "1");
    if (auto pdh = std::get_if<DisplayHandle>(&d)) return pdh->isValid();
    if (auto pah = std::get_if<AssetHandle>(&d)) return pah->isValid();
    return def;
}

int64_t Variant::toInt64(int64_t def) const noexcept {
    const auto& d = storage_->data;
    if (auto p = std::get_if<int64_t>(&d)) return *p;
    if (auto pr = std::get_if<double>(&d)) return static_cast<int64_t>(*pr);
    if (auto pb = std::get_if<bool>(&d))   return *pb ? 1 : 0;
    if (auto ps = std::get_if<std::string>(&d)) {
        try { return static_cast<int64_t>(std::stoll(*ps)); } catch(...) { return def; }
    }
    return def;
}

double Variant::toDouble(double def) const noexcept {
    const auto& d = storage_->data;
    if (auto p = std::get_if<double>(&d))  return *p;
    if (auto pi = std::get_if<int64_t>(&d))return static_cast<double>(*pi);
    if (auto pb = std::get_if<bool>(&d))   return *pb ? 1.0 : 0.0;
    if (auto ps = std::get_if<std::string>(&d)) {
        try { return std::stod(*ps); } catch(...) { return def; }
    }
    return def;
}

std::string Variant::toString(std::string def) const noexcept {
    const auto& d = storage_->data;
    if (auto p = std::get_if<std::string>(&d)) return *p;
    if (auto pi = std::get_if<int64_t>(&d))    return std::to_string(*pi);
    if (auto pr = std::get_if<double>(&d))     return std::to_string(*pr);
    if (auto pb = std::get_if<bool>(&d))       return *pb ? "true" : "false";
    if (auto pdh = std::get_if<DisplayHandle>(&d)) return pdh->str();
    if (auto pah = std::get_if<AssetHandle>(&d)) {
        std::string s = pah->getName();
        if (!pah->getType().empty()) {
            s += " (" + pah->getType() + ")";
        }
        if (!pah->getFilename().empty()) {
            s += " @" + pah->getFilename();
        }
        return s;
    }
    if (std::holds_alternative<std::monostate>(d)) return "null";
    return def;
}

DisplayHandle Variant::toDisplayHandle(DisplayHandle def) const noexcept {
    if (auto p = std::get_if<DisplayHandle>(&storage_->data)) return *p;
    return def;
}

AssetHandle Variant::toAssetHandle(AssetHandle def) const noexcept {
    if (auto p = std::get_if<AssetHandle>(&storage_->data)) return *p;
    return def;
}

// Array/Object helpers
const VariantStorage::Array* Variant::array() const noexcept {
    return std::get_if<VariantStorage::Array>(&storage_->data);
}
VariantStorage::Array* Variant::array() noexcept {
    return std::get_if<VariantStorage::Array>(&storage_->data);
}
const VariantStorage::Object* Variant::object() const noexcept {
    return std::get_if<VariantStorage::Object>(&storage_->data);
}
VariantStorage::Object* Variant::object() noexcept {
    return std::get_if<VariantStorage::Object>(&storage_->data);
}

const DisplayHandle* Variant::displayHandle() const noexcept {
    return std::get_if<DisplayHandle>(&storage_->data);
}

DisplayHandle* Variant::displayHandle() noexcept {
    return std::get_if<DisplayHandle>(&storage_->data);
}

const AssetHandle* Variant::assetHandle() const noexcept {
    return std::get_if<AssetHandle>(&storage_->data);
}

AssetHandle* Variant::assetHandle() noexcept {
    return std::get_if<AssetHandle>(&storage_->data);
}

size_t Variant::size() const noexcept {
    if (auto a = array())  return a->size();
    if (auto o = object()) return o->size();
    return 0;
}

void Variant::push(Variant v) {
    if (!isArray()) storage_->data = VariantStorage::Array{};
    auto &a = std::get<VariantStorage::Array>(storage_->data);
    a.push_back(std::make_shared<Variant>(std::move(v)));
}

const Variant* Variant::at(size_t i) const noexcept {
    if (auto a = array()) {
        if (i < a->size() && (*a)[i]) return (*a)[i].get();
    }
    return nullptr;
}
Variant* Variant::at(size_t i) noexcept {
    if (auto a = array()) {
        if (i < a->size() && (*a)[i]) return (*a)[i].get();
    }
    return nullptr;
}

void Variant::set(const std::string& key, Variant v) {
    if (!isObject()) storage_->data = VariantStorage::Object{};
    auto& obj = std::get<VariantStorage::Object>(storage_->data);
    obj.insert_or_assign(key, std::make_shared<Variant>(std::move(v)));
}

const Variant* Variant::get(const std::string& key) const noexcept {
    if (auto o = object()) {
        auto it = o->find(key);
        if (it != o->end() && it->second) return it->second.get();
    }
    return nullptr;
}
Variant* Variant::get(const std::string& key) noexcept {
    if (auto o = object()) {
        auto it = o->find(key);
        if (it != o->end() && it->second) return it->second.get();
    }
    return nullptr;
}

bool Variant::getPath(const PathView& path, Variant& out) const {
    return getPathImpl(*this, path, out, nullptr);
}

bool Variant::getPath(const std::string& path, Variant& out) const {
    std::string err;
    PathView pv = parsePath(path, &err);
    if (pv.empty()) {
        SDOM::CoreAPI::setErrorMessage(err.c_str());
        return false;
    }
    std::string traversalErr;
    if (!getPathImpl(*this, pv, out, &traversalErr)) {
        SDOM::CoreAPI::setErrorMessage(traversalErr.c_str());
        return false;
    }
    return true;
}

Variant Variant::getPathOr(const std::string& path, const Variant& fallback) const {
    Variant out;
    if (getPath(path, out)) return out;
    return fallback;
}

bool Variant::pathExists(const PathView& path) const {
    if (path.empty()) return false;
    const Variant* cur = this;
    for (const auto& elem : path.elements()) {
        if (elem.kind == PathElement::Kind::Key) {
            if (!cur->isObject()) return false;
            const auto* obj = cur->object();
            if (!obj) return false;
            auto it = obj->find(elem.key);
            if (it == obj->end() || !it->second) return false;
            cur = it->second.get();
        } else {
            if (!cur->isArray()) return false;
            const auto* arr = cur->array();
            if (!arr || elem.index < 0 || static_cast<size_t>(elem.index) >= arr->size()) return false;
            const auto& ptr = (*arr)[static_cast<size_t>(elem.index)];
            if (!ptr) return false;
            cur = ptr.get();
        }
    }
    return true;
}

bool Variant::pathExists(const std::string& path) const {
    std::string err;
    PathView pv = parsePath(path, &err);
    if (pv.empty()) return false;
    return pathExists(pv);
}

bool Variant::setPath(const PathView& path, const Variant& value, const PathOptions& opts) {
    if (path.empty()) return false;
    Variant* cur = this;
    const auto& elems = path.elements();

    for (size_t i = 0; i < elems.size(); ++i) {
        const bool last = (i + 1 == elems.size());
        const auto& elem = elems[i];

        if (elem.kind == PathElement::Kind::Key) {
            if (!cur->isObject()) {
                if (!opts.create_intermediates) return false;
                cur->storage_->data = VariantStorage::Object{};
            }
            auto* obj = cur->object();
            if (last) {
                (*obj)[elem.key] = std::make_shared<Variant>(value);
                return true;
            }

            auto it = obj->find(elem.key);
            if (it == obj->end() || !it->second) {
                if (!opts.create_intermediates) return false;
                Variant next = (elems[i + 1].kind == PathElement::Kind::Key) ? Variant::makeObject() : Variant::makeArray();
                auto sp = std::make_shared<Variant>(std::move(next));
                (*obj)[elem.key] = sp;
                cur = sp.get();
            } else {
                cur = it->second.get();
            }
        } else {
            if (!cur->isArray()) {
                if (!opts.create_intermediates) return false;
                cur->storage_->data = VariantStorage::Array{};
            }
            auto* arr = cur->array();
            if (!arr || elem.index < 0) return false;
            size_t idx = static_cast<size_t>(elem.index);
            if (idx >= arr->size()) {
                if (!(opts.create_intermediates && opts.extend_arrays_with_null)) return false;
                while (arr->size() <= idx) arr->push_back(std::make_shared<Variant>(Variant()));
            }
            if (last) {
                (*arr)[idx] = std::make_shared<Variant>(value);
                return true;
            }
            if (!(*arr)[idx]) {
                if (!opts.create_intermediates) return false;
                Variant next = (elems[i + 1].kind == PathElement::Kind::Key) ? Variant::makeObject() : Variant::makeArray();
                (*arr)[idx] = std::make_shared<Variant>(std::move(next));
            }
            cur = (*arr)[idx].get();
        }
    }

    return false;
}

bool Variant::setPath(const std::string& path, const Variant& value, const PathOptions& opts) {
    std::string err;
    PathView pv = parsePath(path, &err);
    if (pv.empty()) {
        SDOM::CoreAPI::setErrorMessage(err.c_str());
        return false;
    }
    return setPath(pv, value, opts);
}

bool Variant::erasePath(const PathView& path) {
    if (path.empty()) return false;
    Variant* cur = this;
    const auto& elems = path.elements();

    for (size_t i = 0; i < elems.size(); ++i) {
        const bool last = (i + 1 == elems.size());
        const auto& elem = elems[i];

        if (elem.kind == PathElement::Kind::Key) {
            if (!cur->isObject()) return false;
            auto* obj = cur->object();
            if (last) {
                return obj && obj->erase(elem.key) > 0;
            }
            auto it = obj ? obj->find(elem.key) : obj->end();
            if (it == obj->end() || !it->second) return false;
            cur = it->second.get();
        } else {
            if (!cur->isArray()) return false;
            auto* arr = cur->array();
            if (!arr || elem.index < 0 || static_cast<size_t>(elem.index) >= arr->size()) return false;
            size_t idx = static_cast<size_t>(elem.index);
            if (last) {
                arr->erase(arr->begin() + static_cast<std::ptrdiff_t>(idx));
                return true;
            }
            const auto& ptr = (*arr)[idx];
            if (!ptr) return false;
            cur = ptr.get();
        }
    }

    return false;
}

bool Variant::erasePath(const std::string& path) {
    std::string err;
    PathView pv = parsePath(path, &err);
    if (pv.empty()) {
        SDOM::CoreAPI::setErrorMessage(err.c_str());
        return false;
    }
    return erasePath(pv);
}

// JSON integration
nlohmann::json Variant::toJson() const {
    const auto& d = storage_->data;
    if (std::holds_alternative<std::monostate>(d)) return nlohmann::json(nullptr);
    if (auto p = std::get_if<bool>(&d))            return *p;
    if (auto p = std::get_if<int64_t>(&d))         return *p;
    if (auto p = std::get_if<double>(&d))          return *p;
    if (auto p = std::get_if<std::string>(&d))     return *p;
    if (auto p = std::get_if<VariantStorage::Array>(&d)) {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& elem : *p) {
            if (!elem) { arr.push_back(nullptr); continue; }
            arr.push_back(elem->toJson());
        }
        return arr;
    }
    if (auto p = std::get_if<VariantStorage::Object>(&d)) {
        nlohmann::json obj = nlohmann::json::object();
        for (const auto& [k, v] : *p) {
            if (!v) { obj[k] = nullptr; continue; }
            obj[k] = v->toJson();
        }
        return obj;
    }
    if (auto p = std::get_if<DisplayHandle>(&d)) {
        nlohmann::json obj = nlohmann::json::object();
        obj["name"] = p->getName();
        obj["type"] = p->getType();
        return obj;
    }
    if (auto p = std::get_if<AssetHandle>(&d)) {
        nlohmann::json obj = nlohmann::json::object();
        obj["name"] = p->getName();
        obj["type"] = p->getType();
        obj["filename"] = p->getFilename();
        return obj;
    }
    if (auto p = std::get_if<VariantStorage::DynamicValue>(&d)) {
        auto conv = Variant::getConverter(p->type);
        if (conv && conv->toJson) {
            try {
                return conv->toJson(*p);
            } catch(...) { /* fallthrough to null */ }
        }
    }
    return nlohmann::json(nullptr);
}

Variant Variant::fromJson(const nlohmann::json& j) {
    if (j.is_null()) return Variant();
    if (j.is_boolean()) return Variant(j.get<bool>());
    if (j.is_number_integer()) return Variant(j.get<int64_t>());
    if (j.is_number_unsigned()) {
        auto u = j.get<uint64_t>();
        if (u <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
            return Variant(static_cast<int64_t>(u));
        }
        return Variant(static_cast<double>(u));
    }
    if (j.is_number_float()) return Variant(j.get<double>());
    if (j.is_string()) return Variant(j.get<std::string>());
    if (j.is_array()) {
        Variant v = Variant::makeArray();
        auto* arr = v.array();
        arr->reserve(j.size());
        for (const auto& elem : j) {
            arr->push_back(std::make_shared<Variant>(Variant::fromJson(elem)));
        }
        return v;
    }
    if (j.is_object()) {
        Variant v = Variant::makeObject();
        auto* obj = v.object();
        for (auto it = j.begin(); it != j.end(); ++it) {
            (*obj)[it.key()] = std::make_shared<Variant>(Variant::fromJson(it.value()));
        }
        return v;
    }

    return Variant();
}


// toDebugString implementation — depth-limited pretty printer for debugging.
std::string Variant::toDebugString(int depth) const {
    if (depth < 0) depth = 0;
    if (depth == 0) return "...";
    switch (type()) {
        case VariantType::Null: return "null";
        case VariantType::Bool: return toBool() ? "true" : "false";
        case VariantType::Int: return std::to_string(toInt64());
        case VariantType::Real: return std::to_string(toDouble());
        case VariantType::String: return std::string("\"") + toString() + "\"";
        case VariantType::Array: {
            const auto* a = array();
            if (!a) return "[]";
            std::string s = "[";
            bool first = true;
            for (const auto &e : *a) {
                if (!e) continue;
                if (!first) s += ", ";
                s += e->toDebugString(depth - 1);
                first = false;
            }
            s += "]";
            return s;
        }
        case VariantType::Object: {
            const auto* o = object();
            if (!o) return "{}";
            std::string s = "{";
            bool first = true;
            for (const auto &kv : *o) {
                if (!kv.second) continue;
                if (!first) s += ", ";
                s += kv.first + ": " + kv.second->toDebugString(depth - 1);
                first = false;
            }
            s += "}";
            return s;
        }
        case VariantType::DisplayHandle: {
            if (const auto* h = displayHandle()) return h->str();
            return "<display-handle-null>";
        }
        case VariantType::AssetHandle: {
            if (const auto* h = assetHandle()) {
                std::string s = h->getName();
                if (!h->getType().empty()) s += " (" + h->getType() + ")";
                if (!h->getFilename().empty()) s += " @" + h->getFilename();
                return s;
            }
            return "<asset-handle-null>";
        }
        case VariantType::Dynamic: return "<dynamic>";
        default: return "<error>";
    }
}

// Operators
bool Variant::numericEqual_(const Variant& a, const Variant& b) {
    double ad, bd;
    ad = a.toDouble(std::numeric_limits<double>::quiet_NaN());
    bd = b.toDouble(std::numeric_limits<double>::quiet_NaN());
    if (std::isnan(ad) || std::isnan(bd)) return false;
    return ad == bd;
}

bool Variant::operator==(const Variant& rhs) const {
    // error state equality
    if (errorFlag_ != rhs.errorFlag_) return false;
    if (errorFlag_ && rhs.errorFlag_) return errorMsg_ == rhs.errorMsg_;

    const auto& a = storage_->data;
    const auto& b = rhs.storage_->data;

    // numeric cross-type equality
    const bool a_is_num = std::holds_alternative<int64_t>(a) || std::holds_alternative<double>(a);
    const bool b_is_num = std::holds_alternative<int64_t>(b) || std::holds_alternative<double>(b);
    if (a_is_num && b_is_num) {
        return numericEqual_(*this, rhs);
    }

    // same-alternative direct compare. For Array/Object we need deep
    // comparison since the container now holds shared_ptr<Variant>.
    if (a.index() == b.index()) {
        if (std::holds_alternative<VariantStorage::Array>(a)) {
            const auto &la = std::get<VariantStorage::Array>(a);
            const auto &lb = std::get<VariantStorage::Array>(b);
            if (la.size() != lb.size()) return false;
            for (size_t i = 0; i < la.size(); ++i) {
                const auto &le = la[i];
                const auto &re = lb[i];
                if (le && re) {
                    if (!(*le == *re)) return false;
                } else if (le || re) {
                    return false; // one is null
                }
            }
            return true;
        }
        if (std::holds_alternative<VariantStorage::Object>(a)) {
            const auto &la = std::get<VariantStorage::Object>(a);
            const auto &lb = std::get<VariantStorage::Object>(b);
            if (la.size() != lb.size()) return false;
            for (const auto &kv : la) {
                auto it = lb.find(kv.first);
                if (it == lb.end()) return false;
                const auto &lv = kv.second;
                const auto &rv = it->second;
                if (lv && rv) {
                    if (!(*lv == *rv)) return false;
                } else if (lv || rv) {
                    return false;
                }
            }
            return true;
        }
        // fallback to std::visit for remaining types
        return std::visit([](const auto& lhs, const auto& rhs)->bool {
            using L = std::decay_t<decltype(lhs)>;
            using R = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<L,R>) {
                return lhs == rhs;
            } else {
                return false;
            }
        }, a, b);
    }

    return false;
}

PathView parsePath(std::string_view path, std::string* outErrorMessage) {
    if (outErrorMessage) outErrorMessage->clear();

    auto fail = [&](std::string msg) {
        if (outErrorMessage) *outErrorMessage = std::move(msg);
        return PathView();
    };

    if (path.empty()) return fail("path is empty");

    std::vector<PathElement> elems;
    size_t i = 0;

    auto is_ident_start = [](char c) {
        const unsigned char uc = static_cast<unsigned char>(c);
        return std::isalpha(uc) || c == '_';
    };

    auto is_ident_body = [](char c) {
        const unsigned char uc = static_cast<unsigned char>(c);
        return std::isalnum(uc) || c == '_';
    };

    while (i < path.size()) {
        const char ch = path[i];

        if (ch == '.') {
            return fail("empty path segment");
        }

        if (ch == '[') {
            if (elems.empty()) return fail("index cannot appear first");
            ++i;
            if (i >= path.size()) return fail("unterminated index");

            const size_t num_start = i;
            while (i < path.size() && std::isdigit(static_cast<unsigned char>(path[i]))) {
                ++i;
            }
            if (num_start == i) return fail("index missing digits");
            if (i >= path.size() || path[i] != ']') return fail("missing closing bracket");

            const std::string num_str(path.substr(num_start, i - num_start));
            int64_t idx = 0;
            try {
                idx = std::stoll(num_str);
            } catch (...) {
                return fail("index is not a valid integer");
            }
            if (idx < 0) return fail("negative indices are not supported");

            elems.push_back(PathElement{PathElement::Kind::Index, {}, idx});
            ++i; // consume ']'

            if (i == path.size()) break;
            if (path[i] == '.') {
                ++i;
                if (i == path.size()) return fail("trailing dot");
                continue;
            }

            // allow chained indices like arr[0][1]
            continue;
        }

        if (!is_ident_start(ch)) return fail("invalid identifier start");

        const size_t start = i;
        ++i;
        while (i < path.size() && is_ident_body(path[i])) ++i;

        elems.push_back(PathElement{PathElement::Kind::Key, std::string(path.substr(start, i - start)), 0});

        if (i == path.size()) break;

        if (path[i] == '.') {
            ++i;
            if (i == path.size()) return fail("trailing dot");
            continue;
        }

        if (path[i] == '[') {
            continue; // index handled in next iteration
        }

        return fail("unexpected character in path");
    }

    if (elems.empty()) return fail("path is empty");
    return PathView(std::move(elems));
}

} // namespace SDOM
