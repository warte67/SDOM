#include <SDOM/SDOM_CoreAPI.hpp>

#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>

#include <mutex>
#include <vector>

namespace SDOM {
namespace CoreAPI {
namespace {

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

SDOM::CAPI::CallResult makeBoolResult(bool value)
{
    return SDOM::CAPI::CallResult::FromBool(value);
}

SDOM::CAPI::CallResult makeStringResult(const std::string& value)
{
    return SDOM::CAPI::CallResult::FromString(value);
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
