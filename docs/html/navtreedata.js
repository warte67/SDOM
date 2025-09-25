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
    [ "SDOM Documentation", "index.html#autotoc_md0", null ],
    [ "SDOM Core Framework Design", "md_core__framework.html", [
      [ "Overview", "md_core__framework.html#autotoc_md2", null ],
      [ "Core Responsibilities", "md_core__framework.html#autotoc_md3", null ],
      [ "Core Initialization Sequence & Pre-Initialization Configuration", "md_core__framework.html#autotoc_md4", [
        [ "Required Pre-Initialization Parameters:", "md_core__framework.html#autotoc_md5", null ],
        [ "Initialization Flow:", "md_core__framework.html#autotoc_md6", null ],
        [ "Example Configuration API:", "md_core__framework.html#autotoc_md7", null ],
        [ "Required Pre-Initialization Parameters:", "md_core__framework.html#autotoc_md9", null ],
        [ "Alternative Initialization: JSON Configuration", "md_core__framework.html#autotoc_md10", [
          [ "Example API:", "md_core__framework.html#autotoc_md11", null ]
        ] ]
      ] ],
      [ "Primary Callbacks", "md_core__framework.html#autotoc_md13", null ],
      [ "Hook & Callback Registration", "md_core__framework.html#autotoc_md14", [
        [ "Example (C++)", "md_core__framework.html#autotoc_md15", null ],
        [ "Example (Python/Rust)", "md_core__framework.html#autotoc_md16", null ]
      ] ],
      [ "Global Accessor Functions", "md_core__framework.html#autotoc_md17", [
        [ "Example Usage (with global accessor)", "md_core__framework.html#autotoc_md18", null ]
      ] ],
      [ "Composition Diagram", "md_core__framework.html#autotoc_md19", null ],
      [ "Extensibility & Singleton Pattern", "md_core__framework.html#autotoc_md20", null ],
      [ "EventListener Support", "md_core__framework.html#autotoc_md21", [
        [ "Example (C)", "md_core__framework.html#autotoc_md22", null ],
        [ "Benefits", "md_core__framework.html#autotoc_md23", null ]
      ] ],
      [ "Registering Custom Stage Objects", "md_core__framework.html#autotoc_md24", [
        [ "Example", "md_core__framework.html#autotoc_md25", null ]
      ] ],
      [ "Integration Methods Summary", "md_core__framework.html#autotoc_md26", [
        [ "Flexibility & Use Cases", "md_core__framework.html#autotoc_md27", null ]
      ] ],
      [ "Performance Considerations", "md_core__framework.html#autotoc_md28", null ],
      [ "Single Stage, Single Window Rationale", "md_core__framework.html#autotoc_md29", null ],
      [ "Overriding the Default Stage", "md_core__framework.html#autotoc_md30", null ],
      [ "Multiple Stages as Scenes", "md_core__framework.html#autotoc_md31", null ],
      [ "Window Resizing & Texture Resolution", "md_core__framework.html#autotoc_md32", null ],
      [ "Shared Display Texture for Stages", "md_core__framework.html#autotoc_md33", null ],
      [ "Stages as Game States or Modes", "md_core__framework.html#autotoc_md34", null ],
      [ "Changing the Active Stage", "md_core__framework.html#autotoc_md35", null ]
    ] ],
    [ "SDOM Display Object Anchoring Design", "md_display__object__anchoring.html", [
      [ "Display Object Anchoring System (Updated September 2, 2025)", "md_display__object__anchoring.html#autotoc_md38", [
        [ "Anchor Naming Conventions", "md_display__object__anchoring.html#autotoc_md39", null ],
        [ "Edge-Based Layout", "md_display__object__anchoring.html#autotoc_md40", null ],
        [ "Position and Size Calculation", "md_display__object__anchoring.html#autotoc_md41", null ],
        [ "Anchor Accessors/Mutators", "md_display__object__anchoring.html#autotoc_md42", null ],
        [ "Changing Anchor Points", "md_display__object__anchoring.html#autotoc_md43", null ],
        [ "Drag-and-Drop and Reparenting", "md_display__object__anchoring.html#autotoc_md44", null ],
        [ "Summary", "md_display__object__anchoring.html#autotoc_md45", null ]
      ] ],
      [ "Overview", "md_display__object__anchoring.html#autotoc_md46", null ],
      [ "Anchor Points", "md_display__object__anchoring.html#autotoc_md47", [
        [ "Reversed versions for user convenience", "md_display__object__anchoring.html#autotoc_md48", null ]
      ] ],
      [ "JSON Interface", "md_display__object__anchoring.html#autotoc_md49", null ],
      [ "Position and Size Calculation", "md_display__object__anchoring.html#autotoc_md50", null ],
      [ "Changing Anchor Points", "md_display__object__anchoring.html#autotoc_md51", null ],
      [ "Hierarchical Anchoring", "md_display__object__anchoring.html#autotoc_md52", null ],
      [ "Backward Compatibility", "md_display__object__anchoring.html#autotoc_md53", null ],
      [ "Benefits", "md_display__object__anchoring.html#autotoc_md54", null ],
      [ "Example Use Cases", "md_display__object__anchoring.html#autotoc_md55", null ]
    ] ],
    [ "DOM Propagation Design", "md_dom__propagation.html", [
      [ "Overview", "md_dom__propagation.html#autotoc_md58", null ],
      [ "Traversal Types", "md_dom__propagation.html#autotoc_md60", [
        [ "1. Core-based DOM Traversal (Type 1)", "md_dom__propagation.html#autotoc_md61", null ],
        [ "2. EventManager-based Event Traversal (Type 2)", "md_dom__propagation.html#autotoc_md63", null ],
        [ "3. Factory-based Global Traversal (Type 3)", "md_dom__propagation.html#autotoc_md65", null ],
        [ "4. World Coordinate Backpropagation Traversal (Type 4)", "md_dom__propagation.html#autotoc_md66", null ]
      ] ],
      [ "Traversal Safety and Modification", "md_dom__propagation.html#autotoc_md67", null ],
      [ "Architectural Separation", "md_dom__propagation.html#autotoc_md68", null ],
      [ "Next Steps", "md_dom__propagation.html#autotoc_md70", null ]
    ] ],
    [ "extensible_factory", "md_extensible__factory.html", [
      [ "Extensible Factory Design", "md_extensible__factory.html#autotoc_md71", [
        [ "Resource Type Management", "md_extensible__factory.html#autotoc_md72", null ],
        [ "Requirements", "md_extensible__factory.html#autotoc_md73", null ],
        [ "Recommended Approach", "md_extensible__factory.html#autotoc_md74", [
          [ "1. Initializer Structs and JSON Configs", "md_extensible__factory.html#autotoc_md75", null ],
          [ "2. Factory Registration", "md_extensible__factory.html#autotoc_md76", null ],
          [ "3. Factory Create Methods", "md_extensible__factory.html#autotoc_md77", null ],
          [ "4. Usage", "md_extensible__factory.html#autotoc_md78", null ]
        ] ],
        [ "Benefits", "md_extensible__factory.html#autotoc_md79", null ],
        [ "Implementation Strategy", "md_extensible__factory.html#autotoc_md81", [
          [ "1. Initializer Structs", "md_extensible__factory.html#autotoc_md82", null ],
          [ "1a. Standardized Constructor Signatures", "md_extensible__factory.html#autotoc_md83", null ],
          [ "2. Private Constructors & Friend Factory", "md_extensible__factory.html#autotoc_md84", null ],
          [ "3. Factory Registration System", "md_extensible__factory.html#autotoc_md85", null ],
          [ "4. Generic Factory Create Method", "md_extensible__factory.html#autotoc_md86", null ],
          [ "5. Extensibility", "md_extensible__factory.html#autotoc_md87", null ],
          [ "6. Error Handling & Validation", "md_extensible__factory.html#autotoc_md88", null ],
          [ "7. Example Usage", "md_extensible__factory.html#autotoc_md89", null ]
        ] ],
        [ "Standardized Constructors for IDisplayObjects", "md_extensible__factory.html#autotoc_md91", null ],
        [ "Configuration Format Choice", "md_extensible__factory.html#autotoc_md92", null ],
        [ "Resource Pointer (<tt>resource_ptr</tt>) Design", "md_extensible__factory.html#autotoc_md93", [
          [ "How It Works", "md_extensible__factory.html#autotoc_md94", null ],
          [ "Example Usage", "md_extensible__factory.html#autotoc_md95", null ],
          [ "Benefits", "md_extensible__factory.html#autotoc_md96", null ],
          [ "Implementation Outline", "md_extensible__factory.html#autotoc_md97", null ],
          [ "Summary", "md_extensible__factory.html#autotoc_md98", null ]
        ] ]
      ] ]
    ] ],
    [ "Inheritance Warning: Avoid Diamond Inheritance", "md_object__handles.html", [
      [ "Two Trees in the Garden", "md_object__handles.html#autotoc_md100", null ],
      [ "Object Handles in SDOM", "md_object__handles.html#autotoc_md101", [
        [ "Overview", "md_object__handles.html#autotoc_md102", null ],
        [ "Handle Types", "md_object__handles.html#autotoc_md103", [
          [ "1. DomHandle", "md_object__handles.html#autotoc_md104", null ],
          [ "2. ResHandle", "md_object__handles.html#autotoc_md105", null ]
        ] ],
        [ "Design Rationale", "md_object__handles.html#autotoc_md106", null ],
        [ "Example Usage", "md_object__handles.html#autotoc_md107", null ],
        [ "Migration Strategy", "md_object__handles.html#autotoc_md108", null ],
        [ "Best Practices", "md_object__handles.html#autotoc_md109", null ]
      ] ]
    ] ],
    [ "Project Description", "md_progress.html", [
      [ "Scripting Flexibility:", "md_progress.html#autotoc_md111", null ],
      [ "Progress Updates", "md_progress.html#autotoc_md112", null ],
      [ "Next Steps:", "md_progress.html#autotoc_md113", null ],
      [ "ToDo:", "md_progress.html#autotoc_md114", null ]
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
"classSDOM_1_1Core.html#a9a4483cf794e1c15ee2ac630dba94166",
"classSDOM_1_1Event.html#af3a872dc47ba1ee24660cbf978a3e7e8",
"classSDOM_1_1Handle.html#a37678d25be2a530a5ceb3159bbf70853",
"classSDOM_1_1IDisplayObject.html#ab1390908afede95eacb5dbf3333ddc9f",
"classSDOM_1_1Stage.html#a514bb1586b9cf3bc4f6043669cf049b2",
"functions_z.html",
"structSDOM_1_1Stage_1_1InitStruct.html#a17adc32c048d68ae0d26a9778f7f4827"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';