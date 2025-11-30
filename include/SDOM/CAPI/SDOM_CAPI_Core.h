#pragma once
// Auto-generated SDOM C API module: Core

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDOM/CAPI/SDOM_CAPI_Handles.h>


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
bool SDOM_ConfigureFromJson(const char* json);
bool SDOM_GetCoreConfig(SDOM_CoreConfig* out_cfg);
bool SDOM_SetStopAfterUnitTests(bool stop);
bool SDOM_SetIsRunning(bool running);
bool SDOM_LoadDomFromJsonFile(const char* filename);
bool SDOM_LoadDomFromJson(const char* json);
bool SDOM_GetBorderColor(SDL_Color* out_color);
bool SDOM_SetBorderColor(const SDL_Color* color);
const char* SDOM_GetWindowTitle(void);
bool SDOM_SetWindowTitle(const char* title);
bool SDOM_PumpEventsOnce(void);
bool SDOM_PushMouseEvent(const char* json);
bool SDOM_PushKeyboardEvent(const char* json);
bool SDOM_HasLuaSupport(void);
const char* SDOM_GetVersionString(void);
const char* SDOM_GetVersionFullString(void);
int SDOM_GetVersionMajor(void);
int SDOM_GetVersionMinor(void);
int SDOM_GetVersionPatch(void);
const char* SDOM_GetVersionCodename(void);
const char* SDOM_GetVersionBuild(void);
const char* SDOM_GetVersionBuildDate(void);
const char* SDOM_GetVersionCommit(void);
const char* SDOM_GetVersionBranch(void);
const char* SDOM_GetVersionCompiler(void);
const char* SDOM_GetVersionPlatform(void);
float SDOM_GetElapsedTime(void);
float SDOM_GetWindowWidth(void);
float SDOM_GetWindowHeight(void);
float SDOM_GetPixelWidth(void);
float SDOM_GetPixelHeight(void);
bool SDOM_GetPreserveAspectRatio(void);
bool SDOM_GetAllowTextureResize(void);
SDL_RendererLogicalPresentation SDOM_GetLogicalPresentation(void);
SDL_WindowFlags SDOM_GetWindowFlags(void);
SDL_PixelFormat SDOM_GetPixelFormat(void);
bool SDOM_IsFullscreen(void);
bool SDOM_IsWindowed(void);
bool SDOM_GetIsTraversing(void);
bool SDOM_HandleTabKeyPress(void);
bool SDOM_HandleTabKeyPressReverse(void);
bool SDOM_GetWindow(SDL_Window** out_window);
bool SDOM_GetRenderer(SDL_Renderer** out_renderer);
bool SDOM_GetTexture(SDL_Texture** out_texture);
bool SDOM_SetWindowWidth(float width);
bool SDOM_SetWindowHeight(float height);
bool SDOM_SetPixelWidth(float width);
bool SDOM_SetPixelHeight(float height);
bool SDOM_SetPreserveAspectRatio(bool preserve);
bool SDOM_SetAllowTextureResize(bool allow);
bool SDOM_SetLogicalPresentation(SDL_RendererLogicalPresentation presentation);
bool SDOM_SetWindowFlags(SDL_WindowFlags flags);
bool SDOM_SetPixelFormat(SDL_PixelFormat fmt);
bool SDOM_SetFullscreen(bool fullscreen);
bool SDOM_SetWindowed(bool windowed);
bool SDOM_SetIsTraversing(bool traversing);
bool SDOM_SetKeyboardFocus(const SDOM_DisplayHandle* handle);
bool SDOM_GetKeyboardFocus(SDOM_DisplayHandle* out_handle);
bool SDOM_ClearKeyboardFocus(void);
bool SDOM_SetMouseHover(const SDOM_DisplayHandle* handle);
bool SDOM_GetMouseHover(SDOM_DisplayHandle* out_handle);
bool SDOM_ClearMouseHover(void);
int SDOM_GetFrameCount(void);
bool SDOM_GetDisplayObject(const char* name, SDOM_DisplayHandle* out_handle);
bool SDOM_HasDisplayObject(const char* name);
bool SDOM_DestroyDisplayObjectByName(const char* name);
bool SDOM_DestroyDisplayObject(const SDOM_DisplayHandle* handle);
bool SDOM_GetAssetObject(const char* name, SDOM_AssetHandle* out_handle);
bool SDOM_HasAssetObject(const char* name);
bool SDOM_DestroyAssetObjectByName(const char* name);
bool SDOM_DestroyAssetObject(const SDOM_AssetHandle* handle);
int SDOM_CountOrphans(void);
bool SDOM_GetOrphans(SDOM_DisplayHandle* out_handles, int* out_count, int max_count);
bool SDOM_DetachOrphans(void);
bool SDOM_CollectGarbage(void);
bool SDOM_AttachFutureChildren(void);
bool SDOM_AddOrphan(const SDOM_DisplayHandle* orphan);
bool SDOM_AddFutureChild(const SDOM_DisplayHandle* child, const SDOM_DisplayHandle* parent, bool useWorld, int worldX, int worldY);
bool SDOM_GetDisplayObjectNames(const char** out_names, int* out_count, int max_count);
bool SDOM_ClearFactory(void);
bool SDOM_PrintObjectRegistry(void);
bool SDOM_GetPropertyNamesForType(const char* type, const char** out_names, int* out_count, int max_count);
bool SDOM_GetCommandNamesForType(const char* type, const char** out_names, int* out_count, int max_count);
bool SDOM_GetFunctionNamesForType(const char* type, const char** out_names, int* out_count, int max_count);
bool SDOM_SetIgnoreRealInput(bool ignore);
bool SDOM_GetIgnoreRealInput(bool* out_ignore);
bool SDOM_GetKeyfocusGray(float* out_gray);
bool SDOM_SetKeyfocusGray(float gray);
bool SDOM_CreateDisplayObjectFromJson(const char* type, const char* json, SDOM_DisplayHandle* out_handle);
bool SDOM_CreateAssetObjectFromJson(const char* type, const char* json, SDOM_AssetHandle* out_handle);
bool SDOM_Run(void);
bool SDOM_GetStopAfterUnitTests(void);
void SDOM_Quit(void);
bool SDOM_SetRootNode(const SDOM_DisplayHandle* handle);
bool SDOM_SetRootNodeByName(const char* name);
bool SDOM_SetStage(const SDOM_DisplayHandle* handle);
bool SDOM_SetStageByName(const char* name);
bool SDOM_GetRootNode(SDOM_DisplayHandle* out_handle);
bool SDOM_GetStageHandle(SDOM_DisplayHandle* out_handle);
bool SDOM_RegisterOnInit(void* fn);
bool SDOM_RegisterOnQuit(void* fn);
bool SDOM_RegisterOnUpdate(void* fn);
bool SDOM_RegisterOnEvent(void* fn);
bool SDOM_RegisterOnRender(void* fn);
bool SDOM_RegisterOnUnitTest(void* fn);
bool SDOM_RegisterOnWindowResize(void* fn);

#ifdef __cplusplus
} // extern "C"
#endif
