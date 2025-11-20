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
    std::string c_name;         // optional explicit C symbol name
    std::string c_signature;    // optional explicit C prototype
    std::string doc;            // line comment for documentation
    bool is_static = false;
    bool exported = true;
    std::any callable;          // optional type-erased callable

    enum class CallableKind : uint8_t { None = 0, CFunctionPtr, CppCallable, LuaRef, PythonRef, GenericRuntime };
    CallableKind callable_kind = CallableKind::None;

    std::uintptr_t c_function_ptr = 0;

    int lua_ref = 0;
    std::uintptr_t lua_state_ptr = 0;

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
    std::string doc;        // line comment for documentation
    std::string category;   // optional grouping/category used by generators
};

struct DataRegistrySnapshot {
    std::vector<TypeInfo> types;
};

class IBindingGenerator;

class DataRegistry {
public:
    DataRegistry();
    ~DataRegistry();

    DataRegistry(const DataRegistry&) = delete;
    DataRegistry& operator=(const DataRegistry&) = delete;
    DataRegistry(DataRegistry&&) = delete;
    DataRegistry& operator=(DataRegistry&&) = delete;

    static DataRegistry& instance();

    bool registerType(const TypeInfo& info);
    bool registerDataType(const TypeInfo& info);

    template<typename Fn>
    void registerFunction(const std::string& typeName, const FunctionInfo& meta, Fn&& fn)
    {
        std::scoped_lock lk(mutex_);
        FunctionInfo m = meta;
        m.callable = std::make_any<std::decay_t<Fn>>(std::forward<Fn>(fn));
        types_[typeName].functions.push_back(std::move(m));
    }

    template<typename R, typename... Args>
    void registerCFunctionPtr(const std::string& typeName, const FunctionInfo& meta, R(*fp)(Args...))
    {
        std::scoped_lock lk(mutex_);
        FunctionInfo m = meta;
        m.callable_kind = FunctionInfo::CallableKind::CFunctionPtr;
        m.c_function_ptr = reinterpret_cast<std::uintptr_t>(fp);
        if (m.c_name.empty()) {
            m.c_name = std::string("SDOM_CFN_") + typeName + "_" + m.name;
        }
        types_[typeName].functions.push_back(std::move(m));
    }

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

    void registerProperty(const std::string& typeName, const PropertyInfo& meta);

    const TypeInfo* lookupType(const std::string& name) const;
    std::vector<std::string> listTypes() const;

    bool generateBindings(const std::string& outDir);
    void addGenerator(std::unique_ptr<IBindingGenerator> generator);

    const std::unordered_map<std::string, TypeInfo>& getAllTypes() const { return types_; }

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, TypeInfo> types_;
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace SDOM

// ---------------------------------------------------------------------------
// Runtime call contract and dispatcher used by generated C API wrappers.
// This is intentionally small and dependency-free so generators can emit
// thin wrappers that marshal to this contract at runtime.
// ---------------------------------------------------------------------------
namespace SDOM {

namespace CAPI {

struct CallArg {
    enum class Kind : uint8_t { None = 0, Int = 1, UInt = 2, Double = 3, Bool = 4, CString = 5, Ptr = 6 };
    Kind kind = Kind::None;
    union {
        std::int64_t i;
        std::uint64_t u;
        double d;
        bool b;
        void* p;
    } v{};
    std::string s;

    CallArg() noexcept = default;
    static CallArg makeInt(std::int64_t x) { CallArg a; a.kind = Kind::Int; a.v.i = x; return a; }
    static CallArg makeUInt(std::uint64_t x) { CallArg a; a.kind = Kind::UInt; a.v.u = x; return a; }
    static CallArg makeDouble(double x) { CallArg a; a.kind = Kind::Double; a.v.d = x; return a; }
    static CallArg makeBool(bool x) { CallArg a; a.kind = Kind::Bool; a.v.b = x; return a; }
    static CallArg makeCString(const char* cstr) { CallArg a; a.kind = Kind::CString; if (cstr) a.s = cstr; return a; }
    static CallArg makePtr(void* p) { CallArg a; a.kind = Kind::Ptr; a.v.p = p; return a; }
};

struct CallResult {
    CallArg::Kind kind = CallArg::Kind::None;
    union {
        std::int64_t i;
        std::uint64_t u;
        double d;
        bool b;
        void* p;
    } v{};
    std::string s;

    static CallResult Void() { return CallResult(); }
    static CallResult FromInt(std::int64_t x) { CallResult r; r.kind = CallArg::Kind::Int; r.v.i = x; return r; }
    static CallResult FromUInt(std::uint64_t x) { CallResult r; r.kind = CallArg::Kind::UInt; r.v.u = x; return r; }
    static CallResult FromDouble(double x) { CallResult r; r.kind = CallArg::Kind::Double; r.v.d = x; return r; }
    static CallResult FromBool(bool x) { CallResult r; r.kind = CallArg::Kind::Bool; r.v.b = x; return r; }
    static CallResult FromPtr(void* p) { CallResult r; r.kind = CallArg::Kind::Ptr; r.v.p = p; return r; }
    static CallResult FromString(std::string s) { CallResult r; r.kind = CallArg::Kind::CString; r.s = std::move(s); return r; }
};

using GenericCallable = std::function<CallResult(const std::vector<CallArg>&)>;

bool registerCallable(const std::string& name, GenericCallable fn);
std::optional<GenericCallable> lookupCallable(const std::string& name);
CallResult invokeCallable(const std::string& name, const std::vector<CallArg>& args);

} // namespace CAPI

} // namespace SDOM

#endif // __SDOM_DATAREGISTRY_HPP__