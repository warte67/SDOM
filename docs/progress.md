# 🧩 SDOM Project Progress Log

SDOM is a **C++23** library built on **SDL3** that brings a DOM-like scene graph and GUI toolkit to 2D applications and games.  
The aim is a small, predictable core for layout, events, and rendering so SDL developers can compose interfaces and in-game UI with familiar patterns and a minimal API surface.

## 🧱 Architecture Overview
A single **Core** orchestrates the SDL runtime (window, renderer, main loop).  
A **Stage** serves as the root of the display tree, while an **EventManager** dispatches input through *capture → target → bubble* phases.  
The **Factory** registers, creates, and tracks resources and display objects by type and unique name, returning typed handles for safe references.  
Layout is driven by an **edge-based anchoring system** that supports asymmetric, responsive designs.

Scripting and configuration are first-class with **Lua via Sol2**, enabling data-driven scenes, rapid iteration, and test automation.  
You can also build everything directly in C++ — Lua is optional — but when enabled it can construct objects, wire scenes, drive dialog, and swap stages at runtime without recompilation.

## 🧠 Documentation Philosophy
SDOM emphasizes clear, living documentation. The Markdown design docs are working papers that evolve with the code — expect them to change as features land and APIs are refined. 

Public APIs are defined in headers and surfaced via Doxygen, with correctness exercised by unit tests.  
If a doc briefly diverges from current behavior, treat the **code as source-of-truth** until the docs catch up.

Diagrams are authored in Mermaid and exported to static SVG/PNG by the repo’s export pipeline, keeping visuals portable while ensuring sources remain editable. Progress updates call out notable documentation changes, and contributions to clarify or correct docs are welcome.

## 📑 Table of Contents
- [📅 Latest Update](#latest-update)
- [⚙️ Scripting and Configuration](#⚙️-scripting-and-configuration)
- [📈 Progress Updates](#📈-progress-updates)
- [✅ UnitTest Modules](#✅-unittest-modules)
---

## ⚙️ Scripting and Configuration
Lua (via Sol2) is first-class but optional — you can script scenes and behavior in Lua, build everything in C++, or mix both.

The **Factory** accepts `sol::table` configs alongside initializer structs, and **Core**’s lifecycle hooks make it easy to integrate scripts for iteration and tests.

📘 See the dedicated doc: [Scripting and Configuration (Lua + C++)](scripting_and_configuration.md)

---

# 📈 Progress Updates

> 🗂️ Older entries are archived in [archive_progress.md](archive_progress.md)
---

# Progress Updates

Note: Older entries are archived in [archive_progress.md](archive_progress.md).

## Previous Progress Updates
- [October 28, 2025](archive_progress.md#october-28-2025) — **Event_UnitTests** Progress:
- [October 27, 2025](archive_progress.md#october-27-2025) — Added **clearKeyboardFocusedObject()** and **clearMouseHoveredObject()** to Core C++, Lua bindings and unit tests.
- [October 25, 2025](archive_progress.md#october-25-2025) — **IDisplayObject_UnitTests** -- Added Unit Tests for the following **IDisplayObject** Lua bindings:
- [October 24, 2025](archive_progress.md#october-24-2025) — **IDisplayObject_UnitTests** Progress:
- [October 23, 2025](archive_progress.md#october-23-2025) — **`Core_UnitTests` Refactor Progress:**
- [October 22, 2025](archive_progress.md#october-22-2025) — **`Core_UnitTests` Refactor Progress:**
- [October 21, 2025](archive_progress.md#october-21-2025) — **Major issues encountered with the dual Lua binding methodology**
- [October 20, 2025](archive_progress.md#october-20-2025) — Implemented a C++ per-type Lua binding registry (`LuaBindingRegistry`) and a small helper (`register_per_type`) to register protected functions reliably.
- [October 19, 2025](archive_progress.md#october-19-2025) — Added `DisplayHandle::ensure_type_bind_table(...)` and a `SDOM_Bindings` registry.
- [October 18, 2025](archive_progress.md#october-18-2025) — **Universally support multiple icon set sizes:**
- [October 17, 2025](archive_progress.md#october-17-2025) — **ScrollBar:**
- [October 16, 2025](archive_progress.md#october-16-2025) — **ArrowButton**
- [October 15, 2025](archive_progress.md#october-15-2025) — **Library Maintenance**
- [October 14, 2025](archive_progress.md#october-14-2025) — Added `Frame` scaffolding and registered the `Frame` DOM type with the Factory; SDOM_Frame.cpp implements minimal constructors and defers behavior to `IPanelObject`.
- [October 13, 2025](archive_progress.md#october-13-2025) — **Added inline numeric style escapes to Label tokenization:**
- [October 12, 2025](archive_progress.md#october-12-2025) — **Internal Resource Renaming:**
- [October 11, 2025](archive_progress.md#october-11-2025) — **BitmapFont & TruetypeFont Resource Creation Improvements:**
- [October 9, 2025](archive_progress.md#october-9-2025) — Codebase rename:
- [October 8, 2025](archive_progress.md#october-8-2025) — drawSprite Lua API / overload consolidation
- [October 7, 2025](archive_progress.md#october-7-2025) — cleaned up the heavy includes from the header files to reduce compile times and improve modularity.
- [October 6, 2025](archive_progress.md#october-6-2025) — Lua binding refactor (IDisplayObject):
- [October 5, 2025](archive_progress.md#october-5-2025) — Refactored Lua Core bindings:
- [October 4, 2025](archive_progress.md#october-4-2025) — Quick fixes & Lua binding polish (morning)
- [October 3, 2025](archive_progress.md#october-3-2025) — Cleaned up noisy debug output across the codebase:
- [October 2, 2025](archive_progress.md#october-2-2025) — **Synthetic Event Handling Robustness:**
- [September 30, 2025](archive_progress.md#september-30-2025) — **Summary:**
- [September 29, 2025](archive_progress.md#september-29-2025) — **Lua Binding Debugging:**
- [September 28, 2025](archive_progress.md#september-28-2025) — **Lua Binding Improvements:**
- [September 27, 2025](archive_progress.md#september-27-2025) — **Lua Integration Complete:**
- [September 26, 2025](archive_progress.md#september-26-2025) — **Box Unit Tests Complete:**
- [September 25, 2025](archive_progress.md#september-25-2025) — **JSON → Lua Transition Complete:**
- [September 24, 2025](archive_progress.md#september-24-2025) — **Event System Debugging:**
- [September 23, 2025](archive_progress.md#september-23-2025) — **Event System Preliminary Testing:**
- [September 22, 2025](archive_progress.md#september-22-2025) — **Unit Test Framework Refined:**
- [September 21, 2025](archive_progress.md#september-21-2025) — Renamed `resource_ptr` to `ResourceHandle` throughout the codebase for clarity.
- [September 20, 2025](archive_progress.md#september-20-2025) — Factory and resource_ptr scaffolding completed; code compiles and runs cleanly.
- [September 19, 2025](archive_progress.md#september-19-2025) — Proof of concept approved. Main development approved. Work begins officially.
- [August 5, 2025](archive_progress.md#august-5-2025) — SDOM secondary rough draft
- [July 2, 2025](archive_progress.md#july-2-2025) — Initial rough draft and proof of concept.


## 🗓️ October 29, 2025 — Temporal Testing Framework
- **README.md** expanded with clearer build instructions for Debian, Arch, Fedora, macOS, and Windows, plus a screenshot of the Test Harness.  
- Integrated version metadata into README.md.  
- **Automated versioning:** Implemented `gen_version.sh` to auto-generate `SDOM_Version.hpp` and update the README version block during builds.  
- **Unit test refactor:** Added a `frame` parameter to all `onUnitTest(int frame)` methods across **IDisplayObject** subclasses to support frame-synchronous testing.  
- **Per-frame test harness:** The `UnitTests` system now runs tests sequentially per frame, logs formatted pass/fail output, and provides detailed summaries.  
- **Core update integration:** Integrated per-frame unit test execution into `Core::onUpdate()`, including automated shutdown and performance reporting.  
- Transitioned UnitTests from static to frame-synchronous scheduling, enabling validation of timers, multithreaded ops, and async subsystems.

- Completed implementation and verification of all **Mouse** and **Keyboard** event tests.  
- Added behavioral tests for `MouseEnter`, `MouseLeave`, `MouseClick`, and `MouseDoubleClick`.  
- Implemented new keyboard tests for `KeyDown` and `KeyUp` (placeholder for `TextInput`).  
- Reviewed and validated the **event propagation model** (capture → target → bubble).  
- Confirmed readiness for `EventManager` refactor:
  - Split `Queue_SDL_Event()` into modular per-category handlers  
  - Optimize event dispatch paths to reduce redundant copies  
- **Next Steps:**
  - Refactor and clean up `EventManager`
  - Introduce `Queue_Internal_Event()` for non-SDL sources
  - Implement **Timer Objects** with global/local dispatch

🧾 **Summary:** Temporal testing system complete; event verification and EventManager refactor prep finalized.


---
## 🗓️ October 30, 2025 — Multi-Frame Unit Tests
- Updated `Event_UnitTests` to validate `EventManager` functionality and performance.
- Updated the SDOM_UnitTests framework to support **multi-frame tests** and performance metrics.
- Confirmed that UnitTests are now **re-entrant** and can span multiple frames.

🧾 **Summary:** Multi-frame test harness verified; event timing logic validated.

---
## 🗓️ October 31, 2025 — 🎃 Core Refactor & Stage Lifecycle Integration

(Yes, I worked on Halloween — scary, isn’t it?)

#### 1️⃣ EventManager Refactor
- Split `Queue_SDL_Event()` into modular sub-dispatchers:
  - `dispatchWindowEvents`, `dispatchKeyboardEvents`, `dispatchMouseEvents`, `dispatchDragEvents`
  - `updateHoverState`, `dispatchWindowEnterLeave`
- Each handles one input domain (window, keyboard, mouse, drag, etc.)
- Reduced event copies, improved readability, and simplified debug flow

#### 2️⃣ Event Dispatch Optimization
- Unified coordinate normalization by event type
- Consistent hit-testing in logical stage space
- Reduced allocations with in-place forwarding
- Simplified `mouseX` / `mouseY` update logic

#### 3️⃣ Drag & Keyboard Improvements
- Fixed phantom drags from uninitialized seeds
- Drag now starts only after valid press + motion
- Added `Drag`, `Dragging`, and `Drop` event handling
- Restored hover stability
- Keyboard events now target focused objects or broadcast globally

#### 4️⃣ Stage Lifecycle Events
- Added `StageOpened` and `StageClosed` support
- Fired automatically in `Core::setRootNode()` on stage switch
- Verified event order and propagation between stages
- Garbage collection confirmed stable during transitions

### 🧠 Unit Tests

| Test | Description | Status |
|:----:|:-------------|:------:|
| `Event_test8` | Mouse behavior verification | ✅ |
| `Event_test9` | Keyboard behavior verification | ✅ |
| `Event_test10` | Multi-frame queue verification | ✅ |
| `Event_test11` | Lifecycle event dispatch verification | ✅ |
| `Event_test12` | Stage lifecycle transition verification | ✅ |

🧾 **Summary:**  
EventManager modularized, drag & keyboard routing fixed, stage lifecycle events integrated, and all core tests passing.  
Next step → migrate tests 9–12 into a new `EventType_UnitTests` module for full event-type coverage.

---
<a id="latest-update"></a>

## 🗓️ November 1, 2025 — EventType UnitTests
- Began work on **EventType_UnitTests** module
- Migrated **Event_test9–12**`** to this module
- Added coverage for all registered **EventType**s (**Added**, **Removed**, **EnterFrame**, etc.)
- Introduced status emoticons for each test type within the EventType class interface


- Added thread safe / DOM safe CoreConfig change requests
  - Temporarily added a **F** key shortcut to toggle fullscreen in the core main loop


#### end-of-day

---

### ✅ Next Steps / To-Do
- [ ] Create new **`EventType_UnitTests`** module  
  - Migrate `Event_test9–12` to this module  
  - Add coverage for all registered `EventType`s (`Added`, `Removed`, `EnterFrame`, etc.)
- [ ] Add tests for input dispatch edge cases  
  - Mouse enter/leave on overlapping objects  
  - Keyboard focus transitions  
- [ ] Implement clipboard and text input events (future `EditBox` support)
- [ ] Review deferred `addChild()` / `removeChild()` event timing  
  - Ensure consistent dispatch after traversal completes  
- [ ] Begin performance profiling for event propagation and queue depth
- [ ] 🔧 **Add output suppression flag**  
  - Introduce a `quiet` or `minimal` mode for UnitTest reports  
  - Only display detailed logs or system-level test output when failures occur  
  - Helps reduce report noise and keep summary results concise



### 🧪 Memory Validation
---
```bash
valgrind --leak-check=full ./prog --stop_after_tests
==136007== LEAK SUMMARY:
==136007==    definitely lost: 0 bytes in 0 blocks
==136007==    indirectly lost: 0 bytes in 0 blocks
==136007==      possibly lost: 0 bytes in 0 blocks
==136007==    still reachable: 287,117 bytes in 3,600 blocks
==136007==         suppressed: 0 bytes in 0 blocks
```


### 🧬 Current DisplayObject Inheritance Hierarchy
---
```
─── IDisplayObject
    ├── IPanelObject
    │   ├── Button
    │   ├── Frame
    │   └── Group
    ├── IRangeControl
    │   ├── IRangeControl_scaffold
    │   ├── ProgressBar
    │   ├── ScrollBar
    │   └── Slider
    ├── IconButton
    │   └── ArrowButton
    ├── Label
    ├── Stage
    └── TristateButton
        ├── CheckButton
        └── RadioButton
```

## ✅ UnitTest Modules

| Module | Status |
|:----------------|:-------:|
| ArrowButton | ☐ |
| AssetHandle | ☐ |
| BitmapFont | ☐ |
| Button | ☐ |
| CheckButton | ☐ |
| CLR | ☐ |
| Core | ✅ |
| DisplayHandle | ☐ |
| Event | 🔄 |
| EventManager | 🔄 |
| EventType | 🔄 |
| Factory | ☐ |
| Frame | ☐ |
| Group | ☐ |
| IAssetObject | ☐ |
| IButtonObject | ☐ |
| IconButton | ☐ |
| IDataObject | ☐ |
| IDisplayObject | ✅ |
| IFontObject | ☐ |
| IPanelObject | ☐ |
| IRangeControl | ☐ |
| Label | ☐ |
| ProgressBar | ☐ |
| RadioButton | ☐ |
| ScrollBar | ☐ |
| Slider | ☐ |
| SpriteSheet | ☐ |
| Stage | ☐ |
| Texture | ☐ |
| TristateButton | ☐ |
| TruetypeFont | ☐ |
| TTFAsset | ☐ |
