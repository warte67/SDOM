# IPanelObject Design Document

## Design Goals for `IPanelObject`

- **Base class** for all scalable UI containers (Panel, Frame, Group, Button, etc.)
- **Fixed 9-slice grid:** uses the predefined index offsets from the default or supplied SpriteSheet (see above tables)
- **Support for theming/skins:** to change appearance, supply a custom SpriteSheet with the same 9-slice layout

If a user wishes to use a different number of tiles, or render their windows, buttons, group boxes, etc. in a different way, they can create custom DisplayObjects. The 9-slice layout and offsets are fixed for all IPanelObject-based panels.

## What is a 9-slice (9-patch) panel?

A **9-slice** (or 9-patch) panel is a technique for drawing scalable UI elements (like panels, buttons, frames) using a grid of 9 regions:
- **4 corners:** drawn at original size (not stretched)
- **4 edges:** stretched in one direction (horizontally or vertically)
- **1 center:** stretched in both directions

## What is a 9-slice (9-patch) panel?

A **9-slice** (or 9-patch) panel is a technique for drawing scalable UI elements (like panels, buttons, frames) using a grid of 9 regions:
- **4 corners:** drawn at original size (not stretched) but they may be clipped towards the inside.
- **4 edges:** stretched in one direction (horizontally or vertically)
- **1 center:** stretched in both directions

**Corner tiles are never stretched.**  
- When the panel is larger than the combined size of the corners, the corners are drawn at their full size, and the edges and center are stretched to fill the remaining space.
- When the panel is *smaller* than the combined width (or height) of the corners, the corners are **clipped** (not stretched) so that only the visible portion of each corner is rendered. For example, if your sprite tiles are 8x8, 12x12, or any other size, the logic remains the same: the implementation uses the actual sprite width and height for each tile, so custom-sized sprite tiles are fully supported.

**Edge tiles** are stretched only in one direction (horizontally for top/bottom edges, vertically for left/right edges).  
**The center tile** is stretched in both directions to fill the remaining space.

This approach allows us to create resizable, skinnable UI containers that look good at any size, using a single sprite sheet of any tile size, while ensuring that corners always retain their intended appearance—even when the panel is very small.


## Reference: sprite_8x8 Icon Index Catalog

For a complete catalog of the available 9-slice graphics and their indices, see [sprite_8x8.md](sprite_8x8.md).

### Predefined 9-Slice Sets in `sprite_8x8`

The default/integrated `sprite_8x8` SpriteSheet provides several predefined 9-slice sets for UI panels and controls. Each set consists of 9 tiles arranged in a 3x3 grid, with the following index offsets:

| Base Index | Set Name                      |
|-----------:|-------------------------------|
|         0  | Button Up                     |
|         9  | Button Up (mouse over/selected)|
|        18  | Button Down                   |
|        27  | Button Down (mouse over/selected)|
|        36  | Frame                         |
|        45  | Group                         |

Within each set, the 9 tiles are arranged as follows:

| Offset | Position        |
|-------:|----------------|
| 0      | Top-Left       |
| 1      | Top-Center     |
| 2      | Top-Right      |
| 3      | Left-Center    |
| 4      | Center         |
| 5      | Right-Center   |
| 6      | Bottom-Left    |
| 7      | Bottom-Center  |
| 8      | Bottom-Right   |

For example, the Frame set uses indices 36–44, where 36 is the top-left corner, 37 is the top-center, and so on.

Refer to [sprite_8x8.md](sprite_8x8.md) for a full list of all available icons and their descriptions.


- **Base class** for all scalable UI containers (Panel, Frame, Group, Button, etc.)
- **Configurable 9-slice grid:** specify which indices in the SpriteSheet correspond to each of the 9 regions
- **Support for theming/skins:** easily swap out the SpriteSheet or indices for different looks
- **Automatic layout:** handles stretching and positioning of each region based on the panel’s size
- **Simple interface:** derived classes only need to specify their skin and content area
- **Content Anchoring:** Content will be attached as child nodes and anchored by their four edges to any of their parent's anchoring reference points. See [display_object_anchoring.md](display_object_anchoring.md) for details.

---

## Suggested Steps

1. **Define the `IPanelObject` interface:**
    - Member for SpriteSheet reference and padding/margins
    - Virtual `drawPanel()` method to render the 9 regions using the fixed offsets
    - Method to set/get the SpriteSheet

2. **Implement a default 9-slice panel:**
    - Use indices 0–53 from your default `sprite_8x8.md` SpriteSheet
    - Use the fixed mapping for corners, edges, and center

3. **Scaffold derived classes:**
    - `Panel`, `Frame`, `Group`, `Button`, etc. inherit from `IPanelObject`
    - Each can override or extend the base drawing/layout as needed

4. **Add JSON/config support:**
    - All DisplayObjects and ResourceObjects, including IPanelObject, should support JSON serialization and deserialization for both reading and writing.
    - Allow specifying the skin (SpriteSheet) in JSON
    - Support for custom themes

**Note:** This is essential for future tooling, such as a form editor, which will require all objects to be JSON-ready for both import/export and live editing. Ensuring robust JSON support now will make future development and testing much easier.

5. **Test with various sizes and skins:**
    - Ensure the 9-slice logic works for all reasonable panel sizes

2. **Implement a default 9-slice panel:**
    - Use indices 0–53 from your default `sprite_8x8.md` SpriteSheet
    - Provide a default mapping for corners, edges, and center

3. **Scaffold derived classes:**
    - `Panel`, `Frame`, `Group`, `Button`, etc. inherit from `IPanelObject`
    - Each can override or extend the base drawing/layout as needed

4. **Add JSON/config support:**
    - Allow specifying the skin (SpriteSheet and indices) in JSON
    - Support for custom themes

5. **Test with various sizes and skins:**
    - Ensure the 9-slice logic works for all reasonable panel sizes

---


## Interface Sketch

```cpp
class IPanelObject : public IDisplayObject {
public:
    void setSpriteSheet(std::shared_ptr<SpriteSheet> sheet);
    virtual void drawPanel(SDL_Renderer* renderer);

protected:
    std::shared_ptr<SpriteSheet> spriteSheet_;
    // Uses fixed 9-slice offsets as described above
    // ...padding, margin, etc...
};
```


## Note on Button Objects and State Handling

The `Button` class is itself a 9-slice (9-panel) object, typically derived from `IPanelObject` for layout and rendering. To provide a common interface for button state (such as up, down, hovered, checked, etc.), a minimal `IButtonObject` interface can be used. This interface should provide:

- `virtual ButtonState getState() const = 0;`
- `virtual void setState(ButtonState state) = 0;`

where `ButtonState` is an extensible enumeration of possible button states (up, down, hovered, checked, unchecked, indeterminate, disabled, etc.).

Button classes may use multiple inheritance to implement both `IPanelObject` and `IButtonObject`. To avoid diamond inheritance issues, concrete button classes should be marked as `final` (e.g., `class Button final : public IPanelObject, public IButtonObject`).

This approach keeps the button state interface flexible and lightweight, while allowing for a wide variety of button types and behaviors. Hover state and tooltip logic are best handled in `IDisplayObject`, so all display objects can benefit from these features.

**Summary of design choices:**
- Use multiple inheritance only for pure interface types (no data members)
- Mark concrete button classes as `final` to prevent inheritance issues
- Keep `IButtonObject` minimal and focused on state
- Place hover/tooltip logic in `IDisplayObject` for maximum flexibility

