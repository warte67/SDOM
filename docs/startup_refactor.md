# Startup Refactor Proposal
The current startup logic in SDOM relies on legacy patterns from its early JSON-based design, where the engine automatically parses a global table named `Core` or `config` to initialize itself. While this approach was convenient for simple bootstrapping, it has become a limitation as the engine and its scripting capabilities have grown. Implicit startup makes the initialization sequence harder to control, debug, and extend, especially for advanced use cases or custom workflows. Mixing configuration parsing with resource creation and engine startup also leads to fragile code and restricts flexibility for users who want more explicit control. By refactoring to an explicit initialization pattern—where users directly call `Core:create()` and `Core:run()`—the engine will support clearer, more maintainable, and modern integration with both Lua and C++. This change will make SDOM easier to document, extend, and use in a wide range of application designs.

Reserving the name `Core` as the central global Lua table remains a sensible design choice, even with the move to explicit initialization. Having a well-known, consistent entry point for engine configuration and control helps users quickly understand how to interact with SDOM from Lua, and it provides a natural place to attach engine-wide methods, properties, and callbacks. By keeping `Core` as the global table, you maintain backward compatibility with existing scripts and examples, while also making it easier to document and standardize the API. The key improvement is to require explicit calls to `Core:create(config)` and `Core:run()`, rather than relying on automatic parsing of the table's contents. This approach combines the clarity and flexibility of explicit startup with the convenience and discoverability of a single, reserved global entry point.

In the current SDOM design, both `Core` and `CoreForward` are used in the Lua state. `Core` serves as the main global entry point for engine configuration and control, while `CoreForward` acts as a forwarding table to preserve engine API bindings if the global `Core` table is overwritten or replaced by user scripts. This pattern ensures that engine methods and properties remain accessible even if scripts redefine or shadow the `Core` table. As we refactor startup logic, we can keep `CoreForward` as an internal mechanism to maintain API stability, but encourage users to interact with the reserved global `Core` table for all engine operations. Documenting this distinction will help users understand the purpose of each and avoid confusion when customizing or extending the engine from Lua.

As part of the next phase of the startup refactor, we plan to transition from exposing the Core and CoreForward tables directly in Lua to using an internal-only identifier such as __sdom_core_object__. This hidden engine handle will manage all internal state and lifecycle, reducing the risk of naming collisions and accidental modification by user scripts. Instead of requiring users to interact with the `Core` table, we will bind all essential engine properties, functions, and methods as global Lua functions, making the API more concise and script-friendly. This approach streamlines the user experience, allowing scripts to call engine features directly without referencing a specific object. Legacy support for Core and CoreForward will remain during the transition, but new documentation and examples will focus on the global function pattern for clarity and maintainability.


**Current limitations:**
- The engine expects a global table named `Core` (or `config`) and auto-parses it, which is a legacy pattern from JSON-centric bootstrapping.
- This makes startup implicit, less flexible, and harder to extend or debug.
- It also mixes config parsing with resource creation and engine startup, which can be fragile.

**Recommended redesign:**

### 1. Explicit Initialization Pattern

**Lua-side:**
```lua
local my_config = {
    window_width = 1200,
    window_height = 800,
    -- ... other settings ...
    children = { ... }
}
Core:create(my_config)
Core:run()
```

**C++-side:**
```cpp
CoreConfig config = { /* ... */ };
core.create(config);
core.run();
```

### 2. Remove Automatic Table Parsing

- Deprecate auto-parsing of global `Core` or `config` tables.
- Require explicit calls to `Core:create(table)` or `Core:create(config_struct)`.

### 3. Separate Config Parsing from Resource Creation

- Make `Core:create()` responsible for parsing the config and creating resources.
- Allow users to call `Core:run()` only after setup is complete.

### 4. Update Documentation and Examples

- Show explicit initialization in both Lua and C++.
- Document the expected config structure and startup sequence.

---

**Migration Checklist:**
1. Refactor `configureFromLua` and `configureFromLuaFile` to only parse and validate config, not auto-create resources.
2. Move resource creation logic into `Core:create(table)` and `Core:create(config_struct)`.
3. Update Lua bindings to expose `create` and `run` methods.
4. Remove legacy auto-table parsing from startup.
5. Update all examples and docs to use explicit initialization.

---

## Step-by-Step Refactoring Strategy

### 1. Deprecate Automatic Table Parsing
- Remove or mark as deprecated the code that automatically parses global tables named `Core` or `config` for engine startup.
- Update warnings and documentation to inform users of the new explicit initialization pattern.

### 2. Introduce Explicit Initialization Methods
- Implement and expose `Core:create()` and `Core:run()` methods in both Lua and C++.
- Ensure these methods handle configuration parsing, resource creation, and engine startup in a clear, user-driven sequence.

### 3. Bind Engine Functions as Lua Globals
- Bind essential engine functions (e.g., `create`, `run`, `shutdown`) as global Lua functions for direct access.
- Internally, reference the engine instance using an obscure identifier (e.g., `__sdom_core_object__`) to avoid naming collisions.

### 4. Maintain Legacy Support During Transition
- Keep `Core` and `CoreForward` available for backward compatibility, but mark them as deprecated.
- Document the transition and encourage users to migrate to the new global function pattern.

### 5. Refactor Resource Creation Logic
- Move resource creation out of config parsing functions and into the explicit `create` method.
- Ensure that configuration parsing and resource instantiation are clearly separated.

### 6. Update Documentation and Examples
- Revise all documentation, tutorials, and example scripts to use the new explicit initialization and global function pattern.
- Clearly explain the new startup sequence and configuration structure.

### 7. Test and Validate
- Thoroughly test the new startup flow in both Lua and C++ environments.
- Validate that legacy scripts still work and that new scripts benefit from improved clarity and flexibility.

---

**Benefits:**
- **Clarity:** Users control when and how the engine starts.
- **Flexibility:** Supports advanced scripting, multi-stage setups, and custom bootstrapping.
- **Maintainability:** Easier to debug, extend, and document.
- **Modern API:** Matches best practices for Lua/C++ engine integration.

---

