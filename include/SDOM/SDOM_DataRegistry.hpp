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

namespace SDOM 
{

    // Forward declaration
    class IBindGenerator;


    // ---------------------------------------------------------
    // EntryKind — categorical discriminator for TypeInfo entries
    // ---------------------------------------------------------
    enum class EntryKind {
        Object,     // Complex reflected object (properties + methods)
        Struct,     // Plain data structure to be emitted in C API
        Enum,       // Enum types
        Global,     // Global constants or singletons
        Function,   // Stand-alone callable (no owning type)
        Alias       // Typedef or language alias
    };

    // ---------------------------------------------------------
    // FunctionInfo — metadata used for reflection + binding
    // ---------------------------------------------------------
    struct FunctionInfo {
        std::string name;
        std::string cpp_signature;     // original C++ signature
        std::vector<std::string> param_types;
        std::string return_type;

        // C API binding names (generated or overridden)
        std::string c_name;            
        std::string c_signature;

        std::string doc;
        bool is_static = false;
        bool exported  = true;

        // Optional runtime callable
        std::any callable;

        enum class CallableKind : uint8_t { 
            None = 0, 
            CFunctionPtr, 
            CppCallable, 
            GenericRuntime 
        };
        CallableKind callable_kind = CallableKind::None;

        std::uintptr_t c_function_ptr = 0;

        // Optional tag for dynamic bindings (Lua, Python, etc.)
        std::string runtime_tag;
        std::any    runtime_handle;
    };

    // ---------------------------------------------------------
    // PropertyInfo — metadata for reflected fields/properties
    // ---------------------------------------------------------
    struct PropertyInfo {
        std::string name;
        std::string cpp_type;
        bool read_only = false;
        std::optional<std::string> default_value;
        std::string doc;

        // Optional getter/setter (type-erased)
        std::any getter;
        std::any setter;
    };

    // ---------------------------------------------------------
    // TypeInfo — unified metadata node stored in DataRegistry
    // ---------------------------------------------------------
    struct TypeInfo {
        std::string name;          // unique type name
        EntryKind   kind = EntryKind::Object;
        std::string file_stem;     // optional file stem (e.g., "Event" as in SDOM_CAPI_Event.h)
        std::string export_name;   // optional export identifier (e.g., "typedef enum SDOM_Event")

        // C++ type string ("Event", "SpriteSheet", etc.)
        std::string cpp_type_id;
        std::optional<std::uint32_t> enum_value; // Optional integral value for enum entries

        // signatures
        std::vector<PropertyInfo> properties;
        std::vector<FunctionInfo> functions;

        // inheritance base types
        std::vector<std::string> bases;

        std::string category;   // optional category (editor, runtime, etc.)
        std::string doc;        // comment/documentation
    };

    // ---------------------------------------------------------
    // DataRegistrySnapshot — immutable at binding time
    // ---------------------------------------------------------
    struct DataRegistrySnapshot {
        std::vector<TypeInfo> types;
    };


    // ---------------------------------------------------------
    // GeneratorConfig structure
    // ---------------------------------------------------------
    struct GeneratorConfig {
        std::unique_ptr<IBindGenerator> generator;

        std::string header_output_dir;
        std::string source_output_dir;

        // Future extensibility:
        bool overwrite = true;
        bool verbose = false;
    };


    // ---------------------------------------------------------
    // DataRegistry — THE reflection store for SDOM
    // ---------------------------------------------------------
    class DataRegistry {
    public:
        DataRegistry();
        ~DataRegistry();

        DataRegistry(const DataRegistry&) = delete;
        DataRegistry& operator=(const DataRegistry&) = delete;
        DataRegistry(DataRegistry&&) = delete;
        DataRegistry& operator=(DataRegistry&&) = delete;

        static DataRegistry& instance();

        // -------------------------------
        // Registration API
        // -------------------------------

        // Register a TypeInfo entry (Object, Struct, Enum, etc.)
        bool registerType(const TypeInfo& info);

        // Alias for registerType (legacy compatibility)
        bool registerDataType(const TypeInfo& info) { return registerType(info); }

        // Register C++ std::function or lambda
        template<typename Fn>
        void registerFunction(const std::string& typeName,
                            const FunctionInfo& meta,
                            Fn&& fn)
        {
            std::scoped_lock lk(mutex_);
            FunctionInfo m = meta;
            m.callable = std::make_any<std::decay_t<Fn>>(std::forward<Fn>(fn));
            m.callable_kind = FunctionInfo::CallableKind::CppCallable;
            entries_[typeName].functions.push_back(std::move(m));
        }

        // Register C function pointer (for CAPI)
        template<typename R, typename... Args>
        void registerCFunctionPtr(const std::string& typeName,
                                const FunctionInfo& meta,
                                R(*fp)(Args...))
        {
            std::scoped_lock lk(mutex_);
            FunctionInfo m = meta;
            m.callable_kind = FunctionInfo::CallableKind::CFunctionPtr;
            m.c_function_ptr = reinterpret_cast<std::uintptr_t>(fp);

            if (m.c_name.empty()) {
                m.c_name = "SDOM_CFN_" + typeName + "_" + m.name;
            }

            entries_[typeName].functions.push_back(std::move(m));
        }

        // Register meta-only function (no runtime callable)
        void registerFunction(const std::string& typeName, const FunctionInfo& meta);

        // Register getter/setter property
        template<typename Getter, typename Setter = std::nullptr_t>
        void registerProperty(const std::string& typeName,
                            const PropertyInfo& meta,
                            Getter&& getter,
                            Setter&& setter = nullptr)
        {
            std::scoped_lock lk(mutex_);

            PropertyInfo p = meta;
            p.getter = std::make_any<std::decay_t<Getter>>(std::forward<Getter>(getter));

            if constexpr (!std::is_same_v<std::decay_t<Setter>, std::nullptr_t>) {
                p.setter = std::make_any<std::decay_t<Setter>>(std::forward<Setter>(setter));
            }

            entries_[typeName].properties.push_back(std::move(p));
        }

        void registerProperty(const std::string& typeName, const PropertyInfo& meta);

        // -------------------------------
        // Lookup / Queries
        // -------------------------------
        const TypeInfo* lookupType(const std::string& name) const;
        TypeInfo& getMutable(const std::string& name);

        std::vector<std::string> listTypes() const;

        const std::unordered_map<std::string, TypeInfo>& getAllTypes() const {
            return entries_;
        }

        // -------------------------------
        // Binding Generation
        // -------------------------------
        bool generateBindings();
        void addGenerator(std::unique_ptr<IBindGenerator> gen,
                        std::string headerDir,
                        std::string sourceDir,
                        bool overwrite = true,
                        bool verbose = false);

        // If generators want a copy of all types in a flat map:
        std::unordered_map<std::string, TypeInfo> getMergedTypes() const;

    private:
        mutable std::mutex mutex_;
        std::unordered_map<std::string, TypeInfo> entries_;
        std::vector<GeneratorConfig> generators_;
    };

} // namespace SDOM

// ===========================================================================
// Runtime dispatcher API for generic ABI function calls
// ===========================================================================
namespace SDOM { namespace CAPI {

    struct CallArg {
        enum class Kind : uint8_t { None=0, Int, UInt, Double, Bool, CString, Ptr };

        Kind kind = Kind::None;

        union {
            std::int64_t   i;
            std::uint64_t  u;
            double         d;
            bool           b;
            void*          p;
        } v{};

        std::string s; // for strings

        CallArg() noexcept = default;

        static CallArg makeInt(std::int64_t x)      { CallArg a; a.kind=Kind::Int;    a.v.i=x; return a; }
        static CallArg makeUInt(std::uint64_t x)    { CallArg a; a.kind=Kind::UInt;   a.v.u=x; return a; }
        static CallArg makeDouble(double x)         { CallArg a; a.kind=Kind::Double; a.v.d=x; return a; }
        static CallArg makeBool(bool x)             { CallArg a; a.kind=Kind::Bool;   a.v.b=x; return a; }
        static CallArg makeCString(const char* s)   { CallArg a; a.kind=Kind::CString; if(s) a.s=s; return a; }
        static CallArg makePtr(void* p)             { CallArg a; a.kind=Kind::Ptr;    a.v.p=p; return a; }
    };

    struct CallResult {
        CallArg::Kind kind = CallArg::Kind::None;

        union {
            std::int64_t   i;
            std::uint64_t  u;
            double         d;
            bool           b;
            void*          p;
        } v{};

        std::string s;

        static CallResult Void()                     { return {}; }
        static CallResult FromInt(std::int64_t x)    { CallResult r; r.kind=CallArg::Kind::Int;    r.v.i=x; return r; }
        static CallResult FromUInt(std::uint64_t x)  { CallResult r; r.kind=CallArg::Kind::UInt;   r.v.u=x; return r; }
        static CallResult FromDouble(double x)       { CallResult r; r.kind=CallArg::Kind::Double; r.v.d=x; return r; }
        static CallResult FromBool(bool x)           { CallResult r; r.kind=CallArg::Kind::Bool;   r.v.b=x; return r; }
        static CallResult FromPtr(void* p)           { CallResult r; r.kind=CallArg::Kind::Ptr;    r.v.p=p; return r; }
        static CallResult FromString(std::string s)  { CallResult r; r.kind=CallArg::Kind::CString; r.s=std::move(s); return r; }
    };

    using GenericCallable = std::function<CallResult(const std::vector<CallArg>&)>;

    bool registerCallable(const std::string& name, GenericCallable fn);
    std::optional<GenericCallable> lookupCallable(const std::string& name);
    CallResult invokeCallable(const std::string& name, const std::vector<CallArg>& args);

}} // namespace SDOM::CAPI

#endif // __SDOM_DATAREGISTRY_HPP__
