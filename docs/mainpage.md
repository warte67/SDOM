# SDOM Documentation

Welcome to the SDOM documentation site.

SDOM (Simple SDL Document Object Model API) is a compact, modern C++23 library that provides a DOM-like API for building UIs and interactive elements on top of SDL3. This documentation includes:

- API reference generated from the public headers
- Design notes and guides (architecture overview, core framework, display object anchoring, etc.)
- Progress updates and roadmaps

Quick links

- Repository: [SDOM on GitHub](https://github.com/warte67/SDOM)
- Build & docs generator: `examples/test/dox` (run from the repo root)
- Progress: `docs/progress.md`

To regenerate docs locally (recommended):

```bash
# prefer SVG diagrams and skip PNG by default
MERMAID_SKIP_PNG=1 bash examples/test/dox
```

To regenerate and commit the generated HTML (only do this when you want to update the published site):

```bash
bash examples/test/dox --commit
```

This page is shown as the documentation main page. If you prefer the repository `README.md` to be used instead, we can switch `docs/Doxyfile` back to `USE_MDFILE_AS_MAINPAGE = README.md`.

---

Enjoy exploring the API and design notes. If something looks off, open an issue or a PR in the GitHub repo.
