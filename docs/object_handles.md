# Object Handles in SDOM

<table>
	<tr>
		<td style="width: 200px; vertical-align: top;">
			<img src="diagrams/artwork/two-trees.png" alt="Two trees representing SDOM's two distinct trees" style="max-width: 200px; border-radius: 4px;"/>
		</td>
			<td style="vertical-align: top; padding-left: 12px;">
				Like two trees in a garden—one bearing the leaves and branches you can see, and one with roots that quietly nourish—SDOM separates concerns into two living structures. The <strong>visual tree</strong> is composed of <code>IDisplayObject</code> nodes that form the interactive scene: they receive events, participate in layout and anchoring, and render to the screen. In parallel, the <strong>resource tree</strong> is composed of <code>IResourceObject</code> nodes that hold images, fonts, audio, and configuration—everything the visuals consume, but which is not itself part of the scene graph. <code>DomHandle</code> safely references nodes in the visual tree, while <code>ResHandle</code> safely references assets in the resource tree, preventing accidental cross-use and eliminating dangling references. Keeping these trees distinct avoids “grafted” diamond-inheritance pathologies, clarifies lifecycles and traversal, and keeps ownership predictable as your application grows.
			</td>
	</tr>
  
</table>

---

## Two Trees in the Garden


<img src="diagrams/object_handles/diagram-01.svg" alt="Two Trees with Handles" style="width:100%; height:auto;"/>

<details>
<summary>View Mermaid source</summary>

```mermaid-norender
flowchart TB
	classDef dom fill:#eef7ff,stroke:#4a90e2,color:#1a3b5d,stroke-width:1px
	classDef res fill:#fff8e6,stroke:#f0ad4e,color:#5a4500,stroke-width:1px
	classDef handle fill:#f7fff0,stroke:#7bb661,color:#234d20,stroke-width:1px

	%% Top center: generic handle concept ("Garden")
	Garden["Handle&lt;T&gt;<br/>Garden"]:::handle

	%% Left stack: ResHandle above Resource tree
	subgraph RES["Resource Tree (assets)"]
		direction TB
		ResHandle["ResHandle"]:::handle
		RRoot["Resources (root)"]:::res
		Img["Image: logo.png"]:::res
		Font["Font: Roboto.ttf"]:::res
		RRoot --> Img
		RRoot --> Font
	end

	%% Right stack: DomHandle above DOM tree
	subgraph DOM["DOM Tree (visual/display)"]
		direction TB
		DomHandle["DomHandle"]:::handle
		Stage["Stage (root)"]:::dom
		Btn["Button"]:::dom
		Panel["Panel"]:::dom
		Stage --> Btn
		Stage --> Panel
	end

	%% Connections from Garden
	Garden --> ResHandle
	Garden --> DomHandle
```

</details>

Why two trees
- Clear separation of UI logic vs. asset management.
- Independent lifecycles and traversal patterns.
- Type-safe handles (`DomHandle`, `ResHandle`) prevent accidental mixing.

## Overview

In SDOM, a handle is a small, typed, value object that identifies something by name and type, then resolves it on demand through the Factory. `DomHandle` refers to nodes in the visual tree (`IDisplayObject`), while `ResHandle` points to assets in the resource tree (`IResourceObject`). Each handle carries only two strings (name and type), making it cheap to copy, easy to compare, and safe to pass across boundaries (e.g., Lua scripts, configs, or tooling).

Resolution is lazy: when you call `get()`, dereference (`->`/`*`), or convert to `bool`, the handle consults the Factory’s registry to fetch the current live object for that name/type. This indirection cleanly decouples the producer of a reference from the owner of the object. As long as the Factory keeps its name→pointer map up to date, existing handles remain valid even if objects are recreated or resources are reloaded behind the scenes. Conversely, a missing or stale entry simply yields a null handle (`!handle`), which you can detect via `isValid()`.

Handles are first-class participants throughout the API. Events carry `DomHandle` for their `target`, `currentTarget`, and `relatedTarget`, which makes them easy to log (name/type) without leaking raw pointers. UI code can store `DomHandle` for parent/child relationships or late-bound lookups, while systems that need assets (images, fonts, audio, configuration) use `ResHandle` to request them from the resource tree. The split between `DomHandle` and `ResHandle` enforces at the type level that display logic never “accidentally” treats a resource like a DOM node (and vice versa), preserving the integrity of the two-tree design.

Key properties at a glance:
- Type-safe: `DomHandle` and `ResHandle` are distinct types, preventing cross-tree misuse.
- Lightweight: stores name/type strings; copyable, comparable, and nullable.
- Portable: ideal for events, persistence, and tooling; you can reconstruct a handle from name/type in your preferred scripting/config system.
- Stable by indirection: resolution goes through the Factory so ownership and lifetimes stay centralized.
- Practical null semantics: missing entries resolve to null; check with `isValid()` or `if (handle)`.

## Handle Types

### 1. DomHandle

- **Purpose:** References DOM objects (display objects) in the scene graph.
- **Underlying Type:** Points to `IDisplayObject`.
- **Usage:** Used for event targets, parent/child relationships, and DOM traversal.
- **Benefits:** Type-safe access to display object members without casting.

### 2. ResHandle

- **Purpose:** References file-based resources (bitmaps, sounds, fonts, etc.).
- **Underlying Type:** Points to `IResourceObject`.
- **Usage:** Used for resource management, loading, and access.
- **Benefits:** Keeps resource logic separate from DOM logic.

## Design Rationale

- **Separation of Concerns:** DOM objects and resources have different lifecycles and usage patterns.
- **Type Safety:** Prevents accidental mixing of handles and objects.
- **Clarity:** Code is easier to read and maintain.

---

## Inheritance Warning: Avoid Diamond Inheritance

<img src="diagrams/object_handles/diagram-02.svg" alt="Diamond Inheritance Pitfall" style="width:100%; height:auto;"/>

<details>
<summary>View Mermaid source</summary>

```mermaid-norender
%%{init: {"flowchart": { "rankSpacing": 40, "nodeSpacing": 140 }}}%%
flowchart TB
		classDef base fill:#eef7ff,stroke:#4a90e2,color:#1a3b5d,stroke-width:1px
		classDef bad  fill:#ffecec,stroke:#e57373,color:#8a1f1f,stroke-width:1px
		classDef handle fill:#f7fff0,stroke:#7bb661,color:#234d20,stroke-width:1px

		%% Top row
		subgraph TOP[" "]
			direction TB
			IDO["IDisplayObject"]:::base
		end

		%% Middle row (left-to-right): Garden -> Bad
		subgraph MID[" "]
			direction LR
			Garden["Handle&lt;T&gt;<br/>Garden"]:::handle
			Bad["BadThing<br/>(inherits both)"]:::bad
		end

		%% Bottom row
		subgraph BOTTOM[" "]
			direction TB
			IRO["IResourceObject"]:::base
		end

		%% Dotted relation lines from Garden (context, not inheritance)
		Garden -. related .- IDO
		Garden -. related .- IRO

		%% Inheritance edges (top/bottom to middle-right)
		IDO --> Bad
		IRO --> Bad
```

</details>

Best practice
- Do not inherit from both `IDisplayObject` and `IResourceObject` in the same class.
- Keep types clearly in one tree or the other to avoid ambiguity and subtle bugs.

## Example Usage

```cpp
DomHandle buttonHandle = factory.getDomHandle("Button1");
IDisplayObject* button = buttonHandle.get();

ResHandle imageHandle = factory.getResHandle("LogoImage");
IResourceObject* image = imageHandle.get();
```

## Migration Strategy

- Refactor all existing `ResourceHandle` usages for display objects to `DomHandle`.
- Introduce `ResHandle` for new resource types.
- Update factory and registry to support both handle types.

## Best Practices

- Always use the appropriate handle type for the object/resource.
- Do not expose public mutators for critical relationships (e.g., parent/child in DOM).
- Use provided methods for tree and resource manipulation.

---

## Optional: Handle Resolution Flow (reference)

<img src="diagrams/object_handles/diagram-03.svg" alt="Handle Resolution" style="width:100%; height:auto;"/>

<details>
<summary>View Mermaid source</summary>

```mermaid-norender
flowchart LR
	classDef core fill:#f5f5f5,stroke:#999,color:#333,stroke-width:1px
	classDef dom  fill:#eef7ff,stroke:#4a90e2,color:#1a3b5d,stroke-width:1px
	classDef handle fill:#f7fff0,stroke:#7bb661,color:#234d20,stroke-width:1px

	Factory["Factory\nregistry: name -> ptr"]:::core
	H["DomHandle\n(name)"]:::handle
	Obj["IDisplayObject"]:::dom

	H -. lookup by name .-> Factory
	Factory --> Obj
```

</details>
