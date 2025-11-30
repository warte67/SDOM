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

bool SDOM_ConfigureFromJson(const char* json) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeCString(json));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_ConfigureFromJson", args);
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

bool SDOM_SetStopAfterUnitTests(bool stop) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeBool(stop));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetStopAfterUnitTests", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetIsRunning(bool running) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeBool(running));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetIsRunning", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_LoadDomFromJsonFile(const char* filename) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeCString(filename));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_LoadDomFromJsonFile", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_LoadDomFromJson(const char* json) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeCString(json));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_LoadDomFromJson", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetBorderColor(SDL_Color* out_color) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_color)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetBorderColor", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetBorderColor(const SDL_Color* color) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(color))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetBorderColor", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

const char* SDOM_GetWindowTitle(void) {
    // Dispatch family: singleton (Core)
    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetWindowTitle", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

bool SDOM_SetWindowTitle(const char* title) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeCString(title));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetWindowTitle", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

float SDOM_GetElapsedTime(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetElapsedTime", {});
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

float SDOM_GetWindowWidth(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetWindowWidth", {});
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

float SDOM_GetWindowHeight(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetWindowHeight", {});
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

float SDOM_GetPixelWidth(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetPixelWidth", {});
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

float SDOM_GetPixelHeight(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetPixelHeight", {});
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

bool SDOM_GetPreserveAspectRatio(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetPreserveAspectRatio", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetAllowTextureResize(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetAllowTextureResize", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

SDL_RendererLogicalPresentation SDOM_GetLogicalPresentation(void) {
    // Dispatch family: singleton (Core)
    // TODO: marshal return type 'SDL_RendererLogicalPresentation'.
    (void)SDOM::CAPI::invokeCallable("SDOM_GetLogicalPresentation", {});
    return {}; // placeholder
}

SDL_WindowFlags SDOM_GetWindowFlags(void) {
    // Dispatch family: singleton (Core)
    // TODO: marshal return type 'SDL_WindowFlags'.
    (void)SDOM::CAPI::invokeCallable("SDOM_GetWindowFlags", {});
    return {}; // placeholder
}

SDL_PixelFormat SDOM_GetPixelFormat(void) {
    // Dispatch family: singleton (Core)
    // TODO: marshal return type 'SDL_PixelFormat'.
    (void)SDOM::CAPI::invokeCallable("SDOM_GetPixelFormat", {});
    return {}; // placeholder
}

bool SDOM_IsFullscreen(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IsFullscreen", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_IsWindowed(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IsWindowed", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetIsTraversing(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetIsTraversing", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_HandleTabKeyPress(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_HandleTabKeyPress", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_HandleTabKeyPressReverse(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_HandleTabKeyPressReverse", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetWindow(SDL_Window** out_window) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_window)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetWindow", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetRenderer(SDL_Renderer** out_renderer) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_renderer)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetRenderer", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetTexture(SDL_Texture** out_texture) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_texture)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetTexture", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetWindowWidth(float width) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(width)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetWindowWidth", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetWindowHeight(float height) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(height)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetWindowHeight", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetPixelWidth(float width) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(width)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetPixelWidth", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetPixelHeight(float height) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(height)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetPixelHeight", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetPreserveAspectRatio(bool preserve) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeBool(preserve));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetPreserveAspectRatio", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetAllowTextureResize(bool allow) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeBool(allow));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetAllowTextureResize", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetLogicalPresentation(SDL_RendererLogicalPresentation presentation) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(presentation)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetLogicalPresentation", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetWindowFlags(SDL_WindowFlags flags) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(flags)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetWindowFlags", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetPixelFormat(SDL_PixelFormat fmt) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(fmt)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetPixelFormat", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetFullscreen(bool fullscreen) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeBool(fullscreen));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetFullscreen", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetWindowed(bool windowed) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeBool(windowed));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetWindowed", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetIsTraversing(bool traversing) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeBool(traversing));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetIsTraversing", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetKeyboardFocus(const SDOM_DisplayHandle* handle) {
    // Dispatch family: singleton (Core)
    if (!handle) {
        SDOM_SetError("SDOM_SetKeyboardFocus: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetKeyboardFocus", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetKeyboardFocus(SDOM_DisplayHandle* out_handle) {
    // Dispatch family: singleton (Core)
    if (!out_handle) {
        SDOM_SetError("SDOM_GetKeyboardFocus: subject 'out_handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_handle)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetKeyboardFocus", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_ClearKeyboardFocus(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_ClearKeyboardFocus", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetMouseHover(const SDOM_DisplayHandle* handle) {
    // Dispatch family: singleton (Core)
    if (!handle) {
        SDOM_SetError("SDOM_SetMouseHover: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetMouseHover", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetMouseHover(SDOM_DisplayHandle* out_handle) {
    // Dispatch family: singleton (Core)
    if (!out_handle) {
        SDOM_SetError("SDOM_GetMouseHover: subject 'out_handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_handle)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetMouseHover", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_ClearMouseHover(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_ClearMouseHover", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

int SDOM_GetFrameCount(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetFrameCount", {});
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

bool SDOM_Run(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Run", {});
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetStopAfterUnitTests(void) {
    // Dispatch family: singleton (Core)
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetStopAfterUnitTests", {});
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

bool SDOM_SetRootNode(const SDOM_DisplayHandle* handle) {
    // Dispatch family: singleton (Core)
    if (!handle) {
        SDOM_SetError("SDOM_SetRootNode: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetRootNode", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetRootNodeByName(const char* name) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeCString(name));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetRootNodeByName", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetStage(const SDOM_DisplayHandle* handle) {
    // Dispatch family: singleton (Core)
    if (!handle) {
        SDOM_SetError("SDOM_SetStage: subject 'handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(handle))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetStage", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_SetStageByName(const char* name) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeCString(name));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_SetStageByName", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetRootNode(SDOM_DisplayHandle* out_handle) {
    // Dispatch family: singleton (Core)
    if (!out_handle) {
        SDOM_SetError("SDOM_GetRootNode: subject 'out_handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_handle)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetRootNode", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_GetStageHandle(SDOM_DisplayHandle* out_handle) {
    // Dispatch family: singleton (Core)
    if (!out_handle) {
        SDOM_SetError("SDOM_GetStageHandle: subject 'out_handle' is null");
        return false;
    }

    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_handle)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_GetStageHandle", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_RegisterOnInit(void* fn) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(fn)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_RegisterOnInit", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_RegisterOnQuit(void* fn) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(fn)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_RegisterOnQuit", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_RegisterOnUpdate(void* fn) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(fn)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_RegisterOnUpdate", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_RegisterOnEvent(void* fn) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(fn)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_RegisterOnEvent", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_RegisterOnRender(void* fn) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(fn)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_RegisterOnRender", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_RegisterOnUnitTest(void* fn) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(fn)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_RegisterOnUnitTest", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_RegisterOnWindowResize(void* fn) {
    // Dispatch family: singleton (Core)
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(fn)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_RegisterOnWindowResize", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

#ifdef __cplusplus
} // extern "C"
#endif
