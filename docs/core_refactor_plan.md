# SDOM Core Refactor Plan (v2)

**Version:** Draft 2  
**Date:** 2025-10-29  
**Author:** Jay Faries  
**Status:** Planning / Pre-Implementation  

---

## Overview

The `SDOM::Core` class currently encapsulates most of the engine’s lifecycle and system management.  
While functional, it has grown to over 2,000 lines and now mixes responsibilities for:

- SDL initialization and window management  
- Event dispatching and queuing  
- Factory and object creation  
- Rendering and texture handling  
- Lifecycle callbacks (init, quit, render, update, etc.)  

This refactor modularizes `Core` into smaller, well-defined subsystems to improve clarity, maintainability, and optimization potential.

---

## Goals

1. Reduce `Core` source size to **< 800 lines**.  
2. Treat the **Window** as a permanent root `IDisplayObject` in the DOM tree.  
3. Move event queuing and dispatch into a cleaner, testable **EventManager** implementation.  
4. Prepare for multi-backend rendering (via an **IRenderer** interface).  
5. Retain a **single public interface** (`Core`) for external APIs and language bindings.  
6. Preserve existing behavior and all current unit tests during refactor.

---

## Revised Architectural Model: Window as the Permanent Root

### Core Concept
In SDOM, the `Window` object is not merely a resource manager for SDL—it is also a first-class **`IDisplayObject`**.  
It represents the *permanent root* of the DOM tree, while the `Stage` acts as a **dynamic scene root**, which may be swapped at runtime.

This ensures the following invariants:
- There is **exactly one Window** in the DOM at any time.
- The Window always has **exactly one child**: the current Stage.
- The Stage can be replaced (e.g., for scene transitions, menus, or tests) without affecting the rest of the engine.

---

### Hierarchy Overview

```
Window::PreRender()
  ├─ Stage::PreRender()
  │    ├─ Button::PreRender()
  │    ├─ Label::PreRender()
  │    └─ Checkbox::PreRender()
  │    ├─ Button::Render()
  │    ├─ Label::Render()
  │    └─ Checkbox::Render()
  └─ Stage::Render()
Window::Render()

```
This ordering guarantees that:
- Parent-before-children composition builds from back to front.
- Children-before-parent overlays preserve proper visual stacking.
- Overlays (Render phase) are drawn last, ensuring focus rings, outlines, or debug visuals always appear on top.

This makes the event system, rendering, and update logic uniformly traversable from the top-level `Window` node.  
Every event begins at the Window and propagates downward through the Stage hierarchy.

---

### Updated Responsibilities

#### `Window` (now derived from `IDisplayObject`)
**Purpose:**  
Acts as both the permanent DOM root and the interface to SDL3’s windowing system.

**Key Responsibilities:**
- Inherit from `IDisplayObject`
- Contain SDL initialization/shutdown logic
- Create and destroy SDL_Window, SDL_Renderer, and primary render texture
- Manage logical presentation and pixel ratio
- Dispatch `WindowEvent`s (resize, focus, show/hide, fullscreen transitions)
- Always contain a single child: the active `Stage`

**Example Definition:**
```cpp
class Window : public IDisplayObject {
public:
    struct InitParams {
        std::string title;
        int width;
        int height;
        SDL_WindowFlags flags;
        SDL_PixelFormat pixelFormat;
        SDL_RendererLogicalPresentation presentation;
    };

    bool onInit(const InitParams& params);
    void onQuit();
    void onRender(SDL_Renderer* renderer) override;
    void onEvent(Event& event) override;
    void onUpdate(float deltaTime) override;

    // Stage management
    void setStage(DisplayHandle newStage);
    DisplayHandle getStage() const;

    // SDL resource access
    SDL_Window* getSDLWindow() const;
    SDL_Renderer* getSDLRenderer() const;
    SDL_Texture* getTexture() const;

private:
    DisplayHandle stage_;   // the current scene root
    SDL_Window* sdlWindow_ = nullptr;
    SDL_Renderer* sdlRenderer_ = nullptr;
    SDL_Texture* renderTarget_ = nullptr;
};
```

---

### `Stage`
**Purpose:**  
Acts as the logical scene root for all display objects.  
Unlike the Window, it can be replaced during runtime.

**Responsibilities:**
- Contain the scene graph (UI, layers, widgets, etc.)
- Handle per-frame updates, rendering, and layout for its children
- Dispatch scene-local events (non-global)
- Optionally maintain per-scene resources or scripts

**Example:**
```cpp
class Stage : public IDisplayObject {
public:
    void onRender(SDL_Renderer* renderer) override;
    void onUpdate(float deltaTime) override;
    void onEvent(Event& event) override;

    void loadScene(const std::string& path);
    void unloadScene();
};
```

---

### Implications for `Core` and `EventManager`

#### `Core`
- `Core` owns exactly one `Window` instance.
- The `Window` owns the current `Stage` as a child `DisplayHandle`.
- `Core::getRootNode()` returns the `Window`, not the `Stage`.
- Lifecycle hooks (`onRender`, `onEvent`, `onUpdate`) are forwarded to the `Window`.

#### `EventManager`
- All events begin dispatch at the `Window` node.
- `EventManager` uses the `Window` as the starting root for capture and bubbling.
- Window-level events (`Focus`, `Resize`, `Show/Hide`) are handled at the root.
- Scene events continue propagating normally through the `Stage`.

---

### Benefits

| Category | Benefit |
|-----------|----------|
| **Architecture** | Stable top-level root simplifies event propagation and rendering. |
| **Scene Management** | Easy scene swaps without tearing down the entire DOM. |
| **Extensibility** | Allows multi-context or multi-window setups later. |
| **Consistency** | All rendering and events flow through a single IDisplayObject hierarchy. |
| **Future-Proofing** | Multi-window or split-view scenarios require minimal API change. |

---

### Example Flow (Simplified)

```cpp
Core::onInit() {
    window_ = std::make_unique<Window>();
    window_->onInit(config);

    DisplayHandle stage = factory_->createDisplayObject("Stage");
    window_->setStage(stage);
}

Core::onUpdate(float dt) {
    window_->onUpdate(dt);
}

Core::onRender() {
    window_->onRender(renderer_);
}

EventManager::dispatchEvent(std::unique_ptr<Event> evnt) {
    DisplayHandle root = getCore().getWindow();
    traverseEvent(root, evnt);
}
```

---

**Summary:**  
This model cements `Window` as the **permanent anchor** of the SDOM tree, simplifying rendering, event propagation, resource management, and scene lifecycle.  
The `Stage` becomes lightweight and easily replaceable — perfectly matching DOM semantics of a live scene container within a persistent display context.
