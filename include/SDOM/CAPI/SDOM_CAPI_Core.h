#pragma once
// Auto-generated SDOM C API module: Core

#include <stdbool.h>
#include <SDL3/SDL.h>


typedef struct SDOM_CoreConfig {
    float windowWidth;
    float windowHeight;
    float pixelWidth;
    float pixelHeight;
    int preserveAspectRatio;
    int allowTextureResize;
    SDL_RendererLogicalPresentation rendererLogicalPresentation;
    SDL_WindowFlags windowFlags;
    SDL_PixelFormat pixelFormat;
    SDL_Color color;
} SDOM_CoreConfig;

#define SDOM_CORECONFIG_DEFAULT  { \
    800.0f, 600.0f, \
    2.0f, 2.0f, \
    1, 0, \
    SDL_LOGICAL_PRESENTATION_LETTERBOX, \
    SDL_WINDOW_RESIZABLE, \
    SDL_PIXELFORMAT_RGBA8888, \
    { 32, 32, 32, 255 } \
}

#ifdef __cplusplus
extern "C" {
#endif

// Callable bindings
const char* SDOM_GetError(void);
bool SDOM_SetError(const char* message);
bool SDOM_Init(uint64_t init_flags);
bool SDOM_Configure(const SDOM_CoreConfig* cfg);
bool SDOM_GetCoreConfig(SDOM_CoreConfig* out_cfg);
void SDOM_Quit(void);

#ifdef __cplusplus
} // extern "C"
#endif
