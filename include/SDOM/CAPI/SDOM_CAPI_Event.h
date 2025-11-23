#pragma once
// Auto-generated SDOM C API module: Event

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

typedef struct SDOM_Event {
    void* impl;                                     ///< Pointer to the underlying C++ SDOM::Event instance.
} SDOM_Event;

#ifdef __cplusplus
extern "C" {
#endif

// Callable bindings
bool SDOM_GetEventType(const SDOM_Event* evt, SDOM_EventType* out_type);

#ifdef __cplusplus
} // extern "C"
#endif
