# ⚙️ SDOM::DataRegistry — C API Generation Subsystem  
**Updated:** November 11, 2025  
**Author:** Jay Faries (warte67)

---

## 🧠 Purpose

> ⚠️ **Note:** This document is a work in progress. None of the interfaces or APIs are set in stone yet. It serves as a *brainstorming and conceptual design* draft — the final implementation will likely evolve based on experimentation and integration feedback.

The **DataRegistry** will include a subcomponent responsible for generating C-compatible header files that mirror the runtime reflection state of SDOM.  
This allows `extern "C"` bindings — for C, Rust, Zig, and others — to be automatically produced as a byproduct of runtime type registration.

In effect, SDOM’s runtime registry becomes a **live reflection compiler** that can emit a minimal procedural interface similar to SDL.

---

## 🧩 Conceptual Flow
```
┌──────────────────────────────┐
│       SDOM::Factory          │
│  - owns DataRegistry         │
│  - registers all types       │
└─────────────┬────────────────┘
              │
              ▼
┌──────────────────────────────┐
│       SDOM::DataRegistry     │
│  - stores TypeInfo metadata  │
│  - exposes reflection API    │
│  - owns IBindingGenerator 🔹 │
└─────────────┬────────────────┘
              │
              ▼
┌──────────────────────────────┐
│   SDOM::IBindingGenerator    │
│  - emits C headers (.h)      │
│  - emits optional .c stubs   │
│  - synchronizes with Factory │
└──────────────────────────────┘
```
---

## 🧩 IBindingGenerator Responsibilities

| Task | Description |
|------|--------------|
| 🔍 **Scan Reflection Data** | Enumerate all registered `TypeInfo`, `PropertyInfo`, and `FunctionInfo` records. |
| 🧱 **Generate Header Prototypes** | Write out `.h` files with proper `extern "C"` signatures. |
| ⚙️ **Generate Procedural Wrappers** | Optionally emit `.c` glue functions that call into the C++ Core. |
| 🔗 **Integrate with Factory** | Allow runtime `Factory::exportBindings(path)` to trigger header generation. |
| 🧩 **Emit Opaque Types** | Automatically declare opaque handle types like `typedef struct SDOM_Button SDOM_Button;`. |
| 🧠 **Track ABI Metadata** | Optionally emit `.json` or `.toml` reflection data for downstream bindings (Rust, Python, Zig). |

---

## 🧩 File Generation Layout

```
/build/generated/
├── sdom_capi.h
├── sdom_capi_core.h
├── sdom_capi_objects.h
├── sdom_capi_assets.h
└── sdom_capi_types.json
```

Example of **sdom_capi_objects.h**:

```cpp
#ifndef SDOM_CAPI_OBJECTS_H
#define SDOM_CAPI_OBJECTS_H

#ifdef __cplusplus
extern "C" {
#endif

    // Opaque type declarations
    typedef struct SDOM_Button SDOM_Button;
    typedef struct SDOM_Label SDOM_Label;

    // Function bindings (auto-generated)
    SDOM_Button* SDOM_Create_Button();
    void SDOM_Destroy_Button(SDOM_Button*);

    const char* SDOM_Button_getText(SDOM_Button*);
    void SDOM_Button_setText(SDOM_Button*, const char* text);
    void SDOM_Button_click(SDOM_Button*);

    // ...

#ifdef __cplusplus
}
#endif

#endif // SDOM_CAPI_OBJECTS_H
```

---

## 🧱 Integration Points

### 1. Factory Hook

`Factory::exportBindings(const std::string& outputDir)`

This method forwards to the registry’s binding generator:

```cpp
bool Factory::exportBindings(const std::string& path) {
    return registry_.generateBindings(path);
}
```

---

### 2. Registry Subsystem Composition

Inside **DataRegistry.hpp**:

```cpp
class DataRegistry {
public:
    bool generateBindings(const std::string& path);
private:
    std::unordered_map<std::string, TypeInfo> types_;
    std::unique_ptr<class IBindingGenerator> cgen_; // 🔹 new component
};
```

---

### 3. Invocation Example

```cpp
Core::getFactory().getRegistry().generateIBindings("./generated");
```

At runtime, this produces headers under `/generated/` reflecting all current type registrations.

---

## 🧩 Code Generation Rules

| Element | Rule |
|----------|------|
| **Type Name** | Mapped to `SDOM_<Type>` prefix, e.g., `SDOM_Button`. |
| **Function Name** | `SDOM_<Type>_<Function>` (e.g., `SDOM_Button_click`). |
| **Property Getter/Setter** | Two functions per property: `SDOM_<Type>_get<Property>` / `SDOM_<Type>_set<Property>`. |
| **Namespace** | All global — C style. |
| **Safety** | Always `extern "C"` with opaque handles. |
| **Return Type** | Default to `int` or `void*` unless overridden by reflection metadata. |
| **Variant Support** | Functions taking or returning `Variant` will expose `void*` or `const char*` stubs with serialization helpers. |

---

## 🧩 Example Generated Mapping

### Properties

| Type | Property | C Getter | C Setter |
|------|-----------|-----------|----------|
| `Button` | `text` | `SDOM_Button_getText(SDOM_Button*)` | `SDOM_Button_setText(SDOM_Button*, const char*)` |
| `Button` | `enabled` | `SDOM_Button_getEnabled(SDOM_Button*)` | `SDOM_Button_setEnabled(SDOM_Button*, bool)` |
| `Label` | `font` | `SDOM_Label_getFont(SDOM_Label*)` | `SDOM_Label_setFont(SDOM_Label*, SDOM_Font*)` |

### Functions

| Type | Function | C Wrapper |
|------|-----------|-----------|
| `Button` | `click()` | `SDOM_Button_click(SDOM_Button*)` |
| `Label` | `measureText()` | `SDOM_Label_measureText(SDOM_Label*, const char*)` |

---

## 🧠 Reflection-to-C Translation Layer

The `IBindingGenerator` uses helper methods:

- std::string generateHeaderPreamble()
- std::string generateOpaqueDeclarations(const TypeInfo&)
- std::string generateFunctionPrototypes(const TypeInfo&)
- std::string generatePropertyPrototypes(const TypeInfo&)
- std::string generateHeaderEpilogue()

Each stage concatenates output to a buffer and writes to disk.

---

## 🧪 Planned Unit Tests: `IBindingGenerator_UnitTests.cpp`

| Test | Purpose |
|------|----------|
| **generate_basic_header** | Ensure header file is created and contains extern "C" section |
| **property_function_translation** | Validate correct getter/setter name generation |
| **multi_type_generation** | Ensure multiple types emit in a single combined header |
| **output_directory_creation** | Confirm directories auto-create if missing |
| **header_idempotence** | Ensure re-generation overwrites safely, no duplicates |

---

## 🔮 Future Extensions

| Idea | Description |
|------|--------------|
| 🧩 **Language Adapters** | Generate optional Rust `bindgen` templates or Zig `@cImport` wrappers. |
| 🧾 **Metadata Export** | Emit a `.json` file describing argument types, defaults, etc. |
| 🧬 **Selective Export** | Allow inclusion/exclusion filters (e.g., only export “public” types). |
| 🔗 **Dynamic ABI Loader** | Optionally compile a small shared library exposing the generated C layer at runtime. |

---

# ⚙️ SDOM::IBindingGenerator — Universal Binding Generation Framework

## 🧠 Purpose

`IBindingGenerator` defines the **abstract interface** for all language binding generators in SDOM.  
It provides a common foundation for exporting the **runtime reflection data** held within the `DataRegistry` into various foreign language interfaces such as:

- C (ABI stable)
- Lua (runtime integrated)
- Rust (via bindgen)
- Zig or Python (via C FFI or codegen)

By separating the generator interface from its implementations, SDOM gains a *modular reflection emission pipeline* that can scale to multiple languages without disrupting the registry core.

---

## 🧩 High-Level Overview
```
┌────────────────────────────────────────────┐
│            SDOM::Factory                   │
│   - owns DataRegistry                      │
│   - calls registry.exportBindings()        │
└────────────────────────────────────────────┘
                               │
                               ▼
┌────────────────────────────────────────────┐
│           SDOM::DataRegistry               │
│  - stores TypeInfo metadata                │
│  - holds collection of BindingGenerators   │
└────────────────────────────────────────────┘
                               │
              ┌────────────────┴────────────────┐
              ▼                                 ▼
┌─────────────────────────────┐   ┌──────────────────────────────┐
│   SDOM::IBindingGenerator   │   │   SDOM::RustBindingGenerator │
│  - emits .h / .c headers    │   │  - emits .rs FFI wrappers    │
│  - opaque handles, ABI safe │   │  - uses extern "C" stubs     │
└─────────────────────────────┘   └──────────────────────────────┘
```
---

## 🧱 Interface Contract

```cpp
namespace SDOM {

class IBindingGenerator {
public:
    virtual ~IBindingGenerator() = default;

    virtual void initialize(DataRegistry* registry) { registry_ = registry; }
    virtual bool generate(const std::string& outputDir) = 0;
    virtual std::string getName() const = 0;

    virtual void setFilter(std::function<bool(const TypeInfo&)> filter) {
        filter_ = std::move(filter);
    }

protected:
    DataRegistry* registry_ = nullptr;
    std::function<bool(const TypeInfo&)> filter_;
};

} // namespace SDOM
```

---

## 🧩 Example Derived Class: `IBindingGenerator`

```cpp
class IBindingGenerator : public IBindingGenerator {
public:
    std::string getName() const override { return "IBindingGenerator"; }
    bool generate(const std::string& outputDir) override;
private:
    std::string generateHeaderPreamble() const;
    std::string generateOpaqueDeclarations(const TypeInfo&) const;
    std::string generatePropertyPrototypes(const TypeInfo&) const;
    std::string generateFunctionPrototypes(const TypeInfo&) const;
    std::string generateHeaderEpilogue() const;
};
```

---

## 🧩 Integration Within DataRegistry

```cpp
class DataRegistry {
public:
    void addGenerator(std::unique_ptr<IBindingGenerator> gen);
    bool exportAllBindings(const std::string& outputDir);
    const std::unordered_map<std::string, TypeInfo>& getAllTypes() const;
private:
    std::unordered_map<std::string, TypeInfo> types_;
    std::vector<std::unique_ptr<IBindingGenerator>> generators_;
};
```

---

## 🧠 Design Philosophy

| Principle | Description |
|------------|--------------|
| 🔁 **Extensible** | Add new generators (C, Rust, Lua, etc.) without touching registry core. |
| ⚙️ **Composable** | Multiple generators can coexist, emitting bindings for several targets simultaneously. |
| 🧱 **Type-Agnostic** | Generators use the same `TypeInfo` reflection schema. |
| 🧩 **Filterable** | Registry can selectively export subsets of types (e.g., only runtime-available, or editor-visible). |
| 🧬 **Self-Describing** | Each generator reports its type and output location for introspection. |

---

## 🧪 Planned Unit Tests: `IBindingGenerator_UnitTests.cpp`

| Test | Description |
|------|--------------|
| **generator_registration** | Verify multiple generators can be added and invoked in sequence. |
| **c_binding_output** | Confirm `IBindingGenerator` emits valid `.h` file. |
| **filtering** | Ensure custom type filter excludes unwanted entries. |
| **failure_recovery** | Handle generator failure gracefully without registry crash. |

---

## 🔮 Future Extensions

| Feature | Description |
|----------|--------------|
| 🧩 **RustBindingGenerator** | Emits `.rs` FFI bindings using extern "C" stubs. |
| 🧩 **LuaBindingGenerator** | Uses runtime Lua metatables to reflect DataRegistry contents into Lua space. |
| 🧩 **IDL Exporter** | Generates `.json` or `.idl` reflection schemas for language-agnostic pipelines. |
| 🧩 **Hybrid Codegen** | Combine compile-time and runtime reflection for header synchronization. |

---

## 🧭 Summary

| Aspect | Description |
|---------|--------------|
| **Interface** | `IBindingGenerator` — a base for language-specific binding emitters. |
| **Concrete Example** | `IBindingGenerator` — emits SDL-style `.h` headers. |
| **Ownership** | Managed by `DataRegistry`. |
| **Benefits** | Extensible, language-agnostic, modular reflection emission system. |
| **Next Step** | Implement the `IBindingGenerator` base class and integrate registration in `DataRegistry::initialize()`. |

---

> “Reflection ends where generation begins — the moment knowledge of type becomes knowledge of language.”  
> — ⚖️ *SDOM Reflection Principle, Phase III*


---

# 🧭 Next Step Implementation Plan — Bootstrapping the Binding Generator System

## Phase 1 — Concrete “BindingGenerator” Prototype
**Goal:** Generate the `extern "C"` API header directly before abstraction.

This initial implementation should be embedded within the `DataRegistry` (or as a simple companion class).  
It focuses entirely on *practical output* and verifying the reflection pipeline.

**Responsibilities:**
- Enumerate all registered `TypeInfo` entries.
- Emit `typedef struct SDOM_<Type>` forward declarations.
- Generate property and command prototypes.
- Write a standalone `.h` file under `/generated/`.

**Example Skeleton:**
```cpp
class BindingGenerator {
public:
    explicit BindingGenerator(DataRegistry& registry);
    bool generateExternC(const std::string& outputPath);
};
```

**Deliverable:** A functioning proof-of-concept C header generator.

---

## Phase 2 — Extract Interface (`IBindingGenerator`)
After confirming the first generator works, evolve it into an interface:

```cpp
class IBindingGenerator {
public:
    virtual ~IBindingGenerator() = default;
    virtual bool generate(const std::string& outputDir) = 0;
    virtual std::string getName() const = 0;
};
```

Refactor the original generator into:

```cpp
class IBindingGenerator final : public IBindingGenerator {
public:
    std::string getName() const override { return "IBindingGenerator"; }
    bool generate(const std::string& outputDir) override;
};
```

The interface should be defined *based on actual usage* from the concrete prototype.

---

## Phase 3 — Integrate with DataRegistry
Embed a flexible generator registration system:

```cpp
registry.addGenerator(std::make_unique<IBindingGenerator>());
registry.exportAllBindings("./generated");
```

**Future extensions:**
- RustBindingGenerator
- LuaBindingGenerator
- IDLBindingGenerator

All can reuse the reflection metadata provided by `DataRegistry` do generate language specific **Interface Declarations**.

**What is an Interface Declaration?**  
An **Interface Declaration** is a generated description of how external programs, scripts, or languages can interact with SDOM objects, properties, and functions — without exposing any internal implementation details.  
In **C/C++**, it typically takes the form of a single `extern "C"` header file containing function prototypes, type forward declarations, and opaque handles (e.g. `SDOM_Core`, `SDOM_DisplayHandle`).  
In **Rust**, it manifests as an `extern "C"` module with `#[link_name]` FFI declarations.  
In **Lua**, it can appear as a runtime reflection table or automatically generated bindings.  
For **IDL** or schema-based formats, it may exist as a structured manifest that other generators can consume.  

Regardless of language, all Interface Declarations serve the same purpose — to provide a **stable, language-appropriate interface contract** between SDOM’s internal reflection system and the external environment.  
These declarations act as **bridges of intent**, defining how other runtimes and tools see the SDOM engine.


---

## Phase 4 — Rework the UnitTest Modules
Once the generator can reliably emit extern “C” bindings:

1. **Regenerate headers** for test modules (`Variant`, `Core`, `Factory`, etc.).
2. **Split tests** into:
   - Native C++ unit tests
   - Extern-layer ABI validation tests (using generated headers)

This ensures ABI consistency and synchronization between runtime reflection and generated API.

---

## Phase 5 — Automate Validation
Create a dedicated test suite for the generator itself:
- Validate generated headers compile cleanly (`gcc -c`).
- Confirm expected function prototypes exist.
- Compare symbol signatures via hash to detect API drift.

---

## Summary
This staged plan ensures:
- Early, working results (Phase 1).
- Organic evolution of the `IBindingGenerator` contract.
- A unified reflection-to-binding pipeline.
- Foundation for multi-language export.
- Readiness for the upcoming **UnitTest system rewrite**.

---

> “Implementation reveals the architecture; reflection reveals the intent.”  
> — *SDOM Reflection Journal, Entry III*











---
# 🚧 **Under Consstruction** 🚧

>
> ⁉️ Not sure if this next section should be another document or integrated into the current one.
>


# 🧩 SDOM Reflection and Interface Declaration Type Mapping
**Updated:** November 11, 2025  
**Author:** Jay Faries (warte67)

---

## ⚙️ Core Idea: Separation of Concern Between Reflection and Representation

### 🧠 `DataRegistry` Knows
- The *true* runtime type (`int`, `float`, `std::string`, `bool`, `Variant`, `std::shared_ptr<IDataObject>`, etc.)
- Whether the entry is a **property** or a **function**
- What its **signature** and **arity** are
- The **underlying C++ type info** (via templated registration or RTTI)

### 🧩 Each `IBindingGenerator` Knows
- The *target language’s* conventions and syntax
- How to map fundamental SDOM types → foreign types  
  (e.g., `int` → `int32_t` in C, `i32` in Rust, `number` in Lua)
- How to represent ownership, references, and strings
- How to name things (e.g., `SDOM_Foo_Bar` vs `Foo.bar`)

---

## 🧱 Strategy: Type Conversion Registry (Per Generator)

Each generator defines a **local translation table** — a dictionary of type mappings — that converts from canonical SDOM type names to target-language representations.

### Example: C_BindingGenerator

```cpp
static const std::unordered_map<std::string, std::string> CTypeMap = {
    {"int", "int32_t"},
    {"float", "float"},
    {"double", "double"},
    {"bool", "bool"},
    {"std::string", "const char*"},
    {"Variant", "void*"},
    {"IDataObject*", "void*"},
};
```

### Example: Rust_BindingGenerator

```cpp
static const std::unordered_map<std::string, std::string> RustTypeMap = {
    {"int", "i32"},
    {"float", "f32"},
    {"double", "f64"},
    {"bool", "bool"},
    {"std::string", "*const c_char"},
    {"Variant", "*mut c_void"},
};
```

---

## 🧩 Hooking It Together

Each generator can query and translate via a local helper:

```cpp
std::string IBindingGenerator::mapType(const std::string& cppType) const {
    if (auto it = typeMap_.find(cppType); it != typeMap_.end())
        return it->second;
    return "void*"; // fallback
}
```

This allows each generator to interpret the same reflection data differently.

---

## 🧩 Data Type Metadata Schema

The reflection schema might look like this:

```cpp
struct TypeInfo {
    std::string name;
    std::vector<PropertyInfo> properties;
    std::vector<FunctionInfo> functions;
};

struct PropertyInfo {
    std::string name;
    std::string cppType;
    bool readOnly;
};

struct FunctionInfo {
    std::string name;
    std::string returnType;
    std::vector<std::string> paramTypes;
};
```

That’s all `DataRegistry` needs to describe.  
From there, `IBindingGenerator` handles language translation.

---

## 🧠 Future Upgrade: TypeAdapter Plug-ins

To keep logic reusable across generators, introduce an adapter interface:

```cpp
class ITypeAdapter {
public:
    virtual std::string mapType(const std::string& cppType) const = 0;
    virtual std::string defaultValue(const std::string& cppType) const = 0;
};
```

Then specialize per language:

```cpp
CTypeAdapter CAdapter;
RustTypeAdapter RAdapter;
```

Each generator simply chooses the correct adapter for its language.

---

## 🔮 Summary of the Approach

| Layer | Responsibility |
|--------|----------------|
| **DataRegistry** | Defines what exists — the reflection truth. |
| **TypeInfo / PropertyInfo / FunctionInfo** | Describe the shape of types and members. |
| **IBindingGenerator** | Defines *how* to represent these in a specific language. |
| **TypeAdapter (optional)** | Centralized conversion logic for data type translation. |

---

## ✨ Key Architectural Takeaway

> SDOM’s reflection layer should *describe reality*, not *interpret it.*  
> Each `IBindingGenerator` is a **translator**, not a **scribe** —  
> it decides how to express that reality in another tongue.
