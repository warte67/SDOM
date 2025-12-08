#pragma once
// Auto-generated SDOM C API module: Handles

#include <stdbool.h>
#include <stdint.h>
#include <SDOM/CAPI/SDOM_CAPI_Handles.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque ABI struct describing an SDOM asset handle.
typedef struct SDOM_AssetHandle {
    uint64_t object_id;                             ///< Factory-assigned identifier (0 == invalid). 
    const char* name;                               ///< Stable asset name token.
    const char* type;                               ///< Asset type string (Texture, Font, …). 
} SDOM_AssetHandle;

// Opaque ABI struct describing an SDOM display handle.
typedef struct SDOM_DisplayHandle {
    uint64_t object_id;                             ///< Factory-assigned identifier (0 == invalid). 
    const char* name;                               ///< Stable display object name token.
    const char* type;                               ///< Display object type string (Button, Label, …). 
} SDOM_DisplayHandle;

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
