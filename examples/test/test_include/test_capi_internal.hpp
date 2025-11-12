// Internal header used by tests to access the concrete TestHandle returned by
// the C API wrapper. This is intentionally in the test tree only.
#ifndef TEST_CAPI_INTERNAL_HPP
#define TEST_CAPI_INTERNAL_HPP

#include <SDOM/SDOM_DisplayHandle.hpp>

struct TestHandle {
    SDOM::DisplayHandle h;
};

#endif // TEST_CAPI_INTERNAL_HPP
