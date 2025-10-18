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
      [ "Core Initialization Sequence & Pre-Initialization Configuration", "md_docs_2core__framework.html#autotoc_md7", [
        [ "Required Pre-Initialization Parameters:", "md_docs_2core__framework.html#autotoc_md8", null ],
        [ "Initialization Flow:", "md_docs_2core__framework.html#autotoc_md9", null ],
        [ "Example Configuration API:", "md_docs_2core__framework.html#autotoc_md10", null ],
        [ "Alternative Initialization: Lua Configuration", "md_docs_2core__framework.html#autotoc_md12", null ]
      ] ],
      [ "Primary Callbacks", "md_docs_2core__framework.html#autotoc_md14", null ],
      [ "Hook & Callback Registration", "md_docs_2core__framework.html#autotoc_md15", [
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
      [ "Extensibility & Singleton Pattern", "md_docs_2core__framework.html#autotoc_md27", null ],
      [ "EventListener Support", "md_docs_2core__framework.html#autotoc_md28", [
        [ "Example (C)", "md_docs_2core__framework.html#autotoc_md29", null ],
        [ "Benefits", "md_docs_2core__framework.html#autotoc_md30", null ]
      ] ],
      [ "Registering Custom Stage Objects", "md_docs_2core__framework.html#autotoc_md31", [
        [ "Example", "md_docs_2core__framework.html#autotoc_md32", null ]
      ] ],
      [ "Integration Methods Summary", "md_docs_2core__framework.html#autotoc_md33", [
        [ "Flexibility & Use Cases", "md_docs_2core__framework.html#autotoc_md34", null ]
      ] ],
      [ "Performance Considerations", "md_docs_2core__framework.html#autotoc_md35", null ],
      [ "Single Stage, Single Window Rationale", "md_docs_2core__framework.html#autotoc_md36", null ],
      [ "Overriding the Default Stage", "md_docs_2core__framework.html#autotoc_md37", null ],
      [ "Multiple Stages as Scenes", "md_docs_2core__framework.html#autotoc_md38", null ],
      [ "Window Resizing & Texture Resolution", "md_docs_2core__framework.html#autotoc_md39", null ],
      [ "Shared Display Texture for Stages", "md_docs_2core__framework.html#autotoc_md40", null ],
      [ "Stages as Game States or Modes", "md_docs_2core__framework.html#autotoc_md41", null ],
      [ "Changing the Active Stage", "md_docs_2core__framework.html#autotoc_md42", null ]
    ] ],
    [ "Display Object Anchoring System", "md_docs_2display__object__anchoring.html", [
      [ "Overview", "md_docs_2display__object__anchoring.html#autotoc_md52", null ],
      [ "Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md53", [
        [ "Anchor Naming Conventions", "md_docs_2display__object__anchoring.html#autotoc_md45", null ],
        [ "Edge-Based Layout", "md_docs_2display__object__anchoring.html#autotoc_md46", null ],
        [ "Position and Size Calculation", "md_docs_2display__object__anchoring.html#autotoc_md47", null ],
        [ "Anchor Accessors/Mutators", "md_docs_2display__object__anchoring.html#autotoc_md48", null ],
        [ "Changing Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md49", null ],
        [ "Drag-and-Drop and Reparenting", "md_docs_2display__object__anchoring.html#autotoc_md50", null ],
        [ "Summary", "md_docs_2display__object__anchoring.html#autotoc_md51", null ],
        [ "Reversed versions for user convenience", "md_docs_2display__object__anchoring.html#autotoc_md54", null ]
      ] ],
      [ "Lua Interface", "md_docs_2display__object__anchoring.html#autotoc_md55", null ],
      [ "Position and Size Calculation", "md_docs_2display__object__anchoring.html#autotoc_md56", null ],
      [ "Changing Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md57", null ],
      [ "Hierarchical Anchoring", "md_docs_2display__object__anchoring.html#autotoc_md58", null ],
      [ "Backward Compatibility & Migration", "md_docs_2display__object__anchoring.html#autotoc_md59", null ],
      [ "Benefits", "md_docs_2display__object__anchoring.html#autotoc_md60", null ],
      [ "Example Use Cases", "md_docs_2display__object__anchoring.html#autotoc_md61", null ],
      [ "Anchoring Diagrams", "md_docs_2display__object__anchoring.html#autotoc_md64", [
        [ "1) Parent Anchor Grid and Child Edge Anchors", "md_docs_2display__object__anchoring.html#autotoc_md65", null ],
        [ "2) Anchor Change Recalculation (No Visual Jump)", "md_docs_2display__object__anchoring.html#autotoc_md66", null ]
      ] ],
      [ "Anchor helpers & parsing", "md_docs_2display__object__anchoring.html#autotoc_md67", null ],
      [ "Migration note", "md_docs_2display__object__anchoring.html#autotoc_md68", null ]
    ] ],
    [ "DOM Propagation Design", "md_docs_2dom__propagation.html", [
      [ "Overview", "md_docs_2dom__propagation.html#autotoc_md70", null ],
      [ "Event Propagation Diagram", "md_docs_2dom__propagation.html#autotoc_md72", null ],
      [ "Traversal Types", "md_docs_2dom__propagation.html#autotoc_md73", [
        [ "1. Core-based DOM Traversal (Type 1)", "md_docs_2dom__propagation.html#autotoc_md74", null ],
        [ "2. EventManager-based Event Traversal (Type 2)", "md_docs_2dom__propagation.html#autotoc_md76", null ],
        [ "3. Factory-based Global Traversal (Type 3)", "md_docs_2dom__propagation.html#autotoc_md78", null ],
        [ "4. World Coordinate Backpropagation Traversal (Type 4)", "md_docs_2dom__propagation.html#autotoc_md79", null ]
      ] ],
      [ "Traversal Safety and Modification", "md_docs_2dom__propagation.html#autotoc_md80", [
        [ "Deferred actions processing (pseudo)", "md_docs_2dom__propagation.html#autotoc_md81", null ],
        [ "Thread-safety notes", "md_docs_2dom__propagation.html#autotoc_md82", null ]
      ] ],
      [ "Architectural Separation", "md_docs_2dom__propagation.html#autotoc_md83", null ],
      [ "Migration note", "md_docs_2dom__propagation.html#autotoc_md84", null ]
    ] ],
    [ "SDOM::Events Design Document", "md_docs_2event__design.html", [
      [ "Overview", "md_docs_2event__design.html#autotoc_md86", null ],
      [ "Goals", "md_docs_2event__design.html#autotoc_md87", null ],
      [ "Event Structure", "md_docs_2event__design.html#autotoc_md88", null ],
      [ "Event Lifecycle", "md_docs_2event__design.html#autotoc_md89", null ],
      [ "Event Types", "md_docs_2event__design.html#autotoc_md90", null ],
      [ "Predefined Event Types", "md_docs_2event__design.html#autotoc_md91", [
        [ "General Events", "md_docs_2event__design.html#autotoc_md92", null ],
        [ "Mouse Events", "md_docs_2event__design.html#autotoc_md93", null ],
        [ "Keyboard Events", "md_docs_2event__design.html#autotoc_md94", null ],
        [ "Timer Events", "md_docs_2event__design.html#autotoc_md95", null ],
        [ "UI Events", "md_docs_2event__design.html#autotoc_md96", null ],
        [ "Drag & Drop Events", "md_docs_2event__design.html#autotoc_md97", null ],
        [ "Clipboard Events", "md_docs_2event__design.html#autotoc_md98", null ],
        [ "Custom Events", "md_docs_2event__design.html#autotoc_md99", null ]
      ] ],
      [ "EventManager Responsibilities", "md_docs_2event__design.html#autotoc_md100", null ],
      [ "Existing Implementation", "md_docs_2event__design.html#autotoc_md101", null ],
      [ "Next Steps", "md_docs_2event__design.html#autotoc_md102", null ],
      [ "Notes", "md_docs_2event__design.html#autotoc_md103", null ],
      [ "Event System Design: Categorizing EventTypes", "md_docs_2event__design.html#autotoc_md105", [
        [ "Proposal", "md_docs_2event__design.html#autotoc_md106", null ],
        [ "Advantages", "md_docs_2event__design.html#autotoc_md107", null ],
        [ "Suggested Implementation", "md_docs_2event__design.html#autotoc_md108", null ],
        [ "Integration", "md_docs_2event__design.html#autotoc_md109", null ],
        [ "Considerations", "md_docs_2event__design.html#autotoc_md110", [
          [ "Listener-Specific Events", "md_docs_2event__design.html#autotoc_md112", [
            [ "Example Behavior", "md_docs_2event__design.html#autotoc_md113", null ],
            [ "Integration", "md_docs_2event__design.html#autotoc_md114", null ]
          ] ]
        ] ]
      ] ]
    ] ],
    [ "Extensible Factory Design", "md_docs_2extensible__factory.html", [
      [ "Factory Overview (Diagram)", "md_docs_2extensible__factory.html#autotoc_md117", null ],
      [ "Resource Type Management", "md_docs_2extensible__factory.html#autotoc_md118", null ],
      [ "Requirements", "md_docs_2extensible__factory.html#autotoc_md119", null ],
      [ "Recommended Approach", "md_docs_2extensible__factory.html#autotoc_md120", [
        [ "1. Initializer Structs and Lua Scripts", "md_docs_2extensible__factory.html#autotoc_md121", null ],
        [ "3. Factory Create Methods", "md_docs_2extensible__factory.html#autotoc_md122", null ],
        [ "4. Usage", "md_docs_2extensible__factory.html#autotoc_md123", null ]
      ] ],
      [ "Benefits", "md_docs_2extensible__factory.html#autotoc_md124", null ],
      [ "Implementation Strategy", "md_docs_2extensible__factory.html#autotoc_md126", [
        [ "1. Initializer Structs", "md_docs_2extensible__factory.html#autotoc_md127", null ],
        [ "1a. Standardized Constructor Signatures", "md_docs_2extensible__factory.html#autotoc_md128", null ],
        [ "2. Private Constructors & Friend Factory", "md_docs_2extensible__factory.html#autotoc_md129", null ],
        [ "3. Factory Registration System", "md_docs_2extensible__factory.html#autotoc_md130", null ],
        [ "4. Generic Factory Create Method", "md_docs_2extensible__factory.html#autotoc_md131", null ],
        [ "5. Extensibility", "md_docs_2extensible__factory.html#autotoc_md132", null ],
        [ "6. Error Handling & Validation", "md_docs_2extensible__factory.html#autotoc_md133", null ],
        [ "7. Example Usage", "md_docs_2extensible__factory.html#autotoc_md134", null ]
      ] ],
      [ "Standardized Constructors for IDisplayObjects", "md_docs_2extensible__factory.html#autotoc_md136", null ],
      [ "Scripting and Configuration Choice", "md_docs_2extensible__factory.html#autotoc_md137", null ],
      [ "Resource Pointer (<tt>resource_ptr</tt>) Design", "md_docs_2extensible__factory.html#autotoc_md138", [
        [ "How It Works", "md_docs_2extensible__factory.html#autotoc_md139", null ],
        [ "Example Usage", "md_docs_2extensible__factory.html#autotoc_md140", null ],
        [ "Benefits", "md_docs_2extensible__factory.html#autotoc_md141", null ],
        [ "Implementation Outline", "md_docs_2extensible__factory.html#autotoc_md142", null ],
        [ "Summary", "md_docs_2extensible__factory.html#autotoc_md143", null ]
      ] ]
    ] ],
    [ "Icon Index Catalog for sprite_8x8 Font", "md_docs_2icon__8x8.html", [
      [ "Icon Glyphs", "md_docs_2icon__8x8.html#autotoc_md145", null ],
      [ "Icon Sets", "md_docs_2icon__8x8.html#autotoc_md146", [
        [ "Sets", "md_docs_2icon__8x8.html#autotoc_md147", null ]
      ] ],
      [ "Customization Potential", "md_docs_2icon__8x8.html#autotoc_md148", [
        [ "Extensibility", "md_docs_2icon__8x8.html#autotoc_md149", null ]
      ] ]
    ] ],
    [ "Design Goals for <tt>IPanelObject</tt>", "md_docs_2IPanelObject.html", [
      [ "IPanelObject Design Document", "md_docs_2IPanelObject.html#autotoc_md151", [
        [ "What is a 9-slice (9-patch) panel?", "md_docs_2IPanelObject.html#autotoc_md152", null ],
        [ "What is a 9-slice (9-patch) panel?", "md_docs_2IPanelObject.html#autotoc_md153", null ],
        [ "Reference: sprite_8x8 Icon Index Catalog", "md_docs_2IPanelObject.html#autotoc_md154", [
          [ "Predefined 9-Slice Sets in <tt>sprite_8x8</tt>", "md_docs_2IPanelObject.html#autotoc_md155", null ]
        ] ],
        [ "Suggested Steps", "md_docs_2IPanelObject.html#autotoc_md157", null ],
        [ "Interface Sketch", "md_docs_2IPanelObject.html#autotoc_md159", null ],
        [ "Note on Button Objects and State Handling", "md_docs_2IPanelObject.html#autotoc_md160", null ]
      ] ]
    ] ],
    [ "Label Text Parsing Design Document", "md_docs_2label__text__parsing.html", [
      [ "Overview", "md_docs_2label__text__parsing.html#autotoc_md162", null ],
      [ "Goals", "md_docs_2label__text__parsing.html#autotoc_md164", null ],
      [ "Retro-Inspired Graphic Glyphs", "md_docs_2label__text__parsing.html#autotoc_md166", [
        [ "Customization Potential", "md_docs_2label__text__parsing.html#autotoc_md167", null ]
      ] ],
      [ "Escape Sequence Syntax", "md_docs_2label__text__parsing.html#autotoc_md168", [
        [ "Supported Color Names", "md_docs_2label__text__parsing.html#autotoc_md170", [
          [ "Color Targets:", "md_docs_2label__text__parsing.html#autotoc_md171", null ]
        ] ],
        [ "Style", "md_docs_2label__text__parsing.html#autotoc_md172", [
          [ "Colors:", "md_docs_2label__text__parsing.html#autotoc_md173", null ]
        ] ],
        [ "Alignment", "md_docs_2label__text__parsing.html#autotoc_md174", null ],
        [ "Quick reference", "md_docs_2label__text__parsing.html#autotoc_md175", null ],
        [ "Alignment", "md_docs_2label__text__parsing.html#autotoc_md177", null ],
        [ "Quick reference", "md_docs_2label__text__parsing.html#autotoc_md178", [
          [ "Bitfield Encoding for Alignment", "md_docs_2label__text__parsing.html#autotoc_md179", null ]
        ] ],
        [ "Special", "md_docs_2label__text__parsing.html#autotoc_md180", null ],
        [ "Escaping Square Brackets", "md_docs_2label__text__parsing.html#autotoc_md181", null ],
        [ "Font Size", "md_docs_2label__text__parsing.html#autotoc_md182", null ]
      ] ],
      [ "Numeric Style Escapes (border / outline / padding / dropshadow)", "md_docs_2label__text__parsing.html#autotoc_md184", [
        [ "Overview", "md_docs_2label__text__parsing.html#autotoc_md185", null ],
        [ "Syntax", "md_docs_2label__text__parsing.html#autotoc_md186", null ],
        [ "Semantics", "md_docs_2label__text__parsing.html#autotoc_md187", null ],
        [ "Examples", "md_docs_2label__text__parsing.html#autotoc_md188", null ],
        [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md189", null ],
        [ "Test seed suggestion (Label_test5)", "md_docs_2label__text__parsing.html#autotoc_md190", null ]
      ] ],
      [ "Token Structure", "md_docs_2label__text__parsing.html#autotoc_md192", [
        [ "Word Tokenization Rules", "md_docs_2label__text__parsing.html#autotoc_md193", [
          [ "Example tokenization:", "md_docs_2label__text__parsing.html#autotoc_md194", null ]
        ] ],
        [ "Escape Tokenization Rules", "md_docs_2label__text__parsing.html#autotoc_md195", null ]
      ] ],
      [ "Parsing Algorithm", "md_docs_2label__text__parsing.html#autotoc_md197", null ],
      [ "Alignment Handling", "md_docs_2label__text__parsing.html#autotoc_md199", null ],
      [ "Word Wrapping & Autosizing", "md_docs_2label__text__parsing.html#autotoc_md201", null ],
      [ "Rendering", "md_docs_2label__text__parsing.html#autotoc_md203", null ],
      [ "Extensibility", "md_docs_2label__text__parsing.html#autotoc_md205", null ],
      [ "Example", "md_docs_2label__text__parsing.html#autotoc_md207", null ],
      [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md209", [
        [ "Label Sizing and Wrapping Rules", "md_docs_2label__text__parsing.html#autotoc_md210", null ],
        [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md211", null ]
      ] ],
      [ "Rendering Phases (Functions):", "md_docs_2label__text__parsing.html#autotoc_md212", [
        [ "1. Pre-Render Layout & Bounds Adjustment", "md_docs_2label__text__parsing.html#autotoc_md213", null ],
        [ "2. Phrase Grouping", "md_docs_2label__text__parsing.html#autotoc_md214", null ],
        [ "3. Phrase Rendering", "md_docs_2label__text__parsing.html#autotoc_md215", null ]
      ] ],
      [ "ASCII Glyphs", "md_docs_2label__text__parsing.html#autotoc_md217", null ],
      [ "Graphic Glyphs", "md_docs_2label__text__parsing.html#autotoc_md218", null ],
      [ "References", "md_docs_2label__text__parsing.html#autotoc_md220", null ]
    ] ],
    [ "Lua commands and properties exposed by SDOM (C++ → Lua binding inventory)", "md_docs_2lua__commands__and__properties.html", [
      [ "Globals / Core (lua <tt>Core</tt> / <tt>CoreForward</tt>)", "md_docs_2lua__commands__and__properties.html#autotoc_md223", null ],
      [ "DisplayHandle methods (registered by <tt>IDisplayObject::_registerLuaBindings</tt>)", "md_docs_2lua__commands__and__properties.html#autotoc_md224", null ],
      [ "Exhaustive DisplayHandle mapping (line-by-line)", "md_docs_2lua__commands__and__properties.html#autotoc_md225", null ],
      [ "AssetHandle and per-asset augmentations", "md_docs_2lua__commands__and__properties.html#autotoc_md226", null ],
      [ "Event and EventType bindings", "md_docs_2lua__commands__and__properties.html#autotoc_md227", null ],
      [ "SDL helpers (lua <tt>SDL_Utils</tt> / <tt>SDL</tt>)", "md_docs_2lua__commands__and__properties.html#autotoc_md228", null ],
      [ "Binding helper summary (src/lua_BindHelpers.cpp)", "md_docs_2lua__commands__and__properties.html#autotoc_md229", null ],
      [ "Remaining work / completeness notes", "md_docs_2lua__commands__and__properties.html#autotoc_md230", null ]
    ] ],
    [ "Object Handles in SDOM", "md_docs_2object__handles.html", [
      [ "Two Trees in the Garden", "md_docs_2object__handles.html#autotoc_md235", null ],
      [ "Overview", "md_docs_2object__handles.html#autotoc_md236", null ],
      [ "Handle Types", "md_docs_2object__handles.html#autotoc_md237", [
        [ "1. DisplayHandle", "md_docs_2object__handles.html#autotoc_md238", null ],
        [ "2. AssetHandle", "md_docs_2object__handles.html#autotoc_md239", null ]
      ] ],
      [ "Design Rationale", "md_docs_2object__handles.html#autotoc_md240", null ],
      [ "Inheritance Warning: Avoid Diamond Inheritance", "md_docs_2object__handles.html#autotoc_md242", null ],
      [ "Example Usage", "md_docs_2object__handles.html#autotoc_md243", null ],
      [ "Migration Strategy", "md_docs_2object__handles.html#autotoc_md244", null ],
      [ "Best Practices", "md_docs_2object__handles.html#autotoc_md245", null ],
      [ "Optional: Handle Resolution Flow (reference)", "md_docs_2object__handles.html#autotoc_md247", null ]
    ] ],
    [ "Project Description and Development Progress", "md_docs_2progress.html", [
      [ "Scripting and Configuration", "md_docs_2progress.html#autotoc_md249", null ],
      [ "Progress Updates", "md_docs_2progress.html#autotoc_md250", [
        [ "[July 2, 2025]", "md_docs_2progress.html#autotoc_md251", null ],
        [ "[August 5, 2025]", "md_docs_2progress.html#autotoc_md253", null ],
        [ "[September 19, 2025]", "md_docs_2progress.html#autotoc_md255", null ],
        [ "[September 20, 2025]", "md_docs_2progress.html#autotoc_md257", null ],
        [ "[September 21, 2025]", "md_docs_2progress.html#autotoc_md259", null ],
        [ "[September 22, 2025]", "md_docs_2progress.html#autotoc_md261", [
          [ "[September 23, 2025]", "md_docs_2progress.html#autotoc_md263", null ]
        ] ],
        [ "[September 24, 2025]", "md_docs_2progress.html#autotoc_md265", null ],
        [ "[September 25, 2025]", "md_docs_2progress.html#autotoc_md267", null ],
        [ "[September 26, 2025]", "md_docs_2progress.html#autotoc_md269", null ],
        [ "[September 27, 2025]", "md_docs_2progress.html#autotoc_md271", null ],
        [ "[September 28, 2025]", "md_docs_2progress.html#autotoc_md273", null ],
        [ "[September 29, 2025]", "md_docs_2progress.html#autotoc_md275", null ],
        [ "[September 30, 2025]", "md_docs_2progress.html#autotoc_md277", null ],
        [ "[October 2, 2025]", "md_docs_2progress.html#autotoc_md279", null ],
        [ "[October 3, 2025]", "md_docs_2progress.html#autotoc_md281", null ],
        [ "[October 4, 2025]", "md_docs_2progress.html#autotoc_md283", null ],
        [ "[October 5, 2025]", "md_docs_2progress.html#autotoc_md285", null ],
        [ "[October 6, 2025]", "md_docs_2progress.html#autotoc_md287", null ],
        [ "[October 7, 2025]", "md_docs_2progress.html#autotoc_md289", null ],
        [ "[October 8, 2025]", "md_docs_2progress.html#autotoc_md291", null ],
        [ "[October 9, 2025]", "md_docs_2progress.html#autotoc_md293", null ],
        [ "[October 11, 2025]", "md_docs_2progress.html#autotoc_md295", null ],
        [ "[October 12, 2025]", "md_docs_2progress.html#autotoc_md297", null ],
        [ "[October 13, 2025]", "md_docs_2progress.html#autotoc_md299", null ],
        [ "[October 14, 2025]", "md_docs_2progress.html#autotoc_md301", null ],
        [ "[October 15, 2025]", "md_docs_2progress.html#autotoc_md303", null ],
        [ "[October 16, 2025]", "md_docs_2progress.html#autotoc_md305", null ],
        [ "[October 17, 2025]", "md_docs_2progress.html#autotoc_md307", null ],
        [ "Next Steps:", "md_docs_2progress.html#autotoc_md309", null ],
        [ "Long Term To Do:", "md_docs_2progress.html#autotoc_md311", null ]
      ] ]
    ] ],
    [ "Scripting and Configuration (Lua & C++)", "md_docs_2scripting__and__configuration.html", [
      [ "Goals", "md_docs_2scripting__and__configuration.html#autotoc_md313", null ],
      [ "Modes of Use", "md_docs_2scripting__and__configuration.html#autotoc_md314", null ],
      [ "Integration Points", "md_docs_2scripting__and__configuration.html#autotoc_md315", null ],
      [ "Example: Creating with Lua", "md_docs_2scripting__and__configuration.html#autotoc_md316", null ],
      [ "Safety and Performance", "md_docs_2scripting__and__configuration.html#autotoc_md317", null ],
      [ "Source of Truth", "md_docs_2scripting__and__configuration.html#autotoc_md318", null ],
      [ "SDOM API considerations for Lua", "md_docs_2scripting__and__configuration.html#autotoc_md319", null ],
      [ "Next steps (Lua integration)", "md_docs_2scripting__and__configuration.html#autotoc_md320", null ],
      [ "Module-style configuration and lifecycle registration (recommended)", "md_docs_2scripting__and__configuration.html#autotoc_md321", null ]
    ] ],
    [ "Startup Refactor Proposal", "md_docs_2startup__refactor.html", [
      [ "Step-by-Step Refactoring Strategy", "md_docs_2startup__refactor.html#autotoc_md329", [
        [ "1. Explicit Initialization Pattern", "md_docs_2startup__refactor.html#autotoc_md323", null ],
        [ "2. Remove Automatic Table Parsing", "md_docs_2startup__refactor.html#autotoc_md324", null ],
        [ "3. Separate Config Parsing from Resource Creation", "md_docs_2startup__refactor.html#autotoc_md325", null ],
        [ "4. Update Documentation and Examples", "md_docs_2startup__refactor.html#autotoc_md326", null ],
        [ "1. Deprecate Automatic Table Parsing", "md_docs_2startup__refactor.html#autotoc_md330", null ],
        [ "2. Introduce Explicit Initialization Methods", "md_docs_2startup__refactor.html#autotoc_md331", null ],
        [ "3. Bind Engine Functions as Lua Globals", "md_docs_2startup__refactor.html#autotoc_md332", null ],
        [ "4. Maintain Legacy Support During Transition", "md_docs_2startup__refactor.html#autotoc_md333", null ],
        [ "5. Refactor Resource Creation Logic", "md_docs_2startup__refactor.html#autotoc_md334", null ],
        [ "6. Update Documentation and Examples", "md_docs_2startup__refactor.html#autotoc_md335", null ],
        [ "7. Test and Validate", "md_docs_2startup__refactor.html#autotoc_md336", null ]
      ] ]
    ] ],
    [ "SDOM: Simple SDL Document Object Model API", "md_README.html", [
      [ "Overview", "md_README.html#autotoc_md340", null ],
      [ "Features", "md_README.html#autotoc_md341", null ],
      [ "IDataObject: Data-Driven Base Type", "md_README.html#autotoc_md342", [
        [ "Key Features", "md_README.html#autotoc_md343", null ]
      ] ],
      [ "Identifier Conventions", "md_README.html#autotoc_md344", null ],
      [ "Getting Started", "md_README.html#autotoc_md346", [
        [ "Prerequisites", "md_README.html#autotoc_md347", null ],
        [ "Building the SDOM Library", "md_README.html#autotoc_md348", null ],
        [ "Running the Example", "md_README.html#autotoc_md349", null ],
        [ "Notable recent changes", "md_README.html#autotoc_md350", null ],
        [ "Migration notes", "md_README.html#autotoc_md351", null ]
      ] ],
      [ "Project Structure", "md_README.html#autotoc_md352", null ],
      [ "SDL3 Build Dependencies", "md_README.html#autotoc_md353", null ],
      [ "Debian Build Dependencies", "md_README.html#autotoc_md354", null ],
      [ "Arch Build Dependencies", "md_README.html#autotoc_md355", null ],
      [ "Freetype on Debian:", "md_README.html#autotoc_md356", null ],
      [ "Building and Installing SDL3_mixer, SDL3_image, and SDL_ttf", "md_README.html#autotoc_md357", null ],
      [ "Build per SDL3:", "md_README.html#autotoc_md358", null ],
      [ "Build and install SDL3 libraries", "md_README.html#autotoc_md359", null ],
      [ "Lua and Sol2", "md_README.html#autotoc_md360", null ],
      [ "Documentation", "md_README.html#autotoc_md361", [
        [ "Exporting Mermaid Diagrams to PNG/SVG (for Doxygen)", "md_README.html#autotoc_md362", null ]
      ] ],
      [ "License", "md_README.html#autotoc_md363", null ],
      [ "Author", "md_README.html#autotoc_md364", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", "namespacemembers_func" ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
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
"SDOM__RadioButton_8hpp.html",
"classSDOM_1_1ArrowButton.html#a4c8906ae32169a62be6fbe41d52704da",
"classSDOM_1_1ArrowButton.html#aea0425944ea126e8ee61f0b64dcdef72",
"classSDOM_1_1BitmapFont.html#ad0b85cd8eafe1b2f92a03573e0ea80b9",
"classSDOM_1_1Button.html#a90e2d7c4dd39c97d030aa80aabf7e048",
"classSDOM_1_1CheckButton.html#a2d49e519abc33993bbbc715b04ffc9dc",
"classSDOM_1_1CheckButton.html#ac60be3f13cda9e5fdf266be78317a34f",
"classSDOM_1_1Core.html#a9eb5a0500bb9fc36ec82cd14e05792e6",
"classSDOM_1_1Event.html#a4c92c87d8a80372b601b3b3917c2258f",
"classSDOM_1_1EventType.html#aa276368587363a849ed1a57a9a1e230a",
"classSDOM_1_1Frame.html#a3f70a5b8822ec45b06aea26d79978709",
"classSDOM_1_1Frame.html#aeaf04e629e7e3c8083126db2868ea669",
"classSDOM_1_1Group.html#a90e2d7c4dd39c97d030aa80aabf7e048",
"classSDOM_1_1IButtonObject.html#a9a08fd4fc25b8316f7fd3ee3ac57f16e",
"classSDOM_1_1IDisplayObject.html#aa1525c87c0e2d74e5838d69baba95ac6",
"classSDOM_1_1IPanelObject.html#a1068567d65e0780958ce3cfe104ebe4a",
"classSDOM_1_1IPanelObject.html#abd7be10060116e26935e894937d0e95b",
"classSDOM_1_1IRangeControl.html#a67a841d829d00c79ebb1e10eacb02974",
"classSDOM_1_1IRangeControl__scaffold.html#a17d740892cc4499713a8fc38632f5e92",
"classSDOM_1_1IRangeControl__scaffold.html#abda87b944bd12092c524a44fc2f534c2",
"classSDOM_1_1IconButton.html#a6c2d2b40744947b37db8974dc783d5c1",
"classSDOM_1_1Label.html#a1947ff718446a6a81cd108b87a475d9f",
"classSDOM_1_1Label.html#a985007476d5838661335a7e1017f0b3a",
"classSDOM_1_1ProgressBar.html#a255e103195aec13146fd7b2341546c2a",
"classSDOM_1_1ProgressBar.html#ac921a4ea3bd56050090f9a185bb397d6",
"classSDOM_1_1RadioButton.html#a6ce8ddfbf93a2dfc287800dbfc4cbfbb",
"classSDOM_1_1SDL__Utils.html#a027dc0dbe963b494fba4ceee1688bf2d",
"classSDOM_1_1ScrollBar.html#a8bf31785ce47d1bcc093e9d9db5b5c48",
"classSDOM_1_1Slider.html#a331c632901b4ccec465aa3d45d005af4",
"classSDOM_1_1Slider.html#addaf39525119d04958b868e104170fb6",
"classSDOM_1_1Stage.html#a32aeb8fee010afd80a0fc0ed0d50faed",
"classSDOM_1_1Stage.html#ae3d0f33be77fd218d52d75316045f663",
"classSDOM_1_1TristateButton.html#a25ad8dccf0a5b148e9ec31e10bea337e",
"classSDOM_1_1TristateButton.html#abeaf8c6088be55a34e21c428b2974164",
"classSDOM_1_1TruetypeFont.html#af975331dcde3f417bc8c0dd87d13e14f",
"lua__IDisplayObject_8cpp.html#a0193b640ad77f143e73ef3f5af801cd4",
"md_docs_2core__framework.html#autotoc_md15",
"md_docs_2startup__refactor.html#autotoc_md335",
"namespaceSDOM.html#ad54c547716db63522c1d3d506059a151",
"structSDOM_1_1Button_1_1InitStruct.html#a0c5a6c60a33488ee178e2881107a2d92",
"structSDOM_1_1Frame_1_1InitStruct.html#a952800d26de94058a2303a0a0b497fcc",
"structSDOM_1_1IRangeControl_1_1InitStruct.html#a017528495a4f046c6242a628e3ef3b69",
"structSDOM_1_1Label_1_1InitStruct.html#ac3b4c19819ff87fd7886678ef807edcf",
"structSDOM_1_1ScrollBar_1_1InitStruct.html#aeb15851ce78d0eccfab25834efedddff",
"structSDOM_1_1TypeCreators.html#a15807ffd6629c821a869c22bbbd8d180"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';