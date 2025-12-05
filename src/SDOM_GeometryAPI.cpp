#include <SDOM/SDOM_GeometryAPI.hpp>

#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Utils.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>

namespace SDOM {
namespace GeometryAPI {
namespace {

constexpr const char* kDefaultAnchorString = "top_left";

const char* fallbackAnchorString()
{
    return kDefaultAnchorString;
}

} // namespace

AnchorPoint anchorPointFromString(const char* name)
{
    if (!name) {
        return AnchorPoint::TOP_LEFT;
    }

    std::string normalized = normalizeAnchorString(name);
    if (normalized.empty() || !validateAnchorPointString(normalized)) {
        return AnchorPoint::TOP_LEFT;
    }

    const auto it = stringToAnchorPoint_.find(normalized);
    if (it != stringToAnchorPoint_.end()) {
        return it->second;
    }

    return AnchorPoint::TOP_LEFT;
}

const char* anchorPointToString(AnchorPoint anchor)
{
    const auto it = anchorPointToString_.find(anchor);
    if (it != anchorPointToString_.end()) {
        return it->second.c_str();
    }
    return fallbackAnchorString();
}

bool setBounds(Bounds* bounds, float left, float top, float right, float bottom)
{
    if (!bounds) {
        return false;
    }

    bounds->left = left;
    bounds->top = top;
    bounds->right = right;
    bounds->bottom = bottom;
    return true;
}

float getBoundsWidth(const Bounds* bounds)
{
    if (!bounds) {
        return 0.0f;
    }
    return bounds->width();
}

float getBoundsHeight(const Bounds* bounds)
{
    if (!bounds) {
        return 0.0f;
    }
    return bounds->height();
}

void registerTypes(DataRegistry& registry)
{
    constexpr const char* moduleType = "GeometryAPI";
    if (!registry.lookupType(moduleType)) {
        SDOM::TypeInfo apiInfo;
        apiInfo.name        = moduleType;
        apiInfo.kind        = SDOM::EntryKind::Global;
        apiInfo.cpp_type_id = "SDOM::GeometryAPI";
        apiInfo.file_stem   = "Geometry";
        apiInfo.export_name = "SDOM_Geometry";
        apiInfo.category    = "Geometry";
        apiInfo.doc         = "Utility helpers for AnchorPoint enums and Bounds structs.";
        apiInfo.subject_kind = "Geometry";
        apiInfo.subject_uses_handle = false;
        apiInfo.has_handle_override = true;
        apiInfo.dispatch_family_override = "singleton";
        apiInfo.module_brief = "AnchorPoint and Bounds helpers for the SDOM C API.";
        apiInfo.module_brief_explicit = true;
        registry.registerType(apiInfo);
    }

    struct EnumDesc {
        AnchorPoint value;
        const char* name;
        const char* doc;
    };

    constexpr std::array<EnumDesc, 9> kAnchorPoints = {{
        { AnchorPoint::TOP_LEFT,      "TOP_LEFT",      "Anchor to the parent's top-left corner." },
        { AnchorPoint::TOP_CENTER,    "TOP_CENTER",    "Anchor to the midpoint of the parent's top edge." },
        { AnchorPoint::TOP_RIGHT,     "TOP_RIGHT",     "Anchor to the parent's top-right corner." },
        { AnchorPoint::MIDDLE_LEFT,   "MIDDLE_LEFT",   "Anchor to the midpoint of the parent's left edge." },
        { AnchorPoint::MIDDLE_CENTER, "MIDDLE_CENTER", "Anchor to the parent's center point." },
        { AnchorPoint::MIDDLE_RIGHT,  "MIDDLE_RIGHT",  "Anchor to the midpoint of the parent's right edge." },
        { AnchorPoint::BOTTOM_LEFT,   "BOTTOM_LEFT",   "Anchor to the parent's bottom-left corner." },
        { AnchorPoint::BOTTOM_CENTER, "BOTTOM_CENTER", "Anchor to the midpoint of the parent's bottom edge." },
        { AnchorPoint::BOTTOM_RIGHT,  "BOTTOM_RIGHT",  "Anchor to the parent's bottom-right corner." }
    }};

    for (const auto& entry : kAnchorPoints) {
        const std::string typeName = std::string("SDOM_AnchorPoint::") + entry.name;
        if (registry.lookupType(typeName)) {
            continue;
        }

        SDOM::TypeInfo enumInfo;
        enumInfo.name        = typeName;
        enumInfo.kind        = SDOM::EntryKind::Enum;
        enumInfo.cpp_type_id = entry.name;
        enumInfo.file_stem   = "Geometry";
        enumInfo.export_name = "SDOM_AnchorPoint";
        enumInfo.enum_value  = static_cast<std::uint32_t>(entry.value);
        enumInfo.category    = "Layout & Anchors";
        enumInfo.doc         = entry.doc;
        registry.registerType(enumInfo);
    }

    if (!registry.lookupType("SDOM_Bounds")) {
        SDOM::TypeInfo structInfo;
        structInfo.name        = "SDOM_Bounds";
        structInfo.kind        = SDOM::EntryKind::Struct;
        structInfo.cpp_type_id = "SDOM::Bounds";
        structInfo.file_stem   = "Geometry";
        structInfo.export_name = "SDOM_Bounds";
        structInfo.category    = "Layout & Anchors";
        structInfo.doc         = "Axis-aligned rectangle storing left/top/right/bottom edges.";

        auto addField = [&](const char* fieldName, const char* doc) {
            SDOM::PropertyInfo prop;
            prop.name      = fieldName;
            prop.cpp_type  = "float";
            prop.read_only = false;
            prop.doc       = doc;
            structInfo.properties.push_back(std::move(prop));
        };

        addField("left",   "Position of the left edge in parent space.");
        addField("top",    "Position of the top edge in parent space.");
        addField("right",  "Position of the right edge in parent space.");
        addField("bottom", "Position of the bottom edge in parent space.");

        registry.registerType(structInfo);
    }

    auto registerFunction = [&](const char* name,
                                const char* cppSig,
                                const char* returnType,
                                const char* cName,
                                const char* cSig,
                                const char* doc,
                                auto&& fn) {
        SDOM::FunctionInfo info;
        info.name          = name;
        info.cpp_signature = cppSig;
        info.return_type   = returnType;
        info.c_name        = cName;
        info.c_signature   = cSig;
        info.doc           = doc;
        registry.registerFunction(moduleType, info, std::forward<decltype(fn)>(fn));
    };

    registerFunction(
        "AnchorPointFromString",
        "SDOM::AnchorPoint GeometryAPI::anchorPointFromString(const char*)",
        "SDOM_AnchorPoint",
        "SDOM_Geometry_AnchorPointFromString",
        "SDOM_AnchorPoint SDOM_Geometry_AnchorPointFromString(const char* name)",
        "Parses the provided string (case-insensitive, supports alias separators) into an AnchorPoint enum.",
        &anchorPointFromString);

    registerFunction(
        "AnchorPointToString",
        "const char* GeometryAPI::anchorPointToString(SDOM::AnchorPoint)",
        "const char*",
        "SDOM_Geometry_AnchorPointToString",
        "const char* SDOM_Geometry_AnchorPointToString(SDOM_AnchorPoint anchor)",
        "Returns the canonical string name for the given AnchorPoint value.",
        &anchorPointToString);

    registerFunction(
        "SetBounds",
        "bool GeometryAPI::setBounds(SDOM::Bounds*, float, float, float, float)",
        "bool",
        "SDOM_Geometry_SetBounds",
        "bool SDOM_Geometry_SetBounds(SDOM_Bounds* bounds, float left, float top, float right, float bottom)",
        "Assigns the four edges of a Bounds struct.",
        &setBounds);

    registerFunction(
        "GetBoundsWidth",
        "float GeometryAPI::getBoundsWidth(const SDOM::Bounds*)",
        "float",
        "SDOM_Geometry_GetBoundsWidth",
        "float SDOM_Geometry_GetBoundsWidth(const SDOM_Bounds* bounds)",
        "Returns the absolute width of the bounds.",
        &getBoundsWidth);

    registerFunction(
        "GetBoundsHeight",
        "float GeometryAPI::getBoundsHeight(const SDOM::Bounds*)",
        "float",
        "SDOM_Geometry_GetBoundsHeight",
        "float SDOM_Geometry_GetBoundsHeight(const SDOM_Bounds* bounds)",
        "Returns the absolute height of the bounds.",
        &getBoundsHeight);
}

} // namespace GeometryAPI
} // namespace SDOM
