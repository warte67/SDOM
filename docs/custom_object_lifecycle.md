# Custom Object Lifecycle in SDOM

## Where to Put Your Logic: Rendering, Layout, Updates, and Input

Every display object in SDOM participates in the **frame loop**.  
If your custom object is going to *move, animate, respond to input, draw things,* or change layout, you’ll do that in one of the core virtual methods it inherits from `IDisplayObject`.

Here’s the lifecycle:

```
onInit()   → Runs once when created
onUpdate() → Runs every frame (logic, animation, timers)
onEvent()  → Runs whenever input or UI events occur
onRender() → Draws the object (background, content, borders, etc.)
onQuit()   → Runs when the program shuts down or object is destroyed
```

Let’s break down what each is **for**, and what it is **not** for.

---

### `onInit()`: Setup, Don’t Draw Yet

This is called *right after* the object is registered in the Factory.

Use it for:
- Creating child objects
- Computing initial layout
- Loading secondary assets
- Connecting to event listeners

```cpp
bool TogglePanel::onInit()
{
    // Example: Create a label inside the panel
    auto label = Core::getFactory().create("Label", { .name="panelText", .text="Hello!" });
    addChild(label);

    return true;
}
```

Don’t draw here — nothing is on screen yet.

---

### `onUpdate(float dt)`: Animation / State Changes

Called every frame before rendering.  
Great for timers, animations, and logic.

```cpp
void TogglePanel::onUpdate(float dt)
{
    if (isCollapsed_) {
        height_ = std::max(height_ - int(200 * dt), 20);
    } else {
        height_ = std::min(height_ + int(200 * dt), 200);
    }
}
```

**Rule of thumb:**  
If it changes *state*, call `setDirty()`.

---

### `onEvent(const Event&)`: Input & UI Interaction

You don’t need to poll SDL manually — the event system handles that.  
You just react to semantic events.

```cpp
void TogglePanel::onEvent(const Event& ev)
{
    if (ev.type == EventType::MouseDown && ev.target == getHandle()) {
        setCollapsed(!isCollapsed_);
    }
}
```

SDOM handles capture → target → bubble automatically.

---

### `onRender()`: *Actually draw something*

This is where you:

- Draw backgrounds
- Draw outlines or borders
- Draw sprites/textures
- Render any immediate-mode visuals

```cpp
void TogglePanel::onRender()
{
    auto r = Core::getRenderer();

    // Fill background
    SDL_Color c = isCollapsed_ ? SDL_Color{50, 50, 50, 255}
                               : SDL_Color{90, 90, 90, 255};
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);

    SDL_FRect rect = { getX(), getY(), getWidth(), getHeight() };
    SDL_RenderFillRect(r, &rect);

    // Borders (optional)
    drawBorder();
}
```

**Important:**  
Don’t draw children here — SDOM handles that automatically.

---

### `onQuit()`: Clean Up *If Needed*

Most objects don’t need custom cleanup because:
- Lifetime is managed by the Factory
- Rendered textures are owned by the renderer or Label’s own cache
- Children are cleaned when removed or orphan-collected

Only override if needed:

```cpp
void TogglePanel::onQuit()
{
    // Most objects will not need this.
}
```

---

## Putting It All Together

```
              +----------------+
              |  constructor   |
              +--------+-------+
                       |
                       v
               +-------+------+
               |   onInit()   |  <-- setup & wiring
               +-------+------+
                       |
        Every frame     |      Input events
     +------------------+-------------+
     |                                |
     v                                v
+----+-----+                     +----+------+
| onUpdate |                     | onEvent   | <-- logic & state change
+----+-----+                     +----+------+
     |                                |
     +---------------+----------------+
                     |
                     v
               +-----+------+
               |  onRender  |  <-- draw to screen
               +------------+
```

---

## Practical Tip — When in Doubt:

| You want to… | Use… |
|--------------|------|
| Create or configure sub-objects | `onInit()` |
| Animate something over time | `onUpdate()` |
| Respond to user input | `onEvent()` |
| Draw visuals | `onRender()` |
| Release external resources | `onQuit()` |

---

