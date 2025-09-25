# Scripting and Configuration (Lua + C++)

SDOM supports two complementary ways to build apps: directly in C++ and via Lua scripting (powered by Sol2). Lua is first‑class but optional—you can adopt it incrementally or run entirely without it. When enabled, Lua accelerates iteration, enables data‑driven scenes, and allows runtime behavior changes without recompilation.

## Goals
- Fast iteration and prototyping for UI, scenes, and behaviors
- Data‑driven creation of resources and display objects
- Runtime stage/scene swapping and dialog scripting
- Test harnesses that exercise systems without rebuilding

## Modes of Use
- C++‑only: Construct everything with initializer structs and APIs.
- C++ + Lua (hybrid): Use Lua for data/config and glue; keep heavy logic in C++.
- Lua‑driven: Define scenes and behavior in Lua; C++ provides performant primitives.

## Integration Points
- Factory: Types register creator lambdas that accept either a type‑specific initializer struct or a Lua `sol::table`.
- Constructors: Each type offers `Type(const InitType&)` and `Type(const sol::table&)` protected/ private constructors; Factory is a friend.
- Lifecycle: Core exposes hooks (init, update, event, render, unit tests) that can call into Lua and vice‑versa.
- Scenes: Swap the active Stage (DOM root) at runtime from C++ or Lua.

## Example: Creating with Lua
```cpp
sol::state lua;
lua.open_libraries(sol::lib::base);

// Define a display object via a Lua table
lua.script("box = { name = 'myBox', x = 10, y = 20, width = 200, height = 150 }");

// Pass the table to the Factory
auto box = factory->create("Box", lua["box"]);
```

## Safety and Performance
- Typed handles (DomHandle/ResHandle) decouple references from object lifetimes.
- Factory enforces unique names and validates creation; lookups are type‑safe.
- Lua boundaries are thin: parse once, then operate on native objects; keep hot paths in C++.

## Source of Truth
Design docs are living and may evolve. When in doubt, defer to headers/Doxygen and unit tests as the canonical behavior, and open an issue or PR to align docs.

## SDOM API considerations for Lua
- Binding conventions:
	- Expose minimal, stable surfaces; prefer methods over public fields.
	- Use clear namespaces/tables (e.g., `sdom.core`, `sdom.factory`, `sdom.events`).
	- Map enums to string constants or integer values; document both.
- Handles and identity:
	- Pass DomHandle/ResHandle (or names) into Lua; avoid exposing raw pointers.
	- Provide `resolve(handle)`/`exists(name)` helpers to validate before use.
- Factory usage:
	- Accept `sol::table` for creation; validate `name` uniqueness and required fields.
	- Offer typed convenience creators (e.g., `factory:create_box(tbl)`) that normalize fields.
- Lifecycle hooks:
	- Allow registering Lua callbacks for init, update(dt), on_event(evt), render, unit_tests.
	- Ensure callbacks are optional and isolated; guard with try/catch and logging.
- Threading:
	- Run Lua on the main thread unless the VM is explicitly made thread‑safe.
	- If background work is needed, post results/events back to the main thread.
- Error handling:
	- Convert Lua errors to structured engine logs; show file/line and stack trace.
	- Fail fast on invalid creations; keep partial state consistent.
- Performance:
	- Keep hot paths in C++; avoid per‑frame table allocations inside tight loops.
	- Prefer prebound closures or lightuserdata for frequently called callbacks.
- Security/sandboxing:
	- Restrict libraries when embedding (only `base` by default); whitelist as needed.
	- Avoid exposing file/network primitives unless required; gate behind config flags.

## Next steps (Lua integration)
- Bind the Factory and core types to Lua (sol2) with a small, documented surface.
- Add a Lua demo scene that constructs a Stage and a few DisplayObjects.
- Wire lifecycle hooks (`onInit`, `onUpdate`, `onEvent`, `onRender`, `onUnitTests`) to optional Lua functions.
- Provide typed creator helpers and validation for common objects (e.g., Box, Label).
- Add unit tests that load Lua scripts to validate creation, events, and anchoring.
