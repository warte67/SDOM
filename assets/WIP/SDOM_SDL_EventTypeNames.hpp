#ifndef SDOM_SDL_EVENT_TYPE_NAMES_HPP
#define SDOM_SDL_EVENT_TYPE_NAMES_HPP

#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>

// Map of SDL_EventType to their string representations
inline const std::unordered_map<SDL_EventType, std::string> SDL_EventTypeNames = {
    {SDL_EVENT_FIRST,                     "SDL_EVENT_FIRST"},                     // Unused (do not remove)
    // Application events
    {SDL_EVENT_QUIT,                      "SDL_EVENT_QUIT"},                      // User-requested quit
    {SDL_EVENT_TERMINATING,               "SDL_EVENT_TERMINATING"},               // The application is being terminated by the OS
    {SDL_EVENT_LOW_MEMORY,                "SDL_EVENT_LOW_MEMORY"},                // The application is low on memory
    {SDL_EVENT_WILL_ENTER_BACKGROUND,     "SDL_EVENT_WILL_ENTER_BACKGROUND"},     // The application is about to enter the background
    {SDL_EVENT_DID_ENTER_BACKGROUND,      "SDL_EVENT_DID_ENTER_BACKGROUND"},      // The application did enter the background
    {SDL_EVENT_WILL_ENTER_FOREGROUND,     "SDL_EVENT_WILL_ENTER_FOREGROUND"},     // The application is about to enter the foreground
    {SDL_EVENT_DID_ENTER_FOREGROUND,      "SDL_EVENT_DID_ENTER_FOREGROUND"},      // The application is now interactive
    {SDL_EVENT_LOCALE_CHANGED,            "SDL_EVENT_LOCALE_CHANGED"},            // The user's locale preferences have changed
    // Display events
    {SDL_EVENT_DISPLAY_ORIENTATION,       "SDL_EVENT_DISPLAY_ORIENTATION"},       // Display orientation has changed
    {SDL_EVENT_DISPLAY_ADDED,             "SDL_EVENT_DISPLAY_ADDED"},             // Display has been added to the system
    {SDL_EVENT_DISPLAY_REMOVED,           "SDL_EVENT_DISPLAY_REMOVED"},           // Display has been removed from the system
    {SDL_EVENT_DISPLAY_MOVED,             "SDL_EVENT_DISPLAY_MOVED"},             // Display has been moved
    {SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED, "SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED"}, // Display has changed desktop mode
    {SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED, "SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED"}, // Display has changed current mode
    {SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED, "SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED"}, // Display has changed content scale
    // Window events
    {SDL_EVENT_WINDOW_SHOWN,              "SDL_EVENT_WINDOW_SHOWN"},              // Window has been shown
    {SDL_EVENT_WINDOW_HIDDEN,             "SDL_EVENT_WINDOW_HIDDEN"},             // Window has been hidden
    {SDL_EVENT_WINDOW_EXPOSED,            "SDL_EVENT_WINDOW_EXPOSED"},            // Window has been exposed
    {SDL_EVENT_WINDOW_MOVED,              "SDL_EVENT_WINDOW_MOVED"},              // Window has been moved
    {SDL_EVENT_WINDOW_RESIZED,            "SDL_EVENT_WINDOW_RESIZED"},            // Window has been resized
    {SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED, "SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED"}, // The pixel size of the window has changed
    {SDL_EVENT_WINDOW_METAL_VIEW_RESIZED, "SDL_EVENT_WINDOW_METAL_VIEW_RESIZED"}, // The pixel size of a Metal view has changed
    {SDL_EVENT_WINDOW_MINIMIZED,          "SDL_EVENT_WINDOW_MINIMIZED"},          // Window has been minimized
    {SDL_EVENT_WINDOW_MAXIMIZED,          "SDL_EVENT_WINDOW_MAXIMIZED"},          // Window has been maximized
    {SDL_EVENT_WINDOW_RESTORED,           "SDL_EVENT_WINDOW_RESTORED"},           // Window has been restored
    {SDL_EVENT_WINDOW_MOUSE_ENTER,        "SDL_EVENT_WINDOW_MOUSE_ENTER"},        // Window has gained mouse focus
    {SDL_EVENT_WINDOW_MOUSE_LEAVE,        "SDL_EVENT_WINDOW_MOUSE_LEAVE"},        // Window has lost mouse focus
    {SDL_EVENT_WINDOW_FOCUS_GAINED,       "SDL_EVENT_WINDOW_FOCUS_GAINED"},       // Window has gained keyboard focus
    {SDL_EVENT_WINDOW_FOCUS_LOST,         "SDL_EVENT_WINDOW_FOCUS_LOST"},         // Window has lost keyboard focus
    {SDL_EVENT_WINDOW_CLOSE_REQUESTED,    "SDL_EVENT_WINDOW_CLOSE_REQUESTED"},    // The window manager requests that the window be closed
    {SDL_EVENT_WINDOW_HIT_TEST,           "SDL_EVENT_WINDOW_HIT_TEST"},           // Window had a hit test
    {SDL_EVENT_WINDOW_ICCPROF_CHANGED,    "SDL_EVENT_WINDOW_ICCPROF_CHANGED"},    // The ICC profile of the window's display has changed
    {SDL_EVENT_WINDOW_DISPLAY_CHANGED,    "SDL_EVENT_WINDOW_DISPLAY_CHANGED"},    // Window has been moved to display
    {SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED, "SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED"}, // Window display scale has been changed
    {SDL_EVENT_WINDOW_SAFE_AREA_CHANGED,  "SDL_EVENT_WINDOW_SAFE_AREA_CHANGED"},  // The window safe area has been changed
    {SDL_EVENT_WINDOW_OCCLUDED,           "SDL_EVENT_WINDOW_OCCLUDED"},           // The window has been occluded
    {SDL_EVENT_WINDOW_ENTER_FULLSCREEN,   "SDL_EVENT_WINDOW_ENTER_FULLSCREEN"},   // The window has entered fullscreen mode
    {SDL_EVENT_WINDOW_LEAVE_FULLSCREEN,   "SDL_EVENT_WINDOW_LEAVE_FULLSCREEN"},   // The window has left fullscreen mode
    {SDL_EVENT_WINDOW_DESTROYED,          "SDL_EVENT_WINDOW_DESTROYED"},          // The window with the associated ID is being or has been destroyed
    {SDL_EVENT_WINDOW_HDR_STATE_CHANGED,  "SDL_EVENT_WINDOW_HDR_STATE_CHANGED"},  // Window HDR properties have changed
    // Keyboard events
    {SDL_EVENT_KEY_DOWN,                  "SDL_EVENT_KEY_DOWN"},                  // Key pressed
    {SDL_EVENT_KEY_UP,                    "SDL_EVENT_KEY_UP"},                    // Key released
    {SDL_EVENT_TEXT_EDITING,              "SDL_EVENT_TEXT_EDITING"},              // Keyboard text editing
    {SDL_EVENT_TEXT_INPUT,                "SDL_EVENT_TEXT_INPUT"},                // Keyboard text input
    {SDL_EVENT_KEYMAP_CHANGED,            "SDL_EVENT_KEYMAP_CHANGED"},            // Keymap changed
    {SDL_EVENT_KEYBOARD_ADDED,            "SDL_EVENT_KEYBOARD_ADDED"},            // A new keyboard has been inserted
    {SDL_EVENT_KEYBOARD_REMOVED,          "SDL_EVENT_KEYBOARD_REMOVED"},          // A keyboard has been removed
    // Mouse events
    {SDL_EVENT_MOUSE_MOTION,              "SDL_EVENT_MOUSE_MOTION"},              // Mouse moved
    {SDL_EVENT_MOUSE_BUTTON_DOWN,         "SDL_EVENT_MOUSE_BUTTON_DOWN"},         // Mouse button pressed
    {SDL_EVENT_MOUSE_BUTTON_UP,           "SDL_EVENT_MOUSE_BUTTON_UP"},           // Mouse button released
    {SDL_EVENT_MOUSE_WHEEL,               "SDL_EVENT_MOUSE_WHEEL"},               // Mouse wheel motion
    {SDL_EVENT_MOUSE_ADDED,               "SDL_EVENT_MOUSE_ADDED"},               // A new mouse has been inserted
    {SDL_EVENT_MOUSE_REMOVED,             "SDL_EVENT_MOUSE_REMOVED"},             // A mouse has been removed
    // Joystick events
    {SDL_EVENT_JOYSTICK_AXIS_MOTION,      "SDL_EVENT_JOYSTICK_AXIS_MOTION"},      // Joystick axis motion
    {SDL_EVENT_JOYSTICK_BALL_MOTION,      "SDL_EVENT_JOYSTICK_BALL_MOTION"},      // Joystick trackball motion
    {SDL_EVENT_JOYSTICK_HAT_MOTION,       "SDL_EVENT_JOYSTICK_HAT_MOTION"},       // Joystick hat position change
    {SDL_EVENT_JOYSTICK_BUTTON_DOWN,      "SDL_EVENT_JOYSTICK_BUTTON_DOWN"},      // Joystick button pressed
    {SDL_EVENT_JOYSTICK_BUTTON_UP,        "SDL_EVENT_JOYSTICK_BUTTON_UP"},        // Joystick button released
    {SDL_EVENT_JOYSTICK_ADDED,            "SDL_EVENT_JOYSTICK_ADDED"},            // A new joystick has been inserted
    {SDL_EVENT_JOYSTICK_REMOVED,          "SDL_EVENT_JOYSTICK_REMOVED"},          // An opened joystick has been removed
    {SDL_EVENT_JOYSTICK_BATTERY_UPDATED,  "SDL_EVENT_JOYSTICK_BATTERY_UPDATED"},  // Joystick battery level change
    // Gamepad events
    {SDL_EVENT_GAMEPAD_AXIS_MOTION,       "SDL_EVENT_GAMEPAD_AXIS_MOTION"},       // Gamepad axis motion
    {SDL_EVENT_GAMEPAD_BUTTON_DOWN,       "SDL_EVENT_GAMEPAD_BUTTON_DOWN"},       // Gamepad button pressed
    {SDL_EVENT_GAMEPAD_BUTTON_UP,         "SDL_EVENT_GAMEPAD_BUTTON_UP"},         // Gamepad button released
    {SDL_EVENT_GAMEPAD_ADDED,             "SDL_EVENT_GAMEPAD_ADDED"},             // A new gamepad has been inserted
    {SDL_EVENT_GAMEPAD_REMOVED,           "SDL_EVENT_GAMEPAD_REMOVED"},           // A gamepad has been removed
    {SDL_EVENT_GAMEPAD_REMAPPED,          "SDL_EVENT_GAMEPAD_REMAPPED"},          // The gamepad mapping was updated
    {SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN,     "SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN"},     // Gamepad touchpad was touched
    {SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION,   "SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION"},   // Gamepad touchpad finger was moved
    {SDL_EVENT_GAMEPAD_TOUCHPAD_UP,       "SDL_EVENT_GAMEPAD_TOUCHPAD_UP"},       // Gamepad touchpad finger was lifted
    {SDL_EVENT_GAMEPAD_SENSOR_UPDATE,     "SDL_EVENT_GAMEPAD_SENSOR_UPDATE"},     // Gamepad sensor was updated
    // Touch events
    {SDL_EVENT_FINGER_DOWN,               "SDL_EVENT_FINGER_DOWN"},               // Finger down
    {SDL_EVENT_FINGER_UP,                 "SDL_EVENT_FINGER_UP"},                 // Finger up
    {SDL_EVENT_FINGER_MOTION,             "SDL_EVENT_FINGER_MOTION"},             // Finger motion
    // Clipboard events
    {SDL_EVENT_CLIPBOARD_UPDATE,          "SDL_EVENT_CLIPBOARD_UPDATE"},          // Clipboard updated
    // Drag and drop events
    {SDL_EVENT_DROP_FILE,                 "SDL_EVENT_DROP_FILE"},                 // File dropped
    {SDL_EVENT_DROP_TEXT,                 "SDL_EVENT_DROP_TEXT"},                 // Text dropped
    {SDL_EVENT_DROP_BEGIN,                "SDL_EVENT_DROP_BEGIN"},                // Drag and drop began
    {SDL_EVENT_DROP_COMPLETE,             "SDL_EVENT_DROP_COMPLETE"},             // Drag and drop completed
    {SDL_EVENT_DROP_POSITION,             "SDL_EVENT_DROP_POSITION"},             // Drag and drop position
    // Audio hotplug events
    {SDL_EVENT_AUDIO_DEVICE_ADDED,        "SDL_EVENT_AUDIO_DEVICE_ADDED"},        // Audio device added
    {SDL_EVENT_AUDIO_DEVICE_REMOVED,      "SDL_EVENT_AUDIO_DEVICE_REMOVED"},      // Audio device removed
    // Sensor events
    {SDL_EVENT_SENSOR_UPDATE,             "SDL_EVENT_SENSOR_UPDATE"},             // Sensor updated
    // Pressure-sensitive pen events
    {SDL_EVENT_PEN_PROXIMITY_IN,          "SDL_EVENT_PEN_PROXIMITY_IN"},          // Pen proximity in
    {SDL_EVENT_PEN_PROXIMITY_OUT,         "SDL_EVENT_PEN_PROXIMITY_OUT"},         // Pen proximity out
    {SDL_EVENT_PEN_DOWN,                  "SDL_EVENT_PEN_DOWN"},                  // Pen down
    {SDL_EVENT_PEN_UP,                    "SDL_EVENT_PEN_UP"},                    // Pen up
    {SDL_EVENT_PEN_BUTTON_DOWN,           "SDL_EVENT_PEN_BUTTON_DOWN"},           // Pen button down
    {SDL_EVENT_PEN_BUTTON_UP,             "SDL_EVENT_PEN_BUTTON_UP"},             // Pen button up
    {SDL_EVENT_PEN_MOTION,                "SDL_EVENT_PEN_MOTION"},                // Pen motion
    {SDL_EVENT_PEN_AXIS,                  "SDL_EVENT_PEN_AXIS"},                  // Pen axis changed
    // Camera hotplug events
    {SDL_EVENT_CAMERA_DEVICE_ADDED,       "SDL_EVENT_CAMERA_DEVICE_ADDED"},       // Camera device added
    {SDL_EVENT_CAMERA_DEVICE_REMOVED,     "SDL_EVENT_CAMERA_DEVICE_REMOVED"},     // Camera device removed
    // Render events
    {SDL_EVENT_RENDER_TARGETS_RESET,      "SDL_EVENT_RENDER_TARGETS_RESET"},      // Render targets reset
    {SDL_EVENT_RENDER_DEVICE_RESET,       "SDL_EVENT_RENDER_DEVICE_RESET"},       // Render device reset
    {SDL_EVENT_RENDER_DEVICE_LOST,        "SDL_EVENT_RENDER_DEVICE_LOST"},        // Render device lost
    // Reserved events for private platforms
    {SDL_EVENT_PRIVATE0,                  "SDL_EVENT_PRIVATE0"},                  // Reserved private event 0
    {SDL_EVENT_PRIVATE1,                  "SDL_EVENT_PRIVATE1"},                  // Reserved private event 1
    {SDL_EVENT_PRIVATE2,                  "SDL_EVENT_PRIVATE2"},                  // Reserved private event 2
    {SDL_EVENT_PRIVATE3,                  "SDL_EVENT_PRIVATE3"},                  // Reserved private event 3
    // Internal events
    {SDL_EVENT_POLL_SENTINEL,             "SDL_EVENT_POLL_SENTINEL"},             // Signals the end of an event poll cycle
    // User events
    {SDL_EVENT_USER,                      "SDL_EVENT_USER"},                      // User-defined event
    {SDL_EVENT_LAST,                      "SDL_EVENT_LAST"}                       // Last event
};

#endif // SDOM_SDL_EVENT_TYPE_NAMES_HPP
