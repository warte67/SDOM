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
        [ "Style", "md_docs_2label__text__parsing.html#autotoc_md172", null ]
      ] ],
      [ "Lua bindings: property-style vs legacy get/set functions", "md_docs_2label__text__parsing.html#autotoc_md173", [
        [ "Alignment", "md_docs_2label__text__parsing.html#autotoc_md175", null ],
        [ "Quick reference", "md_docs_2label__text__parsing.html#autotoc_md176", null ],
        [ "Alignment", "md_docs_2label__text__parsing.html#autotoc_md178", null ],
        [ "Quick reference", "md_docs_2label__text__parsing.html#autotoc_md179", [
          [ "Colors:", "md_docs_2label__text__parsing.html#autotoc_md174", null ],
          [ "Bitfield Encoding for Alignment", "md_docs_2label__text__parsing.html#autotoc_md180", null ]
        ] ],
        [ "Special", "md_docs_2label__text__parsing.html#autotoc_md181", null ],
        [ "Escaping Square Brackets", "md_docs_2label__text__parsing.html#autotoc_md182", null ],
        [ "Font Size", "md_docs_2label__text__parsing.html#autotoc_md183", null ]
      ] ],
      [ "Numeric Style Escapes (border / outline / padding / dropshadow)", "md_docs_2label__text__parsing.html#autotoc_md185", [
        [ "Overview", "md_docs_2label__text__parsing.html#autotoc_md186", null ],
        [ "Syntax", "md_docs_2label__text__parsing.html#autotoc_md187", null ],
        [ "Semantics", "md_docs_2label__text__parsing.html#autotoc_md188", null ],
        [ "Examples", "md_docs_2label__text__parsing.html#autotoc_md189", null ],
        [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md190", null ],
        [ "Test seed suggestion (Label_test5)", "md_docs_2label__text__parsing.html#autotoc_md191", null ]
      ] ],
      [ "Token Structure", "md_docs_2label__text__parsing.html#autotoc_md193", [
        [ "Word Tokenization Rules", "md_docs_2label__text__parsing.html#autotoc_md194", [
          [ "Example tokenization:", "md_docs_2label__text__parsing.html#autotoc_md195", null ]
        ] ],
        [ "Escape Tokenization Rules", "md_docs_2label__text__parsing.html#autotoc_md196", null ]
      ] ],
      [ "Parsing Algorithm", "md_docs_2label__text__parsing.html#autotoc_md198", null ],
      [ "Alignment Handling", "md_docs_2label__text__parsing.html#autotoc_md200", null ],
      [ "Word Wrapping & Autosizing", "md_docs_2label__text__parsing.html#autotoc_md202", null ],
      [ "Rendering", "md_docs_2label__text__parsing.html#autotoc_md204", null ],
      [ "Extensibility", "md_docs_2label__text__parsing.html#autotoc_md206", null ],
      [ "Example", "md_docs_2label__text__parsing.html#autotoc_md208", null ],
      [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md210", [
        [ "Label Sizing and Wrapping Rules", "md_docs_2label__text__parsing.html#autotoc_md211", null ],
        [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md212", null ]
      ] ],
      [ "Rendering Phases (Functions):", "md_docs_2label__text__parsing.html#autotoc_md213", [
        [ "1. Pre-Render Layout & Bounds Adjustment", "md_docs_2label__text__parsing.html#autotoc_md214", null ],
        [ "2. Phrase Grouping", "md_docs_2label__text__parsing.html#autotoc_md215", null ],
        [ "3. Phrase Rendering", "md_docs_2label__text__parsing.html#autotoc_md216", null ]
      ] ],
      [ "ASCII Glyphs", "md_docs_2label__text__parsing.html#autotoc_md218", null ],
      [ "Graphic Glyphs", "md_docs_2label__text__parsing.html#autotoc_md219", null ],
      [ "References", "md_docs_2label__text__parsing.html#autotoc_md221", null ]
    ] ],
    [ "Lua commands and properties exposed by SDOM (C++ → Lua binding inventory)", "md_docs_2lua__commands__and__properties.html", [
      [ "Globals / Core (lua <tt>Core</tt> / <tt>CoreForward</tt>)", "md_docs_2lua__commands__and__properties.html#autotoc_md224", null ],
      [ "DisplayHandle methods (registered by <tt>IDisplayObject::_registerLuaBindings</tt>)", "md_docs_2lua__commands__and__properties.html#autotoc_md225", null ],
      [ "Exhaustive DisplayHandle mapping (line-by-line)", "md_docs_2lua__commands__and__properties.html#autotoc_md226", null ],
      [ "AssetHandle and per-asset augmentations", "md_docs_2lua__commands__and__properties.html#autotoc_md227", null ],
      [ "Event and EventType bindings", "md_docs_2lua__commands__and__properties.html#autotoc_md228", null ],
      [ "SDL helpers (lua <tt>SDL_Utils</tt> / <tt>SDL</tt>)", "md_docs_2lua__commands__and__properties.html#autotoc_md229", null ],
      [ "Binding helper summary (src/lua_BindHelpers.cpp)", "md_docs_2lua__commands__and__properties.html#autotoc_md230", null ],
      [ "Remaining work / completeness notes", "md_docs_2lua__commands__and__properties.html#autotoc_md231", null ]
    ] ],
    [ "Object Handles in SDOM", "md_docs_2object__handles.html", [
      [ "Two Trees in the Garden", "md_docs_2object__handles.html#autotoc_md236", null ],
      [ "Overview", "md_docs_2object__handles.html#autotoc_md237", null ],
      [ "Handle Types", "md_docs_2object__handles.html#autotoc_md238", [
        [ "1. DisplayHandle", "md_docs_2object__handles.html#autotoc_md239", null ],
        [ "2. AssetHandle", "md_docs_2object__handles.html#autotoc_md240", null ]
      ] ],
      [ "Design Rationale", "md_docs_2object__handles.html#autotoc_md241", null ],
      [ "Inheritance Warning: Avoid Diamond Inheritance", "md_docs_2object__handles.html#autotoc_md243", null ],
      [ "Example Usage", "md_docs_2object__handles.html#autotoc_md244", null ],
      [ "Migration Strategy", "md_docs_2object__handles.html#autotoc_md245", null ],
      [ "Best Practices", "md_docs_2object__handles.html#autotoc_md246", null ],
      [ "Optional: Handle Resolution Flow (reference)", "md_docs_2object__handles.html#autotoc_md248", null ]
    ] ],
    [ "Project Description and Development Progress", "md_docs_2progress.html", [
      [ "Scripting and Configuration", "md_docs_2progress.html#autotoc_md250", null ],
      [ "Progress Updates", "md_docs_2progress.html#autotoc_md251", [
        [ "[July 2, 2025]", "md_docs_2progress.html#autotoc_md252", null ],
        [ "[August 5, 2025]", "md_docs_2progress.html#autotoc_md254", null ],
        [ "[September 19, 2025]", "md_docs_2progress.html#autotoc_md256", null ],
        [ "[September 20, 2025]", "md_docs_2progress.html#autotoc_md258", null ],
        [ "[September 21, 2025]", "md_docs_2progress.html#autotoc_md260", null ],
        [ "[September 22, 2025]", "md_docs_2progress.html#autotoc_md262", [
          [ "[September 23, 2025]", "md_docs_2progress.html#autotoc_md264", null ]
        ] ],
        [ "[September 24, 2025]", "md_docs_2progress.html#autotoc_md266", null ],
        [ "[September 25, 2025]", "md_docs_2progress.html#autotoc_md268", null ],
        [ "[September 26, 2025]", "md_docs_2progress.html#autotoc_md270", null ],
        [ "[September 27, 2025]", "md_docs_2progress.html#autotoc_md272", null ],
        [ "[September 28, 2025]", "md_docs_2progress.html#autotoc_md274", null ],
        [ "[September 29, 2025]", "md_docs_2progress.html#autotoc_md276", null ],
        [ "[September 30, 2025]", "md_docs_2progress.html#autotoc_md278", null ],
        [ "[October 2, 2025]", "md_docs_2progress.html#autotoc_md280", null ],
        [ "[October 3, 2025]", "md_docs_2progress.html#autotoc_md282", null ],
        [ "[October 4, 2025]", "md_docs_2progress.html#autotoc_md284", null ],
        [ "[October 5, 2025]", "md_docs_2progress.html#autotoc_md286", null ],
        [ "[October 6, 2025]", "md_docs_2progress.html#autotoc_md288", null ],
        [ "[October 7, 2025]", "md_docs_2progress.html#autotoc_md290", null ],
        [ "[October 8, 2025]", "md_docs_2progress.html#autotoc_md292", null ],
        [ "[October 9, 2025]", "md_docs_2progress.html#autotoc_md294", null ],
        [ "[October 11, 2025]", "md_docs_2progress.html#autotoc_md296", null ],
        [ "[October 12, 2025]", "md_docs_2progress.html#autotoc_md298", null ],
        [ "[October 13, 2025]", "md_docs_2progress.html#autotoc_md300", null ],
        [ "[October 14, 2025]", "md_docs_2progress.html#autotoc_md302", null ],
        [ "[October 15, 2025]", "md_docs_2progress.html#autotoc_md304", null ],
        [ "[October 16, 2025]", "md_docs_2progress.html#autotoc_md306", null ],
        [ "[October 17, 2025]", "md_docs_2progress.html#autotoc_md308", null ],
        [ "[October 18, 2025]", "md_docs_2progress.html#autotoc_md310", [
          [ "[October 19, 2025]", "md_docs_2progress.html#autotoc_md312", null ]
        ] ],
        [ "Next Steps:", "md_docs_2progress.html#autotoc_md314", null ],
        [ "Long Term To Do:", "md_docs_2progress.html#autotoc_md316", null ]
      ] ]
    ] ],
    [ "Scripting and Configuration (Lua & C++)", "md_docs_2scripting__and__configuration.html", [
      [ "Goals", "md_docs_2scripting__and__configuration.html#autotoc_md318", null ],
      [ "Modes of Use", "md_docs_2scripting__and__configuration.html#autotoc_md319", null ],
      [ "Integration Points", "md_docs_2scripting__and__configuration.html#autotoc_md320", null ],
      [ "Example: Creating with Lua", "md_docs_2scripting__and__configuration.html#autotoc_md321", null ],
      [ "Safety and Performance", "md_docs_2scripting__and__configuration.html#autotoc_md322", null ],
      [ "Source of Truth", "md_docs_2scripting__and__configuration.html#autotoc_md323", null ],
      [ "SDOM API considerations for Lua", "md_docs_2scripting__and__configuration.html#autotoc_md324", null ],
      [ "Next steps (Lua integration)", "md_docs_2scripting__and__configuration.html#autotoc_md325", null ],
      [ "Module-style configuration and lifecycle registration (recommended)", "md_docs_2scripting__and__configuration.html#autotoc_md326", null ]
    ] ],
    [ "Startup Refactor Proposal", "md_docs_2startup__refactor.html", [
      [ "Step-by-Step Refactoring Strategy", "md_docs_2startup__refactor.html#autotoc_md334", [
        [ "1. Explicit Initialization Pattern", "md_docs_2startup__refactor.html#autotoc_md328", null ],
        [ "2. Remove Automatic Table Parsing", "md_docs_2startup__refactor.html#autotoc_md329", null ],
        [ "3. Separate Config Parsing from Resource Creation", "md_docs_2startup__refactor.html#autotoc_md330", null ],
        [ "4. Update Documentation and Examples", "md_docs_2startup__refactor.html#autotoc_md331", null ],
        [ "1. Deprecate Automatic Table Parsing", "md_docs_2startup__refactor.html#autotoc_md335", null ],
        [ "2. Introduce Explicit Initialization Methods", "md_docs_2startup__refactor.html#autotoc_md336", null ],
        [ "3. Bind Engine Functions as Lua Globals", "md_docs_2startup__refactor.html#autotoc_md337", null ],
        [ "4. Maintain Legacy Support During Transition", "md_docs_2startup__refactor.html#autotoc_md338", null ],
        [ "5. Refactor Resource Creation Logic", "md_docs_2startup__refactor.html#autotoc_md339", null ],
        [ "6. Update Documentation and Examples", "md_docs_2startup__refactor.html#autotoc_md340", null ],
        [ "7. Test and Validate", "md_docs_2startup__refactor.html#autotoc_md341", null ]
      ] ]
    ] ],
    [ "SDOM: Simple SDL Document Object Model API", "md_README.html", [
      [ "Overview", "md_README.html#autotoc_md345", null ],
      [ "Features", "md_README.html#autotoc_md346", null ],
      [ "IDataObject: Data-Driven Base Type", "md_README.html#autotoc_md347", [
        [ "Key Features", "md_README.html#autotoc_md348", null ]
      ] ],
      [ "Identifier Conventions", "md_README.html#autotoc_md349", null ],
      [ "Getting Started", "md_README.html#autotoc_md351", [
        [ "Prerequisites", "md_README.html#autotoc_md352", null ],
        [ "Building the SDOM Library", "md_README.html#autotoc_md353", null ],
        [ "Running the Example", "md_README.html#autotoc_md354", null ],
        [ "Notable recent changes", "md_README.html#autotoc_md355", null ],
        [ "Migration notes", "md_README.html#autotoc_md356", null ]
      ] ],
      [ "Project Structure", "md_README.html#autotoc_md357", null ],
      [ "SDL3 Build Dependencies", "md_README.html#autotoc_md358", null ],
      [ "Debian Build Dependencies", "md_README.html#autotoc_md359", null ],
      [ "Arch Build Dependencies", "md_README.html#autotoc_md360", null ],
      [ "Freetype on Debian:", "md_README.html#autotoc_md361", null ],
      [ "Building and Installing SDL3_mixer, SDL3_image, and SDL_ttf", "md_README.html#autotoc_md362", null ],
      [ "Build per SDL3:", "md_README.html#autotoc_md363", null ],
      [ "Build and install SDL3 libraries", "md_README.html#autotoc_md364", null ],
      [ "Lua and Sol2", "md_README.html#autotoc_md365", null ],
      [ "Documentation", "md_README.html#autotoc_md366", [
        [ "Exporting Mermaid Diagrams to PNG/SVG (for Doxygen)", "md_README.html#autotoc_md367", null ]
      ] ],
      [ "License", "md_README.html#autotoc_md368", null ],
      [ "Author", "md_README.html#autotoc_md369", null ]
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
"SDOM__RadioButton_8cpp.html",
"classSDOM_1_1ArrowButton.html#a44654ae721dfab2be1b8bd574169ab09",
"classSDOM_1_1ArrowButton.html#ade893cf4fc691595cbc67c531b0fac14",
"classSDOM_1_1BitmapFont.html#abf9a0b8bf17fd03471910db1503720d2",
"classSDOM_1_1Button.html#a7f5754bdccbd440ab6083fead1daa7cf",
"classSDOM_1_1CheckButton.html#a1ab73450fbdc7b278a871ca14454c92f",
"classSDOM_1_1CheckButton.html#aade178775d0c9afbd0f3c49f8dfe02c6",
"classSDOM_1_1Core.html#a6c4854090319dbc681a275dada027e40",
"classSDOM_1_1Event.html#a080aac422b33308070477a6c1fc2c6d6",
"classSDOM_1_1EventType.html#a49d1a8bc7ae29dded840e30b679b0cf2",
"classSDOM_1_1Frame.html#a20ec88bd7b74e55076b20b37b6178079",
"classSDOM_1_1Frame.html#ac452d690d98c233a22b72ab565ca83c3",
"classSDOM_1_1Group.html#a5eebc9e24fcd4fd4e8e022950b1a9fbf",
"classSDOM_1_1Group.html#af73756bb0bfa49f9ed02a3e655bb9a1b",
"classSDOM_1_1IDisplayObject.html#a59f0351159eb9bac6a1c89b56639d49f",
"classSDOM_1_1IFontObject.html#a2e9b7226af0dace557b62f9e43329928",
"classSDOM_1_1IPanelObject.html#a6afd00e9f5372ececdb22bf929726fd6",
"classSDOM_1_1IRangeControl.html#a0fd3a9dc763e395e3d42260f89a5dff8",
"classSDOM_1_1IRangeControl.html#aade178775d0c9afbd0f3c49f8dfe02c6",
"classSDOM_1_1IRangeControl__scaffold.html#a61d29e8b0d760a623f4e9b19f8ae9065",
"classSDOM_1_1IRangeControl__scaffold.html#aff4057454b6d7cb9a282c9d3b00a4777",
"classSDOM_1_1IconButton.html#aa11891a86850d2ae1b14906770960033",
"classSDOM_1_1Label.html#a3c2910aa54834bd1fd44e456861832b2a606b51cc1c9d0b4af394419a22f2ff1f",
"classSDOM_1_1Label.html#a9ed2761595e2f29ab0c0d42c85438dabae217fe6799c2b0bfe0740530b5c78200",
"classSDOM_1_1ProgressBar.html#a20ec88bd7b74e55076b20b37b6178079",
"classSDOM_1_1ProgressBar.html#ac102134ccecbe1f6eebfce14f4354799",
"classSDOM_1_1RadioButton.html#a60d1c916d6cadc5c8edd47cb12af7f06",
"classSDOM_1_1RadioButton.html#af59db273b688afffd4a92a2c5c634d5e",
"classSDOM_1_1ScrollBar.html#a78c02e58e9597308146b211d5a6d084e",
"classSDOM_1_1Slider.html#a17d740892cc4499713a8fc38632f5e92",
"classSDOM_1_1Slider.html#ab1fb45b14b9542095d9cf9eb462d85ed",
"classSDOM_1_1Stage.html#a06b31bef3a5e23e20039341de1aa8a1a",
"classSDOM_1_1Stage.html#aad60efcd3f8a36bf6690ff3877bbe179",
"classSDOM_1_1Texture.html#ad0998244c5f497a5ce11a534d321328d",
"classSDOM_1_1TristateButton.html#a92705cb6f9a11683ed0e568db61e7482",
"classSDOM_1_1TruetypeFont.html#a65dd8d559c0b3cedf12993bffeba6aff",
"lua__Core_8cpp.html#a5954f2e93498bf0f6b59e1d33f170b49",
"lua__IDisplayObject_8hpp.html#a672ebac0aaaba30f71f8905d7fa61bdf",
"md_docs_2label__text__parsing.html#autotoc_md171",
"namespaceSDOM.html#a4cd347d96b005542f3709c6ab6d55beb",
"namespacemembers_func_h.html",
"structSDOM_1_1CheckButton_1_1InitStruct.html#a5e12d174ee70eec6c3fe907a7cececc3",
"structSDOM_1_1Group_1_1InitStruct.html#ad46a8b92a7c9ff721e1edb631c9d95ff",
"structSDOM_1_1IRangeControl__scaffold_1_1InitStruct.html#a2d89b34df7efd356fb474dfe984011e3",
"structSDOM_1_1ProgressBar_1_1InitStruct.html#a17adc32c048d68ae0d26a9778f7f4827",
"structSDOM_1_1Slider_1_1InitStruct.html#ac3b4c19819ff87fd7886678ef807edcf"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';