#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_IBindingGenerator.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

#include "../test_include/test_capi.h"
#include "../test_include/test_capi_internal.hpp"

#include <SDOM/SDOM_Label.hpp>
#include <memory>

using namespace SDOM;

// ---------------------------------------------------------------------------
// C wrapper implementations (moved here from C_API_EndToEnd.cpp)
// ---------------------------------------------------------------------------
extern "C" {

sdom_handle_t sdom_test_create_label(const char* name)
{
    if (name == nullptr) {
        name = "capi_label";
    }
    // Create a Label with minimal InitStruct and return a heap-allocated
    // TestHandle that contains the DisplayHandle.
    // Use the Label-specific InitStruct so we can provide a font resource
    // and avoid noisy debug output about missing fonts in unit tests.
    SDOM::Label::InitStruct init;
    init.name = std::string(name);
    init.type = std::string("Label");
    // Provide a sane default font resource used by the codebase tests.
    // Label::InitStruct already defaults this to "internal_font_8x8", but
    // being explicit here makes the intent clear and avoids depending on
    // the Label default constructor in case this code is refactored.
    init.resourceName = std::string("internal_font_8x8");
    DisplayHandle h = Core::getInstance().createDisplayObject("Label", init);
    TestHandle* th = new TestHandle{h};
    return reinterpret_cast<sdom_handle_t>(th);
}

const char* sdom_test_get_name(sdom_handle_t handle)
{
    if (handle == nullptr) {
        return nullptr;
    }
    TestHandle* th = reinterpret_cast<TestHandle*>(handle);
    if (!th->h.isValid() || !th->h.get()) {
        return nullptr;
    }
    // getName() returns a std::string by value; returning c_str() from that
    // temporary yields a dangling pointer. Use a thread_local string to hold
    // a stable copy for the caller (tests in this repo do not free the
    // pointer). This keeps the API simple for test usage.
    thread_local std::string name_buf;
    name_buf = th->h.get()->getName();
    return name_buf.c_str();
}

void sdom_test_set_name(sdom_handle_t handle, const char* newName)
{
    if (handle == nullptr || newName == nullptr) {
        return;
    }
    TestHandle* th = reinterpret_cast<TestHandle*>(handle);
    if (!th->h.isValid() || !th->h.get()) {
        return;
    }
    th->h.get()->setName(std::string(newName));
}

void sdom_test_destroy(sdom_handle_t handle)
{
    if (handle == nullptr) {
        return;
    }
    TestHandle* th = reinterpret_cast<TestHandle*>(handle);
    // destroy underlying object by name via Core so lifecycle is correct
    if (th->h.isValid() && th->h.get()) {
        std::string nm = th->h.getName();
        Core::getInstance().destroyDisplayObject(nm);
    }
    delete th;
}

} // extern "C"


// ---------------------------------------------------------------------------
// Legacy test0 + Lua test (moved from DataRegistry_UnitTest.cpp)
// ---------------------------------------------------------------------------

// Example template test from the original file. Keep it here so there's a
// single canonical DataRegistry_UnitTests() implementation for the module.
bool DataRegistry_test0([[maybe_unused]] std::vector<std::string>& errors)
{
    // Placeholder - keep returning finished=true so the harness is stable.
    return true;
}


bool DataRegistry_LUA_Tests(std::vector<std::string>& errors)
{
    return UnitTests::getInstance().run_lua_tests(errors, "src/DataRegistry_UnitTests.lua");
}

// ---------------------------------------------------------------------------
// Deadlock regression test (moved here)
// ---------------------------------------------------------------------------
class DeadlockGenerator : public IBindingGenerator {
public:
    bool generate([[maybe_unused]] const DataRegistrySnapshot& snapshot, const std::string& outDir) override {
        // Intentionally call back into the live registry (misuse) to reproduce
        // the historical deadlock where generateBindings held the mutex.
        auto names = DataRegistry::instance().listTypes();
        std::filesystem::create_directories(outDir);
        std::ofstream(std::filesystem::path(outDir) / ".deadlock_marker") << "ok";
        (void)names;
        return true;
    }
};

// ---------------------------------------------------------------------------
// C API end-to-end test (moved here)
// ---------------------------------------------------------------------------

// Forward declare test implementations used by the UnitTests harness.
namespace SDOM {
    bool DataRegistry_Deadlock_Regression_test_impl(std::vector<std::string>& errors);
    bool C_API_EndToEnd_test_impl(std::vector<std::string>& errors);
}

namespace SDOM {

bool DataRegistry_UnitTests()
{
    const std::string objName = "DataRegistry";
    UnitTests& ut = UnitTests::getInstance();

    static bool registered = false;
    if (!registered) {
        ut.add_test(objName, "Test DataRegistry (template)", DataRegistry_test0);

        // Generator / integration tests
        ut.add_test(objName, "Generator: deadlock regression", DataRegistry_Deadlock_Regression_test_impl);
        ut.add_test(objName, "C API end-to-end: create/get/set/restore name", C_API_EndToEnd_test_impl);
        registered = true;

        // LUA Tests
        ut.add_test(objName, "Lua: src/DataRegistry_UnitTests.lua", DataRegistry_LUA_Tests, false);

    }
    return true;
}

bool DataRegistry_Deadlock_Regression_test_impl(std::vector<std::string>& errors)
{
    DataRegistry reg;
    TypeInfo ti; ti.name = "DD_TestType"; reg.registerType(ti);

    std::string outDir = "build/test_out_dataregistry_deadlock";
    try { std::filesystem::remove_all(outDir); } catch(...) {}

    reg.addGenerator(std::make_unique<DeadlockGenerator>());

    bool ok = reg.generateBindings(outDir);
    if (!ok) { errors.push_back("generateBindings() returned false"); return true; }

    if (!std::filesystem::exists(std::filesystem::path(outDir) / ".deadlock_marker")) {
        errors.push_back("deadlock marker not written by generator");
    }

    return true;
}

bool C_API_EndToEnd_test_impl(std::vector<std::string>& errors)
{
    // Create object via the C wrapper
    const char* initial = "capi_label_original";
    sdom_handle_t h = sdom_test_create_label(initial);
    if (!h) { errors.push_back("sdom_test_create_label returned null"); return true; }

    const char* name_c = sdom_test_get_name(h);
    if (!name_c) { errors.push_back("sdom_test_get_name returned null"); sdom_test_destroy(h); return true; }
    if (std::strcmp(name_c, initial) != 0) {
        errors.push_back(std::string("Initial name mismatch: expected '") + initial + "' got '" + name_c + "'");
        sdom_test_destroy(h);
        return true;
    }

    // Set a new name via C API
    const char* newname = "capi_label_renamed";
    sdom_test_set_name(h, newname);

    // Verify via the underlying C++ DisplayHandle through the internal TestHandle
    TestHandle* th = reinterpret_cast<TestHandle*>(h);
    if (!th) { errors.push_back("internal TestHandle pointer is null"); sdom_test_destroy(h); return true; }
    if (!th->h.isValid() || !th->h.get()) { errors.push_back("DisplayHandle invalid after rename"); sdom_test_destroy(h); return true; }

    std::string cpp_name = th->h.get()->getName();
    if (cpp_name != std::string(newname)) {
        errors.push_back(std::string("C++ getName() mismatch after set: expected '") + newname + "' got '" + cpp_name + "'");
        sdom_test_destroy(h);
        return true;
    }

    // Restore original name via C API and verify round-trip
    sdom_test_set_name(h, initial);
    const char* restored = sdom_test_get_name(h);
    if (!restored) { errors.push_back("sdom_test_get_name returned null after restore"); sdom_test_destroy(h); return true; }
    if (std::strcmp(restored, initial) != 0) {
        errors.push_back(std::string("Restored name mismatch: expected '") + initial + "' got '" + restored + "'");
        sdom_test_destroy(h);
        return true;
    }

    // Clean up
    sdom_test_destroy(h);

    return true;
}

} // namespace SDOM
