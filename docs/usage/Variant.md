# Variant usage and examples

This page contains longer, tutorial-style examples for the `Variant` type: registering converters, creating dynamic values, snapshotting Lua tables, and using `VariantView` for read-only access.

## Registering a converter

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

## Creating dynamic variants

Use `Variant::makeDynamic` or `Variant::makeDynamicValue` to create dynamic-held values:

```cpp
auto p = std::make_shared<TestPoint>(TestPoint{7,11});
Variant v = Variant::makeDynamic<TestPoint>(p);
// or
Variant v2(p); // constructor overload that takes a shared_ptr<T>
```

`Variant::toLua` will consult the registered converter to produce a Lua value.

## Snapshotting Lua tables and table storage modes

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

## VariantView (read-only, non-owning)

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

## Example: converter lookup by name

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

These examples are expanded from the brief notes in the header `include/SDOM/SDOM_Variant.hpp`. Use this page for tutorial-style documentation and longer examples that are convenient to render to HTML or a wiki.
