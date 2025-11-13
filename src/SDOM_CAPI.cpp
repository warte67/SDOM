// SDOM_CAPI.cpp
#include "SDOM/SDOM_CAPI.h"
#include <SDOM/SDOM_Core.hpp>
#include <iostream>
#include <mutex>
#include <cstdarg>
#include <vector>
#include <string>

static std::string sdom_last_error;
static std::mutex  sdom_error_mutex;

extern "C" const char* SDOM_GetError(void)
{
    std::lock_guard<std::mutex> lk(sdom_error_mutex);
    if (!sdom_last_error.empty()) {
        return sdom_last_error.c_str();
    }
    return SDL_GetError();
}


static SDOM::Core::CoreConfig convert_coreconfig(const SDOM_CoreConfig* c)
{
    SDOM::Core::CoreConfig out;
    if (!c) return out; // leave C++ defaults
    out.windowWidth = c->windowWidth;
    out.windowHeight = c->windowHeight;
    out.pixelWidth = c->pixelWidth;
    out.pixelHeight = c->pixelHeight;
    out.preserveAspectRatio = (c->preserveAspectRatio != 0);
    out.allowTextureResize = (c->allowTextureResize != 0);
    out.rendererLogicalPresentation = c->rendererLogicalPresentation;
    out.windowFlags = c->windowFlags;
    out.pixelFormat = c->pixelFormat;
    out.color = c->color;
    return out;
}

extern "C" bool SDOM_Init(void)
{
    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        if (core.getWindow() || core.getRenderer()) return true; // idempotent
        SDOM_CoreConfig def = SDOM_CORECONFIG_DEFAULT;
        SDOM::Core::CoreConfig cc = convert_coreconfig(&def);
        core.configure(cc);
        return true;
    } catch (const SDOM::Exception& e) {
        SDL_SetError("SDOM_Init error: %s", e.what());
        return false;
    } catch (const std::exception& e) {
        SDL_SetError("SDOM_Init std::exception: %s", e.what());
        return false;
    } catch (...) {
        SDL_SetError("SDOM_Init unknown error");
        return false;
    }
}

extern "C" bool SDOM_Configure(const SDOM_CoreConfig* cfg)
{
    if (!cfg) {
        SDL_SetError("SDOM_Configure: null cfg");
        return false;
    }
    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        SDOM::Core::CoreConfig cc = convert_coreconfig(cfg);
        core.configure(cc);
        return true;
    } catch (const SDOM::Exception& e) {
        SDL_SetError("SDOM_Configure error: %s", e.what());
        return false;
    } catch (const std::exception& e) {
        SDL_SetError("SDOM_Configure std::exception: %s", e.what());
        return false;
    } catch (...) {
        SDL_SetError("SDOM_Configure unknown error");
        return false;
    }
}

extern "C" bool SDOM_GetCoreConfig(SDOM_CoreConfig* out_cfg)
{
    if (!out_cfg) {
        SDL_SetError("SDOM_GetCoreConfig: out_cfg is null");
        return false;
    }
    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        SDOM::Core::CoreConfig cc = core.getConfig(); // copy
        out_cfg->windowWidth = cc.windowWidth;
        out_cfg->windowHeight = cc.windowHeight;
        out_cfg->pixelWidth = cc.pixelWidth;
        out_cfg->pixelHeight = cc.pixelHeight;
        out_cfg->preserveAspectRatio = cc.preserveAspectRatio ? 1 : 0;
        out_cfg->allowTextureResize = cc.allowTextureResize ? 1 : 0;
        out_cfg->rendererLogicalPresentation = cc.rendererLogicalPresentation;
        out_cfg->windowFlags = cc.windowFlags;
        out_cfg->pixelFormat = cc.pixelFormat;
        out_cfg->color = cc.color;
        return true;
    } catch (const SDOM::Exception& e) {
        SDL_SetError("SDOM_GetCoreConfig error: %s", e.what());
        return false;
    } catch (const std::exception& e) {
        SDL_SetError("SDOM_GetCoreConfig std::exception: %s", e.what());
        return false;
    } catch (...) {
        SDL_SetError("SDOM_GetCoreConfig unknown error");
        return false;
    }
}

extern "C" bool SDOM_SetWindowSize(int width, int height)
{
    if (width <= 0 || height <= 0) {
        SDL_SetError("SDOM_SetWindowSize: invalid size %d x %d", width, height);
        return false;
    }

    SDOM_CoreConfig cfg;
    if (!SDOM_GetCoreConfig(&cfg)) return false;
    cfg.windowWidth  = (float)width;
    cfg.windowHeight = (float)height;
    return SDOM_Configure(&cfg);
}

extern "C" bool SDOM_GetWindowSize(int* width, int* height)
{
    if (!width || !height) {
        SDL_SetError("SDOM_GetWindowSize: output pointers must not be NULL");
        return false;
    }

    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        *width  = static_cast<int>(core.getWindowWidth());
        *height = static_cast<int>(core.getWindowHeight());
        return true;
    } catch (const SDOM::Exception& e) {
        SDL_SetError("SDOM_GetWindowSize error: %s", e.what());
        return false;
    } catch (const std::exception& e) {
        SDL_SetError("SDOM_GetWindowSize std::exception: %s", e.what());
        return false;
    } catch (...) {
        SDL_SetError("SDOM_GetWindowSize unknown error");
        return false;
    }
}


extern "C" bool SDOM_SetPixelSize_f(float width, float height)
{
    if (width <= 0.0f || height <= 0.0f) {
        SDL_SetError("SDOM_SetPixelSize: invalid size %f x %f", width, height);
        return false;
    }

    SDOM_CoreConfig cfg;
    if (!SDOM_GetCoreConfig(&cfg)) return false;
    cfg.pixelWidth  = width;
    cfg.pixelHeight = height;
    return SDOM_Configure(&cfg);
}

extern "C" bool SDOM_GetPixelSize_f(float* width, float* height)
{
    if (!width || !height) {
        SDL_SetError("SDOM_GetPixelSize: output pointers must not be NULL");
        return false;
    }

    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        *width  = core.getPixelWidth();
        *height = core.getPixelHeight();
        return true;
    } catch (const SDOM::Exception& e) {
        SDL_SetError("SDOM_GetPixelSize error: %s", e.what());
        return false;
    } catch (const std::exception& e) {
        SDL_SetError("SDOM_GetPixelSize std::exception: %s", e.what());
        return false;
    } catch (...) {
        SDL_SetError("SDOM_GetPixelSize unknown error");
        return false;
    }
}

extern "C" void SDOM_Quit(void)
{
    try {
        SDOM::Core::getInstance().onQuit();
    } catch (...) { /* best-effort cleanup */ }
}


