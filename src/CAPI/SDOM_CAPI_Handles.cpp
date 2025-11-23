#include <SDOM/CAPI/SDOM_CAPI_Handles.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

bool SDOM_AssetHandle_IsValid(const SDOM_AssetHandle* handle) {
    (void)handle;

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_AssetHandle_IsValid", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_DisplayHandle_IsValid(const SDOM_DisplayHandle* handle) {
    (void)handle;

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_DisplayHandle_IsValid", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

#ifdef __cplusplus
} // extern "C"
#endif
