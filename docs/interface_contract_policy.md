# 🧾 SDOM Interface Contract Policy

_This document defines the standards, responsibilities, and enforcement model governing SDOM’s public interface, documentation, and behavioral stability._

---

## 📘 Purpose

The **SDOM Interface Contract Policy** establishes a clear and traceable agreement between:
- **Developers** maintaining and extending the SDOM API, and  
- **Integrators / Users** relying on its published interface through C++, Lua, or future bindings.

This policy ensures that once an interface is published and documented, it remains consistent, predictable, and reliable across future releases — or is changed transparently through explicit versioning and deprecation procedures.

---

## 🧩 Contract Layers

SDOM’s interface contract is enforced across four layers, each serving a specific role:

| Layer | Description | Tooling / Enforcement |
|-------|--------------|----------------------|
| **1. Source Code (C++)** | Defines actual interface signatures, inheritance, and visibility. | Compiler / Doxygen |
| **2. Doxygen Documentation** | Self-correcting design document that mirrors implementation. | Doxygen, CI warnings |
| **3. Wiki Documentation** | Public-facing, human-readable contract derived from Doxygen. | Markdown export, Wiki sync |
| **4. Unit Tests (C++ & Lua)** | Behavioral contract validation for event dispatch, properties, and bindings. | Test Harness, Lua scripts |

---

## ⚙️ Contract Rules

### 🧱 1. Documentation Consistency
- Every public class, method, and property **must** have a Doxygen comment block.
- The Doxygen header should include:
  - `@brief` summary
  - `@section` tables for **Properties**, **Functions**, and **Events**
  - At least one `@luaExample` or code block demonstrating usage.
- Lua bindings must be **documented in the same header** that defines their C++ implementation.

### 🧠 2. Behavioral Parity
- Lua and C++ APIs should behave identically unless explicitly documented otherwise.
- All Lua-exposed methods and properties should correspond directly to their C++ counterparts.
- Any divergence must be clearly marked with a `@note` and explained in the Doxygen block.

### 🧩 3. Breaking Changes
Once published (i.e., appearing in the Doxygen documentation or Wiki):
- Removing or renaming a function, property, or event **constitutes a breaking change.**
- Changing a function’s signature, return type, or semantics **requires a minor or major version increment.**
- Breaking changes must be recorded in:
  - `CHANGELOG.md`
  - The affected class’s Doxygen comment under a **“Breaking Changes”** subsection
  - The Wiki, under the **version history** note for that object

### 🧾 4. Deprecation Procedure

To deprecate an interface safely and consistently:
1. **Annotate the symbol in C++** using the standard attribute:
   ```cpp
   [[deprecated("Use newFunction() instead.")]]
   void oldFunction();
   ```
   - The `[[deprecated]]` attribute triggers compiler warnings across all major toolchains (GCC, Clang, MSVC).
   - Always include a clear message describing the preferred replacement.
2. **Add a matching Doxygen `@deprecated` tag** in the function or class comment block:
   ```cpp
   /**
    * @brief Old render API.
    * @deprecated Use drawTexture() instead.
    */
   [[deprecated("Use drawTexture() instead.")]]
   void renderTexture();
   ```
   - This ensures the deprecation note appears in the generated documentation.
   - The message in Doxygen and the attribute should match for clarity.
3. **Update documentation and metadata:**
   - Add an entry to `CHANGELOG.md` under a **“Deprecated”** section.
   - Include a short explanation in the Wiki or object documentation (under “Version Notes”).
   - Update any Lua bindings to log a gentle runtime warning if the deprecated function is invoked.
4. **Maintain backward compatibility for one minor release** before removing the symbol entirely.
5. **Removal policy:**
   - After one full minor version, the symbol may be removed in the next release if no dependent systems remain.
   - When removed, update the Wiki and documentation to mark it as “Removed in vX.Y.Z”.

---

## 🧪 Validation & Enforcement

| Category | Enforcement Method |
|-----------|--------------------|
| Missing documentation | Doxygen `WARN_IF_UNDOCUMENTED` + CI check |
| Lua binding mismatch | Lua UnitTest suite |
| Behavioral regression | Re-entrant UnitTest harness (C++ / Lua) |
| Memory / lifecycle violations | Valgrind & sanitizers |
| Contract violation (breaking change) | Manual review + version bump requirement |

---

## 🧰 Versioning Strategy

SDOM follows **semantic versioning** principles with minor adjustments for pre-alpha state.

| Type | Format | Description |
|------|---------|-------------|
| **Major** | `X.0.0` | Incompatible changes to API or object structure |
| **Minor** | `0.X.0` | Additions or extensions that remain backward compatible |
| **Patch** | `0.0.X` | Non-breaking fixes, internal refactors, or documentation updates |
| **Build** | `0.5.128` | Internal or experimental build identifier (auto-generated) |

> ⚠️ During early pre-alpha (0.x.y), SDOM reserves the right to adjust public interfaces as needed — but all changes must still be documented and versioned.

---

## 🧭 Release Process

1. **Run All Unit Tests**  
   Confirm 100% pass rate in both C++ and Lua test harnesses.

2. **Generate Documentation**  
   Run `doxygen docs/Doxyfile` and verify that no warnings or missing sections exist.

3. **Regenerate Wiki / Markdown Pages**  
   Export class documentation for user-facing pages.

4. **Update Version and Changelog**  
   Increment the version as appropriate and document any breaking, deprecated, or new features.

5. **Tag and Release**  
   Tag in Git (`vX.Y.Z`), push, and ensure GitHub Pages reflects the updated documentation.

---

## 🧩 Future Goals

- 🧱 Automate Doxygen → Markdown → Wiki pipeline (via `scripts/gen_docs.sh`)
- 🧩 Integrate Doxygen XML with `doxybook2` for GitHub Wiki export
- 📜 Include Lua API tables automatically in public docs
- ⚙️ Implement CI job to detect breaking API diffs between releases

---

## ✨ Summary

This policy transforms SDOM’s documentation into a **living design contract** —
an enforceable agreement between maintainers and users that ensures:

- Consistency between C++ and Lua layers  
- Predictable versioning and change management  
- Self-updating, traceable documentation  
- High developer confidence and long-term maintainability  

By adhering to this contract, SDOM’s API remains not only functional —  
but *reliable, discoverable, and future-proof*.

---

**Maintained by:**  
Jay Faries ([warte67](https://github.com/warte67))  
© 2025 SDOM Project – Licensed under the ZLIB License.
