#pragma once
// Auto-generated SDOM C API module: Handles

#include <cstdint> // For uint64_t
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
bool SDOM_AssetHandle_IsValid(const SDOM_AssetHandle* handle);
bool SDOM_DisplayHandle_IsValid(const SDOM_DisplayHandle* handle);

#ifdef __cplusplus
} // extern "C"
#endif
