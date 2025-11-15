/* Repository-level shim for generated C API */
#ifndef SDOM_CAPI_OBJECTS_GENERATED_H
#define SDOM_CAPI_OBJECTS_GENERATED_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__has_include)
  #if __has_include("../../../build/capi_generated/sdom_capi_objects.h")
    #include "../../../build/capi_generated/sdom_capi_objects.h"
  #elif __has_include(<SDOM/CAPI/sdom_capi_objects.h>)
    #include <SDOM/CAPI/sdom_capi_objects.h>
  #else
    typedef void* sdom_handle_t; /* minimal fallback */
  #endif
#else
  #if __has_include(<SDOM/CAPI/sdom_capi_objects.h>)
    #include <SDOM/CAPI/sdom_capi_objects.h>
  #else
    typedef void* sdom_handle_t; /* minimal fallback */
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif // SDOM_CAPI_OBJECTS_GENERATED_H
