// SDOM_CAPI.h
#pragma once
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDOM_CoreConfig {
    float windowWidth;
    float windowHeight;
    float pixelWidth;
    float pixelHeight;
    int   preserveAspectRatio;    /* bool as int */
    int   allowTextureResize;     /* bool as int */
    SDL_RendererLogicalPresentation rendererLogicalPresentation;
    SDL_WindowFlags                   windowFlags;
    SDL_PixelFormat                   pixelFormat;
    SDL_Color                         color;
} SDOM_CoreConfig;

/* Designated initializer macro for C users */
#define SDOM_CORECONFIG_DEFAULT  { \
    800.0f, 600.0f, \
    2.0f, 2.0f, \
    1, 0, \
    SDL_LOGICAL_PRESENTATION_LETTERBOX, \
    SDL_WINDOW_RESIZABLE, \
    SDL_PIXELFORMAT_RGBA8888, \
    { 32, 32, 32, 255 } \
}

/* Error helpers (SDL-like) */
const char* SDOM_GetError(void);              /* returns last SDOM error or SDL_GetError() */
bool SDOM_SetError(const char* fmt, ...);    /* set last SDOM error (fmt printf-style) */

/* C ABI - SDL style bool returns: true => success, false => failure */
bool SDOM_Init(void);                            /* idempotent, configures Core with defaults */
bool SDOM_Configure(const SDOM_CoreConfig* cfg); /* apply a specific config */
bool SDOM_GetCoreConfig(SDOM_CoreConfig* out_cfg); /* copy current Core config into out_cfg */

/* Convenience: set window size using SDL-style ints; returns true on success */
bool SDOM_SetWindowSize(int width, int height);
bool SDOM_GetWindowSize(int* width, int* height);

bool SDOM_SetPixelSize_f(float width, float height);
bool SDOM_GetPixelSize_f(float* width, float* height);

/* SDL-style event polling (wrapper around SDL_PollEvent).
   Returns true if an event was returned in 'event', false otherwise.
*/
bool SDOM_PollEvent(SDL_Event* event);


void SDOM_Quit(void);

/* Stable ID registry for FFI: removed — use existing Factory registration semantics */

#ifdef __cplusplus
}
#endif
