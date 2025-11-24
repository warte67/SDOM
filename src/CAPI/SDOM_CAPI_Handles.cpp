#include <SDOM/CAPI/SDOM_CAPI_Handles.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

bool SDOM_AssetHandle_IsValid(const SDOM_AssetHandle* handle) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_AssetHandle_IsValid", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_DisplayHandle_IsValid(const SDOM_DisplayHandle* handle) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_DisplayHandle_IsValid", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

#ifdef __cplusplus
} // extern "C"
#endif
