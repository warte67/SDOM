# SDOM Core Function and Property Registry Design

## Overview
This document describes the design of a **dual-key Core function registry** for SDOM.  
The system provides **string-based reflection** for scripting interfaces (Lua, Python, etc.)  
and **integer-based identifiers** for fast, low-overhead access in C++ and the engine runtime.

---

## Goals
- Maintain a **single unified Core interface**.
- Allow both **human-readable (string)** and **fast integer** access to functions and properties.
- Eliminate per-frame string hashing overhead.
- Provide a stable ABI for external bindings (C API, Lua, Python).
- Keep reflection and scripting capability fully intact.

---

## Architecture

### Core Concept
Each function or property is registered once with **both a string name and an integer ID**.

```cpp
functionMapByName["getX"] = { FN_GET_X, "getX", [](auto& args){ return Variant(window_->getX()); } };
functionMapByName["setX"] = { FN_SET_X, "setX", [](auto& args){ window_->setX(args[0].get<int>()); return Variant(); } };

// Link both ways (zero overhead after init)
for (auto& [name, entry] : functionMapByName)
    functionMapById[entry.id] = &entry;
```

This creates a **bidirectional lookup**:
- Strings → Reflection / Scripting
- IDs → Fast Engine Dispatch

---

## Data Structures

```cpp
struct FunctionEntry
{
    uint32_t id;
    std::string name;
    std::function<Variant(const std::vector<Variant>&)> fn;
};

std::unordered_map<std::string, FunctionEntry> functionMapByName;
std::unordered_map<uint32_t, FunctionEntry*> functionMapById;
```

---

## Function Registration

```cpp
functionMapByName["getX"] = { FN_GET_X, "getX", [](auto& args){ return Variant(window_->getX()); } };
functionMapByName["setX"] = { FN_SET_X, "setX", [](auto& args){ window_->setX(args[0].get<int>()); return Variant(); } };

for (auto& [name, entry] : functionMapByName)
    functionMapById[entry.id] = &entry;
```

---

## Function Call Paths

### Reflection / Scripting

```cpp
Variant Core::call(const std::string& name, const std::vector<Variant>& args)
{
    auto it = functionMapByName.find(name);
    if (it != functionMapByName.end())
        return it->second.fn(args);
    throw std::runtime_error("Unknown function: " + name);
}
```

### Fast Integer Dispatch

```cpp
Variant Core::call(uint32_t id, const std::vector<Variant>& args)
{
    auto it = functionMapById.find(id);
    if (it != functionMapById.end())
        return it->second->fn(args);
    throw std::runtime_error("Unknown function ID");
}
```

---

## Enumerated IDs

Each function or property is given a unique enumerated ID for fast lookup:

```cpp
enum CoreFunctionID : uint32_t {
    FN_GET_X,
    FN_SET_X,
    FN_GET_Y,
    FN_SET_Y,
    FN_COUNT
};

enum CorePropertyID : uint32_t {
    PROP_WINDOW_WIDTH,
    PROP_WINDOW_HEIGHT,
    PROP_ROOT_NODE,
    PROP_COUNT
};
```

---

## Macros for Ultra-Fast Access

```cpp
#define CORE_CALL(fn_id, ...) Core::getInstance().call(fn_id, { __VA_ARGS__ })
```

Usage:

```cpp
CORE_CALL(FN_SET_X, Variant(1280));
int x = CORE_CALL(FN_GET_X).get<int>();
```

This expands to pure integer-based dispatch — zero runtime hashing or lookup.

---

## Lua / C ABI Integration

### Lua Example

```cpp
int Core::luaCall(const std::string& name, sol::variadic_args args)
{
    std::vector<Variant> vargs;
    for (auto v : args) vargs.emplace_back(toVariant(v));
    return call(name, vargs).as<int>();
}
```

### C ABI Example

```c
extern "C" int sdom_call(uint32_t fn_id, SDOM_Variant* args, int argc)
{
    return Core::getInstance().call(fn_id, convertArgs(args, argc)).i;
}
```

---

## Benefits Summary

| Feature | Benefit |
|----------|----------|
| 🔁 **Dual-key flexibility** | Works with both human-readable and fast numeric lookups |
| ⚡ **Zero runtime hashing** | All resolution happens once at registration |
| 🧱 **Reflection preserved** | You can still introspect by string names (for Lua, Python, etc.) |
| 🧩 **Cache-friendly** | Pointers to lambdas stored contiguously; no string churn |
| 🧠 **Future-safe** | Easy to serialize function IDs across DLLs, plugins, or save files |
| 🧪 **Unit-testable** | Test both lookup modes independently |

---

## Optional Macro Registration System

To simplify registration, you can define:

```cpp
#define REGISTER_CORE_FUNCTION(id, name, lambda) \
    Core::getInstance().registerFunction(id, name, lambda);
```

Usage:

```cpp
REGISTER_CORE_FUNCTION(FN_GET_X, "getX", [](){ return Variant(window_->getX()); });
REGISTER_CORE_FUNCTION(FN_SET_X, "setX", [](auto& args){ window_->setX(args[0].get<int>()); return Variant(); });
```

---

## Future Extensions

- **Auto-generated headers**: generate C and C++ wrappers from the registry at build time.
- **Property reflection**: extend the same system to handle properties with `get/set` pairs.
- **Hot-reloadable API**: allow external plugins to register functions dynamically.

---

## Summary

This design provides:
- A **single unified Core interface** across all languages.
- **Integer-based dispatch** for maximum speed.
- **String-based reflection** for scripting and introspection.
- A flexible, future-proof registry architecture.

