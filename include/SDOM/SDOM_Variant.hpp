
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

        using Data = std::variant<
        std::nullptr_t,
        bool,
        int64_t,
        double,
        std::string,
        Array,
        Object,
        DynamicValue,
        sol::object,
        std::monostate // used for Error sentinel
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
    Variant(std::nullptr_t);
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
    bool isNull()   const noexcept { return std::holds_alternative<std::nullptr_t>(storage_->data); }
    bool isBool()   const noexcept { return std::holds_alternative<bool>(storage_->data); }
    bool isInt()    const noexcept { return std::holds_alternative<int64_t>(storage_->data); }
    bool isReal()   const noexcept { return std::holds_alternative<double>(storage_->data); }
    bool isString() const noexcept { return std::holds_alternative<std::string>(storage_->data); }
    bool isArray()  const noexcept { return std::holds_alternative<VariantStorage::Array>(storage_->data); }
    bool isObject() const noexcept { return std::holds_alternative<VariantStorage::Object>(storage_->data); }
    bool isLuaRef() const noexcept { return std::holds_alternative<sol::object>(storage_->data); }
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
        VariantRegistry::getConverterMap()[std::type_index(typeid(T))] = std::move(entry);
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
    private:
    };

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
