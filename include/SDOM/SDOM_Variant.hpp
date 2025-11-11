
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
    using DynamicPtr= std::shared_ptr<void>;

    using Data = std::variant<
        std::nullptr_t,
        bool,
        int64_t,
        double,
        std::string,
        Array,
        Object,
        DynamicPtr,
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
    bool isDynamic()const noexcept { return std::holds_alternative<VariantStorage::DynamicPtr>(storage_->data); }

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
