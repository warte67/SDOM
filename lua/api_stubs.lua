-- lua/api_stubs.lua
-- Editor-only Lua API stubs for SDOM bindings (for LuaLS/EmmyLua).
-- Not loaded at runtime. Keep in sync with Core and IDisplayObject binders.

---@diagnostic disable: undefined-global
---@diagnostic disable: duplicate-set-field

-- Types ---------------------------------------------------------------------
---@class EventType
---@field name string

---@class Bounds
---@field left number
---@field right number
---@field top number
---@field bottom number

---@class SDL_Color
---@field r integer
---@field g integer
---@field b integer
---@field a integer

---@class DisplayHandle

---@class UnitTests

---@class Factory


-- Global convenience --------------------------------------------------------
---@type Core
Core = Core or {}

-- ---@type UnitTests
-- UnitTests = UnitTests or {}

-- ---@type Factory
-- Factory = Factory or {}

IconIndex = IconIndex or {}
ArrowDirection = ArrowDirection or {}
EventType = EventType or {}

--- Global helper mirroring Core:configure(table)
---@param cfg table
-- Intentionally global helper alias for Core.configure
---@diagnostic disable-next-line: lowercase-global
function configure(cfg) end

-- lua/api_stubs.lua
-- Editor-only Lua API stubs for SDOM bindings (for LuaLS/EmmyLua).
-- Not loaded at runtime. Keep these stubs roughly aligned with C++ binders.

---@diagnostic disable: undefined-global
---@diagnostic disable: duplicate-set-field

---@class DisplayHandle
---@field isValid fun(self: DisplayHandle): boolean
---@field get fun(self: DisplayHandle): any
---@field getName fun(self: DisplayHandle): string
---@field getType fun(self: DisplayHandle): string
---@field setName fun(self: DisplayHandle, name: string)
---@field setType fun(self: DisplayHandle, typeName: string)
---@field cleanAll fun(self: DisplayHandle)
---@field getDirty fun(self: DisplayHandle): boolean
---@field setDirty fun(self: DisplayHandle)
---@field isDirty fun(self: DisplayHandle): boolean
---@field printTree fun(self: DisplayHandle)
---@field addEventListener fun(self: DisplayHandle, spec: table)
---@field addEventListener fun(self: DisplayHandle, type: (EventType|string), listener: fun(e: any), useCapture?: boolean, priority?: integer)
---@field removeEventListener fun(self: DisplayHandle, spec: table)
---@field removeEventListener fun(self: DisplayHandle, type: (EventType|string), listener: fun(e: any), useCapture?: boolean)
---@field hasEventListener fun(self: DisplayHandle, type: EventType, useCapture?: boolean): boolean
---@field queue_event fun(self: DisplayHandle, type: EventType, init?: fun(e: any))
---@field addChild fun(self: DisplayHandle, child: (DisplayHandle|string|table)): boolean
---@field removeChild fun(self: DisplayHandle, child: (DisplayHandle|string|table)): boolean
---@field hasChild fun(self: DisplayHandle, child: (DisplayHandle|string|table)): boolean
---@field getChild fun(self: DisplayHandle, spec: (string|table)): (DisplayHandle|nil)
---@field getChildren fun(self: DisplayHandle): DisplayHandle[]
---@field countChildren fun(self: DisplayHandle): integer
---@field getParent fun(self: DisplayHandle): (DisplayHandle|nil)
---@field setParent fun(self: DisplayHandle, parent: DisplayHandle)
---@field isAncestorOf fun(self: DisplayHandle, child: (DisplayHandle|string)): boolean
---@field isDescendantOf fun(self: DisplayHandle, ancestor: (DisplayHandle|string)): boolean
---@field removeFromParent fun(self: DisplayHandle): boolean
---@field removeDescendant fun(self: DisplayHandle, target: (DisplayHandle|string)): boolean
---@field getBounds fun(self: DisplayHandle): Bounds
---@field setBounds fun(self: DisplayHandle, b: Bounds|table)
---@field getColor fun(self: DisplayHandle): SDL_Color
---@field setColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field getForegroundColor fun(self: DisplayHandle): SDL_Color
---@field setForegroundColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field getBackgroundColor fun(self: DisplayHandle): SDL_Color
---@field setBackgroundColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field getBorderColor fun(self: DisplayHandle): SDL_Color
---@field setBorderColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field getOutlineColor fun(self: DisplayHandle): SDL_Color
---@field setOutlineColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field getDropshadowColor fun(self: DisplayHandle): SDL_Color
---@field setDropshadowColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field hasBorder fun(self: DisplayHandle): boolean
---@field hasBackground fun(self: DisplayHandle): boolean
---@field setBorder fun(self: DisplayHandle, enabled: boolean)
---@field setBackground fun(self: DisplayHandle, enabled: boolean)
---@field getMaxPriority fun(self: DisplayHandle): integer
---@field getMinPriority fun(self: DisplayHandle): integer
---@field getPriority fun(self: DisplayHandle): integer
---@field setPriority fun(self: DisplayHandle, value: integer)
---@field setPriority fun(self: DisplayHandle, spec: table)
---@field setPriority fun(self: DisplayHandle, spec: (DisplayHandle|string|table), value: integer)
---@field sortChildrenByPriority fun(self: DisplayHandle)
---@field setToHighestPriority fun(self: DisplayHandle)
---@field setToHighestPriority fun(self: DisplayHandle, spec: (DisplayHandle|string|table))
---@field setToLowestPriority fun(self: DisplayHandle)
---@field setToLowestPriority fun(self: DisplayHandle, spec: (DisplayHandle|string|table))
---@field moveToTop fun(self: DisplayHandle)
---@field moveToTop fun(self: DisplayHandle, spec: (DisplayHandle|string|table))
---@field moveToBottom fun(self: DisplayHandle)
---@field moveToBottom fun(self: DisplayHandle, spec: table)
---@field bringToFront fun(self: DisplayHandle)
---@field bringToFront fun(self: DisplayHandle, spec: table)
---@field sendToBack fun(self: DisplayHandle)
---@field sendToBack fun(self: DisplayHandle, spec: table)
---@field sendToBackAfter fun(self: DisplayHandle, limit: DisplayHandle)
---@field sendToBackAfter fun(self: DisplayHandle, spec: (DisplayHandle|string|table), limit: DisplayHandle)
---@field getZOrder fun(self: DisplayHandle): integer
---@field setZOrder fun(self: DisplayHandle, z: integer)
---@field setZOrder fun(self: DisplayHandle, spec: table)
---@field setKeyboardFocus fun(self: DisplayHandle)
---@field isKeyboardFocused fun(self: DisplayHandle): boolean
---@field isMouseHovered fun(self: DisplayHandle): boolean
---@field isClickable fun(self: DisplayHandle): boolean
---@field setClickable fun(self: DisplayHandle, enabled: boolean)
---@field isEnabled fun(self: DisplayHandle): boolean
---@field setEnabled fun(self: DisplayHandle, enabled: boolean)
---@field isHidden fun(self: DisplayHandle): boolean
---@field setHidden fun(self: DisplayHandle, hidden: boolean)
---@field isVisible fun(self: DisplayHandle): boolean
---@field setVisible fun(self: DisplayHandle, visible: boolean)
---@field getTabPriority fun(self: DisplayHandle): integer
---@field setTabPriority fun(self: DisplayHandle, index: integer)
---@field isTabEnabled fun(self: DisplayHandle): boolean
---@field setTabEnabled fun(self: DisplayHandle, enabled: boolean)
---@field getX fun(self: DisplayHandle): integer
---@field getY fun(self: DisplayHandle): integer
---@field getWidth fun(self: DisplayHandle): integer
---@field getHeight fun(self: DisplayHandle): integer
---@field setX fun(self: DisplayHandle, x: integer)
---@field setY fun(self: DisplayHandle, y: integer)
---@field setWidth fun(self: DisplayHandle, w: integer)
---@field setHeight fun(self: DisplayHandle, h: integer)
---@field getAnchorTop fun(self: DisplayHandle): integer
---@field getAnchorLeft fun(self: DisplayHandle): integer
---@field getAnchorBottom fun(self: DisplayHandle): integer
---@field getAnchorRight fun(self: DisplayHandle): integer
---@field setAnchorTop fun(self: DisplayHandle, ap: integer)
---@field setAnchorLeft fun(self: DisplayHandle, ap: integer)
---@field setAnchorBottom fun(self: DisplayHandle, ap: integer)
---@field setAnchorRight fun(self: DisplayHandle, ap: integer)
---@field getLeft fun(self: DisplayHandle): number
---@field getRight fun(self: DisplayHandle): number
---@field getTop fun(self: DisplayHandle): number
---@field getBottom fun(self: DisplayHandle): number
---@field setLeft fun(self: DisplayHandle, v: number)
---@field setRight fun(self: DisplayHandle, v: number)
---@field setTop fun(self: DisplayHandle, v: number)
---@field setBottom fun(self: DisplayHandle, v: number)
---@field getLocalLeft fun(self: DisplayHandle): number
---@field getLocalRight fun(self: DisplayHandle): number
---@field getLocalTop fun(self: DisplayHandle): number
---@field getLocalBottom fun(self: DisplayHandle): number
---@field setLocalLeft fun(self: DisplayHandle, v: number)
---@field setLocalRight fun(self: DisplayHandle, v: number)
---@field setLocalTop fun(self: DisplayHandle, v: number)
---@field setLocalBottom fun(self: DisplayHandle, v: number)
---@field orphanPolicyFromString fun(self: DisplayHandle, s: string): integer
---@field orphanPolicyToString fun(self: DisplayHandle, policy: integer): string
---@field setOrphanRetentionPolicy fun(self: DisplayHandle, s: string)
---@field getOrphanRetentionPolicyString fun(self: DisplayHandle): string
---@field getOrphanGrace fun(self: DisplayHandle): integer
---@field setOrphanGrace fun(self: DisplayHandle, ms: integer)
---@field value number
---@field min number
---@field max number

---@class AssetHandle
---@field isValid fun(self: AssetHandle): boolean
---@field getName fun(self: AssetHandle): string
---@field getType fun(self: AssetHandle): string
---@field getFilename fun(self: AssetHandle): string
---@field setName fun(self: AssetHandle, name: string)
---@field setType fun(self: AssetHandle, typeName: string)
---@field setFilename fun(self: AssetHandle, filename: string)

---@class Core
---@field quit fun(self: Core)
---@field shutdown fun(self: Core)
---@field configure fun(self: Core, cfg: table)
---@field configureFromFile fun(self: Core, filename: string)
---@field registerOn fun(self: Core, name: "Init"|"Quit"|"Update"|"Event"|"Render"|"UnitTest"|"WindowResize", fn: function)
---@field setRootNode fun(self: Core, nameOrHandle: (string|DisplayHandle))
---@field setRoot fun(self: Core, nameOrHandle: (string|DisplayHandle))
---@field setStage fun(self: Core, nameOrHandle: (string|DisplayHandle))
---@field getRoot fun(self: Core): DisplayHandle
---@field getRootHandle fun(self: Core): DisplayHandle
---@field getStage fun(self: Core): DisplayHandle
---@field getStageHandle fun(self: Core): DisplayHandle
---@field getIsTraversing fun(self: Core): boolean
---@field setIsTraversing fun(self: Core, traversing: boolean)
---@field createDisplayObject fun(self: Core, typeName: string, cfg: table): DisplayHandle
---@field getDisplayObject fun(self: Core, name: string): (DisplayHandle|nil)
---@field hasDisplayObject fun(self: Core, name: string): boolean
---@field createAssetObject fun(self: Core, typeName: string, cfg: table): AssetHandle
---@field createAsset fun(self: Core, typeName: string, cfg: table): AssetHandle
---@field getAssetObject fun(self: Core, name: string): (AssetHandle|nil)
---@field hasAssetObject fun(self: Core, name: string): boolean
---@field doTabKeyPressForward fun(self: Core)
---@field doTabKeyPressReverse fun(self: Core)
---@field handleTabKeyPress fun(self: Core)
---@field handleTabKeyPressReverse fun(self: Core)
---@field setKeyboardFocusedObject fun(self: Core, handle: DisplayHandle)
---@field getKeyboardFocusedObject fun(self: Core): DisplayHandle
---@field setMouseHoveredObject fun(self: Core, handle: DisplayHandle)
---@field getMouseHoveredObject fun(self: Core): DisplayHandle
---@field getWindowTitle fun(self: Core): string
---@field setWindowTitle fun(self: Core, title: string)
---@field getElapsedTime fun(self: Core): number
---@field getDeltaTime fun(self: Core): number
---@field pumpEventsOnce fun(self: Core)
---@field pushMouseEvent fun(self: Core, args: table)
---@field pushKeyboardEvent fun(self: Core, args: table)
---@field destroyDisplayObject fun(self: Core, name: string)
---@field countOrphanedDisplayObjects fun(self: Core): integer
---@field getOrphanedDisplayObjects fun(self: Core): DisplayHandle[]
---@field collectGarbage fun(self: Core)
---@field listDisplayObjectNames fun(self: Core): string[]
---@field printObjectRegistry fun(self: Core)

---@class Stage: DisplayHandle
---@field getMouseX fun(self: Stage): integer
---@field getMouseY fun(self: Stage): integer
---@field setMouseX fun(self: Stage, x: integer)
---@field setMouseY fun(self: Stage, y: integer)

---@type Core
Core = Core or {}

---@type UnitTests
UnitTests = UnitTests or {}

---@type Factory
Factory = Factory or {}

IconIndex = IconIndex or {}
ArrowDirection = ArrowDirection or {}
EventType = EventType or {}

-- Optional editor grouping: category-specific pseudo-classes inheriting DisplayHandle.
-- These duplicate a subset of fields to help IDEs show grouped docs while the
-- main DisplayHandle block above remains the single source for analyzers.

---@class DisplayHandle_Identity: DisplayHandle
---@field isValid fun(self: DisplayHandle): boolean
---@field get fun(self: DisplayHandle): any
---@field getName fun(self: DisplayHandle): string
---@field getType fun(self: DisplayHandle): string
---@field setName fun(self: DisplayHandle, name: string)
---@field setType fun(self: DisplayHandle, typeName: string)

---@class DisplayHandle_State: DisplayHandle
---@field cleanAll fun(self: DisplayHandle)
---@field getDirty fun(self: DisplayHandle): boolean
---@field setDirty fun(self: DisplayHandle)
---@field isDirty fun(self: DisplayHandle): boolean

---@class DisplayHandle_Debug: DisplayHandle
---@field printTree fun(self: DisplayHandle)

---@class DisplayHandle_Events: DisplayHandle
---@field addEventListener fun(self: DisplayHandle, spec: table)
---@field addEventListener fun(self: DisplayHandle, type: (EventType|string), listener: fun(e: any), useCapture?: boolean, priority?: integer)
---@field removeEventListener fun(self: DisplayHandle, spec: table)
---@field removeEventListener fun(self: DisplayHandle, type: (EventType|string), listener: fun(e: any), useCapture?: boolean)
---@field hasEventListener fun(self: DisplayHandle, type: EventType, useCapture?: boolean): boolean
---@field queue_event fun(self: DisplayHandle, type: EventType, init?: fun(e: any))

---@class DisplayHandle_Hierarchy: DisplayHandle
---@field addChild fun(self: DisplayHandle, child: (DisplayHandle|string|table)): boolean
---@field removeChild fun(self: DisplayHandle, child: (DisplayHandle|string|table)): boolean
---@field hasChild fun(self: DisplayHandle, child: (DisplayHandle|string|table)): boolean
---@field getChild fun(self: DisplayHandle, spec: (string|table)): (DisplayHandle|nil)
---@field getChildren fun(self: DisplayHandle): DisplayHandle[]
---@field countChildren fun(self: DisplayHandle): integer
---@field getParent fun(self: DisplayHandle): (DisplayHandle|nil)
---@field setParent fun(self: DisplayHandle, parent: DisplayHandle)
---@field isAncestorOf fun(self: DisplayHandle, child: (DisplayHandle|string)): boolean
---@field isDescendantOf fun(self: DisplayHandle, ancestor: (DisplayHandle|string)): boolean
---@field removeFromParent fun(self: DisplayHandle): boolean
---@field removeDescendant fun(self: DisplayHandle, target: (DisplayHandle|string)): boolean

---@class DisplayHandle_BoundsColors: DisplayHandle
---@field getBounds fun(self: DisplayHandle): Bounds
---@field setBounds fun(self: DisplayHandle, b: Bounds|table)
---@field getColor fun(self: DisplayHandle): SDL_Color
---@field setColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field getForegroundColor fun(self: DisplayHandle): SDL_Color
---@field setForegroundColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field getBackgroundColor fun(self: DisplayHandle): SDL_Color
---@field setBackgroundColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field getBorderColor fun(self: DisplayHandle): SDL_Color
---@field setBorderColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field getOutlineColor fun(self: DisplayHandle): SDL_Color
---@field setOutlineColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field getDropshadowColor fun(self: DisplayHandle): SDL_Color
---@field setDropshadowColor fun(self: DisplayHandle, c: SDL_Color|table)
---@field hasBorder fun(self: DisplayHandle): boolean
---@field hasBackground fun(self: DisplayHandle): boolean
---@field setBorder fun(self: DisplayHandle, enabled: boolean)
---@field setBackground fun(self: DisplayHandle, enabled: boolean)

---@class DisplayHandle_PriorityZ: DisplayHandle
---@field getMaxPriority fun(self: DisplayHandle): integer
---@field getMinPriority fun(self: DisplayHandle): integer
---@field getPriority fun(self: DisplayHandle): integer
---@field setPriority fun(self: DisplayHandle, value: integer)
---@field setPriority fun(self: DisplayHandle, spec: table)
---@field setPriority fun(self: DisplayHandle, spec: (DisplayHandle|string|table), value: integer)
---@field sortChildrenByPriority fun(self: DisplayHandle)
---@field setToHighestPriority fun(self: DisplayHandle)
---@field setToHighestPriority fun(self: DisplayHandle, spec: (DisplayHandle|string|table))
---@field setToLowestPriority fun(self: DisplayHandle)
---@field setToLowestPriority fun(self: DisplayHandle, spec: (DisplayHandle|string|table))
---@field moveToTop fun(self: DisplayHandle)
---@field moveToTop fun(self: DisplayHandle, spec: (DisplayHandle|string|table))
---@field moveToBottom fun(self: DisplayHandle)
---@field moveToBottom fun(self: DisplayHandle, spec: table)
---@field bringToFront fun(self: DisplayHandle)
---@field bringToFront fun(self: DisplayHandle, spec: table)
---@field sendToBack fun(self: DisplayHandle)
---@field sendToBack fun(self: DisplayHandle, spec: table)
---@field sendToBackAfter fun(self: DisplayHandle, limit: DisplayHandle)
---@field sendToBackAfter fun(self: DisplayHandle, spec: (DisplayHandle|string|table), limit: DisplayHandle)
---@field getZOrder fun(self: DisplayHandle): integer
---@field setZOrder fun(self: DisplayHandle, z: integer)
---@field setZOrder fun(self: DisplayHandle, spec: table)

---@class DisplayHandle_Focus: DisplayHandle
---@field setKeyboardFocus fun(self: DisplayHandle)
---@field isKeyboardFocused fun(self: DisplayHandle): boolean
---@field isMouseHovered fun(self: DisplayHandle): boolean
---@field isClickable fun(self: DisplayHandle): boolean
---@field setClickable fun(self: DisplayHandle, enabled: boolean)
---@field isEnabled fun(self: DisplayHandle): boolean
---@field setEnabled fun(self: DisplayHandle, enabled: boolean)
---@field isHidden fun(self: DisplayHandle): boolean
---@field setHidden fun(self: DisplayHandle, hidden: boolean)
---@field isVisible fun(self: DisplayHandle): boolean
---@field setVisible fun(self: DisplayHandle, visible: boolean)

---@class DisplayHandle_Tabs: DisplayHandle
---@field getTabPriority fun(self: DisplayHandle): integer
---@field setTabPriority fun(self: DisplayHandle, index: integer)
---@field isTabEnabled fun(self: DisplayHandle): boolean
---@field setTabEnabled fun(self: DisplayHandle, enabled: boolean)

---@class DisplayHandle_Geometry: DisplayHandle
---@field getX fun(self: DisplayHandle): integer
---@field getY fun(self: DisplayHandle): integer
---@field getWidth fun(self: DisplayHandle): integer
---@field getHeight fun(self: DisplayHandle): integer
---@field setX fun(self: DisplayHandle, x: integer)
---@field setY fun(self: DisplayHandle, y: integer)
---@field setWidth fun(self: DisplayHandle, w: integer)
---@field setHeight fun(self: DisplayHandle, h: integer)

---@class DisplayHandle_Anchors: DisplayHandle
---@field getAnchorTop fun(self: DisplayHandle): integer
---@field getAnchorLeft fun(self: DisplayHandle): integer
---@field getAnchorBottom fun(self: DisplayHandle): integer
---@field getAnchorRight fun(self: DisplayHandle): integer
---@field setAnchorTop fun(self: DisplayHandle, ap: integer)
---@field setAnchorLeft fun(self: DisplayHandle, ap: integer)
---@field setAnchorBottom fun(self: DisplayHandle, ap: integer)
---@field setAnchorRight fun(self: DisplayHandle, ap: integer)

---@class DisplayHandle_WorldEdges: DisplayHandle
---@field getLeft fun(self: DisplayHandle): number
---@field getRight fun(self: DisplayHandle): number
---@field getTop fun(self: DisplayHandle): number
---@field getBottom fun(self: DisplayHandle): number
---@field setLeft fun(self: DisplayHandle, v: number)
---@field setRight fun(self: DisplayHandle, v: number)
---@field setTop fun(self: DisplayHandle, v: number)
---@field setBottom fun(self: DisplayHandle, v: number)

---@class DisplayHandle_LocalEdges: DisplayHandle
---@field getLocalLeft fun(self: DisplayHandle): number
---@field getLocalRight fun(self: DisplayHandle): number
---@field getLocalTop fun(self: DisplayHandle): number
---@field getLocalBottom fun(self: DisplayHandle): number
---@field setLocalLeft fun(self: DisplayHandle, v: number)
---@field setLocalRight fun(self: DisplayHandle, v: number)
---@field setLocalTop fun(self: DisplayHandle, v: number)
---@field setLocalBottom fun(self: DisplayHandle, v: number)

---@class DisplayHandle_Orphan: DisplayHandle
---@field orphanPolicyFromString fun(self: DisplayHandle, s: string): integer
---@field orphanPolicyToString fun(self: DisplayHandle, policy: integer): string
---@field setOrphanRetentionPolicy fun(self: DisplayHandle, s: string)
---@field getOrphanRetentionPolicyString fun(self: DisplayHandle): string
---@field getOrphanGrace fun(self: DisplayHandle): integer
---@field setOrphanGrace fun(self: DisplayHandle, ms: integer)

---@class DisplayHandle_Range: DisplayHandle
---@field value number
---@field min number
---@field max number

return true
