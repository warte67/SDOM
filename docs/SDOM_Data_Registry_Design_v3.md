# 🧩 SDOM Data Registry System Design (v3 — Lua Adapter Extension)

### Project: Simple SDL3 Document Object Model (SDOM)
### Component: Data Registry Architecture
### Author: Warte
### Status: Proposal (Planned for v0.6.x)

---

## **1. Overview**

The SDOM **Data Registry System** provides a unified metadata and execution model for all engine-level functions, properties, and types.  
This version (v3) adds explicit **Lua-specific adapters** to address the unique calling conventions and type handling requirements of Sol3 and LuaJIT.

The registry remains the central authority for reflection, documentation, and cross-language bindings while supporting per-language execution paths when required.

---

## **2. Goals**

- 🔁 **Unified Reflection and Execution:** Centralize function and property definitions in one registry.
- ⚙️ **Cross-Language Adapters:** Support C++, Lua, C, Python, and Rust through dedicated adapter lambdas.
- 🧠 **Runtime Introspection:** Allow detailed introspection and runtime binding.
- 🧩 **Hierarchical Organization:** Data grouped by object type, preserving inheritance hierarchies.
- 🧩 **Fail-Fast Mode:** No silent fallbacks; invalid lookups terminate with debug information.

---

## **3. Core Data Structures**

### 3.1 Parameter Metadata

```cpp
struct ParameterInfo {
    std::string name;
    std::string type;
    std::string description;
};
```

### 3.2 Function Metadata

```cpp
struct FunctionAdapters {
    // Canonical engine-side variant-based callable
    std::function<Variant(const std::vector<Variant>&)> cpp;

    // Direct Lua binding adapter (Sol3 compatible, no variant indirection)
    std::function<sol::function(sol::state&)> lua;

    // Future expansion hooks for other languages (C, Rust, Python)
    std::function<void*(void**)> c_api;   // C FFI adapter
    std::function<std::string()> rust;    // Rust stub generator
    std::function<std::string()> python;  // Python stub generator
};
```

```cpp
struct FunctionInfo {
    std::string name;
    std::string returnType;
    std::string description;
    std::vector<ParameterInfo> parameters;
    FunctionAdapters adapters;
};
```

### 3.3 Property Metadata

```cpp
struct PropertyInfo {
    std::string name;
    std::string type;
    std::string description;
    std::function<Variant()> getter;
    std::function<void(const Variant&)> setter;
    std::function<sol::function(sol::state&)> lua_getter; // Lua-safe getter
    std::function<sol::function(sol::state&)> lua_setter; // Lua-safe setter
};
```

### 3.4 Registry Node

```cpp
struct RegistryNode {
    std::string typeName;
    std::vector<FunctionInfo> functions;
    std::vector<PropertyInfo> properties;
    std::unordered_map<std::string, RegistryNode*> children;
};
```

---

## **4. Integration with IDataObject**

Each object type (Button, Label, Slider, etc.) defines its own metadata in `registerMetadata()`:

```cpp
void Button::registerMetadata(RegistryNode& root) {
    RegistryNode& node = root.createChild("Button");

    node.addProperty({
        "text", "string", "The visible label text.",
        [this]() { return Variant(this->text_); },
        [this](const Variant& v) { this->text_ = v.get<std::string>(); },
        [this](sol::state& L) {
            return sol::make_object(L, this->text_);
        },
        [this](sol::state& L) {
            return sol::make_object(L, [this](const std::string& v){ this->setText(v); });
        }
    });

    node.addFunction({
        "setText", "void", "Sets the button label.",
        { {"text", "string", "The new label value."} },
        {
            // Canonical C++ lambda
            .cpp = [](const std::vector<Variant>& args) {
                auto* self = static_cast<Button*>(args[0].getPtr());
                self->setText(args[1].get<std::string>());
                return Variant();
            },
            // Lua adapter
            .lua = [](sol::state& L) {
                return sol::make_object(L, [](Button& self, const std::string& txt){
                    self.setText(txt);
                });
            }
        }
    });
}
```

---

## **5. Registry Initialization**

The registry is auto-generated during startup via all registered `IDataObject` types.

```cpp
void SDOM::initializeRegistry() {
    dataRegistry.clear();
    for (auto& type : Factory::getAllRegisteredTypes()) {
        if (auto* obj = Factory::create(type)) {
            obj->registerMetadata(dataRegistryRoot);
            delete obj;
        }
    }
}
```

---

## **6. Serialization and Persistence**

### **Versioning**

```cpp
struct RegistryHeader {
    uint32_t versionMajor;
    uint32_t versionMinor;
    uint32_t versionPatch;
    uint32_t entryCount;
};
```

### **Workflow**

```cpp
if (Registry::loadFromFile("data_registry.bin") && Registry::matchesVersion(currentVersion)) {
    Registry::useLoadedData();
} else {
    Registry::generate();
    Registry::saveToFile("data_registry.bin");
}
```

---

## **7. Cross-Language Code Generation**

| Language | Output | Tool |
|-----------|---------|------|
| Lua | `api_stubs.lua` | `sdom_bindgen_lua` |
| C | `sdom_c_api.h` | `sdom_bindgen_c` |
| Python | `sdom_py_bindings.py` | `sdom_bindgen_py` |
| **Rust** | `sdom_bindings.rs` | **`sdom_bindgen_rs`** |

The generator tools read the binary registry to emit language bindings using each function’s metadata and adapters.

---

## **8. Registry Visualization**

```cpp
void Registry::printRegistryTree(const RegistryNode& node, int depth = 0) {
    std::string indent(depth * 2, ' ');
    std::cout << indent << "Object Type: " << node.typeName << "\n";

    if (!node.functions.empty()) {
        std::cout << indent << "| Return Type | Function | Arguments | Description |\n";
        for (auto& f : node.functions) {
            std::cout << indent << "| " << f.returnType << " | " << f.name << " | ";
            for (auto& p : f.parameters) std::cout << p.type << " " << p.name << ", ";
            std::cout << " | " << f.description << " |\n";
        }
    }

    if (!node.properties.empty()) {
        std::cout << indent << "| Type | Property | Description |\n";
        for (auto& p : node.properties)
            std::cout << indent << "| " << p.type << " | " << p.name << " | " << p.description << " |\n";
    }

    for (auto& [_, child] : node.children)
        printRegistryTree(*child, depth + 1);
}
```

---

## **9. Advantages**

| Feature | Description |
|----------|--------------|
| 🧠 **Unified Reflection** | Registry describes all engine types, methods, and properties |
| ⚡ **Executable Entries** | Function and property adapters callable from any runtime |
| 🦀 **Cross-Language** | Supports Lua, C, Python, Rust |
| 💾 **Persistent** | Binary registry caching for fast startup |
| 🧩 **Language-Safe Adapters** | Lua bindings remain stable and native to Sol3 |
| 🔍 **Debuggable** | Structured dump output with full metadata |

---

## **10. Version Roadmap**

| Version | Milestone | Key Addition |
|----------|------------|---------------|
| **v0.6.x** | Core Registry Integration | Lua adapter fields added, replaces legacy bindings |
| **v0.7.x** | Visual Editor | Uses registry introspection for live editing |
| **v0.8.x** | Audio / Networking | New systems auto-register with IDataObject |
| **v0.9.x** | Feature Freeze | Registry schema locked; ABI finalization |

---

## **11. Summary**

The **Lua Adapter Extension** ensures that each registry function and property can provide both a **C++ variant-based** and **Lua-native** callable interface.  
This creates a fully flexible runtime reflection system that supports dynamic binding and binding generation across multiple languages while preserving native semantics for each.

---
