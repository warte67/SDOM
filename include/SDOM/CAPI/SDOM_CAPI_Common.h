/* SDOM_CAPI_Common.h
 * Common C API definitions for SDOM generated C API.
 * Provides stable error codes and cross-language helpers.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SDOM_CAPI_OK 0
#define SDOM_CAPI_ERR_INVALID_HANDLE -1
#define SDOM_CAPI_ERR_NOT_FOUND -2
#define SDOM_CAPI_ERR_INVALID_ARG -3
#define SDOM_CAPI_ERR_INTERNAL -4

/* Allocate a malloc-based string copy for cross-ABI consumers.
 * Caller must free with `SDOM_FreeString` (which calls free()). */
char* SDOM_StrDup(const char* s);

/* Free strings allocated by the C API (malloc-based). */
void SDOM_FreeString(char* s);

#ifdef __cplusplus
}
#endif
