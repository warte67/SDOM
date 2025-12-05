#include <algorithm>

#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_IBindGenerator.hpp>

namespace SDOM {

IBindGenerator::BindModuleMap IBindGenerator::buildModuleMap(
    const std::unordered_map<std::string, TypeInfo>& types) const {
    BindModuleMap modules;

    for (const auto& [_, type] : types) {
        if (type.file_stem.empty()) {
            continue;
        }

        BindModule& module = modules[type.file_stem];
        module.file_stem = type.file_stem;

        if (!type.module_brief.empty()) {
            if (type.module_brief_explicit) {
                if (!module.module_brief_explicit) {
                    module.module_brief = type.module_brief;
                    module.module_brief_explicit = true;
                }
            } else if (!module.module_brief_explicit && module.module_brief.empty()) {
                module.module_brief = type.module_brief;
            }
        }

        const TypeInfo* type_ptr = &type;

        switch (type.kind) {
        case EntryKind::Enum:
            module.enums.push_back(type_ptr);
            break;
        case EntryKind::Struct:
            module.structs.push_back(type_ptr);
            break;
        case EntryKind::Object:
            module.objects.push_back(type_ptr);
            break;
        case EntryKind::Global:
            module.globals.push_back(type_ptr);
            break;
        case EntryKind::Function:
            module.functions.push_back(type_ptr);
            break;
        case EntryKind::Alias:
            module.aliases.push_back(type_ptr);
            break;
        default:
            module.objects.push_back(type_ptr);
            break;
        }
    }

    auto sort_bucket = [](auto& bucket) {
        std::sort(bucket.begin(), bucket.end(),
                  [](const TypeInfo* lhs, const TypeInfo* rhs) {
                      return lhs->name < rhs->name;
                  });
    };

    for (auto& [_, module] : modules) {
        sort_bucket(module.enums);
        sort_bucket(module.structs);
        sort_bucket(module.objects);
        sort_bucket(module.globals);
        sort_bucket(module.functions);
        sort_bucket(module.aliases);
    }

    return modules;
}

bool IBindGenerator::moduleHasFunctions(const BindModule& module) const {
    auto contains_functions = [](const std::vector<const TypeInfo*>& bucket) {
        for (const TypeInfo* type : bucket) {
            if (!type) {
                continue;
            }

            if (!type->functions.empty()) {
                return true;
            }
        }

        return false;
    };

    return contains_functions(module.objects) || contains_functions(module.structs) ||
           contains_functions(module.globals) || contains_functions(module.functions) ||
           contains_functions(module.aliases);
}

} // namespace SDOM
