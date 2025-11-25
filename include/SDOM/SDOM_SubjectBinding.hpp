#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace SDOM {

struct TypeInfo;
struct FunctionInfo;

enum class SubjectDispatchFamily : std::uint8_t {
    MethodTable = 0,
    Singleton,
    EventRouter,
    Custom
};

struct SubjectKindDescriptor {
    std::string name;
    SubjectDispatchFamily dispatch_family = SubjectDispatchFamily::MethodTable;
    bool uses_handle = true;
    bool owns_method_table = true;
};

struct SubjectTypeDescriptor {
    std::string name;
    std::string cpp_type_id;
    std::string file_stem;
    std::string subject_kind;
    SubjectDispatchFamily dispatch_family = SubjectDispatchFamily::MethodTable;
    bool uses_handle = true;
    bool owns_method_table = true;
    bool has_function_exports = false;
    const TypeInfo* type = nullptr;
};

struct FunctionBindingDescriptor {
    std::string name;
    std::string c_name;
    std::string c_signature;
    std::string owner_type_name;
    std::string subject_kind;
    SubjectDispatchFamily dispatch_family = SubjectDispatchFamily::MethodTable;
    const TypeInfo* owner = nullptr;
    const FunctionInfo* function = nullptr;
};

struct BindingManifest {
    std::vector<SubjectKindDescriptor> subject_kinds;
    std::vector<SubjectTypeDescriptor> type_bindings;
    std::vector<FunctionBindingDescriptor> functions;
};

const SubjectKindDescriptor& lookupSubjectKindDescriptor(const std::string& name);
SubjectDispatchFamily dispatchFamilyFromString(const std::string& label);
const char* dispatchFamilyToString(SubjectDispatchFamily family);

BindingManifest buildBindingManifest(const std::unordered_map<std::string, TypeInfo>& types);
std::string manifestToJson(const BindingManifest& manifest, int indent = 2);

} // namespace SDOM
