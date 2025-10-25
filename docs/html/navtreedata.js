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
      [ "Runtime Flow (High Level)", "md_docs_2architecture__overview.html#autotoc_md13", null ]
    ] ],
    [ "Core Framework Design", "md_docs_2core__framework.html", [
      [ "Overview", "md_docs_2core__framework.html#autotoc_md15", null ],
      [ "Core Responsibilities", "md_docs_2core__framework.html#autotoc_md16", null ],
      [ "Core Initialization Sequence &amp; Pre-Initialization Configuration", "md_docs_2core__framework.html#autotoc_md17", [
        [ "Required Pre-Initialization Parameters:", "md_docs_2core__framework.html#autotoc_md18", null ],
        [ "Initialization Flow:", "md_docs_2core__framework.html#autotoc_md19", null ],
        [ "Example Configuration API:", "md_docs_2core__framework.html#autotoc_md20", null ],
        [ "Alternative Initialization: Lua Configuration", "md_docs_2core__framework.html#autotoc_md22", null ]
      ] ],
      [ "Primary Callbacks", "md_docs_2core__framework.html#autotoc_md24", null ],
      [ "Hook &amp; Callback Registration", "md_docs_2core__framework.html#autotoc_md25", [
        [ "Example (C++)", "md_docs_2core__framework.html#autotoc_md26", null ],
        [ "Example (Python/Rust)", "md_docs_2core__framework.html#autotoc_md27", null ]
      ] ],
      [ "Global Accessor Functions", "md_docs_2core__framework.html#autotoc_md28", [
        [ "Example Usage (with global accessor)", "md_docs_2core__framework.html#autotoc_md29", null ]
      ] ],
      [ "Composition Diagram", "md_docs_2core__framework.html#autotoc_md30", null ],
      [ "Event Flow", "md_docs_2core__framework.html#autotoc_md31", null ],
      [ "Unit Tests", "md_docs_2core__framework.html#autotoc_md32", null ],
      [ "Stage", "md_docs_2core__framework.html#autotoc_md33", null ],
      [ "SDOM Comprehensive Architecture Diagram", "md_docs_2core__framework.html#autotoc_md34", null ],
      [ "SDOM Event System Integration", "md_docs_2core__framework.html#autotoc_md35", null ],
      [ "Additional Interfaces and Utilities", "md_docs_2core__framework.html#autotoc_md36", null ],
      [ "Extensibility &amp; Singleton Pattern", "md_docs_2core__framework.html#autotoc_md37", null ],
      [ "EventListener Support", "md_docs_2core__framework.html#autotoc_md38", [
        [ "Example (C)", "md_docs_2core__framework.html#autotoc_md39", null ],
        [ "Benefits", "md_docs_2core__framework.html#autotoc_md40", null ]
      ] ],
      [ "Registering Custom Stage Objects", "md_docs_2core__framework.html#autotoc_md41", [
        [ "Example", "md_docs_2core__framework.html#autotoc_md42", null ]
      ] ],
      [ "Integration Methods Summary", "md_docs_2core__framework.html#autotoc_md43", [
        [ "Flexibility &amp; Use Cases", "md_docs_2core__framework.html#autotoc_md44", null ]
      ] ],
      [ "Performance Considerations", "md_docs_2core__framework.html#autotoc_md45", null ],
      [ "Single Stage, Single Window Rationale", "md_docs_2core__framework.html#autotoc_md46", null ],
      [ "Overriding the Default Stage", "md_docs_2core__framework.html#autotoc_md47", null ],
      [ "Multiple Stages as Scenes", "md_docs_2core__framework.html#autotoc_md48", null ],
      [ "Window Resizing &amp; Texture Resolution", "md_docs_2core__framework.html#autotoc_md49", null ],
      [ "Shared Display Texture for Stages", "md_docs_2core__framework.html#autotoc_md50", null ],
      [ "Stages as Game States or Modes", "md_docs_2core__framework.html#autotoc_md51", null ],
      [ "Changing the Active Stage", "md_docs_2core__framework.html#autotoc_md52", null ]
    ] ],
    [ "Where to Put Your Logic: Rendering, Layout, Updates, and Input", "md_docs_2custom__object__lifecycle.html", [
      [ "Putting It All Together", "md_docs_2custom__object__lifecycle.html#autotoc_md66", null ],
      [ "Practical Tip — When in Doubt:", "md_docs_2custom__object__lifecycle.html#autotoc_md68", null ]
    ] ],
    [ "Display Object Anchoring System", "md_docs_2display__object__anchoring.html", [
      [ "Overview", "md_docs_2display__object__anchoring.html#autotoc_md78", null ],
      [ "Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md79", [
        [ "Anchor Naming Conventions", "md_docs_2display__object__anchoring.html#autotoc_md71", null ],
        [ "Edge-Based Layout", "md_docs_2display__object__anchoring.html#autotoc_md72", null ],
        [ "Position and Size Calculation", "md_docs_2display__object__anchoring.html#autotoc_md73", null ],
        [ "Anchor Accessors/Mutators", "md_docs_2display__object__anchoring.html#autotoc_md74", null ],
        [ "Changing Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md75", null ],
        [ "Drag-and-Drop and Reparenting", "md_docs_2display__object__anchoring.html#autotoc_md76", null ],
        [ "Summary", "md_docs_2display__object__anchoring.html#autotoc_md77", null ],
        [ "Reversed versions for user convenience", "md_docs_2display__object__anchoring.html#autotoc_md80", null ]
      ] ],
      [ "Lua Interface", "md_docs_2display__object__anchoring.html#autotoc_md81", null ],
      [ "Position and Size Calculation", "md_docs_2display__object__anchoring.html#autotoc_md82", null ],
      [ "Changing Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md83", null ],
      [ "Hierarchical Anchoring", "md_docs_2display__object__anchoring.html#autotoc_md84", null ],
      [ "Backward Compatibility &amp; Migration", "md_docs_2display__object__anchoring.html#autotoc_md85", null ],
      [ "Benefits", "md_docs_2display__object__anchoring.html#autotoc_md86", null ],
      [ "Example Use Cases", "md_docs_2display__object__anchoring.html#autotoc_md87", null ],
      [ "Anchoring Diagrams", "md_docs_2display__object__anchoring.html#autotoc_md90", [
        [ "1) Parent Anchor Grid and Child Edge Anchors", "md_docs_2display__object__anchoring.html#autotoc_md91", null ],
        [ "2) Anchor Change Recalculation (No Visual Jump)", "md_docs_2display__object__anchoring.html#autotoc_md92", null ]
      ] ],
      [ "Anchor helpers &amp; parsing", "md_docs_2display__object__anchoring.html#autotoc_md93", null ],
      [ "Migration note", "md_docs_2display__object__anchoring.html#autotoc_md94", null ]
    ] ],
    [ "DOM Propagation Design", "md_docs_2dom__propagation.html", [
      [ "Overview", "md_docs_2dom__propagation.html#autotoc_md96", null ],
      [ "Event Propagation Diagram", "md_docs_2dom__propagation.html#autotoc_md98", null ],
      [ "Traversal Types", "md_docs_2dom__propagation.html#autotoc_md99", [
        [ "1. Core-based DOM Traversal (Type 1)", "md_docs_2dom__propagation.html#autotoc_md100", null ],
        [ "2. EventManager-based Event Traversal (Type 2)", "md_docs_2dom__propagation.html#autotoc_md102", null ],
        [ "3. Factory-based Global Traversal (Type 3)", "md_docs_2dom__propagation.html#autotoc_md104", null ],
        [ "4. World Coordinate Backpropagation Traversal (Type 4)", "md_docs_2dom__propagation.html#autotoc_md105", null ]
      ] ],
      [ "Traversal Safety and Modification", "md_docs_2dom__propagation.html#autotoc_md106", [
        [ "Deferred actions processing (pseudo)", "md_docs_2dom__propagation.html#autotoc_md107", null ],
        [ "Thread-safety notes", "md_docs_2dom__propagation.html#autotoc_md108", null ]
      ] ],
      [ "Architectural Separation", "md_docs_2dom__propagation.html#autotoc_md109", null ],
      [ "Migration note", "md_docs_2dom__propagation.html#autotoc_md110", null ]
    ] ],
    [ "SDOM::Events Design Document", "md_docs_2event__design.html", [
      [ "Overview", "md_docs_2event__design.html#autotoc_md112", null ],
      [ "Goals", "md_docs_2event__design.html#autotoc_md113", null ],
      [ "Event Structure", "md_docs_2event__design.html#autotoc_md114", null ],
      [ "Event Lifecycle", "md_docs_2event__design.html#autotoc_md115", null ],
      [ "Event Types", "md_docs_2event__design.html#autotoc_md116", null ],
      [ "Predefined Event Types", "md_docs_2event__design.html#autotoc_md117", [
        [ "General Events", "md_docs_2event__design.html#autotoc_md118", null ],
        [ "Mouse Events", "md_docs_2event__design.html#autotoc_md119", null ],
        [ "Keyboard Events", "md_docs_2event__design.html#autotoc_md120", null ],
        [ "Timer Events", "md_docs_2event__design.html#autotoc_md121", null ],
        [ "UI Events", "md_docs_2event__design.html#autotoc_md122", null ],
        [ "Drag &amp; Drop Events", "md_docs_2event__design.html#autotoc_md123", null ],
        [ "Clipboard Events", "md_docs_2event__design.html#autotoc_md124", null ],
        [ "Custom Events", "md_docs_2event__design.html#autotoc_md125", null ]
      ] ],
      [ "EventManager Responsibilities", "md_docs_2event__design.html#autotoc_md126", null ],
      [ "Existing Implementation", "md_docs_2event__design.html#autotoc_md127", null ],
      [ "Next Steps", "md_docs_2event__design.html#autotoc_md128", null ],
      [ "Notes", "md_docs_2event__design.html#autotoc_md129", null ],
      [ "Event System Design: Categorizing EventTypes", "md_docs_2event__design.html#autotoc_md131", [
        [ "Proposal", "md_docs_2event__design.html#autotoc_md132", null ],
        [ "Advantages", "md_docs_2event__design.html#autotoc_md133", null ],
        [ "Suggested Implementation", "md_docs_2event__design.html#autotoc_md134", null ],
        [ "Integration", "md_docs_2event__design.html#autotoc_md135", null ],
        [ "Considerations", "md_docs_2event__design.html#autotoc_md136", [
          [ "Listener-Specific Events", "md_docs_2event__design.html#autotoc_md138", [
            [ "Example Behavior", "md_docs_2event__design.html#autotoc_md139", null ],
            [ "Integration", "md_docs_2event__design.html#autotoc_md140", null ]
          ] ]
        ] ]
      ] ]
    ] ],
    [ "extensible_factory", "md_docs_2extensible__factory.html", [
      [ "Extensible Factory Design", "md_docs_2extensible__factory.html#autotoc_md142", null ],
      [ "Lua Application Framework (Test + Runtime Layer)", "md_docs_2extensible__factory.html#autotoc_md146", [
        [ "Now, One More Step:", "md_docs_2extensible__factory.html#autotoc_md151", [
          [ "Callback Modules", "md_docs_2extensible__factory.html#autotoc_md147", null ],
          [ "Display Object Event Listeners", "md_docs_2extensible__factory.html#autotoc_md148", null ],
          [ "Larger Applications", "md_docs_2extensible__factory.html#autotoc_md149", null ],
          [ "Lua flows naturally into the core systems:", "md_docs_2extensible__factory.html#autotoc_md152", null ],
          [ "✅ What this Diagram Communicates", "md_docs_2extensible__factory.html#autotoc_md153", null ],
          [ "This directly reinforces:", "md_docs_2extensible__factory.html#autotoc_md154", null ]
        ] ]
      ] ],
      [ "How a Display Object Registers Itself with the Factory", "md_docs_2extensible__factory.html#autotoc_md156", [
        [ "Visual Model: Where Registration Lives", "md_docs_2extensible__factory.html#autotoc_md160", null ],
        [ "A Minimal Example", "md_docs_2extensible__factory.html#autotoc_md162", [
          [ "The Core Idea", "md_docs_2extensible__factory.html#autotoc_md158", null ],
          [ "What Registration Actually Looks Like Inside the Factory", "md_docs_2extensible__factory.html#autotoc_md164", null ]
        ] ],
        [ "Creating a New Display Object Type", "md_docs_2extensible__factory.html#autotoc_md166", [
          [ "1. Class Declaration", "md_docs_2extensible__factory.html#autotoc_md168", null ],
          [ "1. Why These Pieces Exist", "md_docs_2extensible__factory.html#autotoc_md170", null ],
          [ "2. Lua Binding (Important Detail!)", "md_docs_2extensible__factory.html#autotoc_md172", null ],
          [ "3. Summary Checklist (Copy This When Creating New Objects)", "md_docs_2extensible__factory.html#autotoc_md174", null ]
        ] ],
        [ "Object Lifetimes, Parents, and Orphans", "md_docs_2extensible__factory.html#autotoc_md176", null ],
        [ "Why Orphans Exist (And Why They’re Actually Useful)", "md_docs_2extensible__factory.html#autotoc_md178", [
          [ "Visualizing the Orphan Lifecycle", "md_docs_2extensible__factory.html#autotoc_md180", null ],
          [ "The Cleanup Loop (Automatically Happens Each Frame)", "md_docs_2extensible__factory.html#autotoc_md182", null ],
          [ "Example: Fade-Out Animation Using GracePeriod", "md_docs_2extensible__factory.html#autotoc_md184", null ],
          [ "Example: UI Reuse With RetainUntilManual", "md_docs_2extensible__factory.html#autotoc_md185", null ],
          [ "Key Takeaways", "md_docs_2extensible__factory.html#autotoc_md187", null ]
        ] ],
        [ "Adding Custom Properties to Your Display Object", "md_docs_2extensible__factory.html#autotoc_md189", [
          [ "1. Add the Property to the InitStruct", "md_docs_2extensible__factory.html#autotoc_md190", null ],
          [ "2. Store the Property in the Class", "md_docs_2extensible__factory.html#autotoc_md192", null ],
          [ "3. Copy it In from Constructors", "md_docs_2extensible__factory.html#autotoc_md194", null ],
          [ "4. Add Getters / Setters", "md_docs_2extensible__factory.html#autotoc_md196", null ],
          [ "5. Expose It to Lua (Inside <span class=\"tt\">_registerLuaBindings()</span>)", "md_docs_2extensible__factory.html#autotoc_md198", null ],
          [ "Visualizing the Flow", "md_docs_2extensible__factory.html#autotoc_md199", null ],
          [ "Optional: Reflecting Properties Back Into Lua Tables", "md_docs_2extensible__factory.html#autotoc_md201", null ]
        ] ],
        [ "Where to Put Your Logic: Rendering, Layout, Updates, and Input", "md_docs_2extensible__factory.html#autotoc_md204", [
          [ "<span class=\"tt\">onInit()</span>: Setup, Don’t Draw Yet", "md_docs_2extensible__factory.html#autotoc_md206", null ],
          [ "<span class=\"tt\">onUpdate(float dt)</span>: Animation / State Changes", "md_docs_2extensible__factory.html#autotoc_md208", null ],
          [ "<span class=\"tt\">onEvent(const Event&amp;)</span>: Input &amp; UI Interaction", "md_docs_2extensible__factory.html#autotoc_md210", null ],
          [ "<span class=\"tt\">onRender()</span>: <em>Actually draw something</em>", "md_docs_2extensible__factory.html#autotoc_md212", null ],
          [ "<span class=\"tt\">onQuit()</span>: Clean Up <em>If Needed</em>", "md_docs_2extensible__factory.html#autotoc_md214", null ]
        ] ],
        [ "Putting It All Together", "md_docs_2extensible__factory.html#autotoc_md216", null ],
        [ "Practical Tip — When in Doubt:", "md_docs_2extensible__factory.html#autotoc_md218", null ],
        [ "✅ Multiple Ways to Create Display Objects", "md_docs_2extensible__factory.html#autotoc_md220", [
          [ "1. Single LUA initialization file:", "md_docs_2extensible__factory.html#autotoc_md221", null ],
          [ "2. Static Lua Scene Definition (Declarative)", "md_docs_2extensible__factory.html#autotoc_md223", null ],
          [ "3. Lua Creating UI Dynamically at Runtime", "md_docs_2extensible__factory.html#autotoc_md224", null ],
          [ "4. C++ Using InitStruct (Strong-Typed Approach)", "md_docs_2extensible__factory.html#autotoc_md225", null ],
          [ "5. C++ Creating the Object Using a Lua Table", "md_docs_2extensible__factory.html#autotoc_md226", null ]
        ] ]
      ] ]
    ] ],
    [ "Icon Index Catalog for sprite_8x8 Font", "md_docs_2icon__8x8.html", [
      [ "Icon Glyphs", "md_docs_2icon__8x8.html#autotoc_md228", null ],
      [ "Extended Icon Glyphs:", "md_docs_2icon__8x8.html#autotoc_md229", null ],
      [ "Icon Sets", "md_docs_2icon__8x8.html#autotoc_md230", [
        [ "Sets", "md_docs_2icon__8x8.html#autotoc_md231", null ]
      ] ],
      [ "Customization Potential", "md_docs_2icon__8x8.html#autotoc_md232", [
        [ "Extensibility", "md_docs_2icon__8x8.html#autotoc_md233", null ]
      ] ]
    ] ],
    [ "Design Goals for <span class=\"tt\">IPanelObject</span>", "md_docs_2IPanelObject.html", [
      [ "IPanelObject Design Document", "md_docs_2IPanelObject.html#autotoc_md235", [
        [ "What is a 9-slice (9-patch) panel?", "md_docs_2IPanelObject.html#autotoc_md236", null ],
        [ "What is a 9-slice (9-patch) panel?", "md_docs_2IPanelObject.html#autotoc_md237", null ],
        [ "Reference: sprite_8x8 Icon Index Catalog", "md_docs_2IPanelObject.html#autotoc_md238", [
          [ "Predefined 9-Slice Sets in <span class=\"tt\">sprite_8x8</span>", "md_docs_2IPanelObject.html#autotoc_md239", null ]
        ] ],
        [ "Suggested Steps", "md_docs_2IPanelObject.html#autotoc_md241", null ],
        [ "Interface Sketch", "md_docs_2IPanelObject.html#autotoc_md243", null ],
        [ "Note on Button Objects and State Handling", "md_docs_2IPanelObject.html#autotoc_md244", null ]
      ] ]
    ] ],
    [ "Label Text Parsing Design Document", "md_docs_2label__text__parsing.html", [
      [ "Overview", "md_docs_2label__text__parsing.html#autotoc_md246", null ],
      [ "Goals", "md_docs_2label__text__parsing.html#autotoc_md248", null ],
      [ "Retro-Inspired Graphic Glyphs", "md_docs_2label__text__parsing.html#autotoc_md250", [
        [ "Customization Potential", "md_docs_2label__text__parsing.html#autotoc_md251", null ]
      ] ],
      [ "Escape Sequence Syntax", "md_docs_2label__text__parsing.html#autotoc_md252", [
        [ "Supported Color Names", "md_docs_2label__text__parsing.html#autotoc_md254", [
          [ "Color Targets:", "md_docs_2label__text__parsing.html#autotoc_md255", null ]
        ] ],
        [ "Style", "md_docs_2label__text__parsing.html#autotoc_md256", null ]
      ] ],
      [ "Lua bindings: property-style vs legacy get/set functions", "md_docs_2label__text__parsing.html#autotoc_md257", [
        [ "Alignment", "md_docs_2label__text__parsing.html#autotoc_md259", null ],
        [ "Quick reference", "md_docs_2label__text__parsing.html#autotoc_md260", null ],
        [ "Alignment", "md_docs_2label__text__parsing.html#autotoc_md262", null ],
        [ "Quick reference", "md_docs_2label__text__parsing.html#autotoc_md263", [
          [ "Colors:", "md_docs_2label__text__parsing.html#autotoc_md258", null ],
          [ "Bitfield Encoding for Alignment", "md_docs_2label__text__parsing.html#autotoc_md264", null ]
        ] ],
        [ "Special", "md_docs_2label__text__parsing.html#autotoc_md265", null ],
        [ "Escaping Square Brackets", "md_docs_2label__text__parsing.html#autotoc_md266", null ],
        [ "Font Size", "md_docs_2label__text__parsing.html#autotoc_md267", null ]
      ] ],
      [ "Numeric Style Escapes (border / outline / padding / dropshadow)", "md_docs_2label__text__parsing.html#autotoc_md269", [
        [ "Overview", "md_docs_2label__text__parsing.html#autotoc_md270", null ],
        [ "Syntax", "md_docs_2label__text__parsing.html#autotoc_md271", null ],
        [ "Semantics", "md_docs_2label__text__parsing.html#autotoc_md272", null ],
        [ "Examples", "md_docs_2label__text__parsing.html#autotoc_md273", null ],
        [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md274", null ],
        [ "Test seed suggestion (Label_test5)", "md_docs_2label__text__parsing.html#autotoc_md275", null ]
      ] ],
      [ "Token Structure", "md_docs_2label__text__parsing.html#autotoc_md277", [
        [ "Word Tokenization Rules", "md_docs_2label__text__parsing.html#autotoc_md278", [
          [ "Example tokenization:", "md_docs_2label__text__parsing.html#autotoc_md279", null ]
        ] ],
        [ "Escape Tokenization Rules", "md_docs_2label__text__parsing.html#autotoc_md280", null ]
      ] ],
      [ "Parsing Algorithm", "md_docs_2label__text__parsing.html#autotoc_md282", null ],
      [ "Alignment Handling", "md_docs_2label__text__parsing.html#autotoc_md284", null ],
      [ "Word Wrapping &amp; Autosizing", "md_docs_2label__text__parsing.html#autotoc_md286", null ],
      [ "Rendering", "md_docs_2label__text__parsing.html#autotoc_md288", null ],
      [ "Extensibility", "md_docs_2label__text__parsing.html#autotoc_md290", null ],
      [ "Example", "md_docs_2label__text__parsing.html#autotoc_md292", null ],
      [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md294", [
        [ "Label Sizing and Wrapping Rules", "md_docs_2label__text__parsing.html#autotoc_md295", null ],
        [ "Implementation Notes", "md_docs_2label__text__parsing.html#autotoc_md296", null ]
      ] ],
      [ "Rendering Phases (Functions):", "md_docs_2label__text__parsing.html#autotoc_md297", [
        [ "1. Pre-Render Layout &amp; Bounds Adjustment", "md_docs_2label__text__parsing.html#autotoc_md298", null ],
        [ "2. Phrase Grouping", "md_docs_2label__text__parsing.html#autotoc_md299", null ],
        [ "3. Phrase Rendering", "md_docs_2label__text__parsing.html#autotoc_md300", null ]
      ] ],
      [ "Label Usage Examples", "md_docs_2label__text__parsing.html#autotoc_md301", [
        [ "<b>Explanation</b>", "md_docs_2label__text__parsing.html#autotoc_md302", null ],
        [ "<b>Text and Formatting</b>", "md_docs_2label__text__parsing.html#autotoc_md304", null ],
        [ "<b>Anchor Reference Diagram</b>", "md_docs_2label__text__parsing.html#autotoc_md306", null ],
        [ "<b>Wrapping &amp; Layout Behavior</b>", "md_docs_2label__text__parsing.html#autotoc_md308", null ],
        [ "<b>Rendering Performance</b>", "md_docs_2label__text__parsing.html#autotoc_md310", null ]
      ] ],
      [ "ASCII Glyphs", "md_docs_2label__text__parsing.html#autotoc_md313", null ],
      [ "Graphic Glyphs", "md_docs_2label__text__parsing.html#autotoc_md314", null ],
      [ "References", "md_docs_2label__text__parsing.html#autotoc_md316", null ]
    ] ],
    [ "Object Handles in SDOM", "md_docs_2object__handles.html", [
      [ "Two Trees in the Garden", "md_docs_2object__handles.html#autotoc_md321", null ],
      [ "Overview", "md_docs_2object__handles.html#autotoc_md322", null ],
      [ "Handle Types", "md_docs_2object__handles.html#autotoc_md323", [
        [ "1. DisplayHandle", "md_docs_2object__handles.html#autotoc_md324", null ],
        [ "2. AssetHandle", "md_docs_2object__handles.html#autotoc_md325", null ]
      ] ],
      [ "Design Rationale", "md_docs_2object__handles.html#autotoc_md326", null ],
      [ "Inheritance Warning: Avoid Diamond Inheritance", "md_docs_2object__handles.html#autotoc_md328", null ],
      [ "Example Usage", "md_docs_2object__handles.html#autotoc_md329", null ],
      [ "Migration Strategy", "md_docs_2object__handles.html#autotoc_md330", null ],
      [ "Best Practices", "md_docs_2object__handles.html#autotoc_md331", null ],
      [ "Optional: Handle Resolution Flow (reference)", "md_docs_2object__handles.html#autotoc_md333", null ]
    ] ],
    [ "Project Description and Development Progress", "md_docs_2progress.html", [
      [ "Scripting and Configuration", "md_docs_2progress.html#autotoc_md335", null ],
      [ "Progress Updates", "md_docs_2progress.html#autotoc_md336", [
        [ "[July 2, 2025]", "md_docs_2progress.html#autotoc_md337", null ],
        [ "[August 5, 2025]", "md_docs_2progress.html#autotoc_md339", null ],
        [ "[September 19, 2025]", "md_docs_2progress.html#autotoc_md341", null ],
        [ "[September 20, 2025]", "md_docs_2progress.html#autotoc_md343", null ],
        [ "[September 21, 2025]", "md_docs_2progress.html#autotoc_md345", null ],
        [ "[September 22, 2025]", "md_docs_2progress.html#autotoc_md347", [
          [ "[September 23, 2025]", "md_docs_2progress.html#autotoc_md349", null ]
        ] ],
        [ "[September 24, 2025]", "md_docs_2progress.html#autotoc_md351", null ],
        [ "[September 25, 2025]", "md_docs_2progress.html#autotoc_md353", null ],
        [ "[September 26, 2025]", "md_docs_2progress.html#autotoc_md355", null ],
        [ "[September 27, 2025]", "md_docs_2progress.html#autotoc_md357", null ],
        [ "[September 28, 2025]", "md_docs_2progress.html#autotoc_md359", null ],
        [ "[September 29, 2025]", "md_docs_2progress.html#autotoc_md361", null ],
        [ "[September 30, 2025]", "md_docs_2progress.html#autotoc_md363", null ],
        [ "[October 2, 2025]", "md_docs_2progress.html#autotoc_md365", null ],
        [ "[October 3, 2025]", "md_docs_2progress.html#autotoc_md367", null ],
        [ "[October 4, 2025]", "md_docs_2progress.html#autotoc_md369", null ],
        [ "[October 5, 2025]", "md_docs_2progress.html#autotoc_md371", null ],
        [ "[October 6, 2025]", "md_docs_2progress.html#autotoc_md373", null ],
        [ "[October 7, 2025]", "md_docs_2progress.html#autotoc_md375", null ],
        [ "[October 8, 2025]", "md_docs_2progress.html#autotoc_md377", null ],
        [ "[October 9, 2025]", "md_docs_2progress.html#autotoc_md379", null ],
        [ "[October 11, 2025]", "md_docs_2progress.html#autotoc_md381", null ],
        [ "[October 12, 2025]", "md_docs_2progress.html#autotoc_md383", null ],
        [ "[October 13, 2025]", "md_docs_2progress.html#autotoc_md385", null ],
        [ "[October 14, 2025]", "md_docs_2progress.html#autotoc_md387", null ],
        [ "[October 15, 2025]", "md_docs_2progress.html#autotoc_md389", null ],
        [ "[October 16, 2025]", "md_docs_2progress.html#autotoc_md391", null ],
        [ "[October 17, 2025]", "md_docs_2progress.html#autotoc_md393", null ],
        [ "[October 18, 2025]", "md_docs_2progress.html#autotoc_md395", [
          [ "[October 19, 2025]", "md_docs_2progress.html#autotoc_md397", null ],
          [ "[October 20, 2025]", "md_docs_2progress.html#autotoc_md399", null ],
          [ "[October 21, 2025]", "md_docs_2progress.html#autotoc_md401", null ],
          [ "[October 22, 2025]", "md_docs_2progress.html#autotoc_md403", null ],
          [ "[October 23, 2025]", "md_docs_2progress.html#autotoc_md405", null ],
          [ "[October 24, 2025]", "md_docs_2progress.html#autotoc_md407", null ]
        ] ],
        [ "Next Steps:", "md_docs_2progress.html#autotoc_md409", null ],
        [ "Long Term To Do:", "md_docs_2progress.html#autotoc_md411", null ],
        [ "Current DisplayObject Inheritance Hierarchy:", "md_docs_2progress.html#autotoc_md412", null ],
        [ "UnitTest Modules", "md_docs_2progress.html#autotoc_md414", null ]
      ] ]
    ] ],
    [ "Scripting and Configuration (Lua &amp; C++)", "md_docs_2scripting__and__configuration.html", [
      [ "Goals", "md_docs_2scripting__and__configuration.html#autotoc_md416", null ],
      [ "Modes of Use", "md_docs_2scripting__and__configuration.html#autotoc_md417", null ],
      [ "Integration Points", "md_docs_2scripting__and__configuration.html#autotoc_md418", null ],
      [ "Example: Creating with Lua", "md_docs_2scripting__and__configuration.html#autotoc_md419", null ],
      [ "Safety and Performance", "md_docs_2scripting__and__configuration.html#autotoc_md420", null ],
      [ "Source of Truth", "md_docs_2scripting__and__configuration.html#autotoc_md421", null ],
      [ "SDOM API considerations for Lua", "md_docs_2scripting__and__configuration.html#autotoc_md422", null ],
      [ "Next steps (Lua integration)", "md_docs_2scripting__and__configuration.html#autotoc_md423", null ],
      [ "Module-style configuration and lifecycle registration (recommended)", "md_docs_2scripting__and__configuration.html#autotoc_md424", null ]
    ] ],
    [ "Startup Refactor Proposal", "md_docs_2startup__refactor.html", [
      [ "Step-by-Step Refactoring Strategy", "md_docs_2startup__refactor.html#autotoc_md432", [
        [ "1. Explicit Initialization Pattern", "md_docs_2startup__refactor.html#autotoc_md426", null ],
        [ "2. Remove Automatic Table Parsing", "md_docs_2startup__refactor.html#autotoc_md427", null ],
        [ "3. Separate Config Parsing from Resource Creation", "md_docs_2startup__refactor.html#autotoc_md428", null ],
        [ "4. Update Documentation and Examples", "md_docs_2startup__refactor.html#autotoc_md429", null ],
        [ "1. Deprecate Automatic Table Parsing", "md_docs_2startup__refactor.html#autotoc_md433", null ],
        [ "2. Introduce Explicit Initialization Methods", "md_docs_2startup__refactor.html#autotoc_md434", null ],
        [ "3. Bind Engine Functions as Lua Globals", "md_docs_2startup__refactor.html#autotoc_md435", null ],
        [ "4. Maintain Legacy Support During Transition", "md_docs_2startup__refactor.html#autotoc_md436", null ],
        [ "5. Refactor Resource Creation Logic", "md_docs_2startup__refactor.html#autotoc_md437", null ],
        [ "6. Update Documentation and Examples", "md_docs_2startup__refactor.html#autotoc_md438", null ],
        [ "7. Test and Validate", "md_docs_2startup__refactor.html#autotoc_md439", null ]
      ] ]
    ] ],
    [ "Arrows", "md_docs_2ui__symbols.html", null ],
    [ "SDOM: Simple SDL Document Object Model API", "md_README.html", [
      [ "Overview", "md_README.html#autotoc_md449", null ],
      [ "Features", "md_README.html#autotoc_md450", null ],
      [ "IDataObject: Data-Driven Base Type", "md_README.html#autotoc_md451", [
        [ "Key Features", "md_README.html#autotoc_md452", null ]
      ] ],
      [ "Identifier Conventions", "md_README.html#autotoc_md453", null ],
      [ "Getting Started", "md_README.html#autotoc_md455", [
        [ "Prerequisites", "md_README.html#autotoc_md456", null ],
        [ "Building the SDOM Library", "md_README.html#autotoc_md457", null ],
        [ "Running the Example", "md_README.html#autotoc_md458", null ],
        [ "Notable recent changes", "md_README.html#autotoc_md459", null ],
        [ "Migration notes", "md_README.html#autotoc_md460", null ]
      ] ],
      [ "Project Structure", "md_README.html#autotoc_md461", null ],
      [ "SDL3 Build Dependencies", "md_README.html#autotoc_md462", null ],
      [ "Debian Build Dependencies", "md_README.html#autotoc_md463", null ],
      [ "Arch Build Dependencies", "md_README.html#autotoc_md464", null ],
      [ "Freetype on Debian:", "md_README.html#autotoc_md465", null ],
      [ "Building and Installing SDL3_mixer, SDL3_image, and SDL_ttf", "md_README.html#autotoc_md466", null ],
      [ "Build per SDL3:", "md_README.html#autotoc_md467", null ],
      [ "Build and install SDL3 libraries", "md_README.html#autotoc_md468", null ],
      [ "Lua and Sol2", "md_README.html#autotoc_md469", null ],
      [ "Documentation", "md_README.html#autotoc_md470", [
        [ "Exporting Mermaid Diagrams to PNG/SVG (for Doxygen)", "md_README.html#autotoc_md471", null ]
      ] ],
      [ "License", "md_README.html#autotoc_md472", null ],
      [ "Author", "md_README.html#autotoc_md473", null ]
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
"classCLR.html#ae04e27b0aabe54dc5cda889874fdd8a4",
"classSDOM_1_1ArrowButton.html#a90e2d7c4dd39c97d030aa80aabf7e048",
"classSDOM_1_1AssetHandle.html#ae010ffb8068741fcc7224fac49acc6c4",
"classSDOM_1_1Button.html#a37fa9705ad5ce2bf5d6c1edb6ae8a0b5",
"classSDOM_1_1Button.html#acb55b6182cfa065a56e544c68c627f98",
"classSDOM_1_1CheckButton.html#a5e292e0a64127c265cda051908f8fcb1",
"classSDOM_1_1CheckButton.html#af1f3fcd8844ca70fa235842bd176a6ab",
"classSDOM_1_1Core.html#ad9973f805f6577aaf8334e89884cf1a1",
"classSDOM_1_1Event.html#aad7d80c66b738e551fe0cfadea2b8a44",
"classSDOM_1_1Exception.html#acfb21b46a45340d9d8efec695e7afbab",
"classSDOM_1_1Frame.html#a606ab9bf41b5c61e3469b92fe69eeddc",
"classSDOM_1_1Frame.html#aff4057454b6d7cb9a282c9d3b00a4777",
"classSDOM_1_1Group.html#a90e2d7c4dd39c97d030aa80aabf7e048",
"classSDOM_1_1IButtonObject.html#a13bc73dac82d7465513fe4135cf5eec7",
"classSDOM_1_1IDisplayObject.html#a90dd1b86ac56a3fb85044e6f0fd5528e",
"classSDOM_1_1IFontObject.html#acf24a17e1284129dbef77cfc229bba05",
"classSDOM_1_1IPanelObject.html#a9110787e36dd32665bd498991bc78705",
"classSDOM_1_1IRangeControl.html#a39682c833400256a68206ea7d7559dd8",
"classSDOM_1_1IRangeControl.html#ad5253b0e2b8136bb9efcd3cecc00274a",
"classSDOM_1_1IRangeControl__scaffold.html#a7f0f271a892ca8bd79749305ac5da27e",
"classSDOM_1_1IconButton.html#a21a0936456c85d904886bba2fa28891f",
"classSDOM_1_1IconButton.html#ac833508be5695b4b45a4ffbfd970c3bb",
"classSDOM_1_1Label.html#a4e9eb7d8424686a6c78d9a66086e4429a07a094a210794e74a0e5e1a1457a92ee",
"classSDOM_1_1Label.html#aa4a5d7801d4403e662e13c66cb5b9ceea9d4989e254422a15442ed8c59c600f1a",
"classSDOM_1_1ProgressBar.html#a3e500051e84bdc598af43f2d76ba9d71",
"classSDOM_1_1ProgressBar.html#addaf39525119d04958b868e104170fb6",
"classSDOM_1_1RadioButton.html#a745d5a367dc1c067ecbd65b181ae4fe6",
"classSDOM_1_1RadioButton.html#aff4057454b6d7cb9a282c9d3b00a4777",
"classSDOM_1_1ScrollBar.html#a80a500f4517a686367ab08b1aa2c488d",
"classSDOM_1_1Slider.html#a21a0936456c85d904886bba2fa28891f",
"classSDOM_1_1Slider.html#abef98ceb11e7781be41a23dec0ac74cc",
"classSDOM_1_1Stage.html#a155821bb6fdbe8a38825e087666399ed",
"classSDOM_1_1Stage.html#aaf12d174e126294c58abd9b4dbd3acca",
"classSDOM_1_1Texture.html#acf24a17e1284129dbef77cfc229bba05",
"classSDOM_1_1TristateButton.html#a8c8e652fc4566409d6e5e9f4ce049de5",
"classSDOM_1_1TruetypeFont.html#a505bc325aa5116514067b9a58b8e0d25",
"md_docs_2IPanelObject.html",
"md_docs_2progress.html#autotoc_md361",
"namespaceSDOM.html#a8ce2c94b307864819ad9da1e88b524ca",
"structSDOM_1_1ArrowButton_1_1InitStruct.html",
"structSDOM_1_1CheckButton_1_1InitStruct.html#ad247c22c826eb4b80ff9081dc27dd745",
"structSDOM_1_1IDisplayObject_1_1InitStruct.html#a0981d916e4b89bb3d08f0a56041e0f08",
"structSDOM_1_1IRangeControl__scaffold_1_1InitStruct.html#ac3ecdea943f2635afac7ee1dd693dcc7",
"structSDOM_1_1ProgressBar_1_1InitStruct.html#ac3b4c19819ff87fd7886678ef807edcf",
"structSDOM_1_1SpriteSheet_1_1InitStruct.html#adc102043835faf1d54ede4d60a9b499b"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';