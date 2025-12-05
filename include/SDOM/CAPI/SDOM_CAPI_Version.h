// =============================================================================
//  SDOM C API binding — AUTO-GENERATED FILE. DO NOT EDIT.
//
//  File: SDOM_CAPI_Version.h
//  Module: Version
//
//  Brief:
//    Build/version metadata
// =============================================================================
//
//  Authors:
//    Jay Faries (warte67) - Primary architect of SDOM
//
//  File Type: Header
//  SDOM Version: 0.5.261 (early pre-alpha)
//  Build Identifier: 2025-12-04_18:57:31_9b0abf93
//  Commit: 9b0abf93 on branch master
//  Compiler: g++ (GCC) 15.2.1 20251112
//  Platform: Linux-x86_64
//  Generated: 2025-12-04_18:57:31
//  Generator: sdom_generate_bindings
//
//  License Notice:
//  *   This software is provided 'as-is', without any express or implied
//  *   warranty.  In no event will the authors be held liable for any damages
//  *   arising from the use of this software.
//  *
//  *   Permission is granted to anyone to use this software for any purpose,
//  *   including commercial applications, and to alter it and redistribute it
//  *   freely, subject to the following restrictions:
//  *
//  *   1. The origin of this software must not be misrepresented; you must not
//  *       claim that you wrote the original software. If you use this software
//  *       in a product, an acknowledgment in the product documentation would be
//  *       appreciated but is not required.
//  *   2. Altered source versions must be plainly marked as such, and must not be
//  *       misrepresented as being the original software.
//  *   3. This notice may not be removed or altered from any source distribution.
// =============================================================================

#pragma once

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
