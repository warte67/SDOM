## Modern COM-like Interface and Hook Design

### Overview
This document outlines a modern approach to the classic Microsoft COM (Component Object Model) interface, adapted for C++ with smart pointer resource management and language interoperability. The goal is to provide a robust, extensible, and safe system for exposing hooks to external languages (such as Lua, Python, etc.) and for managing paired lifecycle functions automatically.

### Key Concepts

- **Smart Pointer Resource Management:**
	- All internal resources are managed via `std::shared_ptr` (or similar smart pointers).
	- Manual `->Release()` calls are eliminated; reference counting ensures automatic cleanup.
	- Double deletion is impossible: resources are only destroyed when all references are released.

- **Paired Lifecycle Functions:**
	- Functions are paired as `first`/`second` (e.g., `init`/`quit`, `create`/`destroy`, `startup`/`shutdown`).
	- The "second" function is always called automatically on shutdown if not already called.
	- If "second" is called, it cannot be called again until "first" is called anew.
	- Reference counting via smart pointers enforces correct lifecycle and prevents resource leaks.

- **Extern "C" Hooks for Interoperability:**
	- All hookable functions are exposed as `extern "C"` for compatibility with other languages and dynamic linking.
	- This enables integration with Lua, Python, and other FFI (foreign function interface) systems.
	- Example:
		```cpp
		extern "C" void SDOM_Init();
		extern "C" void SDOM_Quit();
		extern "C" SDOM_Handle SDOM_CreateBox(const char* name, int x, int y, int w, int h);
		extern "C" void SDOM_DestroyBox(SDOM_Handle handle);
		```

- **Lua Integration:**
	- Hooks can be registered and called from Lua scripts using Sol2 or similar binding libraries.
	- Lua can invoke C++ hooks, and C++ can call Lua functions for event handling, testing, or scripting.
	- Example (Lua):
		```lua
		local box = SDOM_CreateBox("testBox", 100, 100, 120, 80)
		SDOM_DestroyBox(box)
		```

### Benefits

- Automatic resource management and cleanup
- Safe, predictable lifecycle for all paired functions
- Easy integration with external languages and tools
- Extensible for future language hooks (Python, etc.)
---
## Dynamic Extensibility Strategies

To make the COM-like hook system dynamically extensible:

### Example: Dynamic Hook Registration in C++
```cpp
// Define a hook function type
using HookFunc = std::function<void(void*)>;
std::unordered_map<std::string, HookFunc> hookRegistry;

// Register a hook at runtime
void registerHook(const std::string& name, HookFunc func) {
	hookRegistry[name] = func;
}

// Invoke a hook by name
void callHook(const std::string& name, void* data) {
	auto it = hookRegistry.find(name);
	if (it != hookRegistry.end()) {
		it->second(data);
	}
}
```

### Example: Extern "C" Entry Point
```cpp
extern "C" void SDOM_RegisterHook(const char* name, HookFunc func) {
	registerHook(std::string(name), func);
}
```

### Example: Lua Integration with Sol2
```cpp
// Bind the registration function to Lua
lua.set_function("registerHook", &registerHook);

// In Lua:
registerHook("onBoxCreated", function(box) print("Box created:", box) end)
```

### Example: Plugin/Module System
```cpp
// Plugin interface
extern "C" void RegisterPluginHooks() {
	SDOM_RegisterHook("onCustomEvent", [](void* data) {
		// Custom event handling code
	});
}

// Load plugin (DLL/shared library) and call RegisterPluginHooks
```

### Example: Reflection and Metadata
```cpp
struct HookMetadata {
	std::string name;
	std::vector<std::string> paramTypes;
	std::string returnType;
};
std::unordered_map<std::string, HookMetadata> hookMetaRegistry;

// Register metadata alongside hooks for introspection
```

### Example: Smart Pointer Resource Management
```cpp
std::shared_ptr<Resource> resource = std::make_shared<Resource>();
// Hooks and plugins use shared_ptr, so resources are cleaned up automatically
```

### 1. Dynamic Hook Registration
- Allow users or external modules to register new hooks at runtime.
- Use a registry (e.g., a map of string/function pointers or std::function) to store hooks.
	```cpp
	using HookFunc = std::function<void(void*)>;
	std::unordered_map<std::string, HookFunc> hookRegistry;

	void registerHook(const std::string& name, HookFunc func) {
		hookRegistry[name] = func;
	}
	```

### 2. Extern "C" Entry Points for Registration
- Expose registration functions as `extern "C"` so they can be called from other languages.
	```cpp
	extern "C" void SDOM_RegisterHook(const char* name, HookFunc func);
	```

### 3. Lua/Script Integration
- Allow Lua (or other scripting languages) to define and register hooks.
- Use Sol2 to bind registration functions so Lua can call them.
	```lua
	SDOM_RegisterHook("onBoxCreated", function(box) print("Box created:", box) end)
	```

### 4. Reflection and Metadata
- Store metadata about hooks (parameters, return types, etc.) for introspection and dynamic invocation.
- Enables scripting languages to discover available hooks and their signatures.

### 5. Plugin/Module System
- Support loading external modules (DLLs/shared libraries) that can register hooks at runtime.
- Use a standardized interface for plugins to expose and register their hooks.

### 6. Smart Pointer and Resource Management
- Ensure all dynamically registered hooks and resources are managed via smart pointers to prevent leaks and double deletion.

---

### Next Steps

- Prototype a few hooks for Lua-driven unit tests
- Design and document the paired function pattern with smart pointer reference counting
- Provide examples for exposing hooks via `extern "C"` and integrating with Lua

---
