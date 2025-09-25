# SDOM Architecture Overview

This diagram captures the main components of SDOM, their relationships, and key external dependencies.

SVG (static image):
![Architecture Diagram](diagrams/architecture_overview/diagram-01.svg)

<details>
<summary>View Mermaid source</summary>

```mermaid-norender
%%{init: { 'theme': 'neutral', 'flowchart': { 'curve': 'linear' } }}%%
flowchart TB

  %% Styling
  classDef core fill:#eef7ff,stroke:#4a90e2,color:#1a3b5d,stroke-width:1px
  classDef mod  fill:#f7fff0,stroke:#7bb661,color:#234d20,stroke-width:1px
  classDef iface fill:#fff8e6,stroke:#f0ad4e,color:#5a4500,stroke-width:1px
  classDef util fill:#f5f5f5,stroke:#999,color:#333,stroke-width:1px
  classDef dep  fill:#f2f2ff,stroke:#7c6fe6,color:#262262,stroke-width:1px
  classDef bin  fill:#e8f9f9,stroke:#22a6b3,color:#125a63,stroke-width:1px

  %% Application / Build
  subgraph APP[Application & Build]
    direction TB
    EX[examples/test/prog]:::bin
    LIB[(libSDOM.a)]:::bin
    CMake[CMake Build System]:::util
    EX -->|links| LIB
  end

  %% SDOM Library internals
  subgraph SDOM[SDOM Library]
    direction TB

  Core["Core (Singleton)\n- run(), configure\n- getFactory(), getStage()\n- SDL window/renderer/texture\n- onInit/onUpdate/onEvent/onRender"]:::core

  Factory["Factory\n- registerType<T>()\n- create<T>()\n- own/manage objects\n- orphan/future-child handling"]:::mod

  EventMgr["EventManager\n- add/removeListener\n- dispatch()\n- capture/target/bubble"]:::mod

  Stage["Stage (Root IDisplayObject)"]:::mod

  IDO["IDisplayObject (interface)\n- visual tree\n- events\n- layout/anchor"]:::iface

  IRO["IResourceObject (interface)\n- data/config\n- assets"]:::iface

  DomHandle["DomHandle<T>"]:::util
  ResHandle["ResHandle<T>"]:::util

    SDLUtils[SDOM_SDL_Utils]:::util
  UnitTests["IUnitTest / Unit tests"]:::util

    Event[Event]:::mod
    EventType[EventType]:::mod

    Core --> Factory
    Core --> EventMgr
    Core --> Stage
    Core --> SDLUtils

    Factory --> IDO
    Factory --> IRO
    IDO --> DomHandle
    IRO --> ResHandle

    EventMgr --> Event
    Event --> EventType

    Stage --> IDO
  end

  %% External dependencies
  subgraph EXT[External Dependencies]
    direction TB
    SDL3[SDL3]:::dep
    SDLimg[SDL3_image]:::dep
    SDLttf[SDL3_ttf]:::dep
    SDLmix[SDL3_mixer]:::dep
    JSON[nlohmann/json]:::dep
    Sol2[sol2]:::dep
    Lua[Lua 5.3/5.4]:::dep
  end

  %% Edges between areas
  Core --> SDL3
  Core --> SDLimg
  Core --> SDLttf
  Core --> SDLmix

  LIB -. uses .-> JSON
  LIB -. optional: bindings .-> Sol2
  Sol2 -. links .-> Lua

  CMake -->|produces| LIB
  CMake -->|builds| EX
```

</details>

Notes
- Core composes the SDL video subsystem (window, renderer, main texture) and orchestrates the main loop and callbacks.
- Factory owns and manages both display objects (IDisplayObject) and resource objects (IResourceObject), exposing safe handles for references.
- EventManager implements capture → target → bubble propagation phases across the display tree.
- External dependencies include SDL3 family, nlohmann/json, and optionally sol2 + Lua for scripting.

---

## Runtime Flow (High Level)

SVG (static image):
![Runtime Flow](diagrams/architecture_overview/diagram-02.svg)

<details>
<summary>View Mermaid source</summary>

```mermaid-norender
sequenceDiagram
  autonumber
  participant Prog as examples/test/prog
  participant Core as SDOM::Core
  participant SDL as SDL3 Subsystem
  participant Factory as Factory
  participant Stage as Stage (Root)
  participant EM as EventManager

  Prog->>Core: configure(...)
  Prog->>Core: run()
  Core->>SDL: init window/renderer/texture
  Core->>Factory: ensure Stage type registered
  Core->>Factory: create<Stage>()
  Factory-->>Core: Stage instance
  Core->>Stage: onInit()
  loop Main Loop
    Core->>EM: poll/dispatch events
    EM->>Stage: propagate events (capture→target→bubble)
    Core->>Stage: onUpdate(dt)
    Core->>Stage: onRender()
  end
  Prog-->>Core: onQuit()
  Core->>SDL: shutdown
```

</details>

This complements the per-module documents in the `docs/` folder (Core framework, anchoring, DOM propagation, extensible factory, handles, etc.).