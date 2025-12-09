#pragma once
// Auto-generated SDOM C API module: Handles

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDOM_DisplayHandle {
    uint64_t object_id;
    const char* name;
    const char* type;
} SDOM_DisplayHandle;

typedef struct SDOM_AssetHandle {
    uint64_t object_id;
    const char* name;
    const char* type;
} SDOM_AssetHandle;

typedef struct SDOM_Variant SDOM_Variant;
typedef SDOM_Variant SDOM_Handle_Variant;

SDOM_Variant SDOM_MakeDisplayHandle(const SDOM_DisplayHandle* handle);
SDOM_Variant SDOM_MakeAssetHandle(const SDOM_AssetHandle* handle);
bool SDOM_Handle_IsValid(const SDOM_Handle_Variant* handle);
bool SDOM_Handle_IsDisplay(const SDOM_Handle_Variant* handle);
bool SDOM_Handle_IsAsset(const SDOM_Handle_Variant* handle);

// Callable bindings

/**
 * @brief Returns true when the variant tags a display handle.
 *
 * C++:   
 * C API: bool SDOM_Handle_IsDisplay(const SDOM_Variant* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Handle_IsDisplay(const SDOM_Variant* handle);

/**
 * @brief Returns true when the variant tags an asset handle.
 *
 * C++:   
 * C API: bool SDOM_Handle_IsAsset(const SDOM_Variant* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Handle_IsAsset(const SDOM_Variant* handle);

/**
 * @brief Resolves the tagged handle and reports whether it maps to a live object.
 *
 * C++:   
 * C API: bool SDOM_Handle_IsValid(const SDOM_Variant* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Handle_IsValid(const SDOM_Variant* handle);

/**
 * @brief Returns the underlying object identifier for display or asset handles; returns 0 otherwise.
 *
 * C++:   
 * C API: uint64_t SDOM_Handle_ObjectId(const SDOM_Variant* handle)
 *
 * @param handle Pointer parameter.
 * @return uint64_t; check SDOM_GetError() for details on failure.
 */
uint64_t SDOM_Handle_ObjectId(const SDOM_Variant* handle);

#ifdef __cplusplus
} // extern "C"
#endif
