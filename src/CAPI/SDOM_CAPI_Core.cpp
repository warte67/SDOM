#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

const char* SDOM_GetError(void) {
    // Dispatch family: singleton (Core)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetError", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

bool SDOM_SetError(const char* message) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeCString(message));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetError", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Init(uint64_t init_flags) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeUInt(static_cast<std::uint64_t>(init_flags)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Init", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Configure(const SDOM_CoreConfig* cfg) {
    // Dispatch family: singleton (Core)
    if (!cfg) {
        SDOM_SetError("SDOM_Configure: subject 'cfg' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(cfg))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Configure", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetCoreConfig(SDOM_CoreConfig* out_cfg) {
    // Dispatch family: singleton (Core)
    if (!out_cfg) {
        SDOM_SetError("SDOM_GetCoreConfig: subject 'out_cfg' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_cfg)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetCoreConfig", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

void SDOM_Quit(void) {
    // Dispatch family: singleton (Core)
    (void)SDOM::CAPI::invokeCallable("SDOM_Quit", {});
    return;
}

#ifdef __cplusplus
} // extern "C"
#endif
