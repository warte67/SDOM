// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_Geometry.cpp
//  Module: Geometry
//
//  Brief:
//    AnchorPoint and Bounds helpers for the SDOM C API.
// =============================================================================
//
//  Authors:
//    Jay Faries (warte67) - Primary architect of SDOM
//
//  File Type: Source
//  SDOM Version: 0.5.263 (early pre-alpha)
//  Build Identifier: 2025-12-04_21:24:47_20e8cb42
//  Commit: 20e8cb42 on branch master
//  Compiler: g++ (GCC) 15.2.1 20251112
//  Platform: Linux-x86_64
//  Generated: 2025-12-04_21:24:47
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

#include <SDOM/CAPI/SDOM_CAPI_Geometry.h>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

SDOM_AnchorPoint SDOM_Geometry_AnchorPointFromString(const char* name) {
    // Dispatch family: singleton (Geometry)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeCString(name));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Geometry_AnchorPointFromString", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<SDOM_AnchorPoint>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<SDOM_AnchorPoint>(callResult.v.i);
    }
    return static_cast<SDOM_AnchorPoint>(0);
}

const char* SDOM_Geometry_AnchorPointToString(SDOM_AnchorPoint anchor) {
    // Dispatch family: singleton (Geometry)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(anchor)));

    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Geometry_AnchorPointToString", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

bool SDOM_Geometry_SetBounds(SDOM_Bounds* bounds, float left, float top, float right, float bottom) {
    // Dispatch family: singleton (Geometry)
    if (!bounds) {
        SDOM_SetError("SDOM_Geometry_SetBounds: subject 'bounds' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(5);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(bounds)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(left)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(top)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(right)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(bottom)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Geometry_SetBounds", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

float SDOM_Geometry_GetBoundsWidth(const SDOM_Bounds* bounds) {
    // Dispatch family: singleton (Geometry)
    if (!bounds) {
        SDOM_SetError("SDOM_Geometry_GetBoundsWidth: subject 'bounds' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(bounds))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Geometry_GetBoundsWidth", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<float>(callResult.v.d);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<float>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<float>(callResult.v.i);
    }
    return static_cast<float>(0);
}

float SDOM_Geometry_GetBoundsHeight(const SDOM_Bounds* bounds) {
    // Dispatch family: singleton (Geometry)
    if (!bounds) {
        SDOM_SetError("SDOM_Geometry_GetBoundsHeight: subject 'bounds' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(bounds))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Geometry_GetBoundsHeight", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<float>(callResult.v.d);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<float>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<float>(callResult.v.i);
    }
    return static_cast<float>(0);
}

#ifdef __cplusplus
} // extern "C"
#endif
