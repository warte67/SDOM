#pragma once
// Auto-generated SDOM C API module: Variant

#include <stdbool.h>
#include <stdint.h>
#include <SDOM/CAPI/SDOM_CAPI_Variant.h>

typedef enum SDOM_VariantType {
    SDOM_VARIANT_TYPE_NULL   = 0,
    SDOM_VARIANT_TYPE_BOOL   = 1,
    SDOM_VARIANT_TYPE_INT    = 2,
    SDOM_VARIANT_TYPE_FLOAT  = 3,
    SDOM_VARIANT_TYPE_STRING = 4,
    SDOM_VARIANT_TYPE_ARRAY  = 5,
    SDOM_VARIANT_TYPE_OBJECT = 6,
    SDOM_VARIANT_TYPE_DYNAMIC= 7
} SDOM_VariantType;

#ifdef __cplusplus
extern "C" {
#endif

// Provisional ABI view of SDOM::Variant; layout may change.
typedef struct SDOM_Variant {
    uint8_t type;           ///< Variant kind tag; mirrors SDOM_VariantType.
    uint8_t reserved[3];    ///< Reserved bytes; must be zero when passed across the ABI.
    uint32_t flags;         ///< Future use bitmask; currently must be zero.
    uint64_t data;          ///< Inline payload or handle depending on type/flags; provisional semantics.
} SDOM_Variant;

// Callable bindings

/**
 * @brief 
 *
 * C++:   
 * C API: uint8_t SDOM_Type(const SDOM_Variant* v)
 *
 * @param v Pointer parameter.
 * @return uint8_t; check SDOM_GetError() for details on failure.
 */
uint8_t SDOM_Type(const SDOM_Variant* v);

/**
 * @brief 
 *
 * C++:   
 * C API: bool SDOM_IsNull(const SDOM_Variant* v)
 *
 * @param v Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_IsNull(const SDOM_Variant* v);

/**
 * @brief 
 *
 * C++:   
 * C API: bool SDOM_IsBool(const SDOM_Variant* v)
 *
 * @param v Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_IsBool(const SDOM_Variant* v);

/**
 * @brief 
 *
 * C++:   
 * C API: bool SDOM_IsInt(const SDOM_Variant* v)
 *
 * @param v Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_IsInt(const SDOM_Variant* v);

/**
 * @brief 
 *
 * C++:   
 * C API: bool SDOM_IsFloat(const SDOM_Variant* v)
 *
 * @param v Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_IsFloat(const SDOM_Variant* v);

/**
 * @brief 
 *
 * C++:   
 * C API: bool SDOM_IsString(const SDOM_Variant* v)
 *
 * @param v Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_IsString(const SDOM_Variant* v);

/**
 * @brief 
 *
 * C++:   
 * C API: SDOM_Variant SDOM_MakeNull(void)
 *
 * @return SDOM_Variant; check SDOM_GetError() for details on failure.
 */
SDOM_Variant SDOM_MakeNull(void);

/**
 * @brief 
 *
 * C++:   
 * C API: SDOM_Variant SDOM_MakeBool(bool b)
 *
 * @return SDOM_Variant; check SDOM_GetError() for details on failure.
 */
SDOM_Variant SDOM_MakeBool(bool b);

/**
 * @brief 
 *
 * C++:   
 * C API: SDOM_Variant SDOM_MakeInt(int64_t i)
 *
 * @return SDOM_Variant; check SDOM_GetError() for details on failure.
 */
SDOM_Variant SDOM_MakeInt(int64_t i);

/**
 * @brief 
 *
 * C++:   
 * C API: SDOM_Variant SDOM_MakeFloat(double f)
 *
 * @return SDOM_Variant; check SDOM_GetError() for details on failure.
 */
SDOM_Variant SDOM_MakeFloat(double f);

/**
 * @brief 
 *
 * C++:   
 * C API: SDOM_Variant SDOM_MakeCString(const char* utf8)
 *
 * @param utf8 Pointer parameter.
 * @return SDOM_Variant; check SDOM_GetError() for details on failure.
 */
SDOM_Variant SDOM_MakeCString(const char* utf8);

/**
 * @brief 
 *
 * C++:   
 * C API: bool SDOM_AsBool(const SDOM_Variant* v)
 *
 * @param v Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_AsBool(const SDOM_Variant* v);

/**
 * @brief 
 *
 * C++:   
 * C API: int64_t SDOM_AsInt(const SDOM_Variant* v)
 *
 * @param v Pointer parameter.
 * @return int64_t; check SDOM_GetError() for details on failure.
 */
int64_t SDOM_AsInt(const SDOM_Variant* v);

/**
 * @brief 
 *
 * C++:   
 * C API: double SDOM_AsFloat(const SDOM_Variant* v)
 *
 * @param v Pointer parameter.
 * @return double; check SDOM_GetError() for details on failure.
 */
double SDOM_AsFloat(const SDOM_Variant* v);

/**
 * @brief 
 *
 * C++:   
 * C API: const char* SDOM_AsString(const SDOM_Variant* v)
 *
 * @param v Pointer parameter.
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_AsString(const SDOM_Variant* v);

/**
 * @brief 
 *
 * C++:   
 * C API: bool SDOM_Variant_GetPath(const SDOM_Variant* root, const char* path, SDOM_Variant* out_value)
 *
 * @param root Pointer parameter.
 * @param path Pointer parameter.
 * @param out_value Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_GetPath(const SDOM_Variant* root, const char* path, SDOM_Variant* out_value);

/**
 * @brief 
 *
 * C++:   
 * C API: bool SDOM_Variant_SetPath(SDOM_Variant* root, const char* path, const SDOM_Variant* value)
 *
 * @param root Pointer parameter.
 * @param path Pointer parameter.
 * @param value Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_SetPath(SDOM_Variant* root, const char* path, const SDOM_Variant* value);

/**
 * @brief 
 *
 * C++:   
 * C API: bool SDOM_Variant_PathExists(const SDOM_Variant* root, const char* path)
 *
 * @param root Pointer parameter.
 * @param path Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_PathExists(const SDOM_Variant* root, const char* path);

/**
 * @brief 
 *
 * C++:   
 * C API: bool SDOM_Variant_ErasePath(SDOM_Variant* root, const char* path)
 *
 * @param root Pointer parameter.
 * @param path Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_ErasePath(SDOM_Variant* root, const char* path);

#ifdef __cplusplus
} // extern "C"
#endif
