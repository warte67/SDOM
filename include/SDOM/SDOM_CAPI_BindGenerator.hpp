#pragma once
#include <functional>
#include <iosfwd>
#include <string>
#include <unordered_map>

#include <SDOM/SDOM_IBindGenerator.hpp>
#include <SDOM/SDOM_SubjectBinding.hpp>

namespace SDOM 
{

    class CAPI_BindGenerator : public IBindGenerator 
    {
    public:
        using IBindGenerator::BindModule;

        const char* name() const override { return "CAPI_BindGenerator"; }

        using CustomEmitter = std::function<void(std::ofstream&,
                                                 const FunctionInfo&,
                                                 const BindModule&,
                                                 const SubjectTypeDescriptor*)>;

        static void registerCustomEmitter(const std::string& subjectKind,
                                          CustomEmitter emitter);

        // Unified signature
        bool generate(const std::unordered_map<std::string, TypeInfo>& types,
                      const DataRegistry& registry) override;

    private:
        void generateHeader(const BindModule& module);
        void generateSource(const BindModule& module);
        void emitBindingManifest(const BindingManifest& manifest) const;

        const SubjectTypeDescriptor* descriptorFor(const std::string& typeName) const;
        void emitMethodTableFunction(std::ofstream& out,
                                     const FunctionInfo& fn,
                                     const BindModule& module,
                                     const SubjectTypeDescriptor* descriptor) const;
        void emitSingletonFunction(std::ofstream& out,
                                   const FunctionInfo& fn,
                                   const BindModule& module,
                                   const SubjectTypeDescriptor* descriptor) const;
        void emitEventRouterFunction(std::ofstream& out,
                                     const FunctionInfo& fn,
                                     const BindModule& module,
                                     const SubjectTypeDescriptor* descriptor) const;
        void emitCustomFunction(std::ofstream& out,
                                const FunctionInfo& fn,
                                const BindModule& module,
                                const SubjectTypeDescriptor* descriptor) const;
        void emitLegacyCallableBody(std::ofstream& out,
                                    const FunctionInfo& fn,
                                    const BindModule& module) const;

        void emitEnums(std::ofstream& out, const BindModule& module) const;
        void emitStructs(std::ofstream& out, const BindModule& module) const;
        void emitFunctionPrototypes(std::ofstream& out, const BindModule& module) const;
        void emitFunctionBodies(std::ofstream& out, const BindModule& module) const;
        static bool moduleNeedsCstdint(const BindModule& module);

        static void forEachCallableType(const BindModule& module,
                                        const std::function<void(const TypeInfo*)>& fn);

        static std::string headerPathFor(const std::string& dir, const std::string& stem);
        static std::string sourcePathFor(const std::string& dir, const std::string& stem);

        static bool isEnumReturnType(const std::string& normalizedType, const BindModule& module);

        BindingManifest latest_manifest_;
        std::unordered_map<std::string, SubjectTypeDescriptor> type_descriptors_;
    };

} // namespace SDOM
