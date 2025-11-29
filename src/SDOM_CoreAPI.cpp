#include <SDOM/SDOM_CoreAPI.hpp>

#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>

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

void quit()
{
    try {
        SDOM::Core::getInstance().onQuit();
    } catch (...) {
        // Best-effort cleanup
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
        "Quit",
        "void Core::capiQuit()",
        "void",
        "SDOM_Quit",
        "void SDOM_Quit(void)",
        "Shuts down the SDOM runtime and releases SDL resources.",
        []() {
            CoreAPI::quit();
        });
}

} // namespace CoreAPI
} // namespace SDOM
