#include <SDOM/CAPI/SDOM_CAPI_Version.h>
#include <SDOM/SDOM_CAPI.h>
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
