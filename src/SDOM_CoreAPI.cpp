#include <SDOM/SDOM_CoreAPI.hpp>
#include <SDOM/SDOM_APIRegistrar.hpp>

#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_EventManager.hpp>

#include <cstring>
#include <mutex>
#include <vector>

namespace SDOM {
namespace CoreAPI {
namespace {

using RegistrarBase = SDOM::API::IAPIRegistrar;

SDOM::CAPI::CallResult makeBoolResult(bool value)
{
    return RegistrarBase::makeBoolResult(value);
}

SDOM::CAPI::CallResult makeStringResult(const std::string& value)
{
    return RegistrarBase::makeStringResult(value);
}

std::string& lastErrorStorage()
{
    static std::string error;
    return error;
}

std::mutex& errorMutex()
{
    static std::mutex mtx;
    return mtx;
}

SDOM::Core::CoreConfig toCoreConfig(const SDOM_CoreConfig* cfg)
{
    SDOM::Core::CoreConfig out;
    if (!cfg) {
        return out;
    }

    out.windowWidth = cfg->windowWidth;
    out.windowHeight = cfg->windowHeight;
    out.pixelWidth = cfg->pixelWidth;
    out.pixelHeight = cfg->pixelHeight;
    out.preserveAspectRatio = cfg->preserveAspectRatio != 0;
    out.allowTextureResize = cfg->allowTextureResize != 0;
    out.rendererLogicalPresentation = cfg->rendererLogicalPresentation;
    out.windowFlags = cfg->windowFlags;
    out.pixelFormat = cfg->pixelFormat;
    out.color = cfg->color;
    return out;
}

void copyCoreConfig(const SDOM::Core::CoreConfig& src, SDOM_CoreConfig& dst)
{
    dst.windowWidth = src.windowWidth;
    dst.windowHeight = src.windowHeight;
    dst.pixelWidth = src.pixelWidth;
    dst.pixelHeight = src.pixelHeight;
    dst.preserveAspectRatio = src.preserveAspectRatio ? 1 : 0;
    dst.allowTextureResize = src.allowTextureResize ? 1 : 0;
    dst.rendererLogicalPresentation = src.rendererLogicalPresentation;
    dst.windowFlags = src.windowFlags;
    dst.pixelFormat = src.pixelFormat;
    dst.color = src.color;
}

bool parseSyntheticMouseEventJson(const char* jsonStr, SDL_Event& outEvent)
{
    if (!jsonStr) {
        setErrorMessage("SDOM_PushMouseEvent: json is null");
        return false;
    }

    try {
        auto& core = SDOM::Core::getInstance();
        nlohmann::json j = nlohmann::json::parse(jsonStr);
        if (!j.contains("x") || !j.contains("y")) {
            setErrorMessage("SDOM_PushMouseEvent: missing x/y");
            return false;
        }

        float stageX = j.value("x", 0.0f);
        float stageY = j.value("y", 0.0f);
        std::string type = j.value("type", std::string("down"));
        int button = j.value("button", 1);
        int clicks = j.value("clicks", 1);

        float winX = 0.0f;
        float winY = 0.0f;
        SDL_Renderer* renderer = core.getRenderer();
        if (renderer) {
            SDL_RenderCoordinatesToWindow(renderer, stageX, stageY, &winX, &winY);
        } else {
            const SDOM::Core::CoreConfig& cfg = core.getConfig();
            winX = stageX * cfg.pixelWidth;
            winY = stageY * cfg.pixelHeight;
        }

        Uint32 winID = 0;
        if (core.getWindow()) {
            winID = SDL_GetWindowID(core.getWindow());
        }

        std::memset(&outEvent, 0, sizeof(outEvent));
        if (type == "up") {
            outEvent.type = SDL_EVENT_MOUSE_BUTTON_UP;
            outEvent.button.windowID = winID;
            outEvent.button.which = 0;
            outEvent.button.button = button;
            outEvent.button.clicks = clicks;
            outEvent.button.x = winX;
            outEvent.button.y = winY;
            outEvent.motion.windowID = winID;
            outEvent.motion.which = 0;
            outEvent.motion.x = winX;
            outEvent.motion.y = winY;
        } else if (type == "motion") {
            outEvent.type = SDL_EVENT_MOUSE_MOTION;
            outEvent.motion.windowID = winID;
            outEvent.motion.which = 0;
            outEvent.motion.x = winX;
            outEvent.motion.y = winY;
        } else {
            outEvent.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
            outEvent.button.windowID = winID;
            outEvent.button.which = 0;
            outEvent.button.button = button;
            outEvent.button.clicks = clicks;
            outEvent.button.x = winX;
            outEvent.button.y = winY;
            outEvent.motion.windowID = winID;
            outEvent.motion.which = 0;
            outEvent.motion.x = winX;
            outEvent.motion.y = winY;
        }

        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_PushMouseEvent unknown error");
        return false;
    }
}

bool parseSyntheticKeyboardEventJson(const char* jsonStr, SDL_Event& outEvent)
{
    if (!jsonStr) {
        setErrorMessage("SDOM_PushKeyboardEvent: json is null");
        return false;
    }

    try {
        auto& core = SDOM::Core::getInstance();
        nlohmann::json j = nlohmann::json::parse(jsonStr);
        if (!j.contains("key")) {
            setErrorMessage("SDOM_PushKeyboardEvent: missing key");
            return false;
        }

        int key = j.value("key", 0);
        int mod = j.value("mod", 0);
        std::string type = j.value("type", std::string("down"));
        int repeat = j.value("repeat", 0);

        std::memset(&outEvent, 0, sizeof(outEvent));
        outEvent.type = (type == "up") ? SDL_EVENT_KEY_UP : SDL_EVENT_KEY_DOWN;
        outEvent.key.windowID = core.getWindow() ? SDL_GetWindowID(core.getWindow()) : 0;
        outEvent.key.timestamp = SDL_GetTicks();
        outEvent.key.repeat = repeat;
        outEvent.key.mod = mod;
        outEvent.key.key = key;

        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_PushKeyboardEvent unknown error");
        return false;
    }
}

DisplayHandle resolveDisplayHandle(const SDOM_DisplayHandle* handle)
{
    if (!handle) return DisplayHandle();

    SDOM::Core& core = SDOM::Core::getInstance();
    SDOM::Factory& factory = core.getFactory();

    if (handle->name) {
        return factory.getDisplayObject(handle->name);
    }
    return DisplayHandle();
}

bool writeDisplayHandleOut(const DisplayHandle& src, SDOM_DisplayHandle* dst)
{
    if (!dst) {
        setErrorMessage("SDOM_GetRootNode: out_handle is null");
        return false;
    }

    if (!src.isValid() || !src.get()) {
        setErrorMessage("SDOM_GetRootNode: handle is not set");
        dst->object_id = 0;
        dst->name = nullptr;
        dst->type = nullptr;
        return false;
    }

    dst->object_id = 0;
    static thread_local std::string s_name;
    static thread_local std::string s_type;
    s_name = src.getName();
    s_type = src.getType();
    dst->name = s_name.c_str();
    dst->type = s_type.c_str();
    return true;
}

bool writeAssetHandleOut(const SDOM::AssetHandle& src, SDOM_AssetHandle* dst, const char* errCtx)
{
    if (!dst) {
        setErrorMessage(errCtx ? errCtx : "Asset handle out is null");
        return false;
    }

    if (!src.isValid() || !src.get()) {
        setErrorMessage(errCtx ? errCtx : "Asset handle is not set");
        dst->object_id = 0;
        dst->name = nullptr;
        dst->type = nullptr;
        return false;
    }

    dst->object_id = 0;
    static thread_local std::string s_name;
    static thread_local std::string s_type;
    s_name = src.getName();
    s_type = src.getType();
    dst->name = s_name.c_str();
    dst->type = s_type.c_str();
    return true;
}

void writeEventHandleOut(const SDOM::Event* src, SDOM_Event* dst)
{
    if (!dst)
        return;

    if (!src)
    {
        dst->impl = nullptr;
        return;
    }

    static thread_local SDOM::Event s_eventScratch;
    s_eventScratch = *src;
    dst->impl = &s_eventScratch;
}

bool parseJsonString(const char* jsonStr, nlohmann::json& out)
{
    if (!jsonStr) return false;
    try {
        out = nlohmann::json::parse(jsonStr);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    }
}

class BootstrapDispatchRegistrations {
public:
    BootstrapDispatchRegistrations()
    {
        using SDOM::CAPI::CallArg;
        using SDOM::CAPI::CallResult;

        SDOM::CAPI::registerCallable("SDOM_GetError",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getErrorString());
            });

        SDOM::CAPI::registerCallable("SDOM_SetError",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* message = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        message = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        message = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::setErrorMessage(message));
            });

        SDOM::CAPI::registerCallable("SDOM_Init",
            [](const std::vector<CallArg>& args) -> CallResult {
                std::uint64_t flags = 0;
                if (!args.empty() && args[0].kind == CallArg::Kind::UInt) {
                    flags = args[0].v.u;
                }
                return makeBoolResult(CoreAPI::init(flags));
            });

        SDOM::CAPI::registerCallable("SDOM_Configure",
            [](const std::vector<CallArg>& args) -> CallResult {
                const SDOM_CoreConfig* cfg = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    cfg = static_cast<const SDOM_CoreConfig*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::configure(cfg));
            });

        SDOM::CAPI::registerCallable("SDOM_GetCoreConfig",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* cfg = (args.empty() || args[0].kind != CallArg::Kind::Ptr)
                    ? nullptr
                    : static_cast<SDOM_CoreConfig*>(args[0].v.p);
                return makeBoolResult(CoreAPI::getCoreConfig(cfg));
            });

        SDOM::CAPI::registerCallable("SDOM_ConfigureFromJson",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* jsonStr = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        jsonStr = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        jsonStr = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::configureFromJson(jsonStr));
            });

        SDOM::CAPI::registerCallable("SDOM_LoadDomFromJson",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* jsonStr = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        jsonStr = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        jsonStr = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::loadDomFromJson(jsonStr));
            });

        SDOM::CAPI::registerCallable("SDOM_GetBorderColor",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* outColor = (args.empty() || args[0].kind != CallArg::Kind::Ptr)
                    ? nullptr
                    : static_cast<SDL_Color*>(args[0].v.p);
                return makeBoolResult(CoreAPI::getBorderColor(outColor));
            });

        SDOM::CAPI::registerCallable("SDOM_SetBorderColor",
            [](const std::vector<CallArg>& args) -> CallResult {
                const SDL_Color* color = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    color = static_cast<const SDL_Color*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::setBorderColor(color));
            });

        SDOM::CAPI::registerCallable("SDOM_GetWindowTitle",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getWindowTitle());
            });

        SDOM::CAPI::registerCallable("SDOM_SetWindowTitle",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* title = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        title = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        title = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::setWindowTitle(title));
            });

        SDOM::CAPI::registerCallable("SDOM_PumpEventsOnce",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::pumpEventsOnce());
            });

        SDOM::CAPI::registerCallable("SDOM_PollEvents",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* evt = (args.empty() || args[0].kind != CallArg::Kind::Ptr)
                    ? nullptr
                    : static_cast<SDOM_Event*>(args[0].v.p);
                return makeBoolResult(CoreAPI::pollEvents(evt));
            });

        SDOM::CAPI::registerCallable("SDOM_Update",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::updatePhase());
            });

        SDOM::CAPI::registerCallable("SDOM_Render",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::renderPhase());
            });

        SDOM::CAPI::registerCallable("SDOM_Present",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::presentPhase());
            });

        SDOM::CAPI::registerCallable("SDOM_RunFrame",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::runFramePhase());
            });

        SDOM::CAPI::registerCallable("SDOM_PushMouseEvent",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* jsonStr = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        jsonStr = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        jsonStr = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::pushMouseEvent(jsonStr));
            });

        SDOM::CAPI::registerCallable("SDOM_PushKeyboardEvent",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* jsonStr = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        jsonStr = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        jsonStr = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::pushKeyboardEvent(jsonStr));
            });

        SDOM::CAPI::registerCallable("SDOM_HasLuaSupport",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::hasLuaSupport());
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionString",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getVersionString());
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionFullString",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getVersionFullString());
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionMajor",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromInt(static_cast<std::int64_t>(CoreAPI::getVersionMajor()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionMinor",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromInt(static_cast<std::int64_t>(CoreAPI::getVersionMinor()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionPatch",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromInt(static_cast<std::int64_t>(CoreAPI::getVersionPatch()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionCodename",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getVersionCodename());
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionBuild",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getVersionBuild());
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionBuildDate",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getVersionBuildDate());
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionCommit",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getVersionCommit());
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionBranch",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getVersionBranch());
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionCompiler",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getVersionCompiler());
            });

        SDOM::CAPI::registerCallable("SDOM_GetVersionPlatform",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeStringResult(CoreAPI::getVersionPlatform());
            });

        SDOM::CAPI::registerCallable("SDOM_GetElapsedTime",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromDouble(static_cast<double>(CoreAPI::getElapsedTime()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetWindowWidth",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromDouble(static_cast<double>(CoreAPI::getWindowWidth()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetWindowHeight",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromDouble(static_cast<double>(CoreAPI::getWindowHeight()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetPixelWidth",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromDouble(static_cast<double>(CoreAPI::getPixelWidth()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetPixelHeight",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromDouble(static_cast<double>(CoreAPI::getPixelHeight()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetPreserveAspectRatio",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::getPreserveAspectRatio());
            });

        SDOM::CAPI::registerCallable("SDOM_GetAllowTextureResize",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::getAllowTextureResize());
            });

        SDOM::CAPI::registerCallable("SDOM_GetLogicalPresentation",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromUInt(static_cast<std::uint64_t>(CoreAPI::getLogicalPresentation()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetWindowFlags",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromUInt(static_cast<std::uint64_t>(CoreAPI::getWindowFlags()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetPixelFormat",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromUInt(static_cast<std::uint64_t>(CoreAPI::getPixelFormat()));
            });

        SDOM::CAPI::registerCallable("SDOM_IsFullscreen",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::isFullscreen());
            });

        SDOM::CAPI::registerCallable("SDOM_IsWindowed",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::isWindowed());
            });

        SDOM::CAPI::registerCallable("SDOM_GetIsTraversing",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::getIsTraversing());
            });

        SDOM::CAPI::registerCallable("SDOM_HandleTabKeyPress",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::handleTabKeyPress());
            });

        SDOM::CAPI::registerCallable("SDOM_HandleTabKeyPressReverse",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::handleTabKeyPressReverse());
            });

        SDOM::CAPI::registerCallable("SDOM_GetWindow",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* outWindow = (args.empty() || args[0].kind != CallArg::Kind::Ptr)
                    ? nullptr
                    : static_cast<SDL_Window**>(args[0].v.p);
                return makeBoolResult(CoreAPI::getWindow(outWindow));
            });

        SDOM::CAPI::registerCallable("SDOM_GetRenderer",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* outRenderer = (args.empty() || args[0].kind != CallArg::Kind::Ptr)
                    ? nullptr
                    : static_cast<SDL_Renderer**>(args[0].v.p);
                return makeBoolResult(CoreAPI::getRenderer(outRenderer));
            });

        SDOM::CAPI::registerCallable("SDOM_GetTexture",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* outTexture = (args.empty() || args[0].kind != CallArg::Kind::Ptr)
                    ? nullptr
                    : static_cast<SDL_Texture**>(args[0].v.p);
                return makeBoolResult(CoreAPI::getTexture(outTexture));
            });

        SDOM::CAPI::registerCallable("SDOM_SetWindowWidth",
            [](const std::vector<CallArg>& args) -> CallResult {
                float width = 0.0f;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Double) width = static_cast<float>(args[0].v.d);
                    else if (args[0].kind == CallArg::Kind::UInt) width = static_cast<float>(args[0].v.u);
                    else if (args[0].kind == CallArg::Kind::Int) width = static_cast<float>(args[0].v.i);
                }
                return makeBoolResult(CoreAPI::setWindowWidth(width));
            });

        SDOM::CAPI::registerCallable("SDOM_SetWindowHeight",
            [](const std::vector<CallArg>& args) -> CallResult {
                float height = 0.0f;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Double) height = static_cast<float>(args[0].v.d);
                    else if (args[0].kind == CallArg::Kind::UInt) height = static_cast<float>(args[0].v.u);
                    else if (args[0].kind == CallArg::Kind::Int) height = static_cast<float>(args[0].v.i);
                }
                return makeBoolResult(CoreAPI::setWindowHeight(height));
            });

        SDOM::CAPI::registerCallable("SDOM_SetPixelWidth",
            [](const std::vector<CallArg>& args) -> CallResult {
                float width = 0.0f;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Double) width = static_cast<float>(args[0].v.d);
                    else if (args[0].kind == CallArg::Kind::UInt) width = static_cast<float>(args[0].v.u);
                    else if (args[0].kind == CallArg::Kind::Int) width = static_cast<float>(args[0].v.i);
                }
                return makeBoolResult(CoreAPI::setPixelWidth(width));
            });

        SDOM::CAPI::registerCallable("SDOM_SetPixelHeight",
            [](const std::vector<CallArg>& args) -> CallResult {
                float height = 0.0f;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Double) height = static_cast<float>(args[0].v.d);
                    else if (args[0].kind == CallArg::Kind::UInt) height = static_cast<float>(args[0].v.u);
                    else if (args[0].kind == CallArg::Kind::Int) height = static_cast<float>(args[0].v.i);
                }
                return makeBoolResult(CoreAPI::setPixelHeight(height));
            });

        SDOM::CAPI::registerCallable("SDOM_SetPreserveAspectRatio",
            [](const std::vector<CallArg>& args) -> CallResult {
                bool preserve = false;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Bool) preserve = args[0].v.b;
                    else if (args[0].kind == CallArg::Kind::UInt) preserve = args[0].v.u != 0;
                    else if (args[0].kind == CallArg::Kind::Int) preserve = args[0].v.i != 0;
                }
                return makeBoolResult(CoreAPI::setPreserveAspectRatio(preserve));
            });

        SDOM::CAPI::registerCallable("SDOM_SetAllowTextureResize",
            [](const std::vector<CallArg>& args) -> CallResult {
                bool allow = false;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Bool) allow = args[0].v.b;
                    else if (args[0].kind == CallArg::Kind::UInt) allow = args[0].v.u != 0;
                    else if (args[0].kind == CallArg::Kind::Int) allow = args[0].v.i != 0;
                }
                return makeBoolResult(CoreAPI::setAllowTextureResize(allow));
            });

        SDOM::CAPI::registerCallable("SDOM_SetLogicalPresentation",
            [](const std::vector<CallArg>& args) -> CallResult {
                SDL_RendererLogicalPresentation pres = SDL_LOGICAL_PRESENTATION_DISABLED;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::UInt) pres = static_cast<SDL_RendererLogicalPresentation>(args[0].v.u);
                    else if (args[0].kind == CallArg::Kind::Int) pres = static_cast<SDL_RendererLogicalPresentation>(args[0].v.i);
                }
                return makeBoolResult(CoreAPI::setLogicalPresentation(pres));
            });

        SDOM::CAPI::registerCallable("SDOM_SetWindowFlags",
            [](const std::vector<CallArg>& args) -> CallResult {
                SDL_WindowFlags flags = static_cast<SDL_WindowFlags>(0);
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::UInt) flags = static_cast<SDL_WindowFlags>(args[0].v.u);
                    else if (args[0].kind == CallArg::Kind::Int) flags = static_cast<SDL_WindowFlags>(args[0].v.i);
                }
                return makeBoolResult(CoreAPI::setWindowFlags(flags));
            });

        SDOM::CAPI::registerCallable("SDOM_SetPixelFormat",
            [](const std::vector<CallArg>& args) -> CallResult {
                SDL_PixelFormat fmt = SDL_PIXELFORMAT_UNKNOWN;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::UInt) fmt = static_cast<SDL_PixelFormat>(args[0].v.u);
                    else if (args[0].kind == CallArg::Kind::Int) fmt = static_cast<SDL_PixelFormat>(args[0].v.i);
                }
                return makeBoolResult(CoreAPI::setPixelFormat(fmt));
            });

        SDOM::CAPI::registerCallable("SDOM_SetFullscreen",
            [](const std::vector<CallArg>& args) -> CallResult {
                bool fullscreen = false;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Bool) fullscreen = args[0].v.b;
                    else if (args[0].kind == CallArg::Kind::UInt) fullscreen = args[0].v.u != 0;
                    else if (args[0].kind == CallArg::Kind::Int) fullscreen = args[0].v.i != 0;
                }
                return makeBoolResult(CoreAPI::setFullscreen(fullscreen));
            });

        SDOM::CAPI::registerCallable("SDOM_SetWindowed",
            [](const std::vector<CallArg>& args) -> CallResult {
                bool windowed = false;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Bool) windowed = args[0].v.b;
                    else if (args[0].kind == CallArg::Kind::UInt) windowed = args[0].v.u != 0;
                    else if (args[0].kind == CallArg::Kind::Int) windowed = args[0].v.i != 0;
                }
                return makeBoolResult(CoreAPI::setWindowed(windowed));
            });

        SDOM::CAPI::registerCallable("SDOM_SetIsTraversing",
            [](const std::vector<CallArg>& args) -> CallResult {
                bool traversing = false;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Bool) traversing = args[0].v.b;
                    else if (args[0].kind == CallArg::Kind::UInt) traversing = args[0].v.u != 0;
                    else if (args[0].kind == CallArg::Kind::Int) traversing = args[0].v.i != 0;
                }
                return makeBoolResult(CoreAPI::setIsTraversing(traversing));
            });

        SDOM::CAPI::registerCallable("SDOM_SetKeyboardFocus",
            [](const std::vector<CallArg>& args) -> CallResult {
                const SDOM_DisplayHandle* handle = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    handle = static_cast<const SDOM_DisplayHandle*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::setKeyboardFocus(handle));
            });

        SDOM::CAPI::registerCallable("SDOM_GetKeyboardFocus",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* outHandle = (args.empty() || args[0].kind != CallArg::Kind::Ptr)
                    ? nullptr
                    : static_cast<SDOM_DisplayHandle*>(args[0].v.p);
                return makeBoolResult(CoreAPI::getKeyboardFocus(outHandle));
            });

        SDOM::CAPI::registerCallable("SDOM_ClearKeyboardFocus",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::clearKeyboardFocus());
            });

        SDOM::CAPI::registerCallable("SDOM_SetMouseHover",
            [](const std::vector<CallArg>& args) -> CallResult {
                const SDOM_DisplayHandle* handle = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    handle = static_cast<const SDOM_DisplayHandle*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::setMouseHover(handle));
            });

        SDOM::CAPI::registerCallable("SDOM_GetMouseHover",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* outHandle = (args.empty() || args[0].kind != CallArg::Kind::Ptr)
                    ? nullptr
                    : static_cast<SDOM_DisplayHandle*>(args[0].v.p);
                return makeBoolResult(CoreAPI::getMouseHover(outHandle));
            });

        SDOM::CAPI::registerCallable("SDOM_ClearMouseHover",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::clearMouseHover());
            });

        SDOM::CAPI::registerCallable("SDOM_GetDisplayObject",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* name = nullptr;
                SDOM_DisplayHandle* outHandle = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) name = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) name = static_cast<const char*>(args[0].v.p);
                }
                if (args.size() > 1 && args[1].kind == CallArg::Kind::Ptr) {
                    outHandle = static_cast<SDOM_DisplayHandle*>(args[1].v.p);
                }
                return makeBoolResult(CoreAPI::getDisplayObject(name, outHandle));
            });

        SDOM::CAPI::registerCallable("SDOM_HasDisplayObject",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* name = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) name = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) name = static_cast<const char*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::hasDisplayObject(name));
            });

        SDOM::CAPI::registerCallable("SDOM_DestroyDisplayObjectByName",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* name = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) name = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) name = static_cast<const char*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::destroyDisplayObjectByName(name));
            });

        SDOM::CAPI::registerCallable("SDOM_DestroyDisplayObject",
            [](const std::vector<CallArg>& args) -> CallResult {
                const SDOM_DisplayHandle* handle = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    handle = static_cast<const SDOM_DisplayHandle*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::destroyDisplayObject(handle));
            });

        SDOM::CAPI::registerCallable("SDOM_GetAssetObject",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* name = nullptr;
                SDOM_AssetHandle* outHandle = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) name = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) name = static_cast<const char*>(args[0].v.p);
                }
                if (args.size() > 1 && args[1].kind == CallArg::Kind::Ptr) {
                    outHandle = static_cast<SDOM_AssetHandle*>(args[1].v.p);
                }
                return makeBoolResult(CoreAPI::getAssetObject(name, outHandle));
            });

        SDOM::CAPI::registerCallable("SDOM_HasAssetObject",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* name = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) name = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) name = static_cast<const char*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::hasAssetObject(name));
            });

        SDOM::CAPI::registerCallable("SDOM_DestroyAssetObjectByName",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* name = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) name = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) name = static_cast<const char*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::destroyAssetObjectByName(name));
            });

        SDOM::CAPI::registerCallable("SDOM_DestroyAssetObject",
            [](const std::vector<CallArg>& args) -> CallResult {
                const SDOM_AssetHandle* handle = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    handle = static_cast<const SDOM_AssetHandle*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::destroyAssetObject(handle));
            });

        SDOM::CAPI::registerCallable("SDOM_CreateDisplayObjectFromJson",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* type = nullptr;
                const char* json = nullptr;
                SDOM_DisplayHandle* outHandle = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) type = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) type = static_cast<const char*>(args[0].v.p);
                }
                if (args.size() > 1) {
                    if (args[1].kind == CallArg::Kind::CString) json = args[1].s.c_str();
                    else if (args[1].kind == CallArg::Kind::Ptr) json = static_cast<const char*>(args[1].v.p);
                }
                if (args.size() > 2 && args[2].kind == CallArg::Kind::Ptr) {
                    outHandle = static_cast<SDOM_DisplayHandle*>(args[2].v.p);
                }
                return makeBoolResult(CoreAPI::createDisplayObjectFromJson(type, json, outHandle));
            });

        SDOM::CAPI::registerCallable("SDOM_CreateAssetObjectFromJson",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* type = nullptr;
                const char* json = nullptr;
                SDOM_AssetHandle* outHandle = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) type = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) type = static_cast<const char*>(args[0].v.p);
                }
                if (args.size() > 1) {
                    if (args[1].kind == CallArg::Kind::CString) json = args[1].s.c_str();
                    else if (args[1].kind == CallArg::Kind::Ptr) json = static_cast<const char*>(args[1].v.p);
                }
                if (args.size() > 2 && args[2].kind == CallArg::Kind::Ptr) {
                    outHandle = static_cast<SDOM_AssetHandle*>(args[2].v.p);
                }
                return makeBoolResult(CoreAPI::createAssetObjectFromJson(type, json, outHandle));
            });

        SDOM::CAPI::registerCallable("SDOM_CountOrphans",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromInt(static_cast<std::int64_t>(CoreAPI::countOrphanedDisplayObjects()));
            });

        SDOM::CAPI::registerCallable("SDOM_GetOrphans",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* outHandles = (args.size() > 0 && args[0].kind == CallArg::Kind::Ptr)
                    ? static_cast<SDOM_DisplayHandle*>(args[0].v.p)
                    : nullptr;
                int* outCount = nullptr;
                if (args.size() > 1 && args[1].kind == CallArg::Kind::Ptr) {
                    outCount = static_cast<int*>(args[1].v.p);
                }
                int maxCount = 0;
                if (args.size() > 2) {
                    if (args[2].kind == CallArg::Kind::Int) maxCount = static_cast<int>(args[2].v.i);
                    else if (args[2].kind == CallArg::Kind::UInt) maxCount = static_cast<int>(args[2].v.u);
                }
                return makeBoolResult(CoreAPI::getOrphanedDisplayObjects(outHandles, outCount, maxCount));
            });

        SDOM::CAPI::registerCallable("SDOM_DetachOrphans",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::detachOrphans());
            });

        SDOM::CAPI::registerCallable("SDOM_CollectGarbage",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::collectGarbage());
            });

        SDOM::CAPI::registerCallable("SDOM_AttachFutureChildren",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::attachFutureChildren());
            });

        SDOM::CAPI::registerCallable("SDOM_AddOrphan",
            [](const std::vector<CallArg>& args) -> CallResult {
                const SDOM_DisplayHandle* orphan = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    orphan = static_cast<const SDOM_DisplayHandle*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::addToOrphanList(orphan));
            });

        SDOM::CAPI::registerCallable("SDOM_AddFutureChild",
            [](const std::vector<CallArg>& args) -> CallResult {
                const SDOM_DisplayHandle* child = nullptr;
                const SDOM_DisplayHandle* parent = nullptr;
                bool useWorld = false;
                int worldX = 0;
                int worldY = 0;

                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    child = static_cast<const SDOM_DisplayHandle*>(args[0].v.p);
                }
                if (args.size() > 1 && args[1].kind == CallArg::Kind::Ptr) {
                    parent = static_cast<const SDOM_DisplayHandle*>(args[1].v.p);
                }
                if (args.size() > 2) {
                    if (args[2].kind == CallArg::Kind::Bool) useWorld = args[2].v.b;
                    else if (args[2].kind == CallArg::Kind::UInt) useWorld = args[2].v.u != 0;
                    else if (args[2].kind == CallArg::Kind::Int) useWorld = args[2].v.i != 0;
                }
                if (args.size() > 3) {
                    if (args[3].kind == CallArg::Kind::Int) worldX = static_cast<int>(args[3].v.i);
                    else if (args[3].kind == CallArg::Kind::UInt) worldX = static_cast<int>(args[3].v.u);
                }
                if (args.size() > 4) {
                    if (args[4].kind == CallArg::Kind::Int) worldY = static_cast<int>(args[4].v.i);
                    else if (args[4].kind == CallArg::Kind::UInt) worldY = static_cast<int>(args[4].v.u);
                }

                return makeBoolResult(CoreAPI::addToFutureChildrenList(child, parent, useWorld, worldX, worldY));
            });

        SDOM::CAPI::registerCallable("SDOM_GetDisplayObjectNames",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* outNames = (args.size() > 0 && args[0].kind == CallArg::Kind::Ptr)
                    ? static_cast<const char**>(args[0].v.p)
                    : nullptr;
                int* outCount = nullptr;
                if (args.size() > 1 && args[1].kind == CallArg::Kind::Ptr) {
                    outCount = static_cast<int*>(args[1].v.p);
                }
                int maxCount = 0;
                if (args.size() > 2) {
                    if (args[2].kind == CallArg::Kind::Int) maxCount = static_cast<int>(args[2].v.i);
                    else if (args[2].kind == CallArg::Kind::UInt) maxCount = static_cast<int>(args[2].v.u);
                }
                return makeBoolResult(CoreAPI::getDisplayObjectNames(outNames, outCount, maxCount));
            });

        SDOM::CAPI::registerCallable("SDOM_ClearFactory",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::clearFactory());
            });

        SDOM::CAPI::registerCallable("SDOM_PrintObjectRegistry",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::printObjectRegistry());
            });

        SDOM::CAPI::registerCallable("SDOM_GetPropertyNamesForType",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* type = nullptr;
                const char** outNames = nullptr;
                int* outCount = nullptr;
                int maxCount = 0;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) type = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) type = static_cast<const char*>(args[0].v.p);
                }
                if (args.size() > 1 && args[1].kind == CallArg::Kind::Ptr) {
                    outNames = static_cast<const char**>(args[1].v.p);
                }
                if (args.size() > 2 && args[2].kind == CallArg::Kind::Ptr) {
                    outCount = static_cast<int*>(args[2].v.p);
                }
                if (args.size() > 3) {
                    if (args[3].kind == CallArg::Kind::Int) maxCount = static_cast<int>(args[3].v.i);
                    else if (args[3].kind == CallArg::Kind::UInt) maxCount = static_cast<int>(args[3].v.u);
                }
                return makeBoolResult(CoreAPI::getPropertyNamesForType(type, outNames, outCount, maxCount));
            });

        SDOM::CAPI::registerCallable("SDOM_GetCommandNamesForType",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* type = nullptr;
                const char** outNames = nullptr;
                int* outCount = nullptr;
                int maxCount = 0;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) type = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) type = static_cast<const char*>(args[0].v.p);
                }
                if (args.size() > 1 && args[1].kind == CallArg::Kind::Ptr) {
                    outNames = static_cast<const char**>(args[1].v.p);
                }
                if (args.size() > 2 && args[2].kind == CallArg::Kind::Ptr) {
                    outCount = static_cast<int*>(args[2].v.p);
                }
                if (args.size() > 3) {
                    if (args[3].kind == CallArg::Kind::Int) maxCount = static_cast<int>(args[3].v.i);
                    else if (args[3].kind == CallArg::Kind::UInt) maxCount = static_cast<int>(args[3].v.u);
                }
                return makeBoolResult(CoreAPI::getCommandNamesForType(type, outNames, outCount, maxCount));
            });

        SDOM::CAPI::registerCallable("SDOM_GetFunctionNamesForType",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* type = nullptr;
                const char** outNames = nullptr;
                int* outCount = nullptr;
                int maxCount = 0;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) type = args[0].s.c_str();
                    else if (args[0].kind == CallArg::Kind::Ptr) type = static_cast<const char*>(args[0].v.p);
                }
                if (args.size() > 1 && args[1].kind == CallArg::Kind::Ptr) {
                    outNames = static_cast<const char**>(args[1].v.p);
                }
                if (args.size() > 2 && args[2].kind == CallArg::Kind::Ptr) {
                    outCount = static_cast<int*>(args[2].v.p);
                }
                if (args.size() > 3) {
                    if (args[3].kind == CallArg::Kind::Int) maxCount = static_cast<int>(args[3].v.i);
                    else if (args[3].kind == CallArg::Kind::UInt) maxCount = static_cast<int>(args[3].v.u);
                }
                return makeBoolResult(CoreAPI::getFunctionNamesForType(type, outNames, outCount, maxCount));
            });

        SDOM::CAPI::registerCallable("SDOM_SetIgnoreRealInput",
            [](const std::vector<CallArg>& args) -> CallResult {
                bool ignore = false;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Bool) ignore = args[0].v.b;
                    else if (args[0].kind == CallArg::Kind::UInt) ignore = args[0].v.u != 0;
                    else if (args[0].kind == CallArg::Kind::Int) ignore = args[0].v.i != 0;
                }
                return makeBoolResult(CoreAPI::setIgnoreRealInput(ignore));
            });

        SDOM::CAPI::registerCallable("SDOM_GetIgnoreRealInput",
            [](const std::vector<CallArg>& args) -> CallResult {
                bool* outIgnore = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    outIgnore = static_cast<bool*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::getIgnoreRealInput(outIgnore));
            });

        SDOM::CAPI::registerCallable("SDOM_GetKeyfocusGray",
            [](const std::vector<CallArg>& args) -> CallResult {
                float* outGray = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    outGray = static_cast<float*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::getKeyfocusGray(outGray));
            });

        SDOM::CAPI::registerCallable("SDOM_SetKeyfocusGray",
            [](const std::vector<CallArg>& args) -> CallResult {
                float gray = 0.0f;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Double) gray = static_cast<float>(args[0].v.d);
                    else if (args[0].kind == CallArg::Kind::UInt) gray = static_cast<float>(args[0].v.u);
                    else if (args[0].kind == CallArg::Kind::Int) gray = static_cast<float>(args[0].v.i);
                }
                return makeBoolResult(CoreAPI::setKeyfocusGray(gray));
            });

        SDOM::CAPI::registerCallable("SDOM_GetFrameCount",
            [](const std::vector<CallArg>&) -> CallResult {
                return SDOM::CAPI::CallResult::FromInt(static_cast<std::int64_t>(CoreAPI::getFrameCount()));
            });

        SDOM::CAPI::registerCallable("SDOM_SetRootNode",
            [](const std::vector<CallArg>& args) -> CallResult {
                const SDOM_DisplayHandle* handle = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    handle = static_cast<const SDOM_DisplayHandle*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::setRootNode(handle));
            });

        SDOM::CAPI::registerCallable("SDOM_SetRootNodeByName",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* name = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        name = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        name = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::setRootNodeByName(name));
            });

        SDOM::CAPI::registerCallable("SDOM_SetStage",
            [](const std::vector<CallArg>& args) -> CallResult {
                const SDOM_DisplayHandle* handle = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    handle = static_cast<const SDOM_DisplayHandle*>(args[0].v.p);
                }
                return makeBoolResult(CoreAPI::setStage(handle));
            });

        SDOM::CAPI::registerCallable("SDOM_SetStageByName",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* name = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        name = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        name = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::setStageByName(name));
            });

        SDOM::CAPI::registerCallable("SDOM_GetRootNode",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* outHandle = (args.empty() || args[0].kind != CallArg::Kind::Ptr)
                    ? nullptr
                    : static_cast<SDOM_DisplayHandle*>(args[0].v.p);
                return makeBoolResult(CoreAPI::getRootNode(outHandle));
            });

        SDOM::CAPI::registerCallable("SDOM_GetStageHandle",
            [](const std::vector<CallArg>& args) -> CallResult {
                auto* outHandle = (args.empty() || args[0].kind != CallArg::Kind::Ptr)
                    ? nullptr
                    : static_cast<SDOM_DisplayHandle*>(args[0].v.p);
                return makeBoolResult(CoreAPI::getStageHandle(outHandle));
            });

        SDOM::CAPI::registerCallable("SDOM_SetStopAfterUnitTests",
            [](const std::vector<CallArg>& args) -> CallResult {
                bool stop = false;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Bool) {
                        stop = args[0].v.b;
                    } else if (args[0].kind == CallArg::Kind::UInt) {
                        stop = (args[0].v.u != 0);
                    }
                }
                return makeBoolResult(CoreAPI::setStopAfterUnitTests(stop));
            });

        SDOM::CAPI::registerCallable("SDOM_GetStopAfterUnitTests",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::getStopAfterUnitTests());
            });

        SDOM::CAPI::registerCallable("SDOM_SetIsRunning",
            [](const std::vector<CallArg>& args) -> CallResult {
                bool running = false;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::Bool) {
                        running = args[0].v.b;
                    } else if (args[0].kind == CallArg::Kind::UInt) {
                        running = (args[0].v.u != 0);
                    }
                }
                return makeBoolResult(CoreAPI::setIsRunning(running));
            });

        SDOM::CAPI::registerCallable("SDOM_LoadDomFromJsonFile",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* filename = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        filename = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        filename = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::loadDomFromJsonFile(filename));
            });

        SDOM::CAPI::registerCallable("SDOM_ConfigureFromJson",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* jsonStr = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        jsonStr = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        jsonStr = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::configureFromJson(jsonStr));
            });

        SDOM::CAPI::registerCallable("SDOM_LoadDomFromJson",
            [](const std::vector<CallArg>& args) -> CallResult {
                const char* jsonStr = nullptr;
                if (!args.empty()) {
                    if (args[0].kind == CallArg::Kind::CString) {
                        jsonStr = args[0].s.c_str();
                    } else if (args[0].kind == CallArg::Kind::Ptr) {
                        jsonStr = static_cast<const char*>(args[0].v.p);
                    }
                }
                return makeBoolResult(CoreAPI::loadDomFromJson(jsonStr));
            });

        // Callback registration callables: accept raw function pointers (passed as Ptr)
        SDOM::CAPI::registerCallable("SDOM_RegisterOnInit",
            [](const std::vector<CallArg>& args) -> CallResult {
                void* fnptr = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    fnptr = args[0].v.p;
                }
                return makeBoolResult(CoreAPI::registerOnInit(fnptr));
            });

        SDOM::CAPI::registerCallable("SDOM_RegisterOnQuit",
            [](const std::vector<CallArg>& args) -> CallResult {
                void* fnptr = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    fnptr = args[0].v.p;
                }
                return makeBoolResult(CoreAPI::registerOnQuit(fnptr));
            });

        SDOM::CAPI::registerCallable("SDOM_RegisterOnUpdate",
            [](const std::vector<CallArg>& args) -> CallResult {
                void* fnptr = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    fnptr = args[0].v.p;
                }
                return makeBoolResult(CoreAPI::registerOnUpdate(fnptr));
            });

        SDOM::CAPI::registerCallable("SDOM_RegisterOnEvent",
            [](const std::vector<CallArg>& args) -> CallResult {
                void* fnptr = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    fnptr = args[0].v.p;
                }
                return makeBoolResult(CoreAPI::registerOnEvent(fnptr));
            });

        SDOM::CAPI::registerCallable("SDOM_RegisterOnRender",
            [](const std::vector<CallArg>& args) -> CallResult {
                void* fnptr = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    fnptr = args[0].v.p;
                }
                return makeBoolResult(CoreAPI::registerOnRender(fnptr));
            });

        SDOM::CAPI::registerCallable("SDOM_RegisterOnUnitTest",
            [](const std::vector<CallArg>& args) -> CallResult {
                void* fnptr = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    fnptr = args[0].v.p;
                }
                return makeBoolResult(CoreAPI::registerOnUnitTest(fnptr));
            });

        SDOM::CAPI::registerCallable("SDOM_RegisterOnWindowResize",
            [](const std::vector<CallArg>& args) -> CallResult {
                void* fnptr = nullptr;
                if (!args.empty() && args[0].kind == CallArg::Kind::Ptr) {
                    fnptr = args[0].v.p;
                }
                return makeBoolResult(CoreAPI::registerOnWindowResize(fnptr));
            });

        SDOM::CAPI::registerCallable("SDOM_Run",
            [](const std::vector<CallArg>&) -> CallResult {
                return makeBoolResult(CoreAPI::run());
            });

        SDOM::CAPI::registerCallable("SDOM_Quit",
            [](const std::vector<CallArg>&) -> CallResult {
                CoreAPI::quit();
                return CallResult::Void();
            });
    }
};

static BootstrapDispatchRegistrations g_coreApiDispatchBootstrap;

} // namespace

const char* getErrorCString()
{
    std::lock_guard<std::mutex> lock(errorMutex());
    if (!lastErrorStorage().empty()) {
        return lastErrorStorage().c_str();
    }
    return SDL_GetError();
}

std::string getErrorString()
{
    std::lock_guard<std::mutex> lock(errorMutex());
    if (!lastErrorStorage().empty()) {
        return lastErrorStorage();
    }
    const char* fallback = SDL_GetError();
    return fallback ? std::string(fallback) : std::string();
}

bool setErrorMessage(const char* message)
{
    const char* resolved = message ? message : "Unknown SDOM error";
    {
        std::lock_guard<std::mutex> lock(errorMutex());
        lastErrorStorage() = resolved;
    }
    SDL_SetError("%s", resolved);
    return true;
}

bool init(uint64_t /*flags*/)
{
    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        if (core.getWindow() || core.getRenderer()) {
            return true;
        }

        SDOM_CoreConfig defaults = SDOM_CORECONFIG_DEFAULT;
        SDOM::Core::CoreConfig cc = toCoreConfig(&defaults);
        core.configure(cc);
        core.resetFramePhaseState();
        core.resetManualFrameTimer();
        return true;
    } catch (const SDOM::Exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_Init unknown error");
        return false;
    }
}

bool configure(const SDOM_CoreConfig* cfg)
{
    if (!cfg) {
        setErrorMessage("SDOM_Configure: cfg is null");
        return false;
    }

    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        SDOM::Core::CoreConfig cc = toCoreConfig(cfg);
        core.configure(cc);
        return true;
    } catch (const SDOM::Exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_Configure unknown error");
        return false;
    }
}

bool getCoreConfig(SDOM_CoreConfig* out_cfg)
{
    if (!out_cfg) {
        setErrorMessage("SDOM_GetCoreConfig: out_cfg is null");
        return false;
    }

    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        SDOM::Core::CoreConfig cc = core.getConfig();
        copyCoreConfig(cc, *out_cfg);
        return true;
    } catch (const SDOM::Exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetCoreConfig unknown error");
        return false;
    }
}

bool setStopAfterUnitTests(bool stop)
{
    try {
        SDOM::Core::getInstance().setStopAfterUnitTests(stop);
        return true;
    } catch (const SDOM::Exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetStopAfterUnitTests unknown error");
        return false;
    }
}

bool getStopAfterUnitTests()
{
    try {
        return SDOM::Core::getInstance().getStopAfterUnitTests();
    } catch (const SDOM::Exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetStopAfterUnitTests unknown error");
        return false;
    }
}

bool setIsRunning(bool running)
{
    try {
        SDOM::Core::getInstance().isRunning(running);
        return true;
    } catch (const SDOM::Exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetIsRunning unknown error");
        return false;
    }
}

bool configureFromJson(const char* json)
{
    if (!json) {
        setErrorMessage("SDOM_ConfigureFromJson: json is null");
        return false;
    }
    try {
        nlohmann::json doc = nlohmann::json::parse(json);
        return SDOM::Core::getInstance().configureFromJson(doc);
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_ConfigureFromJson unknown error");
        return false;
    }
}

bool loadDomFromJson(const char* json)
{
    if (!json) {
        setErrorMessage("SDOM_LoadDomFromJson: json is null");
        return false;
    }
    try {
        nlohmann::json doc = nlohmann::json::parse(json);
        return SDOM::Core::getInstance().loadProjectFromJson(doc);
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_LoadDomFromJson unknown error");
        return false;
    }
}

bool loadDomFromJsonFile(const char* filename)
{
    if (!filename) {
        setErrorMessage("SDOM_LoadDomFromJsonFile: filename is null");
        return false;
    }

    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        if (core.loadProjectFromJsonFile(filename)) {
            return true;
        }
        setErrorMessage("SDOM_LoadDomFromJsonFile: Core::loadProjectFromJsonFile returned false");
        return false;
    } catch (const SDOM::Exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_LoadDomFromJsonFile unknown error");
        return false;
    }
}

bool getBorderColor(SDL_Color* out_color)
{
    if (!out_color) {
        setErrorMessage("SDOM_GetBorderColor: out_color is null");
        return false;
    }

    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        *out_color = core.getColor();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetBorderColor unknown error");
        return false;
    }
}

bool setBorderColor(const SDL_Color* color)
{
    if (!color) {
        setErrorMessage("SDOM_SetBorderColor: color is null");
        return false;
    }
    try {
        SDOM::Core::getInstance().setColor(*color);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetBorderColor unknown error");
        return false;
    }
}

bool setRootNode(const SDOM_DisplayHandle* handle)
{
    if (!handle) {
        setErrorMessage("SDOM_SetRootNode: handle is null");
        return false;
    }

    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        DisplayHandle resolved = resolveDisplayHandle(handle);
        if (!resolved.isValid()) {
            setErrorMessage("SDOM_SetRootNode: handle is invalid");
            return false;
        }
        if (!dynamic_cast<SDOM::Stage*>(resolved.get())) {
            setErrorMessage("SDOM_SetRootNode: handle is not a Stage");
            return false;
        }
        core.setRootNode(resolved);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetRootNode unknown error");
        return false;
    }
}

bool setRootNodeByName(const char* name)
{
    if (!name) {
        setErrorMessage("SDOM_SetRootNodeByName: name is null");
        return false;
    }

    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        SDOM::Factory& factory = core.getFactory();
        DisplayHandle handle = factory.getDisplayObject(name);
        if (!handle.isValid()) {
            setErrorMessage("SDOM_SetRootNodeByName: stage not found");
            return false;
        }
        if (!dynamic_cast<SDOM::Stage*>(handle.get())) {
            setErrorMessage("SDOM_SetRootNodeByName: handle is not a Stage");
            return false;
        }
        core.setRootNode(handle);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetRootNodeByName unknown error");
        return false;
    }
}

bool setStage(const SDOM_DisplayHandle* handle)
{
    try {
        return setRootNode(handle);
    } catch (...) {
        setErrorMessage("SDOM_SetStage unknown error");
        return false;
    }
}

bool setStageByName(const char* name)
{
    try {
        return setRootNodeByName(name);
    } catch (...) {
        setErrorMessage("SDOM_SetStageByName unknown error");
        return false;
    }
}

bool getRootNode(SDOM_DisplayHandle* out_handle)
{
    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        DisplayHandle root = core.getRootNode();
        return writeDisplayHandleOut(root, out_handle);
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetRootNode unknown error");
        return false;
    }
}

bool getStageHandle(SDOM_DisplayHandle* out_handle)
{
    try {
        return getRootNode(out_handle);
    } catch (...) {
        setErrorMessage("SDOM_GetStageHandle unknown error");
        return false;
    }
}

bool run()
{
    try {
        return SDOM::Core::getInstance().run();
    } catch (const SDOM::Exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_Run unknown error");
        return false;
    }
}

void quit()
{
    try {
        SDOM::Core::getInstance().onQuit();
    } catch (...) {
        // Best-effort cleanup
    }
}

bool pumpEventsOnce()
{
    try {
        SDOM::Core::getInstance().pumpEventsOnce();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_PumpEventsOnce unknown error");
        return false;
    }
}

bool pushMouseEvent(const char* json)
{
    SDL_Event ev;
    if (!parseSyntheticMouseEventJson(json, ev)) {
        return false;
    }

    try {
        SDOM::Core::getInstance().getEventManager().Queue_SDL_Event(ev);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_PushMouseEvent dispatch error");
        return false;
    }
}

bool pushKeyboardEvent(const char* json)
{
    SDL_Event ev;
    if (!parseSyntheticKeyboardEventJson(json, ev)) {
        return false;
    }

    try {
        SDOM::Core::getInstance().getEventManager().Queue_SDL_Event(ev);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_PushKeyboardEvent dispatch error");
        return false;
    }
}

const char* getWindowTitle()
{
    try {
        static thread_local std::string s_title;
        s_title = SDOM::Core::getInstance().getWindowTitle();
        return s_title.c_str();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return nullptr;
    } catch (...) {
        setErrorMessage("SDOM_GetWindowTitle unknown error");
        return nullptr;
    }
}

bool setWindowTitle(const char* title)
{
    if (!title) {
        setErrorMessage("SDOM_SetWindowTitle: title is null");
        return false;
    }

    try {
        SDOM::Core::getInstance().setWindowTitle(title);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetWindowTitle unknown error");
        return false;
    }
}

bool pollEvents(SDOM_Event* evt)
{
    if (!evt) {
        setErrorMessage("SDOM_PollEvents: evt is null");
        return false;
    }

    writeEventHandleOut(nullptr, evt);

    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        SDOM::Event snapshot;
        auto outcome = core.pollEventsPhase(&snapshot, false);
        if (!outcome.errorMessage.empty())
            setErrorMessage(outcome.errorMessage.c_str());

        if (outcome.fatalError)
            return false;

        if (outcome.phaseCompleted)
            writeEventHandleOut(&snapshot, evt);

        return outcome.phaseCompleted;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_PollEvents unknown error");
        return false;
    }
}

bool updatePhase()
{
    try {
        auto outcome = SDOM::Core::getInstance().updatePhase(false);
        if (!outcome.errorMessage.empty())
            setErrorMessage(outcome.errorMessage.c_str());
        if (outcome.fatalError)
            return false;
        return outcome.phaseCompleted && !outcome.autoCorrected;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_Update unknown error");
        return false;
    }
}

bool renderPhase()
{
    try {
        auto outcome = SDOM::Core::getInstance().renderPhase(false);
        if (!outcome.errorMessage.empty())
            setErrorMessage(outcome.errorMessage.c_str());
        if (outcome.fatalError)
            return false;
        return outcome.phaseCompleted && !outcome.autoCorrected;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_Render unknown error");
        return false;
    }
}

bool presentPhase()
{
    try {
        auto outcome = SDOM::Core::getInstance().presentPhase(false);
        if (!outcome.errorMessage.empty())
            setErrorMessage(outcome.errorMessage.c_str());
        if (outcome.fatalError)
            return false;
        return outcome.phaseCompleted;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_Present unknown error");
        return false;
    }
}

bool runFramePhase()
{
    try {
        auto outcome = SDOM::Core::getInstance().runFramePhase();
        if (!outcome.errorMessage.empty())
            setErrorMessage(outcome.errorMessage.c_str());
        if (outcome.fatalError)
            return false;
        return outcome.phaseCompleted;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_RunFrame unknown error");
        return false;
    }
}

bool hasLuaSupport()
{
#ifdef SDOM_ENABLE_LUA_BINDINGS
    return true;
#else
    return false;
#endif
}

const char* getVersionString()
{
    try {
        static thread_local std::string s_value;
        s_value = SDOM::Core::getInstance().getVersionString();
        return s_value.c_str();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return nullptr;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionString unknown error");
        return nullptr;
    }
}

const char* getVersionFullString()
{
    try {
        static thread_local std::string s_value;
        s_value = SDOM::Core::getInstance().getVersionFullString();
        return s_value.c_str();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return nullptr;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionFullString unknown error");
        return nullptr;
    }
}

int getVersionMajor()
{
    try {
        return SDOM::Core::getInstance().getVersionMajor();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return 0;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionMajor unknown error");
        return 0;
    }
}

int getVersionMinor()
{
    try {
        return SDOM::Core::getInstance().getVersionMinor();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return 0;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionMinor unknown error");
        return 0;
    }
}

int getVersionPatch()
{
    try {
        return SDOM::Core::getInstance().getVersionPatch();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return 0;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionPatch unknown error");
        return 0;
    }
}

const char* getVersionCodename()
{
    try {
        static thread_local std::string s_value;
        s_value = SDOM::Core::getInstance().getVersionCodename();
        return s_value.c_str();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return nullptr;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionCodename unknown error");
        return nullptr;
    }
}

const char* getVersionBuild()
{
    try {
        static thread_local std::string s_value;
        s_value = SDOM::Core::getInstance().getVersionBuild();
        return s_value.c_str();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return nullptr;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionBuild unknown error");
        return nullptr;
    }
}

const char* getVersionBuildDate()
{
    try {
        static thread_local std::string s_value;
        s_value = SDOM::Core::getInstance().getVersionBuildDate();
        return s_value.c_str();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return nullptr;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionBuildDate unknown error");
        return nullptr;
    }
}

const char* getVersionCommit()
{
    try {
        static thread_local std::string s_value;
        s_value = SDOM::Core::getInstance().getVersionCommit();
        return s_value.c_str();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return nullptr;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionCommit unknown error");
        return nullptr;
    }
}

const char* getVersionBranch()
{
    try {
        static thread_local std::string s_value;
        s_value = SDOM::Core::getInstance().getVersionBranch();
        return s_value.c_str();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return nullptr;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionBranch unknown error");
        return nullptr;
    }
}

const char* getVersionCompiler()
{
    try {
        static thread_local std::string s_value;
        s_value = SDOM::Core::getInstance().getVersionCompiler();
        return s_value.c_str();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return nullptr;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionCompiler unknown error");
        return nullptr;
    }
}

const char* getVersionPlatform()
{
    try {
        static thread_local std::string s_value;
        s_value = SDOM::Core::getInstance().getVersionPlatform();
        return s_value.c_str();
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return nullptr;
    } catch (...) {
        setErrorMessage("SDOM_GetVersionPlatform unknown error");
        return nullptr;
    }
}

float getElapsedTime()
{
    try {
        return SDOM::Core::getInstance().getElapsedTime();
    } catch (...) {
        return 0.0f;
    }
}

float getWindowWidth()
{
    try {
        return SDOM::Core::getInstance().getWindowWidth();
    } catch (...) {
        return 0.0f;
    }
}

float getWindowHeight()
{
    try {
        return SDOM::Core::getInstance().getWindowHeight();
    } catch (...) {
        return 0.0f;
    }
}

float getPixelWidth()
{
    try {
        return SDOM::Core::getInstance().getPixelWidth();
    } catch (...) {
        return 0.0f;
    }
}

float getPixelHeight()
{
    try {
        return SDOM::Core::getInstance().getPixelHeight();
    } catch (...) {
        return 0.0f;
    }
}

bool getPreserveAspectRatio()
{
    try {
        return SDOM::Core::getInstance().getPreserveAspectRatio();
    } catch (...) {
        return false;
    }
}

bool getAllowTextureResize()
{
    try {
        return SDOM::Core::getInstance().getAllowTextureResize();
    } catch (...) {
        return false;
    }
}

SDL_RendererLogicalPresentation getLogicalPresentation()
{
    try {
        return SDOM::Core::getInstance().getRendererLogicalPresentation();
    } catch (...) {
        return SDL_LOGICAL_PRESENTATION_DISABLED;
    }
}

SDL_WindowFlags getWindowFlags()
{
    try {
        return SDOM::Core::getInstance().getWindowFlags();
    } catch (...) {
        return static_cast<SDL_WindowFlags>(0);
    }
}

SDL_PixelFormat getPixelFormat()
{
    try {
        return SDOM::Core::getInstance().getPixelFormat();
    } catch (...) {
        return SDL_PIXELFORMAT_UNKNOWN;
    }
}

bool isFullscreen()
{
    try {
        return SDOM::Core::getInstance().isFullscreen();
    } catch (...) {
        return false;
    }
}

bool isWindowed()
{
    try {
        return SDOM::Core::getInstance().isWindowed();
    } catch (...) {
        return false;
    }
}

bool getIsTraversing()
{
    try {
        return SDOM::Core::getInstance().getIsTraversing();
    } catch (...) {
        return false;
    }
}

bool handleTabKeyPress()
{
    try {
        SDOM::Core::getInstance().handleTabKeyPress();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_HandleTabKeyPress unknown error");
        return false;
    }
}

bool handleTabKeyPressReverse()
{
    try {
        SDOM::Core::getInstance().handleTabKeyPressReverse();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_HandleTabKeyPressReverse unknown error");
        return false;
    }
}

bool setKeyboardFocus(const SDOM_DisplayHandle* handle)
{
    if (!handle) {
        setErrorMessage("SDOM_SetKeyboardFocus: handle is null");
        return false;
    }

    try {
        DisplayHandle resolved = resolveDisplayHandle(handle);
        if (!resolved.isValid()) {
            setErrorMessage("SDOM_SetKeyboardFocus: handle is invalid");
            return false;
        }
        SDOM::Core::getInstance().setKeyboardFocusedObject(resolved);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetKeyboardFocus unknown error");
        return false;
    }
}

bool getKeyboardFocus(SDOM_DisplayHandle* out_handle)
{
    if (!out_handle) {
        setErrorMessage("SDOM_GetKeyboardFocus: out_handle is null");
        return false;
    }

    try {
        DisplayHandle focused = SDOM::Core::getInstance().getKeyboardFocusedObject();
        if (!focused.isValid() || !focused.get()) {
            out_handle->object_id = 0;
            out_handle->name = nullptr;
            out_handle->type = nullptr;
            setErrorMessage("SDOM_GetKeyboardFocus: focus not set");
            return false;
        }
        static thread_local std::string s_name;
        static thread_local std::string s_type;
        s_name = focused.getName();
        s_type = focused.getType();
        out_handle->object_id = 0;
        out_handle->name = s_name.c_str();
        out_handle->type = s_type.c_str();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetKeyboardFocus unknown error");
        return false;
    }
}

bool clearKeyboardFocus()
{
    try {
        SDOM::Core::getInstance().clearKeyboardFocusedObject();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_ClearKeyboardFocus unknown error");
        return false;
    }
}

bool setMouseHover(const SDOM_DisplayHandle* handle)
{
    if (!handle) {
        setErrorMessage("SDOM_SetMouseHover: handle is null");
        return false;
    }

    try {
        DisplayHandle resolved = resolveDisplayHandle(handle);
        if (!resolved.isValid()) {
            setErrorMessage("SDOM_SetMouseHover: handle is invalid");
            return false;
        }
        SDOM::Core::getInstance().setMouseHoveredObject(resolved);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetMouseHover unknown error");
        return false;
    }
}

bool getMouseHover(SDOM_DisplayHandle* out_handle)
{
    if (!out_handle) {
        setErrorMessage("SDOM_GetMouseHover: out_handle is null");
        return false;
    }

    try {
        DisplayHandle hovered = SDOM::Core::getInstance().getMouseHoveredObject();
        if (!hovered.isValid() || !hovered.get()) {
            out_handle->object_id = 0;
            out_handle->name = nullptr;
            out_handle->type = nullptr;
            setErrorMessage("SDOM_GetMouseHover: hover not set");
            return false;
        }
        static thread_local std::string s_name;
        static thread_local std::string s_type;
        s_name = hovered.getName();
        s_type = hovered.getType();
        out_handle->object_id = 0;
        out_handle->name = s_name.c_str();
        out_handle->type = s_type.c_str();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetMouseHover unknown error");
        return false;
    }
}

bool clearMouseHover()
{
    try {
        SDOM::Core::getInstance().clearMouseHoveredObject();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_ClearMouseHover unknown error");
        return false;
    }
}

int getFrameCount()
{
    try {
        return SDOM::Core::getInstance().getFrameCount();
    } catch (...) {
        return 0;
    }
}

bool createDisplayObjectFromJson(const char* type, const char* json, SDOM_DisplayHandle* out_handle)
{
    if (!type) {
        setErrorMessage("SDOM_CreateDisplayObjectFromJson: type is null");
        return false;
    }
    if (!json) {
        setErrorMessage("SDOM_CreateDisplayObjectFromJson: json is null");
        return false;
    }
    if (!out_handle) {
        setErrorMessage("SDOM_CreateDisplayObjectFromJson: out_handle is null");
        return false;
    }

    try {
        nlohmann::json doc = nlohmann::json::parse(json);
        SDOM::Factory& factory = SDOM::Core::getInstance().getFactory();
        DisplayHandle handle = factory.createDisplayObjectFromJson(type, doc);
        if (!handle.isValid() || !handle.get()) {
            setErrorMessage("SDOM_CreateDisplayObjectFromJson: creation failed");
            return false;
        }
        return writeDisplayHandleOut(handle, out_handle);
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_CreateDisplayObjectFromJson unknown error");
        return false;
    }
}

bool createAssetObjectFromJson(const char* type, const char* json, SDOM_AssetHandle* out_handle)
{
    if (!type) {
        setErrorMessage("SDOM_CreateAssetObjectFromJson: type is null");
        return false;
    }
    if (!json) {
        setErrorMessage("SDOM_CreateAssetObjectFromJson: json is null");
        return false;
    }
    if (!out_handle) {
        setErrorMessage("SDOM_CreateAssetObjectFromJson: out_handle is null");
        return false;
    }

    try {
        nlohmann::json doc = nlohmann::json::parse(json);
        SDOM::Factory& factory = SDOM::Core::getInstance().getFactory();
        SDOM::AssetHandle handle = factory.createAssetObjectFromJson(type, doc);
        if (!handle.isValid() || !handle.get()) {
            setErrorMessage("SDOM_CreateAssetObjectFromJson: creation failed");
            return false;
        }
        return writeAssetHandleOut(handle, out_handle, "SDOM_CreateAssetObjectFromJson: handle is not set");
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_CreateAssetObjectFromJson unknown error");
        return false;
    }
}

bool getDisplayObject(const char* name, SDOM_DisplayHandle* out_handle)
{
    if (!name) {
        setErrorMessage("SDOM_GetDisplayObject: name is null");
        return false;
    }
    if (!out_handle) {
        setErrorMessage("SDOM_GetDisplayObject: out_handle is null");
        return false;
    }

    try {
        DisplayHandle handle = SDOM::Core::getInstance().getDisplayObject(name);
        if (!handle.isValid() || !handle.get()) {
            setErrorMessage("SDOM_GetDisplayObject: not found");
            out_handle->object_id = 0;
            out_handle->name = nullptr;
            out_handle->type = nullptr;
            return false;
        }
        return writeDisplayHandleOut(handle, out_handle);
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetDisplayObject unknown error");
        return false;
    }
}

bool hasDisplayObject(const char* name)
{
    if (!name) {
        setErrorMessage("SDOM_HasDisplayObject: name is null");
        return false;
    }
    try {
        return SDOM::Core::getInstance().hasDisplayObject(name);
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_HasDisplayObject unknown error");
        return false;
    }
}

bool destroyDisplayObjectByName(const char* name)
{
    if (!name) {
        setErrorMessage("SDOM_DestroyDisplayObjectByName: name is null");
        return false;
    }
    try {
        SDOM::Core::getInstance().destroyDisplayObject(name);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_DestroyDisplayObjectByName unknown error");
        return false;
    }
}

bool destroyDisplayObject(const SDOM_DisplayHandle* handle)
{
    if (!handle) {
        setErrorMessage("SDOM_DestroyDisplayObject: handle is null");
        return false;
    }
    try {
        DisplayHandle resolved = resolveDisplayHandle(handle);
        if (!resolved.isValid()) {
            setErrorMessage("SDOM_DestroyDisplayObject: handle is invalid");
            return false;
        }
        SDOM::Core::getInstance().destroyDisplayObject(resolved.getName());
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_DestroyDisplayObject unknown error");
        return false;
    }
}

bool getAssetObject(const char* name, SDOM_AssetHandle* out_handle)
{
    if (!name) {
        setErrorMessage("SDOM_GetAssetObject: name is null");
        return false;
    }
    if (!out_handle) {
        setErrorMessage("SDOM_GetAssetObject: out_handle is null");
        return false;
    }

    try {
        SDOM::AssetHandle handle = SDOM::Core::getInstance().getAssetObject(name);
        if (!handle.isValid() || !handle.get()) {
            setErrorMessage("SDOM_GetAssetObject: not found");
            out_handle->object_id = 0;
            out_handle->name = nullptr;
            out_handle->type = nullptr;
            return false;
        }
        return writeAssetHandleOut(handle, out_handle, "SDOM_GetAssetObject: handle is not set");
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetAssetObject unknown error");
        return false;
    }
}

bool hasAssetObject(const char* name)
{
    if (!name) {
        setErrorMessage("SDOM_HasAssetObject: name is null");
        return false;
    }
    try {
        return SDOM::Core::getInstance().hasAssetObject(name);
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_HasAssetObject unknown error");
        return false;
    }
}

bool destroyAssetObjectByName(const char* name)
{
    if (!name) {
        setErrorMessage("SDOM_DestroyAssetObjectByName: name is null");
        return false;
    }
    try {
        SDOM::Core::getInstance().destroyAssetObject(name);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_DestroyAssetObjectByName unknown error");
        return false;
    }
}

bool destroyAssetObject(const SDOM_AssetHandle* handle)
{
    if (!handle) {
        setErrorMessage("SDOM_DestroyAssetObject: handle is null");
        return false;
    }
    try {
        const char* name = handle->name;
        if (!name || std::string(name).empty()) {
            setErrorMessage("SDOM_DestroyAssetObject: handle name is empty");
            return false;
        }
        SDOM::Core::getInstance().destroyAssetObject(name);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_DestroyAssetObject unknown error");
        return false;
    }
}

int countOrphanedDisplayObjects()
{
    try {
        return SDOM::Core::getInstance().countOrphanedDisplayObjects();
    } catch (...) {
        return 0;
    }
}

bool getOrphanedDisplayObjects(SDOM_DisplayHandle* out_handles, int* out_count, int max_count)
{
    if (!out_handles || !out_count) {
        setErrorMessage("SDOM_GetOrphans: out_handles or out_count is null");
        return false;
    }
    if (max_count <= 0) {
        setErrorMessage("SDOM_GetOrphans: max_count must be > 0");
        return false;
    }

    try {
        std::vector<DisplayHandle> orphans = SDOM::Core::getInstance().getOrphanedDisplayObjects();
        const int n = static_cast<int>(orphans.size());
        *out_count = n;
        const int to_copy = std::min(max_count, n);

        // Copy handles into out array
        static thread_local std::vector<std::string> names;
        static thread_local std::vector<std::string> types;
        names.clear();
        types.clear();
        names.reserve(to_copy);
        types.reserve(to_copy);

        for (int i = 0; i < to_copy; ++i) {
            auto& h = orphans[static_cast<size_t>(i)];
            names.push_back(h.getName());
            types.push_back(h.getType());
            out_handles[i].object_id = 0;
            out_handles[i].name = names.back().c_str();
            out_handles[i].type = types.back().c_str();
        }
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetOrphans unknown error");
        return false;
    }
}

bool detachOrphans()
{
    try {
        SDOM::Core::getInstance().detachOrphans();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_DetachOrphans unknown error");
        return false;
    }
}

bool collectGarbage()
{
    try {
        auto outcome = SDOM::Core::getInstance().collectGarbagePhase(false);
        if (!outcome.errorMessage.empty())
            setErrorMessage(outcome.errorMessage.c_str());
        return !outcome.fatalError;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_CollectGarbage unknown error");
        return false;
    }
}

bool attachFutureChildren()
{
    try {
        SDOM::Core::getInstance().attachFutureChildren();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_AttachFutureChildren unknown error");
        return false;
    }
}

bool addToOrphanList(const SDOM_DisplayHandle* orphan)
{
    if (!orphan) {
        setErrorMessage("SDOM_AddOrphan: orphan is null");
        return false;
    }
    try {
        DisplayHandle h = resolveDisplayHandle(orphan);
        SDOM::Core::getInstance().addToOrphanList(h);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_AddOrphan unknown error");
        return false;
    }
}

bool addToFutureChildrenList(const SDOM_DisplayHandle* child, const SDOM_DisplayHandle* parent, bool useWorld, int worldX, int worldY)
{
    if (!child || !parent) {
        setErrorMessage("SDOM_AddFutureChild: child or parent is null");
        return false;
    }
    try {
        DisplayHandle c = resolveDisplayHandle(child);
        DisplayHandle p = resolveDisplayHandle(parent);
        SDOM::Core::getInstance().addToFutureChildrenList(c, p, useWorld, worldX, worldY);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_AddFutureChild unknown error");
        return false;
    }
}

bool getDisplayObjectNames(const char** out_names, int* out_count, int max_count)
{
    if (!out_names || !out_count) {
        setErrorMessage("SDOM_GetDisplayObjectNames: out_names or out_count is null");
        return false;
    }
    if (max_count <= 0) {
        setErrorMessage("SDOM_GetDisplayObjectNames: max_count must be > 0");
        return false;
    }

    try {
        std::vector<std::string> names = SDOM::Core::getInstance().getDisplayObjectNames();
        const int n = static_cast<int>(names.size());
        *out_count = n;
        const int to_copy = std::min(max_count, n);
        static thread_local std::vector<std::string> s_names;
        s_names.clear();
        s_names.reserve(to_copy);
        for (int i = 0; i < to_copy; ++i) {
            s_names.push_back(names[static_cast<size_t>(i)]);
            out_names[i] = s_names.back().c_str();
        }
        if (to_copy < max_count) {
            out_names[to_copy] = nullptr; // null-terminate if space
        }
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetDisplayObjectNames unknown error");
        return false;
    }
}

bool clearFactory()
{
    try {
        SDOM::Core::getInstance().clearFactory();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_ClearFactory unknown error");
        return false;
    }
}

bool printObjectRegistry()
{
    try {
        SDOM::Core::getInstance().printObjectRegistry();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_PrintObjectRegistry unknown error");
        return false;
    }
}

bool getPropertyNamesForType(const char* type, const char** out_names, int* out_count, int max_count)
{
    setErrorMessage("SDOM_GetPropertyNamesForType not implemented");
    (void)type; (void)out_names; (void)out_count; (void)max_count;
    return false;
}

bool getCommandNamesForType(const char* type, const char** out_names, int* out_count, int max_count)
{
    setErrorMessage("SDOM_GetCommandNamesForType not implemented");
    (void)type; (void)out_names; (void)out_count; (void)max_count;
    return false;
}

bool getFunctionNamesForType(const char* type, const char** out_names, int* out_count, int max_count)
{
    setErrorMessage("SDOM_GetFunctionNamesForType not implemented");
    (void)type; (void)out_names; (void)out_count; (void)max_count;
    return false;
}

bool setIgnoreRealInput(bool ignore)
{
    try {
        SDOM::Core::getInstance().setIgnoreRealInput(ignore);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetIgnoreRealInput unknown error");
        return false;
    }
}

bool getIgnoreRealInput(bool* out_ignore)
{
    if (!out_ignore) {
        setErrorMessage("SDOM_GetIgnoreRealInput: out_ignore is null");
        return false;
    }
    try {
        *out_ignore = SDOM::Core::getInstance().getIgnoreRealInput();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetIgnoreRealInput unknown error");
        return false;
    }
}

bool getKeyfocusGray(float* out_gray)
{
    if (!out_gray) {
        setErrorMessage("SDOM_GetKeyfocusGray: out_gray is null");
        return false;
    }
    try {
        *out_gray = SDOM::Core::getInstance().getKeyfocusGray();
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetKeyfocusGray unknown error");
        return false;
    }
}

bool setKeyfocusGray(float gray)
{
    try {
        SDOM::Core::getInstance().setKeyfocusGray(gray);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetKeyfocusGray unknown error");
        return false;
    }
}

bool getWindow(SDL_Window** out_window)
{
    if (!out_window) {
        setErrorMessage("SDOM_GetWindow: out_window is null");
        return false;
    }
    *out_window = nullptr;
    try {
        *out_window = SDOM::Core::getInstance().getWindow();
        return *out_window != nullptr;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetWindow unknown error");
        return false;
    }
}

bool getRenderer(SDL_Renderer** out_renderer)
{
    if (!out_renderer) {
        setErrorMessage("SDOM_GetRenderer: out_renderer is null");
        return false;
    }
    *out_renderer = nullptr;
    try {
        *out_renderer = SDOM::Core::getInstance().getRenderer();
        return *out_renderer != nullptr;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetRenderer unknown error");
        return false;
    }
}

bool getTexture(SDL_Texture** out_texture)
{
    if (!out_texture) {
        setErrorMessage("SDOM_GetTexture: out_texture is null");
        return false;
    }
    *out_texture = nullptr;
    try {
        *out_texture = SDOM::Core::getInstance().getTexture();
        return *out_texture != nullptr;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_GetTexture unknown error");
        return false;
    }
}

bool setWindowWidth(float width)
{
    try {
        SDOM::Core::getInstance().setWindowWidth(width);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetWindowWidth unknown error");
        return false;
    }
}

bool setWindowHeight(float height)
{
    try {
        SDOM::Core::getInstance().setWindowHeight(height);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetWindowHeight unknown error");
        return false;
    }
}

bool setPixelWidth(float width)
{
    try {
        SDOM::Core::getInstance().setPixelWidth(width);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetPixelWidth unknown error");
        return false;
    }
}

bool setPixelHeight(float height)
{
    try {
        SDOM::Core::getInstance().setPixelHeight(height);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetPixelHeight unknown error");
        return false;
    }
}

bool setPreserveAspectRatio(bool preserve)
{
    try {
        SDOM::Core::getInstance().setPreserveAspectRatio(preserve);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetPreserveAspectRatio unknown error");
        return false;
    }
}

bool setAllowTextureResize(bool allow)
{
    try {
        SDOM::Core::getInstance().setAllowTextureResize(allow);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetAllowTextureResize unknown error");
        return false;
    }
}

bool setLogicalPresentation(SDL_RendererLogicalPresentation presentation)
{
    try {
        SDOM::Core::getInstance().setRendererLogicalPresentation(presentation);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetLogicalPresentation unknown error");
        return false;
    }
}

bool setWindowFlags(SDL_WindowFlags flags)
{
    try {
        SDOM::Core::getInstance().setWindowFlags(flags);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetWindowFlags unknown error");
        return false;
    }
}

bool setPixelFormat(SDL_PixelFormat format)
{
    try {
        SDOM::Core::getInstance().setPixelFormat(format);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetPixelFormat unknown error");
        return false;
    }
}

bool setFullscreen(bool fullscreen)
{
    try {
        SDOM::Core::getInstance().setFullscreen(fullscreen);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetFullscreen unknown error");
        return false;
    }
}

bool setWindowed(bool windowed)
{
    try {
        SDOM::Core::getInstance().setWindowed(windowed);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetWindowed unknown error");
        return false;
    }
}

bool setIsTraversing(bool traversing)
{
    try {
        SDOM::Core::getInstance().setIsTraversing(traversing);
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_SetIsTraversing unknown error");
        return false;
    }
}
// Callback registration adapters: accept opaque `void*` function pointers
// and internally cast them to the expected signatures. End-user typedefs
// live with the consumer; do not expose them in generated headers.
bool registerOnInit(void* fnptr)
{
    try {
        if (!fnptr) {
            SDOM::Core::getInstance().registerOnInit(nullptr);
            return true;
        }
        auto cb = reinterpret_cast<bool(*)(void)>(fnptr);
        SDOM::Core::getInstance().registerOnInit([cb]() -> bool {
            return cb();
        });
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_RegisterOnInit unknown error");
        return false;
    }
}

bool registerOnQuit(void* fnptr)
{
    try {
        if (!fnptr) {
            SDOM::Core::getInstance().registerOnQuit(nullptr);
            return true;
        }
        auto cb = reinterpret_cast<void(*)(void)>(fnptr);
        SDOM::Core::getInstance().registerOnQuit([cb]() {
            cb();
        });
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_RegisterOnQuit unknown error");
        return false;
    }
}

bool registerOnUpdate(void* fnptr)
{
    try {
        if (!fnptr) {
            SDOM::Core::getInstance().registerOnUpdate(nullptr);
            return true;
        }
        auto cb = reinterpret_cast<void(*)(float)>(fnptr);
        SDOM::Core::getInstance().registerOnUpdate([cb](float dt) {
            cb(dt);
        });
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_RegisterOnUpdate unknown error");
        return false;
    }
}

bool registerOnEvent(void* fnptr)
{
    try {
        if (!fnptr) {
            SDOM::Core::getInstance().registerOnEvent(nullptr);
            return true;
        }
        auto cb = reinterpret_cast<void(*)(SDOM_Event*)>(fnptr);
        SDOM::Core::getInstance().registerOnEvent([cb](const SDOM::Event& ev) {
            SDOM_Event wrapper;
            wrapper.impl = const_cast<void*>(static_cast<const void*>(&ev));
            cb(&wrapper);
        });
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_RegisterOnEvent unknown error");
        return false;
    }
}

bool registerOnRender(void* fnptr)
{
    try {
        if (!fnptr) {
            SDOM::Core::getInstance().registerOnRender(nullptr);
            return true;
        }
        auto cb = reinterpret_cast<void(*)(void)>(fnptr);
        SDOM::Core::getInstance().registerOnRender([cb]() {
            cb();
        });
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_RegisterOnRender unknown error");
        return false;
    }
}

bool registerOnUnitTest(void* fnptr)
{
    try {
        if (!fnptr) {
            SDOM::Core::getInstance().registerOnUnitTest(nullptr);
            return true;
        }
        auto cb = reinterpret_cast<bool(*)(void)>(fnptr);
        SDOM::Core::getInstance().registerOnUnitTest([cb]() -> bool {
            return cb();
        });
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_RegisterOnUnitTest unknown error");
        return false;
    }
}

bool registerOnWindowResize(void* fnptr)
{
    try {
        if (!fnptr) {
            SDOM::Core::getInstance().registerOnWindowResize(nullptr);
            return true;
        }
        auto cb = reinterpret_cast<void(*)(int,int)>(fnptr);
        SDOM::Core::getInstance().registerOnWindowResize([cb](int w, int h) {
            cb(w, h);
        });
        return true;
    } catch (const std::exception& e) {
        setErrorMessage(e.what());
        return false;
    } catch (...) {
        setErrorMessage("SDOM_RegisterOnWindowResize unknown error");
        return false;
    }
}

void registerBindings(Core& core, const std::string& typeName)
{
    core.registerMethod(
        typeName,
        "GetError",
        "std::string Core::capiGetError() const",
        "const char*",
        "SDOM_GetError",
        "const char* SDOM_GetError(void)",
        "Returns the most recent SDOM error string (or SDL_GetError when none).",
        []() -> std::string {
            return CoreAPI::getErrorString();
        });

    core.registerMethod(
        typeName,
        "SetError",
        "bool Core::capiSetError(const char* message)",
        "bool",
        "SDOM_SetError",
        "bool SDOM_SetError(const char* message)",
        "Sets the global SDOM error string consumers read via SDOM_GetError().",
        [](const char* message) -> bool {
            return CoreAPI::setErrorMessage(message);
        });

    core.registerMethod(
        typeName,
        "Init",
        "bool Core::capiInit(std::uint64_t flags)",
        "bool",
        "SDOM_Init",
        "bool SDOM_Init(uint64_t init_flags)",
        "Initializes the SDOM Core singleton using default configuration values.",
        [](std::uint64_t flags) -> bool {
            return CoreAPI::init(flags);
        });

    core.registerMethod(
        typeName,
        "Configure",
        "bool Core::capiConfigure(const SDOM_CoreConfig* cfg)",
        "bool",
        "SDOM_Configure",
        "bool SDOM_Configure(const SDOM_CoreConfig* cfg)",
        "Applies the supplied SDOM_CoreConfig to the Core singleton.",
        [](const SDOM_CoreConfig* cfg) -> bool {
            return CoreAPI::configure(cfg);
        });

    core.registerMethod(
        typeName,
        "ConfigureFromJson",
        "bool Core::capiConfigureFromJson(const char* json)",
        "bool",
        "SDOM_ConfigureFromJson",
        "bool SDOM_ConfigureFromJson(const char* json)",
        "Parses a JSON string into a CoreConfig and applies it.",
        [](const char* json) -> bool {
            return CoreAPI::configureFromJson(json);
        });

    core.registerMethod(
        typeName,
        "GetCoreConfig",
        "bool Core::capiGetCoreConfig(SDOM_CoreConfig* out_cfg)",
        "bool",
        "SDOM_GetCoreConfig",
        "bool SDOM_GetCoreConfig(SDOM_CoreConfig* out_cfg)",
        "Copies the active Core configuration into out_cfg.",
        [](SDOM_CoreConfig* out_cfg) -> bool {
            return CoreAPI::getCoreConfig(out_cfg);
        });

    core.registerMethod(
        typeName,
        "SetStopAfterUnitTests",
        "bool Core::capiSetStopAfterUnitTests(bool stop)",
        "bool",
        "SDOM_SetStopAfterUnitTests",
        "bool SDOM_SetStopAfterUnitTests(bool stop)",
        "Instructs the Core main loop to exit automatically after unit tests complete.",
        [](bool stop) -> bool {
            return CoreAPI::setStopAfterUnitTests(stop);
        });

    core.registerMethod(
        typeName,
        "SetIsRunning",
        "bool Core::capiSetIsRunning(bool running)",
        "bool",
        "SDOM_SetIsRunning",
        "bool SDOM_SetIsRunning(bool running)",
        "Start or stop the Core main loop without invoking a full shutdown; accepts true to run, false to pause.",
        [](bool running) -> bool {
            return CoreAPI::setIsRunning(running);
        });

    core.registerMethod(
        typeName,
        "LoadDomFromJsonFile",
        "bool Core::capiLoadDomFromJsonFile(const char* filename)",
        "bool",
        "SDOM_LoadDomFromJsonFile",
        "bool SDOM_LoadDomFromJsonFile(const char* filename)",
        "Loads Core configuration, resources, and DOM hierarchy from the provided JSON document.",
        [](const char* filename) -> bool {
            return CoreAPI::loadDomFromJsonFile(filename);
        });

    core.registerMethod(
        typeName,
        "LoadDomFromJson",
        "bool Core::capiLoadDomFromJson(const char* json)",
        "bool",
        "SDOM_LoadDomFromJson",
        "bool SDOM_LoadDomFromJson(const char* json)",
        "Loads Core configuration, resources, and DOM hierarchy from an in-memory JSON string.",
        [](const char* json) -> bool {
            return CoreAPI::loadDomFromJson(json);
        });

    core.registerMethod(
        typeName,
        "GetBorderColor",
        "bool Core::capiGetBorderColor(SDL_Color* out_color)",
        "bool",
        "SDOM_GetBorderColor",
        "bool SDOM_GetBorderColor(SDL_Color* out_color)",
        "Retrieves the current frame border color.",
        [](SDL_Color* out_color) -> bool {
            return CoreAPI::getBorderColor(out_color);
        });

    core.registerMethod(
        typeName,
        "SetBorderColor",
        "bool Core::capiSetBorderColor(const SDL_Color* color)",
        "bool",
        "SDOM_SetBorderColor",
        "bool SDOM_SetBorderColor(const SDL_Color* color)",
        "Sets the frame border color and applies it.",
        [](const SDL_Color* color) -> bool {
            return CoreAPI::setBorderColor(color);
        });

    core.registerMethod(
        typeName,
        "GetWindowTitle",
        "const char* Core::capiGetWindowTitle() const",
        "const char*",
        "SDOM_GetWindowTitle",
        "const char* SDOM_GetWindowTitle(void)",
        "Returns the cached window title (borrowed string).",
        []() -> const char* {
            return CoreAPI::getWindowTitle();
        });

    core.registerMethod(
        typeName,
        "SetWindowTitle",
        "bool Core::capiSetWindowTitle(const char* title)",
        "bool",
        "SDOM_SetWindowTitle",
        "bool SDOM_SetWindowTitle(const char* title)",
        "Sets the window title; applies immediately when window is live.",
        [](const char* title) -> bool {
            return CoreAPI::setWindowTitle(title);
        });

    core.registerMethod(
        typeName,
        "PumpEventsOnce",
        "bool Core::capiPumpEventsOnce()",
        "bool",
        "SDOM_PumpEventsOnce",
        "bool SDOM_PumpEventsOnce(void)",
        "Pumps SDL events one iteration for headless/deterministic testing.",
        []() -> bool {
            return CoreAPI::pumpEventsOnce();
        });

    core.registerMethod(
        typeName,
        "PushMouseEvent",
        "bool Core::capiPushMouseEvent(const char* json)",
        "bool",
        "SDOM_PushMouseEvent",
        "bool SDOM_PushMouseEvent(const char* json)",
        "Queues a synthetic mouse event from a JSON description.",
        [](const char* json) -> bool {
            return CoreAPI::pushMouseEvent(json);
        });

    core.registerMethod(
        typeName,
        "PushKeyboardEvent",
        "bool Core::capiPushKeyboardEvent(const char* json)",
        "bool",
        "SDOM_PushKeyboardEvent",
        "bool SDOM_PushKeyboardEvent(const char* json)",
        "Queues a synthetic keyboard event from a JSON description.",
        [](const char* json) -> bool {
            return CoreAPI::pushKeyboardEvent(json);
        });

    core.registerMethod(
        typeName,
        "HasLuaSupport",
        "bool Core::capiHasLuaSupport() const",
        "bool",
        "SDOM_HasLuaSupport",
        "bool SDOM_HasLuaSupport(void)",
        "Returns true when Lua support is compiled in and available.",
        []() -> bool {
            return CoreAPI::hasLuaSupport();
        });

    core.registerMethod(
        typeName,
        "GetVersionString",
        "const char* Core::capiGetVersionString() const",
        "const char*",
        "SDOM_GetVersionString",
        "const char* SDOM_GetVersionString(void)",
        "Returns semantic version string.",
        []() -> const char* {
            return CoreAPI::getVersionString();
        });

    core.registerMethod(
        typeName,
        "GetVersionFullString",
        "const char* Core::capiGetVersionFullString() const",
        "const char*",
        "SDOM_GetVersionFullString",
        "const char* SDOM_GetVersionFullString(void)",
        "Returns full version string including build metadata.",
        []() -> const char* {
            return CoreAPI::getVersionFullString();
        });

    core.registerMethod(
        typeName,
        "GetVersionMajor",
        "int Core::capiGetVersionMajor() const",
        "int",
        "SDOM_GetVersionMajor",
        "int SDOM_GetVersionMajor(void)",
        "Returns major version component.",
        []() -> int {
            return CoreAPI::getVersionMajor();
        });

    core.registerMethod(
        typeName,
        "GetVersionMinor",
        "int Core::capiGetVersionMinor() const",
        "int",
        "SDOM_GetVersionMinor",
        "int SDOM_GetVersionMinor(void)",
        "Returns minor version component.",
        []() -> int {
            return CoreAPI::getVersionMinor();
        });

    core.registerMethod(
        typeName,
        "GetVersionPatch",
        "int Core::capiGetVersionPatch() const",
        "int",
        "SDOM_GetVersionPatch",
        "int SDOM_GetVersionPatch(void)",
        "Returns patch version component.",
        []() -> int {
            return CoreAPI::getVersionPatch();
        });

    core.registerMethod(
        typeName,
        "GetVersionCodename",
        "const char* Core::capiGetVersionCodename() const",
        "const char*",
        "SDOM_GetVersionCodename",
        "const char* SDOM_GetVersionCodename(void)",
        "Returns codename string.",
        []() -> const char* {
            return CoreAPI::getVersionCodename();
        });

    core.registerMethod(
        typeName,
        "GetVersionBuild",
        "const char* Core::capiGetVersionBuild() const",
        "const char*",
        "SDOM_GetVersionBuild",
        "const char* SDOM_GetVersionBuild(void)",
        "Returns build identifier.",
        []() -> const char* {
            return CoreAPI::getVersionBuild();
        });

    core.registerMethod(
        typeName,
        "GetVersionBuildDate",
        "const char* Core::capiGetVersionBuildDate() const",
        "const char*",
        "SDOM_GetVersionBuildDate",
        "const char* SDOM_GetVersionBuildDate(void)",
        "Returns build date/time string.",
        []() -> const char* {
            return CoreAPI::getVersionBuildDate();
        });

    core.registerMethod(
        typeName,
        "GetVersionCommit",
        "const char* Core::capiGetVersionCommit() const",
        "const char*",
        "SDOM_GetVersionCommit",
        "const char* SDOM_GetVersionCommit(void)",
        "Returns commit hash.",
        []() -> const char* {
            return CoreAPI::getVersionCommit();
        });

    core.registerMethod(
        typeName,
        "GetVersionBranch",
        "const char* Core::capiGetVersionBranch() const",
        "const char*",
        "SDOM_GetVersionBranch",
        "const char* SDOM_GetVersionBranch(void)",
        "Returns branch name.",
        []() -> const char* {
            return CoreAPI::getVersionBranch();
        });

    core.registerMethod(
        typeName,
        "GetVersionCompiler",
        "const char* Core::capiGetVersionCompiler() const",
        "const char*",
        "SDOM_GetVersionCompiler",
        "const char* SDOM_GetVersionCompiler(void)",
        "Returns compiler string.",
        []() -> const char* {
            return CoreAPI::getVersionCompiler();
        });

    core.registerMethod(
        typeName,
        "GetVersionPlatform",
        "const char* Core::capiGetVersionPlatform() const",
        "const char*",
        "SDOM_GetVersionPlatform",
        "const char* SDOM_GetVersionPlatform(void)",
        "Returns platform description.",
        []() -> const char* {
            return CoreAPI::getVersionPlatform();
        });

    core.registerMethod(
        typeName,
        "GetElapsedTime",
        "float Core::capiGetElapsedTime() const",
        "float",
        "SDOM_GetElapsedTime",
        "float SDOM_GetElapsedTime(void)",
        "Returns the last frame delta time in seconds.",
        []() -> float {
            return CoreAPI::getElapsedTime();
        });

    core.registerMethod(
        typeName,
        "GetWindowWidth",
        "float Core::capiGetWindowWidth() const",
        "float",
        "SDOM_GetWindowWidth",
        "float SDOM_GetWindowWidth(void)",
        "Returns the configured window width.",
        []() -> float {
            return CoreAPI::getWindowWidth();
        });

    core.registerMethod(
        typeName,
        "GetWindowHeight",
        "float Core::capiGetWindowHeight() const",
        "float",
        "SDOM_GetWindowHeight",
        "float SDOM_GetWindowHeight(void)",
        "Returns the configured window height.",
        []() -> float {
            return CoreAPI::getWindowHeight();
        });

    core.registerMethod(
        typeName,
        "GetPixelWidth",
        "float Core::capiGetPixelWidth() const",
        "float",
        "SDOM_GetPixelWidth",
        "float SDOM_GetPixelWidth(void)",
        "Returns the configured pixel width.",
        []() -> float {
            return CoreAPI::getPixelWidth();
        });

    core.registerMethod(
        typeName,
        "GetPixelHeight",
        "float Core::capiGetPixelHeight() const",
        "float",
        "SDOM_GetPixelHeight",
        "float SDOM_GetPixelHeight(void)",
        "Returns the configured pixel height.",
        []() -> float {
            return CoreAPI::getPixelHeight();
        });

    core.registerMethod(
        typeName,
        "GetPreserveAspectRatio",
        "bool Core::capiGetPreserveAspectRatio() const",
        "bool",
        "SDOM_GetPreserveAspectRatio",
        "bool SDOM_GetPreserveAspectRatio(void)",
        "Returns whether aspect ratio preservation is enabled.",
        []() -> bool {
            return CoreAPI::getPreserveAspectRatio();
        });

    core.registerMethod(
        typeName,
        "GetAllowTextureResize",
        "bool Core::capiGetAllowTextureResize() const",
        "bool",
        "SDOM_GetAllowTextureResize",
        "bool SDOM_GetAllowTextureResize(void)",
        "Returns whether texture resize is allowed.",
        []() -> bool {
            return CoreAPI::getAllowTextureResize();
        });

    core.registerMethod(
        typeName,
        "GetLogicalPresentation",
        "SDL_RendererLogicalPresentation Core::capiGetLogicalPresentation() const",
        "SDL_RendererLogicalPresentation",
        "SDOM_GetLogicalPresentation",
        "SDL_RendererLogicalPresentation SDOM_GetLogicalPresentation(void)",
        "Returns the logical presentation mode.",
        []() -> SDL_RendererLogicalPresentation {
            return CoreAPI::getLogicalPresentation();
        });

    core.registerMethod(
        typeName,
        "GetWindowFlags",
        "SDL_WindowFlags Core::capiGetWindowFlags() const",
        "SDL_WindowFlags",
        "SDOM_GetWindowFlags",
        "SDL_WindowFlags SDOM_GetWindowFlags(void)",
        "Returns current window flags.",
        []() -> SDL_WindowFlags {
            return CoreAPI::getWindowFlags();
        });

    core.registerMethod(
        typeName,
        "GetPixelFormat",
        "SDL_PixelFormat Core::capiGetPixelFormat() const",
        "SDL_PixelFormat",
        "SDOM_GetPixelFormat",
        "SDL_PixelFormat SDOM_GetPixelFormat(void)",
        "Returns the pixel format.",
        []() -> SDL_PixelFormat {
            return CoreAPI::getPixelFormat();
        });

    core.registerMethod(
        typeName,
        "IsFullscreen",
        "bool Core::capiIsFullscreen() const",
        "bool",
        "SDOM_IsFullscreen",
        "bool SDOM_IsFullscreen(void)",
        "Returns true if Core is in fullscreen mode.",
        []() -> bool {
            return CoreAPI::isFullscreen();
        });

    core.registerMethod(
        typeName,
        "IsWindowed",
        "bool Core::capiIsWindowed() const",
        "bool",
        "SDOM_IsWindowed",
        "bool SDOM_IsWindowed(void)",
        "Returns true if Core is windowed.",
        []() -> bool {
            return CoreAPI::isWindowed();
        });

    core.registerMethod(
        typeName,
        "GetIsTraversing",
        "bool Core::capiGetIsTraversing() const",
        "bool",
        "SDOM_GetIsTraversing",
        "bool SDOM_GetIsTraversing(void)",
        "Returns true while the DOM is actively being traversed.",
        []() -> bool {
            return CoreAPI::getIsTraversing();
        });

    core.registerMethod(
        typeName,
        "HandleTabKeyPress",
        "bool Core::capiHandleTabKeyPress()",
        "bool",
        "SDOM_HandleTabKeyPress",
        "bool SDOM_HandleTabKeyPress(void)",
        "Moves focus forward (testing/editor only).",
        []() -> bool {
            return CoreAPI::handleTabKeyPress();
        });

    core.registerMethod(
        typeName,
        "HandleTabKeyPressReverse",
        "bool Core::capiHandleTabKeyPressReverse()",
        "bool",
        "SDOM_HandleTabKeyPressReverse",
        "bool SDOM_HandleTabKeyPressReverse(void)",
        "Moves focus backward (testing/editor only).",
        []() -> bool {
            return CoreAPI::handleTabKeyPressReverse();
        });

    core.registerMethod(
        typeName,
        "GetIsTraversing",
        "bool Core::capiGetIsTraversing() const",
        "bool",
        "SDOM_GetIsTraversing",
        "bool SDOM_GetIsTraversing(void)",
        "Returns true while the DOM is actively being traversed.",
        []() -> bool {
            return CoreAPI::getIsTraversing();
        });

    core.registerMethod(
        typeName,
        "GetWindow",
        "bool Core::capiGetWindow(SDL_Window** out_window)",
        "bool",
        "SDOM_GetWindow",
        "bool SDOM_GetWindow(SDL_Window** out_window)",
        "Borrows the SDL_Window owned by Core; do not destroy it.",
        [](SDL_Window** out_window) -> bool {
            return CoreAPI::getWindow(out_window);
        });

    core.registerMethod(
        typeName,
        "GetRenderer",
        "bool Core::capiGetRenderer(SDL_Renderer** out_renderer)",
        "bool",
        "SDOM_GetRenderer",
        "bool SDOM_GetRenderer(SDL_Renderer** out_renderer)",
        "Borrows the SDL_Renderer owned by Core; do not destroy it.",
        [](SDL_Renderer** out_renderer) -> bool {
            return CoreAPI::getRenderer(out_renderer);
        });

    core.registerMethod(
        typeName,
        "GetTexture",
        "bool Core::capiGetTexture(SDL_Texture** out_texture)",
        "bool",
        "SDOM_GetTexture",
        "bool SDOM_GetTexture(SDL_Texture** out_texture)",
        "Borrows the SDL_Texture owned by Core; do not destroy it.",
        [](SDL_Texture** out_texture) -> bool {
            return CoreAPI::getTexture(out_texture);
        });

    core.registerMethod(
        typeName,
        "SetWindowWidth",
        "bool Core::capiSetWindowWidth(float width)",
        "bool",
        "SDOM_SetWindowWidth",
        "bool SDOM_SetWindowWidth(float width)",
        "Sets window width; may trigger reconfigure.",
        [](float width) -> bool {
            return CoreAPI::setWindowWidth(width);
        });

    core.registerMethod(
        typeName,
        "SetWindowHeight",
        "bool Core::capiSetWindowHeight(float height)",
        "bool",
        "SDOM_SetWindowHeight",
        "bool SDOM_SetWindowHeight(float height)",
        "Sets window height; may trigger reconfigure.",
        [](float height) -> bool {
            return CoreAPI::setWindowHeight(height);
        });

    core.registerMethod(
        typeName,
        "SetPixelWidth",
        "bool Core::capiSetPixelWidth(float width)",
        "bool",
        "SDOM_SetPixelWidth",
        "bool SDOM_SetPixelWidth(float width)",
        "Sets pixel width.",
        [](float width) -> bool {
            return CoreAPI::setPixelWidth(width);
        });

    core.registerMethod(
        typeName,
        "SetPixelHeight",
        "bool Core::capiSetPixelHeight(float height)",
        "bool",
        "SDOM_SetPixelHeight",
        "bool SDOM_SetPixelHeight(float height)",
        "Sets pixel height.",
        [](float height) -> bool {
            return CoreAPI::setPixelHeight(height);
        });

    core.registerMethod(
        typeName,
        "SetPreserveAspectRatio",
        "bool Core::capiSetPreserveAspectRatio(bool preserve)",
        "bool",
        "SDOM_SetPreserveAspectRatio",
        "bool SDOM_SetPreserveAspectRatio(bool preserve)",
        "Sets aspect ratio preservation.",
        [](bool preserve) -> bool {
            return CoreAPI::setPreserveAspectRatio(preserve);
        });

    core.registerMethod(
        typeName,
        "SetAllowTextureResize",
        "bool Core::capiSetAllowTextureResize(bool allow)",
        "bool",
        "SDOM_SetAllowTextureResize",
        "bool SDOM_SetAllowTextureResize(bool allow)",
        "Sets whether texture resize is allowed.",
        [](bool allow) -> bool {
            return CoreAPI::setAllowTextureResize(allow);
        });

    core.registerMethod(
        typeName,
        "SetLogicalPresentation",
        "bool Core::capiSetLogicalPresentation(SDL_RendererLogicalPresentation presentation)",
        "bool",
        "SDOM_SetLogicalPresentation",
        "bool SDOM_SetLogicalPresentation(SDL_RendererLogicalPresentation presentation)",
        "Sets logical presentation mode.",
        [](SDL_RendererLogicalPresentation presentation) -> bool {
            return CoreAPI::setLogicalPresentation(presentation);
        });

    core.registerMethod(
        typeName,
        "SetWindowFlags",
        "bool Core::capiSetWindowFlags(SDL_WindowFlags flags)",
        "bool",
        "SDOM_SetWindowFlags",
        "bool SDOM_SetWindowFlags(SDL_WindowFlags flags)",
        "Sets window flags.",
        [](SDL_WindowFlags flags) -> bool {
            return CoreAPI::setWindowFlags(flags);
        });

    core.registerMethod(
        typeName,
        "SetPixelFormat",
        "bool Core::capiSetPixelFormat(SDL_PixelFormat fmt)",
        "bool",
        "SDOM_SetPixelFormat",
        "bool SDOM_SetPixelFormat(SDL_PixelFormat fmt)",
        "Sets pixel format.",
        [](SDL_PixelFormat fmt) -> bool {
            return CoreAPI::setPixelFormat(fmt);
        });

    core.registerMethod(
        typeName,
        "SetFullscreen",
        "bool Core::capiSetFullscreen(bool fullscreen)",
        "bool",
        "SDOM_SetFullscreen",
        "bool SDOM_SetFullscreen(bool fullscreen)",
        "Toggles fullscreen.",
        [](bool fullscreen) -> bool {
            return CoreAPI::setFullscreen(fullscreen);
        });

    core.registerMethod(
        typeName,
        "SetWindowed",
        "bool Core::capiSetWindowed(bool windowed)",
        "bool",
        "SDOM_SetWindowed",
        "bool SDOM_SetWindowed(bool windowed)",
        "Toggles windowed mode.",
        [](bool windowed) -> bool {
            return CoreAPI::setWindowed(windowed);
        });

    core.registerMethod(
        typeName,
        "SetIsTraversing",
        "bool Core::capiSetIsTraversing(bool traversing)",
        "bool",
        "SDOM_SetIsTraversing",
        "bool SDOM_SetIsTraversing(bool traversing)",
        "Sets traversal flag; primarily for tests/tooling.",
        [](bool traversing) -> bool {
            return CoreAPI::setIsTraversing(traversing);
        });

    core.registerMethod(
        typeName,
        "SetKeyboardFocus",
        "bool Core::capiSetKeyboardFocus(const SDOM_DisplayHandle* handle)",
        "bool",
        "SDOM_SetKeyboardFocus",
        "bool SDOM_SetKeyboardFocus(const SDOM_DisplayHandle* handle)",
        "Directly sets keyboard focus (testing/editor only).",
        [](const SDOM_DisplayHandle* handle) -> bool {
            return CoreAPI::setKeyboardFocus(handle);
        });

    core.registerMethod(
        typeName,
        "GetKeyboardFocus",
        "bool Core::capiGetKeyboardFocus(SDOM_DisplayHandle* out_handle)",
        "bool",
        "SDOM_GetKeyboardFocus",
        "bool SDOM_GetKeyboardFocus(SDOM_DisplayHandle* out_handle)",
        "Retrieves the current keyboard focus handle.",
        [](SDOM_DisplayHandle* out_handle) -> bool {
            return CoreAPI::getKeyboardFocus(out_handle);
        });

    core.registerMethod(
        typeName,
        "ClearKeyboardFocus",
        "bool Core::capiClearKeyboardFocus()",
        "bool",
        "SDOM_ClearKeyboardFocus",
        "bool SDOM_ClearKeyboardFocus(void)",
        "Clears keyboard focus.",
        []() -> bool {
            return CoreAPI::clearKeyboardFocus();
        });

    core.registerMethod(
        typeName,
        "SetMouseHover",
        "bool Core::capiSetMouseHover(const SDOM_DisplayHandle* handle)",
        "bool",
        "SDOM_SetMouseHover",
        "bool SDOM_SetMouseHover(const SDOM_DisplayHandle* handle)",
        "Directly sets mouse hover (testing/editor only).",
        [](const SDOM_DisplayHandle* handle) -> bool {
            return CoreAPI::setMouseHover(handle);
        });

    core.registerMethod(
        typeName,
        "GetMouseHover",
        "bool Core::capiGetMouseHover(SDOM_DisplayHandle* out_handle)",
        "bool",
        "SDOM_GetMouseHover",
        "bool SDOM_GetMouseHover(SDOM_DisplayHandle* out_handle)",
        "Retrieves the current mouse hover handle.",
        [](SDOM_DisplayHandle* out_handle) -> bool {
            return CoreAPI::getMouseHover(out_handle);
        });

    core.registerMethod(
        typeName,
        "ClearMouseHover",
        "bool Core::capiClearMouseHover()",
        "bool",
        "SDOM_ClearMouseHover",
        "bool SDOM_ClearMouseHover(void)",
        "Clears mouse hover.",
        []() -> bool {
            return CoreAPI::clearMouseHover();
        });

    core.registerMethod(
        typeName,
        "GetFrameCount",
        "int Core::capiGetFrameCount() const",
        "int",
        "SDOM_GetFrameCount",
        "int SDOM_GetFrameCount(void)",
        "Returns the current frame count (testing/tooling).",
        []() -> int {
            return CoreAPI::getFrameCount();
        });

    core.registerMethod(
        typeName,
        "GetDisplayObject",
        "bool Core::capiGetDisplayObject(const char* name, SDOM_DisplayHandle* out_handle)",
        "bool",
        "SDOM_GetDisplayObject",
        "bool SDOM_GetDisplayObject(const char* name, SDOM_DisplayHandle* out_handle)",
        "Lookup a display object by name and return its handle.",
        [](const char* name, SDOM_DisplayHandle* out_handle) -> bool {
            return CoreAPI::getDisplayObject(name, out_handle);
        });

    core.registerMethod(
        typeName,
        "HasDisplayObject",
        "bool Core::capiHasDisplayObject(const char* name) const",
        "bool",
        "SDOM_HasDisplayObject",
        "bool SDOM_HasDisplayObject(const char* name)",
        "Returns true if a display object by that name exists.",
        [](const char* name) -> bool {
            return CoreAPI::hasDisplayObject(name);
        });

    core.registerMethod(
        typeName,
        "DestroyDisplayObjectByName",
        "bool Core::capiDestroyDisplayObjectByName(const char* name)",
        "bool",
        "SDOM_DestroyDisplayObjectByName",
        "bool SDOM_DestroyDisplayObjectByName(const char* name)",
        "Destroys a display object by name.",
        [](const char* name) -> bool {
            return CoreAPI::destroyDisplayObjectByName(name);
        });

    core.registerMethod(
        typeName,
        "DestroyDisplayObject",
        "bool Core::capiDestroyDisplayObject(const SDOM_DisplayHandle* handle)",
        "bool",
        "SDOM_DestroyDisplayObject",
        "bool SDOM_DestroyDisplayObject(const SDOM_DisplayHandle* handle)",
        "Destroys a display object by handle.",
        [](const SDOM_DisplayHandle* handle) -> bool {
            return CoreAPI::destroyDisplayObject(handle);
        });

    core.registerMethod(
        typeName,
        "GetAssetObject",
        "bool Core::capiGetAssetObject(const char* name, SDOM_AssetHandle* out_handle)",
        "bool",
        "SDOM_GetAssetObject",
        "bool SDOM_GetAssetObject(const char* name, SDOM_AssetHandle* out_handle)",
        "Lookup an asset by name and return its handle.",
        [](const char* name, SDOM_AssetHandle* out_handle) -> bool {
            return CoreAPI::getAssetObject(name, out_handle);
        });

    core.registerMethod(
        typeName,
        "HasAssetObject",
        "bool Core::capiHasAssetObject(const char* name) const",
        "bool",
        "SDOM_HasAssetObject",
        "bool SDOM_HasAssetObject(const char* name)",
        "Returns true if an asset by that name exists.",
        [](const char* name) -> bool {
            return CoreAPI::hasAssetObject(name);
        });

    core.registerMethod(
        typeName,
        "DestroyAssetObjectByName",
        "bool Core::capiDestroyAssetObjectByName(const char* name)",
        "bool",
        "SDOM_DestroyAssetObjectByName",
        "bool SDOM_DestroyAssetObjectByName(const char* name)",
        "Destroys an asset by name.",
        [](const char* name) -> bool {
            return CoreAPI::destroyAssetObjectByName(name);
        });

    core.registerMethod(
        typeName,
        "DestroyAssetObject",
        "bool Core::capiDestroyAssetObject(const SDOM_AssetHandle* handle)",
        "bool",
        "SDOM_DestroyAssetObject",
        "bool SDOM_DestroyAssetObject(const SDOM_AssetHandle* handle)",
        "Destroys an asset by handle.",
        [](const SDOM_AssetHandle* handle) -> bool {
            return CoreAPI::destroyAssetObject(handle);
        });

    core.registerMethod(
        typeName,
        "CountOrphans",
        "int Core::capiCountOrphans() const",
        "int",
        "SDOM_CountOrphans",
        "int SDOM_CountOrphans(void)",
        "Counts orphaned display objects (testing/editor).",
        []() -> int {
            return CoreAPI::countOrphanedDisplayObjects();
        });

    core.registerMethod(
        typeName,
        "GetOrphans",
        "bool Core::capiGetOrphans(SDOM_DisplayHandle* out_handles, int* out_count, int max_count)",
        "bool",
        "SDOM_GetOrphans",
        "bool SDOM_GetOrphans(SDOM_DisplayHandle* out_handles, int* out_count, int max_count)",
        "Returns orphaned display handles up to max_count; testing/editor only.",
        [](SDOM_DisplayHandle* out_handles, int* out_count, int max_count) -> bool {
            return CoreAPI::getOrphanedDisplayObjects(out_handles, out_count, max_count);
        });

    core.registerMethod(
        typeName,
        "DetachOrphans",
        "bool Core::capiDetachOrphans()",
        "bool",
        "SDOM_DetachOrphans",
        "bool SDOM_DetachOrphans(void)",
        "Detaches orphaned display objects (testing/editor).",
        []() -> bool {
            return CoreAPI::detachOrphans();
        });

    core.registerMethod(
        typeName,
        "CollectGarbage",
        "bool Core::capiCollectGarbage()",
        "bool",
        "SDOM_CollectGarbage",
        "bool SDOM_CollectGarbage(void)",
        "Collects unreachable objects.",
        []() -> bool {
            return CoreAPI::collectGarbage();
        });

    core.registerMethod(
        typeName,
        "AttachFutureChildren",
        "bool Core::capiAttachFutureChildren()",
        "bool",
        "SDOM_AttachFutureChildren",
        "bool SDOM_AttachFutureChildren(void)",
        "Attaches deferred children (testing/editor).",
        []() -> bool {
            return CoreAPI::attachFutureChildren();
        });

    core.registerMethod(
        typeName,
        "AddOrphan",
        "bool Core::capiAddOrphan(const SDOM_DisplayHandle* orphan)",
        "bool",
        "SDOM_AddOrphan",
        "bool SDOM_AddOrphan(const SDOM_DisplayHandle* orphan)",
        "Adds an orphan handle (testing/editor).",
        [](const SDOM_DisplayHandle* orphan) -> bool {
            return CoreAPI::addToOrphanList(orphan);
        });

    core.registerMethod(
        typeName,
        "AddFutureChild",
        "bool Core::capiAddFutureChild(const SDOM_DisplayHandle* child, const SDOM_DisplayHandle* parent, bool useWorld, int worldX, int worldY)",
        "bool",
        "SDOM_AddFutureChild",
        "bool SDOM_AddFutureChild(const SDOM_DisplayHandle* child, const SDOM_DisplayHandle* parent, bool useWorld, int worldX, int worldY)",
        "Queues a future child relationship (testing/editor).",
        [](const SDOM_DisplayHandle* child, const SDOM_DisplayHandle* parent, bool useWorld, int worldX, int worldY) -> bool {
            return CoreAPI::addToFutureChildrenList(child, parent, useWorld, worldX, worldY);
        });

    core.registerMethod(
        typeName,
        "GetDisplayObjectNames",
        "bool Core::capiGetDisplayObjectNames(const char** out_names, int* out_count, int max_count)",
        "bool",
        "SDOM_GetDisplayObjectNames",
        "bool SDOM_GetDisplayObjectNames(const char** out_names, int* out_count, int max_count)",
        "Returns display object names up to max_count; null-terminates when space permits.",
        [](const char** out_names, int* out_count, int max_count) -> bool {
            return CoreAPI::getDisplayObjectNames(out_names, out_count, max_count);
        });

    core.registerMethod(
        typeName,
        "ClearFactory",
        "bool Core::capiClearFactory()",
        "bool",
        "SDOM_ClearFactory",
        "bool SDOM_ClearFactory(void)",
        "Clears factory registrations (hot-reload/test).",
        []() -> bool {
            return CoreAPI::clearFactory();
        });

    core.registerMethod(
        typeName,
        "PrintObjectRegistry",
        "bool Core::capiPrintObjectRegistry() const",
        "bool",
        "SDOM_PrintObjectRegistry",
        "bool SDOM_PrintObjectRegistry(void)",
        "Prints registered objects (debug).",
        []() -> bool {
            return CoreAPI::printObjectRegistry();
        });

    core.registerMethod(
        typeName,
        "GetPropertyNamesForType",
        "bool Core::capiGetPropertyNamesForType(const char* type, const char** out_names, int* out_count, int max_count) const",
        "bool",
        "SDOM_GetPropertyNamesForType",
        "bool SDOM_GetPropertyNamesForType(const char* type, const char** out_names, int* out_count, int max_count)",
        "Returns property names for a type (currently stubbed).",
        [](const char* type, const char** out_names, int* out_count, int max_count) -> bool {
            return CoreAPI::getPropertyNamesForType(type, out_names, out_count, max_count);
        });

    core.registerMethod(
        typeName,
        "GetCommandNamesForType",
        "bool Core::capiGetCommandNamesForType(const char* type, const char** out_names, int* out_count, int max_count) const",
        "bool",
        "SDOM_GetCommandNamesForType",
        "bool SDOM_GetCommandNamesForType(const char* type, const char** out_names, int* out_count, int max_count)",
        "Returns command names for a type (currently stubbed).",
        [](const char* type, const char** out_names, int* out_count, int max_count) -> bool {
            return CoreAPI::getCommandNamesForType(type, out_names, out_count, max_count);
        });

    core.registerMethod(
        typeName,
        "GetFunctionNamesForType",
        "bool Core::capiGetFunctionNamesForType(const char* type, const char** out_names, int* out_count, int max_count) const",
        "bool",
        "SDOM_GetFunctionNamesForType",
        "bool SDOM_GetFunctionNamesForType(const char* type, const char** out_names, int* out_count, int max_count)",
        "Returns function names for a type (currently stubbed).",
        [](const char* type, const char** out_names, int* out_count, int max_count) -> bool {
            return CoreAPI::getFunctionNamesForType(type, out_names, out_count, max_count);
        });

    core.registerMethod(
        typeName,
        "SetIgnoreRealInput",
        "bool Core::capiSetIgnoreRealInput(bool ignore)",
        "bool",
        "SDOM_SetIgnoreRealInput",
        "bool SDOM_SetIgnoreRealInput(bool ignore)",
        "Testing: ignore real input while processing synthetic events.",
        [](bool ignore) -> bool {
            return CoreAPI::setIgnoreRealInput(ignore);
        });

    core.registerMethod(
        typeName,
        "GetIgnoreRealInput",
        "bool Core::capiGetIgnoreRealInput(bool* out_ignore) const",
        "bool",
        "SDOM_GetIgnoreRealInput",
        "bool SDOM_GetIgnoreRealInput(bool* out_ignore)",
        "Testing: read ignore-real-input flag.",
        [](bool* out_ignore) -> bool {
            return CoreAPI::getIgnoreRealInput(out_ignore);
        });

    core.registerMethod(
        typeName,
        "GetKeyfocusGray",
        "bool Core::capiGetKeyfocusGray(float* out_gray) const",
        "bool",
        "SDOM_GetKeyfocusGray",
        "bool SDOM_GetKeyfocusGray(float* out_gray)",
        "Testing: retrieve keyfocus gray level.",
        [](float* out_gray) -> bool {
            return CoreAPI::getKeyfocusGray(out_gray);
        });

    core.registerMethod(
        typeName,
        "SetKeyfocusGray",
        "bool Core::capiSetKeyfocusGray(float gray)",
        "bool",
        "SDOM_SetKeyfocusGray",
        "bool SDOM_SetKeyfocusGray(float gray)",
        "Testing: set keyfocus gray level.",
        [](float gray) -> bool {
            return CoreAPI::setKeyfocusGray(gray);
        });

    core.registerMethod(
        typeName,
        "CreateDisplayObjectFromJson",
        "bool Core::capiCreateDisplayObjectFromJson(const char* type, const char* json, SDOM_DisplayHandle* out_handle)",
        "bool",
        "SDOM_CreateDisplayObjectFromJson",
        "bool SDOM_CreateDisplayObjectFromJson(const char* type, const char* json, SDOM_DisplayHandle* out_handle)",
        "Creates a display object by type name from a JSON string; returns handle via out param.",
        [](const char* type, const char* json, SDOM_DisplayHandle* out_handle) -> bool {
            return CoreAPI::createDisplayObjectFromJson(type, json, out_handle);
        });

    core.registerMethod(
        typeName,
        "CreateAssetObjectFromJson",
        "bool Core::capiCreateAssetObjectFromJson(const char* type, const char* json, SDOM_AssetHandle* out_handle)",
        "bool",
        "SDOM_CreateAssetObjectFromJson",
        "bool SDOM_CreateAssetObjectFromJson(const char* type, const char* json, SDOM_AssetHandle* out_handle)",
        "Creates an asset object by type name from a JSON string; returns handle via out param.",
        [](const char* type, const char* json, SDOM_AssetHandle* out_handle) -> bool {
            return CoreAPI::createAssetObjectFromJson(type, json, out_handle);
        });

    core.registerMethod(
        typeName,
        "PollEvents",
        "bool Core::capiPollEvents(SDOM_Event* evt)",
        "bool",
        "SDOM_PollEvents",
        "bool SDOM_PollEvents(SDOM_Event* evt)",
        "Heals the previous frame if needed then pumps a single prioritized event.",
        [](SDOM_Event* evt) -> bool {
            return CoreAPI::pollEvents(evt);
        });

    core.registerMethod(
        typeName,
        "Update",
        "bool Core::capiUpdatePhase()",
        "bool",
        "SDOM_Update",
        "bool SDOM_Update(void)",
        "Manual Update phase entry point with automatic healing for prior phases.",
        []() -> bool {
            return CoreAPI::updatePhase();
        });

    core.registerMethod(
        typeName,
        "Render",
        "bool Core::capiRenderPhase()",
        "bool",
        "SDOM_Render",
        "bool SDOM_Render(void)",
        "Manual Render phase entry point; auto-runs missing prerequisites.",
        []() -> bool {
            return CoreAPI::renderPhase();
        });

    core.registerMethod(
        typeName,
        "Present",
        "bool Core::capiPresentPhase()",
        "bool",
        "SDOM_Present",
        "bool SDOM_Present(void)",
        "Manual Present phase entry point that also ensures GC.",
        []() -> bool {
            return CoreAPI::presentPhase();
        });

    core.registerMethod(
        typeName,
        "RunFrame",
        "bool Core::capiRunFrame()",
        "bool",
        "SDOM_RunFrame",
        "bool SDOM_RunFrame(void)",
        "Convenience helper that executes Update → Render → Present in order.",
        []() -> bool {
            return CoreAPI::runFramePhase();
        });

    core.registerMethod(
        typeName,
        "Run",
        "bool Core::capiRun()",
        "bool",
        "SDOM_Run",
        "bool SDOM_Run(void)",
        "Enters the Core main loop until quit or stop-after-tests is triggered.",
        []() -> bool {
            return CoreAPI::run();
        });

    core.registerMethod(
        typeName,
        "GetStopAfterUnitTests",
        "bool Core::capiGetStopAfterUnitTests()",
        "bool",
        "SDOM_GetStopAfterUnitTests",
        "bool SDOM_GetStopAfterUnitTests(void)",
        "Returns whether Core is configured to stop after unit tests.",
        []() -> bool {
            return CoreAPI::getStopAfterUnitTests();
        });

    core.registerMethod(
        typeName,
        "Quit",
        "void Core::capiQuit()",
        "void",
        "SDOM_Quit",
        "void SDOM_Quit(void)",
        "Shuts down the SDOM runtime and releases SDL resources.",
        []() {
            CoreAPI::quit();
        });

    core.registerMethod(
        typeName,
        "SetRootNode",
        "bool Core::capiSetRootNode(const SDOM_DisplayHandle* handle)",
        "bool",
        "SDOM_SetRootNode",
        "bool SDOM_SetRootNode(const SDOM_DisplayHandle* handle)",
        "Sets the active stage/root node using a DisplayHandle.",
        [](const SDOM_DisplayHandle* handle) -> bool {
            return CoreAPI::setRootNode(handle);
        });

    core.registerMethod(
        typeName,
        "SetRootNodeByName",
        "bool Core::capiSetRootNodeByName(const char* name)",
        "bool",
        "SDOM_SetRootNodeByName",
        "bool SDOM_SetRootNodeByName(const char* name)",
        "Sets the active stage/root node by object name.",
        [](const char* name) -> bool {
            return CoreAPI::setRootNodeByName(name);
        });

    core.registerMethod(
        typeName,
        "SetStage",
        "bool Core::capiSetStage(const SDOM_DisplayHandle* handle)",
        "bool",
        "SDOM_SetStage",
        "bool SDOM_SetStage(const SDOM_DisplayHandle* handle)",
        "Alias for SetRootNode using a DisplayHandle.",
        [](const SDOM_DisplayHandle* handle) -> bool {
            return CoreAPI::setStage(handle);
        });

    core.registerMethod(
        typeName,
        "SetStageByName",
        "bool Core::capiSetStageByName(const char* name)",
        "bool",
        "SDOM_SetStageByName",
        "bool SDOM_SetStageByName(const char* name)",
        "Alias for SetRootNodeByName using an object name.",
        [](const char* name) -> bool {
            return CoreAPI::setStageByName(name);
        });

    core.registerMethod(
        typeName,
        "GetRootNode",
        "bool Core::capiGetRootNode(SDOM_DisplayHandle* out_handle)",
        "bool",
        "SDOM_GetRootNode",
        "bool SDOM_GetRootNode(SDOM_DisplayHandle* out_handle)",
        "Retrieves the active stage/root node handle.",
        [](SDOM_DisplayHandle* out_handle) -> bool {
            return CoreAPI::getRootNode(out_handle);
        });

    core.registerMethod(
        typeName,
        "GetStageHandle",
        "bool Core::capiGetStageHandle(SDOM_DisplayHandle* out_handle)",
        "bool",
        "SDOM_GetStageHandle",
        "bool SDOM_GetStageHandle(SDOM_DisplayHandle* out_handle)",
        "Alias for GetRootNode; returns the active stage handle.",
        [](SDOM_DisplayHandle* out_handle) -> bool {
            return CoreAPI::getStageHandle(out_handle);
        });

    // Callback registration methods (exposed to binding generator / Lua)
    core.registerMethod(
        typeName,
        "RegisterOnInit",
        "bool Core::capiRegisterOnInit(bool (*fn)(void))",
        "bool",
        "SDOM_RegisterOnInit",
        "bool SDOM_RegisterOnInit(void* fn)",
        "Registers a C function pointer or Lua callback to run during init.",
        [](void* fnptr) -> bool {
            return CoreAPI::registerOnInit(fnptr);
        });

    core.registerMethod(
        typeName,
        "RegisterOnQuit",
        "bool Core::capiRegisterOnQuit(void (*fn)(void))",
        "bool",
        "SDOM_RegisterOnQuit",
        "bool SDOM_RegisterOnQuit(void* fn)",
        "Registers a callback invoked during shutdown.",
        [](void* fnptr) -> bool {
            return CoreAPI::registerOnQuit(fnptr);
        });

    core.registerMethod(
        typeName,
        "RegisterOnUpdate",
        "bool Core::capiRegisterOnUpdate(void (*fn)(float))",
        "bool",
        "SDOM_RegisterOnUpdate",
        "bool SDOM_RegisterOnUpdate(void* fn)",
        "Registers a per-frame update callback.",
        [](void* fnptr) -> bool {
            return CoreAPI::registerOnUpdate(fnptr);
        });

    core.registerMethod(
        typeName,
        "RegisterOnEvent",
        "bool Core::capiRegisterOnEvent(void (*fn)(const SDOM_Event*))",
        "bool",
        "SDOM_RegisterOnEvent",
        "bool SDOM_RegisterOnEvent(void* fn)",
        "Registers a listener for raw SDOM events.",
        [](void* fnptr) -> bool {
            return CoreAPI::registerOnEvent(fnptr);
        });

    core.registerMethod(
        typeName,
        "RegisterOnRender",
        "bool Core::capiRegisterOnRender(void (*fn)(void))",
        "bool",
        "SDOM_RegisterOnRender",
        "bool SDOM_RegisterOnRender(void* fn)",
        "Registers a per-frame render callback.",
        [](void* fnptr) -> bool {
            return CoreAPI::registerOnRender(fnptr);
        });

    core.registerMethod(
        typeName,
        "RegisterOnUnitTest",
        "bool Core::capiRegisterOnUnitTest(bool (*fn)(void))",
        "bool",
        "SDOM_RegisterOnUnitTest",
        "bool SDOM_RegisterOnUnitTest(void* fn)",
        "Registers a unit-test callback used by scripted tests.",
        [](void* fnptr) -> bool {
            return CoreAPI::registerOnUnitTest(fnptr);
        });

    core.registerMethod(
        typeName,
        "RegisterOnWindowResize",
        "bool Core::capiRegisterOnWindowResize(void (*fn)(int,int))",
        "bool",
        "SDOM_RegisterOnWindowResize",
        "bool SDOM_RegisterOnWindowResize(void* fn)",
        "Registers a callback for window resize notifications.",
        [](void* fnptr) -> bool {
            return CoreAPI::registerOnWindowResize(fnptr);
        });
}

} // namespace CoreAPI
} // namespace SDOM
