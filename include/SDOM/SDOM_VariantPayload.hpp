#pragma once
#ifndef SDOM_VARIANTPAYLOAD_HPP
#define SDOM_VARIANTPAYLOAD_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <json.hpp>

#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>

namespace SDOM {

class Event;
class VariantPayload;

// ─────────────────────────────────────────────────────────────────────────────
// Variant type tag shared by dynamic Variants and static Invariants
// ─────────────────────────────────────────────────────────────────────────────
enum class VariantType : uint8_t {
    Null = 0,
    Bool,
    Int,
    Real,
    String,
    Array,
    Object,
    Dynamic,
    DisplayHandle,
    AssetHandle,
    Event,
    Error
};

struct JsonConversionOptions {
    std::size_t max_depth = 64;
    bool preserve_large_unsigned = true;
    bool stringify_non_finite = true;
};

// Generic storage template so both VariantPayload and future Variant types can
// reuse the same layout without depending on each other directly.
template <typename NodeT>
struct VariantNodeStorage {
    using Array = std::vector<std::shared_ptr<NodeT>>;
    using Object = std::unordered_map<std::string, std::shared_ptr<NodeT>>;

    struct DynamicValue {
        std::shared_ptr<void> ptr;
        std::type_index type{typeid(void)};
        std::string typeName;

        bool operator==(const DynamicValue& other) const noexcept {
            return ptr == other.ptr && type == other.type && typeName == other.typeName;
        }
    };

    using Data = std::variant<
        std::monostate,
        bool,
        int64_t,
        double,
        std::string,
        Array,
        Object,
        DisplayHandle,
        AssetHandle,
        Event*,
        DynamicValue>;

    Data data{};
};

using VariantPayloadStorage = VariantNodeStorage<VariantPayload>;

class VariantPayload {
public:
    friend class Variant;
    using Storage = VariantPayloadStorage;
    using Array = Storage::Array;
    using Object = Storage::Object;
    using DynamicValue = Storage::DynamicValue;

    VariantPayload();
    explicit VariantPayload(bool value);
    explicit VariantPayload(int32_t value);
    explicit VariantPayload(int64_t value);
    explicit VariantPayload(double value);
    explicit VariantPayload(const char* value);
    explicit VariantPayload(std::string value);
    explicit VariantPayload(const DisplayHandle& handle);
    explicit VariantPayload(DisplayHandle&& handle);
    explicit VariantPayload(const AssetHandle& handle);
    explicit VariantPayload(AssetHandle&& handle);
    explicit VariantPayload(Event* evt);
    explicit VariantPayload(std::shared_ptr<void> dynamicPtr,
                            std::type_index type,
                            std::string typeName);
    explicit VariantPayload(Array arrayData);
    explicit VariantPayload(Object objectData);

    VariantPayload(const VariantPayload&) = default;
    VariantPayload& operator=(const VariantPayload&) = default;
    VariantPayload(VariantPayload&&) noexcept = default;
    VariantPayload& operator=(VariantPayload&&) noexcept = default;

    static VariantPayload makeArray();
    static VariantPayload makeObject();
    static VariantPayload makeError(std::string message);

    VariantType type() const noexcept;

    bool isNull() const noexcept;
    bool isBool() const noexcept;
    bool isInt() const noexcept;
    bool isReal() const noexcept;
    bool isString() const noexcept;
    bool isArray() const noexcept;
    bool isObject() const noexcept;
    bool isDynamic() const noexcept;
    bool isDisplayHandle() const noexcept;
    bool isAssetHandle() const noexcept;
    bool isEvent() const noexcept;
    bool isError() const noexcept { return errorFlag_; }

    bool toBool(bool def = false) const noexcept;
    int64_t toInt64(int64_t def = 0) const noexcept;
    double toDouble(double def = 0.0) const noexcept;
    std::string toString(std::string def = {}) const noexcept;

    std::optional<bool> tryBool() const noexcept;
    std::optional<int64_t> tryInt64() const noexcept;
    std::optional<double> tryDouble() const noexcept;
    std::optional<std::string> tryString() const noexcept;

    const Array* array() const noexcept;
    const Object* object() const noexcept;

    const DisplayHandle* displayHandle() const noexcept;
    const AssetHandle* assetHandle() const noexcept;
    Event* event() const noexcept;
    const DynamicValue* dynamicValue() const noexcept;

    bool hasError() const noexcept { return errorFlag_; }
    const std::string& errorMessage() const noexcept { return errorMsg_; }

    nlohmann::json toJson(const JsonConversionOptions& opts = {}) const;
    static VariantPayload fromJson(const nlohmann::json& json,
                                   const JsonConversionOptions& opts = {},
                                   std::string* err = nullptr);

private:
    Storage storage_;
    bool errorFlag_{false};
    std::string errorMsg_;

    Storage& mutableStorage() noexcept { return storage_; }
    const Storage& storage() const noexcept { return storage_; }
    Storage::Data& data() noexcept { return storage_.data; }
    const Storage::Data& data() const noexcept { return storage_.data; }
    Array* mutableArray() noexcept { return std::get_if<Array>(&storage_.data); }
    Object* mutableObject() noexcept { return std::get_if<Object>(&storage_.data); }
    VariantType deduceType() const noexcept;
};

} // namespace SDOM

#endif // SDOM_VARIANTPAYLOAD_HPP
