#include <cstring>
#include <SDOM/CAPI/SDOM_CAPI_Variant.h>

extern "C" {

// --- Type access -----------------------------------------------------------
uint8_t SDOM_Type(const SDOM_Variant* v) {
    if (!v) return 0;
    return v->type;
}

bool SDOM_IsNull(const SDOM_Variant* v)   { return SDOM_Type(v) == SDOM_VARIANT_TYPE_NULL; }
bool SDOM_IsBool(const SDOM_Variant* v)   { return SDOM_Type(v) == SDOM_VARIANT_TYPE_BOOL; }
bool SDOM_IsInt(const SDOM_Variant* v)    { return SDOM_Type(v) == SDOM_VARIANT_TYPE_INT; }
bool SDOM_IsFloat(const SDOM_Variant* v)  { return SDOM_Type(v) == SDOM_VARIANT_TYPE_FLOAT; }
bool SDOM_IsString(const SDOM_Variant* v) { return SDOM_Type(v) == SDOM_VARIANT_TYPE_STRING; }

// --- Constructors ----------------------------------------------------------
SDOM_Variant SDOM_MakeNull(void) {
    SDOM_Variant v { SDOM_VARIANT_TYPE_NULL, {0,0,0}, 0, 0 };
    return v;
}

SDOM_Variant SDOM_MakeBool(bool b) {
    SDOM_Variant v { SDOM_VARIANT_TYPE_BOOL, {0,0,0}, 0, static_cast<uint64_t>(b) };
    return v;
}

SDOM_Variant SDOM_MakeInt(int64_t i) {
    SDOM_Variant v { SDOM_VARIANT_TYPE_INT, {0,0,0}, 0, static_cast<uint64_t>(i) };
    return v;
}

SDOM_Variant SDOM_MakeFloat(double f) {
    uint64_t d = 0;
    std::memcpy(&d, &f, sizeof(double));
    SDOM_Variant v { SDOM_VARIANT_TYPE_FLOAT, {0,0,0}, 0, d };
    return v;
}

SDOM_Variant SDOM_MakeCString(const char* utf8) {
    SDOM_Variant v { SDOM_VARIANT_TYPE_STRING, {0,0,0}, 0, reinterpret_cast<uint64_t>(utf8) };
    return v;
}

// --- Accessors -------------------------------------------------------------
bool SDOM_AsBool(const SDOM_Variant* v) {
    return SDOM_IsBool(v) ? static_cast<bool>(v->data) : false;
}

int64_t SDOM_AsInt(const SDOM_Variant* v) {
    return SDOM_IsInt(v) ? static_cast<int64_t>(v->data) : 0;
}

double SDOM_AsFloat(const SDOM_Variant* v) {
    if (!SDOM_IsFloat(v)) return 0.0;
    double f;
    std::memcpy(&f, &v->data, sizeof(double));
    return f;
}

const char* SDOM_AsString(const SDOM_Variant* v) {
    return SDOM_IsString(v) ? reinterpret_cast<const char*>(v->data) : "";
}

} // extern "C"
