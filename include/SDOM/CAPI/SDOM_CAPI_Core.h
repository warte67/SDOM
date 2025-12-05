// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_Core.h
//  Module: Core
//
//  Brief:
//    The Core C API provides external applications a stable interface to
//    initialize and control the SDOM engine without requiring direct access to
//    C++ internals. It exposes configuration controls for window sizing,
//    renderer behavior, and pixel scaling while also managing the main
//    execution phases of a frame in a safe, state-correct manner. Through this
//    module, programs can drive event polling, DOM updates, rendering, and
//    presenting just like SDL, but with SDOM’s enhanced guarantees around
//    ordering and implicit phase healing. It is the lowest-level entry point
//    for embedding SDOM inside host applications or scripting environments, and
//    serves as the foundation for all generated bindings in other languages
//    such as Lua and (future) Rust.
// =============================================================================
//
//  Authors:
//    Jay Faries (warte67) - Primary architect of SDOM
//
//  File Type: Header
//  SDOM Version: 0.5.263 (early pre-alpha)
//  Build Identifier: 2025-12-04_21:24:47_20e8cb42
//  Commit: 20e8cb42 on branch master
//  Compiler: g++ (GCC) 15.2.1 20251112
//  Platform: Linux-x86_64
//  Generated: 2025-12-04_21:24:47
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

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDOM/CAPI/SDOM_CAPI_Handles.h>
#include <SDOM/CAPI/SDOM_CAPI_Event.h>


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

/**
 * @brief Returns the most recent SDOM error string (or SDL_GetError when none).
 *
 * C++:   std::string Core::capiGetError() const
 * C API: const char* SDOM_GetError(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetError(void);

/**
 * @brief Sets the global SDOM error string consumers read via SDOM_GetError().
 *
 * C++:   bool Core::capiSetError(const char* message)
 * C API: bool SDOM_SetError(const char* message)
 *
 * @param message Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetError(const char* message);

/**
 * @brief Initializes the SDOM Core singleton using default configuration values.
 *
 * C++:   bool Core::capiInit(std::uint64_t flags)
 * C API: bool SDOM_Init(uint64_t init_flags)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Init(uint64_t init_flags);

/**
 * @brief Applies the supplied SDOM_CoreConfig to the Core singleton.
 *
 * C++:   bool Core::capiConfigure(const SDOM_CoreConfig* cfg)
 * C API: bool SDOM_Configure(const SDOM_CoreConfig* cfg)
 *
 * @param cfg Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Configure(const SDOM_CoreConfig* cfg);

/**
 * @brief Parses a JSON string into a CoreConfig and applies it.
 *
 * C++:   bool Core::capiConfigureFromJson(const char* json)
 * C API: bool SDOM_ConfigureFromJson(const char* json)
 *
 * @param json Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_ConfigureFromJson(const char* json);

/**
 * @brief Copies the active Core configuration into out_cfg.
 *
 * C++:   bool Core::capiGetCoreConfig(SDOM_CoreConfig* out_cfg)
 * C API: bool SDOM_GetCoreConfig(SDOM_CoreConfig* out_cfg)
 *
 * @param out_cfg Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetCoreConfig(SDOM_CoreConfig* out_cfg);

/**
 * @brief Instructs the Core main loop to exit automatically after unit tests complete.
 *
 * C++:   bool Core::capiSetStopAfterUnitTests(bool stop)
 * C API: bool SDOM_SetStopAfterUnitTests(bool stop)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetStopAfterUnitTests(bool stop);

/**
 * @brief Start or stop the Core main loop without invoking a full shutdown; accepts true to run, false to pause.
 *
 * C++:   bool Core::capiSetIsRunning(bool running)
 * C API: bool SDOM_SetIsRunning(bool running)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetIsRunning(bool running);

/**
 * @brief Loads Core configuration, resources, and DOM hierarchy from the provided JSON document.
 *
 * C++:   bool Core::capiLoadDomFromJsonFile(const char* filename)
 * C API: bool SDOM_LoadDomFromJsonFile(const char* filename)
 *
 * @param filename Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_LoadDomFromJsonFile(const char* filename);

/**
 * @brief Loads Core configuration, resources, and DOM hierarchy from an in-memory JSON string.
 *
 * C++:   bool Core::capiLoadDomFromJson(const char* json)
 * C API: bool SDOM_LoadDomFromJson(const char* json)
 *
 * @param json Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_LoadDomFromJson(const char* json);

/**
 * @brief Retrieves the current frame border color.
 *
 * C++:   bool Core::capiGetBorderColor(SDL_Color* out_color)
 * C API: bool SDOM_GetBorderColor(SDL_Color* out_color)
 *
 * @param out_color Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetBorderColor(SDL_Color* out_color);

/**
 * @brief Sets the frame border color and applies it.
 *
 * C++:   bool Core::capiSetBorderColor(const SDL_Color* color)
 * C API: bool SDOM_SetBorderColor(const SDL_Color* color)
 *
 * @param color Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetBorderColor(const SDL_Color* color);

/**
 * @brief Returns the cached window title (borrowed string).
 *
 * C++:   const char* Core::capiGetWindowTitle() const
 * C API: const char* SDOM_GetWindowTitle(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetWindowTitle(void);

/**
 * @brief Sets the window title; applies immediately when window is live.
 *
 * C++:   bool Core::capiSetWindowTitle(const char* title)
 * C API: bool SDOM_SetWindowTitle(const char* title)
 *
 * @param title Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetWindowTitle(const char* title);

/**
 * @brief Pumps SDL events one iteration for headless/deterministic testing.
 *
 * C++:   bool Core::capiPumpEventsOnce()
 * C API: bool SDOM_PumpEventsOnce(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_PumpEventsOnce(void);

/**
 * @brief Queues a synthetic mouse event from a JSON description.
 *
 * C++:   bool Core::capiPushMouseEvent(const char* json)
 * C API: bool SDOM_PushMouseEvent(const char* json)
 *
 * @param json Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_PushMouseEvent(const char* json);

/**
 * @brief Queues a synthetic keyboard event from a JSON description.
 *
 * C++:   bool Core::capiPushKeyboardEvent(const char* json)
 * C API: bool SDOM_PushKeyboardEvent(const char* json)
 *
 * @param json Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_PushKeyboardEvent(const char* json);

/**
 * @brief Returns true when Lua support is compiled in and available.
 *
 * C++:   bool Core::capiHasLuaSupport() const
 * C API: bool SDOM_HasLuaSupport(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_HasLuaSupport(void);

/**
 * @brief Returns semantic version string.
 *
 * C++:   const char* Core::capiGetVersionString() const
 * C API: const char* SDOM_GetVersionString(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionString(void);

/**
 * @brief Returns full version string including build metadata.
 *
 * C++:   const char* Core::capiGetVersionFullString() const
 * C API: const char* SDOM_GetVersionFullString(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionFullString(void);

/**
 * @brief Returns major version component.
 *
 * C++:   int Core::capiGetVersionMajor() const
 * C API: int SDOM_GetVersionMajor(void)
 *
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetVersionMajor(void);

/**
 * @brief Returns minor version component.
 *
 * C++:   int Core::capiGetVersionMinor() const
 * C API: int SDOM_GetVersionMinor(void)
 *
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetVersionMinor(void);

/**
 * @brief Returns patch version component.
 *
 * C++:   int Core::capiGetVersionPatch() const
 * C API: int SDOM_GetVersionPatch(void)
 *
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetVersionPatch(void);

/**
 * @brief Returns codename string.
 *
 * C++:   const char* Core::capiGetVersionCodename() const
 * C API: const char* SDOM_GetVersionCodename(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionCodename(void);

/**
 * @brief Returns build identifier.
 *
 * C++:   const char* Core::capiGetVersionBuild() const
 * C API: const char* SDOM_GetVersionBuild(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionBuild(void);

/**
 * @brief Returns build date/time string.
 *
 * C++:   const char* Core::capiGetVersionBuildDate() const
 * C API: const char* SDOM_GetVersionBuildDate(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionBuildDate(void);

/**
 * @brief Returns commit hash.
 *
 * C++:   const char* Core::capiGetVersionCommit() const
 * C API: const char* SDOM_GetVersionCommit(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionCommit(void);

/**
 * @brief Returns branch name.
 *
 * C++:   const char* Core::capiGetVersionBranch() const
 * C API: const char* SDOM_GetVersionBranch(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionBranch(void);

/**
 * @brief Returns compiler string.
 *
 * C++:   const char* Core::capiGetVersionCompiler() const
 * C API: const char* SDOM_GetVersionCompiler(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionCompiler(void);

/**
 * @brief Returns platform description.
 *
 * C++:   const char* Core::capiGetVersionPlatform() const
 * C API: const char* SDOM_GetVersionPlatform(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionPlatform(void);

/**
 * @brief Returns the last frame delta time in seconds.
 *
 * C++:   float Core::capiGetElapsedTime() const
 * C API: float SDOM_GetElapsedTime(void)
 *
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetElapsedTime(void);

/**
 * @brief Returns the configured window width.
 *
 * C++:   float Core::capiGetWindowWidth() const
 * C API: float SDOM_GetWindowWidth(void)
 *
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetWindowWidth(void);

/**
 * @brief Returns the configured window height.
 *
 * C++:   float Core::capiGetWindowHeight() const
 * C API: float SDOM_GetWindowHeight(void)
 *
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetWindowHeight(void);

/**
 * @brief Returns the configured pixel width.
 *
 * C++:   float Core::capiGetPixelWidth() const
 * C API: float SDOM_GetPixelWidth(void)
 *
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetPixelWidth(void);

/**
 * @brief Returns the configured pixel height.
 *
 * C++:   float Core::capiGetPixelHeight() const
 * C API: float SDOM_GetPixelHeight(void)
 *
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetPixelHeight(void);

/**
 * @brief Returns whether aspect ratio preservation is enabled.
 *
 * C++:   bool Core::capiGetPreserveAspectRatio() const
 * C API: bool SDOM_GetPreserveAspectRatio(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetPreserveAspectRatio(void);

/**
 * @brief Returns whether texture resize is allowed.
 *
 * C++:   bool Core::capiGetAllowTextureResize() const
 * C API: bool SDOM_GetAllowTextureResize(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetAllowTextureResize(void);

/**
 * @brief Returns the logical presentation mode.
 *
 * C++:   SDL_RendererLogicalPresentation Core::capiGetLogicalPresentation() const
 * C API: SDL_RendererLogicalPresentation SDOM_GetLogicalPresentation(void)
 *
 * @return SDL_RendererLogicalPresentation; check SDOM_GetError() for details on failure.
 */
SDL_RendererLogicalPresentation SDOM_GetLogicalPresentation(void);

/**
 * @brief Returns current window flags.
 *
 * C++:   SDL_WindowFlags Core::capiGetWindowFlags() const
 * C API: SDL_WindowFlags SDOM_GetWindowFlags(void)
 *
 * @return SDL_WindowFlags; check SDOM_GetError() for details on failure.
 */
SDL_WindowFlags SDOM_GetWindowFlags(void);

/**
 * @brief Returns the pixel format.
 *
 * C++:   SDL_PixelFormat Core::capiGetPixelFormat() const
 * C API: SDL_PixelFormat SDOM_GetPixelFormat(void)
 *
 * @return SDL_PixelFormat; check SDOM_GetError() for details on failure.
 */
SDL_PixelFormat SDOM_GetPixelFormat(void);

/**
 * @brief Returns true if Core is in fullscreen mode.
 *
 * C++:   bool Core::capiIsFullscreen() const
 * C API: bool SDOM_IsFullscreen(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_IsFullscreen(void);

/**
 * @brief Returns true if Core is windowed.
 *
 * C++:   bool Core::capiIsWindowed() const
 * C API: bool SDOM_IsWindowed(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_IsWindowed(void);

/**
 * @brief Returns true while the DOM is actively being traversed.
 *
 * C++:   bool Core::capiGetIsTraversing() const
 * C API: bool SDOM_GetIsTraversing(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetIsTraversing(void);

/**
 * @brief Moves focus forward (testing/editor only).
 *
 * C++:   bool Core::capiHandleTabKeyPress()
 * C API: bool SDOM_HandleTabKeyPress(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_HandleTabKeyPress(void);

/**
 * @brief Moves focus backward (testing/editor only).
 *
 * C++:   bool Core::capiHandleTabKeyPressReverse()
 * C API: bool SDOM_HandleTabKeyPressReverse(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_HandleTabKeyPressReverse(void);

/**
 * @brief Borrows the SDL_Window owned by Core; do not destroy it.
 *
 * C++:   bool Core::capiGetWindow(SDL_Window** out_window)
 * C API: bool SDOM_GetWindow(SDL_Window** out_window)
 *
 * @param out_window Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetWindow(SDL_Window** out_window);

/**
 * @brief Borrows the SDL_Renderer owned by Core; do not destroy it.
 *
 * C++:   bool Core::capiGetRenderer(SDL_Renderer** out_renderer)
 * C API: bool SDOM_GetRenderer(SDL_Renderer** out_renderer)
 *
 * @param out_renderer Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetRenderer(SDL_Renderer** out_renderer);

/**
 * @brief Borrows the SDL_Texture owned by Core; do not destroy it.
 *
 * C++:   bool Core::capiGetTexture(SDL_Texture** out_texture)
 * C API: bool SDOM_GetTexture(SDL_Texture** out_texture)
 *
 * @param out_texture Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetTexture(SDL_Texture** out_texture);

/**
 * @brief Sets window width; may trigger reconfigure.
 *
 * C++:   bool Core::capiSetWindowWidth(float width)
 * C API: bool SDOM_SetWindowWidth(float width)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetWindowWidth(float width);

/**
 * @brief Sets window height; may trigger reconfigure.
 *
 * C++:   bool Core::capiSetWindowHeight(float height)
 * C API: bool SDOM_SetWindowHeight(float height)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetWindowHeight(float height);

/**
 * @brief Sets pixel width.
 *
 * C++:   bool Core::capiSetPixelWidth(float width)
 * C API: bool SDOM_SetPixelWidth(float width)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetPixelWidth(float width);

/**
 * @brief Sets pixel height.
 *
 * C++:   bool Core::capiSetPixelHeight(float height)
 * C API: bool SDOM_SetPixelHeight(float height)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetPixelHeight(float height);

/**
 * @brief Sets aspect ratio preservation.
 *
 * C++:   bool Core::capiSetPreserveAspectRatio(bool preserve)
 * C API: bool SDOM_SetPreserveAspectRatio(bool preserve)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetPreserveAspectRatio(bool preserve);

/**
 * @brief Sets whether texture resize is allowed.
 *
 * C++:   bool Core::capiSetAllowTextureResize(bool allow)
 * C API: bool SDOM_SetAllowTextureResize(bool allow)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetAllowTextureResize(bool allow);

/**
 * @brief Sets logical presentation mode.
 *
 * C++:   bool Core::capiSetLogicalPresentation(SDL_RendererLogicalPresentation presentation)
 * C API: bool SDOM_SetLogicalPresentation(SDL_RendererLogicalPresentation presentation)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetLogicalPresentation(SDL_RendererLogicalPresentation presentation);

/**
 * @brief Sets window flags.
 *
 * C++:   bool Core::capiSetWindowFlags(SDL_WindowFlags flags)
 * C API: bool SDOM_SetWindowFlags(SDL_WindowFlags flags)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetWindowFlags(SDL_WindowFlags flags);

/**
 * @brief Sets pixel format.
 *
 * C++:   bool Core::capiSetPixelFormat(SDL_PixelFormat fmt)
 * C API: bool SDOM_SetPixelFormat(SDL_PixelFormat fmt)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetPixelFormat(SDL_PixelFormat fmt);

/**
 * @brief Toggles fullscreen.
 *
 * C++:   bool Core::capiSetFullscreen(bool fullscreen)
 * C API: bool SDOM_SetFullscreen(bool fullscreen)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetFullscreen(bool fullscreen);

/**
 * @brief Toggles windowed mode.
 *
 * C++:   bool Core::capiSetWindowed(bool windowed)
 * C API: bool SDOM_SetWindowed(bool windowed)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetWindowed(bool windowed);

/**
 * @brief Sets traversal flag; primarily for tests/tooling.
 *
 * C++:   bool Core::capiSetIsTraversing(bool traversing)
 * C API: bool SDOM_SetIsTraversing(bool traversing)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetIsTraversing(bool traversing);

/**
 * @brief Directly sets keyboard focus (testing/editor only).
 *
 * C++:   bool Core::capiSetKeyboardFocus(const SDOM_DisplayHandle* handle)
 * C API: bool SDOM_SetKeyboardFocus(const SDOM_DisplayHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetKeyboardFocus(const SDOM_DisplayHandle* handle);

/**
 * @brief Retrieves the current keyboard focus handle.
 *
 * C++:   bool Core::capiGetKeyboardFocus(SDOM_DisplayHandle* out_handle)
 * C API: bool SDOM_GetKeyboardFocus(SDOM_DisplayHandle* out_handle)
 *
 * @param out_handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetKeyboardFocus(SDOM_DisplayHandle* out_handle);

/**
 * @brief Clears keyboard focus.
 *
 * C++:   bool Core::capiClearKeyboardFocus()
 * C API: bool SDOM_ClearKeyboardFocus(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_ClearKeyboardFocus(void);

/**
 * @brief Directly sets mouse hover (testing/editor only).
 *
 * C++:   bool Core::capiSetMouseHover(const SDOM_DisplayHandle* handle)
 * C API: bool SDOM_SetMouseHover(const SDOM_DisplayHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetMouseHover(const SDOM_DisplayHandle* handle);

/**
 * @brief Retrieves the current mouse hover handle.
 *
 * C++:   bool Core::capiGetMouseHover(SDOM_DisplayHandle* out_handle)
 * C API: bool SDOM_GetMouseHover(SDOM_DisplayHandle* out_handle)
 *
 * @param out_handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetMouseHover(SDOM_DisplayHandle* out_handle);

/**
 * @brief Clears mouse hover.
 *
 * C++:   bool Core::capiClearMouseHover()
 * C API: bool SDOM_ClearMouseHover(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_ClearMouseHover(void);

/**
 * @brief Returns the current frame count (testing/tooling).
 *
 * C++:   int Core::capiGetFrameCount() const
 * C API: int SDOM_GetFrameCount(void)
 *
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetFrameCount(void);

/**
 * @brief Lookup a display object by name and return its handle.
 *
 * C++:   bool Core::capiGetDisplayObject(const char* name, SDOM_DisplayHandle* out_handle)
 * C API: bool SDOM_GetDisplayObject(const char* name, SDOM_DisplayHandle* out_handle)
 *
 * @param name Pointer parameter.
 * @param out_handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetDisplayObject(const char* name, SDOM_DisplayHandle* out_handle);

/**
 * @brief Returns true if a display object by that name exists.
 *
 * C++:   bool Core::capiHasDisplayObject(const char* name) const
 * C API: bool SDOM_HasDisplayObject(const char* name)
 *
 * @param name Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_HasDisplayObject(const char* name);

/**
 * @brief Destroys a display object by name.
 *
 * C++:   bool Core::capiDestroyDisplayObjectByName(const char* name)
 * C API: bool SDOM_DestroyDisplayObjectByName(const char* name)
 *
 * @param name Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_DestroyDisplayObjectByName(const char* name);

/**
 * @brief Destroys a display object by handle.
 *
 * C++:   bool Core::capiDestroyDisplayObject(const SDOM_DisplayHandle* handle)
 * C API: bool SDOM_DestroyDisplayObject(const SDOM_DisplayHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_DestroyDisplayObject(const SDOM_DisplayHandle* handle);

/**
 * @brief Lookup an asset by name and return its handle.
 *
 * C++:   bool Core::capiGetAssetObject(const char* name, SDOM_AssetHandle* out_handle)
 * C API: bool SDOM_GetAssetObject(const char* name, SDOM_AssetHandle* out_handle)
 *
 * @param name Pointer parameter.
 * @param out_handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetAssetObject(const char* name, SDOM_AssetHandle* out_handle);

/**
 * @brief Returns true if an asset by that name exists.
 *
 * C++:   bool Core::capiHasAssetObject(const char* name) const
 * C API: bool SDOM_HasAssetObject(const char* name)
 *
 * @param name Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_HasAssetObject(const char* name);

/**
 * @brief Destroys an asset by name.
 *
 * C++:   bool Core::capiDestroyAssetObjectByName(const char* name)
 * C API: bool SDOM_DestroyAssetObjectByName(const char* name)
 *
 * @param name Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_DestroyAssetObjectByName(const char* name);

/**
 * @brief Destroys an asset by handle.
 *
 * C++:   bool Core::capiDestroyAssetObject(const SDOM_AssetHandle* handle)
 * C API: bool SDOM_DestroyAssetObject(const SDOM_AssetHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_DestroyAssetObject(const SDOM_AssetHandle* handle);

/**
 * @brief Counts orphaned display objects (testing/editor).
 *
 * C++:   int Core::capiCountOrphans() const
 * C API: int SDOM_CountOrphans(void)
 *
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_CountOrphans(void);

/**
 * @brief Returns orphaned display handles up to max_count; testing/editor only.
 *
 * C++:   bool Core::capiGetOrphans(SDOM_DisplayHandle* out_handles, int* out_count, int max_count)
 * C API: bool SDOM_GetOrphans(SDOM_DisplayHandle* out_handles, int* out_count, int max_count)
 *
 * @param out_handles Pointer parameter.
 * @param out_count Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetOrphans(SDOM_DisplayHandle* out_handles, int* out_count, int max_count);

/**
 * @brief Detaches orphaned display objects (testing/editor).
 *
 * C++:   bool Core::capiDetachOrphans()
 * C API: bool SDOM_DetachOrphans(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_DetachOrphans(void);

/**
 * @brief Collects unreachable objects.
 *
 * C++:   bool Core::capiCollectGarbage()
 * C API: bool SDOM_CollectGarbage(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_CollectGarbage(void);

/**
 * @brief Attaches deferred children (testing/editor).
 *
 * C++:   bool Core::capiAttachFutureChildren()
 * C API: bool SDOM_AttachFutureChildren(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_AttachFutureChildren(void);

/**
 * @brief Adds an orphan handle (testing/editor).
 *
 * C++:   bool Core::capiAddOrphan(const SDOM_DisplayHandle* orphan)
 * C API: bool SDOM_AddOrphan(const SDOM_DisplayHandle* orphan)
 *
 * @param orphan Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_AddOrphan(const SDOM_DisplayHandle* orphan);

/**
 * @brief Queues a future child relationship (testing/editor).
 *
 * C++:   bool Core::capiAddFutureChild(const SDOM_DisplayHandle* child, const SDOM_DisplayHandle* parent, bool useWorld, int worldX, int worldY)
 * C API: bool SDOM_AddFutureChild(const SDOM_DisplayHandle* child, const SDOM_DisplayHandle* parent, bool useWorld, int worldX, int worldY)
 *
 * @param child Pointer parameter.
 * @param parent Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_AddFutureChild(const SDOM_DisplayHandle* child, const SDOM_DisplayHandle* parent, bool useWorld, int worldX, int worldY);

/**
 * @brief Returns display object names up to max_count; null-terminates when space permits.
 *
 * C++:   bool Core::capiGetDisplayObjectNames(const char** out_names, int* out_count, int max_count)
 * C API: bool SDOM_GetDisplayObjectNames(const char** out_names, int* out_count, int max_count)
 *
 * @param out_names Pointer parameter.
 * @param out_count Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetDisplayObjectNames(const char** out_names, int* out_count, int max_count);

/**
 * @brief Clears factory registrations (hot-reload/test).
 *
 * C++:   bool Core::capiClearFactory()
 * C API: bool SDOM_ClearFactory(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_ClearFactory(void);

/**
 * @brief Prints registered objects (debug).
 *
 * C++:   bool Core::capiPrintObjectRegistry() const
 * C API: bool SDOM_PrintObjectRegistry(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_PrintObjectRegistry(void);

/**
 * @brief Returns property names for a type (currently stubbed).
 *
 * C++:   bool Core::capiGetPropertyNamesForType(const char* type, const char** out_names, int* out_count, int max_count) const
 * C API: bool SDOM_GetPropertyNamesForType(const char* type, const char** out_names, int* out_count, int max_count)
 *
 * @param type Pointer parameter.
 * @param out_names Pointer parameter.
 * @param out_count Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetPropertyNamesForType(const char* type, const char** out_names, int* out_count, int max_count);

/**
 * @brief Returns command names for a type (currently stubbed).
 *
 * C++:   bool Core::capiGetCommandNamesForType(const char* type, const char** out_names, int* out_count, int max_count) const
 * C API: bool SDOM_GetCommandNamesForType(const char* type, const char** out_names, int* out_count, int max_count)
 *
 * @param type Pointer parameter.
 * @param out_names Pointer parameter.
 * @param out_count Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetCommandNamesForType(const char* type, const char** out_names, int* out_count, int max_count);

/**
 * @brief Returns function names for a type (currently stubbed).
 *
 * C++:   bool Core::capiGetFunctionNamesForType(const char* type, const char** out_names, int* out_count, int max_count) const
 * C API: bool SDOM_GetFunctionNamesForType(const char* type, const char** out_names, int* out_count, int max_count)
 *
 * @param type Pointer parameter.
 * @param out_names Pointer parameter.
 * @param out_count Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetFunctionNamesForType(const char* type, const char** out_names, int* out_count, int max_count);

/**
 * @brief Testing: ignore real input while processing synthetic events.
 *
 * C++:   bool Core::capiSetIgnoreRealInput(bool ignore)
 * C API: bool SDOM_SetIgnoreRealInput(bool ignore)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetIgnoreRealInput(bool ignore);

/**
 * @brief Testing: read ignore-real-input flag.
 *
 * C++:   bool Core::capiGetIgnoreRealInput(bool* out_ignore) const
 * C API: bool SDOM_GetIgnoreRealInput(bool* out_ignore)
 *
 * @param out_ignore Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetIgnoreRealInput(bool* out_ignore);

/**
 * @brief Testing: retrieve keyfocus gray level.
 *
 * C++:   bool Core::capiGetKeyfocusGray(float* out_gray) const
 * C API: bool SDOM_GetKeyfocusGray(float* out_gray)
 *
 * @param out_gray Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetKeyfocusGray(float* out_gray);

/**
 * @brief Testing: set keyfocus gray level.
 *
 * C++:   bool Core::capiSetKeyfocusGray(float gray)
 * C API: bool SDOM_SetKeyfocusGray(float gray)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetKeyfocusGray(float gray);

/**
 * @brief Creates a display object by type name from a JSON string; returns handle via out param.
 *
 * C++:   bool Core::capiCreateDisplayObjectFromJson(const char* type, const char* json, SDOM_DisplayHandle* out_handle)
 * C API: bool SDOM_CreateDisplayObjectFromJson(const char* type, const char* json, SDOM_DisplayHandle* out_handle)
 *
 * @param type Pointer parameter.
 * @param json Pointer parameter.
 * @param out_handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_CreateDisplayObjectFromJson(const char* type, const char* json, SDOM_DisplayHandle* out_handle);

/**
 * @brief Creates an asset object by type name from a JSON string; returns handle via out param.
 *
 * C++:   bool Core::capiCreateAssetObjectFromJson(const char* type, const char* json, SDOM_AssetHandle* out_handle)
 * C API: bool SDOM_CreateAssetObjectFromJson(const char* type, const char* json, SDOM_AssetHandle* out_handle)
 *
 * @param type Pointer parameter.
 * @param json Pointer parameter.
 * @param out_handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_CreateAssetObjectFromJson(const char* type, const char* json, SDOM_AssetHandle* out_handle);

/**
 * @brief Heals the previous frame if needed then pumps a single prioritized event.
 *
 * C++:   bool Core::capiPollEvents(SDOM_Event* evt)
 * C API: bool SDOM_PollEvents(SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_PollEvents(SDOM_Event* evt);

/**
 * @brief Manual Update phase entry point with automatic healing for prior phases.
 *
 * C++:   bool Core::capiUpdatePhase()
 * C API: bool SDOM_Update(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Update(void);

/**
 * @brief Manual Render phase entry point; auto-runs missing prerequisites.
 *
 * C++:   bool Core::capiRenderPhase()
 * C API: bool SDOM_Render(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Render(void);

/**
 * @brief Manual Present phase entry point that also ensures GC.
 *
 * C++:   bool Core::capiPresentPhase()
 * C API: bool SDOM_Present(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Present(void);

/**
 * @brief Convenience helper that executes Update → Render → Present in order.
 *
 * C++:   bool Core::capiRunFrame()
 * C API: bool SDOM_RunFrame(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_RunFrame(void);

/**
 * @brief Enters the Core main loop until quit or stop-after-tests is triggered.
 *
 * C++:   bool Core::capiRun()
 * C API: bool SDOM_Run(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_Run(void);

/**
 * @brief Returns whether Core is configured to stop after unit tests.
 *
 * C++:   bool Core::capiGetStopAfterUnitTests()
 * C API: bool SDOM_GetStopAfterUnitTests(void)
 *
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetStopAfterUnitTests(void);

/**
 * @brief Shuts down the SDOM runtime and releases SDL resources.
 *
 * C++:   void Core::capiQuit()
 * C API: void SDOM_Quit(void)
 *
 * @return void; check SDOM_GetError() for details on failure.
 */
void SDOM_Quit(void);

/**
 * @brief Sets the active stage/root node using a DisplayHandle.
 *
 * C++:   bool Core::capiSetRootNode(const SDOM_DisplayHandle* handle)
 * C API: bool SDOM_SetRootNode(const SDOM_DisplayHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetRootNode(const SDOM_DisplayHandle* handle);

/**
 * @brief Sets the active stage/root node by object name.
 *
 * C++:   bool Core::capiSetRootNodeByName(const char* name)
 * C API: bool SDOM_SetRootNodeByName(const char* name)
 *
 * @param name Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetRootNodeByName(const char* name);

/**
 * @brief Alias for SetRootNode using a DisplayHandle.
 *
 * C++:   bool Core::capiSetStage(const SDOM_DisplayHandle* handle)
 * C API: bool SDOM_SetStage(const SDOM_DisplayHandle* handle)
 *
 * @param handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetStage(const SDOM_DisplayHandle* handle);

/**
 * @brief Alias for SetRootNodeByName using an object name.
 *
 * C++:   bool Core::capiSetStageByName(const char* name)
 * C API: bool SDOM_SetStageByName(const char* name)
 *
 * @param name Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetStageByName(const char* name);

/**
 * @brief Retrieves the active stage/root node handle.
 *
 * C++:   bool Core::capiGetRootNode(SDOM_DisplayHandle* out_handle)
 * C API: bool SDOM_GetRootNode(SDOM_DisplayHandle* out_handle)
 *
 * @param out_handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetRootNode(SDOM_DisplayHandle* out_handle);

/**
 * @brief Alias for GetRootNode; returns the active stage handle.
 *
 * C++:   bool Core::capiGetStageHandle(SDOM_DisplayHandle* out_handle)
 * C API: bool SDOM_GetStageHandle(SDOM_DisplayHandle* out_handle)
 *
 * @param out_handle Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetStageHandle(SDOM_DisplayHandle* out_handle);

/**
 * @brief Registers a C function pointer or Lua callback to run during init.
 *
 * C++:   bool Core::capiRegisterOnInit(bool (*fn)(void))
 * C API: bool SDOM_RegisterOnInit(void* fn)
 *
 * @param fn Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_RegisterOnInit(void* fn);

/**
 * @brief Registers a callback invoked during shutdown.
 *
 * C++:   bool Core::capiRegisterOnQuit(void (*fn)(void))
 * C API: bool SDOM_RegisterOnQuit(void* fn)
 *
 * @param fn Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_RegisterOnQuit(void* fn);

/**
 * @brief Registers a per-frame update callback.
 *
 * C++:   bool Core::capiRegisterOnUpdate(void (*fn)(float))
 * C API: bool SDOM_RegisterOnUpdate(void* fn)
 *
 * @param fn Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_RegisterOnUpdate(void* fn);

/**
 * @brief Registers a listener for raw SDOM events.
 *
 * C++:   bool Core::capiRegisterOnEvent(void (*fn)(const SDOM_Event*))
 * C API: bool SDOM_RegisterOnEvent(void* fn)
 *
 * @param fn Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_RegisterOnEvent(void* fn);

/**
 * @brief Registers a per-frame render callback.
 *
 * C++:   bool Core::capiRegisterOnRender(void (*fn)(void))
 * C API: bool SDOM_RegisterOnRender(void* fn)
 *
 * @param fn Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_RegisterOnRender(void* fn);

/**
 * @brief Registers a unit-test callback used by scripted tests.
 *
 * C++:   bool Core::capiRegisterOnUnitTest(bool (*fn)(void))
 * C API: bool SDOM_RegisterOnUnitTest(void* fn)
 *
 * @param fn Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_RegisterOnUnitTest(void* fn);

/**
 * @brief Registers a callback for window resize notifications.
 *
 * C++:   bool Core::capiRegisterOnWindowResize(void (*fn)(int,int))
 * C API: bool SDOM_RegisterOnWindowResize(void* fn)
 *
 * @param fn Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_RegisterOnWindowResize(void* fn);

#ifdef __cplusplus
} // extern "C"
#endif
