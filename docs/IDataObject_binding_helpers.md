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

---

# 2. Required Additions to `IDataObject`

## 2.1 Access to the correct DataRegistry

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

---

## 2.2 Create or retrieve a `TypeInfo` entry

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

---

## 2.3 Register an opaque C API handle

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

        registry().registerFunction(typeName, fi, std::forward<Fn>(fn));
    }
```

---

## 2.5 Register enumeration values

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

---

Generated for SDOM development — November 22, 2025.
