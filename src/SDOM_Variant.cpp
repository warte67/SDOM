
#include <SDOM/SDOM_Variant.hpp>

#include <cmath>
#include <limits>
#include <mutex>

// Nothing to implement out-of-line here; registry helpers are inline in header.

namespace SDOM {

// ─────────────────────────────────────────────────────────────────────────────
// Constructors
// ─────────────────────────────────────────────────────────────────────────────
Variant::Variant()
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = std::monostate{};
}

// Variant(nullptr) overload removed — default constructor represents null

Variant::Variant(bool b)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = b;
}

Variant::Variant(int32_t i)
: Variant(static_cast<int64_t>(i)) {}

Variant::Variant(int64_t i)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = i;
}

Variant::Variant(double d)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = d;
}

Variant::Variant(const char* s)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = std::string{s ? s : ""};
}

Variant::Variant(std::string s)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = std::move(s);
}

Variant::Variant(const std::vector<Variant>& arr)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = arr;
}

Variant::Variant(std::vector<Variant>&& arr)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = std::move(arr);
}

Variant::Variant(const std::unordered_map<std::string, Variant>& obj)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = obj;
}

Variant::Variant(std::unordered_map<std::string, Variant>&& obj)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = std::move(obj);
}

Variant::Variant(const sol::object& o)
: storage_(std::make_shared<VariantStorage>()) {
    storage_->data = VariantStorage::LuaRefValue{o};
}

// (templated dynamic constructor is defined in the header)

// Factories
Variant Variant::makeArray() {
    Variant v;
    v.storage_->data = VariantStorage::Array{};
    return v;
}

Variant Variant::makeObject() {
    Variant v;
    v.storage_->data = VariantStorage::Object{};
    return v;
}

// Error helpers
Variant Variant::makeError(std::string msg) {
    Variant v;
    v.storage_->data = std::monostate{};
    v.errorFlag_ = true;
    v.errorMsg_  = std::move(msg);
    return v;
}

// Type info
VariantType Variant::type() const noexcept {
    if (errorFlag_) return VariantType::Error;

    const auto& d = storage_->data;
    if (std::holds_alternative<std::monostate>(d))                 return VariantType::Null;
    if (std::holds_alternative<bool>(d))                           return VariantType::Bool;
    if (std::holds_alternative<int64_t>(d))                        return VariantType::Int;
    if (std::holds_alternative<double>(d))                         return VariantType::Real;
    if (std::holds_alternative<std::string>(d))                    return VariantType::String;
    if (std::holds_alternative<VariantStorage::Array>(d))          return VariantType::Array;
    if (std::holds_alternative<VariantStorage::Object>(d))         return VariantType::Object;
    if (std::holds_alternative<VariantStorage::DynamicValue>(d))     return VariantType::Dynamic;
    if (std::holds_alternative<VariantStorage::LuaRefValue>(d))    return VariantType::LuaRef;
    return VariantType::Error;
}

// Conversions
bool Variant::toBool(bool def) const noexcept {
    const auto& d = storage_->data;
    if (auto p = std::get_if<bool>(&d)) return *p;
    if (auto pi = std::get_if<int64_t>(&d)) return *pi != 0;
    if (auto pr = std::get_if<double>(&d))  return *pr != 0.0;
    if (auto ps = std::get_if<std::string>(&d)) return (*ps == "true") || (*ps == "1");
    return def;
}

int64_t Variant::toInt64(int64_t def) const noexcept {
    const auto& d = storage_->data;
    if (auto p = std::get_if<int64_t>(&d)) return *p;
    if (auto pr = std::get_if<double>(&d)) return static_cast<int64_t>(*pr);
    if (auto pb = std::get_if<bool>(&d))   return *pb ? 1 : 0;
    if (auto ps = std::get_if<std::string>(&d)) {
        try { return static_cast<int64_t>(std::stoll(*ps)); } catch(...) { return def; }
    }
    return def;
}

double Variant::toDouble(double def) const noexcept {
    const auto& d = storage_->data;
    if (auto p = std::get_if<double>(&d))  return *p;
    if (auto pi = std::get_if<int64_t>(&d))return static_cast<double>(*pi);
    if (auto pb = std::get_if<bool>(&d))   return *pb ? 1.0 : 0.0;
    if (auto ps = std::get_if<std::string>(&d)) {
        try { return std::stod(*ps); } catch(...) { return def; }
    }
    return def;
}

std::string Variant::toString(std::string def) const noexcept {
    const auto& d = storage_->data;
    if (auto p = std::get_if<std::string>(&d)) return *p;
    if (auto pi = std::get_if<int64_t>(&d))    return std::to_string(*pi);
    if (auto pr = std::get_if<double>(&d))     return std::to_string(*pr);
    if (auto pb = std::get_if<bool>(&d))       return *pb ? "true" : "false";
    if (std::holds_alternative<std::monostate>(d)) return "null";
    return def;
}

// Array/Object helpers
const VariantStorage::Array* Variant::array() const noexcept {
    return std::get_if<VariantStorage::Array>(&storage_->data);
}
VariantStorage::Array* Variant::array() noexcept {
    return std::get_if<VariantStorage::Array>(&storage_->data);
}
const VariantStorage::Object* Variant::object() const noexcept {
    return std::get_if<VariantStorage::Object>(&storage_->data);
}
VariantStorage::Object* Variant::object() noexcept {
    return std::get_if<VariantStorage::Object>(&storage_->data);
}

size_t Variant::size() const noexcept {
    if (auto a = array())  return a->size();
    if (auto o = object()) return o->size();
    return 0;
}

void Variant::push(Variant v) {
    if (!isArray()) storage_->data = VariantStorage::Array{};
    std::get<VariantStorage::Array>(storage_->data).push_back(std::move(v));
}

const Variant* Variant::at(size_t i) const noexcept {
    if (auto a = array()) {
        if (i < a->size()) return &(*a)[i];
    }
    return nullptr;
}
Variant* Variant::at(size_t i) noexcept {
    if (auto a = array()) {
        if (i < a->size()) return &(*a)[i];
    }
    return nullptr;
}

void Variant::set(std::string key, Variant v) {
    if (!isObject()) storage_->data = VariantStorage::Object{};
    auto& obj = std::get<VariantStorage::Object>(storage_->data);
    obj.insert_or_assign(std::move(key), std::move(v));
}

const Variant* Variant::get(const std::string& key) const noexcept {
    if (auto o = object()) {
        auto it = o->find(key);
        if (it != o->end()) return &it->second;
    }
    return nullptr;
}
Variant* Variant::get(const std::string& key) noexcept {
    if (auto o = object()) {
        auto it = o->find(key);
        if (it != o->end()) return &it->second;
    }
    return nullptr;
}

// Lua integration
Variant Variant::fromLuaNumber_(double n) {
    // Prefer int when representable exactly
    double intpart;
    if (std::modf(n, &intpart) == 0.0) {
        // fits into 64-bit?
        if (n >= static_cast<double>(std::numeric_limits<int64_t>::min()) &&
            n <= static_cast<double>(std::numeric_limits<int64_t>::max())) {
            return Variant(static_cast<int64_t>(n));
        }
    }
    return Variant(n);
}

Variant Variant::fromLuaTable_(const sol::table& t) {
    // Decide array vs object
    bool isArray = true;
    size_t maxIndex = 0;
    size_t count = 0;
    for (auto& kv : t) {
        count++;
        if (kv.first.get_type() == sol::type::number) {
            double idxd = kv.first.as<double>();
            double ip;
            if (std::modf(idxd, &ip) != 0.0 || idxd <= 0) { isArray = false; break; }
            size_t idx = static_cast<size_t>(idxd);
            if (idx > maxIndex) maxIndex = idx;
        } else {
            isArray = false;
            break;
        }
    }

    if (isArray && (maxIndex == count || count == 0)) {
        // packed array 1..n
        Variant v = Variant::makeArray();
        auto* arr = v.array();
        arr->reserve(maxIndex);
        for (size_t i = 1; i <= maxIndex; ++i) {
            sol::object elem = t[i];
            arr->push_back(Variant::fromLuaObject(elem));
        }
        return v;
    } else {
        Variant v = Variant::makeObject();
        auto* obj = v.object();
        for (auto& kv : t) {
            std::string key;
            if (kv.first.get_type() == sol::type::string) {
                key = std::string(kv.first.as<std::string_view>());
            } else {
                // stringify non-string keys
                sol::object k = kv.first;
                sol::state_view L = t.lua_state();
                sol::object s = sol::make_object(L, k);
                // fallback textualization
                switch (k.get_type()) {
                    case sol::type::number: key = std::to_string(k.as<double>()); break;
                    case sol::type::boolean: key = k.as<bool>() ? "true" : "false"; break;
                    default: key = "<key>"; break;
                }
            }
            (*obj)[key] = Variant::fromLuaObject(kv.second);
        }
        return v;
    }
}

Variant Variant::fromLuaObject(const sol::object& o) {
    if (!o.valid())            return Variant(); // Null
    switch (o.get_type()) {
        case sol::type::nil:       return Variant();
        case sol::type::boolean:   return Variant(o.as<bool>());
        case sol::type::number:    return fromLuaNumber_(o.as<double>());
        case sol::type::string:    return Variant(std::string(o.as<std::string_view>()));
        case sol::type::table:
            // Respect global table storage mode: copy (default) or keep as LuaRef
            if (Variant::getTableStorageMode() == Variant::TableStorageMode::KeepLuaRef) {
                return Variant(o); // store as LuaRefValue
            }
            return fromLuaTable_(o.as<sol::table>());
        case sol::type::userdata:
        case sol::type::lightuserdata:
        case sol::type::thread:
        case sol::type::function:
            // Hold opaque reference for now
            return Variant(o);
        default:
            // Opaque fallback
            return Variant(o);
    }
}

Variant Variant::snapshot() const {
    // Default snapshot: if we have a stored LuaRefValue with an associated
    // lua_State pointer, attempt to snapshot using that state. If no
    // LuaRefValue is present, return *this.
    const auto& d = storage_->data;
    if (auto p = std::get_if<VariantStorage::LuaRefValue>(&d)) {
        if (!p->obj.valid() || p->L == nullptr) return *this;
        try {
            sol::state_view sv(p->L);
            return snapshot(sv);
        } catch(...) {
            return *this;
        }
    }
    return *this;
}

Variant Variant::snapshot(sol::state_view L) const {
    const auto& d = storage_->data;
    if (auto p = std::get_if<VariantStorage::LuaRefValue>(&d)) {
        if (!p->obj.valid()) return *this;
        if (!p->validFor(L)) return *this;
        try {
            if (p->obj.get_type() == sol::type::table) {
                sol::table t = p->obj.as<sol::table>();
                return fromLuaTable_(t);
            }
        } catch(...) {
            return *this;
        }
    }
    return *this;
}

// toDebugString implementation — depth-limited pretty printer for debugging.
std::string Variant::toDebugString(int depth) const {
    if (depth < 0) depth = 0;
    if (depth == 0) return "...";
    switch (type()) {
        case VariantType::Null: return "null";
        case VariantType::Bool: return toBool() ? "true" : "false";
        case VariantType::Int: return std::to_string(toInt64());
        case VariantType::Real: return std::to_string(toDouble());
        case VariantType::String: return std::string("\"") + toString() + "\"";
        case VariantType::Array: {
            const auto* a = array();
            if (!a) return "[]";
            std::string s = "[";
            bool first = true;
            for (const auto &e : *a) {
                if (!first) s += ", ";
                s += e.toDebugString(depth - 1);
                first = false;
            }
            s += "]";
            return s;
        }
        case VariantType::Object: {
            const auto* o = object();
            if (!o) return "{}";
            std::string s = "{";
            bool first = true;
            for (const auto &kv : *o) {
                if (!first) s += ", ";
                s += kv.first + ": " + kv.second.toDebugString(depth - 1);
                first = false;
            }
            s += "}";
            return s;
        }
        case VariantType::Dynamic: return "<dynamic>";
        case VariantType::LuaRef: return "<luaref>";
        default: return "<error>";
    }
}

sol::object Variant::toLua(sol::state_view L) const {
    const auto& d = storage_->data;
    if (std::holds_alternative<std::monostate>(d)) return sol::make_object(L, sol::lua_nil);
    if (auto p = std::get_if<bool>(&d))            return sol::make_object(L, *p);
    if (auto p = std::get_if<int64_t>(&d))         return sol::make_object(L, *p);
    if (auto p = std::get_if<double>(&d))          return sol::make_object(L, *p);
    if (auto p = std::get_if<std::string>(&d))     return sol::make_object(L, *p);
    if (auto p = std::get_if<VariantStorage::Array>(&d)) {
        sol::table t = L.create_table(static_cast<int>(p->size()), 0);
        int i = 1;
        for (const auto& elem : *p) {
            t[i++] = elem.toLua(L);
        }
        return t;
    }
    if (auto p = std::get_if<VariantStorage::Object>(&d)) {
        sol::table t = L.create_table(0, static_cast<int>(p->size()));
        for (const auto& [k, v] : *p) {
            t[k] = v.toLua(L);
        }
        return t;
    }
    if (auto p = std::get_if<VariantStorage::LuaRefValue>(&d)) {
        if (p->validFor(L)) return p->obj;
        return sol::make_object(L, sol::lua_nil);
    }
    if (auto p = std::get_if<VariantStorage::DynamicValue>(&d)) {
        // If a converter exists for this dynamic type, call it
        auto conv = Variant::getConverter(p->type);
        if (conv && conv->toLua) {
            try {
                return conv->toLua(*p, L);
            } catch(...) { /* fallthrough to nil */ }
        }
    }
    // No conversion available: return nil
    return sol::make_object(L, sol::lua_nil);
}

// Operators
bool Variant::numericEqual_(const Variant& a, const Variant& b) {
    double ad, bd;
    ad = a.toDouble(std::numeric_limits<double>::quiet_NaN());
    bd = b.toDouble(std::numeric_limits<double>::quiet_NaN());
    if (std::isnan(ad) || std::isnan(bd)) return false;
    return ad == bd;
}

bool Variant::operator==(const Variant& rhs) const {
    // error state equality
    if (errorFlag_ != rhs.errorFlag_) return false;
    if (errorFlag_ && rhs.errorFlag_) return errorMsg_ == rhs.errorMsg_;

    const auto& a = storage_->data;
    const auto& b = rhs.storage_->data;

    // numeric cross-type equality
    const bool a_is_num = std::holds_alternative<int64_t>(a) || std::holds_alternative<double>(a);
    const bool b_is_num = std::holds_alternative<int64_t>(b) || std::holds_alternative<double>(b);
    if (a_is_num && b_is_num) {
        return numericEqual_(*this, rhs);
    }

    // same-alternative direct compare (all types used here have operator==)
    if (a.index() == b.index()) {
        return std::visit([](const auto& lhs, const auto& rhs)->bool {
            using L = std::decay_t<decltype(lhs)>;
            using R = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<L,R>) {
                return lhs == rhs;
            } else {
                return false;
            }
        }, a, b);
    }

    return false;
}

} // namespace SDOM
