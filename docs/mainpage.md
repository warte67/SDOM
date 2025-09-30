## SDOM Documentation

SDOM (Simple SDL Document Object Model API) is a compact, modern C++23 library that provides a structured approach to building graphical user interfaces and interactive scenes using SDL3. SDOM fully supports SDL3. The Document Object Model is, in short, a robust graphical user-interface system: a tree of display objects and containers with well-defined properties, parent/child relationships, event propagation, and lifecycle semantics—allowing code to traverse, query, and update elements in a declarative, DOM-like way similar to web DOM concepts but tailored to SDL rendering and input. 

The design centers on three core ideas: composability, data-driven configuration, and portability. Composability comes from a small collection of display object primitives and handles that can be composed into complex UIs. Data-driven configuration means scenes and object properties can be created and adjusted from Lua (via Sol2) without recompiling — as much or as little Lua as you want can be used to drive an application. We plan to provide dedicated SDL3 bindings for Lua so scripting can directly leverage SDL3 features; that work is in progress. Importantly, SDL3 is already fully available to C and C++ code using SDOM, and first-class bindings for other host languages (for example Rust or Python) are planned for future releases.

For authors and maintainers, SDOM provides full API reference generated from the public headers along with design notes and worked examples. The documentation pages include architecture overviews, anchoring/layout explanations, and annotated examples that show how to create, initialize, and test display objects. Common tasks—initializing Core, registering factories, wiring input events, and driving unit tests—are demonstrated in the examples to shorten the learning curve. 

Note: SDOM is a work in progress. While the library is suitable for experimentation and small integrations, it is not yet recommended for extensive production use. Please open issues or pull requests if you try something that doesn't work; feedback is very welcome.  

See the project progress notes: [`docs/progress.md`](progress.md)

If you are evaluating SDOM for a project, start with the `architecture_overview.md` and `core_framework.md` pages, then explore the API reference for the specific interfaces you need. The GitHub repository contains the source, build scripts, and the example test app; use the `examples/test` harness to experiment quickly. For questions or contributions, open an issue or submit a pull request on the repository.

View the project on GitHub: [warte67/SDOM](https://github.com/warte67/SDOM)
