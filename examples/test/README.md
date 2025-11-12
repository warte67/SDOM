# Examples / Test harness

This directory contains the `prog` test harness and helper scripts for
running unit tests and generating C API headers for external bindings.

Generation workflow
-------------------
- The generator writes files into `examples/test/api_gen` by default.
- To regenerate the C API header and a version marker, run the wrapper:

  ./tools/run_c_binding_gen.sh

  This will prefer a locally-built helper if available and write the
  generated header plus a `.version` marker into `examples/test/api_gen`.

Two-step development workflow
-----------------------------
If you modify any parts of the project that affect the C API surface:

1. Update the project version (run the `ver` or `scripts/gen_version.sh` script
   so `include/SDOM/SDOM_Version.hpp` reflects the change).
2. Regenerate the C header:

   ./tools/run_c_binding_gen.sh

3. Rebuild the test harness (the tests will pick up the generated header):

   cmake -S examples/test -B examples/test/build
   cmake --build examples/test/build -j

CI considerations
-----------------
- CI should run the generator step (or invoke the CMake target that runs it)
  before compiling the tests to ensure the generated header matches the
  repository version. The examples/test CMakeLists includes a small
  `dataregistry_generator` target and will run the generator when the
  version marker is missing or out of date.

Notes
-----
- Generated artifacts live in `examples/test/api_gen` to be persistent across
  clean builds and easy to inspect.
- The generated header contains version macros (e.g. `SDOM_CAPI_GENERATED_VERSION_STRING`)
  and a `.version` file for quick parsing in build scripts.
