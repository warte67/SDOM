#pragma once
// Auto-generated SDOM C API module: Version

#include <stdbool.h>

typedef struct SDOM_Version {
    void* impl;                                     ///< Opaque pointer to underlying C++ instance.
} SDOM_Version;

#ifdef __cplusplus
extern "C" {
#endif

// Callable bindings

/**
 * @brief Returns the compact semantic version (e.g., v0.5.244).
 *
 * C++:   std::string Version::toString() const
 * C API: const char* SDOM_GetVersionString(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionString(void);

/**
 * @brief Returns the verbose build string with codename and metadata.
 *
 * C++:   std::string Version::fullString() const
 * C API: const char* SDOM_GetVersionFullString(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionFullString(void);

/**
 * @brief Returns the major semantic version number.
 *
 * C++:   int Version::getMajor() const
 * C API: int SDOM_GetVersionMajor(void)
 *
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetVersionMajor(void);

/**
 * @brief Returns the minor semantic version number.
 *
 * C++:   int Version::getMinor() const
 * C API: int SDOM_GetVersionMinor(void)
 *
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetVersionMinor(void);

/**
 * @brief Returns the patch/build counter for the current release.
 *
 * C++:   int Version::getPatch() const
 * C API: int SDOM_GetVersionPatch(void)
 *
 * @return int; check SDOM_GetError() for details on failure.
 */
int SDOM_GetVersionPatch(void);

/**
 * @brief Returns the codename associated with this build.
 *
 * C++:   std::string Version::getCodename() const
 * C API: const char* SDOM_GetVersionCodename(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionCodename(void);

/**
 * @brief Returns the unique build identifier (timestamp + commit).
 *
 * C++:   std::string Version::getBuild() const
 * C API: const char* SDOM_GetVersionBuild(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionBuild(void);

/**
 * @brief Returns the UTC build timestamp for this binary.
 *
 * C++:   std::string Version::getBuildDate() const
 * C API: const char* SDOM_GetVersionBuildDate(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionBuildDate(void);

/**
 * @brief Returns the git commit hash used for this build.
 *
 * C++:   std::string Version::getCommit() const
 * C API: const char* SDOM_GetVersionCommit(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionCommit(void);

/**
 * @brief Returns the git branch name captured during build.
 *
 * C++:   std::string Version::getBranch() const
 * C API: const char* SDOM_GetVersionBranch(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionBranch(void);

/**
 * @brief Returns the compiler string used to produce this build.
 *
 * C++:   std::string Version::getCompiler() const
 * C API: const char* SDOM_GetVersionCompiler(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionCompiler(void);

/**
 * @brief Returns the platform triplet captured during build.
 *
 * C++:   std::string Version::getPlatform() const
 * C API: const char* SDOM_GetVersionPlatform(void)
 *
 * @return const char*; check SDOM_GetError() for details on failure.
 */
const char* SDOM_GetVersionPlatform(void);

#ifdef __cplusplus
} // extern "C"
#endif
