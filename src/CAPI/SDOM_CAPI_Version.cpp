// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_Version.cpp
//  Module: Version
//
//  Brief:
//    Build/version metadata
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

#include <SDOM/CAPI/SDOM_CAPI_Version.h>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

const char* SDOM_GetVersionString(void) {
    // Dispatch family: singleton (Version)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionString", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

const char* SDOM_GetVersionFullString(void) {
    // Dispatch family: singleton (Version)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionFullString", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

int SDOM_GetVersionMajor(void) {
    // Dispatch family: singleton (Version)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionMajor", {});
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<int>(callResult.v.i);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<int>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<int>(callResult.v.d);
    }
    return static_cast<int>(0);
}

int SDOM_GetVersionMinor(void) {
    // Dispatch family: singleton (Version)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionMinor", {});
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<int>(callResult.v.i);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<int>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<int>(callResult.v.d);
    }
    return static_cast<int>(0);
}

int SDOM_GetVersionPatch(void) {
    // Dispatch family: singleton (Version)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionPatch", {});
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<int>(callResult.v.i);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<int>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<int>(callResult.v.d);
    }
    return static_cast<int>(0);
}

const char* SDOM_GetVersionCodename(void) {
    // Dispatch family: singleton (Version)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionCodename", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

const char* SDOM_GetVersionBuild(void) {
    // Dispatch family: singleton (Version)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionBuild", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

const char* SDOM_GetVersionBuildDate(void) {
    // Dispatch family: singleton (Version)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionBuildDate", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

const char* SDOM_GetVersionCommit(void) {
    // Dispatch family: singleton (Version)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionCommit", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

const char* SDOM_GetVersionBranch(void) {
    // Dispatch family: singleton (Version)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionBranch", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

const char* SDOM_GetVersionCompiler(void) {
    // Dispatch family: singleton (Version)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionCompiler", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

const char* SDOM_GetVersionPlatform(void) {
    // Dispatch family: singleton (Version)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetVersionPlatform", {});
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
