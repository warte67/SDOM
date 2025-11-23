#include <SDOM/CAPI/SDOM_CAPI_Event.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

bool SDOM_GetEventType(const SDOM_Event* evt, SDOM_EventType* out_type) {
    (void)evt;
    (void)out_type;

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventType", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

const char* SDOM_GetEventTypeName(const SDOM_Event* evt) {
    (void)evt;

    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventTypeName", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

SDOM_EventPhase SDOM_GetEventPhase(const SDOM_Event* evt) {
    (void)evt;

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventPhase", {});
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<SDOM_EventPhase>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<SDOM_EventPhase>(callResult.v.i);
    }
    return static_cast<SDOM_EventPhase>(0);
}

bool SDOM_SetEventPhase(SDOM_Event* evt, SDOM_EventPhase phase) {
    (void)evt;
    (void)phase;

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventPhase", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

const char* SDOM_GetEventPhaseString(const SDOM_Event* evt) {
    (void)evt;

    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventPhaseString", {});
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
