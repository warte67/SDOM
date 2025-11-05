--- lua/api_stubs.lua
--- Editor-only Lua API stubs for SDOM bindings (for LuaLS/EmmyLua).
--- Not loaded at runtime. Keep in sync with Core and IDisplayObject binders.
---@meta
---@diagnostic disable: undefined-global, duplicate-set-field, need-check-nil

-- (Global disables also applied above via @meta header.)

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
---@enum ArrowDirection
ArrowDirection = {
    Left = 0,
    Right = 1,
    Up = 2,
    Down = 3
}

---@enum ArrowState
ArrowState = {
    Raised = 0,
    Depressed = 1
}

---@alias ArrowDirectionString
---| 'left'
---| 'right'
---| 'up'
---| 'down'

---@alias ArrowStateString
---| 'raised'
---| 'depressed'


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
---@field getChild fun(self: DisplayHandle, spec: (string|table)): DisplayHandle
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
-- SpriteSheet-specific helpers (attached to AssetHandle for SpriteSheet assets)
---@field setSpriteWidth fun(self: AssetHandle, width: integer)
---@field setSpriteHeight fun(self: AssetHandle, height: integer)
---@field setSpriteSize fun(self: AssetHandle, width: integer, height: integer)
---@field getSpriteWidth fun(self: AssetHandle): integer
---@field getSpriteHeight fun(self: AssetHandle): integer
---@field getSpriteSize fun(self: AssetHandle): table
---@field getSpriteCount fun(self: AssetHandle): integer
---@field getSpriteX fun(self: AssetHandle, spriteIndex: integer): integer
---@field getSpriteY fun(self: AssetHandle, spriteIndex: integer): integer
---@field drawSprite fun(self: AssetHandle, spriteIndex: integer, x: integer, y: integer, color?: (SDL_Color|table), scaleMode?: string)
---@field drawSprite fun(self: AssetHandle, spriteIndex: integer, dstRect: table, color?: (SDL_Color|table), scaleMode?: string)
---@field drawSprite fun(self: AssetHandle, spriteIndex: integer, srcRect: table, dstRect: table, color?: (SDL_Color|table), scaleMode?: string)
---@field drawSprite_dst fun(self: AssetHandle, spriteIndex: integer, dstRect: table, color?: (SDL_Color|table), scaleMode?: string)
---@field drawSprite_ext fun(self: AssetHandle, spriteIndex: integer, srcRect: table, dstRect: table, color?: (SDL_Color|table), scaleMode?: string)

---@class Core
---@field quit fun(self: Core)
---@field shutdown fun(self: Core)
---@field configure fun(self: Core, cfg: table)
---@field configureFromFile fun(self: Core, filename: string)
---@field registerOn fun(self: Core, name: "Init"|"Quit"|"Update"|"Event"|"Render"|"UnitTest"|"WindowResize", fn: function)
---@field setRootNodeByName fun(self: Core, name: string)
---@field setRootNode fun(self: Core, nameOrHandle: (string|DisplayHandle))
---@field setRoot fun(self: Core, nameOrHandle: (string|DisplayHandle))
---@field setStageByName fun(self: Core, name: string)
---@field setStage fun(self: Core, nameOrHandle: (string|DisplayHandle))
---@field getRoot fun(self: Core): DisplayHandle
---@field getRootHandle fun(self: Core): DisplayHandle
---@field getStage fun(self: Core): DisplayHandle
---@field getStageHandle fun(self: Core): DisplayHandle
---@field getIsTraversing fun(self: Core): boolean
---@field setIsTraversing fun(self: Core, traversing: boolean)
---@field createDisplayObject fun(self: Core, typeName: string, cfg: table): DisplayHandle
---@field getDisplayObject fun(self: Core, name: string): DisplayHandle
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
---@field clearKeyboardFocusedObject fun(self: Core)
---@field setMouseHoveredObject fun(self: Core, handle: DisplayHandle)
---@field clearMouseHoveredObject fun(self: Core)
---@field getMouseHoveredObject fun(self: Core): DisplayHandle
---@field getWindowTitle fun(self: Core): string
---@field setWindowTitle fun(self: Core, title: string)
---@field getPixelWidth fun(self: Core): number
---@field getPixelHeight fun(self: Core): number
---@field getPixelFormat fun(self: Core): integer
---@field getWindowFlags fun(self: Core): integer
---@field getWindowWidth fun(self: Core): number
---@field getWindowHeight fun(self: Core): number
---@field getPreserveAspectRatio fun(self: Core): boolean
---@field getAllowTextureResize fun(self: Core): boolean
---@field getRendererLogicalPresentation fun(self: Core): integer
---@field setPixelWidth fun(self: Core, width: number)
---@field setPixelHeight fun(self: Core, height: number)
---@field setPixelFormat fun(self: Core, format: integer)
---@field setWindowFlags fun(self: Core, flags: integer)
---@field setWindowWidth fun(self: Core, width: number)
---@field setWindowHeight fun(self: Core, height: number)
---@field setPreserveAspectRatio fun(self: Core, preserve: boolean)
---@field setAllowTextureResize fun(self: Core, allow: boolean)
---@field setRendererLogicalPresentation fun(self: Core, presentation: integer)
---@field getFrameCount fun(self: Core): integer
---@field getElapsedTime fun(self: Core): number
---@field getDeltaTime fun(self: Core): number
---@field pumpEventsOnce fun(self: Core)
---@field pushMouseEvent fun(self: Core, args: table)
---@field pushKeyboardEvent fun(self: Core, args: table)
---@field destroyDisplayObject fun(self: Core, target: (string|DisplayHandle|table))
---@field destroyAssetObject fun(self: Core, target: (string|DisplayHandle|table))
---@field countOrphanedDisplayObjects fun(self: Core): integer
---@field getOrphanedDisplayObjects fun(self: Core): DisplayHandle[]
---@field collectGarbage fun(self: Core)
---@field clearFactory fun(self: Core)
---@field findAssetByFilename fun(self: Core, filename: string): AssetHandle
---@field findSpriteSheetByParams fun(self: Core, spec: { filename: string, spriteW: integer, spriteH: integer }): AssetHandle
---@field unloadAllAssetObjects fun(self: Core)
---@field reloadAllAssetObjects fun(self: Core)
---@field getDisplayObjectNames fun(self: Core): string[]
---@field printObjectRegistry fun(self: Core)

---@class Stage: DisplayHandle
---@field getMouseX fun(self: Stage): integer
---@field getMouseY fun(self: Stage): integer
---@field setMouseX fun(self: Stage, x: integer)
---@field setMouseY fun(self: Stage, y: integer)

---@type Core
Core = Core or {}

-- SDL_Utils ---------------------------------------------------------------
---@class SDL_FRect
---@field x number
---@field y number
---@field w number
---@field h number

---@class SDL_Utils
SDL_Utils = SDL_Utils or {}

---@param format integer
---@return string
function SDL_Utils.pixelFormatToString(format) end

---@param name string
---@return integer
function SDL_Utils.pixelFormatFromString(name) end

---@param flags integer
---@return string
function SDL_Utils.windowFlagsToString(flags) end

---@param name string
---@return integer
function SDL_Utils.windowFlagsFromString(name) end

---@param flags integer
---@return string
function SDL_Utils.rendererLogicalPresentationToString(flags) end

---@param name string
---@return integer
function SDL_Utils.rendererLogicalPresentationFromString(name) end

---@param keycode integer
---@param keymod integer
---@return integer
function SDL_Utils.keyToAscii(keycode, keymod) end

---@param ms integer
function SDL_Utils.Delay(ms) end

---@param ev any
---@return table
function SDL_Utils.eventToLuaTable(ev) end

---@param t table
---@return SDL_FRect
function SDL_Utils.tableToFRect(t) end

---@param o any
---@return SDL_Color
function SDL_Utils.colorFromSol(o) end

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

---@class DisplayHandle
--- Synthetic Lua property (via __index/__newindex binding)
---@field is_hidden boolean     -- mirrors isHidden()/setHidden()
---@field x number              -- mirrors getX()/setX()
---@field y number              -- mirrors getY()/setY()
---@field width number          -- mirrors getWidth()/setWidth()
---@field height number         -- mirrors getHeight()/setHeight()
---@field w number              -- alias of width
---@field h number              -- alias of height
---@field color SDL_Color       -- mirrors getColor()/setColor()
---@field foreground_color SDL_Color    -- mirrors getForegroundColor()/setForegroundColor()
---@field background_color SDL_Color    -- mirrors getBackgroundColor()/setBackgroundColor()
---@field border_color SDL_Color        -- mirrors getBorderColor()/setBorderColor()
---@field outline_color SDL_Color       -- mirrors getOutlineColor()/setOutlineColor()
---@field dropshadow_color SDL_Color    -- mirrors getDropshadowColor()/setDropshadowColor()
---@field anchor_top integer    -- mirrors getAnchorTop()/setAnchorTop()
---@field anchor_left integer   -- mirrors getAnchorLeft()/setAnchorLeft()
---@field anchor_bottom integer -- mirrors getAnchorBottom()/setAnchorBottom()
---@field anchor_right integer  -- mirrors getAnchorRight()/setAnchorRight()
---@field z_order integer       -- mirrors getZOrder()/setZOrder()
---@field priority integer      -- mirrors getPriority()/setPriority()
---@field is_clickable boolean  -- mirrors isClickable()/setClickable()
---@field is_enabled boolean    -- mirrors isEnabled()/setEnabled()
---@field tab_priority integer  -- mirrors getTabPriority()/setTabPriority()
---@field tab_enabled boolean   -- mirrors isTabEnabled()/setTabEnabled()
---@field left number           -- mirrors getLeft()/setLeft()
---@field right number          -- mirrors getRight()/setRight()
---@field top number            -- mirrors getTop()/setTop()
---@field bottom number         -- mirrors getBottom()/setBottom()
---@field local_left number     -- mirrors getLocalLeft()/setLocalLeft()
---@field local_right number    -- mirrors getLocalRight()/setLocalRight()
---@field local_top number      -- mirrors getLocalTop()/setLocalTop()
---@field local_bottom number   -- mirrors getLocalBottom()/setLocalBottom()
---@field orphan_grace number   -- mirrors getOrphanGrace()/setOrphanGrace()
---@field orphan_policy string  -- mirrors getOrphanRetentionPolicyString()/setOrphanRetentionPolicy()

---@class ArrowButton : DisplayHandle
---Represents a directional button derived from IconButton.
---Provides direction and arrow-state controls.
---@field direction ArrowDirectionString|integer  -- 'up'|'down'|'left'|'right'
---@field getDirection fun(self: ArrowButton): ArrowDirectionString|integer
---@field setDirection fun(self: ArrowButton, dir: (ArrowDirectionString|integer))
---@field getArrowState fun(self: ArrowButton): ArrowStateString|integer
---@field getIconIndex fun(self: ArrowButton): integer

---@param typeName "ArrowButton"
---@param cfg table
---@return ArrowButton
function Core:createDisplayObject(typeName, cfg) end

---Creates a new SDL_Color
---@param r integer|nil
---@param g integer|nil
---@param b integer|nil
---@param a integer|nil
---@return SDL_Color
function SDL_Color:new(r, g, b, a) end

---Builds an SDL_Color from a Lua table `{r=,g=,b=,a=}` or `{r,g,b,a}`
---@param t table
---@return SDL_Color
function SDL_Color.fromTable(t) end

function SDL_Color:getR() end
function SDL_Color:getG() end
function SDL_Color:getB() end
function SDL_Color:getA() end

-- Export
_G.SDL_Color = SDL_Color

---@class SDL_Utils
local SDL_Utils = {}

---Converts an SDL_PixelFormat to a string.
---@param fmt integer
---@return string
function SDL_Utils.pixelFormatToString(fmt) end
---@param self SDL_Utils
---@param fmt integer
---@return string
function SDL_Utils:pixelFormatToString(fmt) end

---Parses a string into an SDL_PixelFormat.
---@param str string
---@return integer
function SDL_Utils.pixelFormatFromString(str) end
---@param self SDL_Utils
---@param str string
---@return integer
function SDL_Utils:pixelFormatFromString(str) end

---Converts window flags to string.
---@param flags integer
---@return string
function SDL_Utils.windowFlagsToString(flags) end
---@param self SDL_Utils
---@param flags integer
---@return string
function SDL_Utils:windowFlagsToString(flags) end

---Parses a string into window flags.
---@param str string
---@return integer
function SDL_Utils.windowFlagsFromString(str) end
---@param self SDL_Utils
---@param str string
---@return integer
function SDL_Utils:windowFlagsFromString(str) end

---Converts renderer presentation mode to string.
---@param val integer
---@return string
function SDL_Utils.rendererLogicalPresentationToString(val) end
---@param self SDL_Utils
---@param val integer
---@return string
function SDL_Utils:rendererLogicalPresentationToString(val) end

---Parses a string into renderer presentation mode.
---@param str string
---@return integer
function SDL_Utils.rendererLogicalPresentationFromString(str) end
---@param self SDL_Utils
---@param str string
---@return integer
function SDL_Utils:rendererLogicalPresentationFromString(str) end

---Converts an SDL_EventType to a string.
---@param event_type integer
---@return string
function SDL_Utils.eventTypeToString(event_type) end
---@param self SDL_Utils
---@param event_type integer
---@return string
function SDL_Utils:eventTypeToString(event_type) end

---Parses a string into an SDL_EventType.
---@param str string
---@return integer
function SDL_Utils.eventTypeFromString(str) end
---@param self SDL_Utils
---@param str string
---@return integer
function SDL_Utils:eventTypeFromString(str) end

---Converts an SDL_Keycode + SDL_Keymod into ASCII.
---@param keycode integer
---@param keymod integer
---@return string|nil
function SDL_Utils.keyToAscii(keycode, keymod) end
---@param self SDL_Utils
---@param keycode integer
---@param keymod integer
---@return string|nil
function SDL_Utils:keyToAscii(keycode, keymod) end

---Sleeps for the given number of milliseconds.
---@param ms integer
function SDL_Utils.delay(ms) end
---@param self integer
---@param ms integer
function SDL_Utils:delay(ms) end

---Converts an SDL_Event to a Lua table.
---@param ev any
---@return table|nil
function SDL_Utils.eventToLuaTable(ev) end
---@param self SDL_Utils
---@param ev any
---@return table|nil
function SDL_Utils:eventToLuaTable(ev) end



-- Export both names: SDL_Utils and SDL
_G.SDL_Utils = SDL_Utils
_G.SDL = SDL_Utils

return true
