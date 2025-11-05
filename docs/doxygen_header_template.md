/**
 * @class SDOM::<ClassName>
 * @brief [Short one-line summary of the object’s role in the DOM]
 * @inherits SDOM::<BaseType>
 * @luaType <LuaUsertypeName>
 *
 * [Expanded description paragraph]
 * Describe how this object participates in the DOM tree: what it visually represents,
 * its typical usage, and how it interacts with parents and children. Mention any 
 * inherited behaviors (e.g., click handling, event propagation, focus).
 *
 * ---
 * @section <ClassName>_Properties Properties
 * | Property | Type | Description |
 * |-----------|------|-------------|
 * | `example_property` | type | Brief description of what it controls. |
 * | `another_property` | type | Description. |
 * | ... | ... | ... |
 *
 * ---
 * @section <ClassName>_Functions Functions
 * | Function | Returns | Description |
 * |-----------|----------|-------------|
 * | `setExample(value)` | void | Sets an example property. |
 * | `getExample()` | type | Returns the current example value. |
 * | `updateSomething(deltaTime)` | void | Example function. |
 *
 * ---
 * @section <ClassName>_Events Events
 * | Event | Description |
 * |--------|-------------|
 * | `EventName` | Fired when condition is met. |
 * | `AnotherEvent` | Explanation. |
 *
 * ---
 * @section <ClassName>_Notes Notes
 * - Implementation notes or Lua/C++ differences.
 * - Mention if derived types extend or override base behavior.
 * - Include cross-references to related objects or patterns (e.g., Button, Slider).
 *
 * ---
 * @section ClassName_Notes Notes
 * - Any implementation notes, platform limitations, or special considerations.
 * - Mention if Lua bindings differ from C++ function signatures.
 * 
 * ---
 * @section License ZLIB License
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */


// ============================================================================
// SDOM <ModuleName>.cpp
// ----------------------------------------------------------------------------
// Implementation file for <ModuleName>
//
// Purpose:
//     <Briefly describe what this file implements — e.g. rendering behavior,
//     event handling, or core logic for the associated class/interface.>
//
// Notes:
//     - Keep helper functions in anonymous namespaces or mark them `static`.
//     - Avoid adding Doxygen comments here; documentation belongs in headers.
//     - Maintain include order: stdlib, SDL, SDOM core, then local headers.
//
// ----------------------------------------------------------------------------
// License: ZLIB
// ----------------------------------------------------------------------------
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
// ----------------------------------------------------------------------------
// Author: Jay Faries (https://github.com/warte67)
// ============================================================================


## 🧩 SDOM Emoji Convention
> A visual shorthand system for SDOM documentation and comments.

| Category | Emoji | Meaning / Use Case | Example |
|-----------|--------|--------------------|----------|
| **Lua Integration** | 📜 | Lua bindings, usertype registration, Lua-visible properties/functions | `// 📜 Lua Bindings`<br>`@section 📜 Lua Bindings` |
| **Lifecycle / Object Phases** | 🌱 | Object lifecycle functions (`onInit`, `onUpdate`, `onRender`, `onQuit`, etc.) | `// 🌱 Lifecycle: onInit()` |
| **Static Factory / Creation** | 🏭 | Static or factory functions that construct SDOM objects | `// 🏭 Factory Method: CreateFromInitStruct()` |
| **Testing / Validation** | 🧪 | Unit testing, validation, or diagnostic code | `// 🧪 Unit Test: validate arrow direction` |
| **Events / Input Handling** | 🎯 | Event-related functions (`onEvent`, event dispatch, cursor/keyboard handling) | `// 🎯 Event Handling` |
| **Rendering / Visual Output** | 🎨 | Drawing, layout, or appearance functions | `// 🎨 Render phase` |
| **Data / Property Management** | 🧩 | Property accessors, bindings, serialization | `// 🧩 Property Accessors` |
| **Internal Logic / Helpers** | ⚙️ | Internal algorithms, computations, private helpers | `// ⚙️ Internal: compute direction offset` |
| **Deprecated / Legacy** | ⚠️ | Deprecated functions or placeholders to be removed | `// ⚠️ [[deprecated]] use new API` |
| **Debug / Diagnostics** | 🔍 | Debug logging or diagnostic code | `// 🔍 Debug Output: EventType` |
| **Factory-Registered Type** | 🪣 | Class registered in the Factory registry | `// 🪣 Registered in Factory as: ArrowButton` |
| **Documentation Note / Policy Reference** | 📘 | Reference to interface contract, identifier policy, or docs | `// 📘 See interface_contract_policy.md` |
