#include <SDOM/CAPI/SDOM_CAPI_Handles.h>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/CAPI/SDOM_CAPI_Variant.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

bool SDOM_Handle_IsDisplay(const SDOM_Variant* handle) {
    // Dispatch family: singleton (Core)
    if (!handle) {
        SDOM_SetError("SDOM_Handle_IsDisplay: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Handle_IsDisplay", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Handle_IsAsset(const SDOM_Variant* handle) {
    // Dispatch family: singleton (Core)
    if (!handle) {
        SDOM_SetError("SDOM_Handle_IsAsset: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Handle_IsAsset", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Handle_IsValid(const SDOM_Variant* handle) {
    // Dispatch family: singleton (Core)
    if (!handle) {
        SDOM_SetError("SDOM_Handle_IsValid: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Handle_IsValid", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

uint64_t SDOM_Handle_ObjectId(const SDOM_Variant* handle) {
    // Dispatch family: singleton (Core)
    if (!handle) {
        SDOM_SetError("SDOM_Handle_ObjectId: subject 'handle' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Handle_ObjectId", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<uint64_t>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        const auto value = callResult.v.i < 0 ? std::int64_t{0} : callResult.v.i;
        return static_cast<uint64_t>(value);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        const auto value = callResult.v.d < 0.0 ? 0.0 : callResult.v.d;
        return static_cast<uint64_t>(value);
    }
    return static_cast<uint64_t>(0);
}

#ifdef __cplusplus
} // extern "C"
#endif
