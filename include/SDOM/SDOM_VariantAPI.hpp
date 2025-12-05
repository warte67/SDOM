#pragma once

#include <cstdint>
#include <string>

#include <SDOM/CAPI/SDOM_CAPI_Variant.h>

namespace SDOM {

class DataRegistry;
class Variant;

enum class VariantType : std::uint8_t;

namespace VariantAPI {

// Creation helpers -----------------------------------------------------------
bool initNull(SDOM_Variant* variant);
bool initBool(SDOM_Variant* variant, bool value);
bool initInt(SDOM_Variant* variant, std::int64_t value);
bool initDouble(SDOM_Variant* variant, double value);
bool initString(SDOM_Variant* variant, const char* value);
bool copy(const SDOM_Variant* source, SDOM_Variant* destination);
bool destroy(SDOM_Variant* variant);

// Introspection --------------------------------------------------------------
VariantType getType(const SDOM_Variant* variant);
bool toBool(const SDOM_Variant* variant, bool* outValue);
bool toInt64(const SDOM_Variant* variant, std::int64_t* outValue);
bool toDouble(const SDOM_Variant* variant, double* outValue);
std::string toString(const SDOM_Variant* variant);

// Registry wiring ------------------------------------------------------------
void registerTypes(DataRegistry& registry);

} // namespace VariantAPI

} // namespace SDOM
