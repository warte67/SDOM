-- ui_symbols.lua
-- Common Unicode UI symbols for use in SDOM Lua scripts.

local UI = {}

-- Arrows
UI.ARROW_UP           = "↑"
UI.ARROW_DOWN         = "↓"
UI.ARROW_LEFT         = "←"
UI.ARROW_RIGHT        = "→"
UI.ARROW_UP_DOUBLE    = "⇑"
UI.ARROW_DOWN_DOUBLE  = "⇓"
UI.ARROW_LEFT_DOUBLE  = "⇐"
UI.ARROW_RIGHT_DOUBLE = "⇒"
UI.ARROW_THIN_RIGHT   = "▸"
UI.ARROW_COLLAPSED    = "▶"
UI.ARROW_EXPANDED     = "▼"

-- Checkbox / Toggle States
UI.CHECKBOX_EMPTY     = "☐"
UI.CHECKBOX_CHECKED   = "☑"
UI.CHECKBOX_CROSSED   = "☒"
UI.CHECKBOX_MIXED     = "◪"
UI.CHECKBOX_FLAT      = "▬"

-- Radio Buttons
UI.RADIO_OFF          = "○"
UI.RADIO_ON           = "●"
UI.RADIO_DISABLED     = "◎"

-- Slider / Scrollbar Components
UI.TRACK_H            = "─"
UI.TRACK_V            = "│"
UI.KNOB_BLOCK         = "■"
UI.KNOB_ROUND         = "●"
UI.ARROW_UP_SMALL     = "▲"
UI.ARROW_DOWN_SMALL   = "▼"
UI.ARROW_LEFT_SMALL   = "◄"
UI.ARROW_RIGHT_SMALL  = "►"

-- Progress Shading
UI.SHADE_LIGHT        = "░"
UI.SHADE_MEDIUM       = "▒"
UI.SHADE_DARK         = "▓"
UI.SHADE_SOLID        = "█"

-- Box Drawing (frames / layout logging)
UI.BOX_TL             = "┌"
UI.BOX_TR             = "┐"
UI.BOX_BL             = "└"
UI.BOX_BR             = "┘"
UI.BOX_H              = "─"
UI.BOX_V              = "│"
UI.BOX_T              = "┬"
UI.BOX_B              = "┴"
UI.BOX_L              = "├"
UI.BOX_R              = "┤"
UI.BOX_C              = "┼"

return UI
