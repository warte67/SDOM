// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_IDisplayObject.h
//  Module: IDisplayObject
//
//  Brief:
//    Common runtime services for SDOM display objects, including dirty flag
//    control and state queries.
// =============================================================================
//
//  Authors:
//    Jay Faries (warte67) - Primary architect of SDOM
//
//  File Type: Header
//  SDOM Version: 0.5.264 (early pre-alpha)
//  Build Identifier: 2025-12-04_22:07:48_ddcdaf40
//  Commit: ddcdaf40 on branch master
//  Compiler: g++ (GCC) 15.2.1 20251112
//  Platform: Linux-x86_64
//  Generated: 2025-12-04_22:07:48
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
#include <SDOM/CAPI/SDOM_CAPI_Handles.h>

typedef enum SDOM_OrphanRetentionPolicy {
    /* DisplayObjects events */
    SDOM_OrphanRetentionPolicy_RetainUntilManual = 0x0000, ///< Never auto-destroy; requires explicit teardown via the factory.
    SDOM_OrphanRetentionPolicy_AutoDestroy = 0x0001, ///< Destroy immediately after the object becomes orphaned.
    SDOM_OrphanRetentionPolicy_GracePeriod = 0x0002 ///< Allow reparenting within a grace window before destruction.
} SDOM_OrphanRetentionPolicy;

#ifdef __cplusplus
extern "C" {
#endif

// Callable bindings

/**
 * @brief Clears the dirty state for the target display object and cascades to its descendants.
 *
 * C++:   void IDisplayObject::cleanAll()
 * C API: bool SDOM_CleanAll(const SDOM_DisplayHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_CleanAll(const SDOM_DisplayHandle* handle);

/**
 * @brief Marks the display object as dirty so it will be refreshed next frame.
 *
 * C++:   bool IDisplayObjectAPI::SetDirty(IDisplayObject* object)
 * C API: bool SDOM_SetDirty(const SDOM_DisplayHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetDirty(const SDOM_DisplayHandle* handle);

/**
 * @brief Explicitly sets the dirty flag on the target display object to the provided state.
 *
 * C++:   bool IDisplayObjectAPI::SetDirtyState(IDisplayObject* object, bool dirty)
 * C API: bool SDOM_SetDirtyState(const SDOM_DisplayHandle* handle, bool dirty)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetDirtyState(const SDOM_DisplayHandle* handle, bool dirty);

/**
 * @brief Returns true if the target display object is currently marked dirty.
 *
 * C++:   bool IDisplayObject::isDirty() const
 * C API: bool SDOM_IsDirty(const SDOM_DisplayHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_IsDirty(const SDOM_DisplayHandle* handle);

/**
 * @brief Prints the display tree for the target object to the configured log/console.
 *
 * C++:   void IDisplayObject::printTree() const
 * C API: bool SDOM_PrintTree(const SDOM_DisplayHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_PrintTree(const SDOM_DisplayHandle* handle);

#ifdef __cplusplus
} // extern "C"
#endif
