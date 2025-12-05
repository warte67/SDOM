// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_Variant.cpp
//  Module: Variant
//
//  Brief:
//    Variant value marshaling utilities for the SDOM C API.
// =============================================================================
//
//  Authors:
//    Jay Faries (warte67) - Primary architect of SDOM
//
//  File Type: Source
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

#include <SDOM/CAPI/SDOM_CAPI_Variant.h>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

bool SDOM_Variant_InitNull(SDOM_Variant* variant) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_InitNull: subject 'variant' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(variant)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_InitNull", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_InitBool(SDOM_Variant* variant, bool value) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_InitBool: subject 'variant' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(variant)));
    args.push_back(SDOM::CAPI::CallArg::makeBool(value));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_InitBool", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_InitInt(SDOM_Variant* variant, int64_t value) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_InitInt: subject 'variant' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(variant)));
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(value)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_InitInt", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_InitDouble(SDOM_Variant* variant, double value) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_InitDouble: subject 'variant' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(variant)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(value)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_InitDouble", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_InitString(SDOM_Variant* variant, const char* utf8) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_InitString: subject 'variant' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(variant)));
    args.push_back(SDOM::CAPI::CallArg::makeCString(utf8));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_InitString", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_Copy(const SDOM_Variant* source, SDOM_Variant* destination) {
    // Dispatch family: singleton (Variant)
    if (!source) {
        SDOM_SetError("SDOM_Variant_Copy: subject 'source' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(source))));
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(destination)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_Copy", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_Destroy(SDOM_Variant* variant) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_Destroy: subject 'variant' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(variant)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_Destroy", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

SDOM_VariantType SDOM_Variant_GetType(const SDOM_Variant* variant) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_GetType: subject 'variant' is null");
        return SDOM_VariantType_Error;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(variant))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_GetType", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<SDOM_VariantType>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<SDOM_VariantType>(callResult.v.i);
    }
    return SDOM_VariantType_Error;
}

bool SDOM_Variant_ToBool(const SDOM_Variant* variant, bool* out_value) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_ToBool: subject 'variant' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(variant))));
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_value)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_ToBool", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_ToInt64(const SDOM_Variant* variant, int64_t* out_value) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_ToInt64: subject 'variant' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(variant))));
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_value)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_ToInt64", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_ToDouble(const SDOM_Variant* variant, double* out_value) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_ToDouble: subject 'variant' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(variant))));
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_value)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_ToDouble", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

const char* SDOM_Variant_ToString(const SDOM_Variant* variant) {
    // Dispatch family: singleton (Variant)
    if (!variant) {
        SDOM_SetError("SDOM_Variant_ToString: subject 'variant' is null");
        return nullptr;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(variant))));

    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_ToString", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

#ifdef __cplusplus
} // extern "C"
#endif
