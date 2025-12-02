# SDOM Main Loop – Phase System

This document defines the **final, robust, self-healing SDOM main-loop model**, updated to include:

- **Two-tier event intake** (internal SDOM events first, then external SDL events)
- **Any-order-safe phase calls**
- **Automatic state healing** across frames
- **Strict frame-boundary rules**
- **`SDOM_PollEvents(SDOM_Event* evt)` with event-out semantics**
- **Deterministic, unbreakable behavior**

---

# 1. Overview

SDOM defines **five independently callable phases**, each of which may be invoked:

- at any time  
- in any order  
- repeatedly  
- or unexpectedly (user code, script, or host application)

SDOM guarantees:

### ✔ No undefined behavior  
### ✔ No crashes  
### ✔ Always self-healing  
### ✔ Always completes required phases  
### ✔ Always maintains consistent frame state  

The five phases:

1. **Poll Events**  
2. **Update**  
3. **Render**  
4. **Present**  
5. **Collect Garbage**

Each phase:

- detects missing prerequisites
- auto-runs missing phases in the correct order
- returns `false` with an error describing the misuse
- continues execution safely

---

# 2. Phase Function Signatures

```c
bool SDOM_PollEvents(SDOM_Event* evt);
bool SDOM_Update(void);
bool SDOM_Render(void);
bool SDOM_Present(void);
bool SDOM_CollectGarbage(void);
bool SDOM_RunFrame(void);
const char* SDOM_GetError(void);
```

---

# 3. Frame-State Flags

```cpp
bool bEventsWerePolled;
bool bWasUpdated;
bool bWasRendered;
bool bGarbageCollected;
```

### Flag Rules

- Flags are **set before execution** of their phase.
- Flags are **reset only by Present()**.
- If a phase must auto-run prerequisites, it returns `false` (non-fatal).
- A phase that has already run this frame returns `true`.

---

# 4. Global Self-Healing Rule

Before running its own logic, **every phase except GC** must:

### 1. Ensure the *previous frame* was completed  
If not, auto-run missing phases:

```
Update → Render → Present
```

### 2. Ensure *current-frame prerequisites* are fulfilled  
Missing prerequisites are auto-run.

### 3. If auto-running was required  
- set an error string  
- return `false`  
- continue safely  

---

# 5. Two-Tier Event Model (IMPORTANT)

SDOM events come from **two sources**:

## A. Internal SDOM Event Queue (highest priority)
- Synthetic DOM events  
- Lifecycle events  
- Hover/focus changes  
- async/scheduled events  
- events generated during dispatch  
- structural events (attach/detach/reparent)

## B. External SDL events (second priority)
- mouse/pointer  
- keyboard  
- controller  
- window  
- OS signals  

### ✔ Internal events must ALWAYS be processed before external SDL events.

This ensures:

- deterministic ordering  
- DOM correctness  
- hover/focus consistency  
- no starvation of internal events  
- proper frame isolation  

---

# 6. SDOM_PollEvents (Updated Definition)

```c
bool SDOM_PollEvents(SDOM_Event* evt);
```

### Purpose:
- Heal last frame if needed
- Pump exactly **one** event per call
- Prefer internal SDOM events first
- Convert & dispatch SDL events second
- Output processed event to caller

### Prerequisites:
- Must ensure last frame is completed
- No current-frame prerequisites

---

## 6.1 Full Behavior

```
[1] Heal previous frame if incomplete:
      Update → Render → Present

[2] Internal SDOM event queue check:
      if internalQueue has events:
           evt = pop internal event
           dispatch internally
           bEventsWerePolled = true
           return true

[3] External SDL queue check:
      if SDL_PollEvent(&raw) == 1:
           convert raw → SDOM_Event
           evt = converted event
           dispatch internally
           bEventsWerePolled = true
           return true

[4] No events in either queue:
      return false (no error)
```

---

# 7. Update Phase

```c
bool SDOM_Update(void);
```

### Prerequisites:
- PollEvents (this frame)

### Behavior:
- Heal previous frame if needed
- Ensure PollEvents has run
- Set bWasUpdated
- Execute DOM logic, lifecycle transitions
- Return true unless fatal error

---

# 8. Render Phase

```c
bool SDOM_Render(void);
```

### Prerequisites:
- PollEvents
- Update

### Behavior:
- Heal previous frame if needed
- Auto-run Update if missing  
  → error: "SDOM_Render called before SDOM_Update"
- Set bWasRendered
- Render tree to internal texture
- Return true

---

# 9. Present Phase

```c
bool SDOM_Present(void);
```

### Prerequisites:
- PollEvents  
- Update  
- Render  
- GC at least once

### Behavior:

1. Heal previous frame  
2. Auto-run missing Poll → Update → Render  
3. Present internal texture  
4. Auto-GC if missing  
5. Reset all flags (start new logical frame)  
6. Return true  

---

# 10. CollectGarbage Phase

```c
bool SDOM_CollectGarbage(void);
```

### Prerequisites:
- None

### Behavior:
- Heal previous frame  
- Set bGarbageCollected  
- Destroy unreachable objects  
- Always return true  
- Reentrant and idempotent  

---

# 11. SDOM_RunFrame

```c
bool SDOM_RunFrame(void);
```

Equivalent to:

```
Update → Render → Present
```

Phases heal themselves exactly as the user-facing ones do.

---

# 12. Error Handling

Out-of-order calls:

- are **never fatal**
- produce an error message
- return `false`
- but still auto-correct and execute safely

Retrieve error with:

```c
const char* SDOM_GetError(void);
```

Example:

```c
SDOM_Render(); // error: "SDOM_Render called before SDOM_Update"
SDOM_Update(); // returns false (already ran)
```

The app continues normally.

---

# 13. Initialization Requirements

Inside `SDOM_Init()`:

```cpp
bEventsWerePolled = false;
bWasUpdated = false;
bWasRendered = false;
bGarbageCollected = false;
```

---

# 14. Thread Safety

- All phases must run on the same thread.
- Cross-thread changes must be deferred.

---

# 15. API Summary Table

| Function               | Return | Prerequisites         | Behavior |
|------------------------|--------|------------------------|----------|
| SDOM_PollEvents(evt)   | bool   | complete last frame    | Pump internal SDOM event first; then SDL |
| SDOM_Update()          | bool   | Poll                   | Update DOM tree |
| SDOM_Render()          | bool   | Poll, Update           | Render to texture |
| SDOM_Present()         | bool   | Poll, Update, Render   | Present + GC + reset |
| SDOM_CollectGarbage()  | bool   | complete last frame    | Destroy orphans |
| SDOM_RunFrame()        | bool   | —                      | Update → Render → Present |
| SDOM_GetError()        | char*  | —                      | Retrieve last error |

---

# 16. Guarantees

1. **Calling phases in any order never breaks SDOM**  
2. **Internal queue is always processed before external SDL events**  
3. **Missing phases are auto-run to maintain correctness**  
4. **Sequence violations return false but continue safely**  
5. **Frame boundaries are always respected**  
6. **Deterministic and fully testable behavior**  

---

# 17. Example of Extreme Misordered Calls

```c
SDOM_Render();      // auto → Update → PollEvents
SDOM_CollectGarbage();
SDOM_Update();      // already ran → return false
SDOM_PollEvents(&evt);
SDOM_Present();     // auto-completes remaining work
```

SDOM heals itself into a correct sequence:

```
Poll → Update → Render → Present
```

Every time, consistently.

---

# End of Document
