/***  SDOM_EventType.hpp  ****************************
 *   ___ ___   ___  __  __   ___             _  _____                 _              
 *  / __|   \ / _ \|  \/  | | __|_ _____ _ _| ||_   _|  _ _ __  ___  | |_  _ __ _ __ 
 *  \__ \ |) | (_) | |\/| | | _|\ V / -_) ' \  _|| || || | '_ \/ -_)_| ' \| '_ \ '_ \
 *  |___/___/ \___/|_|  |_|_|___|\_/\___|_||_\__||_| \_, | .__/\___(_)_||_| .__/ .__/
 *                       |___|                       |__/|_|              |_|  |_|   
 *  
 * The SDOM_EventType class defines and manages the various types of events that can 
 * be dispatched and handled within the SDOM framework. It provides a flexible system 
 * for registering both predefined and custom event types, each with configurable 
 * propagation properties such as capturing, bubbling, target-only, and global scope. 
 * EventType instances are used to categorize and identify events throughout the event 
 * system, enabling robust event-driven programming and fine-grained control over event 
 * flow. By supporting both standard UI events (mouse, keyboard, window, etc.) and 
 * user-defined events, the EventType class forms the foundation for extensible and 
 * scalable event handling in SDOM-based applications.
 * 
 * 
 *   This software is provided 'as-is', without any express or implied
 *   warranty.  In no event will the authors be held liable for any damages
 *   arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *   including commercial applications, and to alter it and redistribute it
 *   freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not
 *       claim that you wrote the original software. If you use this software
 *       in a product, an acknowledgment in the product documentation would be
 *       appreciated but is not required.
 *   2. Altered source versions must be plainly marked as such, and must not be
 *       misrepresented as being the original software.
 *   3. This notice may not be removed or altered from any source distribution.
 *
 * Released under the ZLIB License.
 * Original Author: Jay Faries (warte67)
 *
 ******************/

#ifndef __SDOM_EVENTTYPE_HPP__
#define __SDOM_EVENTTYPE_HPP__

#include <unordered_set>


namespace SDOM
{
    class EventType
    {
    public:
        // Predefined event types (* denotes completed)
        static EventType None;              // * for no event
        static EventType SDL_Event;         // * for raw SDL events
        static EventType Quit;              // * for application quit events
        static EventType EnterFrame;        // * called every frame (consider dispatching for EventListeners only)
        // Mouse event types    
        static EventType MouseButtonUp;     // * for mouse button up events
        static EventType MouseButtonDown;   // * for mouse button down events
        static EventType MouseWheel;        // * for mouse wheel
        static EventType MouseMove;         // * for mouse movement
        static EventType MouseClick;        // * for single mouse clicks
        static EventType MouseDoubleClick;  // * for double mouse clicks
        static EventType MouseEnter;        // * for mouse entering an object
        static EventType MouseLeave;        // * for mouse leaving an object
        // Stage event types    
        static EventType StageClosed;       // * for when a stage is closed
        // Keyboard event types 
        static EventType KeyDown;           // for when a key is pressed down (includes IME or Shift Modified ascii)
        static EventType KeyUp;             // for when a key is released
        // Timer event types    
        static EventType Timer;             // generic timer event
        static EventType Tick;              // periodic update
        static EventType Timeout;           // one-shot timer expiration event
        // Window event types   
        static EventType FocusGained;       // * for widgets, windows, or controls
        static EventType FocusLost;         // * for widgets, windows, or controls
        static EventType Resize;            // * for window or control size changes
        static EventType Move;              // * for IDisplayObject position changes
        static EventType Show;              // * for visibility changes
        static EventType Hide;              // * for visibility changes
        static EventType EnterFullscreen;   // * for entering fullscreen mode
        static EventType LeaveFullscreen;   // * for leaving fullscreen mode
        // General UI event types
        static EventType ValueChanged;      // for sliders, text fields, etc.
        static EventType StateChanged;      // for checkboxes, radio buttons, etc.
        static EventType SelectionChanged;  // for list boxes, combo boxes, etc.
        static EventType Enabled;           // when IDisplayObjects become enabled
        static EventType Disabled;          // when IDisplayObjects become disabled
        static EventType Active;            // REMOVE ACTIVE
        static EventType Inactive;          // REMOVE INACTIVE
        static EventType Visible;           // when a IDisplayObject becomes visible
        static EventType Hidden;            // when a IDisplayObject becomes hidden        
        // Drag & Drop event types  
        static EventType Drag;              // * when a drag operation starts
        static EventType Dragging;          // * when a drag operation is ongoing
        static EventType Drop;              // * when an item is dropped
        // Clipboard event types    
        static EventType ClipboardCopy;     // when content is copied to the clipboard
        static EventType ClipboardPaste;    // when content is pasted from the clipboard


        // Main DisplayObject event types (NEW EventTypes to be added)
        
        // Application lifecycle event types
        static EventType Added;             // when a DisplayObject is added to the display list
        static EventType Removed;           // when a DisplayObject is removed from the display list
        static EventType AddedToStage;      // when a DisplayObject is added to the stage
        static EventType RemovedFromStage;  // when a DisplayObject is removed from the stage
        // Event Listener Only Events
        static EventType OnInit;            // OnInit is Dispatched to EventListeners
        static EventType OnQuit;            // OnQuit is Dispatched to EventListeners
        static EventType OnEvent;           // OnEvent is Dispatched to EventListeners
        static EventType OnUpdate;          // OnUpdate is Dispatched to EventListeners
        static EventType OnRender;          // OnRender is Dispatched to EventListeners
        static EventType OnPreRender;       // OnPreRender is Dispatched to EventListeners (before stage children render)


        // Custom User event types
        static EventType User;            // custom user event type


               
        explicit EventType(const std::string& name) 
            : name(name), captures_(true), bubbles_(true), targetOnly_(false), global_(false)
        { registerEventType(name, this); }

        explicit EventType(const std::string& name, bool captures, bool bubbles, bool targetOnly, bool global) 
            : name(name), captures_(captures), bubbles_(bubbles), targetOnly_(targetOnly), global_(global) 
        { 
            registerEventType(name, this); 
        }

        const std::string& getName() const { return name; }

        bool operator==(const EventType& other) const { return name == other.name; }
        bool operator!=(const EventType& other) const { return name != other.name; }
        bool operator<(const EventType& other) const { return name < other.name; }

        static void registerEventType(const std::string& name, EventType* ptr) {
            if (registry.find(name) == registry.end()) {
                registry.insert({name, ptr});
            }
        }

        static bool isRegistered(const std::string& name) {
            return registry.find(name) != registry.end();
        }

        // Return the internal registry mapping event name -> EventType*
        static const std::unordered_map<std::string, EventType*>& getRegistry() { return registry; }

    // // Ensure all predefined EventType static instances are inserted into
    // // the registry. Calling this from startup code avoids static
    // // initialization order issues on some platforms.
    // // NOTE: Each EventType constructor already registers itself via
    // // registerEventType(name, this). This helper is therefore
    // // usually redundant, but is retained here to force initialization
    // ///ODR-use of the predefined statics in cases where static
    // // initialization order across translation units could lead to an
    // // empty registry. It is kept for backward-compatibility and safety.
    // static void registerAll();

    // Register EventType usertype/table in a Lua state so scripts can
    // access EventType constants and query properties.
    static void registerLua(sol::state_view lua);

        // getters
        bool getCaptures() const;
        bool getBubbles() const;
        bool getTargetOnly() const;
        bool getGlobal() const;

        // setters
        EventType& setCaptures(bool captures);
        EventType& setBubbles(bool bubbles);
        EventType& setTargetOnly(bool targetOnly);
        EventType& setGlobal(bool global);

    private:
        std::string name;
    static inline std::unordered_map<std::string, EventType*> registry;

        bool captures_ = true;
        bool bubbles_ = true;
        bool targetOnly_ = false;
        bool global_ = false;

    };

} // namespace SDOM

#endif // __SDOM_EVENTTYPE_HPP__