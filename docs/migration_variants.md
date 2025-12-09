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
- ☐ Ensure it always emits handle/event POD structs plus the variant helpers (Make*, Is*, tag checks) and keeps Handles/Variant includes uncoupled.
- ✅ Add an option to emit variant-sibling C API signatures (e.g., _V or overloads) for Core/Event when enabled (toggle via `SDOM_CAPI_EMIT_VARIANT_SIBLINGS`, default on).
2. ☐ Core C API variant siblings:
- ☐ Add variant-taking forms for create/get/destroy display/asset, stage/root/mouse/keyboard focus. Keep POD versions.
- ☐ Marshal in one place: incoming variant → checked tag → POD struct → C++ handle; outgoing POD → variant via SDOM_Make*.
- ☐ Clear errors on tag mismatch/null.
3. ☐ Event C API variant support:
- ☐ Add helpers for Event ↔ Variant.
- ☐ Add variant forms for get/set target/current/related targets; guard tags and nulls.
4. ☐ Tests:
- ☐ Round-trip per type: POD → variant → POD and compare id/name/type.
- ☐ Parity tests: variant API result matches POD API result for core lookups/destroys/events.
- ☐ Include-cycle check: generated headers remain acyclic (Handles vs Variant).
5. ☐ Bindings:
- ☐ Update Lua (and future bindings) to prefer variant forms; keep POD wrappers temporarily.
- ☐ Deprecation:
- ☐ Doc-only deprecate POD forms once variant paths are proven; remove only when downstreams are off them.
