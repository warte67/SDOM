# SDOM Data Registry Design — Combined Version 3 → 4

## 🧩 Overview
The **SDOM Data Registry** provides a unified, extensible framework for cross-language reflection, function/property registration, and dynamic binding generation across all supported environments (C++, Lua, C, and Rust).

This combined document captures the **evolution from Version 3 (standalone registry)** to **Version 4 (Factory-integrated registry)**, illustrating the progression from per-object reflection to centralized metadata ownership under the Factory system.

---

# 🧩 SDOM Data Registry Design — Version 3

## 🎯 Goals
- Centralize all API metadata in one registry.
- Enable runtime reflection and function/property enumeration.
- Provide a consistent structure for automated binding generation.
- Maintain lightweight runtime overhead.

---

## 🧠 Architecture

Each SDOM type registers its callable functions and properties at startup via the `IDataObject` interface. The registry stores metadata in a tree of `RegistryNode` objects.

### RegistryNode Structure

```cpp
struct FunctionInfo {
    uint32_t id;
    std::string name;
    std::string returnType;
    std::function<Variant(const std::vector<Variant>&)> fn;
    std::function<sol::function()> luaFn; // Lua-specific lambda if needed
    std::vector<ParameterInfo> parameters;
    std::string description;
};

struct PropertyInfo {
    std::string name;
    std::string type;
    uint32_t getterId;
    uint32_t setterId;
    std::string description;
};

struct RegistryNode {
    std::string typeName;
    std::unordered_map<std::string, FunctionInfo> functions;
    std::unordered_map<std::string, PropertyInfo> properties;
};
```

### IDataObject Interface

```cpp
class IDataObject {
public:
    virtual void registerMetadata(RegistryNode& node) const = 0;
};
```

Each subclass of `IDataObject` populates its own `RegistryNode` using helper calls such as:

```cpp
node.addFunction("setText", FN_SET_TEXT, "void", { {"text", "string", "Button label text"} },
                 "Sets the visible text for this button.");

node.addProperty("text", "string", FN_GET_TEXT, FN_SET_TEXT, "Displayed label text.");
```

---

## 🧮 Central Registry

A singleton `DataRegistry` stores all `RegistryNode`s keyed by type name:

```cpp
class DataRegistry {
public:
    static DataRegistry& getInstance();

    void registerNode(const std::string& typeName, const RegistryNode& node) {
        registry[typeName] = node;
    }

    const RegistryNode* getNode(const std::string& typeName) const {
        auto it = registry.find(typeName);
        return (it != registry.end()) ? &it->second : nullptr;
    }

private:
    std::unordered_map<std::string, RegistryNode> registry;
};
```

---

## 🧩 Example Registration

```cpp
void Button::registerMetadata(RegistryNode& node) const {
    node.functions["setText"] = {
        .id = FN_SET_TEXT,
        .name = "setText",
        .returnType = "void",
        .fn = [](auto& args){ /* call internal setter */ },
        .luaFn = [](){ /* sol::function wrapper */ },
        .parameters = {{"text", "string", "Label text"}},
        .description = "Sets the visible text label on the button."
    };

    node.properties["text"] = {
        .name = "text",
        .type = "string",
        .getterId = FN_GET_TEXT,
        .setterId = FN_SET_TEXT,
        .description = "Visible text label bound to the button."
    };
}
```

---

## 🔗 Cross-Language Binding Generation

Each language generator consumes the Data Registry:

| Language | Generation Method |
|-----------|------------------|
| **C++** | Uses direct variant dispatch through IDs |
| **Lua** | Builds sol2 bindings dynamically from registry entries |
| **C** | Generates static API header (`sdom_api.h`) from registry data |
| **Rust** | Generates trait and struct wrappers from registry metadata |

---

## 📦 Serialization

The registry can serialize itself into a binary format (`data_registry.bin`) containing:
- SDOM API version
- Type names, function metadata, argument and property data

At runtime, the registry loads this file if valid, otherwise regenerates it.

---

## 🧠 Summary (v3)
- Each SDOM type defines its own metadata independently.
- `IDataObject` acts as the reflection root.
- The central `DataRegistry` aggregates all metadata.
- Supports full automatic binding generation for all supported languages.

> *Version 3 represented the standalone registry system prior to Factory integration.*

---

# 🏭 SDOM Data Registry Design — Version 4 (Factory Integration)

## Core Principle
> The Factory owns the objects — therefore, it also owns their registries.

Each SDOM object type (e.g., `Button`, `TristateButton`, `Stage`) is registered with the Factory by a unique string identifier. This registration process now includes a **reference to that object's metadata node** in the central Data Registry.

This ensures every registered object type in the Factory has direct access to its own reflection data (functions, properties, descriptions, etc.).

### ObjectRegistration Structure

```cpp
struct ObjectRegistration {
    std::string typeName;
    CreateFn createFn;
    DestroyFn destroyFn;
    RegistryNode* dataRegistry;
};
```

### Factory Example

```cpp
class Factory {
public:
    static Factory& getInstance();

    template<typename T>
    void registerType(const std::string& name) {
        ObjectRegistration reg;
        reg.typeName = name;
        reg.createFn = []() { return std::make_shared<T>(); };
        reg.dataRegistry = DataRegistry::getInstance().createNode(name);

        T tempObj;
        tempObj.registerMetadata(*reg.dataRegistry);

        objectRegistry[name] = reg;
    }

    RegistryNode* getRegistryNode(const std::string& typeName) {
        return objectRegistry[typeName].dataRegistry;
    }

private:
    std::unordered_map<std::string, ObjectRegistration> objectRegistry;
};
```

### Access Flow
```
DisplayHandle → Factory (lookup by string)
                      ↓
           ObjectRegistration {
               createFn,
               dataRegistry → RegistryNode (functions, properties, descriptions)
           }
```

---

## 🧭 Implementation Roadmap: From v3 → v4

### Key Differences
| v3 | v4 |
|----|----|
| Central DataRegistry singleton holds all nodes | Factory now owns and manages per-type registries |
| Objects register directly with DataRegistry | Objects register through Factory, which assigns their RegistryNode |
| No link between object creation and metadata | Creation and reflection unified under Factory control |
| Focused on per-object reflection | Focused on global Factory-managed metadata tree |

### Migration Steps
1. Disable all Lua bindings temporarily.  
2. Strip old per-object Lua binding code.  
3. Integrate Registry with Factory.  
4. Validate Data Registry generation.  
5. Rebuild Lua Bindings via Registry.  
6. Serialize Registry.  
7. Prepare for v0.6.0 milestone.

### Archival Notes
- Preserve **v3** as a reference document.  
- Maintain branch/tag `legacy-lua-bindings` for old system.  
- Keep `v4` as the live evolving spec tied to Factory integration.

---

## 📜 Summary
- The Factory is now the authoritative registry root.
- Each object type carries its own metadata schema through its `RegistryNode`.
- Cross-language binding generation is unified, fast, and versioned.
- `IDataObject` serves as the universal reflection interface.
- This system prepares SDOM for automated binding generation, editor integration, and future plugin extensibility.

> 🧩 *Each registered type carries its own DNA — the Factory just keeps the genome.*
