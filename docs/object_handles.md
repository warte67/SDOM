## Inheritance Warning: Avoid Diamond Inheritance

Care must be taken to avoid inheriting from leaves of both trees (i.e., from both `IDisplayObject` and `IResourceObject`).

- Mixing these base types in a single class can lead to diamond inheritance issues, ambiguous member resolution, and unpredictable behavior.
- Each tree serves a distinct purpose and should remain separate in the inheritance hierarchy.
- Always design new types to clearly belong to one tree or the other, never both.

**Best Practice:**

- Do not create classes that inherit from both `IDisplayObject` and `IResourceObject`.
- Keep DOM objects and resource objects strictly separated to maintain clarity and avoid subtle bugs.
# Two Trees in the Garden


In SDOM, the architecture is built around two distinct trees, both composed of `IDataObject` instances:


1. **DOM Tree (IDisplayObject-based):**
	- Each node is an `IDisplayObject`, representing a visual or interactive element in the application.
	- Managed using `DomHandle` for safe traversal, event propagation, and manipulation.
	- Examples: buttons, panels, containers, custom display objects.

2. **Resource Tree (IResourceObject-based):**
	- Each node is an `IResourceObject`, representing a file-based resource such as images, sounds, fonts, and other assets.
	- Managed using `ResHandle` for efficient loading, caching, and access.
	- Examples: bitmap images, sound files, TrueType fonts, bitmap fonts.


**Why Two Trees?**

- Separating the DOM and resource trees allows for clear boundaries between UI logic and asset management.
- Each tree has its own lifecycle, relationships, and access patterns, reducing complexity and bugs.
- Handles for each tree (`DomHandle` and `ResHandle`) ensure type safety and prevent accidental mixing of display objects and resources.

This dual-tree approach is fundamental to SDOM's design, supporting robust, scalable, and maintainable applications.
# Object Handles in SDOM

## Overview

SDOM uses specialized handle types to manage references to objects and resources in the system. This design ensures type safety, clarity, and robust resource management.

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
