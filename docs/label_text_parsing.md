# Label Text Parsing Design Document

## Overview
This document describes the design and implementation strategy for parsing and rendering text in the SDOM2 `Label` class, including support for word wrapping, inline style changes, escape sequences, and advanced alignment.

---

## Goals
- Support word wrapping and autosizing for labels.
- Allow inline style changes (color, bold, underline, etc.) via escape sequences.
- Enable mixed horizontal and vertical alignment within a single label.
- Efficiently tokenize and process text for rendering.

---

## Escape Sequence Syntax
---

## Default Escape Sequences


### Color Palette (8 colors)
- `[color=black]`   — Black (default)
- `[color=red]`     — Red
- `[color=green]`   — Green
- `[color=yellow]`  — Yellow
- `[color=blue]`    — Blue
- `[color=magenta]` — Magenta
- `[color=cyan]`    — Cyan
- `[color=white]`   — White
- `[RGB=rrggbb]`   — Set text color to custom 6-digit hex RGB value (e.g., `[RGB=FF8800]`)
- `[RGBA=rrggbbaa]` — Set text color to custom 8-digit hex RGBA value (e.g., `[RGBA=FF8800FF]`)
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
- `[RGBA:0000007f:shadow] — sets the drop shadow to transparent black


### Style
- `[bold]`       — Bold text
- `[italic]`     — Italic text
- `[underline]`  — Underlined text
- `[strike]`     — Strikethrough text
- `[outline]`    — Outlined text
- `[dropshadow]` — Dropshadow text
- `[reset]`      — Reset all styles to default

#### Colors:
```
SDL_Color Label::foregroundColor = {255, 255, 255, 255}; // "fgnd" (default)
SDL_Color Label::backgroundColor = {0, 0, 0, 0};         // "bgnd"
SDL_Color Label::borderColor = {0, 0, 0, 0};             // "border"
SDL_Color FontStyle::outlineColor = {0, 0, 0, 255};      // "outline"
SDL_Color FontStyle::dropShadowColor = {0, 0, 0, 128};   // "shadow"
```

### Alignment
-
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

---

## References
- SDOM2 `Label` class
- `LabelStyle` and `FontStyle` structs
- SDL2/SDL3 rendering best practices

---

*This document will be updated as the implementation progresses.*
