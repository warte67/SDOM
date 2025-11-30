#pragma once

#include <cstdint>
#include <string>

#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/CAPI/SDOM_CAPI_Event.h>

// CoreAPI adapter entrypoints accept opaque function pointers as `void*`.
// End-user callback function pointer typedefs are intentionally not
// declared here (they belong to the consumer). The adapters will cast
// the opaque pointer to the appropriate signature internally.

namespace SDOM {

class Core;

namespace CoreAPI {

// Runtime helpers
const char* getErrorCString();
std::string getErrorString();
bool setErrorMessage(const char* message);
bool init(uint64_t flags);
bool configure(const SDOM_CoreConfig* cfg);
bool getCoreConfig(SDOM_CoreConfig* out_cfg);
bool setStopAfterUnitTests(bool stop);
bool getStopAfterUnitTests();
bool setIsRunning(bool running);
bool configureFromJson(const char* json);
bool loadDomFromJson(const char* json);
bool loadDomFromJsonFile(const char* filename);
bool run();
void quit();
const char* getWindowTitle();
bool setWindowTitle(const char* title);
float getElapsedTime();
float getWindowWidth();
float getWindowHeight();
float getPixelWidth();
float getPixelHeight();
bool getPreserveAspectRatio();
bool getAllowTextureResize();
SDL_RendererLogicalPresentation getLogicalPresentation();
SDL_WindowFlags getWindowFlags();
SDL_PixelFormat getPixelFormat();
bool isFullscreen();
bool isWindowed();
bool getIsTraversing();
bool handleTabKeyPress();
bool handleTabKeyPressReverse();
bool setKeyboardFocus(const SDOM_DisplayHandle* handle);
bool getKeyboardFocus(SDOM_DisplayHandle* out_handle);
bool clearKeyboardFocus();
bool setMouseHover(const SDOM_DisplayHandle* handle);
bool getMouseHover(SDOM_DisplayHandle* out_handle);
bool clearMouseHover();
int getFrameCount();
bool getBorderColor(SDL_Color* out_color);
bool setBorderColor(const SDL_Color* color);
bool setWindowWidth(float width);
bool setWindowHeight(float height);
bool setPixelWidth(float width);
bool setPixelHeight(float height);
bool setPreserveAspectRatio(bool preserve);
bool setAllowTextureResize(bool allow);
bool setLogicalPresentation(SDL_RendererLogicalPresentation presentation);
bool setWindowFlags(SDL_WindowFlags flags);
bool setPixelFormat(SDL_PixelFormat format);
bool setFullscreen(bool fullscreen);
bool setWindowed(bool windowed);
bool setIsTraversing(bool traversing);
bool setKeyboardFocus(const SDOM_DisplayHandle* handle);
bool getKeyboardFocus(SDOM_DisplayHandle* out_handle);
bool clearKeyboardFocus();
bool setMouseHover(const SDOM_DisplayHandle* handle);
bool getMouseHover(SDOM_DisplayHandle* out_handle);
bool clearMouseHover();
bool getWindow(SDL_Window** out_window);
bool getRenderer(SDL_Renderer** out_renderer);
bool getTexture(SDL_Texture** out_texture);
bool setRootNode(const SDOM_DisplayHandle* handle);
bool setRootNodeByName(const char* name);
bool setStage(const SDOM_DisplayHandle* handle);
bool setStageByName(const char* name);
bool getRootNode(SDOM_DisplayHandle* out_handle);
bool getStageHandle(SDOM_DisplayHandle* out_handle);

// Callback registration adapters (C function pointer -> std::function)
bool registerOnInit(void* fnptr);
bool registerOnQuit(void* fnptr);
bool registerOnUpdate(void* fnptr);
bool registerOnEvent(void* fnptr);
bool registerOnRender(void* fnptr);
bool registerOnUnitTest(void* fnptr);
bool registerOnWindowResize(void* fnptr);

// Bindings
void registerBindings(Core& core, const std::string& typeName);

} // namespace CoreAPI

} // namespace SDOM
