// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_Handles.h
//  Module: Handles
//
//  Brief:
//    Opaque ABI struct describing an SDOM asset handle.
// =============================================================================
//
//  Authors:
//    Jay Faries (warte67) - Primary architect of SDOM
//
//  File Type: Header
//  SDOM Version: 0.5.266 (early pre-alpha)
//  Build Identifier: 2025-12-05_00:31:56_07198d15
//  Commit: 07198d15 on branch master
//  Compiler: g++ (GCC) 15.2.1 20251112
//  Platform: Linux-x86_64
//  Generated: 2025-12-05_00:31:56
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
#include <SDOM/CAPI/SDOM_CAPI_Handles.h>

typedef struct SDOM_AssetHandle {
    uint64_t object_id;                             ///< Factory-assigned identifier (0 == invalid). 
    const char* name;                               ///< Stable asset name token.
    const char* type;                               ///< Asset type string (Texture, Font, …). 
} SDOM_AssetHandle;

typedef struct SDOM_DisplayHandle {
    uint64_t object_id;                             ///< Factory-assigned identifier (0 == invalid). 
    const char* name;                               ///< Stable display object name token.
    const char* type;                               ///< Display object type string (Button, Label, …). 
} SDOM_DisplayHandle;

#ifdef __cplusplus
extern "C" {
#endif

// Callable bindings

/**
 * @brief Returns true if the referenced asset is still registered and resolves to a live instance.
 *
 * C++:   bool AssetHandle::isValid() const
 * C API: bool SDOM_AssetHandle_IsValid(const SDOM_AssetHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_AssetHandle_IsValid(const SDOM_AssetHandle* handle);

/**
 * @brief Returns true if the referenced display object currently resolves to a live instance.
 *
 * C++:   bool DisplayHandle::isValid() const
 * C API: bool SDOM_DisplayHandle_IsValid(const SDOM_DisplayHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_DisplayHandle_IsValid(const SDOM_DisplayHandle* handle);

#ifdef __cplusplus
} // extern "C"
#endif
