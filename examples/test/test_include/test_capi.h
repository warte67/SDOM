// Small test-only C API for exercising cross-language boundary in unit tests
#ifndef TEST_CAPI_H
#define TEST_CAPI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void* sdom_handle_t;

// Create a Label-like display object for tests. Returns an opaque handle.
sdom_handle_t sdom_test_create_label(const char* name);

// Get the current name of the object. Returned pointer is owned by the
// underlying C++ object and should be copied if needed.
const char* sdom_test_get_name(sdom_handle_t);

// Set the object's name via the C API wrapper.
void sdom_test_set_name(sdom_handle_t, const char* newName);

// Destroy the test handle and underlying object.
void sdom_test_destroy(sdom_handle_t);

#ifdef __cplusplus
}
#endif

#endif // TEST_CAPI_H
