#include <SDOM/CAPI/SDOM_CAPI_Event.h>
#include <SDOM/SDOM_DataRegistry.hpp>

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

#ifdef __cplusplus
} // extern "C"
#endif
