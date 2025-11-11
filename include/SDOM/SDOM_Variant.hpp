
#pragma once
#ifndef SDOM_VARIANT_HPP
#define SDOM_VARIANT_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <optional>

// SDOM ships with Lua/sol; Variant integrates directly
#include <sol/sol.hpp>
#include <functional>

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
    using Array     = std::vector<Variant>;
    using Object    = std::unordered_map<std::string, Variant>;
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
    Variant(bool b);
    Variant(int32_t i);
    Variant(int64_t i);
    Variant(double d);
    Variant(const char* s);
    Variant(std::string s);
    Variant(const std::vector<Variant>& arr);
    Variant(std::vector<Variant>&& arr);
    Variant(const std::unordered_map<std::string, Variant>& obj);
    Variant(std::unordered_map<std::string, Variant>&& obj);
    Variant(const sol::object& o);

    // Factories for composite
    static Variant makeArray();
    static Variant makeObject();

    // Error helpers
    static Variant makeError(std::string msg);
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
    std::shared_ptr<T> asDynamic() const noexcept {
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

    // Accessors for dynamic metadata
    std::optional<std::string> dynamicTypeName() const noexcept {
        if (!isDynamic()) return std::nullopt;
        const auto& dv = std::get<VariantStorage::DynamicValue>(storage_->data);
        return dv.typeName.empty() ? std::nullopt : std::optional<std::string>(dv.typeName);
    }

    std::type_index dynamicTypeIndex() const noexcept {
        if (!isDynamic()) return typeid(void);
        const auto& dv = std::get<VariantStorage::DynamicValue>(storage_->data);
        return dv.type;
    }

    // Converter API (minimal): allow registering per-type converters for
    // converting DynamicValue -> Lua and Variant -> DynamicValue
    using DynamicToLuaFn = std::function<sol::object(const VariantStorage::DynamicValue&, sol::state_view)>;
    using VariantToDynamicFn = std::function<std::shared_ptr<void>(const Variant&)>;

    struct ConverterEntry {
        DynamicToLuaFn toLua;
        VariantToDynamicFn fromVariant;
    };

    // Register a converter for a C++ type T
    template<typename T>
    static void registerConverter(const std::string& typeName, ConverterEntry entry) {
        VariantRegistry::registerType<T>(typeName);
        std::lock_guard<std::mutex> lk(VariantRegistry::getMutex());
        // Populate both type_index->converter and name->converter maps. ConverterEntry
        // uses std::function which is copyable, so copy-insert into both maps.
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
        // Converter map accessors
        static std::unordered_map<std::type_index, ConverterEntry>& getConverterMap() {
            static std::unordered_map<std::type_index, ConverterEntry> conv;
            return conv;
        }
        // Name-based converter map (allows lookup by registered type name)
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
    void set(std::string key, Variant v);  // ensures Object
    const Variant* get(const std::string& key) const noexcept;
    Variant*       get(const std::string& key) noexcept;

    // Lua integration
    sol::object toLua(sol::state_view L) const;
    static Variant fromLuaObject(const sol::object& o);
    // If this Variant holds a LuaRef to a table, produce a deep-copied
    // Variant (Array/Object) snapshot of that table's contents. If this
    // Variant is not a LuaRef to a table, returns a copy of *this.
    Variant snapshot() const;

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
                if (i < a->size()) return &(*a)[i];
            }
            return nullptr;
        }

        const Variant* get(const std::string& key) const noexcept {
            if (!storage) return nullptr;
            if (auto o = std::get_if<VariantStorage::Object>(&storage->data)) {
                auto it = o->find(key);
                if (it != o->end()) return &it->second;
            }
            return nullptr;
        }

    private:
        const VariantStorage* storage = nullptr;
    };

    // Operators
    bool operator==(const Variant& rhs) const;
    bool operator!=(const Variant& rhs) const { return !(*this == rhs); }

private:
    std::shared_ptr<VariantStorage> storage_;
    bool        errorFlag_ = false;
    std::string errorMsg_;

    // helpers
    static Variant fromLuaTable_(const sol::table& t);
    static Variant fromLuaNumber_(double n);
    static bool    numericEqual_(const Variant& a, const Variant& b);
};

} // namespace SDOM

#endif // SDOM_VARIANT_HPP
