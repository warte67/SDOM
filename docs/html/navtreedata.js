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
    [ "extensible_factory", "md_extensible__factory.html", [
      [ "Extensible Factory Design", "md_extensible__factory.html#autotoc_md37", [
        [ "Resource Type Management", "md_extensible__factory.html#autotoc_md38", null ],
        [ "Requirements", "md_extensible__factory.html#autotoc_md39", null ],
        [ "Recommended Approach", "md_extensible__factory.html#autotoc_md40", [
          [ "1. Initializer Structs and JSON Configs", "md_extensible__factory.html#autotoc_md41", null ],
          [ "2. Factory Registration", "md_extensible__factory.html#autotoc_md42", null ],
          [ "3. Factory Create Methods", "md_extensible__factory.html#autotoc_md43", null ],
          [ "4. Usage", "md_extensible__factory.html#autotoc_md44", null ]
        ] ],
        [ "Benefits", "md_extensible__factory.html#autotoc_md45", null ],
        [ "Implementation Strategy", "md_extensible__factory.html#autotoc_md47", [
          [ "1. Initializer Structs", "md_extensible__factory.html#autotoc_md48", null ],
          [ "1a. Standardized Constructor Signatures", "md_extensible__factory.html#autotoc_md49", null ],
          [ "2. Private Constructors & Friend Factory", "md_extensible__factory.html#autotoc_md50", null ],
          [ "3. Factory Registration System", "md_extensible__factory.html#autotoc_md51", null ],
          [ "4. Generic Factory Create Method", "md_extensible__factory.html#autotoc_md52", null ],
          [ "5. Extensibility", "md_extensible__factory.html#autotoc_md53", null ],
          [ "6. Error Handling & Validation", "md_extensible__factory.html#autotoc_md54", null ],
          [ "7. Example Usage", "md_extensible__factory.html#autotoc_md55", null ]
        ] ],
        [ "Standardized Constructors for IDisplayObjects", "md_extensible__factory.html#autotoc_md57", null ],
        [ "Configuration Format Choice", "md_extensible__factory.html#autotoc_md58", null ],
        [ "Resource Pointer (<tt>resource_ptr</tt>) Design", "md_extensible__factory.html#autotoc_md59", [
          [ "How It Works", "md_extensible__factory.html#autotoc_md60", null ],
          [ "Example Usage", "md_extensible__factory.html#autotoc_md61", null ],
          [ "Benefits", "md_extensible__factory.html#autotoc_md62", null ],
          [ "Implementation Outline", "md_extensible__factory.html#autotoc_md63", null ],
          [ "Summary", "md_extensible__factory.html#autotoc_md64", null ]
        ] ]
      ] ]
    ] ],
    [ "Project Description", "md_progress.html", [
      [ "Progress Updates", "md_progress.html#autotoc_md66", null ],
      [ "ToDo:", "md_progress.html#autotoc_md67", null ]
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
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"SDOM_8hpp.html",
"classSDOM_1_1Event.html#a40e2f80ca042eb2b73d3b63a2cb25652",
"classSDOM_1_1EventType.html#ae33aed4c8c5283f57310f57f0d89bb6d",
"classSDOM_1_1IDisplayObject.html#affc34595a0f3e17e51801c130caa30ab",
"namespaceSDOM.html#adca0aa068ac9fc59e78b78c430a92980a8d81ac82421d1b03da58fccb9174892e"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';