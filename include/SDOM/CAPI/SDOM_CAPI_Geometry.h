// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_Geometry.h
//  Module: Geometry
//
//  Brief:
//    AnchorPoint and Bounds helpers for the SDOM C API.
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

typedef enum SDOM_AnchorPoint {
    /* Layout & Anchors events */
    SDOM_AnchorPoint_TOP_LEFT = 0x0000,             ///< Anchor to the parent's top-left corner.
    SDOM_AnchorPoint_TOP_CENTER = 0x0001,           ///< Anchor to the midpoint of the parent's top edge.
    SDOM_AnchorPoint_TOP_RIGHT = 0x0002,            ///< Anchor to the parent's top-right corner.
    SDOM_AnchorPoint_MIDDLE_LEFT = 0x0003,          ///< Anchor to the midpoint of the parent's left edge.
    SDOM_AnchorPoint_MIDDLE_CENTER = 0x0004,        ///< Anchor to the parent's center point.
    SDOM_AnchorPoint_MIDDLE_RIGHT = 0x0005,         ///< Anchor to the midpoint of the parent's right edge.
    SDOM_AnchorPoint_BOTTOM_LEFT = 0x0006,          ///< Anchor to the parent's bottom-left corner.
    SDOM_AnchorPoint_BOTTOM_CENTER = 0x0007,        ///< Anchor to the midpoint of the parent's bottom edge.
    SDOM_AnchorPoint_BOTTOM_RIGHT = 0x0008          ///< Anchor to the parent's bottom-right corner.
} SDOM_AnchorPoint;

typedef struct SDOM_Bounds {
    float left;                                     ///< Position of the left edge in parent space.
    float top;                                      ///< Position of the top edge in parent space.
    float right;                                    ///< Position of the right edge in parent space.
    float bottom;                                   ///< Position of the bottom edge in parent space.
} SDOM_Bounds;

#ifdef __cplusplus
extern "C" {
#endif

// Callable bindings

/**
 * @brief Parses the provided string (case-insensitive, supports alias separators) into an AnchorPoint enum.
 *
 * C++:   SDOM::AnchorPoint GeometryAPI::anchorPointFromString(const char*)
 * C API: SDOM_AnchorPoint SDOM_Geometry_AnchorPointFromString(const char* name)
 *
 * @param name Pointer parameter.
 * @return SDOM_AnchorPoint; check SDOM_GetError() for details on failure.
 */
SDOM_AnchorPoint SDOM_Geometry_AnchorPointFromString(const char* name);

/**
 * @brief Returns the canonical string name for the given AnchorPoint value.
 *
 * C++:   const char* GeometryAPI::anchorPointToString(SDOM::AnchorPoint)
 * C API: const char* SDOM_Geometry_AnchorPointToString(SDOM_AnchorPoint anchor)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_Geometry_AnchorPointToString(SDOM_AnchorPoint anchor);

/**
 * @brief Assigns the four edges of a Bounds struct.
 *
 * C++:   bool GeometryAPI::setBounds(SDOM::Bounds*, float, float, float, float)
 * C API: bool SDOM_Geometry_SetBounds(SDOM_Bounds* bounds, float left, float top, float right, float bottom)
 *
 * @param bounds Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Geometry_SetBounds(SDOM_Bounds* bounds, float left, float top, float right, float bottom);

/**
 * @brief Returns the absolute width of the bounds.
 *
 * C++:   float GeometryAPI::getBoundsWidth(const SDOM::Bounds*)
 * C API: float SDOM_Geometry_GetBoundsWidth(const SDOM_Bounds* bounds)
 *
 * @param bounds Pointer parameter.
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_Geometry_GetBoundsWidth(const SDOM_Bounds* bounds);

/**
 * @brief Returns the absolute height of the bounds.
 *
 * C++:   float GeometryAPI::getBoundsHeight(const SDOM::Bounds*)
 * C API: float SDOM_Geometry_GetBoundsHeight(const SDOM_Bounds* bounds)
 *
 * @param bounds Pointer parameter.
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_Geometry_GetBoundsHeight(const SDOM_Bounds* bounds);

#ifdef __cplusplus
} // extern "C"
#endif
