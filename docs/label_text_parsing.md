# Label Text Parsing Design Document

## Overview
The Label system in the SDOM API is a lightweight, scriptable text rendering component designed for both retro-style bitmap fonts and modern truetype rendering. It tokenizes input into word and escape tokens so inline formatting (color, bold, outline, padding, dropshadow, alignment, numeric style tweaks) can be applied on a per-token basis. Escape sequences use case‑insensitive square‑bracket tags (e.g., [bold], [color=#RRGGBB], [pad=5x7]) and support nested scopes and explicit resets so authors can compose rich inline styles without separate layout passes.

Rendering is separated into tokenization, layout (wrapping, alignment grouping, autosizing), and phrase rendering phases. Each token carries a LabelStyle/FontStyle snapshot (including numeric fields like borderThickness and dropshadow offsets) so grouping into phrases lets the engine render visually identical runs efficiently. Alignment is encoded compactly (nine alignment modes) and tokens are sorted into the appropriate buckets for deterministic placement and multi-line wrapping behavior.

The Label supports autosizing, clipping to parent bounds, and both per-token and per-phrase style effects. It exposes token information to Lua (e.g., via DisplayHandle:getTokenList()), enabling unit tests and scripts to inspect tokenized output and verify style propagation. The implementation is forgiving: malformed escape parameters are validated and ignored (logged) rather than throwing, which improves robustness for live content authored in Lua.

Finally, the system is designed to be extensible: new escapes can be added with minimal tokenizer changes, and the rendering pipeline cleanly separates font-specific handling (bitmap vs TTF) from token management and layout logic. This makes the Label suitable for game UI text, HUD displays, and retro-styled interfaces while remaining script-friendly and testable.

---

## Goals
- Support word wrapping and autosizing for labels.
- Allow inline style changes (color, bold, underline, etc.) via escape sequences.
- Enable mixed horizontal and vertical alignment within a single label.
- Efficiently tokenize and process text for rendering.

---

## Retro-Inspired Graphic Glyphs

The graphic glyphs in the `internal_font_8x8` font are designed with a retro aesthetic, reminiscent of the character sets used in vintage computers like the Tandy systems. These glyphs provide a nostalgic yet functional approach to creating visual elements in text-based environments. The four sets of graphic glyphs—line-based, solid-on-clear, dithered-on-clear, and solid-on-dithered—offer a versatile toolkit for rendering simple graphics, icons, and patterns.

Note: This needs to be updated to use the new font_8x8 and icon_8x8 (or icon_10x10) sets.

### Customization Potential

While the space used for these retro-styled graphic glyphs could have been allocated for special characters like Greek math symbols, the nostalgic appeal of these glyphs felt more fitting. Additionally, the design allows for end-user customization, enabling users to redefine or expand the glyph set to suit their specific needs.

## Escape Sequence Syntax
The Label escape sequence syntax uses square-bracket tokens (e.g., `[bold]`, `[color=red]`, `[size=12]`) to change rendering state inline. Escape tags are case‑insensitive and come in two forms: simple toggles (`[bold]`, `[reset]`) and parameterized tags (`[color=#RRGGBB]`, `[pad=5x7]`, `[dropshadow=4,6]`). Tags are not rendered as text; instead they update the current FontStyle/LabelStyle that will be applied to subsequently emitted word tokens. Numeric and hex parameters are validated before use; malformed parameters are ignored and do not throw. Use double‑bracket escapes (`[[]` and `[]]`) to emit literal `[` and `]` characters.

Escape sequences are tokenized separately from word tokens so the parser can push and pop style state. Opening tags apply a change that remains in effect until a matching closing tag (e.g., `[/pad]`) or until a `[reset]` is encountered; nested tags are supported and restored in LIFO order. Special parsing rules: escapes inside double-quoted blocks are treated as literal text, bracket escapes are single tokens, and numeric escapes affect specific style fields (borderThickness, outlineThickness, padding_horiz/vert, dropshadowOffsetX/Y). For predictable results prefer explicit parameters and well-formed tags; the tokenizer logs but tolerates malformed or unsupported tags to avoid breaking rendering.// filepath: /home/jay/Documents/GitHub/SDOM/docs/label_text_parsing.md
The Label escape sequence syntax uses square-bracket tokens (e.g., `[bold]`, `[color=red]`, `[size=12]`) to change rendering state inline. Escape tags are case‑insensitive and come in two forms: simple toggles (`[bold]`, `[reset]`) and parameterized tags (`[color=#RRGGBB]`, `[pad=5x7]`, `[dropshadow=4,6]`). Tags are not rendered as text; instead they update the current FontStyle/LabelStyle that will be applied to subsequently emitted word tokens. Numeric and hex parameters are validated before use; malformed parameters are ignored and do not throw. Use double‑bracket escapes (`[[]` and `[]]`) to emit literal `[` and `]` characters.

Escape sequences are tokenized separately from word tokens so the parser can push and pop style state. Opening tags apply a change that remains in effect until a matching closing tag (e.g., `[/pad]`) or until a `[reset]` is encountered; nested tags are supported and restored in LIFO order. Special parsing rules: escapes inside double-quoted blocks are treated as literal text, bracket escapes are single tokens, and numeric escapes affect specific style fields (borderThickness, outlineThickness, padding_horiz/vert, dropshadowOffsetX/Y). For predictable results prefer explicit parameters and well-formed tags; the tokenizer logs but tolerates malformed or unsupported tags to avoid breaking rendering.

---

### Supported Color Names

You can use any of the following color names in label escape tags (e.g., `[color=name]`, `[fgnd=name]`, `[bgnd=name]`, `[border=name]`, `[outline=name]`, `[shadow=name]`):

**Grayscale gradient (6 steps):**
- `black`      — {   0,   0,   0, 255 }
- `dk_gray`    — {  51,  51,  51, 255 }
- `md_gray`    — { 102, 102, 102, 255 }
- `gray`       — { 153, 153, 153, 255 }
- `lt_gray`    — { 204, 204, 204, 255 }
- `white`      — { 255, 255, 255, 255 }

**Light colors:**
- `lt_red`     — { 255,  32,  32, 255 }
- `lt_green`   — {  32, 255,  32, 255 }
- `lt_yellow`  — { 255, 255,  32, 255 }
- `lt_blue`    — {  32,  32, 255, 255 }
- `lt_magenta` — { 255,  32, 255, 255 }
- `lt_cyan`    — {  32, 255, 255, 255 }

**Normal colors:**
- `red`        — { 224,   0,   0, 255 }
- `green`      — {   0, 224,   0, 255 }
- `yellow`     — { 224, 224,   0, 255 }
- `blue`       — {   0,   0, 224, 255 }
- `magenta`    — { 224,   0, 224, 255 }
- `cyan`       — {   0, 224, 224, 255 }

**Dark colors:**
- `dk_red`     — { 192,   0,   0, 255 }
- `dk_green`   — {   0, 192,   0, 255 }
- `dk_yellow`  — { 192, 192,   0, 255 }
- `dk_blue`    — {   0,   0, 192, 255 }
- `dk_magenta` — { 192,   0, 192, 255 }
- `dk_cyan`    — {   0, 192, 192, 255 }

**New Colors:**
- `tan`         — { 255, 204, 153, 255 }
- `orange`      — { 255, 128,   0, 255 }
- `brown`       — { 128,  96,   0, 255 }
- `pink`        — { 255, 128, 192, 255 }
- `purple`      — { 128,   0, 128, 255 }
- `violet`      — { 165,  42, 255, 255 }
- `lavender`    — { 230, 230, 250, 255 }
- `gold`        — { 255, 215,   0, 255 }
- `silver`      — { 192, 192, 192, 255 }
- `bronze`      — { 205, 127,  50, 255 }
- `lime`        — { 128, 255,   0, 255 }
- `mint`        — { 170, 255, 195, 255 }
- `dk_olive`    — {  64,  64,   0, 255 }
- `olive`       — {  96,  96,   0, 255 }
- `olive_drab`  — { 107, 142,  35, 255 }
- `sea_green`   — {  46, 139,  87, 255 }
- `forest_green`— {  34, 139,  34, 255 }
- `sky_blue`    — { 135, 206, 235, 255 }
- `royal_blue`  — {  65, 105, 225, 255 }
- `steel_blue`  — {  70, 130, 180, 255 }
- `midnight_blue` — {  25,  25, 112, 255 }
- `crimson`     — { 220,  20,  60, 255 }
- `maroon`      — { 128,   0,   0, 255 }
- `coral`       — { 255, 127,  80, 255 }
- `salmon`      — { 250, 128, 114, 255 }
- `khaki`       — { 240, 230, 140, 255 }
- `beige`       — { 245, 245, 220, 255 }
- `wheat`       — { 245, 222, 179, 255 }
- `chocolate`   — { 210, 105,  30, 255 }
- `sienna`      — { 160,  82,  45, 255 }
- `peru`        — { 205, 133,  63, 255 }
- `indigo`      — {  75,   0, 130, 255 }
- `turquoise`   — {  64, 224, 208, 255 }
- `aquamarine`  — { 127, 255, 212, 255 }
- `chartreuse`  — { 127, 255,   0, 255 }
- `teal`        — {   0, 128, 128, 255 }
- `navy`        — {   0,   0, 128, 255 }

**Custom Colors:**
- `[RGB=rrggbb]`   — Set text color to custom 6-digit hex RGB value (e.g., `[RGB=FF8800]`)
- `[RGBA=rrggbbaa]` — Set text color to custom 8-digit hex RGBA value (e.g., `[RGBA=FF8800FF]`)

**Note:**  
All color names are case-insensitive.  
You may use these names in any supported color escape tag.

#### Color Targets:
```
Label::foregroundColor     // "fgnd" (default)
Label::backgroundColor     // "bgnd"
Label::borderColor         // "border"
FontStyle::outlineColor    // "outline"
FontStyle::dropShadowColor // "shadow"
```
- `[color=yellow:fgnd]`   — sets the foreground to yellow
- `[color=red:bgnd]`      — sets the background to red
- `[color=green:border]`  — sets the border to green
- `[RGB:222222:outline]`  — sets the outline color to dark gray
- `[RGBA:0000007f:shadow]` — sets the drop shadow to transparent black

### Style

## Lua bindings: property-style vs legacy get/set functions

The Label Lua bindings expose most style fields in two symmetric forms so existing scripts and newer code can both work:

- Property-style (preferred): properties are readable and writable as plain Lua values/tables on the `DisplayHandle` returned from `createDisplayObject`.
- Legacy function-style (compat): getters/setters named `getXxx...` / `setXxx...` are also provided to preserve older code.

Common mappings (examples):

- Colors
  - Property-style: `h.foreground_color`, `h.background_color`, `h.border_color`, `h.outline_color`, `h.dropshadow_color` (tables with keys `r,g,b,a` or array-style `[r,g,b,a]`).
  - Function-style: `h:getForegroundColor()`, `h:setForegroundColor(tbl)` etc.

- Font flags and booleans
  - Property-style: `h.bold`, `h.italic`, `h.underline`, `h.strikethrough`, `h.outline`, `h.dropshadow`, `h.wordwrap`, `h.auto_resize` (booleans).
  - Function-style: `h:getBold()`, `h:setBold(true)` etc.

Example Lua (both styles):

```lua
local h = createDisplayObject("Label", { name = "lbl", type = "Label", resource_name = "internal_font_8x8" })

-- property-style read
local fg = h.foreground_color        -- table { r=..., g=..., b=..., a=... }

-- function-style read
local fg2 = h:getForegroundColor()  -- equivalent table

-- property-style write
h.foreground_color = { r = 255, g = 128, b = 0, a = 255 }

-- function-style write
h:setForegroundColor({ 255, 128, 0, 255 })

-- boolean property
h.bold = true

-- boolean legacy setter
h:setBold(true)
```

Notes:
- Both access forms are implemented to be equivalent; tests exercise both styles to ensure parity. Use property-style for new code for clarity, and function-style when maintaining older scripts.


Style escapes enable common typographic effects. Most are toggles that remain in effect until a matching closing escape (e.g., `[/bold]`) or a global `[reset]`. Closing escapes are strongly recommended for predictable nesting and restoration of prior styles.

- `[bold] ... [/bold]` — Bold text. If your font supports a bold metric this will use it; otherwise rendering emulates bold where possible.  
- `[italic] ... [/italic]` — Italic text. Uses an italic font variant if available or a simulated slant.  
- `[underline] ... [/underline]` — Underlined text. Drawn under the glyphs for the enclosed tokens.  
- `[strike] ... [/strike]` — Strikethrough (line through text) for the enclosed tokens.  
- `[outline]` or `[outline=N] ... [/outline]` — Outlined text. Optional numeric parameter `N` sets outline thickness; omit `N` to enable with the default thickness. Closing `[/outline]` restores the previous outline thickness.  
- `[dropshadow]` or `[dropshadow=X,Y] ... [/dropshadow]` — Enable a drop shadow for enclosed tokens. Optional `X,Y` set the shadow offset; defaults are used if omitted. `[/dropshadow]` restores previous shadow state.  
- `[reset]` — Immediate full reset: clears all active style, color, alignment, and numeric overrides and restores the baseline LabelStyle. `[reset]` has no separate closing tag.

Notes:
- Tags are case‑insensitive. Both forms (explicit close `[/tag]` and global `[reset]`) are accepted; prefer explicit closes for nested changes.
- Numeric/parameterized variants validate inputs and ignore malformed values instead of throwing.
- Styles stack: entering a style pushes the previous value and `[/tag]` pops it (LIFO). `[reset]` clears the stack entirely.
- Escape sequences are not rendered as text; they affect subsequent word tokens only.


#### Colors:
```
SDL_Color Label::foregroundColor = {255, 255, 255, 255}; // "fgnd" (default)
SDL_Color Label::backgroundColor = {0, 0, 0, 0};         // "bgnd"
SDL_Color Label::borderColor = {0, 0, 0, 0};             // "border"
SDL_Color FontStyle::outlineColor = {0, 0, 0, 255};      // "outline"
SDL_Color FontStyle::dropShadowColor = {0, 0, 0, 128};   // "shadow"
```

### Alignment

Labels support independent horizontal and vertical alignment. Horizontal modes: left, center, right. Vertical modes: top, middle, bottom. Alignment is stored in the LabelStyle (either as two explicit fields or a compact bitfield) and is applied per-token so different parts of the same label can be aligned differently.

Two ways to set alignment:
- Default / property-level: the label object exposes default alignment flags (e.g., label.align_h and label.align_v). These apply to all tokens unless an inline alignment escape overrides them.
- Inline escape tags: use `[align=left]`, `[align=center]`, `[align=right]`, `[align=top]`, `[align=middle]`, `[align=bottom]`. Inline tags change only the matching axis:
  - `[align=left|center|right]` changes horizontal alignment.
  - `[align=top|middle|bottom]` changes vertical alignment.
Inline alignment remains in effect until a matching closing tag (e.g., `[/align]`) or `[reset]` restores the previous alignment. Nested alignment tags are supported and restored in LIFO order.

Token grouping and rendering
- After tokenization every word token carries its LabelStyle snapshot, including the effective alignment for that token.
- Tokens are sorted into nine alignment groups (horizontal × vertical): TOP_LEFT, TOP_CENTER, TOP_RIGHT, MIDDLE_LEFT, MIDDLE_CENTER, MIDDLE_RIGHT, BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT.
- Each alignment group is laid out and rendered independently. Within a group, words are wrapped into lines using the label width (minus padding) and then positioned according to the group's horizontal rule. The group's vertical rule controls the overall vertical placement of those lines inside the label bounds.
- This grouping lets the label mix different alignments (for example a left-aligned header followed by a centered score) while still computing layout deterministically and efficiently.

Precedence and semantics
- Inline alignment escapes override the label's default alignment for subsequent tokens only. A closing `[/align]` or `[reset]` restores the previous alignment state.
- If an inline tag specifies a horizontal keyword it changes only the horizontal axis; vertical keywords change only the vertical axis. Using `[align=center]` inside text will center horizontally while leaving vertical alignment unchanged.
- When `auto_width`/`auto_height` is enabled, alignment calculations use the final computed label bounds (after autosizing). Centering and middle placement are therefore computed against those adjusted bounds. If the label is clipped to its parent, alignment is performed within the clipped bounds.

Examples
- Inline horizontal override:
  ```
  Score: [align=center]1000[/align] Level: 3
  ```
  The "1000" token(s) will be centered while surrounding text uses the label default.
- Inline vertical usage (useful for multi-line composed labels):
  ```
  [align=top]Header[/align]
  [align=middle]Body[/align]
  [align=bottom]Footer[/align]
  ```

Implementation notes
- Store alignment in the LabelStyle (two fields or a compact bitfield). When tokenizing, snapshot the current alignment into the token so layout can proceed purely from token data.
- When grouping tokens into nine buckets, preserve original token order within each bucket so phrase grouping and wrapping remain deterministic.
- Because alignment is per-token, phrase formation (runs of identical style) should be done per-alignment group to allow efficient phrase-level rendering (caching TTF surfaces or batching bitmap glyph draws).
- Tests: add unit tests that create labels mixing `[align=...]` escapes and verify that tokens are placed into the correct alignment buckets and that rendered bounding boxes reflect the requested rules.

Performance
- The nine-group approach enables parallel/independent layout passes for each alignment mode and reduces branching during rendering. It also makes it straightforward to only re-layout the alignment groups affected by a style change.

### Quick reference
- Default property: label.align = { horizontal = left|center|right, vertical = top|middle|bottom }
- Inline tags: `[align=left|center|right|top|middle|bottom]` and `[/align]`
- Inline tags override default until closed or reset.
- Tokens are grouped into the nine alignment queues and laid out/rendered per-queue.

---

### Alignment

Labels support independent horizontal and vertical alignment. Horizontal modes: left, center, right. Vertical modes: top, middle, bottom. Alignment is stored in the LabelStyle (either as two explicit fields or a compact bitfield) and is applied per-token so different parts of the same label can be aligned differently.

Two ways to set alignment:
- Default / property-level: the label object exposes default alignment flags (e.g., label.align_h and label.align_v). These apply to all tokens unless an inline alignment escape overrides them.
- Inline escape tags: use `[align=left]`, `[align=center]`, `[align=right]`, `[align=top]`, `[align=middle]`, `[align=bottom]`. Inline tags change only the matching axis:
  - `[align=left|center|right]` changes horizontal alignment.
  - `[align=top|middle|bottom]` changes vertical alignment.
Inline alignment remains in effect until a matching closing tag (e.g., `[/align]`) or `[reset]` restores the previous alignment. Nested alignment tags are supported and restored in LIFO order.

Token grouping and rendering
- After tokenization every word token carries its LabelStyle snapshot, including the effective alignment for that token.
- Tokens are sorted into nine alignment groups (horizontal × vertical): TOP_LEFT, TOP_CENTER, TOP_RIGHT, MIDDLE_LEFT, MIDDLE_CENTER, MIDDLE_RIGHT, BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT.
- Each alignment group is laid out and rendered independently. Within a group, words are wrapped into lines using the label width (minus padding) and then positioned according to the group's horizontal rule. The group's vertical rule controls the overall vertical placement of those lines inside the label bounds.
- This grouping lets the label mix different alignments (for example a left-aligned header followed by a centered score) while still computing layout deterministically and efficiently.

Precedence and semantics
- Inline alignment escapes override the label's default alignment for subsequent tokens only. A closing `[/align]` or `[reset]` restores the previous alignment state.
- If an inline tag specifies a horizontal keyword it changes only the horizontal axis; vertical keywords change only the vertical axis. Using `[align=center]` inside text will center horizontally while leaving vertical alignment unchanged.
- When `auto_width`/`auto_height` is enabled, alignment calculations use the final computed label bounds (after autosizing). Centering and middle placement are therefore computed against those adjusted bounds. If the label is clipped to its parent, alignment is performed within the clipped bounds.

Examples
- Inline horizontal override:
  ```
  Score: [align=center]1000[/align] Level: 3
  ```
  The "1000" token(s) will be centered while surrounding text uses the label default.
- Inline vertical usage (useful for multi-line composed labels):
  ```
  [align=top]Header[/align]
  [align=middle]Body[/align]
  [align=bottom]Footer[/align]
  ```

Implementation notes
- Store alignment in the LabelStyle (two fields or a compact bitfield). When tokenizing, snapshot the current alignment into the token so layout can proceed purely from token data.
- When grouping tokens into nine buckets, preserve original token order within each bucket so phrase grouping and wrapping remain deterministic.
- Because alignment is per-token, phrase formation (runs of identical style) should be done per-alignment group to allow efficient phrase-level rendering (caching TTF surfaces or batching bitmap glyph draws).
- Tests: add unit tests that create labels mixing `[align=...]` escapes and verify that tokens are placed into the correct alignment buckets and that rendered bounding boxes reflect the requested rules.

Performance
- The nine-group approach enables parallel/independent layout passes for each alignment mode and reduces branching during rendering. It also makes it straightforward to only re-layout the alignment groups affected by a style change.

### Quick reference
- Default property: label.align = { horizontal = left|center|right, vertical = top|middle|bottom }
- Inline tags: `[align=left|center|right|top|middle|bottom]` and `[/align]`
- Inline tags override default until closed or reset.
- Tokens are grouped into the nine alignment queues and laid out/rendered per-queue.



#### Bitfield Encoding for Alignment

Alignment values can be encoded as bitfields to represent the nine possible text alignment modes:

```
left   = 0b0001
center = 0b0010
right  = 0b0011
top    = 0b0100
middle = 0b1000
bottom = 0b1100

(zero bit field indicates DEFAULT horizontal or DEFAULT vertical)

// Combined alignments
TOP_LEFT      = top    | left
TOP_CENTER    = top    | center
TOP_RIGHT     = top    | right
MIDDLE_LEFT   = middle | left
MIDDLE_CENTER = middle | center
MIDDLE_RIGHT  = middle | right
BOTTOM_LEFT   = bottom | left
BOTTOM_CENTER = bottom | center
BOTTOM_RIGHT  = bottom | right
```

This bitfield approach allows efficient representation and sorting of word tokens into the nine alignment vectors for rendering.
- `[align=left]`    — Left align 
- `[align=center]`  — Center align (horizontal)
- `[align=right]`   — Right align 
- `[align=top]`     — Top align
- `[align=middle]`  — Middle align (vertical)
- `[align=bottom]`  — Bottom align

### Special
- `[[]` — Render `[` character
- `[]]` — Render `]` character
- `\"`  — Render `"` character 
 Escape sequences are used to change rendering state (style, color, alignment) within the label text. Example syntax:
 - `[color=#FF0000]` — changes text color to red
 - `[bold]` — enables bold style
 - `[align=center]` — sets alignment to center (horizontal or vertical)
 - `[reset]` — resets all styles to default

 ### Escaping Square Brackets

 To render literal square brackets in label text, use double brackets:

 - `[[]` renders a single `[` character
 - `[]]` renders a single `]` character

 This approach avoids ambiguity with style escape sequences and is user-friendly for authors.

Escape sequences are not rendered as text, but affect the rendering of subsequent tokens.

### Font Size
- `[size=16]` — Sets the font size to 16 for all subsequent tokens.
- `[size=8]`  — Sets the font size to 8 (e.g., for retro bitmap fonts).

  Note: Inter text size escapes are supported at the beginning of the text string only.  Adding size changes mid string may have undefined results.

---

## Numeric Style Escapes (border / outline / padding / dropshadow)

### Overview
These escapes allow inline control of numeric rendering parameters that affect phrase rendering (thickness, padding, and drop-shadow offsets). They complement boolean style escapes (bold, italic, outline-enable, dropshadow-enable) by letting authors tune numeric values inline.

### Syntax
- `[border=N] ... [/border]` — set border thickness to N for the enclosed tokens.
- `[outline=N] ... [/outline]` — set outline thickness to N for the enclosed tokens.
- `[pad=WxH] ... [/pad]` (alias) or `[padding=WxH] ... [/padding]` — set horizontal and vertical padding to W and H.
- Parameter variants:
  - `W` sets both horizontal and vertical padding to W.
  - `WxH` sets `padding_horiz = W` and `padding_vert = H`.
- Both closing forms (`[/pad]` and `[/padding]`) are accepted and restore previous values.
  - Example: `[pad=5x7]` sets padding_horiz=5 and padding_vert=7.
- `[dropshadow=X,Y] ... [/dropshadow]` — set drop shadow offsets dropshadowOffsetX=X and dropshadowOffsetY=Y.
  - Example: `[dropshadow=4,6]` sets dropshadowOffsetX=4 and dropshadowOffsetY=6.

Notes:
- Numeric parameters are integer values. Parameters are validated before conversion; malformed params are ignored (no exception thrown) and the default/current value is retained.
- Short forms and synonyms (`[pad]` vs `[padding]`) are supported for convenience.

### Semantics
- Opening tag applies the numeric change to the current style for subsequent tokens until the matching closing tag.
- Closing tag restores the previous numeric value(s). Nested tags are supported; restoration uses per-field stacks (or a snapshot stack) so nested overrides restore correctly.
- If an opening tag omits a numeric parameter (e.g., `[border]`), the tag may act as an enable toggle or use a documented default — prefer explicit numeric parameters in tests/scripts.
- The numeric fields are stored in `FontStyle` and exposed to Lua token inspection as:
  - `borderThickness`, `outlineThickness`, `padding_horiz`, `padding_vert`, `dropshadowOffsetX`, `dropshadowOffsetY`

### Examples
- Border thickness 3 for the word "BORDER":
  ```
  Start [border=3]BORDER[/border] End
  ```
  `BORDER` tokens will have `style.borderThickness == 3`.
- Outline and padding:
  ```
  [outline=2]O1[/outline] [pad=5x7]P1[/pad]
  ```
  - `O1` will carry `style.outlineThickness == 2`.
  - `P1` will carry `style.padding_horiz == 5` and `style.padding_vert == 7`.
- Dropshadow offsets:
  ```
  [dropshadow=4,6]Shadowed[/dropshadow]
  ```
  - Tokens inside will have `style.dropshadowOffsetX == 4` and `style.dropshadowOffsetY == 6`.

### Implementation Notes
- Tokenizer responsibilities:
  - Recognize numeric-parameter escapes and split tag name / parameter string.
  - Validate numeric parameters (digits, optional separators) before calling std::stoi to avoid exceptions.
  - Maintain per-field stacks (or a FontStyle snapshot stack) to support nested tags and proper restoration on close.
  - Record escape tokens with the style state *before* applying the tag action (current tokenizer semantic). Word tokens are recorded with the style state after applying the action.
- Defaults and documentation:
  - Ensure header defaults in `SDOM_IFontObject.hpp` and doc defaults match (synchronize default values in docs and headers).
- Lua bindings / testing:
  - `DisplayHandle:getTokenList()` returns tokens where `token.style` includes numeric fields named exactly as in `FontStyle`.
  - Add unit tests (Label_test5) that create labels containing the numeric escapes, call `tokenizeText()`, then assert the expected numeric values are present on the word tokens inside the tags.
- Error handling:
  - On malformed numeric params, avoid throwing — log or ignore and continue. Tests should assert presence of values only when syntactically valid parameters are used.

### Test seed suggestion (Label_test5)
```
local txt = "Start [border=3]B1[/border] [outline=2]O1[/outline] [pad=5x7]P1[/pad] [dropshadow=4,6]DS1[/dropshadow] End"
```
- After `h:setText(txt)` and `h:tokenizeText()`, verify that the relevant word tokens expose:
  - `style.borderThickness == 3`
  - `style.outlineThickness == 2`
  - `style.padding_horiz == 5` and `style.padding_vert == 7`
  - `style.dropshadowOffsetX == 4` and `style.dropshadowOffsetY == 6`


---

## Token Structure

Each parsed token represents either a word or an escape sequence:

```cpp
enum class TokenType { Word, Escape };
struct LabelToken {
    TokenType type;
    std::string text;      // For words
    std::string escape;    // For escape sequences
    LabelStyle style;      // Style at this token
};
```

 **Word tokens**: Actual text to render, with associated style and alignment.
 **Escape tokens**: Instructions to change style, color, alignment, etc.

 **Token Types:**
 - Word: Plain text, whitespace, punctuation
 - Escape: Style change, color, font, alignment
 - Bracket Escape: Literal '[' or ']'

 ### Word Tokenization Rules
- Quoted text (enclosed in double quotes) is tokenized as word tokens, but spaces and punctuation within the quoted text are still tokenized as individual word tokens (not grouped).
- Escape sequences occurring within a double-quote block are treated as word tokens, not escape tokens.
- To render a literal double quote character, use the escape sequence `["].`
- Single quotes are always tokenized as punctuation or as part of a word (e.g., apostrophes).
#### Example tokenization:
Input: 
```
"Hello, [b]world!" 'test' ["]
```
Output: 
```
"
Hello
,
[b]
world
!
"
'
test
'
"
```

 - Word tokens are sequences of characters not part of escape sequences or bracket escapes.
 - Token boundaries are defined by:
     - Whitespace (space, tab, newline)
     - Escape sequence start (`[`)
     - Bracket escapes (`[[]`, `[]]`)
- Punctuation marks (e.g., `.`, `,`, `!`, `?`) are each treated as their own word token.
- Spaces are each treated as their own token (one space-per-token), allowing for adjustable space width.
- Tabs (`\t`) are treated as their own token.
- Newlines (`\n`) are treated as their own token.
- Words are sequences of non-whitespace, non-punctuation, non-escape, and non-bracket characters.
- Example tokenization:
    - Input: `Hello,  [b]world[[]]![]]\t\n`
    - Tokens: `Hello`, `,`, ` `, ` `, `[b]`, `world`, `[[]]`, `!`, `[]]`, `\t`, `\n`

 **Special Cases:**
 - Bracket escapes (`[[]`, `[]]`) are treated as single tokens, not split into word/escape.
 - Escape sequences (e.g., `[b]`, `[color=red]`) are not part of word tokens.

- **Escape tokens**: Instructions to change style, color, alignment, etc.

### Escape Tokenization Rules

- Escape tokens are recognized as pairs of square brackets (e.g., `[b]`, `[color=red]`, `[reset]`) that are not within an enclosing double-quote block.
- Any sequence starting with `[` and ending with `]` outside of double quotes is treated as an escape token.
- Escape tokens inside double quotes are treated as word tokens, not escape tokens.
- Bracket escapes (`[[]`, `[]]`) are not treated as escape tokens, but as word tokens representing literal brackets.



---

## Parsing Algorithm
1. **Initialize** default `LabelStyle`.
2. **Scan input string**:
    - When an escape sequence is encountered, parse and update the current `LabelStyle`.
    - When a word is encountered, create a token with the current style and alignment.
3. **Store tokens** in a list for further processing.

---

## Alignment Handling
- Each word token carries its own alignment (horizontal and vertical) from `LabelStyle`.
- For rendering, tokens are sorted into nine groups based on alignment:
    - LEFT+TOP, LEFT+MIDDLE, LEFT+BOTTOM
    - CENTER+TOP, CENTER+MIDDLE, CENTER+BOTTOM
    - RIGHT+TOP, RIGHT+MIDDLE, RIGHT+BOTTOM
- Each group is rendered according to its alignment rules.

---

## Word Wrapping & Autosizing
- Measure each word using font metrics.
- Accumulate words into lines, wrapping when the line width exceeds label bounds (minus padding).
- If `auto_width` or `auto_height` is enabled, adjust label bounds to fit content.
- If not, clip content to label bounds.

---

## Rendering
- For each line and token:
    - Apply the token's style and alignment.
    - Render the word using the associated font and style.
    - Skip rendering for escape tokens.

---

## Extensibility
- New escape sequences can be added for additional styles (e.g., outline, dropshadow).
- The tokenization and rendering logic is modular, allowing future expansion (e.g., hyperlinks, images).

---

## Example
```
[bold][color=#00FF00]Score:[reset] 100 [align=right][color=#FF0000]Game Over
```
- "Score:" is bold and green.
- "100" is default style.
- "Game Over" is right-aligned and red.

---

## Implementation Notes

### Label Sizing and Wrapping Rules

| wordwrap | auto_width | auto_height | clip_to_parent | Behavior                                                                 |
|----------|------------|-------------|----------------|--------------------------------------------------------------------------|
| false    | false      | false       | any            | Single line, fixed size, overflow is clipped at label bounds.            |
| false    | true       | false       | any            | Single line, label expands width to fit text, height fixed.              |
| false    | false      | true        | any            | Single line, width fixed, label expands height (rare, but possible).     |
| false    | true       | true        | any            | Single line, label expands both width and height to fit text.            |
| true     | false      | false       | any            | Multi-line, fixed size, overflow is clipped at label bounds.             |
| true     | true       | false       | any            | Multi-line, label expands width to fit longest wrapped line, height fixed. Max width is clamped to the minimum of a user-specified value (if set) and the parent's right edge. |
| true     | false      | true        | any            | Multi-line, width fixed, label expands height to fit all lines.          |
| true     | true       | true        | any            | Multi-line, label expands both width and height to fit all text.         |
  
**Note:**
For both axes, the maximum auto_width/auto_height should be clamped to the minimum of a user-specified maximum (if provided) and the label's parent's bounds. This ensures the label never exceeds its parent's area or a user-imposed limit.

- If `clip_to_parent` is true, always clip rendering to parent’s bounds, regardless of label size.

### Implementation Notes
- When `auto_width` or `auto_height` is true, recalculate the label’s size after tokenizing/layout.
- When `clip_to_parent` is true, use SDL’s clipping/scissor rect to restrict rendering.
- Always document these behaviors for other developers.

## Rendering Phases (Functions):
### 1. Pre-Render Layout & Bounds Adjustment
- **Purpose:** Calculate the final size and position of the label before rendering.
- **Actions:**
  - Use tokenizeText() to break the text into tokens.
  - Measure the required width/height based on tokens, wordwrap, and style.
  -If auto_width/auto_height is enabled, adjust the label’s bounds.
  - If clip_to_parent or similar is enabled, set up the SDL clip/scissor region to the label or parent bounds.
### 2. Phrase Grouping
  - **Purpose:** Group tokens into “phrases” (runs of tokens with identical rendering attributes and on the same line).- **Actions:**
    - Iterate through each alignment list.
    - For each line, group consecutive tokens with the same `FontStyle` (or at least the same visual attributes).
    - Store these as `PhraseToken` objects: `{startX, text, style, lineY, ...}` in a new map, e.g. `std::map<AlignQueue, std::vector<PhraseToken>> phraseAlignLists_;`
### 3. Phrase Rendering
  - **Purpose:** Render each phrase efficiently and with correct visual effects.
  - **Actions:**
    - For each phrase in order:
      - If TTF: Use `TTF_RenderText_Blended()` for the phrase.
      - `BitmapFont`: This should also render text in phrase groups adding display attributes like TruetypeFont does.
      - Apply all style effects (underline, strikethrough, outline, dropshadow, etc.) at the phrase level.
      - Render backgrounds, borders, etc., as needed.
    - Restore the previous SDL clip region if it was changed.

## Label Usage Examples
```lua
{
    name = "blueishBoxLabel",
    type = "Label",

    anchor_left = "left",
    x = 185,
    anchor_top = "top",
    y = 75,
    anchor_right = "right",
    width = 240,
    anchor_bottom = "bottom",
    height = 215,

    wordwrap = true,
    auto_resize = false,

    outline = true,
    border = true,
    border_color = { r = 255, g = 255, b = 255, a = 64 },

    text = "The [bold][color=lt_red]quick brown[color=white][/bold] fox jumps over the lazy dog by the river...",
    resource_name = "internal_ttf",
    font_size = 16,
    alignment = "center",
    foreground_color = { r = 255, g = 255, b = 255, a = 255 },
    max_width = 500
}
```
### **Explanation**

This label is positioned and sized relative to its parent using **edge anchors**:

| Property | Meaning |
|---------|---------|
| `anchor_left = "left"`   | The label’s left edge attaches to the parent’s left edge. |
| `anchor_top = "top"`     | The label’s top edge attaches to the parent’s top edge.   |
| `anchor_right = "right"` | The label’s right edge follows the parent’s right edge.   |
| `anchor_bottom = "bottom"` | The label’s bottom edge follows the parent’s bottom edge. |

Because all four edges are anchored, the label **automatically resizes** when the parent resizes.

The `x`, `y`, `width`, and `height` values serve as the **initial placement** before anchors adjust final geometry.  
When a parent changes size, the anchors override width/height to maintain the layout relationship.

---

### **Text and Formatting**

The `text` field supports **inline formatting tokens**:

| Token | Effect |
|-------|--------|
| `[bold]` / `[/bold]` | Toggles bold text rendering. |
| `[color=...]` | Changes the foreground text color (supports named colors and RGBA). |
| `[dropshadow]`, `[outline]`, etc. | Optional style tags depending on current font renderer capabilities. |

This allows rich typography inside a single string (e.g. multi-color, mixed styles, emphasis).

---
### **Anchor Reference Diagram**

Each edge of a Label (or any `IDisplayObject`) may anchor to one of the parent’s edges.  
The anchor determines *how the object moves or resizes when the parent changes size.*

```
Parent Object Bounds
┌──────────────────────────────────────────────┐
│                                              │
│ (anchor_top = "top")                         │
│ ▲                                            │
│ │                                            │
│ ┌───────────────────────────────┐            │
│ │ (anchor_left="left") Label    │            │ 
│ │ ┌────────────┐                │            │
│ │ │ TEXT       │                │            │
│ │ └────────────┘                │            │
│ └───────────────────────────────┘            │
│ │                  (anchor_right="right") ──▶│
│ ▼                                            │
│ (anchor_bottom = "bottom")                   │
│                                              │
└──────────────────────────────────────────────┘
```

---

### **Wrapping & Layout Behavior**

| Property | Purpose |
|---------|---------|
| `wordwrap = true` | Text automatically wraps when reaching the right edge. |
| `alignment = "center"` | Controls line justification (`left`, `center`, `right`). |
| `max_width = 500` | Hard upper limit on wrapping width. |
| `auto_resize = false` | Size stays fixed; text wraps to fit within bounds. |

If the label becomes too small, wrapped text may overlap or run vertically beyond bounds.  
This is expected — the layout assumes the user sizes labels appropriately.

---

### **Rendering Performance**

Labels are **cached** to an off-screen texture.  
The label only re-renders when it becomes **dirty**, which occurs when:

- Text changes
- Size changes
- Font or formatting changes

Once rendered, drawing the label each frame is inexpensive.

---



---
## ASCII Glyphs

| Normal Index | Normal Character | Bold Index | Bold Character |
|--------------|------------------|------------|----------------|
| 0            | (space)          | 96         | (space)        |
| 1            | !                | 97         | !              |
| 2            | "                | 98         | "              |
| 3            | #                | 99         | #              |
| 4            | $                | 100        | $              |
| 5            | %                | 101        | %              |
| 6            | &                | 102         | &              |
| 7            | '                | 103        | '              |
| 8            | (                | 104        | (              |
| 9            | )                | 105        | )              |
| 10           | *                | 106        | *              |
| 11           | +                | 107        | +              |
| 12           | ,                | 108        | ,              |
| 13           | -                | 109        | -              |
| 14           | .                | 110        | .              |
| 15           | /                | 111        | /              |
| 16           | 0                | 112        | 0              |
| 17           | 1                | 113        | 1              |
| 18           | 2                | 114        | 2              |
| 19           | 3                | 115        | 3              |
| 20           | 4                | 116        | 4              |
| 21           | 5                | 117        | 5              |
| 22           | 6                | 118        | 6              |
| 23           | 7                | 119        | 7              |
| 24           | 8                | 120        | 8              |
| 25           | 9                | 121        | 9              |
| 26           | :                | 122        | :              |
| 27           | ;                | 123        | ;              |
| 28           | <                | 124        | <              |
| 29           | =                | 125        | =              |
| 30           | >                | 126        | >              |
| 31           | ?                | 127        | ?              |
| 32           | @                | 128        | @              |
| 33           | A                | 129        | A              |
| 34           | B                | 130        | B              |
| 35           | C                | 131        | C              |
| 36           | D                | 132        | D              |
| 37           | E                | 133        | E              |
| 38           | F                | 134        | F              |
| 39           | G                | 135        | G              |
| 40           | H                | 136        | H              |
| 41           | I                | 137        | I              |
| 42           | J                | 138        | J              |
| 43           | K                | 139        | K              |
| 44           | L                | 140        | L              |
| 45           | M                | 141        | M              |
| 46           | N                | 142        | N              |
| 47           | O                | 143        | O              |
| 48           | P                | 144        | P              |
| 49           | Q                | 145        | Q              |
| 50           | R                | 146        | R              |
| 51           | S                | 147        | S              |
| 52           | T                | 148        | T              |
| 53           | U                | 149        | U              |
| 54           | V                | 150        | V              |
| 55           | W                | 151        | W              |
| 56           | X                | 152        | X              |
| 57           | Y                | 153        | Y              |
| 58           | Z                | 154        | Z              |
| 59           | [                | 155        | [              |
| 60           | \                | 156        | \              |
| 61           | ]                | 157        | ]              |
| 62           | ^                | 158        | ^              |
| 63           | _                | 159        | _              |
| 64           | `                | 160        | `              |
| 65           | a                | 161        | a              |
| 66           | b                | 162        | b              |
| 67           | c                | 163        | c              |
| 68           | d                | 164        | d              |
| 69           | e                | 165        | e              |
| 70           | f                | 166        | f              |
| 71           | g                | 167        | g              |
| 72           | h                | 168        | h              |
| 73           | i                | 169        | i              |
| 74           | j                | 170        | j              |
| 75           | k                | 171        | k              |
| 76           | l                | 172        | l              |
| 77           | m                | 173        | m              |
| 78           | n                | 174        | n              |
| 79           | o                | 175        | o              |
| 80           | p                | 176        | p              |
| 81           | q                | 177        | q              |
| 82           | r                | 178        | r              |
| 83           | s                | 179        | s              |
| 84           | t                | 180        | t              |
| 85           | u                | 181        | u              |
| 86           | v                | 182        | v              |
| 87           | w                | 183        | w              |
| 88           | x                | 184        | x              |
| 89           | y                | 185        | y              |
| 90           | z                | 186        | z              |
| 91           | {                | 187        | {              |
| 92           | \|                | 188        | \|              |
| 93           | }                | 189        | }              |
| 94           | ~                | 190        | ~              |
| 95           | (C)              | 191        | (C)            |

This table represents the first 192 glyphs in the `default_8x8` font. Additional glyphs, such as icons, can be cataloged in subsequent sections.

## Graphic Glyphs

| Index | Icon Description          |
|-------|---------------------------|
| 192   | Line None                 |
| 193   | Line North                |
| 194   | Line East                 |
| 195   | Line North-East           |
| 196   | Line South                |
| 197   | Line North-South          |
| 198   | Line East-South           |
| 199   | Line North-East-South     |
| 200   | Line West                 |
| 201   | Line North-West           |
| 202   | Line East-West            |
| 203   | Line North-East-West      |
| 204   | Line South-West           |
| 205   | Line North-South-West     |
| 206   | Line East-South-West      |
| 207   | Line North-East-South-West|
| 208   | Solid None                 |
| 209   | Solid Upper-Right          |
| 210   | Solid Lower-Right          |
| 211   | Solid Upper-Right, Lower-Right |
| 212   | Solid Lower-Left           |
| 213   | Solid Upper-Right, Lower-Left  |
| 214   | Solid Lower-Right, Lower-Left  |
| 215   | Solid Upper-Right, Lower-Right, Lower-Left |
| 216   | Solid Upper-Left           |
| 217   | Solid Upper-Right, Upper-Left |
| 218   | Solid Lower-Right, Upper-Left |
| 219   | Solid Upper-Right, Lower-Right, Upper-Left |
| 220   | Solid Lower-Left, Upper-Left |
| 221   | Solid Upper-Right, Lower-Left, Upper-Left |
| 222   | Solid Lower-Right, Lower-Left, Upper-Left |
| 223   | Solid Full                 |
| 224   | Dithered None                 |
| 225   | Dithered Upper-Right          |
| 226   | Dithered Lower-Right          |
| 227   | Dithered Upper-Right, Lower-Right |
| 228   | Dithered Lower-Left           |
| 229   | Dithered Upper-Right, Lower-Left  |
| 230   | Dithered Lower-Right, Lower-Left  |
| 231   | Dithered Upper-Right, Lower-Right, Lower-Left |
| 232   | Dithered Upper-Left           |
| 233   | Dithered Upper-Right, Upper-Left |
| 234   | Dithered Lower-Right, Upper-Left |
| 235   | Dithered Upper-Right, Lower-Right, Upper-Left |
| 236   | Dithered Lower-Left, Upper-Left |
| 237   | Dithered Upper-Right, Lower-Left, Upper-Left |
| 238   | Dithered Lower-Right, Lower-Left, Upper-Left |
| 239   | Dithered Full                 |
| 240   | Solid on Dithered None                 |
| 241   | Solid on Dithered Upper-Right          |
| 242   | Solid on Dithered Lower-Right          |
| 243   | Solid on Dithered Upper-Right, Lower-Right |
| 244   | Solid on Dithered Lower-Left           |
| 245   | Solid on Dithered Upper-Right, Lower-Left  |
| 246   | Solid on Dithered Lower-Right, Lower-Left  |
| 247   | Solid on Dithered Upper-Right, Lower-Right, Lower-Left |
| 248   | Solid on Dithered Upper-Left           |
| 249   | Solid on Dithered Upper-Right, Upper-Left |
| 250   | Solid on Dithered Lower-Right, Upper-Left |
| 251   | Solid on Dithered Upper-Right, Lower-Right, Upper-Left |
| 252   | Solid on Dithered Lower-Left, Upper-Left |
| 253   | Solid on Dithered Upper-Right, Lower-Left, Upper-Left |
| 254   | Solid on Dithered Lower-Right, Lower-Left, Upper-Left |
| 255   | Solid on Dithered Full                 |

---

## References
- SDOM `Label` class
- `LabelStyle` and `FontStyle` structs
- SDL2/SDL3 rendering best practices

---

*This document will be updated as the implementation progresses.*
