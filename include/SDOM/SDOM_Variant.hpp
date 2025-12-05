
#pragma once
#ifndef SDOM_VARIANT_HPP
#define SDOM_VARIANT_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <typeindex>
#include <optional>

#include <mutex>
#include <functional>

// SDOM ships with Lua/sol; Variant integrates directly
#include <sol/sol.hpp>

namespace SDOM {

// ─────────────────────────────────────────────────────────────────────────────
// Variant type tag
// ─────────────────────────────────────────────────────────────────────────────
enum class VariantType : uint8_t {
    Null = 0,
    Bool,
    Int,
    Real,
    String,
    Array,      // std::vector<Variant>
    Object,     // std::unordered_map<std::string, Variant>
    Dynamic,    // std::shared_ptr<void> (extensible user type)
    LuaRef,     // sol::object (opaque reference)
    Error
};

class Variant;

// ─────────────────────────────────────────────────────────────────────────────
// Internal storage node (shared so copies are cheap)
// ─────────────────────────────────────────────────────────────────────────────
struct VariantStorage {
    // Use indirection for containers so Variant can be forward-declared
    // without requiring a complete type at the point these aliases are
    // instantiated. Using shared_ptr keeps ownership semantics simple and
    // avoids changing external APIs dramatically at the call site; callers
    // that iterate elements will now receive shared_ptr<Variant> elements
    // and should dereference them.
    using Array     = std::vector<std::shared_ptr<Variant>>;
    using Object    = std::unordered_map<std::string, std::shared_ptr<Variant>>;
    struct DynamicValue {
        std::shared_ptr<void> ptr;
        std::type_index type{typeid(void)};
        std::string typeName;

        bool operator==(const DynamicValue& o) const noexcept {
            return ptr == o.ptr && type == o.type && typeName == o.typeName;
        }
    };

    struct LuaRefValue {
        sol::object obj;
        lua_State* L = nullptr;

        LuaRefValue() = default;
        LuaRefValue(const sol::object& o)
            : obj(o)
            , L(o.lua_state())
        {}
        bool operator==(const LuaRefValue& o) const noexcept {
            return L == o.L && obj == o.obj;
        }
        bool validFor(sol::state_view sv) const noexcept {
            return L == sv.lua_state() && obj.valid();
        }
    };

    // Note: Variant stores Lua references in a small wrapper (LuaRefValue)
    // which keeps the `sol::object` and the originating `lua_State*` so
    // accesses can validate the reference against the target state. This
    // prevents returning a `sol::object` bound to a different `lua_State`.
    //
    // Tables are treated specially: by default they are converted (deep
    // copied) into Variant `Array`/`Object` structures (this acts as a
    // snapshot of the table contents). A global table storage mode allows
    // callers to request that tables be stored as Lua references instead
    // (i.e. kept as `LuaRefValue`) when desired.

        using Data = std::variant<
            std::monostate, // Null
            bool,
            int64_t,
            double,
            std::string,
            Array,
            Object,
            DynamicValue,
            LuaRefValue
        >;

    Data data{};
};

// ─────────────────────────────────────────────────────────────────────────────
// Variant class
// ─────────────────────────────────────────────────────────────────────────────
class Variant {
public:
    // Constructors
    Variant();                                      // Null
    // (removed Variant(std::nullptr_t) overload — use default ctor for null)
    explicit Variant(bool b);
    explicit Variant(int32_t i);
    explicit Variant(int64_t i);
    explicit Variant(double d);
    explicit Variant(const char* s);
    explicit Variant(std::string s);
    explicit Variant(const std::vector<Variant>& arr);
    explicit Variant(std::vector<Variant>&& arr);
    explicit Variant(const std::unordered_map<std::string, Variant>& obj);
    explicit Variant(std::unordered_map<std::string, Variant>&& obj);
    explicit Variant(const sol::object& o);

    // Explicitly opt into move semantics to document intent and allow
    // noexcept move operations for container optimizations.
    Variant(Variant&&) noexcept = default;
    Variant& operator=(Variant&&) noexcept = default;

    // Ensure Variant remains copyable: explicitly default copy ctor/assign.
    // If we only declare move operations, the copy operations would be
    // implicitly declared as deleted which breaks STL containers that
    // require copy-constructible/assignable elements (e.g. vector/unordered_map).
    Variant(const Variant&) = default;
    Variant& operator=(const Variant&) = default;

    // Factories for composite
    static Variant makeArray();
    static Variant makeObject();

    // Error helpers
    static Variant makeError(const std::string& msg);
    bool hasError() const noexcept { return errorFlag_; }
    const std::string& errorMessage() const noexcept { return errorMsg_; }

    // Type info
    VariantType type() const noexcept;
    bool isNull()   const noexcept { return std::holds_alternative<std::monostate>(storage_->data); }
    bool isBool()   const noexcept { return std::holds_alternative<bool>(storage_->data); }
    bool isInt()    const noexcept { return std::holds_alternative<int64_t>(storage_->data); }
    bool isReal()   const noexcept { return std::holds_alternative<double>(storage_->data); }
    bool isString() const noexcept { return std::holds_alternative<std::string>(storage_->data); }
    bool isArray()  const noexcept { return std::holds_alternative<VariantStorage::Array>(storage_->data); }
    bool isObject() const noexcept { return std::holds_alternative<VariantStorage::Object>(storage_->data); }
    bool isLuaRef() const noexcept { return std::holds_alternative<VariantStorage::LuaRefValue>(storage_->data); }
    bool isDynamic()const noexcept { return std::holds_alternative<VariantStorage::DynamicValue>(storage_->data); }

    // Conversions (safe, no-throw; return default on mismatch)
    bool                toBool(bool def=false) const noexcept;
    int64_t             toInt64(int64_t def=0) const noexcept;
    double              toDouble(double def=0.0) const noexcept;
    std::string         toString(std::string def={}) const noexcept;

    // Array/Object helpers (return nullptr if not that type)
    const VariantStorage::Array*  array()  const noexcept;
    VariantStorage::Array*        array()  noexcept;
    const VariantStorage::Object* object() const noexcept;
    VariantStorage::Object*       object() noexcept;

    // Dynamic helpers
    template<typename T>
    Variant(std::shared_ptr<T> p)
    : storage_(std::make_shared<VariantStorage>())
    {
        VariantStorage::DynamicValue dv;
        dv.ptr = std::static_pointer_cast<void>(p);
        dv.type = typeid(T);
        auto tn = VariantRegistry::getTypeName(dv.type);
        dv.typeName = tn ? *tn : std::string();
        storage_->data = dv;
    }

    template<typename T>
    std::shared_ptr<T> asDynamic() const {
        if (!std::holds_alternative<VariantStorage::DynamicValue>(storage_->data)) return nullptr;
        const auto& dv = std::get<VariantStorage::DynamicValue>(storage_->data);
        if (dv.type != typeid(T)) return nullptr;
        return std::static_pointer_cast<T>(dv.ptr);
    }

    // Create a DynamicValue from a typed shared_ptr. This centralizes type_name
    // lookup and ensures the DynamicValue is consistently populated.
    template<typename T>
    static VariantStorage::DynamicValue makeDynamicValue(std::shared_ptr<T> p) {
        VariantStorage::DynamicValue dv;
        dv.ptr = std::static_pointer_cast<void>(p);
        dv.type = std::type_index(typeid(T));
        auto tn = VariantRegistry::getTypeName(dv.type);
        dv.typeName = tn ? *tn : std::string();
        return dv;
    }

    // Create a Variant that holds a DynamicValue pointing to a typed shared_ptr.
    template<typename T>
    static Variant makeDynamic(std::shared_ptr<T> p) {
        // Use the templated dynamic constructor defined above.
        return Variant(std::move(p));
    }

    // In-place construction helper: construct T with forwarded args and wrap in Variant
    template<typename T, typename... Args>
    static Variant makeDynamic(Args&&... args) {
        return Variant(std::make_shared<T>(std::forward<Args>(args)...));
    }

    // Accessors for dynamic metadata
    std::optional<std::string> dynamicTypeName() const {
        if (!isDynamic()) return std::nullopt;
        const auto& dv = std::get<VariantStorage::DynamicValue>(storage_->data);
        return dv.typeName.empty() ? std::nullopt : std::optional<std::string>(dv.typeName);
    }

    std::type_index dynamicTypeIndex() const {
        if (!isDynamic()) return typeid(void);
        const auto& dv = std::get<VariantStorage::DynamicValue>(storage_->data);
        return dv.type;
    }

    // Minimal converter API stub for Dynamic -> native conversions (Lua disabled).
    // Only `fromVariant` is kept so callers can map Variants to native pointers.
    using VariantToDynamicFn = std::function<std::shared_ptr<void>(const Variant&)>;

    struct ConverterEntry {
        VariantToDynamicFn fromVariant;
    };

    // Register a converter for a C++ type T (native-only)
    template<typename T>
    static void registerConverter(const std::string& typeName, ConverterEntry entry) {
        VariantRegistry::registerType<T>(typeName);
        std::lock_guard<std::mutex> lk(VariantRegistry::getMutex());
        VariantRegistry::getConverterMap()[std::type_index(typeid(T))] = entry;
        VariantRegistry::getConverterMapByName()[typeName] = entry;
    }

    // Lookup converter by type_index (returns nullptr if absent)
    static ConverterEntry* getConverter(std::type_index t) {
        std::lock_guard<std::mutex> lk(VariantRegistry::getMutex());
        auto &m = VariantRegistry::getConverterMap();
        auto it = m.find(t);
        if (it == m.end()) return nullptr;
        return &it->second;
    }

    // Registry for external dynamic types (minimal: type name lookup)
    struct VariantRegistry {
        static void registerType(std::type_index t, const std::string& name) {
            std::lock_guard<std::mutex> lk(getMutex());
            getMap()[t] = name;
        }
        template<typename T>
        static void registerType(const std::string& name) { registerType(typeid(T), name); }
        static std::optional<std::string> getTypeName(std::type_index t) {
            std::lock_guard<std::mutex> lk(getMutex());
            auto &m = getMap();
            auto it = m.find(t);
            if (it == m.end()) return std::nullopt;
            return it->second;
        }
    public:
        static std::unordered_map<std::type_index, std::string>& getMap() {
            static std::unordered_map<std::type_index, std::string> map_;
            return map_;
        }
        static std::mutex& getMutex() {
            static std::mutex mtx;
            return mtx;
        }
        // Converter map accessors (native only)
        static std::unordered_map<std::type_index, ConverterEntry>& getConverterMap() {
            static std::unordered_map<std::type_index, ConverterEntry> conv;
            return conv;
        }
        static std::unordered_map<std::string, ConverterEntry>& getConverterMapByName() {
            static std::unordered_map<std::string, ConverterEntry> convByName;
            return convByName;
        }
        static ConverterEntry* getConverterByName(const std::string& name) {
            std::lock_guard<std::mutex> lk(getMutex());
            auto &m = getConverterMapByName();
            auto it = m.find(name);
            if (it == m.end()) return nullptr;
            return &it->second;
        }
    private:
    };

    // Lookup converter by registered type name (returns nullptr if absent)
    static ConverterEntry* getConverterByName(const std::string& name) {
        return VariantRegistry::getConverterByName(name);
    }

    // Register a converter by name only. This is useful for runtime-only types
    // where a concrete C++ type_index may not be available at registration time.
    static void registerConverterByName(const std::string& name, ConverterEntry entry) {
        std::lock_guard<std::mutex> lk(VariantRegistry::getMutex());
        VariantRegistry::getConverterMapByName()[name] = std::move(entry);
    }

    // -------------------- Inline API usage notes --------------------
    // The following short examples are intended to live close to the API so
    // future readers can quickly understand common usage patterns.
    //
    // Registering a converter by name only
    // ------------------------------------
    // Variant::registerConverterByName("RuntimeOnlyType", std::move(ce));
    //
    // Constructing dynamic Variants
    // -----------------------------
    // auto v = Variant::makeDynamic<MyType>(std::make_shared<MyType>(...));
    // auto v2 = Variant::makeDynamic<MyType>(/* constructor args for MyType */);
    // auto dv = Variant::makeDynamicValue<MyType>(shared_ptr<MyType>);
    //
    // Snapshotting & Lua table ownership
    // ----------------------------------
    // Variant::setTableStorageMode(Variant::TableStorageMode::KeepLuaRef);
    // Variant luaRef = Variant::fromLuaObject(some_table);
    // Variant copy = luaRef.snapshot(); // deep-copied Variant::Object/Array
    // Variant::setTableStorageMode(Variant::TableStorageMode::Copy);
    //
    // VariantView read-only traversal
    // -------------------------------
    // Variant::VariantView view(variant_instance);
    // if (view.isObject()) { if (const Variant* v = view.get("key")) { /* inspect v */ } }
    // ------------------------------------------------------------------------------

    // Table storage mode controls how incoming Lua tables are handled:
    // - Copy (default): tables are converted into Variant::Array/Object (a snapshot)
    // - KeepLuaRef: tables are stored as a LuaRefValue (opaque reference tied to the lua_State)
    enum class TableStorageMode : uint8_t {
        Copy = 0,
        KeepLuaRef
    };

    // Global table storage mode. Inline static ensures internal linkage and
    // avoids needing an out-of-line definition.
    inline static TableStorageMode tableStorageMode = TableStorageMode::Copy;

    static void setTableStorageMode(TableStorageMode m) { tableStorageMode = m; }
    static TableStorageMode getTableStorageMode() { return tableStorageMode; }

    // Structured helpers
    // Array
    size_t size() const noexcept;
    void   push(Variant v);                // ensures Array
    const Variant* at(size_t i) const noexcept;
    Variant*       at(size_t i) noexcept;

    // Object
    void set(const std::string& key, Variant v);  // ensures Object
    const Variant* get(const std::string& key) const noexcept;
    Variant*       get(const std::string& key) noexcept;

    // Lua integration
    sol::object toLua(sol::state_view L) const;
    static Variant fromLuaObject(const sol::object& o);
    // If this Variant holds a LuaRef to a table, produce a deep-copied
    // Variant (Array/Object) snapshot of that table's contents. If this
    // Variant is not a LuaRef to a table, returns a copy of *this.
    Variant snapshot() const;
    // Snapshot into a deep-copied Variant using the provided Lua state.
    // If this Variant holds a LuaRefValue, the snapshot will only be
    // produced when the LuaRef is valid for the provided state; otherwise
    // a copy of *this is returned.
    Variant snapshot(sol::state_view L) const;

    // Non-noexcept structured stringifier for debugging that limits
    // recursion by depth. Depth=0 yields "..." for nested structures.
    std::string toDebugString(int depth = 1) const;

    // Non-owning view for read-only traversal
    class VariantView {
    public:
        VariantView() = default;
        VariantView(const Variant& v) : storage(v.storage_.get()) {}

        VariantType type() const noexcept {
            if (!storage) return VariantType::Error;
            // reuse Variant::type semantics without errorFlag
            const auto& d = storage->data;
            if (std::holds_alternative<std::monostate>(d)) return VariantType::Null;
            if (std::holds_alternative<bool>(d)) return VariantType::Bool;
            if (std::holds_alternative<int64_t>(d)) return VariantType::Int;
            if (std::holds_alternative<double>(d)) return VariantType::Real;
            if (std::holds_alternative<std::string>(d)) return VariantType::String;
            if (std::holds_alternative<VariantStorage::Array>(d)) return VariantType::Array;
            if (std::holds_alternative<VariantStorage::Object>(d)) return VariantType::Object;
            if (std::holds_alternative<VariantStorage::DynamicValue>(d)) return VariantType::Dynamic;
            if (std::holds_alternative<VariantStorage::LuaRefValue>(d)) return VariantType::LuaRef;
            return VariantType::Error;
        }

        bool isNull() const noexcept { return storage && std::holds_alternative<std::monostate>(storage->data); }
        bool isArray() const noexcept { return storage && std::holds_alternative<VariantStorage::Array>(storage->data); }
        bool isObject() const noexcept { return storage && std::holds_alternative<VariantStorage::Object>(storage->data); }
        bool isLuaRef() const noexcept { return storage && std::holds_alternative<VariantStorage::LuaRefValue>(storage->data); }

        size_t size() const noexcept {
            if (!storage) return 0;
                if (auto a = std::get_if<VariantStorage::Array>(&storage->data)) return a->size();
                if (auto o = std::get_if<VariantStorage::Object>(&storage->data)) return o->size();
            return 0;
        }

        const Variant* at(size_t i) const noexcept {
            if (!storage) return nullptr;
            if (auto a = std::get_if<VariantStorage::Array>(&storage->data)) {
                if (i < a->size() && (*a)[i]) return (*a)[i].get();
            }
            return nullptr;
        }

        const Variant* get(const std::string& key) const noexcept {
            if (!storage) return nullptr;
            if (auto o = std::get_if<VariantStorage::Object>(&storage->data)) {
                auto it = o->find(key);
                if (it != o->end() && it->second) return it->second.get();
            }
            return nullptr;
        }

    private:
        const VariantStorage* storage = nullptr;
    };

    // Operators
    bool operator==(const Variant& rhs) const;
    bool operator!=(const Variant& rhs) const { return !(*this == rhs); }

    // Allow VariantHash to access private storage for efficient hashing.
    friend struct VariantHash;

private:
    std::shared_ptr<VariantStorage> storage_;
    bool        errorFlag_ = false;
    std::string errorMsg_;

    // helpers
    static Variant fromLuaTable_(const sol::table& t, int depth = 0, std::unordered_set<const void*>* visited = nullptr);
    static Variant fromLuaNumber_(double n);
    static bool    numericEqual_(const Variant& a, const Variant& b);

    // Internal helper used by fromLuaObject to track recursion depth.
    static Variant fromLuaObject(const sol::object& o, int depth, std::unordered_set<const void*>* visited);
};

// Hash support for Variant: best-effort, consistent with operator== for
// the implemented cases. VariantHash is declared outside the class but
// is a friend so it can access internal storage for pointer-based types.
struct VariantHash {
    size_t operator()(const Variant& v) const noexcept {
        using std::size_t;
        using std::hash;
        switch (v.type()) {
            case VariantType::Null: return 0x9e3779b97f4a7c15ULL;
            case VariantType::Bool: return hash<bool>()(v.toBool());
            case VariantType::Int: return hash<int64_t>()(v.toInt64());
            case VariantType::Real: return hash<long double>()(v.toDouble());
            case VariantType::String: return hash<std::string>()(v.toString());
            case VariantType::Array: {
                size_t h = 1469598103934665603ULL;
                if (auto a = v.array()) {
                    for (const auto &e : *a) {
                        if (!e) continue;
                        size_t eh = operator()(*e);
                        h ^= eh + 0x9e3779b97f4a7c15ULL + (h<<6) + (h>>2);
                    }
                }
                return h;
            }
            case VariantType::Object: {
                size_t h = 0x9e3779b9;
                if (auto o = v.object()) {
                    for (const auto &kv : *o) {
                        if (!kv.second) continue;
                        size_t hk = hash<std::string>()(kv.first);
                        size_t hv = operator()(*kv.second);
                        // order-independent combine
                        h ^= hk + 0x9e3779b97f4a7c15ULL + (hv<<6) + (hv>>2);
                        h ^= hv + 0x9e3779b97f4a7c15ULL + (hk<<6) + (hk>>2);
                    }
                    h ^= o->size() + 0x9e3779b97f4a7c15ULL;
                }
                return h;
            }
            case VariantType::Dynamic: {
                if (auto dv = std::get_if<VariantStorage::DynamicValue>(&v.storage_->data)) {
                    size_t p = std::hash<const void*>()(dv->ptr.get());
                    size_t t = dv->type.hash_code();
                    return p ^ (t + 0x9e3779b97f4a7c15ULL + (p<<6) + (p>>2));
                }
                return 0;
            }
            case VariantType::LuaRef: {
                if (auto lr = std::get_if<VariantStorage::LuaRefValue>(&v.storage_->data)) {
                    size_t Lp = std::hash<const void*>()(lr->L);
                    size_t tp = std::hash<int>()(static_cast<int>(lr->obj.get_type()));
                    return Lp ^ (tp + 0x9e3779b97f4a7c15ULL + (Lp<<6) + (Lp>>2));
                }
                return 0;
            }
            default: return 0;
        }
    }
};

} // namespace SDOM
#endif // SDOM_VARIANT_HPP
