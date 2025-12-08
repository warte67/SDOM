#pragma once
// Auto-generated SDOM C API module: Event

#include <stdbool.h>
#include <SDOM/CAPI/SDOM_CAPI_Handles.h>
#include <SDOM/CAPI/SDOM_CAPI_Event.h>

typedef enum SDOM_EventPhase {
    /* Event Phase events */
    SDOM_EventPhase_Capture = 0x0000,               ///< Event is traveling from the root down toward the target (capture phase).
    SDOM_EventPhase_Target = 0x0001,                ///< Event has reached the target object; handlers on the target run here.
    SDOM_EventPhase_Bubbling = 0x0002               ///< Event is traveling upward from the target back toward the root (bubble phase).
} SDOM_EventPhase;

typedef enum SDOM_EventType {
    /* Core events */
    SDOM_EventType_Quit = 0x0001,                   ///< Signals that the application or main stage is closing. Global only.
    SDOM_EventType_None = 0x0002,                   ///< A general-purpose EventType for testing or to represent a non-event.

    /* Application events */
    SDOM_EventType_Added = 0x0100,                  ///< Dispatched when an object is added as a child to another display object.
    SDOM_EventType_Removed = 0x0101,                ///< Dispatched when an object is removed from its parent container.
    SDOM_EventType_AddedToStage = 0x0102,           ///< Emitted when an object becomes part of the active stage hierarchy.
    SDOM_EventType_RemovedFromStage = 0x0103,       ///< Emitted when an object is detached from the stage hierarchy.
    SDOM_EventType_StageOpened = 0x0104,            ///< Indicates that a new stage or window has been opened and initialized.
    SDOM_EventType_StageClosed = 0x0105,            ///< Indicates that a stage or window has been closed and destroyed.

    /* Input events */
    SDOM_EventType_KeyDown = 0x0200,                ///< Keyboard key pressed down; bubbles through active hierarchy.
    SDOM_EventType_KeyUp = 0x0201,                  ///< Keyboard key released; bubbles through active hierarchy.
    SDOM_EventType_TextInput = 0x0202,              ///< Text input event carrying UTF-8 text from the input system.
    SDOM_EventType_MouseButtonDown = 0x0203,        ///< Mouse button pressed on a target object.
    SDOM_EventType_MouseButtonUp = 0x0204,          ///< Mouse button released over a target object.
    SDOM_EventType_MouseWheel = 0x0205,             ///< Mouse wheel scrolled; carries wheel delta values.
    SDOM_EventType_MouseMove = 0x0206,              ///< Mouse moved within or over a target object (target-only).
    SDOM_EventType_MouseClick = 0x0207,             ///< Mouse button clicked (press + release) on a target object.
    SDOM_EventType_MouseDoubleClick = 0x0208,       ///< Mouse double-clicked on a target object.
    SDOM_EventType_MouseEnter = 0x0209,             ///< Pointer entered the bounds of an object; bubbles for hover tracking.
    SDOM_EventType_MouseLeave = 0x020A,             ///< Pointer left the bounds of an object; bubbles for hover tracking.

    /* Window events */
    SDOM_EventType_FocusGained = 0x0300,            ///< The window or stage gained input focus.
    SDOM_EventType_FocusLost = 0x0301,              ///< The window or stage lost input focus.
    SDOM_EventType_Resize = 0x0302,                 ///< Window or stage resized; width and height values updated.
    SDOM_EventType_Move = 0x0303,                   ///< Window or stage moved; position values updated.
    SDOM_EventType_Show = 0x0304,                   ///< Window or object became visible on screen.
    SDOM_EventType_Hide = 0x0305,                   ///< Window or object hidden from view.
    SDOM_EventType_EnterFullscreen = 0x0306,        ///< Application entered fullscreen mode.
    SDOM_EventType_LeaveFullscreen = 0x0307,        ///< Application exited fullscreen mode.

    /* UI events */
    SDOM_EventType_ValueChanged = 0x0400,           ///< Value of a control or property has changed.
    SDOM_EventType_StateChanged = 0x0401,           ///< UI element or component state changed (e.g., active, toggled).
    SDOM_EventType_SelectionChanged = 0x0402,       ///< User selection or highlight changed within a list or group.
    SDOM_EventType_Enabled = 0x0403,                ///< Object or control has been enabled and can now receive input.
    SDOM_EventType_Disabled = 0x0404,               ///< Object or control has been disabled and can no longer receive input.
    SDOM_EventType_Visible = 0x0405,                ///< Object became visible within its parent hierarchy.
    SDOM_EventType_Hidden = 0x0406,                 ///< Object became hidden within its parent hierarchy.

    /* DragAndDrop events */
    SDOM_EventType_Drag = 0x0500,                   ///< Dragging in progress; position updated while dragging.
    SDOM_EventType_Dragging = 0x0501,               ///< Continuous drag event emitted while dragging is active.
    SDOM_EventType_Drop = 0x0502,                   ///< An item or data payload was dropped onto a valid target.

    /* Timer events */
    SDOM_EventType_TimerStart = 0x0600,             ///< Timer started or resumed counting.
    SDOM_EventType_TimerStop = 0x0601,              ///< Timer stopped and reset to initial state.
    SDOM_EventType_TimerPause = 0x0602,             ///< Timer paused but not reset.
    SDOM_EventType_TimerTick = 0x0603,              ///< Timer tick event; emitted on each interval step.
    SDOM_EventType_TimerCycleComplete = 0x0604,     ///< Timer completed one full interval cycle.
    SDOM_EventType_TimerComplete = 0x0605,          ///< Timer finished all cycles and has reached completion.

    /* Clipboard events */
    SDOM_EventType_ClipboardCopy = 0x0700,          ///< Data copied to the system clipboard.
    SDOM_EventType_ClipboardPaste = 0x0701,         ///< Data pasted from the system clipboard.

    /* Listener events */
    SDOM_EventType_OnInit = 0x0800,                 ///< Initialization callback for objects; called after creation.
    SDOM_EventType_OnQuit = 0x0801,                 ///< Global callback fired during shutdown sequence.
    SDOM_EventType_OnEvent = 0x0802,                ///< Generic hook for catching all events before normal dispatch.
    SDOM_EventType_OnUpdate = 0x0803,               ///< Called once per frame before rendering; main update loop hook.
    SDOM_EventType_OnRender = 0x0804,               ///< Called each frame when object should perform its rendering.
    SDOM_EventType_OnPreRender = 0x0805,            ///< Called before OnRender for pre-draw logic such as layout or transforms.

    /* Frame events */
    SDOM_EventType_EnterFrame = 0x0900,             ///< Legacy per-frame tick event (use OnUpdate instead).
    SDOM_EventType_SDL_Event = 0x0901,              ///< Raw SDL event wrapper; dispatched when unhandled by internal systems.
    SDOM_EventType_User = 0x0902                    ///< Reserved for custom or user-defined events created at runtime.
} SDOM_EventType;

#ifdef __cplusplus
extern "C" {
#endif

// Opaque C handle for SDOM::Event.
typedef struct SDOM_Event {
    void* impl;                                     ///< Opaque pointer to underlying C++ instance.
} SDOM_Event;

// Callable bindings

/**
 * @brief Retrieves the numeric SDOM_EventType id for this event.
 *
 * C++:   EventType Event::getType() const
 * C API: bool SDOM_GetEventType(const SDOM_Event* evt, SDOM_EventType* out_type)
 *
 * @param evt Pointer parameter.
 * @param out_type Pointer parameter.
 * @return EventType; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetEventType(const SDOM_Event* evt, SDOM_EventType* out_type);

/**
 * @brief Retrieves the EventType name associated with this event.
 *
 * C++:   std::string Event::getTypeName() const
 * C API: const char* SDOM_GetEventTypeName(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetEventTypeName(const SDOM_Event* evt);

/**
 * @brief Returns the current propagation phase for this event.
 *
 * C++:   Event::Phase Event::getPhase() const
 * C API: SDOM_EventPhase SDOM_GetEventPhase(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return SDOM_EventPhase; check SDOM_GetError() for details on failure.
 */
SDOM_EventPhase SDOM_GetEventPhase(const SDOM_Event* evt);

/**
 * @brief Sets the propagation phase for this event.
 *
 * C++:   Event& Event::setPhase(Event::Phase phase)
 * C API: bool SDOM_SetEventPhase(SDOM_Event* evt, SDOM_EventPhase phase)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventPhase(SDOM_Event* evt, SDOM_EventPhase phase);

/**
 * @brief Retrieves the textual description of the current dispatch phase.
 *
 * C++:   std::string Event::getPhaseString() const
 * C API: const char* SDOM_GetEventPhaseString(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetEventPhaseString(const SDOM_Event* evt);

/**
 * @brief Copies the event's target DisplayHandle into an SDOM_DisplayHandle snapshot.
 *
 * C++:   DisplayHandle Event::getTarget() const
 * C API: bool SDOM_GetEventTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target)
 *
 * @param evt Pointer parameter.
 * @param out_target Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetEventTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target);

/**
 * @brief Assigns a new DisplayHandle target snapshot to the event.
 *
 * C++:   Event& Event::setTarget(DisplayHandle newTarget)
 * C API: bool SDOM_SetEventTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target)
 *
 * @param evt Pointer parameter.
 * @param new_target Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target);

/**
 * @brief Copies the current dispatch target DisplayHandle into an SDOM_DisplayHandle snapshot.
 *
 * C++:   DisplayHandle Event::getCurrentTarget() const
 * C API: bool SDOM_GetEventCurrentTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target)
 *
 * @param evt Pointer parameter.
 * @param out_target Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetEventCurrentTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target);

/**
 * @brief Assigns a new current dispatch target via an SDOM_DisplayHandle snapshot.
 *
 * C++:   Event& Event::setCurrentTarget(DisplayHandle newCurrentTarget)
 * C API: bool SDOM_SetEventCurrentTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target)
 *
 * @param evt Pointer parameter.
 * @param new_target Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventCurrentTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target);

/**
 * @brief Copies the related target DisplayHandle (if any) into an SDOM_DisplayHandle snapshot.
 *
 * C++:   DisplayHandle Event::getRelatedTarget() const
 * C API: bool SDOM_GetEventRelatedTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target)
 *
 * @param evt Pointer parameter.
 * @param out_target Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetEventRelatedTarget(const SDOM_Event* evt, SDOM_DisplayHandle* out_target);

/**
 * @brief Assigns a new related target DisplayHandle snapshot on the event.
 *
 * C++:   Event& Event::setRelatedTarget(DisplayHandle newRelatedTarget)
 * C API: bool SDOM_SetEventRelatedTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target)
 *
 * @param evt Pointer parameter.
 * @param new_target Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventRelatedTarget(SDOM_Event* evt, const SDOM_DisplayHandle* new_target);

/**
 * @brief Serializes the embedded SDL_Event into a JSON snapshot for interop.
 *
 * C++:   std::string Event::getSDL_EventJson() const
 * C API: const char* SDOM_GetEventSDLJson(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetEventSDLJson(const SDOM_Event* evt);

/**
 * @brief Reconstructs the embedded SDL_Event from a JSON snapshot.
 *
 * C++:   Event& Event::setSDL_EventJson(const std::string& json)
 * C API: bool SDOM_SetEventSDLJson(SDOM_Event* evt, const char* json)
 *
 * @param evt Pointer parameter.
 * @param json Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventSDLJson(SDOM_Event* evt, const char* json);

/**
 * @brief Returns true if propagation has been stopped for this event.
 *
 * C++:   bool Event::isPropagationStopped() const
 * C API: bool SDOM_IsEventPropagationStopped(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_IsEventPropagationStopped(const SDOM_Event* evt);

/**
 * @brief Stops event propagation (capture/bubble) for this event instance.
 *
 * C++:   Event& Event::stopPropagation()
 * C API: bool SDOM_StopEventPropagation(SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_StopEventPropagation(SDOM_Event* evt);

/**
 * @brief Returns true if the event's default behavior has been disabled.
 *
 * C++:   bool Event::isDefaultBehaviorDisabled() const
 * C API: bool SDOM_IsEventDefaultBehaviorDisabled(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_IsEventDefaultBehaviorDisabled(const SDOM_Event* evt);

/**
 * @brief Enables or disables the event's default behavior.
 *
 * C++:   Event& Event::setDisableDefaultBehavior(bool disable)
 * C API: bool SDOM_SetEventDisableDefaultBehavior(SDOM_Event* evt, bool disable)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventDisableDefaultBehavior(SDOM_Event* evt, bool disable);

/**
 * @brief Returns whether this event is currently using capture semantics.
 *
 * C++:   bool Event::getUseCapture() const
 * C API: bool SDOM_GetEventUseCapture(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_GetEventUseCapture(const SDOM_Event* evt);

/**
 * @brief Sets whether this event should use capture semantics during dispatch.
 *
 * C++:   Event& Event::setUseCapture(bool useCapture)
 * C API: bool SDOM_SetEventUseCapture(SDOM_Event* evt, bool use_capture)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventUseCapture(SDOM_Event* evt, bool use_capture);

/**
 * @brief Retrieves the elapsed time associated with this event in seconds.
 *
 * C++:   float Event::getElapsedTime() const
 * C API: float SDOM_GetEventElapsedTime(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetEventElapsedTime(const SDOM_Event* evt);

/**
 * @brief Sets the elapsed time (in seconds) for this event instance.
 *
 * C++:   Event& Event::setElapsedTime(float elapsedTime)
 * C API: bool SDOM_SetEventElapsedTime(SDOM_Event* evt, float elapsed_time)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventElapsedTime(SDOM_Event* evt, float elapsed_time);

/**
 * @brief Retrieves the cached mouse X coordinate for the event.
 *
 * C++:   float Event::getMouseX() const
 * C API: float SDOM_GetEventMouseX(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetEventMouseX(const SDOM_Event* evt);

/**
 * @brief Sets the mouse X coordinate for the event payload.
 *
 * C++:   Event& Event::setMouseX(float x)
 * C API: bool SDOM_SetEventMouseX(SDOM_Event* evt, float x)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventMouseX(SDOM_Event* evt, float x);

/**
 * @brief Retrieves the cached mouse Y coordinate for the event.
 *
 * C++:   float Event::getMouseY() const
 * C API: float SDOM_GetEventMouseY(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetEventMouseY(const SDOM_Event* evt);

/**
 * @brief Sets the mouse Y coordinate for the event payload.
 *
 * C++:   Event& Event::setMouseY(float y)
 * C API: bool SDOM_SetEventMouseY(SDOM_Event* evt, float y)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventMouseY(SDOM_Event* evt, float y);

/**
 * @brief Returns the horizontal wheel delta collected for the event.
 *
 * C++:   float Event::getWheelX() const
 * C API: float SDOM_GetEventWheelX(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetEventWheelX(const SDOM_Event* evt);

/**
 * @brief Sets the horizontal wheel delta in the payload.
 *
 * C++:   Event& Event::setWheelX(float x)
 * C API: bool SDOM_SetEventWheelX(SDOM_Event* evt, float x)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventWheelX(SDOM_Event* evt, float x);

/**
 * @brief Returns the vertical wheel delta collected for the event.
 *
 * C++:   float Event::getWheelY() const
 * C API: float SDOM_GetEventWheelY(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetEventWheelY(const SDOM_Event* evt);

/**
 * @brief Sets the vertical wheel delta in the payload.
 *
 * C++:   Event& Event::setWheelY(float y)
 * C API: bool SDOM_SetEventWheelY(SDOM_Event* evt, float y)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventWheelY(SDOM_Event* evt, float y);

/**
 * @brief Provides the drag offset (X axis) for drag events.
 *
 * C++:   float Event::getDragOffsetX() const
 * C API: float SDOM_GetEventDragOffsetX(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetEventDragOffsetX(const SDOM_Event* evt);

/**
 * @brief Stores the drag offset (X axis) into the event payload.
 *
 * C++:   Event& Event::setDragOffsetX(float offsetX)
 * C API: bool SDOM_SetEventDragOffsetX(SDOM_Event* evt, float offset_x)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventDragOffsetX(SDOM_Event* evt, float offset_x);

/**
 * @brief Provides the drag offset (Y axis) for drag events.
 *
 * C++:   float Event::getDragOffsetY() const
 * C API: float SDOM_GetEventDragOffsetY(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return float; check SDOM_GetError() for details on failure.
 */
float SDOM_GetEventDragOffsetY(const SDOM_Event* evt);

/**
 * @brief Stores the drag offset (Y axis) into the event payload.
 *
 * C++:   Event& Event::setDragOffsetY(float offsetY)
 * C API: bool SDOM_SetEventDragOffsetY(SDOM_Event* evt, float offset_y)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventDragOffsetY(SDOM_Event* evt, float offset_y);

/**
 * @brief Returns the click count (single/double/etc.) stored in the event.
 *
 * C++:   int Event::getClickCount() const
 * C API: int SDOM_GetEventClickCount(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetEventClickCount(const SDOM_Event* evt);

/**
 * @brief Sets the click count metadata for the event payload.
 *
 * C++:   Event& Event::setClickCount(int count)
 * C API: bool SDOM_SetEventClickCount(SDOM_Event* evt, int count)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventClickCount(SDOM_Event* evt, int count);

/**
 * @brief Returns the mouse button identifier that triggered the event.
 *
 * C++:   uint8_t Event::getButton() const
 * C API: uint8_t SDOM_GetEventButton(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return uint8_t; check SDOM_GetError() for details on failure.
 */
uint8_t SDOM_GetEventButton(const SDOM_Event* evt);

/**
 * @brief Sets the mouse button identifier for this event payload.
 *
 * C++:   Event& Event::setButton(uint8_t btn)
 * C API: bool SDOM_SetEventButton(SDOM_Event* evt, uint8_t button)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventButton(SDOM_Event* evt, uint8_t button);

/**
 * @brief Retrieves the SDL scancode associated with the keyboard event.
 *
 * C++:   SDL_Scancode Event::getScanCode() const
 * C API: int SDOM_GetEventScanCode(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetEventScanCode(const SDOM_Event* evt);

/**
 * @brief Sets the SDL scancode metadata for the keyboard event.
 *
 * C++:   Event& Event::setScanCode(SDL_Scancode scancode)
 * C API: bool SDOM_SetEventScanCode(SDOM_Event* evt, int scancode)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventScanCode(SDOM_Event* evt, int scancode);

/**
 * @brief Retrieves the SDL keycode value stored on the event.
 *
 * C++:   SDL_Keycode Event::getKeycode() const
 * C API: int SDOM_GetEventKeycode(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetEventKeycode(const SDOM_Event* evt);

/**
 * @brief Sets the SDL keycode value for the event payload.
 *
 * C++:   Event& Event::setKeycode(SDL_Keycode keycode)
 * C API: bool SDOM_SetEventKeycode(SDOM_Event* evt, int keycode)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventKeycode(SDOM_Event* evt, int keycode);

/**
 * @brief Returns the SDL key modifier mask stored on the event.
 *
 * C++:   Uint16 Event::getKeymod() const
 * C API: int SDOM_GetEventKeymod(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetEventKeymod(const SDOM_Event* evt);

/**
 * @brief Sets the key modifier mask for the keyboard event.
 *
 * C++:   Event& Event::setKeymod(Uint16 keymod)
 * C API: bool SDOM_SetEventKeymod(SDOM_Event* evt, int keymod)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventKeymod(SDOM_Event* evt, int keymod);

/**
 * @brief Returns the translated ASCII code (if available) for the keypress.
 *
 * C++:   int Event::getAsciiCode() const
 * C API: int SDOM_GetEventAsciiCode(const SDOM_Event* evt)
 *
 * @param evt Pointer parameter.
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetEventAsciiCode(const SDOM_Event* evt);

/**
 * @brief Sets the translated ASCII code for the keyboard event payload.
 *
 * C++:   Event& Event::setAsciiCode(int asciiCode)
 * C API: bool SDOM_SetEventAsciiCode(SDOM_Event* evt, int ascii_code)
 *
 * @param evt Pointer parameter.
 * @return bool; check SDOM_GetError() for details on failure.
 */
bool SDOM_SetEventAsciiCode(SDOM_Event* evt, int ascii_code);

#ifdef __cplusplus
} // extern "C"
#endif
