# Variant Migration Playbook

Goal: move C API consumers from POD handles/events to `SDOM_Variant` while keeping the build green and downstreams stable.

> Reminder: Core is a singleton; Core C API omits a `subject` parameter because the subject is always the global Core instance.

## Principles
- **Dual path first, removal last:** keep POD handle/event structs during transition; add variant-taking equivalents alongside them.
- **Generator-first:** change data registration and the CAPI generator; never hand-edit generated outputs.
- **Single source of truth:** centralize struct↔variant conversion in helpers/emitter logic; avoid per-callsite ad hoc conversions.
- **Clear tagging:** use stable `VariantType` tags for display handles, asset handles, and events; guard with `IsDisplay/IsAsset/IsValid` (and event tag checks) that set errors on mismatch.
- **Include hygiene:** keep `Handles` header lightweight (no Variant include); `Variant` may include `Handles`; generator filters dependency includes to avoid cycles.
- **Test each step:** add round-trip tests (struct → variant → struct) and API-level tests for variant-taking functions to ensure parity with POD paths.
- **Deprecate with evidence:** only remove POD forms after usage is zero and variant tests are green; document deprecations in changelog/README.

## API Surface Strategy
- Add variant siblings (e.g., `_V` suffix or overload) for C API functions that currently take/return handle/event structs.
- Keep return/parameter names consistent; bool return + `SDOM_GetError` remains the contract.
- Document POD forms as legacy/preferred-to-be-migrated; mark variants as the preferred path.

## Conversion Helpers
- Provide `SDOM_MakeDisplayHandle`, `SDOM_MakeAssetHandle`, and event-to-variant helpers; keep them in one module.
- Provide struct extractors from variants (checked by tag) and ensure they fail with a clear error when tags don’t match.

## Phased Rollout (suggested order)
1) **Handles foundation:** keep POD structs, ensure generator emits both structs and variant helpers; add round-trip tests.
2) **Core lookups & accessors:** add variant-taking forms for get/create/destroy/display/asset APIs; keep POD forms.
3) **Events:** add event-variant helpers and variant-taking event C APIs; guard tag checks.
4) **Bindings:** update higher-level bindings (Lua, future languages) to use variants; keep POD wrappers temporarily if needed.
5) **Deprecation sweep:** mark POD forms deprecated in docs; remove once no usages remain and tests pass variant-only.

## Concrete Plan (working list)
- Short term: keep POD handles/events; regenerate to ensure structs + variant helpers emit; verify round-trip tests for display/asset handles.
- Next: add variant siblings to Core C API for create/get/destroy display/asset, stage/root/mouse/keyboard focus functions. Keep POD; note in docs.
- Then: event path—add variant helpers and variant-based getters/setters for targets/related targets; ensure tag guards and error messages.
- Later: adjust Lua bindings (and future bindings) to favor variants while retaining POD wrappers during migration.
- Finally: deprecate/remove POD forms once downstream usage is zero and variant tests are green; announce in changelog/README.

## Error Handling & Guards
- Variant APIs must reject wrong tags and nulls with clear `SDOM_SetError` messages.
- Core remains a singleton; the “subject” is implicit—document that variant subject is omitted for Core APIs.

## Documentation & Communication
- State in README and changelog that binding changes are generator-driven and variants are the preferred cross-language type.
- Note the temporary coexistence of POD handles/events with variants and the removal criteria.

## Testing Checklist
- Struct↔variant round-trips per type.
- Variant API parity tests vs POD API results.
- Include-cycle guard (Handles vs Variant) stays clean after generator runs.



## Here’s a practical implementation plan to move toward variant-first without breaking things:

1. ✅ Generator updates (if not already):
- ✅ Keep Handles header lightweight (no Variant include) and avoid Variant self-include; include hygiene is clean after regen.
- ✅ Ensure generator emits handle/event POD structs plus their variant helpers (Make*, Is*, tag checks) — event helpers now generated.
- ✅ Add an option to emit variant-sibling C API signatures (e.g., _V or overloads) for Core/Event when enabled (toggle via `SDOM_CAPI_EMIT_VARIANT_SIBLINGS`, default on).
2. ✅ Core C API variant siblings:
- ✅ Add variant-taking forms for create/get/destroy display/asset, stage/root/mouse/keyboard focus. Keep POD versions.
- ✅ Marshal in one place: incoming variant → checked tag → POD struct → C++ handle; outgoing POD → variant via SDOM_Make*.
- ✅ Clear errors on tag mismatch/null.
3. ✅ Event C API variant support:
- ✅ Add helpers for Event ↔ Variant.
- ✅ Add variant forms for get/set target/current/related targets; guard tags and nulls.
4. ✅ Tests:
- ✅ Round-trip per type: POD → variant → POD and compare id/name/type.
- ✅ Parity tests: variant API result matches POD API result for core lookups/destroys/events.
- ✅ Include-cycle check: generated headers remain acyclic (Handles vs Variant).
5. ☐ Bindings (deferred):
- (Lua bindings not started; skip for now)
- ☐ Update future bindings to prefer variant forms; keep POD wrappers temporarily.
- ☐ Deprecation:
- ☐ Doc-only deprecate POD forms once variant paths are proven; remove only when downstreams are off them.


5. Bindings & Deprecation (future phase)

Status: Deferred. Variant-first C API is now stable; bindings can be updated later in a controlled pass.

5.1 Binding Strategy (Lua and future languages)

- Rule: all high-level bindings should prefer `_V` (Variant) forms internally.
- POD handle/event structs are considered low-level C-only artifacts.
- Lua-visible APIs should:
  - Consume and return SDOM_Variant handles.
  - Only use POD handle structs inside thin C shims, if needed.

Planned steps:
- Add a small Lua-facing helper module:
  - `SDOM.MakeDisplayHandle(...)`
  - `SDOM.MakeAssetHandle(...)`
  - `SDOM.IsDisplayHandle(...)`, `SDOM.IsAssetHandle(...)`
- Update existing Lua bindings:
  - Replace calls to POD-based functions with `_V` siblings where available.
  - Keep old entry points as thin adapters (POD → Variant) for a while.
- Add Lua tests that:
  - Create/display objects via Lua using variants.
  - Use focus/hover and verify behavior.
  - Exercise event targets via Variant paths where exposed.

5.2 Deprecation Plan (POD forms)

We will NOT remove POD forms yet, but we will:

- Mark functions that take `SDOM_DisplayHandle` / `SDOM_AssetHandle` / `SDOM_Event` PODs as **legacy** in the C API docs.
- Document that `_V` forms are the preferred interface going forward.
- Add a deprecation checklist:

  - [ ] All internal bindings (Lua, etc.) updated to `_V`.
  - [ ] No in-tree code calls the POD versions except compatibility wrappers.
  - [ ] A full CI cycle verifies Variant-only bindings.
  - [ ] A release notes entry announces a future removal window.

Only after those conditions are met will POD-only forms be candidates for removal in a future major revision.
