#pragma once
#ifndef __SDOM_DATAREGISTRY_HPP__
#define __SDOM_DATAREGISTRY_HPP__

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <any>
#include <functional>
#include <optional>
#include <memory>
#include <type_traits>
#include <utility>

namespace SDOM {

struct FunctionInfo {
    std::string name;
    std::string cpp_signature;
    std::vector<std::string> param_types;
    std::string return_type;
    std::string c_name; // optional explicit C symbol name
    std::string c_signature; // optional explicit C prototype
    std::string doc;
    bool is_static = false;
    bool exported = true;
    std::any callable; // optional type-erased callable
};

struct PropertyInfo {
    std::string name;
    std::string cpp_type;
    bool read_only = false;
    std::optional<std::string> default_value;
    std::string doc;
    std::any getter;
    std::any setter;
};

struct TypeInfo {
    std::string name;
    std::string cpp_type_id;
    std::vector<PropertyInfo> properties;
    std::vector<FunctionInfo> functions;
    std::vector<std::string> bases;
    std::string doc;
};

// Snapshot of registry metadata passed to generators. This is intentionally
// lightweight and immutable so generators can't accidentally call back into
// the live registry and cause deadlocks.
struct DataRegistrySnapshot {
    std::vector<TypeInfo> types;
};

class IBindingGenerator;

class DataRegistry {
public:
    DataRegistry();
    ~DataRegistry();

    // Non-copyable, non-movable to avoid accidental inline instantiation
    // of special members that would require a complete IBindingGenerator
    // type in translation units that include this header.
    DataRegistry(const DataRegistry&) = delete;
    DataRegistry& operator=(const DataRegistry&) = delete;
    DataRegistry(DataRegistry&&) = delete;
    DataRegistry& operator=(DataRegistry&&) = delete;

    // Meyers singleton for fallback during migration
    static DataRegistry& instance();

    // Type-level registration
    bool registerType(const TypeInfo& info);

    template<typename Fn>
    void registerFunction(const std::string& typeName, const FunctionInfo& meta, Fn&& fn)
    {
        std::scoped_lock lk(mutex_);
        FunctionInfo m = meta;
        m.callable = std::make_any<std::decay_t<Fn>>(std::forward<Fn>(fn));
        types_[typeName].functions.push_back(std::move(m));
    }

    template<typename Getter, typename Setter = std::nullptr_t>
    void registerProperty(const std::string& typeName, const PropertyInfo& meta, Getter&& getter, Setter&& setter = nullptr)
    {
        std::scoped_lock lk(mutex_);
        PropertyInfo p = meta;
        p.getter = std::make_any<std::decay_t<Getter>>(std::forward<Getter>(getter));
        if constexpr (!std::is_same_v<std::decay_t<Setter>, std::nullptr_t>) {
            p.setter = std::make_any<std::decay_t<Setter>>(std::forward<Setter>(setter));
        }
        types_[typeName].properties.push_back(std::move(p));
    }

    // Query
    const TypeInfo* lookupType(const std::string& name) const;
    std::vector<std::string> listTypes() const;

        // Generator integration point (phase-2): placeholder API will be
        // implemented once the IBindingGenerator interface is defined.
        bool generateBindings(const std::string& outDir);

        // Register a custom binding generator. Ownership is transferred into
        // the registry so generators can be invoked later by generateBindings().
        void addGenerator(std::unique_ptr<IBindingGenerator> generator);

    const std::unordered_map<std::string, TypeInfo>& getAllTypes() const { return types_; }

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, TypeInfo> types_;
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace SDOM

#endif // __SDOM_DATAREGISTRY_HPP__
// SDOM_DataRegistry.hpp
#pragma once



namespace SDOM
{

} // END namespace SDOM