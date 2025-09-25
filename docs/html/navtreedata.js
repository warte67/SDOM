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
    [ "SDOM Architecture Overview", "md_architecture__overview.html", [
      [ "Runtime Flow (High Level)", "md_architecture__overview.html#autotoc_md2", null ]
    ] ],
    [ "SDOM Core Framework Design", "md_core__framework.html", [
      [ "Overview", "md_core__framework.html#autotoc_md4", null ],
      [ "Core Responsibilities", "md_core__framework.html#autotoc_md5", null ],
      [ "Core Initialization Sequence &amp; Pre-Initialization Configuration", "md_core__framework.html#autotoc_md6", [
        [ "Required Pre-Initialization Parameters:", "md_core__framework.html#autotoc_md7", null ],
        [ "Initialization Flow:", "md_core__framework.html#autotoc_md8", null ],
        [ "Example Configuration API:", "md_core__framework.html#autotoc_md9", null ],
        [ "Required Pre-Initialization Parameters:", "md_core__framework.html#autotoc_md11", null ],
        [ "Alternative Initialization: JSON Configuration", "md_core__framework.html#autotoc_md12", [
          [ "Example API:", "md_core__framework.html#autotoc_md13", null ]
        ] ]
      ] ],
      [ "Primary Callbacks", "md_core__framework.html#autotoc_md15", null ],
      [ "Hook &amp; Callback Registration", "md_core__framework.html#autotoc_md16", [
        [ "Example (C++)", "md_core__framework.html#autotoc_md17", null ],
        [ "Example (Python/Rust)", "md_core__framework.html#autotoc_md18", null ]
      ] ],
      [ "Global Accessor Functions", "md_core__framework.html#autotoc_md19", [
        [ "Example Usage (with global accessor)", "md_core__framework.html#autotoc_md20", null ]
      ] ],
      [ "Composition Diagram", "md_core__framework.html#autotoc_md21", null ],
      [ "Event Flow", "md_core__framework.html#autotoc_md22", null ],
      [ "Unit Tests", "md_core__framework.html#autotoc_md23", null ],
      [ "Stage", "md_core__framework.html#autotoc_md24", null ],
      [ "SDOM Comprehensive Architecture Diagram", "md_core__framework.html#autotoc_md25", null ],
      [ "SDOM Event System Integration", "md_core__framework.html#autotoc_md26", null ],
      [ "Additional Interfaces and Utilities", "md_core__framework.html#autotoc_md27", null ],
      [ "Extensibility &amp; Singleton Pattern", "md_core__framework.html#autotoc_md28", null ],
      [ "EventListener Support", "md_core__framework.html#autotoc_md29", [
        [ "Example (C)", "md_core__framework.html#autotoc_md30", null ],
        [ "Benefits", "md_core__framework.html#autotoc_md31", null ]
      ] ],
      [ "Registering Custom Stage Objects", "md_core__framework.html#autotoc_md32", [
        [ "Example", "md_core__framework.html#autotoc_md33", null ]
      ] ],
      [ "Integration Methods Summary", "md_core__framework.html#autotoc_md34", [
        [ "Flexibility &amp; Use Cases", "md_core__framework.html#autotoc_md35", null ]
      ] ],
      [ "Performance Considerations", "md_core__framework.html#autotoc_md36", null ],
      [ "Single Stage, Single Window Rationale", "md_core__framework.html#autotoc_md37", null ],
      [ "Overriding the Default Stage", "md_core__framework.html#autotoc_md38", null ],
      [ "Multiple Stages as Scenes", "md_core__framework.html#autotoc_md39", null ],
      [ "Window Resizing &amp; Texture Resolution", "md_core__framework.html#autotoc_md40", null ],
      [ "Shared Display Texture for Stages", "md_core__framework.html#autotoc_md41", null ],
      [ "Stages as Game States or Modes", "md_core__framework.html#autotoc_md42", null ],
      [ "Changing the Active Stage", "md_core__framework.html#autotoc_md43", null ]
    ] ],
    [ "SDOM Display Object Anchoring Design", "md_display__object__anchoring.html", [
      [ "Display Object Anchoring System (Updated September 2, 2025)", "md_display__object__anchoring.html#autotoc_md46", [
        [ "Anchor Naming Conventions", "md_display__object__anchoring.html#autotoc_md47", null ],
        [ "Edge-Based Layout", "md_display__object__anchoring.html#autotoc_md48", null ],
        [ "Position and Size Calculation", "md_display__object__anchoring.html#autotoc_md49", null ],
        [ "Anchor Accessors/Mutators", "md_display__object__anchoring.html#autotoc_md50", null ],
        [ "Changing Anchor Points", "md_display__object__anchoring.html#autotoc_md51", null ],
        [ "Drag-and-Drop and Reparenting", "md_display__object__anchoring.html#autotoc_md52", null ],
        [ "Summary", "md_display__object__anchoring.html#autotoc_md53", null ]
      ] ],
      [ "Overview", "md_display__object__anchoring.html#autotoc_md54", null ],
      [ "Anchor Points", "md_display__object__anchoring.html#autotoc_md55", [
        [ "Reversed versions for user convenience", "md_display__object__anchoring.html#autotoc_md56", null ]
      ] ],
      [ "JSON Interface", "md_display__object__anchoring.html#autotoc_md57", null ],
      [ "Position and Size Calculation", "md_display__object__anchoring.html#autotoc_md58", null ],
      [ "Changing Anchor Points", "md_display__object__anchoring.html#autotoc_md59", null ],
      [ "Hierarchical Anchoring", "md_display__object__anchoring.html#autotoc_md60", null ],
      [ "Backward Compatibility", "md_display__object__anchoring.html#autotoc_md61", null ],
      [ "Benefits", "md_display__object__anchoring.html#autotoc_md62", null ],
      [ "Example Use Cases", "md_display__object__anchoring.html#autotoc_md63", null ]
    ] ],
    [ "DOM Propagation Design", "md_dom__propagation.html", [
      [ "Overview", "md_dom__propagation.html#autotoc_md66", null ],
      [ "Event Propagation Diagram", "md_dom__propagation.html#autotoc_md68", null ],
      [ "Traversal Types", "md_dom__propagation.html#autotoc_md69", [
        [ "1. Core-based DOM Traversal (Type 1)", "md_dom__propagation.html#autotoc_md70", null ],
        [ "2. EventManager-based Event Traversal (Type 2)", "md_dom__propagation.html#autotoc_md72", null ],
        [ "3. Factory-based Global Traversal (Type 3)", "md_dom__propagation.html#autotoc_md74", null ],
        [ "4. World Coordinate Backpropagation Traversal (Type 4)", "md_dom__propagation.html#autotoc_md75", null ]
      ] ],
      [ "Traversal Safety and Modification", "md_dom__propagation.html#autotoc_md76", null ],
      [ "Architectural Separation", "md_dom__propagation.html#autotoc_md77", null ],
      [ "Next Steps", "md_dom__propagation.html#autotoc_md79", null ]
    ] ],
    [ "extensible_factory", "md_extensible__factory.html", [
      [ "Extensible Factory Design", "md_extensible__factory.html#autotoc_md80", [
        [ "Factory Overview (Diagram)", "md_extensible__factory.html#autotoc_md81", null ],
        [ "Resource Type Management", "md_extensible__factory.html#autotoc_md82", null ],
        [ "Requirements", "md_extensible__factory.html#autotoc_md83", null ],
        [ "Recommended Approach", "md_extensible__factory.html#autotoc_md84", [
          [ "1. Initializer Structs and JSON Configs", "md_extensible__factory.html#autotoc_md85", null ],
          [ "2. Factory Registration", "md_extensible__factory.html#autotoc_md86", null ],
          [ "3. Factory Create Methods", "md_extensible__factory.html#autotoc_md87", null ],
          [ "4. Usage", "md_extensible__factory.html#autotoc_md88", null ]
        ] ],
        [ "Benefits", "md_extensible__factory.html#autotoc_md89", null ],
        [ "Implementation Strategy", "md_extensible__factory.html#autotoc_md91", [
          [ "1. Initializer Structs", "md_extensible__factory.html#autotoc_md92", null ],
          [ "1a. Standardized Constructor Signatures", "md_extensible__factory.html#autotoc_md93", null ],
          [ "2. Private Constructors &amp; Friend Factory", "md_extensible__factory.html#autotoc_md94", null ],
          [ "3. Factory Registration System", "md_extensible__factory.html#autotoc_md95", null ],
          [ "4. Generic Factory Create Method", "md_extensible__factory.html#autotoc_md96", null ],
          [ "5. Extensibility", "md_extensible__factory.html#autotoc_md97", null ],
          [ "6. Error Handling &amp; Validation", "md_extensible__factory.html#autotoc_md98", null ],
          [ "7. Example Usage", "md_extensible__factory.html#autotoc_md99", null ]
        ] ],
        [ "Standardized Constructors for IDisplayObjects", "md_extensible__factory.html#autotoc_md101", null ],
        [ "Configuration Format Choice", "md_extensible__factory.html#autotoc_md102", null ],
        [ "Resource Pointer (<span class=\"tt\">resource_ptr</span>) Design", "md_extensible__factory.html#autotoc_md103", [
          [ "How It Works", "md_extensible__factory.html#autotoc_md104", null ],
          [ "Example Usage", "md_extensible__factory.html#autotoc_md105", null ],
          [ "Benefits", "md_extensible__factory.html#autotoc_md106", null ],
          [ "Implementation Outline", "md_extensible__factory.html#autotoc_md107", null ],
          [ "Summary", "md_extensible__factory.html#autotoc_md108", null ]
        ] ]
      ] ]
    ] ],
    [ "Inheritance Warning: Avoid Diamond Inheritance", "md_object__handles.html", [
      [ "Two Trees in the Garden", "md_object__handles.html#autotoc_md110", null ],
      [ "Object Handles in SDOM", "md_object__handles.html#autotoc_md111", [
        [ "Overview", "md_object__handles.html#autotoc_md112", null ],
        [ "Handle Types", "md_object__handles.html#autotoc_md113", [
          [ "1. DomHandle", "md_object__handles.html#autotoc_md114", null ],
          [ "2. ResHandle", "md_object__handles.html#autotoc_md115", null ]
        ] ],
        [ "Design Rationale", "md_object__handles.html#autotoc_md116", null ],
        [ "Example Usage", "md_object__handles.html#autotoc_md117", null ],
        [ "Migration Strategy", "md_object__handles.html#autotoc_md118", null ],
        [ "Best Practices", "md_object__handles.html#autotoc_md119", null ]
      ] ]
    ] ],
    [ "Project Description", "md_progress.html", [
      [ "Scripting Flexibility:", "md_progress.html#autotoc_md121", null ],
      [ "Progress Updates", "md_progress.html#autotoc_md122", null ],
      [ "Next Steps:", "md_progress.html#autotoc_md123", null ],
      [ "ToDo:", "md_progress.html#autotoc_md124", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
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
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"SDOM_8cpp.html",
"classSDOM_1_1Core.html#ae793ad3e0301faefabc0906398d1ab9d",
"classSDOM_1_1EventType.html#a1de6c66c960e9955f31b373ef0c34c43",
"classSDOM_1_1IDataObject.html#a4701e8775b9ab1332f1d9f4c7f8059fb",
"classSDOM_1_1IDisplayObject.html#af0dbd5b2d68b64152081ddad87f5fb1f",
"classSDOM_1_1Stage.html#a780b503a81e095549da8442c2e934db0",
"md_display__object__anchoring.html#autotoc_md46",
"structSDOM_1_1TypeCreators.html#aefd8ec7858a1c0c9ed7d3a57d79f1b46"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';