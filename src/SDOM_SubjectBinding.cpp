#include <SDOM/SDOM_SubjectBinding.hpp>

#include <SDOM/SDOM_DataRegistry.hpp>
#include <json.hpp>

#include <algorithm>
#include <utility>

namespace SDOM {
namespace {

using json = nlohmann::json;

struct SubjectKindStorage {
    std::unordered_map<std::string, SubjectKindDescriptor> descriptors;

    SubjectKindStorage()
    {
        registerBuiltin("DisplayObject", SubjectDispatchFamily::MethodTable, true, true);
        registerBuiltin("AssetObject", SubjectDispatchFamily::MethodTable, true, true);
        registerBuiltin("Core", SubjectDispatchFamily::Singleton, false, true);
        registerBuiltin("Event", SubjectDispatchFamily::EventRouter, false, false);
        registerBuiltin("EventType", SubjectDispatchFamily::EventRouter, false, false);
    }

    void registerBuiltin(const std::string& name,
                         SubjectDispatchFamily family,
                         bool uses_handle,
                         bool owns_table)
    {
        SubjectKindDescriptor desc;
        desc.name = name;
        desc.dispatch_family = family;
        desc.uses_handle = uses_handle;
        desc.owns_method_table = owns_table;
        descriptors.emplace(name, std::move(desc));
    }
};

SubjectKindStorage& kindStorage()
{
    static SubjectKindStorage storage;
    return storage;
}

std::string deduceSubjectKindName(const TypeInfo& type)
{
    if (!type.subject_kind.empty()) {
        return type.subject_kind;
    }

    switch (type.kind) {
    case EntryKind::Object:
        return "DisplayObject";
    case EntryKind::Global:
        return "Core";
    default:
        return "DisplayObject";
    }
}

SubjectDispatchFamily applyDispatchOverride(const TypeInfo& type,
                                            const SubjectKindDescriptor& descriptor)
{
    if (type.dispatch_family_override.empty()) {
        return descriptor.dispatch_family;
    }
    return dispatchFamilyFromString(type.dispatch_family_override);
}

bool applyHandleOverride(const TypeInfo& type,
                         const SubjectKindDescriptor& descriptor)
{
    if (type.subject_kind.empty()) {
        return descriptor.uses_handle;
    }

    if (!type.has_handle_override) {
        return descriptor.uses_handle;
    }

    return type.subject_uses_handle;
}

json subjectKindToJson(const SubjectKindDescriptor& desc)
{
    json j;
    j["name"] = desc.name;
    j["dispatch_family"] = dispatchFamilyToString(desc.dispatch_family);
    j["uses_handle"] = desc.uses_handle;
    j["owns_method_table"] = desc.owns_method_table;
    return j;
}

json typeBindingToJson(const SubjectTypeDescriptor& type)
{
    json j;
    j["name"] = type.name;
    j["cpp_type_id"] = type.cpp_type_id;
    j["file_stem"] = type.file_stem;
    j["subject_kind"] = type.subject_kind;
    j["dispatch_family"] = dispatchFamilyToString(type.dispatch_family);
    j["uses_handle"] = type.uses_handle;
    j["owns_method_table"] = type.owns_method_table;
    j["has_function_exports"] = type.has_function_exports;
    return j;
}

json functionBindingToJson(const FunctionBindingDescriptor& fn)
{
    json j;
    j["name"] = fn.name;
    j["c_name"] = fn.c_name;
    j["c_signature"] = fn.c_signature;
    j["owner_type"] = fn.owner_type_name;
    j["subject_kind"] = fn.subject_kind;
    j["dispatch_family"] = dispatchFamilyToString(fn.dispatch_family);
    return j;
}

} // namespace

const SubjectKindDescriptor& lookupSubjectKindDescriptor(const std::string& name)
{
    auto& storage = kindStorage();
    const auto it = storage.descriptors.find(name);
    if (it != storage.descriptors.end()) {
        return it->second;
    }

    SubjectKindDescriptor desc;
    desc.name = name.empty() ? std::string("DisplayObject") : name;
    desc.dispatch_family = name.empty() ? SubjectDispatchFamily::MethodTable : SubjectDispatchFamily::Custom;
    desc.uses_handle = true;
    desc.owns_method_table = (desc.dispatch_family == SubjectDispatchFamily::MethodTable);

    const auto [inserted, _] = storage.descriptors.emplace(desc.name, std::move(desc));
    return inserted->second;
}

SubjectDispatchFamily dispatchFamilyFromString(const std::string& label)
{
    if (label == "method_table") {
        return SubjectDispatchFamily::MethodTable;
    }
    if (label == "singleton") {
        return SubjectDispatchFamily::Singleton;
    }
    if (label == "event_router") {
        return SubjectDispatchFamily::EventRouter;
    }
    return SubjectDispatchFamily::Custom;
}

const char* dispatchFamilyToString(SubjectDispatchFamily family)
{
    switch (family) {
    case SubjectDispatchFamily::MethodTable:
        return "method_table";
    case SubjectDispatchFamily::Singleton:
        return "singleton";
    case SubjectDispatchFamily::EventRouter:
        return "event_router";
    case SubjectDispatchFamily::Custom:
    default:
        return "custom";
    }
}

BindingManifest buildBindingManifest(const std::unordered_map<std::string, TypeInfo>& types)
{
    BindingManifest manifest;

    for (const auto& [_, desc] : kindStorage().descriptors) {
        manifest.subject_kinds.push_back(desc);
    }

    for (const auto& [_, type] : types) {
        const bool hasFunctions = std::any_of(type.functions.begin(), type.functions.end(), [](const FunctionInfo& fn) {
            return fn.exported && !fn.c_signature.empty();
        });

        const bool hasSubjectKind = !type.subject_kind.empty();
        if (!hasFunctions && type.kind != EntryKind::Global && !hasSubjectKind) {
            continue;
        }

        SubjectTypeDescriptor descriptor;
        descriptor.name = type.name;
        descriptor.cpp_type_id = type.cpp_type_id;
        descriptor.file_stem = type.file_stem;
        descriptor.type = &type;

        descriptor.subject_kind = deduceSubjectKindName(type);
        const SubjectKindDescriptor& kindDesc = lookupSubjectKindDescriptor(descriptor.subject_kind);
        descriptor.dispatch_family = applyDispatchOverride(type, kindDesc);
        descriptor.uses_handle = applyHandleOverride(type, kindDesc);
        descriptor.owns_method_table = kindDesc.owns_method_table;
        descriptor.has_function_exports = hasFunctions;

        manifest.type_bindings.push_back(descriptor);

        if (hasFunctions) {
            for (const auto& fn : type.functions) {
                if (!fn.exported || fn.c_signature.empty()) {
                    continue;
                }

                FunctionBindingDescriptor fnDesc;
                fnDesc.name = fn.name;
                fnDesc.c_name = fn.c_name;
                fnDesc.c_signature = fn.c_signature;
                fnDesc.owner_type_name = type.name;
                fnDesc.subject_kind = descriptor.subject_kind;
                fnDesc.dispatch_family = descriptor.dispatch_family;
                fnDesc.owner = &type;
                fnDesc.function = &fn;

                manifest.functions.push_back(std::move(fnDesc));
            }
        }
    }

    return manifest;
}

std::string manifestToJson(const BindingManifest& manifest, int indent)
{
    json root;
    root["manifest_version"] = 1;

    json kindArray = json::array();
    for (const auto& kind : manifest.subject_kinds) {
        kindArray.push_back(subjectKindToJson(kind));
    }
    root["subject_kinds"] = kindArray;

    json typeArray = json::array();
    for (const auto& type : manifest.type_bindings) {
        typeArray.push_back(typeBindingToJson(type));
    }
    root["types"] = typeArray;

    json fnArray = json::array();
    for (const auto& fn : manifest.functions) {
        fnArray.push_back(functionBindingToJson(fn));
    }
    root["functions"] = fnArray;

    return root.dump(indent);
}

} // namespace SDOM
