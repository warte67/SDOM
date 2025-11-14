#pragma once
#ifndef __SDOM_DATAREGISTRY_HPP__
#define __SDOM_DATAREGISTRY_HPP__

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <any>
#include <functional>
#include <cstdint>
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
    
    /**
     * Explicit callable kind to help binding generators decide how to
     * emit bindings. Generators should prefer an explicit `c_signature`
     * / `c_name` when present, otherwise fall back to the recorded
     * callable kind and typed slots below.
     */
    enum class CallableKind : uint8_t { None = 0, CFunctionPtr, CppCallable, LuaRef, PythonRef, GenericRuntime };
    CallableKind callable_kind = CallableKind::None;

    /**
     * If `callable_kind == CFunctionPtr` this stores the raw function
     * pointer value reinterpreted as an integer. It is expected to hold
     * an `extern "C"` function pointer (ABI-stable) when used by
     * generators or runtime dispatchers.
     */
    std::uintptr_t c_function_ptr = 0;

    /**
     * If `callable_kind == LuaRef` this stores the `luaL_ref` integer
     * value created by the runtime and (optionally) an opaque pointer to
     * the `lua_State` held in `lua_state_ptr`.
     */
    int lua_ref = 0;
    std::uintptr_t lua_state_ptr = 0; // opaque pointer to lua_State if needed

    /**
     * Optional runtime tag (e.g. "lua", "python") and an opaque
     * runtime_handle for generator/runtime-specific uses.
     */
    std::string runtime_tag;
    std::any runtime_handle;
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
    std::string category; // optional grouping/category used by generators
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
    // Alias that matches IDataObject naming: register a complete TypeInfo
    bool registerDataType(const TypeInfo& info);

    template<typename Fn>
    void registerFunction(const std::string& typeName, const FunctionInfo& meta, Fn&& fn)
    {
        std::scoped_lock lk(mutex_);
        FunctionInfo m = meta;
        m.callable = std::make_any<std::decay_t<Fn>>(std::forward<Fn>(fn));
        types_[typeName].functions.push_back(std::move(m));
    }

    /**
     * Register an extern "C" function pointer for the named type.
     *
     * This stores the pointer in `FunctionInfo::c_function_ptr` and marks
     * the callable kind as `CFunctionPtr`. Generators may emit an `extern`
     * prototype that maps to this function pointer or generate a thin
     * wrapper that calls it.
     *
     * Example:
     * ```cpp
     * void my_c_func(int);
     * FunctionInfo fi{ .name = "DoThing", .param_types = {"int"}, .return_type = "void" };
     * DataRegistry::instance().registerCFunctionPtr("SomeType", fi, &my_c_func);
     * ```
     */
    template<typename R, typename... Args>
    void registerCFunctionPtr(const std::string& typeName, const FunctionInfo& meta, R(*fp)(Args...))
    {
        std::scoped_lock lk(mutex_);
        FunctionInfo m = meta;
        m.callable_kind = FunctionInfo::CallableKind::CFunctionPtr;
        m.c_function_ptr = reinterpret_cast<std::uintptr_t>(fp);
        if (m.c_name.empty()) {
            // best-effort generated name; generators can sanitize further
            m.c_name = std::string("SDOM_CFN_") + typeName + "_" + m.name;
        }
        types_[typeName].functions.push_back(std::move(m));
    }

    /**
     * Register a Lua function reference (luaL_ref) for the named type.
     * The registry will store the integer ref and optional lua_State
     * pointer. Generators can emit C wrappers that dispatch to the Lua
     * runtime using these refs.
     *
     * Note: ownership semantics: the caller is responsible for creating
     * the luaL_ref and ensuring the `lua_State` remains valid while the
     * registry holds the ref. Unregistering (and luaL_unref) should be
     * handled by runtime code when needed.
     */
    void registerLuaFunction(const std::string& typeName, FunctionInfo meta, int luaRef, void* luaStatePtr = nullptr)
    {
        std::scoped_lock lk(mutex_);
        meta.callable_kind = FunctionInfo::CallableKind::LuaRef;
        meta.lua_ref = luaRef;
        meta.lua_state_ptr = reinterpret_cast<std::uintptr_t>(luaStatePtr);
        if (meta.c_name.empty()) {
            meta.c_name = std::string("SDOM_LUA_") + typeName + "_" + meta.name;
        }
        types_[typeName].functions.push_back(std::move(meta));
    }

    // Non-template overload: register a FunctionInfo without an associated callable
    void registerFunction(const std::string& typeName, const FunctionInfo& meta);

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

    // Non-template overload: register a PropertyInfo without associated getter/setter callables
    void registerProperty(const std::string& typeName, const PropertyInfo& meta);

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