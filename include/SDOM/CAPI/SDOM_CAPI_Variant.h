// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_Variant.h
//  Module: Variant
//
//  Brief:
//    Variant value marshaling utilities for the SDOM C API.
// =============================================================================
//
//  Authors:
//    Jay Faries (warte67) - Primary architect of SDOM
//
//  File Type: Header
//  SDOM Version: 0.5.261 (early pre-alpha)
//  Build Identifier: 2025-12-04_18:57:31_9b0abf93
//  Commit: 9b0abf93 on branch master
//  Compiler: g++ (GCC) 15.2.1 20251112
//  Platform: Linux-x86_64
//  Generated: 2025-12-04_18:57:31
//  Generator: sdom_generate_bindings
//
//  License Notice:
//  *   This software is provided 'as-is', without any express or implied
//  *   warranty.  In no event will the authors be held liable for any damages
//  *   arising from the use of this software.
//  *
//  *   Permission is granted to anyone to use this software for any purpose,
//  *   including commercial applications, and to alter it and redistribute it
//  *   freely, subject to the following restrictions:
//  *
//  *   1. The origin of this software must not be misrepresented; you must not
//  *       claim that you wrote the original software. If you use this software
//  *       in a product, an acknowledgment in the product documentation would be
//  *       appreciated but is not required.
//  *   2. Altered source versions must be plainly marked as such, and must not be
//  *       misrepresented as being the original software.
//  *   3. This notice may not be removed or altered from any source distribution.
// =============================================================================

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum SDOM_VariantType {
    /* Variant events */
    SDOM_VariantType_Null = 0x0000,                 ///< Represents an explicit null.
    SDOM_VariantType_Bool = 0x0001,                 ///< Boolean value.
    SDOM_VariantType_Int = 0x0002,                  ///< Signed 64-bit integer.
    SDOM_VariantType_Real = 0x0003,                 ///< Double-precision floating point.
    SDOM_VariantType_String = 0x0004,               ///< UTF-8 encoded string.
    SDOM_VariantType_Array = 0x0005,                ///< Ordered array of Variant values.
    SDOM_VariantType_Object = 0x0006,               ///< Key/value object of string keys to Variant values.
    SDOM_VariantType_Dynamic = 0x0007,              ///< Shared pointer to an arbitrary registered type.
    SDOM_VariantType_LuaRef = 0x0008,               ///< Opaque reference to a Lua object.
    SDOM_VariantType_Error = 0x0009                 ///< Represents a variant that recorded an error state.
} SDOM_VariantType;

typedef struct SDOM_Variant {
    void* impl;                                     ///< Opaque pointer to internal Variant storage.
} SDOM_Variant;

#ifdef __cplusplus
extern "C" {
#endif

// Callable bindings

/**
 * @brief Initializes a variant to the null state.
 *
 * C++:   bool VariantAPI::initNull(SDOM_Variant*)
 * C API: bool SDOM_Variant_InitNull(SDOM_Variant* variant)
 *
 * @param variant Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_InitNull(SDOM_Variant* variant);

/**
 * @brief Initializes a variant with the specified boolean value.
 *
 * C++:   bool VariantAPI::initBool(SDOM_Variant*, bool)
 * C API: bool SDOM_Variant_InitBool(SDOM_Variant* variant, bool value)
 *
 * @param variant Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_InitBool(SDOM_Variant* variant, bool value);

/**
 * @brief Initializes a variant with the provided 64-bit integer.
 *
 * C++:   bool VariantAPI::initInt(SDOM_Variant*, std::int64_t)
 * C API: bool SDOM_Variant_InitInt(SDOM_Variant* variant, int64_t value)
 *
 * @param variant Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_InitInt(SDOM_Variant* variant, int64_t value);

/**
 * @brief Initializes a variant with the provided double value.
 *
 * C++:   bool VariantAPI::initDouble(SDOM_Variant*, double)
 * C API: bool SDOM_Variant_InitDouble(SDOM_Variant* variant, double value)
 *
 * @param variant Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_InitDouble(SDOM_Variant* variant, double value);

/**
 * @brief Initializes a variant with the provided UTF-8 string.
 *
 * C++:   bool VariantAPI::initString(SDOM_Variant*, const char*)
 * C API: bool SDOM_Variant_InitString(SDOM_Variant* variant, const char* utf8)
 *
 * @param variant Pointer parameter.
 * @param utf8 Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_InitString(SDOM_Variant* variant, const char* utf8);

/**
 * @brief Copies the contents of one variant handle into another.
 *
 * C++:   bool VariantAPI::copy(const SDOM_Variant*, SDOM_Variant*)
 * C API: bool SDOM_Variant_Copy(const SDOM_Variant* source, SDOM_Variant* destination)
 *
 * @param source Pointer parameter.
 * @param destination Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_Copy(const SDOM_Variant* source, SDOM_Variant* destination);

/**
 * @brief Releases any resources held by the variant handle.
 *
 * C++:   bool VariantAPI::destroy(SDOM_Variant*)
 * C API: bool SDOM_Variant_Destroy(SDOM_Variant* variant)
 *
 * @param variant Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_Destroy(SDOM_Variant* variant);

/**
 * @brief Returns the enumerated type tag for the variant.
 *
 * C++:   VariantType VariantAPI::getType(const SDOM_Variant*)
 * C API: SDOM_VariantType SDOM_Variant_GetType(const SDOM_Variant* variant)
 *
 * @param variant Pointer parameter.
 * @return SDOM_VariantType; check SDOM_GetError() for details on failure.
 */
SDOM_VariantType SDOM_Variant_GetType(const SDOM_Variant* variant);

/**
 * @brief Converts the variant to a boolean using SDOM conversion semantics.
 *
 * C++:   bool VariantAPI::toBool(const SDOM_Variant*, bool*)
 * C API: bool SDOM_Variant_ToBool(const SDOM_Variant* variant, bool* out_value)
 *
 * @param variant Pointer parameter.
 * @param out_value Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_ToBool(const SDOM_Variant* variant, bool* out_value);

/**
 * @brief Converts the variant to a 64-bit integer using SDOM conversion semantics.
 *
 * C++:   bool VariantAPI::toInt64(const SDOM_Variant*, std::int64_t*)
 * C API: bool SDOM_Variant_ToInt64(const SDOM_Variant* variant, int64_t* out_value)
 *
 * @param variant Pointer parameter.
 * @param out_value Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_ToInt64(const SDOM_Variant* variant, int64_t* out_value);

/**
 * @brief Converts the variant to a double using SDOM conversion semantics.
 *
 * C++:   bool VariantAPI::toDouble(const SDOM_Variant*, double*)
 * C API: bool SDOM_Variant_ToDouble(const SDOM_Variant* variant, double* out_value)
 *
 * @param variant Pointer parameter.
 * @param out_value Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Variant_ToDouble(const SDOM_Variant* variant, double* out_value);

/**
 * @brief Converts the variant to its string representation.
 *
 * C++:   std::string VariantAPI::toString(const SDOM_Variant*)
 * C API: const char* SDOM_Variant_ToString(const SDOM_Variant* variant)
 *
 * @param variant Pointer parameter.
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_Variant_ToString(const SDOM_Variant* variant);

#ifdef __cplusplus
} // extern "C"
#endif
