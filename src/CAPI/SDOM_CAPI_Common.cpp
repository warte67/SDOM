// Minimal implementations for C API string helpers used by generated C API
#include <cstdlib>
#include <cstring>
#include <SDOM/CAPI/SDOM_CAPI_Common.h>

extern "C" {

char* SDOM_StrDup(const char* s) {
    if (!s) return nullptr;
    size_t len = std::strlen(s);
    char* out = static_cast<char*>(std::malloc(len + 1));
    if (!out) return nullptr;
    std::memcpy(out, s, len + 1);
    return out;
}

void SDOM_FreeString(char* s) {
    if (!s) return;
    std::free(s);
}

} // extern "C"
