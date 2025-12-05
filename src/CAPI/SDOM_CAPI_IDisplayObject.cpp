// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_IDisplayObject.cpp
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

#include <SDOM/CAPI/SDOM_CAPI_IDisplayObject.h>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

bool SDOM_IDisplayObject_CleanAll(const SDOM_DisplayHandle* handle) {
    // Dispatch family: method_table (DisplayObject)
    if (!handle) {
        SDOM_SetError("SDOM_IDisplayObject_CleanAll: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IDisplayObject_CleanAll", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_IDisplayObject_SetDirty(const SDOM_DisplayHandle* handle) {
    // Dispatch family: method_table (DisplayObject)
    if (!handle) {
        SDOM_SetError("SDOM_IDisplayObject_SetDirty: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IDisplayObject_SetDirty", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_IDisplayObject_SetDirtyState(const SDOM_DisplayHandle* handle, bool dirty) {
    // Dispatch family: method_table (DisplayObject)
    if (!handle) {
        SDOM_SetError("SDOM_IDisplayObject_SetDirtyState: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));
    args.push_back(SDOM::CAPI::CallArg::makeBool(dirty));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IDisplayObject_SetDirtyState", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_IDisplayObject_IsDirty(const SDOM_DisplayHandle* handle) {
    // Dispatch family: method_table (DisplayObject)
    if (!handle) {
        SDOM_SetError("SDOM_IDisplayObject_IsDirty: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IDisplayObject_IsDirty", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

#ifdef __cplusplus
} // extern "C"
#endif
