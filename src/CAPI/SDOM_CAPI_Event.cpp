#include <SDOM/CAPI/SDOM_CAPI_Event.h>
#include <SDOM/SDOM_CAPI.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

bool SDOM_GetEventType(const SDOM_Event* evt, SDOM_EventType* out_type) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventType: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(evt))));
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_type)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventType", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

const char* SDOM_GetEventTypeName(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventTypeName: subject 'evt' is null");
        return nullptr;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(evt))));

    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventTypeName", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

SDOM_EventPhase SDOM_GetEventPhase(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventPhase: subject 'evt' is null");
        return static_cast<SDOM_EventPhase>(0);
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(evt))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventPhase", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<SDOM_EventPhase>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<SDOM_EventPhase>(callResult.v.i);
    }
    return static_cast<SDOM_EventPhase>(0);
}

bool SDOM_SetEventPhase(SDOM_Event* evt, SDOM_EventPhase phase) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventPhase: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(phase)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventPhase", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

const char* SDOM_GetEventPhaseString(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventPhaseString: subject 'evt' is null");
        return nullptr;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(evt))));

    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventPhaseString", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

bool SDOM_GetEventTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventTarget: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(evt))));
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_target)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventTarget", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetEventTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventTarget: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(evt)));
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(new_target))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventTarget", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetEventCurrentTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventCurrentTarget: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(evt))));
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_target)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventCurrentTarget", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetEventCurrentTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventCurrentTarget: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(evt)));
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(new_target))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventCurrentTarget", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetEventRelatedTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventRelatedTarget: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(evt))));
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_target)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventRelatedTarget", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetEventRelatedTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventRelatedTarget: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(evt)));
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(new_target))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventRelatedTarget", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

#ifdef __cplusplus
} // extern "C"
#endif
