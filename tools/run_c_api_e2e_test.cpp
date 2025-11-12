// Minimal runner to execute only the C API end-to-end unit test without
// loading the full examples/test Lua config. Built as a small executable for
// developers to run the isolated test.

#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>

// Forward-declare registration function from the test translation unit.
namespace SDOM { bool C_API_EndToEnd_UnitTests(); }

int main(int argc, char** argv)
{
    // Ensure Core singleton is constructed (initializes subsystems needed by tests)
    SDOM::Core& core = SDOM::getCore(); (void)core;

    // Clear any pre-registered tests and register only our end-to-end test.
    SDOM::UnitTests::getInstance().clear_tests();
    SDOM::C_API_EndToEnd_UnitTests();

    // Run all tests for our module and return status code
    bool ok = SDOM::UnitTests::getInstance().run_all("C_API_EndToEnd");
    return ok ? 0 : 1;
}
