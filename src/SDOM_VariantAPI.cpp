#include <SDOM/SDOM_VariantAPI.hpp>

#include <SDOM/CAPI/SDOM_CAPI_Variant.h>
#include <SDOM/SDOM_CoreAPI.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_Variant.hpp>

#include <array>
#include <new>
#include <utility>

namespace SDOM {
namespace VariantAPI {
namespace {

struct VariantHandle {
    Variant value;
};

VariantHandle* getHandle(const SDOM_Variant* variant, const char* errCtx)
{
    if (!variant || !variant->impl) {
        if (errCtx) {
            SDOM::CoreAPI::setErrorMessage(errCtx);
        }
        return nullptr;
    }
    return static_cast<VariantHandle*>(variant->impl);
}

VariantHandle* ensureHandle(SDOM_Variant* variant, const char* errCtx)
{
    if (!variant) {
        SDOM::CoreAPI::setErrorMessage(errCtx ? errCtx : "SDOM_Variant pointer is null");
        return nullptr;
    }

    if (!variant->impl) {
        auto* handle = new (std::nothrow) VariantHandle();
        if (!handle) {
            SDOM::CoreAPI::setErrorMessage("Failed to allocate Variant handle");
            return nullptr;
        }
        variant->impl = handle;
    }

    return static_cast<VariantHandle*>(variant->impl);
}

bool assignValue(SDOM_Variant* variant, Variant value, const char* errCtx)
{
    auto* handle = ensureHandle(variant, errCtx);
    if (!handle) {
        return false;
    }

    handle->value = std::move(value);
    return true;
}

} // namespace

bool initNull(SDOM_Variant* variant)
{
    return assignValue(variant, Variant(), "SDOM_Variant_InitNull: variant is null");
}

bool initBool(SDOM_Variant* variant, bool value)
{
    return assignValue(variant, Variant(value), "SDOM_Variant_InitBool: variant is null");
}

bool initInt(SDOM_Variant* variant, std::int64_t value)
{
    return assignValue(variant, Variant(value), "SDOM_Variant_InitInt: variant is null");
}

bool initDouble(SDOM_Variant* variant, double value)
{
    return assignValue(variant, Variant(value), "SDOM_Variant_InitDouble: variant is null");
}

bool initString(SDOM_Variant* variant, const char* value)
{
    return assignValue(variant, Variant(value ? value : ""), "SDOM_Variant_InitString: variant is null");
}

bool copy(const SDOM_Variant* source, SDOM_Variant* destination)
{
    auto* srcHandle = getHandle(source, "SDOM_Variant_Copy: source is null");
    if (!srcHandle) {
        return false;
    }
    return assignValue(destination, srcHandle->value, "SDOM_Variant_Copy: destination is null");
}

bool destroy(SDOM_Variant* variant)
{
    if (!variant) {
        SDOM::CoreAPI::setErrorMessage("SDOM_Variant_Destroy: variant is null");
        return false;
    }

    if (variant->impl) {
        auto* handle = static_cast<VariantHandle*>(variant->impl);
        delete handle;
        variant->impl = nullptr;
    }

    return true;
}

VariantType getType(const SDOM_Variant* variant)
{
    auto* handle = getHandle(variant, "SDOM_Variant_GetType: variant is null");
    if (!handle) {
        return VariantType::Error;
    }
    return handle->value.type();
}

bool toBool(const SDOM_Variant* variant, bool* outValue)
{
    if (!outValue) {
        SDOM::CoreAPI::setErrorMessage("SDOM_Variant_ToBool: outValue is null");
        return false;
    }

    auto* handle = getHandle(variant, "SDOM_Variant_ToBool: variant is null");
    if (!handle) {
        return false;
    }

    *outValue = handle->value.toBool();
    return true;
}

bool toInt64(const SDOM_Variant* variant, std::int64_t* outValue)
{
    if (!outValue) {
        SDOM::CoreAPI::setErrorMessage("SDOM_Variant_ToInt64: outValue is null");
        return false;
    }

    auto* handle = getHandle(variant, "SDOM_Variant_ToInt64: variant is null");
    if (!handle) {
        return false;
    }

    *outValue = handle->value.toInt64();
    return true;
}

bool toDouble(const SDOM_Variant* variant, double* outValue)
{
    if (!outValue) {
        SDOM::CoreAPI::setErrorMessage("SDOM_Variant_ToDouble: outValue is null");
        return false;
    }

    auto* handle = getHandle(variant, "SDOM_Variant_ToDouble: variant is null");
    if (!handle) {
        return false;
    }

    *outValue = handle->value.toDouble();
    return true;
}

std::string toString(const SDOM_Variant* variant)
{
    auto* handle = getHandle(variant, "SDOM_Variant_ToString: variant is null");
    if (!handle) {
        return {};
    }

    return handle->value.toString();
}

void registerTypes(DataRegistry& registry)
{
    const char* moduleType = "VariantAPI";
    if (!registry.lookupType(moduleType)) {
        SDOM::TypeInfo apiInfo;
        apiInfo.name        = moduleType;
        apiInfo.kind        = SDOM::EntryKind::Global;
        apiInfo.cpp_type_id = "SDOM::VariantAPI";
        apiInfo.file_stem   = "Variant";
        apiInfo.export_name = "SDOM_Variant";
        apiInfo.category    = "Variant";
        apiInfo.doc         = "C API helpers for SDOM::Variant value marshaling.";
        apiInfo.subject_kind = "Variant";
        apiInfo.subject_uses_handle = false;
        apiInfo.has_handle_override = true;
        apiInfo.dispatch_family_override = "singleton";
        apiInfo.module_brief = "Variant value marshaling utilities for the SDOM C API.";
        apiInfo.module_brief_explicit = true;
        registry.registerType(apiInfo);
    }

    if (!registry.lookupType("SDOM_Variant")) {
        SDOM::TypeInfo structInfo;
        structInfo.name        = "SDOM_Variant";
        structInfo.kind        = SDOM::EntryKind::Struct;
        structInfo.cpp_type_id = "SDOM::Variant";
        structInfo.file_stem   = "Variant";
        structInfo.export_name = "SDOM_Variant";
        structInfo.category    = "Variant";
        structInfo.doc         = "Opaque ABI struct describing an SDOM::Variant value.";

        SDOM::PropertyInfo ptrField;
        ptrField.name      = "impl";
        ptrField.cpp_type  = "void*";
        ptrField.read_only = false;
        ptrField.doc       = "Opaque pointer to internal Variant storage.";
        structInfo.properties.push_back(std::move(ptrField));

        registry.registerType(structInfo);
    }

    struct EnumDesc {
        VariantType value;
        const char* name;
        const char* doc;
    };

    constexpr std::array<EnumDesc, 10> kVariantTypes = {{
        { VariantType::Null,   "Null",   "Represents an explicit null." },
        { VariantType::Bool,   "Bool",   "Boolean value." },
        { VariantType::Int,    "Int",    "Signed 64-bit integer." },
        { VariantType::Real,   "Real",   "Double-precision floating point." },
        { VariantType::String, "String", "UTF-8 encoded string." },
        { VariantType::Array,  "Array",  "Ordered array of Variant values." },
        { VariantType::Object, "Object", "Key/value object of string keys to Variant values." },
        { VariantType::Dynamic,"Dynamic","Shared pointer to an arbitrary registered type." },
        { VariantType::LuaRef, "LuaRef", "Opaque reference to a Lua object." },
        { VariantType::Error,  "Error",  "Represents a variant that recorded an error state." }
    }};

    for (const auto& entry : kVariantTypes) {
        const std::string typeName = std::string("SDOM_VariantType::") + entry.name;
        if (registry.lookupType(typeName)) {
            continue;
        }

        SDOM::TypeInfo enumInfo;
        enumInfo.name        = typeName;
        enumInfo.kind        = SDOM::EntryKind::Enum;
        enumInfo.cpp_type_id = entry.name;
        enumInfo.file_stem   = "Variant";
        enumInfo.export_name = "SDOM_VariantType";
        enumInfo.enum_value  = static_cast<std::uint32_t>(entry.value);
        enumInfo.category    = "Variant";
        enumInfo.doc         = entry.doc;
        registry.registerType(enumInfo);
    }

    auto registerFunction = [&](const char* name,
                                const char* cppSig,
                                const char* returnType,
                                const char* cName,
                                const char* cSig,
                                const char* doc,
                                auto&& fn) {
        SDOM::FunctionInfo info;
        info.name = name;
        info.cpp_signature = cppSig;
        info.return_type = returnType;
        info.c_name = cName;
        info.c_signature = cSig;
        info.doc = doc;
        registry.registerFunction(moduleType, info, std::forward<decltype(fn)>(fn));
    };

    registerFunction(
        "InitNull",
        "bool VariantAPI::initNull(SDOM_Variant*)",
        "bool",
        "SDOM_Variant_InitNull",
        "bool SDOM_Variant_InitNull(SDOM_Variant* variant)",
        "Initializes a variant to the null state.",
        &initNull);

    registerFunction(
        "InitBool",
        "bool VariantAPI::initBool(SDOM_Variant*, bool)",
        "bool",
        "SDOM_Variant_InitBool",
        "bool SDOM_Variant_InitBool(SDOM_Variant* variant, bool value)",
        "Initializes a variant with the specified boolean value.",
        &initBool);

    registerFunction(
        "InitInt",
        "bool VariantAPI::initInt(SDOM_Variant*, std::int64_t)",
        "bool",
        "SDOM_Variant_InitInt",
        "bool SDOM_Variant_InitInt(SDOM_Variant* variant, int64_t value)",
        "Initializes a variant with the provided 64-bit integer.",
        &initInt);

    registerFunction(
        "InitDouble",
        "bool VariantAPI::initDouble(SDOM_Variant*, double)",
        "bool",
        "SDOM_Variant_InitDouble",
        "bool SDOM_Variant_InitDouble(SDOM_Variant* variant, double value)",
        "Initializes a variant with the provided double value.",
        &initDouble);

    registerFunction(
        "InitString",
        "bool VariantAPI::initString(SDOM_Variant*, const char*)",
        "bool",
        "SDOM_Variant_InitString",
        "bool SDOM_Variant_InitString(SDOM_Variant* variant, const char* utf8)",
        "Initializes a variant with the provided UTF-8 string.",
        &initString);

    registerFunction(
        "Copy",
        "bool VariantAPI::copy(const SDOM_Variant*, SDOM_Variant*)",
        "bool",
        "SDOM_Variant_Copy",
        "bool SDOM_Variant_Copy(const SDOM_Variant* source, SDOM_Variant* destination)",
        "Copies the contents of one variant handle into another.",
        &copy);

    registerFunction(
        "Destroy",
        "bool VariantAPI::destroy(SDOM_Variant*)",
        "bool",
        "SDOM_Variant_Destroy",
        "bool SDOM_Variant_Destroy(SDOM_Variant* variant)",
        "Releases any resources held by the variant handle.",
        &destroy);

    registerFunction(
        "GetType",
        "VariantType VariantAPI::getType(const SDOM_Variant*)",
        "SDOM_VariantType",
        "SDOM_Variant_GetType",
        "SDOM_VariantType SDOM_Variant_GetType(const SDOM_Variant* variant)",
        "Returns the enumerated type tag for the variant.",
        &getType);

    registerFunction(
        "ToBool",
        "bool VariantAPI::toBool(const SDOM_Variant*, bool*)",
        "bool",
        "SDOM_Variant_ToBool",
        "bool SDOM_Variant_ToBool(const SDOM_Variant* variant, bool* out_value)",
        "Converts the variant to a boolean using SDOM conversion semantics.",
        &toBool);

    registerFunction(
        "ToInt64",
        "bool VariantAPI::toInt64(const SDOM_Variant*, std::int64_t*)",
        "bool",
        "SDOM_Variant_ToInt64",
        "bool SDOM_Variant_ToInt64(const SDOM_Variant* variant, int64_t* out_value)",
        "Converts the variant to a 64-bit integer using SDOM conversion semantics.",
        &toInt64);

    registerFunction(
        "ToDouble",
        "bool VariantAPI::toDouble(const SDOM_Variant*, double*)",
        "bool",
        "SDOM_Variant_ToDouble",
        "bool SDOM_Variant_ToDouble(const SDOM_Variant* variant, double* out_value)",
        "Converts the variant to a double using SDOM conversion semantics.",
        &toDouble);

    registerFunction(
        "ToString",
        "std::string VariantAPI::toString(const SDOM_Variant*)",
        "const char*",
        "SDOM_Variant_ToString",
        "const char* SDOM_Variant_ToString(const SDOM_Variant* variant)",
        "Converts the variant to its string representation.",
        &toString);
}

} // namespace VariantAPI
} // namespace SDOM
