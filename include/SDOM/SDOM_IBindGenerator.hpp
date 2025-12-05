#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace SDOM {

struct TypeInfo;
class DataRegistry;

class IBindGenerator {
public:
    virtual ~IBindGenerator() = default;

    virtual const char* name() const = 0;

    // Set by DataRegistry before generation
    virtual void setOutputDirs(const std::string& headerDir,
                               const std::string& sourceDir)
    {
        header_dir_ = headerDir;
        source_dir_ = sourceDir;
    }

    // The unified, required signature
    virtual bool generate(const std::unordered_map<std::string, TypeInfo>& types,
                          const DataRegistry& registry) = 0;

protected:
    struct BindModule {
        std::string file_stem;
        std::vector<const TypeInfo*> enums;
        std::vector<const TypeInfo*> structs;
        std::vector<const TypeInfo*> objects;
        std::vector<const TypeInfo*> globals;
        std::vector<const TypeInfo*> functions;
        std::vector<const TypeInfo*> aliases;
        std::string module_brief;
        bool module_brief_explicit = false;

        bool empty() const {
            return enums.empty() && structs.empty() && objects.empty() && globals.empty() &&
                   functions.empty() && aliases.empty();
        }
    };

    using BindModuleMap = std::map<std::string, BindModule>;

    BindModuleMap buildModuleMap(const std::unordered_map<std::string, TypeInfo>& types) const;
    bool moduleHasFunctions(const BindModule& module) const;

    std::string header_dir_;
    std::string source_dir_;
};

} // namespace SDOM
