/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "SDOM - Simple SDL Document Object Model", "index.html", [
    [ "SDOM Documentation", "index.html", null ],
    [ "Architecture Overview", "md_docs_2architecture__overview.html", [
      [ "Runtime Flow (High Level)", "md_docs_2architecture__overview.html#autotoc_md3", null ]
    ] ],
    [ "Core Framework Design", "md_docs_2core__framework.html", [
      [ "Overview", "md_docs_2core__framework.html#autotoc_md5", null ],
      [ "Core Responsibilities", "md_docs_2core__framework.html#autotoc_md6", null ],
      [ "Core Initialization Sequence &amp; Pre-Initialization Configuration", "md_docs_2core__framework.html#autotoc_md7", [
        [ "Required Pre-Initialization Parameters:", "md_docs_2core__framework.html#autotoc_md8", null ],
        [ "Initialization Flow:", "md_docs_2core__framework.html#autotoc_md9", null ],
        [ "Example Configuration API:", "md_docs_2core__framework.html#autotoc_md10", null ],
        [ "Alternative Initialization: Lua Configuration", "md_docs_2core__framework.html#autotoc_md12", null ]
      ] ],
      [ "Primary Callbacks", "md_docs_2core__framework.html#autotoc_md14", null ],
      [ "Hook &amp; Callback Registration", "md_docs_2core__framework.html#autotoc_md15", [
        [ "Example (C++)", "md_docs_2core__framework.html#autotoc_md16", null ],
        [ "Example (Python/Rust)", "md_docs_2core__framework.html#autotoc_md17", null ]
      ] ],
      [ "Global Accessor Functions", "md_docs_2core__framework.html#autotoc_md18", [
        [ "Example Usage (with global accessor)", "md_docs_2core__framework.html#autotoc_md19", null ]
      ] ],
      [ "Composition Diagram", "md_docs_2core__framework.html#autotoc_md20", null ],
      [ "Event Flow", "md_docs_2core__framework.html#autotoc_md21", null ],
      [ "Unit Tests", "md_docs_2core__framework.html#autotoc_md22", null ],
      [ "Stage", "md_docs_2core__framework.html#autotoc_md23", null ],
      [ "SDOM Comprehensive Architecture Diagram", "md_docs_2core__framework.html#autotoc_md24", null ],
      [ "SDOM Event System Integration", "md_docs_2core__framework.html#autotoc_md25", null ],
      [ "Additional Interfaces and Utilities", "md_docs_2core__framework.html#autotoc_md26", null ],
      [ "Extensibility &amp; Singleton Pattern", "md_docs_2core__framework.html#autotoc_md27", null ],
      [ "EventListener Support", "md_docs_2core__framework.html#autotoc_md28", [
        [ "Example (C)", "md_docs_2core__framework.html#autotoc_md29", null ],
        [ "Benefits", "md_docs_2core__framework.html#autotoc_md30", null ]
      ] ],
      [ "Registering Custom Stage Objects", "md_docs_2core__framework.html#autotoc_md31", [
        [ "Example", "md_docs_2core__framework.html#autotoc_md32", null ]
      ] ],
      [ "Integration Methods Summary", "md_docs_2core__framework.html#autotoc_md33", [
        [ "Flexibility &amp; Use Cases", "md_docs_2core__framework.html#autotoc_md34", null ]
      ] ],
      [ "Performance Considerations", "md_docs_2core__framework.html#autotoc_md35", null ],
      [ "Single Stage, Single Window Rationale", "md_docs_2core__framework.html#autotoc_md36", null ],
      [ "Overriding the Default Stage", "md_docs_2core__framework.html#autotoc_md37", null ],
      [ "Multiple Stages as Scenes", "md_docs_2core__framework.html#autotoc_md38", null ],
      [ "Window Resizing &amp; Texture Resolution", "md_docs_2core__framework.html#autotoc_md39", null ],
      [ "Shared Display Texture for Stages", "md_docs_2core__framework.html#autotoc_md40", null ],
      [ "Stages as Game States or Modes", "md_docs_2core__framework.html#autotoc_md41", null ],
      [ "Changing the Active Stage", "md_docs_2core__framework.html#autotoc_md42", null ]
    ] ],
    [ "Glyph Index Catalog for default_8x8 Font", "md_docs_2default__font__8x8.html", [
      [ "ASCII Glyphs", "md_docs_2default__font__8x8.html#autotoc_md45", null ],
      [ "Graphic Glyphs", "md_docs_2default__font__8x8.html#autotoc_md46", null ],
      [ "Retro-Inspired Graphic Glyphs", "md_docs_2default__font__8x8.html#autotoc_md47", [
        [ "Customization Potential", "md_docs_2default__font__8x8.html#autotoc_md48", null ]
      ] ],
      [ "Escape Sequences for Text Rendering", "md_docs_2default__font__8x8.html#autotoc_md49", [
        [ "Text Styles", "md_docs_2default__font__8x8.html#autotoc_md50", null ],
        [ "Text Colors", "md_docs_2default__font__8x8.html#autotoc_md51", null ],
        [ "Alignment (Planned)", "md_docs_2default__font__8x8.html#autotoc_md52", null ],
        [ "Alignment Behavior", "md_docs_2default__font__8x8.html#autotoc_md53", null ]
      ] ],
      [ "Line Rendering with Word Wrap and Justification", "md_docs_2default__font__8x8.html#autotoc_md54", null ]
    ] ],
    [ "Display Object Anchoring System", "md_docs_2display__object__anchoring.html", [
      [ "Overview", "md_docs_2display__object__anchoring.html#autotoc_md63", null ],
      [ "Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md64", [
        [ "Anchor Naming Conventions", "md_docs_2display__object__anchoring.html#autotoc_md56", null ],
        [ "Edge-Based Layout", "md_docs_2display__object__anchoring.html#autotoc_md57", null ],
        [ "Position and Size Calculation", "md_docs_2display__object__anchoring.html#autotoc_md58", null ],
        [ "Anchor Accessors/Mutators", "md_docs_2display__object__anchoring.html#autotoc_md59", null ],
        [ "Changing Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md60", null ],
        [ "Drag-and-Drop and Reparenting", "md_docs_2display__object__anchoring.html#autotoc_md61", null ],
        [ "Summary", "md_docs_2display__object__anchoring.html#autotoc_md62", null ],
        [ "Reversed versions for user convenience", "md_docs_2display__object__anchoring.html#autotoc_md65", null ]
      ] ],
      [ "Lua Interface", "md_docs_2display__object__anchoring.html#autotoc_md66", null ],
      [ "Position and Size Calculation", "md_docs_2display__object__anchoring.html#autotoc_md67", null ],
      [ "Changing Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md68", null ],
      [ "Hierarchical Anchoring", "md_docs_2display__object__anchoring.html#autotoc_md69", null ],
      [ "Backward Compatibility &amp; Migration", "md_docs_2display__object__anchoring.html#autotoc_md70", null ],
      [ "Benefits", "md_docs_2display__object__anchoring.html#autotoc_md71", null ],
      [ "Example Use Cases", "md_docs_2display__object__anchoring.html#autotoc_md72", null ],
      [ "Anchoring Diagrams", "md_docs_2display__object__anchoring.html#autotoc_md75", [
        [ "1) Parent Anchor Grid and Child Edge Anchors", "md_docs_2display__object__anchoring.html#autotoc_md76", null ],
        [ "2) Anchor Change Recalculation (No Visual Jump)", "md_docs_2display__object__anchoring.html#autotoc_md77", null ]
      ] ],
      [ "Anchor helpers &amp; parsing", "md_docs_2display__object__anchoring.html#autotoc_md78", null ],
      [ "Migration note", "md_docs_2display__object__anchoring.html#autotoc_md79", null ]
    ] ],
    [ "DOM Propagation Design", "md_docs_2dom__propagation.html", [
      [ "Overview", "md_docs_2dom__propagation.html#autotoc_md81", null ],
      [ "Event Propagation Diagram", "md_docs_2dom__propagation.html#autotoc_md83", null ],
      [ "Traversal Types", "md_docs_2dom__propagation.html#autotoc_md84", [
        [ "1. Core-based DOM Traversal (Type 1)", "md_docs_2dom__propagation.html#autotoc_md85", null ],
        [ "2. EventManager-based Event Traversal (Type 2)", "md_docs_2dom__propagation.html#autotoc_md87", null ],
        [ "3. Factory-based Global Traversal (Type 3)", "md_docs_2dom__propagation.html#autotoc_md89", null ],
        [ "4. World Coordinate Backpropagation Traversal (Type 4)", "md_docs_2dom__propagation.html#autotoc_md90", null ]
      ] ],
      [ "Traversal Safety and Modification", "md_docs_2dom__propagation.html#autotoc_md91", [
        [ "Deferred actions processing (pseudo)", "md_docs_2dom__propagation.html#autotoc_md92", null ],
        [ "Thread-safety notes", "md_docs_2dom__propagation.html#autotoc_md93", null ]
      ] ],
      [ "Architectural Separation", "md_docs_2dom__propagation.html#autotoc_md94", null ],
      [ "Migration note", "md_docs_2dom__propagation.html#autotoc_md95", null ],
      [ "Next Steps", "md_docs_2dom__propagation.html#autotoc_md97", null ]
    ] ],
    [ "SDOM::Events Design Document", "md_docs_2event__design.html", [
      [ "Overview", "md_docs_2event__design.html#autotoc_md99", null ],
      [ "Goals", "md_docs_2event__design.html#autotoc_md100", null ],
      [ "Event Structure", "md_docs_2event__design.html#autotoc_md101", null ],
      [ "Event Lifecycle", "md_docs_2event__design.html#autotoc_md102", null ],
      [ "Event Types", "md_docs_2event__design.html#autotoc_md103", null ],
      [ "Predefined Event Types", "md_docs_2event__design.html#autotoc_md104", [
        [ "General Events", "md_docs_2event__design.html#autotoc_md105", null ],
        [ "Mouse Events", "md_docs_2event__design.html#autotoc_md106", null ],
        [ "Keyboard Events", "md_docs_2event__design.html#autotoc_md107", null ],
        [ "Timer Events", "md_docs_2event__design.html#autotoc_md108", null ],
        [ "UI Events", "md_docs_2event__design.html#autotoc_md109", null ],
        [ "Drag &amp; Drop Events", "md_docs_2event__design.html#autotoc_md110", null ],
        [ "Clipboard Events", "md_docs_2event__design.html#autotoc_md111", null ],
        [ "Custom Events", "md_docs_2event__design.html#autotoc_md112", null ]
      ] ],
      [ "EventManager Responsibilities", "md_docs_2event__design.html#autotoc_md113", null ],
      [ "Existing Implementation", "md_docs_2event__design.html#autotoc_md114", null ],
      [ "Next Steps", "md_docs_2event__design.html#autotoc_md115", null ],
      [ "Notes", "md_docs_2event__design.html#autotoc_md116", null ],
      [ "Event System Design: Categorizing EventTypes", "md_docs_2event__design.html#autotoc_md118", [
        [ "Proposal", "md_docs_2event__design.html#autotoc_md119", null ],
        [ "Advantages", "md_docs_2event__design.html#autotoc_md120", null ],
        [ "Suggested Implementation", "md_docs_2event__design.html#autotoc_md121", null ],
        [ "Integration", "md_docs_2event__design.html#autotoc_md122", null ],
        [ "Considerations", "md_docs_2event__design.html#autotoc_md123", [
          [ "Listener-Specific Events", "md_docs_2event__design.html#autotoc_md125", [
            [ "Example Behavior", "md_docs_2event__design.html#autotoc_md126", null ],
            [ "Integration", "md_docs_2event__design.html#autotoc_md127", null ]
          ] ]
        ] ]
      ] ]
    ] ],
    [ "Modern COM-like Interface and Hook Design", "md_docs_2exposing__hooks.html", [
      [ "- Extensible for future language hooks (Python, etc.)", "md_docs_2exposing__hooks.html#autotoc_md133", null ],
      [ "Dynamic Extensibility Strategies", "md_docs_2exposing__hooks.html#autotoc_md134", [
        [ "Overview", "md_docs_2exposing__hooks.html#autotoc_md130", null ],
        [ "Key Concepts", "md_docs_2exposing__hooks.html#autotoc_md131", null ],
        [ "Benefits", "md_docs_2exposing__hooks.html#autotoc_md132", null ],
        [ "Example: Dynamic Hook Registration in C++", "md_docs_2exposing__hooks.html#autotoc_md135", null ],
        [ "Example: Extern \"C\" Entry Point", "md_docs_2exposing__hooks.html#autotoc_md136", null ],
        [ "Example: Lua Integration with Sol2", "md_docs_2exposing__hooks.html#autotoc_md137", null ],
        [ "Example: Plugin/Module System", "md_docs_2exposing__hooks.html#autotoc_md138", null ],
        [ "Example: Reflection and Metadata", "md_docs_2exposing__hooks.html#autotoc_md139", null ],
        [ "Example: Smart Pointer Resource Management", "md_docs_2exposing__hooks.html#autotoc_md140", null ],
        [ "1. Dynamic Hook Registration", "md_docs_2exposing__hooks.html#autotoc_md141", null ],
        [ "2. Extern \"C\" Entry Points for Registration", "md_docs_2exposing__hooks.html#autotoc_md142", null ],
        [ "3. Lua/Script Integration", "md_docs_2exposing__hooks.html#autotoc_md143", null ],
        [ "4. Reflection and Metadata", "md_docs_2exposing__hooks.html#autotoc_md144", null ],
        [ "5. Plugin/Module System", "md_docs_2exposing__hooks.html#autotoc_md145", null ],
        [ "6. Smart Pointer and Resource Management", "md_docs_2exposing__hooks.html#autotoc_md146", null ],
        [ "Next Steps", "md_docs_2exposing__hooks.html#autotoc_md148", null ]
      ] ]
    ] ],
    [ "Extensible Factory Design", "md_docs_2extensible__factory.html", [
      [ "Factory Overview (Diagram)", "md_docs_2extensible__factory.html#autotoc_md151", null ],
      [ "Resource Type Management", "md_docs_2extensible__factory.html#autotoc_md152", null ],
      [ "Requirements", "md_docs_2extensible__factory.html#autotoc_md153", null ],
      [ "Recommended Approach", "md_docs_2extensible__factory.html#autotoc_md154", [
        [ "1. Initializer Structs and Lua Scripts", "md_docs_2extensible__factory.html#autotoc_md155", null ],
        [ "3. Factory Create Methods", "md_docs_2extensible__factory.html#autotoc_md156", null ],
        [ "4. Usage", "md_docs_2extensible__factory.html#autotoc_md157", null ]
      ] ],
      [ "Benefits", "md_docs_2extensible__factory.html#autotoc_md158", null ],
      [ "Implementation Strategy", "md_docs_2extensible__factory.html#autotoc_md160", [
        [ "1. Initializer Structs", "md_docs_2extensible__factory.html#autotoc_md161", null ],
        [ "1a. Standardized Constructor Signatures", "md_docs_2extensible__factory.html#autotoc_md162", null ],
        [ "2. Private Constructors &amp; Friend Factory", "md_docs_2extensible__factory.html#autotoc_md163", null ],
        [ "3. Factory Registration System", "md_docs_2extensible__factory.html#autotoc_md164", null ],
        [ "4. Generic Factory Create Method", "md_docs_2extensible__factory.html#autotoc_md165", null ],
        [ "5. Extensibility", "md_docs_2extensible__factory.html#autotoc_md166", null ],
        [ "6. Error Handling &amp; Validation", "md_docs_2extensible__factory.html#autotoc_md167", null ],
        [ "7. Example Usage", "md_docs_2extensible__factory.html#autotoc_md168", null ]
      ] ],
      [ "Standardized Constructors for IDisplayObjects", "md_docs_2extensible__factory.html#autotoc_md170", null ],
      [ "Scripting and Configuration Choice", "md_docs_2extensible__factory.html#autotoc_md171", null ],
      [ "Resource Pointer (<span class=\"tt\">resource_ptr</span>) Design", "md_docs_2extensible__factory.html#autotoc_md172", [
        [ "How It Works", "md_docs_2extensible__factory.html#autotoc_md173", null ],
        [ "Example Usage", "md_docs_2extensible__factory.html#autotoc_md174", null ],
        [ "Benefits", "md_docs_2extensible__factory.html#autotoc_md175", null ],
        [ "Implementation Outline", "md_docs_2extensible__factory.html#autotoc_md176", null ],
        [ "Summary", "md_docs_2extensible__factory.html#autotoc_md177", null ]
      ] ]
    ] ],
    [ "Icon Index Catalog for sprite_8x8 Font", "md_docs_2icon__8x8.html", [
      [ "Icon Glyphs", "md_docs_2icon__8x8.html#autotoc_md179", null ],
      [ "Icon Sets", "md_docs_2icon__8x8.html#autotoc_md180", [
        [ "Sets", "md_docs_2icon__8x8.html#autotoc_md181", null ]
      ] ],
      [ "Customization Potential", "md_docs_2icon__8x8.html#autotoc_md182", [
        [ "Extensibility", "md_docs_2icon__8x8.html#autotoc_md183", null ]
      ] ]
    ] ],
    [ "Design Goals for <span class=\"tt\">IPanelObject</span>", "md_docs_2IPanelObject.html", [
      [ "IPanelObject Design Document", "md_docs_2IPanelObject.html#autotoc_md185", [
        [ "What is a 9-slice (9-patch) panel?", "md_docs_2IPanelObject.html#autotoc_md186", null ],
        [ "What is a 9-slice (9-patch) panel?", "md_docs_2IPanelObject.html#autotoc_md187", null ],
        [ "Reference: sprite_8x8 Icon Index Catalog", "md_docs_2IPanelObject.html#autotoc_md188", [
          [ "Predefined 9-Slice Sets in <span class=\"tt\">sprite_8x8</span>", "md_docs_2IPanelObject.html#autotoc_md189", null ]
        ] ],
        [ "Suggested Steps", "md_docs_2IPanelObject.html#autotoc_md191", null ],
        [ "Interface Sketch", "md_docs_2IPanelObject.html#autotoc_md193", null ],
        [ "Note on Button Objects and State Handling", "md_docs_2IPanelObject.html#autotoc_md194", null ]
      ] ]
    ] ],
    [ "Label Text Parsing Design Document", "md_docs_2label__text__parsing.html", [
      [ "Overview", "md_docs_2label__text__parsing.html#autotoc_md196", null ],
      [ "Goals", "md_docs_2label__text__parsing.html#autotoc_md198", null ],
      [ "## Escape Sequence Syntax", "md_docs_2label__text__parsing.html#autotoc_md200", [
        [ "Supported Color Names", "md_docs_2label__text__parsing.html#autotoc_md201", [
          [ "Color Targets:", "md_docs_2label__text__parsing.html#autotoc_md202", null ]
        ] ],
        [ "Style", "md_docs_2label__text__parsing.html#autotoc_md203", [
          [ "Colors:", "md_docs_2label__text__parsing.html#autotoc_md204", null ]
        ] ],
        [ "Alignment", "md_docs_2label__text__parsing.html#autotoc_md205", [
          [ "Bitfield Encoding for Alignment", "md_docs_2label__text__parsing.html#autotoc_md206", null ]
        ] ],
        [ "Special", "md_docs_2label__text__parsing.html#autotoc_md207", null ],
        [ "Escaping Square Brackets", "md_docs_2label__text__parsing.html#autotoc_md208", null ],
        [ "Font Size", "md_docs_2label__text__parsing.html#autotoc_md209", null ]
      ] ],
      [ "Numeric Style Escapes (border / outline / padding / dropshadow)", "md_docs_2label__text__parsing.html#autotoc_md211", [
        [ "Overview", "md_docs_2label__text__parsing.html#autotoc_md212", null ],
        [ "Syntax", "md_docs_2label__text__parsing.html#autotoc_md213", null ],
        [ "Semantics", "md_docs_2label__text__parsing.html#autotoc_md214", null ],
        [ "Examples", "md_docs_2label__text__parsing.html#autotoc_md215", null ],
        [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md216", null ],
        [ "Test seed suggestion (Label_test5)", "md_docs_2label__text__parsing.html#autotoc_md217", null ]
      ] ],
      [ "Token Structure", "md_docs_2label__text__parsing.html#autotoc_md219", [
        [ "Word Tokenization Rules", "md_docs_2label__text__parsing.html#autotoc_md220", [
          [ "Example tokenization:", "md_docs_2label__text__parsing.html#autotoc_md221", null ]
        ] ],
        [ "Escape Tokenization Rules", "md_docs_2label__text__parsing.html#autotoc_md222", null ]
      ] ],
      [ "Parsing Algorithm", "md_docs_2label__text__parsing.html#autotoc_md224", null ],
      [ "Alignment Handling", "md_docs_2label__text__parsing.html#autotoc_md226", null ],
      [ "Word Wrapping &amp; Autosizing", "md_docs_2label__text__parsing.html#autotoc_md228", null ],
      [ "Rendering", "md_docs_2label__text__parsing.html#autotoc_md230", null ],
      [ "Extensibility", "md_docs_2label__text__parsing.html#autotoc_md232", null ],
      [ "Example", "md_docs_2label__text__parsing.html#autotoc_md234", null ],
      [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md236", [
        [ "Label Sizing and Wrapping Rules", "md_docs_2label__text__parsing.html#autotoc_md237", null ],
        [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md238", null ]
      ] ],
      [ "Rendering Phases (Functions):", "md_docs_2label__text__parsing.html#autotoc_md239", [
        [ "1. Pre-Render Layout &amp; Bounds Adjustment", "md_docs_2label__text__parsing.html#autotoc_md240", null ],
        [ "2. Phrase Grouping", "md_docs_2label__text__parsing.html#autotoc_md241", null ],
        [ "3. Phrase Rendering", "md_docs_2label__text__parsing.html#autotoc_md242", null ]
      ] ],
      [ "References", "md_docs_2label__text__parsing.html#autotoc_md244", null ]
    ] ],
    [ "Lua commands and properties exposed by SDOM (C++ → Lua binding inventory)", "md_docs_2lua__commands__and__properties.html", [
      [ "Globals / Core (lua <span class=\"tt\">Core</span> / <span class=\"tt\">CoreForward</span>)", "md_docs_2lua__commands__and__properties.html#autotoc_md247", null ],
      [ "DisplayHandle methods (registered by <span class=\"tt\">IDisplayObject::_registerLuaBindings</span>)", "md_docs_2lua__commands__and__properties.html#autotoc_md248", null ],
      [ "Exhaustive DisplayHandle mapping (line-by-line)", "md_docs_2lua__commands__and__properties.html#autotoc_md249", null ],
      [ "AssetHandle and per-asset augmentations", "md_docs_2lua__commands__and__properties.html#autotoc_md250", null ],
      [ "Event and EventType bindings", "md_docs_2lua__commands__and__properties.html#autotoc_md251", null ],
      [ "SDL helpers (lua <span class=\"tt\">SDL_Utils</span> / <span class=\"tt\">SDL</span>)", "md_docs_2lua__commands__and__properties.html#autotoc_md252", null ],
      [ "Binding helper summary (src/lua_BindHelpers.cpp)", "md_docs_2lua__commands__and__properties.html#autotoc_md253", null ],
      [ "Remaining work / completeness notes", "md_docs_2lua__commands__and__properties.html#autotoc_md254", null ]
    ] ],
    [ "Object Handles in SDOM", "md_docs_2object__handles.html", [
      [ "Two Trees in the Garden", "md_docs_2object__handles.html#autotoc_md259", null ],
      [ "Overview", "md_docs_2object__handles.html#autotoc_md260", null ],
      [ "Handle Types", "md_docs_2object__handles.html#autotoc_md261", [
        [ "1. DomHandle", "md_docs_2object__handles.html#autotoc_md262", null ],
        [ "2. ResHandle", "md_docs_2object__handles.html#autotoc_md263", null ]
      ] ],
      [ "Design Rationale", "md_docs_2object__handles.html#autotoc_md264", null ],
      [ "Inheritance Warning: Avoid Diamond Inheritance", "md_docs_2object__handles.html#autotoc_md266", null ],
      [ "Example Usage", "md_docs_2object__handles.html#autotoc_md267", null ],
      [ "Migration Strategy", "md_docs_2object__handles.html#autotoc_md268", null ],
      [ "Best Practices", "md_docs_2object__handles.html#autotoc_md269", null ],
      [ "Optional: Handle Resolution Flow (reference)", "md_docs_2object__handles.html#autotoc_md271", null ]
    ] ],
    [ "Project Description and Development Progress", "md_docs_2progress.html", [
      [ "Scripting and Configuration", "md_docs_2progress.html#autotoc_md273", null ],
      [ "Progress Updates", "md_docs_2progress.html#autotoc_md274", [
        [ "[July 2, 2025]", "md_docs_2progress.html#autotoc_md275", null ],
        [ "[August 5, 2025]", "md_docs_2progress.html#autotoc_md277", null ],
        [ "[September 19, 2025]", "md_docs_2progress.html#autotoc_md279", null ],
        [ "[September 20, 2025]", "md_docs_2progress.html#autotoc_md281", null ],
        [ "[September 21, 2025]", "md_docs_2progress.html#autotoc_md283", null ],
        [ "[September 22, 2025]", "md_docs_2progress.html#autotoc_md285", [
          [ "[September 23, 2025]", "md_docs_2progress.html#autotoc_md287", null ]
        ] ],
        [ "[September 24, 2025]", "md_docs_2progress.html#autotoc_md289", null ],
        [ "[September 25, 2025]", "md_docs_2progress.html#autotoc_md291", null ],
        [ "[September 26, 2025]", "md_docs_2progress.html#autotoc_md293", null ],
        [ "[September 27, 2025]", "md_docs_2progress.html#autotoc_md295", null ],
        [ "[September 28, 2025]", "md_docs_2progress.html#autotoc_md297", null ],
        [ "[September 29, 2025]", "md_docs_2progress.html#autotoc_md299", null ],
        [ "[September 30, 2025]", "md_docs_2progress.html#autotoc_md301", null ],
        [ "[October 2, 2025]", "md_docs_2progress.html#autotoc_md303", null ],
        [ "[October 3, 2025]", "md_docs_2progress.html#autotoc_md305", null ],
        [ "[October 4, 2025]", "md_docs_2progress.html#autotoc_md307", null ],
        [ "[October 5, 2025]", "md_docs_2progress.html#autotoc_md309", null ],
        [ "[October 6, 2025]", "md_docs_2progress.html#autotoc_md311", null ],
        [ "[October 7, 2025]", "md_docs_2progress.html#autotoc_md313", null ],
        [ "[October 8, 2025]", "md_docs_2progress.html#autotoc_md315", null ],
        [ "[October 9, 2025]", "md_docs_2progress.html#autotoc_md317", null ],
        [ "[October 11, 2025]", "md_docs_2progress.html#autotoc_md319", null ],
        [ "[October 12, 2025]", "md_docs_2progress.html#autotoc_md321", null ],
        [ "[October 13, 2025]", "md_docs_2progress.html#autotoc_md323", null ],
        [ "[October 14, 2025]", "md_docs_2progress.html#autotoc_md325", null ],
        [ "[October 15, 2025]", "md_docs_2progress.html#autotoc_md327", null ],
        [ "Next Steps:", "md_docs_2progress.html#autotoc_md329", null ],
        [ "ToDo:", "md_docs_2progress.html#autotoc_md331", null ]
      ] ]
    ] ],
    [ "Scripting and Configuration (Lua &amp; C++)", "md_docs_2scripting__and__configuration.html", [
      [ "Goals", "md_docs_2scripting__and__configuration.html#autotoc_md334", null ],
      [ "Modes of Use", "md_docs_2scripting__and__configuration.html#autotoc_md335", null ],
      [ "Integration Points", "md_docs_2scripting__and__configuration.html#autotoc_md336", null ],
      [ "Example: Creating with Lua", "md_docs_2scripting__and__configuration.html#autotoc_md337", null ],
      [ "Safety and Performance", "md_docs_2scripting__and__configuration.html#autotoc_md338", null ],
      [ "Source of Truth", "md_docs_2scripting__and__configuration.html#autotoc_md339", null ],
      [ "SDOM API considerations for Lua", "md_docs_2scripting__and__configuration.html#autotoc_md340", null ],
      [ "Next steps (Lua integration)", "md_docs_2scripting__and__configuration.html#autotoc_md341", null ],
      [ "Module-style configuration and lifecycle registration (recommended)", "md_docs_2scripting__and__configuration.html#autotoc_md342", null ]
    ] ],
    [ "Startup Refactor Proposal", "md_docs_2startup__refactor.html", [
      [ "Step-by-Step Refactoring Strategy", "md_docs_2startup__refactor.html#autotoc_md350", [
        [ "1. Explicit Initialization Pattern", "md_docs_2startup__refactor.html#autotoc_md344", null ],
        [ "2. Remove Automatic Table Parsing", "md_docs_2startup__refactor.html#autotoc_md345", null ],
        [ "3. Separate Config Parsing from Resource Creation", "md_docs_2startup__refactor.html#autotoc_md346", null ],
        [ "4. Update Documentation and Examples", "md_docs_2startup__refactor.html#autotoc_md347", null ],
        [ "1. Deprecate Automatic Table Parsing", "md_docs_2startup__refactor.html#autotoc_md351", null ],
        [ "2. Introduce Explicit Initialization Methods", "md_docs_2startup__refactor.html#autotoc_md352", null ],
        [ "3. Bind Engine Functions as Lua Globals", "md_docs_2startup__refactor.html#autotoc_md353", null ],
        [ "4. Maintain Legacy Support During Transition", "md_docs_2startup__refactor.html#autotoc_md354", null ],
        [ "5. Refactor Resource Creation Logic", "md_docs_2startup__refactor.html#autotoc_md355", null ],
        [ "6. Update Documentation and Examples", "md_docs_2startup__refactor.html#autotoc_md356", null ],
        [ "7. Test and Validate", "md_docs_2startup__refactor.html#autotoc_md357", null ]
      ] ]
    ] ],
    [ "SDOM: Simple SDL Document Object Model API", "md_README.html", [
      [ "Overview", "md_README.html#autotoc_md361", null ],
      [ "Features", "md_README.html#autotoc_md362", null ],
      [ "IDataObject: Data-Driven Base Type", "md_README.html#autotoc_md363", [
        [ "Key Features", "md_README.html#autotoc_md364", null ]
      ] ],
      [ "Identifier Conventions", "md_README.html#autotoc_md365", null ],
      [ "Getting Started", "md_README.html#autotoc_md367", [
        [ "Prerequisites", "md_README.html#autotoc_md368", null ],
        [ "Building the SDOM Library", "md_README.html#autotoc_md369", null ],
        [ "Running the Example", "md_README.html#autotoc_md370", null ],
        [ "Notable recent changes", "md_README.html#autotoc_md371", null ],
        [ "Migration notes", "md_README.html#autotoc_md372", null ]
      ] ],
      [ "Project Structure", "md_README.html#autotoc_md373", null ],
      [ "SDL3 Build Dependencies", "md_README.html#autotoc_md374", null ],
      [ "Debian Build Dependencies", "md_README.html#autotoc_md375", null ],
      [ "Arch Build Dependencies", "md_README.html#autotoc_md376", null ],
      [ "Freetype on Debian:", "md_README.html#autotoc_md377", null ],
      [ "Building and Installing SDL3_mixer, SDL3_image, and SDL_ttf", "md_README.html#autotoc_md378", null ],
      [ "Build per SDL3:", "md_README.html#autotoc_md379", null ],
      [ "Build and install SDL3 libraries", "md_README.html#autotoc_md380", null ],
      [ "Lua and Sol2", "md_README.html#autotoc_md381", null ],
      [ "Documentation", "md_README.html#autotoc_md382", [
        [ "Exporting Mermaid Diagrams to PNG/SVG (for Doxygen)", "md_README.html#autotoc_md383", null ]
      ] ],
      [ "License", "md_README.html#autotoc_md384", null ],
      [ "Author", "md_README.html#autotoc_md385", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", "namespacemembers_func" ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", "functions_vars" ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"SDOM_8cpp.html",
"classSDOM_1_1AssetHandle.html#ab5c77313c1b036ba0148410a33d2985a",
"classSDOM_1_1Button.html#a38e4838139dc7a4faf0639df880812dd",
"classSDOM_1_1Button.html#ae7525c9e7db2bda1a850805dceee2504",
"classSDOM_1_1Checkbox.html#a8640caa502d5209d30159c593b59a475",
"classSDOM_1_1Core.html#a4b26de07efff540f121763c7574d6c49",
"classSDOM_1_1DisplayHandle.html#aa5431aa2ae62561212d2129cd4c86ddb",
"classSDOM_1_1EventType.html#a1711b7f6c22bf1886d475411121fdcc1",
"classSDOM_1_1Frame.html#a0acfe38923fa152340b9733da753545f",
"classSDOM_1_1Frame.html#ac452d690d98c233a22b72ab565ca83c3",
"classSDOM_1_1Group.html#a765c1a5a98db78263e66fa63a95711c0",
"classSDOM_1_1IAssetObject.html#acf24a17e1284129dbef77cfc229bba05",
"classSDOM_1_1IDisplayObject.html#aa1f11950c5bbe98f9a367c5d9887826c",
"classSDOM_1_1IPanelObject.html#a1947ff718446a6a81cd108b87a475d9f",
"classSDOM_1_1IPanelObject.html#acd625b31a2f66caeabb6848bb26065e1",
"classSDOM_1_1IconButton.html#a853fddbff86054aae8abc808cf55d1b6",
"classSDOM_1_1Label.html#a3c2910aa54834bd1fd44e456861832b2a45bd1d5b32931106efbf1a82fe6a732f",
"classSDOM_1_1Label.html#aacb83eae3c954d0f9a6a1b28b123ec6e",
"classSDOM_1_1Radiobox.html#a5934f6442c1a5ef0c39aaeab9ab59260",
"classSDOM_1_1Radiobox.html#aff4057454b6d7cb9a282c9d3b00a4777",
"classSDOM_1_1Stage.html#a451932c42a292a8c19f84a10e2db94f4",
"classSDOM_1_1TTFAsset.html",
"classSDOM_1_1TruetypeFont.html#ac46b92264fc31de65adb17ecb1985319",
"md_docs_2display__object__anchoring.html#autotoc_md79",
"namespaceSDOM.html#a24547b713443265c84f66fb691319dc9",
"namespaceSDOM.html#adf53bca06e8a020f771e843c14584582",
"structSDOM_1_1Checkbox_1_1InitStruct.html#ae874560eec66d299017bf5041a9b00a2",
"structSDOM_1_1IDisplayObject_1_1InitStruct.html#a2d89b34df7efd356fb474dfe984011e3",
"structSDOM_1_1Label_1_1InitStruct.html#ab41dcfc6259800a732c4eccd40452ae6"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';