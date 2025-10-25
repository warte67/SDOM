# Events Design Document

## Overview
SDOM::Events are designed to provide a robust and extensible mechanism for event handling within the SDOM framework. These events will be derived from SDL_Events and will propagate through the DOM hierarchy, ensuring that all relevant objects receive and process events appropriately.

## Goals
- **Seamless Conversion**: Convert SDL_Events into SDOM::Events within the EventManager.
- **Propagation**: Ensure events propagate correctly through the DOM hierarchy.
- **Extensibility**: Allow for custom event types and handlers.
- **Performance**: Minimize overhead during event processing.

## Event Structure
SDOM::Events will encapsulate the following:
- **Type**: The type of the event (e.g., mouse click, key press).
- **Source**: The originating object or stage.
- **Target**: The intended recipient of the event.
- **Data**: Additional data associated with the event (e.g., mouse coordinates, key codes).
- **Timestamp**: The time at which the event occurred.

## Event Lifecycle
1. **Creation**: Events are created by converting SDL_Events within the EventManager.
2. **Queueing**: Events are added to the EventQueue for processing.
3. **Dispatching**: Events are dispatched to the appropriate targets.
4. **Handling**: Targets process the events using registered handlers.

## Event Types
SDOM::Events will support the following types:
- **Mouse Events**: Click, move, scroll.
- **Keyboard Events**: Key press, key release.
- **Window Events**: Resize, close, focus.
- **Custom Events**: User-defined events for specific application needs.

## Predefined Event Types
The `EventType` class defines a number of predefined events for use within the SDOM framework. Below is a categorized list:

### General Events
- `None`: Represents no event.
- `SDL_Event`: Generic SDL event.
- `Quit`: Application quit event.
- `EnterFrame`: Frame enter event.

### Mouse Events
- `MouseButtonUp`: Mouse button released.
- `MouseButtonDown`: Mouse button pressed.
- `MouseWheel`: Mouse wheel scrolled.
- `MouseMove`: Mouse moved.
- `MouseClick`: Mouse clicked.
- `MouseDoubleClick`: Mouse double-clicked.
- `MouseEnter`: Represents the event when the mouse enters a target.
- `MouseLeave`: Represents the event when the mouse leaves a target.

### Keyboard Events
- `KeyDown`: Key pressed.
- `KeyUp`: Key released.
- `TextInput`: Text input event.

### Timer Events
- `Timer`: Timer event.
- `Tick`: Tick event.
- `Timeout`: Timeout event.

### UI Events
- `FocusGained`: Focus gained.
- `FocusLost`: Focus lost.
- `Resize`: Window resized.
- `Move`: Window moved.
- `Show`: Window shown.
- `Hide`: Window hidden.
- `ValueChanged`: Value changed.
- `StateChanged`: State changed.

### Drag & Drop Events
- `Drag`: Drag started.
- `Dragging`: Drag in progress.
- `Drop`: Drop event.

### Clipboard Events
- `ClipboardCopy`: Clipboard copy.
- `ClipboardPaste`: Clipboard paste.

### Custom Events
- `User`: User-defined event.

This list provides a foundation for event handling and can be extended as needed.

## EventManager Responsibilities
- **Queue_SDL_Event()**: Convert SDL_Events into SDOM::Events and add them to the EventQueue.
- **DispatchEvents()**: Process the EventQueue and propagate events through the DOM hierarchy.
- **RegisterHandlers()**: Allow objects to register event handlers for specific event types.

## Existing Implementation
The `SDOM::Event` class is already present in the codebase, though it is not fully complete. This document will guide its refinement and integration into the event handling system.

## Next Steps
1. Implement the SDOM::Event class.
2. Develop the EventManager::Queue_SDL_Event() method.
3. Create the EventQueue for managing events.
4. Implement event propagation and handling logic.

## Notes
- Ensure thread safety for event processing.
- Optimize for high-frequency events like mouse movement.
- Test event propagation using the three boxes initialized from JSON.

---

# Event System Design: Categorizing EventTypes

## Proposal
Create a centralized table for `EventTypes` with characteristic flags to define their default behavior. This table would indicate whether an event:
- Captures by default
- Bubbles by default
- Is target-only
- Is global by default

## Advantages
1. **Centralized Configuration**:
   - Ensures consistent behavior across the codebase.
2. **Extensibility**:
   - Simplifies adding or modifying event types.
3. **Improved Readability**:
   - Provides a clear overview of event behaviors.
4. **Dynamic Behavior**:
   - Allows runtime adjustments for flexible systems.
5. **Performance Optimization**:
   - Streamlines event dispatching by categorizing events.

## Suggested Implementation
Define a structure to represent event characteristics:
```cpp
struct EventTypeProperties {
    bool isCapturing;  // Captures by default
    bool isBubbling;   // Bubbles by default
    bool isTargetOnly; // Target-only
    bool isGlobal;     // Global by default
};

// Example table for EventTypes
std::unordered_map<EventType, EventTypeProperties> eventTypeTable = {
    {EventType::FocusLost, {true, false, true, false}},
    {EventType::FocusGained, {true, false, true, false}},
    {EventType::Quit, {false, false, false, true}},
    {EventType::EnterFrame, {false, false, false, true}},
    {EventType::SDL_Event, {false, true, false, false}},
    // Add more EventTypes as needed
};
```
## Integration
1. **Event Dispatch Logic:** Use the table to determine how events are handled by default:
``` cpp 
auto properties = eventTypeTable[event.getType()];
if (properties.isGlobal) {
    dispatchGlobalEvent(event);
} else if (properties.isCapturing) {
    dispatchCapturingEvent(event);
} else if (properties.isBubbling) {
    dispatchBubblingEvent(event);
} else if (properties.isTargetOnly) {
    dispatchTargetEvent(event);
}
```
2. **Validation:** Ensure all `EventTypes` are covered in the table.
3. **Documentation:** Clearly document the table and its usage.
## Considerations
- **Default Behavior:** Decide if undefined types should have a default behavior or throw an error.
- **Performance:** Optimize table lookups if necessary.
- **Testing:** Test edge cases, such as conflicting flags (e.g. both isGlobal and isTargetOnly).
- **JSON Scalability:** Ensure the implementation of all required JSON properties for each `EventType`.


---

### Listener-Specific Events

Certain events, such as `EnterFrame`, `Timer`, `Tick`, and `Timeout`, are designed to only respond to `EventListeners` that explicitly register for them. These events have all propagation behaviors (`Captures`, `Bubbles`, `TargetOnly`, `Global`) set to `false` in the `eventTypeTable`. This indicates that they bypass normal propagation and are dispatched directly to registered listeners.

#### Example Behavior
- **EnterFrame**: Used for frame-based updates, dispatched only to listeners that request it.
- **Timer**: Dispatched when a timer completes, but only to registered listeners.
- **Tick**: Periodic updates, sent directly to listeners.
- **Timeout**: One-shot timer expiration, sent to specific listeners.

#### Integration
The `eventTypeTable` defines these events as:
```cpp
{ EventType::EnterFrame, { false, false, false, false }},
{ EventType::Timer,      { false, false, false, false }},
{ EventType::Tick,       { false, false, false, false }},
{ EventType::Timeout,    { false, false, false, false }},
```

During event dispatch, the system checks these flags and directly notifies registered listeners without propagating through the DOM hierarchy.

---
This document will evolve as the design and implementation progress.
