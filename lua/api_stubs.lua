--@ -1,73 +0,0 @@
-- lua/api_stubs.lua
-- Comprehensive editor-only Lua API stubs for SDOM bindings.
-- Designed for EmmyLua / LuaLS (sumneko) to provide completions and suppress
-- undefined-global diagnostics in IDEs. This file is NOT loaded at runtime.

---@diagnostic disable: undefined-global
---@diagnostic disable: duplicate-set-field

-- DisplayHandle ------------------------------------------------------------
---@class DisplayHandle
---@field isValid fun(self): boolean
---@field getName fun(self): string
---@field getType fun(self): string
---@field setName fun(self, name: string): nil
---@field setType fun(self, typeName: string): nil
---@field get fun(self): any
---@field getX fun(self): integer
---@field getY fun(self): integer
---@field getWidth fun(self): integer
---@field getHeight fun(self): integer
---@field addChild fun(self, child: (DisplayHandle|string|table)): boolean
---@field removeChild fun(self, child: (DisplayHandle|string|table)): boolean
---@field hasChild fun(self, child: (DisplayHandle|string|table)): boolean
---@field getChild fun(self, spec: (string|table)): (DisplayHandle|nil)

-- AssetHandle ----------------------------------------------------------------
---@class AssetHandle
---@field isValid fun(self): boolean
---@field getName fun(self): string
---@field getType fun(self): string
---@field getFilename fun(self): string

-- EventType (partial) -------------------------------------------------------
---@class EventType
---@field name string

-- UnitTests helper ----------------------------------------------------------
---@class UnitTests
---@field push_error fun(msg: string): nil
---@field get_results fun(): table

-- Factory (partial) ---------------------------------------------------------
---@class Factory
---@field createDisplayObject fun(self, typeName: string, config: table): DisplayHandle
---@field createAssetObject fun(self, typeName: string, config: table): AssetHandle

-- Core singleton (partial) --------------------------------------------------
---@class Core
---@field getStage fun(self): DisplayHandle
---@field setStage fun(self, DisplayHandle): nil
---@field setRootNode fun(self, name: string): nil
---@field getStageHandle fun(self): DisplayHandle
---@field getRoot fun(self): DisplayHandle
---@field getRootNode fun(self): DisplayHandle
---@field configure fun(self, cfg: table): nil
---@field configureFromFile fun(self, filename: string): nil
---@field registerOn fun(self, name: string, fn: fun(...): any): nil
---@field createDisplayObject fun(self, typeName: string, cfg: table): DisplayHandle
---@field countOrphanedDisplayObjects fun(self): integer
---@field getDisplayObject fun(self, name: string): DisplayHandle
---@field hasDisplayObject fun(self, name: string): boolean
---@field createAssetObject fun(self, typeName: string, cfg: table): AssetHandle
---@field collectGarbage fun(self): nil
---@field getIsTraversing fun(self): boolean
---@field setIsTraversing fun(self, traversing: boolean): nil
---@field getAssetObject fun(self, name: string): AssetHandle
---@field pumpEventsOnce fun(self): nil
---@field pushMouseEvent fun(self, args: table): nil
---@field pushKeyboardEvent fun(self, args: table): nil

-- IDisplayObject (partial) -------------------------------------------------
---@class IDisplayObject
---@field addChild fun(self, child: (DisplayHandle|string|table)): boolean
---@field removeChild fun(self, child: (DisplayHandle|string|table)): boolean
---@field hasChild fun(self, child: (DisplayHandle|string|table)): boolean
---@field getChild fun(self, spec: (string|table)): (DisplayHandle|nil)

-- Stage (partial) -----------------------------------------------------------
---@class Stage: IDisplayObject
---@field getMouseX fun(self): integer
---@field getMouseY fun(self): integer
---@field setMouseX fun(self, x: integer): nil
---@field setMouseY fun(self, y: integer): nil



-- Global top-level convenience functions (provided by bindings)
---@type Core
Core = Core or {}

---@type UnitTests
UnitTests = UnitTests or {}

---@type Factory
Factory = Factory or {}

-- Optional: expose named constants/tables used by scripts
IconIndex = IconIndex or {}
ArrowDirection = ArrowDirection or {}
EventType = EventType or {}

return true
