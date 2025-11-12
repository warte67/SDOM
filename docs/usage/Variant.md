# Variant Usage and Examples

This page contains longer, tutorial-style examples for the `Variant` type: registering converters, creating dynamic values, snapshotting Lua tables, and using `VariantView` for read-only access.

## Registering a Converter

To interoperate custom C++ types with Lua and `Variant`, register a `Variant::ConverterEntry` for your type:

```cpp
struct TestPoint { int x; int y; };

Variant::ConverterEntry ce;
ce.toLua = [](const VariantStorage::DynamicValue& dv, sol::state_view L)->sol::object {
    auto p = std::static_pointer_cast<TestPoint>(dv.ptr);
    sol::table t = L.create_table();
    t["x"] = p->x;
    t["y"] = p->y;
    return t;
};

ce.fromVariant = [](const Variant& v)->std::shared_ptr<void> {
    auto out = std::make_shared<TestPoint>();
    if (v.isObject()) {
        const Variant* vx = v.get("x");
        const Variant* vy = v.get("y");
        if (vx) out->x = static_cast<int>(vx->toInt64());
        if (vy) out->y = static_cast<int>(vy->toInt64());
        return std::static_pointer_cast<void>(out);
    }
    return nullptr; // unsupported shape
};

Variant::registerConverter<TestPoint>("TestPoint", std::move(ce));
```

The name (`"TestPoint"`) is optional but convenient: you can look up converters by `std::type_index` or by name via `getConverterByName`.

---

## Creating Dynamic Variants

Use `Variant::makeDynamic` or `Variant::makeDynamicValue` to create dynamic-held values:

```cpp
auto p = std::make_shared<TestPoint>(TestPoint{7,11});
Variant v = Variant::makeDynamic<TestPoint>(p);
// or
Variant v2(p); // constructor overload that takes a shared_ptr<T>
```

`Variant::toLua` will consult the registered converter to produce a Lua value.

---

## Snapshotting Lua Tables and Table Storage Modes

By default, Lua tables converted into `Variant` are copied into `Variant` objects/arrays. To keep opaque Lua references for tables, change the global table storage mode:

```cpp
auto prev = Variant::getTableStorageMode();
Variant::setTableStorageMode(Variant::TableStorageMode::KeepLuaRef);

// Convert a table into a Variant that stores a LuaRef
sol::table t = L.create_table(); t["a"] = 123;
Variant v = Variant::fromLuaObject(t);
assert(v.isLuaRef());

// Make a deep copy snapshot (an object Variant) of the referenced table
Variant snap = v.snapshot();
assert(snap.isObject());

Variant::setTableStorageMode(prev);
```

---

## VariantView (Read-Only, Non-Owning)

`Variant::VariantView` provides efficient read-only access to a `Variant` without copying. It's useful for functions that should not mutate the underlying variant.

```cpp
Variant arr = Variant::makeArray();
arr.push(Variant(1)); arr.push(Variant(2));
Variant::VariantView view(arr);
if (view.isArray()) {
    for (size_t i=0;i<view.size();++i) {
        auto e = view.at(i);
        // e is a pointer to the original Variant elements
    }
}
```

---

## Example: Converter Lookup by Name

You can register converters by name and later retrieve them via `getConverterByName` when you don't have a `type_index` handy.

```cpp
auto ce = Variant::getConverterByName("TestPoint");
if (ce) {
    auto p = std::make_shared<TestPoint>(); p->x=3; p->y=4;
    auto dyn = Variant::makeDynamicValue<TestPoint>(p);
    sol::object o = ce->toLua(dyn, L);
}
```

---

## Roundtrip Conversion (C++ ↔ Lua ↔ C++)

Demonstrate how a registered converter performs a full roundtrip through Lua and back to C++.

```cpp
// Assume TestPoint converter is already registered.
sol::state L;
L.open_libraries(sol::lib::base);

// Create C++ object and wrap as Variant
auto p = std::make_shared<TestPoint>(TestPoint{5, 9});
Variant v = Variant::makeDynamic<TestPoint>(p);

// Convert to Lua table
sol::object luaObj = v.toLua(L);
L["pt"] = luaObj;

// Modify from Lua
L.script("pt.x = pt.x + 10; pt.y = pt.y * 2");

// Deserialize back into C++
Variant updated = Variant::fromLuaObject(L["pt"]);
// obtain the dynamic pointer from the Variant and cast to the concrete type
// (many codebases provide either `asDynamic()` -> std::shared_ptr<void>
// or a templated `asDynamic<T>()` helper; here we use the non-templated
// form and cast explicitly for clarity)
auto dyn_ptr = updated.asDynamic(); // std::shared_ptr<void>
auto p2 = std::static_pointer_cast<TestPoint>(dyn_ptr);

std::cout << "x=" << p2->x << ", y=" << p2->y << std::endl;
// Expected output: x=15, y=18
```

This shows how Variants support editable Lua-side objects with transparent re-import into C++ via the registered converter.

---

## Nested Table Conversion

Illustrate recursive mapping of Lua tables into nested `Variant` objects.

```cpp
sol::state L;
L.open_libraries(sol::lib::base);

L.script(R"(
    cfg = {
        window = { width = 640, height = 480 },
        title  = "Hello SDOM",
        flags  = { fullscreen = false, vsync = true }
    }
)");

Variant v = Variant::fromLuaObject(L["cfg"]);
assert(v.isObject());

const Variant* window = v.get("window");
std::cout << "Resolution: "
          << window->get("width")->toInt64() << "x"
          << window->get("height")->toInt64() << std::endl;
```

This shows how arbitrary Lua data maps into hierarchical Variant structures.

---

## Thread-Safe Converter Registration

Demonstrate that the registry safely handles concurrent converter registration and lookup.

```cpp
#include <thread>

void registerTypes() {
    Variant::ConverterEntry ce = /* ... */;
    Variant::registerConverterByName("ThreadedType", std::move(ce));
}

void lookupTypes() {
    auto ce = Variant::getConverterByName("ThreadedType");
    if (ce) { /* use converter */ }
}

int main() {
    std::thread t1(registerTypes);
    std::thread t2(lookupTypes);
    t1.join(); t2.join();
}
```

The registry’s internal mutex ensures correctness under multithreaded registration and lookup.

---

## Error Handling and Debugging Example

Show a realistic case where a conversion fails and produces a meaningful diagnostic.

```cpp
Variant v = Variant::fromLuaObject(L["missing_field_table"]);

if (v.hasError()) {
    SDOM_LOG_ERROR("Variant error: {}", v.errorMessage());
    std::cout << v.toDebugString() << std::endl;
}
```

Output example:
```
VariantError: key 'x' → expected number, got nil
{ "y": 10 }
```

This demonstrates how `Variant` remains valid and inspectable even when conversion fails.

---

## Using VariantView in Registry Inspection

Show a high-level reflection use case — iterating over Variants without ownership transfer.

```cpp
void printObjectKeys(const Variant& v) {
    Variant::VariantView view(v);
    if (!view.isObject()) return;
    for (size_t i = 0; i < view.size(); ++i) {
        const Variant* val = view.at(i);
        if (val) std::cout << val->toString() << "\n";
    }
}
```

---

## Best Practices

- Always ensure `ConverterEntry` functions are **exception-safe** and **thread-safe**.  
  Converters may run from multiple threads during Lua reflection or asynchronous serialization.
- Use `Variant::makeDynamicValue<T>` when you need a raw `DynamicValue` to pass to converters directly.
- Prefer `Variant::makeDynamic<T>` when you’re producing a full `Variant` for reflection or script access.
- Keep converters pure — they should not mutate global Lua state, only construct or read tables.
- When using `KeepLuaRef`, remember that the referenced table’s lifetime is tied to its Lua state.  
  Snapshot before the Lua VM shuts down to avoid invalid references.

---

## Integration with DataRegistry

In reflection contexts, Variants serve as the serialization and inspection backbone for `IDataObject`-derived classes.  
When a registered type exposes properties or fields, the `DataRegistry` automatically wraps them in `Variant` instances.  
Converters registered via `Variant::registerConverter<T>()` are invoked automatically whenever those properties cross the Lua/C++ boundary.

---

## Troubleshooting Common Pitfalls

- ❗ Failing to snapshot a LuaRef before the VM shuts down can lead to invalid references.  
- ⚙️ Forgetting `explicit` on converter lambdas may cause ambiguous overloads.  
- 🧵 Registering converters in multiple threads is supported, but initialization order still matters.  
- 💾 Always verify `Variant::hasError()` after deserialization from Lua, especially when reading from user scripts.

---

## Summary

This page provides an end-to-end guide to Variant usage: from registering custom converters to snapshotting Lua data and inspecting objects read-only.  
Together with the [SDOM Variant Design Specification](../extensible_variant.md), it forms the complete developer reference for SDOM’s type system and cross-language reflection model.
