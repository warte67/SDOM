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
#define SDOM_CORECONFIG_DEFAULT (SDOM_CoreConfig){ \
    .windowWidth = 800.0f, .windowHeight = 600.0f,  \
    .pixelWidth = 2.0f, .pixelHeight = 2.0f,        \
    .preserveAspectRatio = 1, .allowTextureResize = 0, \
    .rendererLogicalPresentation = SDL_LOGICAL_PRESENTATION_LETTERBOX, \
    .windowFlags = SDL_WINDOW_RESIZABLE,            \
    .pixelFormat = SDL_PIXELFORMAT_RGBA8888,        \
    .color = { 32, 32, 32, 255 }                    \
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

#ifdef __cplusplus
}
#endif
// ...existing code...


// struct SDOM_Event {
//     EventType type;                                 // Type of the event, e.g., KeyDown, MouseClick, etc.
//     DisplayHandle target = nullptr;                 // Target of the event, usually the object that triggered it    
//     DisplayHandle currentTarget = nullptr;          // Current target during event propagation
//     DisplayHandle relatedTarget = nullptr;          // For events that involve a related target (e.g., drag and drop)
//     SDL_Event sdlEvent;                             // underlying SDL event
//     Phase currentPhase;                     // Current phase of the event propagation
//     bool propagationStopped = false;        // Indicates if event propagation is stopped
//     bool disableDefaultBehavior = false;    // Indicates if default behavior is disabled
//     bool useCapture = false;                // Indicates if the event is in the capture phase
//     float fElapsedTime = 0.0f;                      // Time elapsed since the last frame

//     // Mouse Event Properties: (Not yet defined as proper JSON properties)
//     float mouse_x;              // mouse x-coordinate
//     float mouse_y;              // mouse y-coordinate
//     float wheelX = 0.0f;        // Horizontal wheel movement
//     float wheelY = 0.0f;        // Vertical wheel movement
//     float dragOffsetX = 0.0f;   // horizontal drag offset
//     float dragOffsetY = 0.0f;   // vertical drag offset
//     int clickCount = 0;         // Number of clicks for double-click detection
//     uint8_t button = 0;         // Mouse button that triggered the event (SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT, etc.)

//     // Keyboard Event Properties: (Not yet defined as proper JSON properties)
//     SDL_Scancode scancode_;     // The physical key pressed
//     SDL_Keycode keycode_;       // The virtual key pressed
//     Uint16 keymod_;             // Modifier keys (e.g., Shift, Ctrl, Alt)
//     int asciiCode_;             // ASCII code of the key pressed    
// };