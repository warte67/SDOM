# SDOM Variant System — Complete Design Specification (v6)

## 📘 Table of Contents
1. [Introduction](#1-introduction)  
2. [Goals & Use Cases](#2-goals--use-cases)  
3. [Core Data Model](#3-core-data-model)  
4. [Type Extensibility](#4-type-extensibility)  
5. [Conversion System](#5-conversion-system)  
6. [Operator Model](#6-operator-model)  
7. [Performance & Memory Layout](#7-performance--memory-layout)  
8. [Error Handling and Validation](#8-error-handling-and-validation)  
9. [Logging & Debugging](#9-logging--debugging)  
10. [Future Expansion](#10-future-expansion)  
11. [Summary](#11-summary)

---

## 1. Introduction
### Design Motivation
The SDOM `Variant` class provides a unified, extensible, and type-safe container capable of representing all major data types exchanged between **C++**, **Lua**, and the SDOM runtime. It replaces fragmented per-type conversion logic with a centralized system for data storage, reflection, and inter-language interoperability.

### Role within SDOM’s Reflection and Lua Integration
`Variant` acts as the foundation for the **DataRegistry**, enabling the reflection and serialization of `IDataObject` instances. It ensures that any data in SDOM—whether primitive, structured, or composite—can be exposed to or derived from Lua scripts seamlessly.

### Guiding Principles
- **Simplicity:** One consistent, predictable data model for all systems.  
- **Safety:** Always return valid data or a clear error indicator.  
- **Interoperability:** Serve as the bridge for C++, Lua, and other future languages.  
- **Performance:** Measure twice, allocate once — minimize heap churn and runtime overhead.

---

## 2. Goals & Use Cases
- **Dynamic Reflection:** Enable runtime introspection and property querying.  
- **Type Conversion:** Unify and simplify type conversions between Lua, C++, and external systems.  
- **ABI Bridging:** Provide a common memory-safe interface for external language bindings.  
- **Lua & External Compatibility:** Represent data in a Lua-friendly structure (tables, primitives).  
- **Inspection:** Allow read-only access to existing Variants through lightweight `VariantView`.

---

## 3. Core Data Model
`Variant` internally uses `std::variant` to store any supported type safely and efficiently.

```cpp
std::variant<std::nullptr_t, bool, int64_t, double, std::string,
             std::vector<Variant>,
             std::unordered_map<std::string, Variant>,
             std::shared_ptr<void>,
             sol::object> data_;
```

### Supported Categories
| Category | Types |
|-----------|--------|
| **Primitives** | bool, int64_t, double, std::string |
| **Composite** | std::vector<Variant>, std::unordered_map<std::string, Variant> |
| **Dynamic** | std::shared_ptr<void> (user-defined extensible payload) |
| **Scripted** | sol::object (Lua reference or userdata) |
| **Special** | nullptr, error state |

### VariantType Enum
Each stored type corresponds to a unique `VariantType` enum value for quick runtime checks.

```cpp
enum class VariantType : uint8_t {
    Null = 0,
    Bool,
    Int,
    Real,
    String,
    Array,
    Object,
    Dynamic,
    LuaRef,
    Error
};
```

### VariantView
A lightweight, read-only view into a `Variant`, used to avoid copies when inspecting registry data.

```cpp
class VariantView {
    const Variant* ref_;
public:
    template<typename T> const T* try_get() const;
};
```

---

## 4. Type Extensibility
Users can extend the Variant system to support custom C++ types:

```cpp
Variant::registerType<MyStruct>([](const MyStruct& s) {
    sol::table t = lua.create_table();
    t["x"] = s.x;
    t["y"] = s.y;
    return t;
});
```

### Conversion Trait System
For consistency across types, all conversions use `VariantConverter<T>` specialization:

```cpp
template<typename T>
struct VariantConverter {
    static Variant fromLua(const sol::object&);
    static sol::object toLua(sol::state_view, const T&);
};
```

All registered types become available for reflection and serialization in the **DataRegistry**.

---

## 5. Conversion System
### C++ → Lua (Serialization)
- Converts C++ primitives and composite types into Lua tables or objects.  
- Handles nested data automatically (recursive flattening).

### Lua → C++ (Deserialization)
- Constructs Variants from Lua types (`sol::object`, `sol::table`, etc.).  
- Detects and wraps nested Lua structures recursively.

### Nested Tables
Lua tables map to `std::unordered_map<std::string, Variant>` internally.

```cpp
Variant fromLuaObject(sol::object o);
sol::table toLuaTable(sol::state_view L) const;
```

### Ownership Semantics
Variants holding `sol::object` maintain **weak references** to avoid dangling handles when Lua states are destroyed.

---

## 6. Operator Model
`Variant` provides native operator overloads for comparison and arithmetic operations:

```cpp
bool operator==(const Variant& rhs) const;
Variant operator+(const Variant& rhs) const;
```

### Comparison Matrix
| Operator | Allowed Types | Notes |
|-----------|----------------|-------|
| `==` | all comparable | cross-numeric coercion |
| `<, >` | numeric, string | error if mixed |
| `+` | numeric, string concatenation | error otherwise |

### Type-Safety Policies
No implicit type promotion beyond numeric coercion.  
Conversions between unrelated types trigger error states.

---

## 7. Performance & Memory Layout
- **Inline storage** for small types (primitives, small strings).  
- **Shared_ptr** management for larger containers (maps, vectors).  
- **Lazy Lua realization:** complex types convert to tables only when required.  
- **Target performance envelope:** < 50 ns for primitive get/set, minimal heap allocation during shallow copies.

---

## 8. Error Handling and Validation
### Default Error Model
Variants always return valid data. When an operation fails, it returns a special Variant with `hasError()` set.

```cpp
class Variant {
public:
    bool hasError() const { return error_; }
    std::string errorMessage() const { return errorMsg_; }
    static Variant makeError(const std::string& msg) {
        Variant v; v.error_ = true; v.errorMsg_ = msg; return v; }
private:
    bool error_ = false;
    std::string errorMsg_;
};
```

### Nested Error Contexts
Errors within nested structures accumulate context for easier debugging:

```
VariantError: key 'color' → expected table {r,g,b,a}, got number
```

### Compile-Time Error Modes
| Mode | Behavior | Use Case |
|------|-----------|----------|
| **Safe (Default)** | Returns error-marked Variant | Runtime builds |
| **Strict** | Throws `VariantTypeError` | Debug & testing |
| **Silent ABI** | No logs, only error flags | External bindings |

---

## 9. Logging & Debugging
All Variant operations integrate with SDOM’s binding macros:

```cpp
BIND_LOG("Variant::toObject() conversion successful");
BIND_WARN("Variant::toNumber() missing fields, defaulting");
BIND_ERR("Variant::fromLuaObject() invalid Lua type");
```

### toString() / Debug Dump
`Variant::toString(int depth = 1)` returns a human-readable representation of nested Variant content.

---

## 10. Future Expansion
- **DataRegistry Synchronization:** Variants form the atomic unit for reflection.  
- **C ABI Harness:** Variant serialization can directly power C-side testing.  
- **ScriptManager Integration:** Unified handling of cross-language bindings (Lua, Python, etc.).  
- **VariantView Optimization:** Allow non-owning traversal of reflected objects.  
- **Type Traits:** Support pluggable reflection via compile-time adapters.

---

## 11. Summary
- Unified dynamic type system for SDOM.  
- Robust Lua ↔ C++ conversion pipeline.  
- Operator-safe, reflection-ready, and error-aware.  
- Extensible for future languages and DataRegistry.  
- Configurable runtime modes for stability or debugging.  
- Includes conversion traits, VariantView, and nested error propagation.

---

**Version:** 6.0 (Enhanced)  
**Author:** Jay Faries (warte67)  
**Project:** SDOM
