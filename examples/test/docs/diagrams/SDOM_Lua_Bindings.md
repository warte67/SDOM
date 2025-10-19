(# SDOM Lua Bindings: per-type binding tables and DisplayHandle routing)

This document describes a recommended pattern to avoid collisions when exposing C++ display object APIs to Lua.

Problem
-------

Historically the project augmented a single shared `DisplayHandle` usertype table with all properties and functions for every concrete display object. That leads to name collisions when many objects (Group, Button, Label, ...) attempt to register identically-named keys (for example `text`, `label_text`, `getLabelText`, ...). Those collisions cause surprising runtime behavior and race conditions depending on registration order.

Recommendation
--------------

Keep the `DisplayHandle` usertype minimal (only generic helpers like `isValid`, `getName`, `getType`, `setName`, `destroy`). Register type-specific methods and properties into per-type binding tables stored in the Lua registry under a stable key such as `SDOM::Bindings`.

Install a single `__index` metamethod on the `DisplayHandle` that, when a key isn't found on the handle itself, consults the per-type table for the concrete object's type and returns the associated member. If the member is a function it should be wrapped in a small closure that forwards the original handle as the first argument so method-style calls continue to work.

Benefits
 - No cross-type collisions; each type manages its own binding surface.
 - The shared `DisplayHandle` remains small and stable.
 - New types can expose arbitrary names without worrying about other types.
 - Per-type tables are created once and are easy to inspect / debug.

Sketch implementation
-------------------

1) Per-type tables (example: `Group`)

```cpp
// in Group::_registerLuaBindings
sol::state_view lua = SDOM::getLua();
sol::table bindings = lua["SDOM::Bindings"]; // create if missing
if (!bindings.valid()) bindings = lua.create_table();

sol::table group_bindings = lua.create_table();
group_bindings.set("getGroupLabel", [](DisplayHandle h)->DisplayHandle { return static_cast<Group*>(h.get())->getLabel(); });
group_bindings.set("getGroupSpriteSheet", [](DisplayHandle h)->AssetHandle { return static_cast<Group*>(h.get())->getSpriteSheet(); });
bindings.set(Group::TypeName, group_bindings);
```

2) DisplayHandle __index router (single install)

```cpp
// in DisplayHandle::ensure_handle_table or bind_minimal
sol::table t = lua[LuaHandleName]; // create if missing
sol::table mt = lua.create_table();
mt["__index"] = lua.safe_function([](sol::this_state ts, sol::table handle, sol::object key)->sol::object {
	sol::state_view lua(ts);
	// check raw value first
	sol::object raw = handle.raw_get_or(key, sol::lua_nil);
	if (raw.valid() && raw != sol::lua_nil) return raw;

	// get the concrete type name from the handle
	std::string typeName;
	try { typeName = handle.raw_get_or("type", sol::lua_nil).as<std::string>(); } catch(...) { /* fallback if necessary */ }

	sol::table bindings = lua["SDOM::Bindings"]; if (!bindings.valid()) return sol::lua_nil;
	sol::object perType = bindings.raw_get_or(typeName, sol::lua_nil); if (!perType.valid() || perType == sol::lua_nil) return sol::lua_nil;
	sol::table tb = perType.as<sol::table>();
	sol::object member = tb.raw_get_or(key, sol::lua_nil); if (!member.valid() || member == sol::lua_nil) return sol::lua_nil;

	if (member.get_type() == sol::type::function) {
		// wrap the function to forward the handle as first arg when called
		sol::function fn = member.as<sol::function>();
		return lua["function"]([fn, handle](sol::variadic_args va, sol::this_state s)->sol::object {
			sol::state_view lua(s);
			// build args: first the handle, then contents of va
			std::vector<sol::object> args;
			args.push_back(sol::make_object(lua, handle));
			for (auto v : va) args.push_back(sol::make_object(lua, v));
			// call the underlying function
			sol::protected_function pf = fn;
			sol::protected_function_result r = pf.call(args);
			if (!r.valid()) return sol::lua_nil;
			return r.get<sol::object>();
		});
	}
	return member;
});
t[sol::metatable_key] = mt;
```

Notes & edge cases
------------------
- Performance: the __index fallback triggers only when a key is not found directly on the handle. Per-type lookup is cheap (table lookup). Wrapping functions has a small runtime cost but is localized.
- Method vs property: the wrapper above forwards the handle so both colon-style and dot-style calls behave sensibly (method style is recommended: `h:getGroupLabel()` or `h:getGroupSpriteSheet()`).
- Backwards compatibility: you can keep a compatibility shim by adding common helpers to the handle table early, but prefer per-type names like `getGroupLabel` to avoid collisions.
- Debugging: the `SDOM::Bindings` registry table can be inspected from Lua for troubleshooting.

Next steps
----------
- I can apply this pattern to the codebase: add the `SDOM::Bindings` registry creation, update `DisplayHandle::ensure_handle_table` to install the `__index` router, and move per-type bindings (Group, Button, Label, ...) into per-type tables.
- Or, if you prefer a smaller change, remove per-type property bindings from `DisplayHandle` and expose only explicit `getGroupLabel`/`getGroupSpriteSheet` names as read-only functions.

Pick which approach you want and I will prepare the patches and run the tests.

---
# Templated Lua Table by Object type

Templating the binding table per concrete object type will work and is exactly the kind of design that avoids the collisions you're seeing.

Below I summarize a concrete, low-risk design you can implement, the tradeoffs, and an incremental migration plan (so you can stop at a compatibility shim if you want).

Proposal (high level)
---------------------
Instead of adding every type-specific key onto one shared DisplayHandle table, create a per-type binding table keyed by the type name (for example SDOM::Bindings["Group"]).
When a concrete type registers its bindings it writes into that per-type table (not the shared handle).
The DisplayHandle usertype stays minimal. It gets a small dispatcher (an __index metamethod and optionally __newindex) which, when a key is not found on the handle itself, looks up the per-type table for the concrete object and returns the member.
If the per-type member is a function, the dispatcher returns a closure that forwards the DisplayHandle as the first argument (so h:myMethod(...) works). For properties you can expose explicit getX/setX functions or encode property descriptors in the per-type table and let the dispatcher implement property semantics.

Why this is good
-----------------
No global key collisions between Group, Button, Label, etc.
Per-type bindings are local, easy to inspect and test from Lua (print(SDOM::Bindings.Group)).
Concrete types can expose arbitrary names (including multiple labels, editors) without worrying about others.
Minimal change to the Lua call surface: scripts still call methods on handles, e.g. h:getGroupLabel() or h:getLabel().

Design details / sketch
-----------------------

Per-type table creation helper
Provide helper like sol::table ensure_type_bind_table(sol::state_view lua, const std::string& typeName) that returns a table stored under a registry key, e.g. lua["SDOM::Bindings"][typeName] = <table>.

Concrete type registration
In Group::_registerLuaBindings, push functions into that per-type table instead of into the shared DisplayHandle:

Dispatching from DisplayHandle
Keep DisplayHandle minimal. When creating the handle usertype, install a metatable with __index (and optionally __newindex) that:
first checks raw_get on the handle (fast path).
if not present, looks up SDOM::Bindings[typeName] and tries to raw_get the key there.
if the member is a function, return a closure that calls the function with the handle as the first argument.
if the member is a plain value, return it directly.
for properties you can either:
expose only getter/setter functions in the per-type table (preferred, simpler), or
encode { kind="property", get=fn, set=fn } and have the dispatcher expose property semantics on reads/writes via __index/__newindex.

Sketch of the dispatcher (conceptual):

```cpp
// conceptual sketch (see notes below for sol2 specifics)
mt["__index"] = lua.safe_function([](sol::this_state S, sol::table handle, sol::object key)->sol::object {
	sol::state_view lua(S);
	// fast path: check handle first
	sol::object raw = handle.raw_get_or(key, sol::lua_nil);
	if (raw.valid() && raw != sol::lua_nil) return raw;

	// get type name
	std::string typeName;
	try { typeName = handle.raw_get_or("type", sol::lua_nil).as<std::string>(); } catch(...) { return sol::lua_nil; }

	sol::table registry = lua["SDOM::Bindings"]; if (!registry.valid()) return sol::lua_nil;
	sol::object per = registry.raw_get_or(typeName, sol::lua_nil); if (!per.valid() || per == sol::lua_nil) return sol::lua_nil;
	sol::table tb = per.as<sol::table>();
	sol::object member = tb.raw_get_or(key, sol::lua_nil); if (!member.valid() || member == sol::lua_nil) return sol::lua_nil;

	if (member.get_type() == sol::type::function) {
		sol::function fn = member.as<sol::function>();
		// return a closure that binds the handle as first arg
		return lua["function"]([fn, handle](sol::variadic_args va, sol::this_state s)->sol::object {
			sol::state_view lua(s);
			std::vector<sol::object> args;
			args.push_back(sol::make_object(lua, handle));
			for (auto v : va) args.push_back(sol::make_object(lua, v));
			sol::protected_function pf = fn;
			sol::protected_function_result r = pf.call(args);
			if (!r.valid()) return sol::lua_nil;
			return r.get<sol::object>();
		});
	}
	return member;
});
```

Notes:

- The closure building above is conceptual; with sol2 you can implement it using sol::protected_function and small forwarding wrappers.
- For __newindex you can support property-style h.some_prop = value by detecting an encoded property descriptor in the per-type table, or by falling back to setX functions (my recommendation is to expose setX/getX functions to keep things unambiguous).

Inheritance (ancestors bindings)
--------------------------------
Two sensible ways to make ancestor bindings visible:

copy ancestor entries into the child's per-type table at registration time (simple, one-time cost).
set the per-type table's metatable __index to the parent's per-type table (cleaner runtime behavior).
Either works; metatable chaining mirrors C++ inheritance and avoids copying.

Example:

```cpp
// when registering Group bindings: parent = ensure_type_bind_table(lua, "IPanelObject");
group_bindings[sol::metatable_key] = lua.create_table_with("__index", parent);
```

Backward compatibility
----------------------
You can keep a small set of commonly-used convenience helpers (getLabel/getSpriteSheet) on the shared DisplayHandle if you want immediate backwards compatibility.
Prefer to update unit tests and scripts to use h:getLabel() and lbl:getText() rather than adding many keys to the shared handle.

Pitfalls & tradeoffs
--------------------
Complexity: the dispatcher introduces an extra table lookup when keys are not found on the handle itself. This is small and acceptable for most UIs (bindings are not extremely hot paths).
Properties: representing sol::property semantics across separate tables requires an encoding (descriptor objects) or simply exposing getter/setter functions which is simpler and robust.
Closure cost: wrapping functions in closures has a small per-lookup cost; you can cache wrapped closures in the handle or the per-type table to avoid repeated wrapper allocations (optional optimization).

Incremental migration plan (safe)
--------------------------------
Implement ensure_type_bind_table and SDOM::Bindings registry.
Add the DisplayHandle dispatcher (__index, maybe __newindex).
Move Group bindings into SDOM::Bindings["Group"] and keep DisplayHandle minimal.
Update Group_UnitTests to use group:getLabel() or group:getGroupLabel() as desired.
Migrate other types (Button, Label) one-by-one, running tests after each move.
Optionally remove deprecated keys from DisplayHandle once everything is migrated.

Offer
-----
I can:

- implement the per-type binding registry + dispatcher in DisplayHandle::ensure_handle_table and add ensure_type_bind_table, then
- move Group's registration into its per-type table and update the test to call methods via the dispatcher, then build & run the tests.
Shall I apply the implementation (do the code edits and run the test suite), or would you prefer to first try the simpler approach of renaming conflicting helpers to explicit names (getGroupLabel, getGroupSpriteSheet) and postpone the dispatcher?

GPT-5 mini • 0x


## Templated per-type handle tables (alternative phrasing)

Another close variation is to template `ensure_handle_table` on the concrete object type when registering bindings, so each type writes into a unique handle table named by the type (for example `DisplayHandle_Group` or `Group_binds`). This keeps ancestor/derived semantics local and avoids adding any keys to a single global handle table.

Idea
----
- Provide a helper `ensure_type_bind_table(lua, typeName)` which returns a table stored under a registry key (for example `SDOM::Bindings[typeName]` or globally named `DisplayHandle_<typeName>`).
- When `_registerLuaBindings` is called for a concrete type, use `ensure_type_bind_table` and populate that table with functions and simple values. Optionally set the table's metatable `__index` to the parent's bind-table so inheritance works automatically.
- Keep the shared `DisplayHandle` only responsible for minimal helpers. Install a single `__index` in the `DisplayHandle` metatable that looks up missing keys in the per-type table for the concrete object's type. Wrap per-type functions as closures that inject the handle as the first argument.

Why this is slightly different
--------------------------------
- It's effectively the same as the per-type registry + dispatcher pattern, but the phrasing emphasises creating distinct handle tables (one per type) instead of storing tables inside a single `SDOM::Bindings` registry table. Both approaches are equivalent; choose the representation that reads best in your codebase.

Quick migration checklist
------------------------
1. Add `ensure_type_bind_table(lua, typeName)` (or `ensure_type_handle_table`) helper and create/attach per-type tables.
2. Update `DisplayHandle::ensure_handle_table` to install an `__index` that forwards to the per-type table when a key is not present on the handle itself.
3. Move `Group` bindings into the per-type table; make the table inherit from the parent's table via metatable `__index` or by copying parent entries.
4. Run tests and iterate; migrate other object types one-by-one.

Example naming choices
----------------------
- `SDOM::Bindings[typeName]` — single registry table mapping type -> table (recommended).
- `DisplayHandle_<TypeName>` — individual global tables, one per type (equally valid).

Recommendation
--------------
Start with the registry-style `SDOM::Bindings` and implement the dispatcher `__index`. It's minimally invasive and gives you a single place to inspect all per-type bindings. If you prefer explicit tables, the same pattern applies and requires only a change to how you look up the per-type table in the dispatcher.

If you'd like I can implement the registry + dispatcher, move `Group` bindings, and run the unit tests. Which should I do next?




