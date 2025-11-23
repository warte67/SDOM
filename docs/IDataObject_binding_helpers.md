# SDOM Binding Helper Consolidation

This document describes the redesign of `IDataObject` to centralize common reflection
and binding-registration utilities for SDOM’s DataRegistry‑based system.  
It replaces duplicated logic found in individual `registerBindingsImpl()` implementations.

---

# 1. Overview

Many SDOM object types perform the same steps during binding registration:

1. Ensure a corresponding `TypeInfo` exists  
2. Register properties  
3. Register C/C++/ABI callable function metadata  
4. Register opaque C handle structs  
5. Register enum members and category/doc metadata  
6. Safely retrieve the correct active `DataRegistry`  

This redesign moves these repeated operations into reusable helpers inside `IDataObject`.

## 1.1 Goals

- Short term: unblock the C API generator by giving every `IDataObject` subclass the same reflection plumbing; Lua and other runtimes will adopt the helpers once the C flow is stable.
- Transition: existing hand-written `registerBindingsImpl()` logic can continue to call directly into the helpers so rolling adoption does not break legacy modules.
- Future: once coverage hits 100%, we can retire bespoke registry code paths and gate new modules on the helper-based workflow.

---

# 2. Helper API and Usage Patterns

Each helper below lives on `IDataObject` and is designed to be called directly from `registerBindingsImpl()` with no extra boilerplate. For each API, the “Usage” paragraph explains when subclasses should invoke it and what assumptions it makes.

## 2.1 Access to the correct DataRegistry

*Usage:* call `registry()` for any mutation (type creation, function registration) performed during binding. The helper first consults the thread-local pointer returned by `activeRegistry()`—set by the generator while bindings are emitted—and falls back to the global singleton when invoked elsewhere (tests, tools).

```cpp
protected:
    SDOM::DataRegistry& registry() const {
        if (auto* r = IDataObject::activeRegistry())
            return *r;
        return SDOM::DataRegistry::instance();
    }

    const SDOM::TypeInfo* lookup(const std::string& name) const {
        return registry().lookupType(name);
    }
```

> `activeRegistry()` is thread-local; subclasses should not cache the raw pointer because generators may swap registries between phases.

---

## 2.2 Create or retrieve a `TypeInfo` entry

*Usage:* invoke once per concrete object type at the top of `registerBindingsImpl()` to guarantee that metadata exists before adding properties or functions. The helper returns a mutable reference to the canonical `TypeInfo` stored in the registry.

```cpp
protected:
    SDOM::TypeInfo& ensureType(const std::string& typeName,
                               SDOM::EntryKind kind,
                               std::string cppType,
                               std::string fileStem,
                               std::string exportName,
                               std::string category,
                               std::string doc)
    {
        if (auto* existing = lookup(typeName))
            return registry().getMutable(typeName);

        SDOM::TypeInfo ti;
        ti.name        = typeName;
        ti.kind        = kind;
        ti.cpp_type_id = cppType;
        ti.file_stem   = fileStem;
        ti.export_name = exportName;
        ti.category    = category;
        ti.doc         = doc;

        registry().registerType(ti);
        return registry().getMutable(typeName);
    }
```

> `DataRegistry::getMutable()` retrieves the authoritative entry and throws (or asserts) if the type is missing, so callers should only use it immediately after `ensureType()` or `registerType()`.

---

## 2.3 Register an opaque C API handle

*Usage:* call once per exported opaque struct. The default implementation emits a single `impl` pointer; if a handle needs richer metadata (extra user data, generation counters), extend the helper before registration or add fields to `ti.properties` inside the call site.

```cpp
protected:
    void registerOpaqueHandle(const std::string& cppType,
                              const std::string& exportName,
                              const std::string& fileStem,
                              const std::string& doc)
    {
        if (lookup(exportName))
            return;

        SDOM::TypeInfo ti;
        ti.name        = exportName;
        ti.kind        = SDOM::EntryKind::Struct;
        ti.cpp_type_id = cppType;
        ti.file_stem   = fileStem;
        ti.export_name = exportName;
        ti.doc         = doc;

        SDOM::PropertyInfo implField;
        implField.name      = "impl";
        implField.cpp_type  = cppType + "*";
        implField.read_only = true;
        implField.doc       = "Opaque pointer to underlying C++ instance.";
        ti.properties.push_back(implField);

        registry().registerType(ti);
    }
```

---

## 2.4 Register a callable method with metadata

*Usage:* wrap every exported instance or static method so the generator sees consistent metadata. Parameters are currently inferred elsewhere (the existing reflection still populates `FunctionInfo::param_types`), so this helper focuses on wiring up names, docs, and runtime callables.

```cpp
protected:
    template<typename Fn>
    void registerMethod(const std::string& typeName,
                        const std::string& name,
                        const std::string& cppSig,
                        const std::string& returnType,
                        const std::string& cName,
                        const std::string& cSig,
                        const std::string& doc,
                        Fn&& fn)
    {
        SDOM::FunctionInfo fi;
        fi.name          = name;
        fi.cpp_signature = cppSig;
        fi.return_type   = returnType;
        fi.c_name        = cName;
        fi.c_signature   = cSig;
        fi.doc           = doc;

        // TODO: wire param_types population/opcode marshalling once unified translators land.
        registry().registerFunction(typeName, fi, std::forward<Fn>(fn));
    }
```

---

## 2.5 Register enumeration values

*Usage:* emit once per enum constant. Because `DataRegistry::registerType()` currently performs a replace-on-duplicate, callers should avoid re-registering the same `(exportName, cppName)` pair during hot reload; future work will add explicit diagnostics.

```cpp
protected:
    void registerEnumValue(const std::string& exportName,
                           const std::string& fileStem,
                           const std::string& cppName,
                           uint32_t value,
                           const std::string& doc)
    {
        SDOM::TypeInfo ti;
        ti.name        = exportName + std::string("::") + cppName;
        ti.kind        = SDOM::EntryKind::Enum;
        ti.cpp_type_id = cppName;
        ti.file_stem   = fileStem;
        ti.export_name = exportName;
        ti.enum_value  = value;
        ti.doc         = doc;

        registry().registerType(ti);
    }
```

---

# 3. Example: Updated `Event::registerBindingsImpl()`

The Event object exercises every helper: it first guarantees the base type exists, registers the shared opaque handle, wires the `getType` callable, then bulk-loads the enum catalog. Compared to the previous manual approach (50+ lines of bespoke registry calls), the declarative flow below makes each step explicit through the numbered comments.

```cpp
void Event::registerBindingsImpl(const std::string& typeName)
{
    // 1. Ensure base TypeInfo
    auto& ti = ensureType(typeName,
                          SDOM::EntryKind::Object,
                          "SDOM::Event",
                          "Event",
                          "SDOM_Event",
                          "Events",
                          "SDOM Event object bindings");

    // 2. Register opaque handle
    registerOpaqueHandle("SDOM::Event",
                         "SDOM_Event",
                         "Event",
                         "Opaque C handle for SDOM::Event.");

    // 3. Register getType() callable
    registerMethod(typeName,
                   "getType",
                   "EventType Event::getType() const",
                   "EventType",
                   "SDOM_GetEventType",
                   "bool SDOM_GetEventType(const SDOM_Event*, SDOM_EventType*)",
                   "Return the EventType id",
                   [](const Event* evt, EventType::IdType* out) {
                       if (!evt || !out) return false;
                       std::lock_guard lock(evt->event_mutex_);
                       *out = const_cast<Event*>(evt)->type.getOrAssignId();
                       return true;
                   });

    // 4. Register EventType values
    for (auto* et : EventType::getAll()) {
        registerEnumValue("SDOM_EventType",
                          "Event",
                          et->getName(),
                          et->getOrAssignId(),
                          et->getDoc());
    }
}
```

---

# 4. Benefits

- 70–80% reduction in duplicate code  
- Subclasses become declarative rather than procedural  
- Reduces mistakes from inconsistent field names  
- CAPI and Lua binding systems gain uniformity  
- Future binding generators become dramatically simpler  
- DataRegistry becomes cleaner and more deterministic  

---

# 5. Next Steps

- Fully integrate helpers into a new `IDataObject.hpp/.cpp`  
- Update all SDOM objects to use `ensureType()` patterns  
- Add common field-type translators for C/Lua output  
- Move enum grouping and module grouping into shared logic  
- Expand `registerMethod()` with automatic C signature generation  
- Begin implementing `LUA_BindGenerator` using same structure  

| Step | Owner | Target milestone |
| --- | --- | --- |
| Land `IDataObject` helper implementation in runtime (`.hpp/.cpp`) | Core Runtime | Sprint 48 |
| Migrate high-traffic objects (Event, Frame, Stage, DataRegistry tools) | Framework Team | Sprint 49 |
| Add shared type translators + enum grouping utilities | Tooling/Bindings | Sprint 50 |
| Extend `registerMethod()` for auto C sig generation | Tooling/Bindings | Sprint 51 |
| Bring Lua generator onto helper stack | Scripting Team | Sprint 52 |

---

Generated for SDOM development — November 22, 2025.
