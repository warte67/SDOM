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
- [🧭 Progress Log Legend](#🧭-progress-log-legend)
- [⚙️ Scripting and Configuration](#⚙️-scripting-and-configuration)
- [📈 Progress Updates](#📈-progress-updates)
- [🗓️ Next Steps / To-Do](#🗓️-next-steps--to-do)
- [✅ UnitTest Modules](#✅-unittest-modules)
---

## 🧭 Progress Log Legend

SDOM uses emoji-based status markers to maintain visual and conceptual continuity between its internal test harness and its development logs.  
Each emoji serves as a compact visual cue — highlighting the intent, state, or verification level of a change at a glance.  
This legend ensures that progress entries, test annotations, and lifecycle documentation remain stylistically aligned across all SDOM source files.

| Emoji | Label | Description |
|:------|:------|:-------------|
| 🧩 | **Synthetic Work Accomplished** | Implementation of new systems, features, or internal structures — “new puzzle pieces” added to the SDOM framework. |
| 🧠 | **Lifecycle Changes** | Modifications to paired or mirrored operations — e.g., *startup/shutdown*, *create/destroy*, *add/remove*, *init/quit* — or other system lifecycle refinements. |
| 📜 | **Lua Related Changes** | Additions or adjustments to Lua bindings, scripting integration, or Lua-side functionality exposed through the SDOM API. |
| 🏆 | **Milestone / Achievement** | Major accomplishment or “glorious moment” marking significant project progress — often following successful test completion, refactor completion, or major subsystem stabilization. |
| 🚧 | **To Do / Next Steps** | Planned features, deferred refactors, or upcoming development priorities. |
| 🌟 | **Summary / Highlights** | A concise reflective summary of the day’s progress, key outcomes, or notable discoveries — placed *at the end* of each daily section. |



[🔝 **Back to Table of Contents**](#📑-table-of-contents)

---

## ⚙️ Scripting and Configuration
Lua (via Sol2) is first-class but optional — you can script scenes and behavior in Lua, build everything in C++, or mix both.

The **Factory** accepts `sol::table` configs alongside initializer structs, and **Core**’s lifecycle hooks make it easy to integrate scripts for iteration and tests.

📘 See the dedicated doc: [Scripting and Configuration (Lua + C++)](scripting_and_configuration.md)

[🔝 **Back to Table of Contents**](#📑-table-of-contents)

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

[🔝 **Back to Table of Contents**](#📑-table-of-contents)


## 🗓️ October 29, 2025 — Temporal Testing Framework  
_Introduction of frame-synchronous test execution and temporal validation systems._

### 🧩 Temporal Test System  
- Expanded **README.md** with cross-platform build instructions (Debian, Arch, Fedora, macOS, Windows) and harness screenshots.  
- Integrated **automated versioning** via `gen_version.sh` to auto-update `SDOM_Version.hpp` and the README version block.  
- Refactored all `onUnitTest(int frame)` methods to support frame-synchronous validation across **IDisplayObject** subclasses.  
- Integrated `UnitTests` per-frame execution into `Core::onUpdate()` for synchronized testing, automated shutdown, and performance reporting.  
- Transitioned UnitTests from static to dynamic per-frame scheduling, enabling validation of async and timed behaviors.

### 🧩 Event Verification  
- Completed and verified all **Mouse** and **Keyboard** event tests.  
- Added behavioral coverage for `MouseEnter`, `MouseLeave`, `MouseClick`, and `MouseDoubleClick`.  
- Implemented `KeyDown` and `KeyUp` tests; defined placeholder for `TextInput`.  
- Validated **event propagation model** (capture → target → bubble).  
- Prepared groundwork for **EventManager** modular refactor:  
  - Split `Queue_SDL_Event()` into category-specific handlers.  
  - Optimized dispatch for reduced event duplication.

### 🏆 Milestone  
- First fully functional **frame-synchronous unit test system** completed and verified.  
- Established the temporal testing foundation for all future lifecycle-based validation.

### 🌟 **Summary:**  
- Frame-based testing is now stable and synchronized with the main loop.  
- Event dispatch verified across mouse and keyboard systems — ready for **EventManager** modularization and timer integration.  

---

## 🗓️ October 30, 2025 — EventManager Modularization Prep  
_Preparatory work for breaking down EventManager and verifying input event flow._

### 🧩 EventManager Refactor Design  
- Analyzed event queuing flow within **Queue_SDL_Event()** and identified clear modular boundaries for `dispatchMouseEvents()`, `dispatchKeyboardEvents()`, and `dispatchWindowEvents()`.  
- Documented proposed hierarchy for internal vs. external event queuing (future `Queue_Internal_Event()`).  
- Refined function naming and encapsulation to align with **Core** lifecycle.

### 🧩 Unit Testing Improvements  
- Enhanced test harness formatting for per-event validation logs.  
- Added detailed error reporting to UnitTests for failed behavioral dispatches.  
- Improved visibility into missing **EventType** registrations during startup.

### 🌟 **Summary:**  
- Refactor groundwork complete — event dispatch paths mapped and modularization ready.  
- Improved logging and validation make failures more diagnosable across input systems.  

---

## 🗓️ October 31, 2025 — EventType Registry and Core Integration  
_Expanded event coverage and verified registry consistency across Core systems._  
*(Yes, I worked on Halloween — scary, isn’t it?)*

### 🧩 EventType UnitTests  
- Began implementation of **EventType_UnitTests**.  
- Migrated **Event_test9–12** into this module.  
- Added verification for all registered **EventType**s (`Added`, `Removed`, `EnterFrame`, etc.).  
- Introduced visual test status indicators (emoji legend) to **EventType** interface headers.

### 🧩 Core Configuration and Fullscreen Handling  
- Added **thread-safe / DOM-safe CoreConfig** deferred update system.  
- Integrated `requestConfigApply()` and `applyPendingConfig()` for main-thread-safe configuration changes.  
- Added **temporary F-key** toggle for fullscreen switching in main loop.  
- Improved configuration consistency — `config_ = config;` now ensures runtime sync after reconfigure.  
- Corrected Lua-driven color parsing and propagation.  
- Updated Core border color to `{r=8, g=0, b=16, a=255}` for letterboxing consistency.

### 🐞 Bug Fixes  
- Fixed double-configuration bug — **Core::configure()** now executes once per session initialization.

### 🏆 Milestone  
- Verified **EventType registry stability** and established **safe, deferred Core reconfiguration**.  
- Marked the first day SDOM could toggle fullscreen dynamically without destabilizing the DOM.

### 🌟 **Summary:**  
- EventType registry now validated; Core reconfiguration is thread-safe and predictable.  
- Foundation laid for future window mutators and fullscreen event handling.  

---

## 🗓️ November 1, 2025 — EventType UnitTests  
_Extended EventType testing, configuration stability, and fullscreen handling refinements._

### 🧩 EventType UnitTests  
- Continued expansion of **EventType_UnitTests** coverage.  
- Migrated **Event_test9–12** into this module.  
- Verified initialization and registry correctness for all defined `EventType` entries.  
- Added groundwork for classification by **Test Mode** (🧩 Synthetic / 🧠 Lifetime / 📜 Lua).  
- Began establishing coverage markers across lifecycle, input, and drag-drop events.  

### 🧩 Core Configuration and Fullscreen Handling  
- Extended support for **thread-safe CoreConfig** changes.  
- Verified fullscreen toggle stability through deferred reconfiguration.  
- Improved Lua config parsing and synchronization with Core runtime state.  
- Updated internal border color and scaling logic for consistent rendering during resize or fullscreen transitions.

### 🐞 Bug Fixes  
- Fixed redundant Core configuration sequence during startup.  
- Ensured proper resource recreation chain (Window → Renderer → Texture).  

### 🌟 **Summary:**  
- EventType framework scaffolding complete; individual event categories now systematically tracked by test mode.  
- Core configuration and fullscreen logic stabilized in preparation for advanced event mutator integration.  

---
<a id="latest-update"></a>

## 🗓️ November 2, 2025 — Event System and Core Configuration Refinements  
_Refactoring event dispatch verification and introducing deferred, thread-safe CoreConfig updates._

### 🧩 SDOM Window Event Mutators  
- Added design notes for future mutators controlling fullscreen / windowed state.  
- Confirmed all window and renderer reconfiguration routes through **Core::setConfig() → Core::reconfigure()**.  
- Identified **requestConfigApply()** / **applyPendingConfig()** as proper deferred-update mechanisms for DOM-safe, thread-safe resource rebuilds.  
- Ensures SDL resources are cleanly recreated without disrupting frame traversal.

### 🧩 Event Testing and Fixes  
- Refactored `runEventBehaviorTest()` → `UnitTests::run_event_behavior_test()` for naming consistency and stronger encapsulation.  
  - Replaced temporary **Box** test object with native **Frame** for SDOM compatibility.  
  - Added focus setup for keyboard and mouse via **core.setKeyboardFocusedObject()** and **core.setMouseHoveredObject()**.  
  - Fixed regression where mouse button events failed due to stricter click-target logic — resolved by setting test object as clickable (`setClickable(true)`).  
- Verified behavioral tests now correctly dispatch **MouseDown**, **MouseUp**, **Click**, and **DoubleClick** without side effects.

### 🏆 Milestone  
- Achieved full **Core–EventManager integration stability** — input events and configuration changes now coexist safely under the same deferred system.  
- SDOM’s event system officially enters **post-stabilization phase** for UI interaction.

### 🌟 **Summary:**  
- Core and EventManager internals are now safer and cleaner — configuration changes are deferred properly, event behavior tests pass consistently, and the framework is ready for more advanced input testing across windows and devices.  

### 🗓️ To-Do For Today
- 🔄 **Complete Core Property Accessors and Mutators**
  - 🔄 Fullscreen *(behavior tested in EventType_UnitTests)*  
    - 🔄 **Core::isFullscreen()**
    - 🔄 **Core::setFullscreen()**
  - 🔄 Windowed *(behavior tested in EventType_UnitTests)*  
    - 🔄 **Core::isWindowed()**
    - 🔄 **Core::setWindowed()**




#### end-of-day

[🔝 **Back to Table of Contents**](#📑-table-of-contents)

---

## 🚧 Next Steps / To-Do

- 🔄 **Create new `EventType_UnitTests` module**  
  - ✅ Migrate `Event_test9–12` into this module  
  - ✅ Move earlier keyboard reentrant tests under `EventType_UnitTests`  
  - 🔄 Expand coverage for all registered `EventType`s (`Added`, `Removed`, `EnterFrame`, etc.)

- ✅ **Add tests for input dispatch edge cases**  
  - ✅ Mouse enter/leave behavior on overlapping objects  
  - ✅ Keyboard focus transition events

- ☐ **Implement new EditBox / IME input system**  
  - ☐ Implement clipboard and text input events *(future `EditBox` support)*

- ✅ **Review deferred `addChild()` / `removeChild()` event timing**  
  - ✅ Ensure consistent dispatch occurs after traversal completes

- 🔄 **Begin performance profiling** for event propagation and queue depth  
  - ⚠️ Current profiling results appear incorrect; investigate methodology and sampling

- ☐ 🔧 **Add output suppression flag**  
  - ☐ Introduce `quiet` or `minimal` mode for UnitTest reports  
  - ☐ Display detailed logs only when failures occur  
  - ☐ Reduce report noise and keep summary results concise


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

[🔝 **Back to Table of Contents**](#📑-table-of-contents)

---

## ✅ UnitTest Modules
Each **UnitTest module** in SDOM represents a focused validation target for a specific subsystem, interface, or control type.  To maintain consistency with SDOM’s internal test harness and development reports, each module is tracked with a concise emoji marker reflecting its current implementation and verification state.  Because SDOM remains in a **pre-alpha** stage, these modules are still evolving — new tests are frequently added, refactored, or reorganized as the underlying architecture stabilizes.  This matrix provides an at-a-glance overview of test coverage, helping developers quickly identify which systems are stable, under review, or awaiting implementation.  

<div style="display: flex; flex-wrap: wrap; justify-content: center; max-width: 950px; margin: auto; gap: 6px;">

<div style="flex: 1; min-width: 280px; margin: 2px;">

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

</div>

<div style="flex: 1; min-width: 280px; margin: 2px;">

| Module | Status |
|:----------------|:-------:|
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

</div>

<div style="flex: 1; min-width: 280px; margin: 2px;">

| Module | Status |
|:----------------|:-------:|
| RadioButton | ☐ |
| ScrollBar | ☐ |
| Slider | ☐ |
| SpriteSheet | ☐ |
| Stage | ☐ |
| Texture | ☐ |
| TristateButton | ☐ |
| TruetypeFont | ☐ |
| TTFAsset | ☐ |

</div>
</div>

- ☐ **Not Yet Implemented** — Placeholder for future tests; structure defined but functionality not yet added.  
- 🔄 **In Progress** — Test is under development or currently being debugged; results are not yet stable.  
- ⚠️ **Failing / Regression** — Test implemented but failing or producing inconsistent results; pending fix or system dependency.  
- ✅ **Verified** — Test has passed all validation modes (synthetic, lifecycle, Lua); stable and reliable.  

[🔝 **Back to Table of Contents**](#📑-table-of-contents)

