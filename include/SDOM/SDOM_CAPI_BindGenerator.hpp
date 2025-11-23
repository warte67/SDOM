#pragma once
#include <functional>
#include <iosfwd>
#include <SDOM/SDOM_IBindGenerator.hpp>
#include <string>

namespace SDOM 
{

    class CAPI_BindGenerator : public IBindGenerator 
    {
    public:
        const char* name() const override { return "CAPI_BindGenerator"; }

        // Unified signature
        bool generate(const std::unordered_map<std::string, TypeInfo>& types,
                      const DataRegistry& registry) override;

    private:
        void generateHeader(const BindModule& module);
        void generateSource(const BindModule& module);

        void emitEnums(std::ofstream& out, const BindModule& module) const;
        void emitStructs(std::ofstream& out, const BindModule& module) const;
        void emitFunctionPrototypes(std::ofstream& out, const BindModule& module) const;
        void emitFunctionBodies(std::ofstream& out, const BindModule& module) const;

        static void forEachCallableType(const BindModule& module,
                                        const std::function<void(const TypeInfo*)>& fn);

        static std::string headerPathFor(const std::string& dir, const std::string& stem);
        static std::string sourcePathFor(const std::string& dir, const std::string& stem);

        static bool isEnumReturnType(const std::string& normalizedType, const BindModule& module);
    };

} // namespace SDOM
