#pragma once
// Auto-generated SDOM C API module: Version

typedef struct SDOM_Version {
    void* impl;                                     ///< Opaque pointer to underlying C++ instance.
} SDOM_Version;

#ifdef __cplusplus
extern "C" {
#endif

// Callable bindings
const char* SDOM_GetVersionString(void);
const char* SDOM_GetVersionFullString(void);
int SDOM_GetVersionMajor(void);
int SDOM_GetVersionMinor(void);
int SDOM_GetVersionPatch(void);
const char* SDOM_GetVersionCodename(void);
const char* SDOM_GetVersionBuild(void);
const char* SDOM_GetVersionBuildDate(void);
const char* SDOM_GetVersionCommit(void);
const char* SDOM_GetVersionBranch(void);
const char* SDOM_GetVersionCompiler(void);
const char* SDOM_GetVersionPlatform(void);

#ifdef __cplusplus
} // extern "C"
#endif
