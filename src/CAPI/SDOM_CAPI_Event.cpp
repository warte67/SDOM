// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_Event.cpp
//  Module: Event
//
//  Brief:
//    Reserved for custom or user-defined events created at runtime.
// =============================================================================
//
//  Authors:
//    Jay Faries (warte67) - Primary architect of SDOM
//
//  File Type: Source
//  SDOM Version: 0.5.264 (early pre-alpha)
//  Build Identifier: 2025-12-04_22:07:48_ddcdaf40
//  Commit: ddcdaf40 on branch master
//  Compiler: g++ (GCC) 15.2.1 20251112
//  Platform: Linux-x86_64
//  Generated: 2025-12-04_22:07:48
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

#include <SDOM/CAPI/SDOM_CAPI_Event.h>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

static inline void* eventImplPtr(const SDOM_Event* evt)
{
    return evt ? evt->impl : nullptr;
}

bool SDOM_GetEventType(const SDOM_Event* evt, SDOM_EventType* out_type) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventType: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
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
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

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
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

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
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
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
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

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
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
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
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
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
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
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
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
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
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
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
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(new_target))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventRelatedTarget", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

const char* SDOM_GetEventSDLJson(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventSDLJson: subject 'evt' is null");
        return nullptr;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventSDLJson", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

bool SDOM_SetEventSDLJson(SDOM_Event* evt, const char* json) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventSDLJson: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeCString(json));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventSDLJson", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_IsEventPropagationStopped(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_IsEventPropagationStopped: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IsEventPropagationStopped", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_StopEventPropagation(SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_StopEventPropagation: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_StopEventPropagation", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_IsEventDefaultBehaviorDisabled(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_IsEventDefaultBehaviorDisabled: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IsEventDefaultBehaviorDisabled", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetEventDisableDefaultBehavior(SDOM_Event* evt, bool disable) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventDisableDefaultBehavior: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeBool(disable));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventDisableDefaultBehavior", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetEventUseCapture(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventUseCapture: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventUseCapture", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetEventUseCapture(SDOM_Event* evt, bool use_capture) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventUseCapture: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeBool(use_capture));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventUseCapture", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

float SDOM_GetEventElapsedTime(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventElapsedTime: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventElapsedTime", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<float>(callResult.v.d);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<float>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<float>(callResult.v.i);
    }
    return static_cast<float>(0);
}

bool SDOM_SetEventElapsedTime(SDOM_Event* evt, float elapsed_time) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventElapsedTime: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(elapsed_time)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventElapsedTime", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

float SDOM_GetEventMouseX(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventMouseX: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventMouseX", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<float>(callResult.v.d);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<float>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<float>(callResult.v.i);
    }
    return static_cast<float>(0);
}

bool SDOM_SetEventMouseX(SDOM_Event* evt, float x) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventMouseX: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(x)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventMouseX", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

float SDOM_GetEventMouseY(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventMouseY: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventMouseY", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<float>(callResult.v.d);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<float>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<float>(callResult.v.i);
    }
    return static_cast<float>(0);
}

bool SDOM_SetEventMouseY(SDOM_Event* evt, float y) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventMouseY: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(y)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventMouseY", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

float SDOM_GetEventWheelX(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventWheelX: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventWheelX", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<float>(callResult.v.d);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<float>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<float>(callResult.v.i);
    }
    return static_cast<float>(0);
}

bool SDOM_SetEventWheelX(SDOM_Event* evt, float x) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventWheelX: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(x)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventWheelX", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

float SDOM_GetEventWheelY(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventWheelY: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventWheelY", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<float>(callResult.v.d);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<float>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<float>(callResult.v.i);
    }
    return static_cast<float>(0);
}

bool SDOM_SetEventWheelY(SDOM_Event* evt, float y) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventWheelY: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(y)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventWheelY", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

float SDOM_GetEventDragOffsetX(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventDragOffsetX: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventDragOffsetX", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<float>(callResult.v.d);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<float>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<float>(callResult.v.i);
    }
    return static_cast<float>(0);
}

bool SDOM_SetEventDragOffsetX(SDOM_Event* evt, float offset_x) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventDragOffsetX: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(offset_x)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventDragOffsetX", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

float SDOM_GetEventDragOffsetY(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventDragOffsetY: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventDragOffsetY", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<float>(callResult.v.d);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<float>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<float>(callResult.v.i);
    }
    return static_cast<float>(0);
}

bool SDOM_SetEventDragOffsetY(SDOM_Event* evt, float offset_y) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventDragOffsetY: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(offset_y)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventDragOffsetY", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

int SDOM_GetEventClickCount(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventClickCount: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventClickCount", args);
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

bool SDOM_SetEventClickCount(SDOM_Event* evt, int count) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventClickCount: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(count)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventClickCount", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

uint8_t SDOM_GetEventButton(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventButton: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventButton", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<uint8_t>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        const auto value = callResult.v.i < 0 ? std::int64_t{0} : callResult.v.i;
        return static_cast<uint8_t>(value);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        const auto value = callResult.v.d < 0.0 ? 0.0 : callResult.v.d;
        return static_cast<uint8_t>(value);
    }
    return static_cast<uint8_t>(0);
}

bool SDOM_SetEventButton(SDOM_Event* evt, uint8_t button) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventButton: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeUInt(static_cast<std::uint64_t>(button)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventButton", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

int SDOM_GetEventScanCode(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventScanCode: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventScanCode", args);
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

bool SDOM_SetEventScanCode(SDOM_Event* evt, int scancode) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventScanCode: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(scancode)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventScanCode", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

int SDOM_GetEventKeycode(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventKeycode: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventKeycode", args);
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

bool SDOM_SetEventKeycode(SDOM_Event* evt, int keycode) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventKeycode: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(keycode)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventKeycode", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

int SDOM_GetEventKeymod(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventKeymod: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventKeymod", args);
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

bool SDOM_SetEventKeymod(SDOM_Event* evt, int keymod) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventKeymod: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(keymod)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventKeymod", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

int SDOM_GetEventAsciiCode(const SDOM_Event* evt) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_GetEventAsciiCode: subject 'evt' is null");
        return {};
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetEventAsciiCode", args);
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

bool SDOM_SetEventAsciiCode(SDOM_Event* evt, int ascii_code) {
    // Dispatch family: event_router (Event)
    if (!evt) {
        SDOM_SetError("SDOM_SetEventAsciiCode: subject 'evt' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(eventImplPtr(evt)));
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(ascii_code)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetEventAsciiCode", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

#ifdef __cplusplus
} // extern "C"
#endif
