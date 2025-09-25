# SDOM Display Object Anchoring Design
## Display Object Anchoring System (Updated September 2, 2025)

### Anchor Naming Conventions
- Anchor points are represented as enums in code: `AnchorPoint::TOP_LEFT`, `AnchorPoint::TOP_CENTER`, `AnchorPoint::TOP_RIGHT`, `AnchorPoint::MIDDLE_LEFT`, `AnchorPoint::MIDDLE_CENTER`, `AnchorPoint::MIDDLE_RIGHT`, `AnchorPoint::BOTTOM_LEFT`, `AnchorPoint::BOTTOM_CENTER`, `AnchorPoint::BOTTOM_RIGHT`.
- In JSON or config, these may be written as strings: `"top_left"`, `"middle_center"`, etc. The mapping is direct.

### Edge-Based Layout
- Each display object tracks its edges: `left_`, `right_`, `top_`, `bottom_`.
- Each edge can have its own anchor point, allowing for asymmetric anchoring.
- All movement, resizing, and anchor changes are performed via edge setters/getters: `setLeft`, `setRight`, `setTop`, `setBottom`, `getLeft`, `getRight`, `getTop`, `getBottom`.

### Position and Size Calculation
- The world position of each edge is calculated as:
    - `getLeft() = parent_anchor_x + left_`
    - `getRight() = parent_anchor_x + right_`
    - `getTop() = parent_anchor_y + top_`
    - `getBottom() = parent_anchor_y + bottom_`
    - Where `parent_anchor_x` and `parent_anchor_y` are determined by the parent's anchor reference for the respective edge.
- **Width and height are always computed as:**
    - `width = getRight() - getLeft()`
    - `height = getBottom() - getTop()`

### Anchor Accessors/Mutators
- Use the following API methods for anchor management:
    - `setAnchorLeft(AnchorPoint ap)`
    - `setAnchorRight(AnchorPoint ap)`
    - `setAnchorTop(AnchorPoint ap)`
    - `setAnchorBottom(AnchorPoint ap)`
    - `getAnchorLeft()`, `getAnchorRight()`, `getAnchorTop()`, `getAnchorBottom()`

### Changing Anchor Points
- When an anchor is changed, the corresponding local offset (`left_`, `right_`, etc.) is recalculated so the world position of the edge does not move.
- Only the local offset for the changed anchor is updated; other edges remain unchanged unless their anchors are also changed.

### Drag-and-Drop and Reparenting
- **When reparenting or during drag-and-drop:**
    - Record the world positions of all edges (`getLeft()`, `getRight()`, `getTop()`, `getBottom()`) before changing parent or anchor.
    - After changing parent or anchor, recalculate all local offsets using the new parent's anchor references and the recorded world positions.
- This ensures the object's position and size remain correct regardless of anchor configuration or parent changes.

### Summary
- The anchoring system is now robust, edge-based, and supports asymmetric anchors.
- All geometry calculations and API methods have been updated to reflect this design.
## Overview
This document describes the anchor point system for positioning and sizing display objects in the SDOM API. Anchoring enables flexible, responsive layouts by allowing each display object to reference its position and size to any of nine anchor points on its parent.

## Anchor Points
Each display object can specify anchor points for its four edges (left/X, top/Y, right/width, bottom/height) in relation to its parent. The nine possible anchor points are:

- `top_left`
- `top_center`
- `top_right`
- `middle_left`
- `middle_center`
- `middle_right`
- `bottom_left`
- `bottom_center`
- `bottom_right`

### Reversed versions for user convenience
- `center_middle`
- `right_middle`
- `left_middle`
- `center_top`
- `right_top`
- `left_top`
- `center_bottom`
- `right_bottom`
- `left_bottom`

For convenience, aliases like `left`, `center`, `right`, `top`, `middle`, and `bottom` are also supported.

## JSON Interface
Anchor points are specified in the JSON configuration for each display object. For backward compatibility, if anchor fields are omitted, all anchors default to `top_left`.

Example:
```json
{
    "x": 10,
    "y": 10,
    "width": -10,
    "height": -10,
    "child_anchor_x": "left",
    "child_anchor_width": "right",
    "child_anchor_y": "top",
    "child_anchor_height": "bottom"
}
```
This configuration creates a child object with a 10-pixel margin on all sides, stretching as the parent resizes.

## Position and Size Calculation
- **X (left edge):** Anchored to the specified parent anchor point, plus offset.
- **Y (top edge):** Anchored to the specified parent anchor point, plus offset.
- **Width (right edge):** Anchored to the specified parent anchor point, minus offset.
- **Height (bottom edge):** Anchored to the specified parent anchor point, minus offset.

For example:
```
child.x = parent.anchor_x + offset_x
child.y = parent.anchor_y + offset_y
child.width = parent.anchor_width - offset_width
child.height = parent.anchor_height - offset_height
```

## Changing Anchor Points
When an anchor point is changed, the offset is recalculated so the display object remains visually in the same position and size. This ensures a smooth user experience and prevents layout jumps.

## Hierarchical Anchoring
Anchor calculations are recursive: each display object references its parent, which may itself be anchored to its own parent, up to the stage. This enables complex, nested layouts.

## Backward Compatibility
Existing JSON and code remain compatible. If anchor fields are not specified, the system defaults to `top_left` anchors, preserving current behavior.

## Benefits
- Flexible, responsive layouts
- Easy margin and stretching logic
- Consistent positioning and sizing
- Backward compatibility

## Example Use Cases
- Center a child object within its parent
- Stretch a child object with margins as the parent resizes
- Align a child to any edge or corner of its parent


---

This anchor system makes SDOM display objects highly flexible and professional, supporting modern UI layout needs.

---

## Anchoring Diagrams

### 1) Parent Anchor Grid and Child Edge Anchors

SVG (static image):
![Anchors Grid](diagrams/display_object_anchoring/diagram-01.svg)

<details>
<summary>View Mermaid source</summary>

```mermaid-norender
%% Parent has 9 anchors; child edges reference any of them with offsets
flowchart TB
    classDef parent fill:#eef7ff,stroke:#4a90e2,color:#1a3b5d
    classDef anchor fill:#fff8e6,stroke:#f0ad4e,color:#5a4500
    classDef child  fill:#f7fff0,stroke:#7bb661,color:#234d20

    subgraph P["Parent (9-point anchor grid)"]
        direction TB
        %% Top row
        TL["top_left"]:::anchor --> TC["top_center"]:::anchor --> TR["top_right"]:::anchor
        %% Middle row
        ML["middle_left"]:::anchor --> MC["middle_center"]:::anchor --> MR["middle_right"]:::anchor
        %% Bottom row
        BL["bottom_left"]:::anchor --> BC["bottom_center"]:::anchor --> BR["bottom_right"]:::anchor
    end

    subgraph C["Child (edges + local offsets)"]
        direction TB
        L["Left edge\nleft_"]:::child
        T["Top edge\ntop_"]:::child
        R["Right edge\nright_"]:::child
        B["Bottom edge\nbottom_"]:::child
    end

    %% Example anchoring links (illustrative)
    L -. anchored to .-> ML
    T -. anchored to .-> TC
    R -. anchored to .-> MR
    B -. anchored to .-> BC

    %% Notes (rendering might ignore comments in some mmdc versions)
```

</details>

Key idea: each edge uses its own parent anchor reference plus a local offset; width/height derive from right-left and bottom-top.

### 2) Anchor Change Recalculation (No Visual Jump)

SVG (static image):
![Anchor Change Flow](diagrams/display_object_anchoring/diagram-02.svg)

<details>
<summary>View Mermaid source</summary>

```mermaid-norender
sequenceDiagram
    autonumber
    participant User as API Caller
    participant Child as DisplayObject
    participant Parent as Parent
    participant Layout as Layout/Anchoring

    User->>Child: setAnchorLeft(newAp)
    Child->>Layout: request anchor change
    Layout->>Parent: get parent_anchor_x for newAp
    Layout->>Child: get current world edges\n(getLeft/Right/Top/Bottom)
    Note over Layout,Child: Recalculate only the affected local offset\nso the world edge stays fixed
    Layout-->>Child: left_ = world_left - parent_anchor_x
    Child-->>User: done (no visual jump)

```

</details>

This flow illustrates the invariant maintained during anchor changes: the world edge position remains the same while only the corresponding local offset is updated.