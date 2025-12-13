#include <SDOM/SDOM_VariantPayload.hpp>

#include <cmath>
#include <limits>

namespace SDOM {

namespace {

template <typename T, typename VariantData>
inline const T* get_if_ptr(const VariantData& data) {
    return std::get_if<T>(&data);
}

} // namespace

VariantPayload::VariantPayload() = default;

VariantPayload::VariantPayload(bool value) { storage_.data = value; }

VariantPayload::VariantPayload(int32_t value)
: VariantPayload(static_cast<int64_t>(value)) {}

VariantPayload::VariantPayload(int64_t value) { storage_.data = value; }

VariantPayload::VariantPayload(double value) { storage_.data = value; }

VariantPayload::VariantPayload(const char* value)
: VariantPayload(value ? std::string(value) : std::string()) {}

VariantPayload::VariantPayload(std::string value) { storage_.data = std::move(value); }

VariantPayload::VariantPayload(const DisplayHandle& handle) { storage_.data = handle; }

VariantPayload::VariantPayload(DisplayHandle&& handle) { storage_.data = std::move(handle); }

VariantPayload::VariantPayload(const AssetHandle& handle) { storage_.data = handle; }

VariantPayload::VariantPayload(AssetHandle&& handle) { storage_.data = std::move(handle); }

VariantPayload::VariantPayload(Event* evt) { storage_.data = evt; }

VariantPayload::VariantPayload(std::shared_ptr<void> dynamicPtr,
                               std::type_index type,
                               std::string typeName) {
    storage_.data = DynamicValue{std::move(dynamicPtr), type, std::move(typeName)};
}

VariantPayload::VariantPayload(Array arrayData) { storage_.data = std::move(arrayData); }

VariantPayload::VariantPayload(Object objectData) { storage_.data = std::move(objectData); }

VariantPayload VariantPayload::makeArray() {
    VariantPayload payload;
    payload.storage_.data = Array{};
    return payload;
}

VariantPayload VariantPayload::makeObject() {
    VariantPayload payload;
    payload.storage_.data = Object{};
    return payload;
}

VariantPayload VariantPayload::makeError(std::string message) {
    VariantPayload payload;
    payload.errorFlag_ = true;
    payload.errorMsg_ = std::move(message);
    payload.storage_.data = std::monostate{};
    return payload;
}

VariantType VariantPayload::type() const noexcept {
    return deduceType();
}

bool VariantPayload::isNull() const noexcept { return type() == VariantType::Null; }
bool VariantPayload::isBool() const noexcept { return type() == VariantType::Bool; }
bool VariantPayload::isInt() const noexcept { return type() == VariantType::Int; }
bool VariantPayload::isReal() const noexcept { return type() == VariantType::Real; }
bool VariantPayload::isString() const noexcept { return type() == VariantType::String; }
bool VariantPayload::isArray() const noexcept { return type() == VariantType::Array; }
bool VariantPayload::isObject() const noexcept { return type() == VariantType::Object; }
bool VariantPayload::isDynamic() const noexcept { return type() == VariantType::Dynamic; }
bool VariantPayload::isDisplayHandle() const noexcept { return type() == VariantType::DisplayHandle; }
bool VariantPayload::isAssetHandle() const noexcept { return type() == VariantType::AssetHandle; }
bool VariantPayload::isEvent() const noexcept { return type() == VariantType::Event; }

bool VariantPayload::toBool(bool def) const noexcept {
    const auto& data = storage_.data;
    if (auto ptr = get_if_ptr<bool>(data)) return *ptr;
    if (auto ptr = get_if_ptr<int64_t>(data)) return *ptr != 0;
    if (auto ptr = get_if_ptr<double>(data)) return *ptr != 0.0;
    if (auto ptr = get_if_ptr<std::string>(data)) return (*ptr == "true") || (*ptr == "1");
    if (auto ptr = get_if_ptr<DisplayHandle>(data)) return ptr->isValid();
    if (auto ptr = get_if_ptr<AssetHandle>(data)) return ptr->isValid();
    return def;
}

int64_t VariantPayload::toInt64(int64_t def) const noexcept {
    const auto& data = storage_.data;
    if (auto ptr = get_if_ptr<int64_t>(data)) return *ptr;
    if (auto ptr = get_if_ptr<double>(data)) return static_cast<int64_t>(*ptr);
    if (auto ptr = get_if_ptr<bool>(data)) return *ptr ? 1 : 0;
    if (auto ptr = get_if_ptr<std::string>(data)) {
        try { return static_cast<int64_t>(std::stoll(*ptr)); } catch (...) { return def; }
    }
    return def;
}

double VariantPayload::toDouble(double def) const noexcept {
    const auto& data = storage_.data;
    if (auto ptr = get_if_ptr<double>(data)) return *ptr;
    if (auto ptr = get_if_ptr<int64_t>(data)) return static_cast<double>(*ptr);
    if (auto ptr = get_if_ptr<bool>(data)) return *ptr ? 1.0 : 0.0;
    if (auto ptr = get_if_ptr<std::string>(data)) {
        try { return std::stod(*ptr); } catch (...) { return def; }
    }
    return def;
}

std::string VariantPayload::toString(std::string def) const noexcept {
    const auto& data = storage_.data;
    if (auto ptr = get_if_ptr<std::string>(data)) return *ptr;
    if (auto ptr = get_if_ptr<int64_t>(data)) return std::to_string(*ptr);
    if (auto ptr = get_if_ptr<double>(data)) return std::to_string(*ptr);
    if (auto ptr = get_if_ptr<bool>(data)) return *ptr ? "true" : "false";
    if (auto ptr = get_if_ptr<DisplayHandle>(data)) return ptr->str();
    if (auto ptr = get_if_ptr<AssetHandle>(data)) {
        std::string s = ptr->getName();
        if (!ptr->getType().empty()) s += " (" + ptr->getType() + ")";
        if (!ptr->getFilename().empty()) s += " @" + ptr->getFilename();
        return s;
    }
    if (std::holds_alternative<std::monostate>(data)) return "null";
    if (std::holds_alternative<Event*>(data)) return "<event>";
    return def;
}

std::optional<bool> VariantPayload::tryBool() const noexcept {
    if (auto ptr = get_if_ptr<bool>(storage_.data)) return *ptr;
    return std::nullopt;
}

std::optional<int64_t> VariantPayload::tryInt64() const noexcept {
    if (auto ptr = get_if_ptr<int64_t>(storage_.data)) return *ptr;
    return std::nullopt;
}

std::optional<double> VariantPayload::tryDouble() const noexcept {
    if (auto ptr = get_if_ptr<double>(storage_.data)) return *ptr;
    return std::nullopt;
}

std::optional<std::string> VariantPayload::tryString() const noexcept {
    if (auto ptr = get_if_ptr<std::string>(storage_.data)) return *ptr;
    return std::nullopt;
}

const VariantPayload::Array* VariantPayload::array() const noexcept {
    return std::get_if<Array>(&storage_.data);
}

const VariantPayload::Object* VariantPayload::object() const noexcept {
    return std::get_if<Object>(&storage_.data);
}

const DisplayHandle* VariantPayload::displayHandle() const noexcept {
    return get_if_ptr<DisplayHandle>(storage_.data);
}

const AssetHandle* VariantPayload::assetHandle() const noexcept {
    return get_if_ptr<AssetHandle>(storage_.data);
}

Event* VariantPayload::event() const noexcept {
    if (auto ptr = get_if_ptr<Event*>(storage_.data)) return *ptr;
    return nullptr;
}

const VariantPayload::DynamicValue* VariantPayload::dynamicValue() const noexcept {
    return get_if_ptr<DynamicValue>(storage_.data);
}

nlohmann::json VariantPayload::toJson(const JsonConversionOptions& opts) const {
    std::function<nlohmann::json(const VariantPayload&, std::size_t)> walk =
        [&](const VariantPayload& node, std::size_t depth) -> nlohmann::json {
            if (depth >= opts.max_depth) {
                return nlohmann::json(nullptr);
            }

            const auto& data = node.storage_.data;
            if (std::holds_alternative<std::monostate>(data)) return nlohmann::json(nullptr);
            if (auto ptr = get_if_ptr<bool>(data)) return *ptr;
            if (auto ptr = get_if_ptr<int64_t>(data)) return *ptr;
            if (auto ptr = get_if_ptr<double>(data)) {
                if (!std::isfinite(*ptr)) {
                    if (opts.stringify_non_finite) {
                        if (std::isnan(*ptr)) return nlohmann::json("nan");
                        if (std::isinf(*ptr)) return nlohmann::json(*ptr > 0 ? "inf" : "-inf");
                    }
                    return nlohmann::json(nullptr);
                }
                return *ptr;
            }
            if (auto ptr = get_if_ptr<std::string>(data)) return *ptr;
            if (auto ptr = std::get_if<Array>(&data)) {
                nlohmann::json arr = nlohmann::json::array();
                for (const auto& child : *ptr) {
                    arr.push_back(child ? walk(*child, depth + 1) : nlohmann::json(nullptr));
                }
                return arr;
            }
            if (auto ptr = std::get_if<Object>(&data)) {
                nlohmann::json obj = nlohmann::json::object();
                for (const auto& [key, child] : *ptr) {
                    obj[key] = child ? walk(*child, depth + 1) : nlohmann::json(nullptr);
                }
                return obj;
            }
            if (auto ptr = get_if_ptr<DisplayHandle>(data)) {
                nlohmann::json obj = nlohmann::json::object();
                obj["name"] = ptr->getName();
                obj["type"] = ptr->getType();
                return obj;
            }
            if (auto ptr = get_if_ptr<AssetHandle>(data)) {
                nlohmann::json obj = nlohmann::json::object();
                obj["name"] = ptr->getName();
                obj["type"] = ptr->getType();
                obj["filename"] = ptr->getFilename();
                return obj;
            }
            return nlohmann::json(nullptr);
        };

    return walk(*this, 0);
}

VariantPayload VariantPayload::fromJson(const nlohmann::json& json,
                                        const JsonConversionOptions& opts,
                                        std::string* err) {
    std::function<VariantPayload(const nlohmann::json&, std::size_t)> walk =
        [&](const nlohmann::json& node, std::size_t depth) -> VariantPayload {
            if (depth >= opts.max_depth) {
                if (err) *err = "JSON max depth exceeded";
                return VariantPayload::makeError("JSON max depth exceeded");
            }

            if (node.is_null()) return VariantPayload();
            if (node.is_boolean()) return VariantPayload(node.get<bool>());
            if (node.is_number_unsigned()) {
                auto u = node.get<uint64_t>();
                if (u <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
                    return VariantPayload(static_cast<int64_t>(u));
                }
                if (opts.preserve_large_unsigned) return VariantPayload(std::to_string(u));
                return VariantPayload(static_cast<double>(u));
            }
            if (node.is_number_integer()) return VariantPayload(node.get<int64_t>());
            if (node.is_number_float()) return VariantPayload(node.get<double>());
            if (node.is_string()) return VariantPayload(node.get<std::string>());
            if (node.is_array()) {
                VariantPayload payload = VariantPayload::makeArray();
                auto* arr = std::get_if<Array>(&payload.storage_.data);
                if (arr) {
                    arr->reserve(node.size());
                    for (const auto& child : node) {
                        VariantPayload entry = walk(child, depth + 1);
                        if (entry.hasError()) return entry;
                        arr->push_back(std::make_shared<VariantPayload>(std::move(entry)));
                    }
                }
                return payload;
            }
            if (node.is_object()) {
                VariantPayload payload = VariantPayload::makeObject();
                auto* obj = std::get_if<Object>(&payload.storage_.data);
                if (obj) {
                    for (auto it = node.begin(); it != node.end(); ++it) {
                        VariantPayload entry = walk(it.value(), depth + 1);
                        if (entry.hasError()) return entry;
                        (*obj)[it.key()] = std::make_shared<VariantPayload>(std::move(entry));
                    }
                }
                return payload;
            }

            return VariantPayload();
        };

    return walk(json, 0);
}

VariantType VariantPayload::deduceType() const noexcept {
    if (errorFlag_) return VariantType::Error;
    const auto& data = storage_.data;
    if (std::holds_alternative<std::monostate>(data)) return VariantType::Null;
    if (std::holds_alternative<bool>(data)) return VariantType::Bool;
    if (std::holds_alternative<int64_t>(data)) return VariantType::Int;
    if (std::holds_alternative<double>(data)) return VariantType::Real;
    if (std::holds_alternative<std::string>(data)) return VariantType::String;
    if (std::holds_alternative<Array>(data)) return VariantType::Array;
    if (std::holds_alternative<Object>(data)) return VariantType::Object;
    if (std::holds_alternative<DisplayHandle>(data)) return VariantType::DisplayHandle;
    if (std::holds_alternative<AssetHandle>(data)) return VariantType::AssetHandle;
    if (std::holds_alternative<Event*>(data)) return VariantType::Event;
    if (std::holds_alternative<DynamicValue>(data)) return VariantType::Dynamic;
    return VariantType::Error;
}

} // namespace SDOM
