cmake_minimum_required(VERSION 3.20)

# ======================================================
#  Project Setup
# ======================================================
project(SDOM VERSION 1.0 LANGUAGES CXX)

# ======================================================
#  Version Auto-Generation (SDOM_Version.hpp)
# ======================================================
set(GEN_VERSION_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/gen_version.sh")
set(VERSION_HEADER "${CMAKE_SOURCE_DIR}/include/SDOM/SDOM_Version.hpp")

if(EXISTS "${GEN_VERSION_SCRIPT}")

    # Gather all SDOM source and header files
    file(GLOB_RECURSE SDOM_SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/include/SDOM/*.hpp"
    )

    # ------------------------------------------------------
    # 🔧 Version Generation Behavior (Choose ONE mode)
    # ------------------------------------------------------
    #
    # 1️⃣  Generate only if missing or script changes:
    #     (Conservative, minimal rebuilds)
    #     ```
    #     add_custom_command(
    #         OUTPUT "${VERSION_HEADER}"
    #         COMMAND "${GEN_VERSION_SCRIPT}"
    #         WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    #         DEPENDS "${GEN_VERSION_SCRIPT}"
    #         COMMENT "Generating SDOM_Version.hpp (only if needed)"
    #         VERBATIM
    #     )
    #     add_custom_target(generate_version DEPENDS "${VERSION_HEADER}")
    #     ```
    #
    # 2️⃣  Generate when any SDOM source/header changes:
    #     (Current default — bump version when API changes)
    #     ✅ Best balance between automation and stability
    #
    add_custom_command(
        OUTPUT "${VERSION_HEADER}"
        COMMAND "${GEN_VERSION_SCRIPT}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        DEPENDS "${GEN_VERSION_SCRIPT}" ${SDOM_SOURCE_FILES}
        COMMENT "Regenerating SDOM_Version.hpp (source files changed)"
        VERBATIM
    )
    add_custom_target(generate_version DEPENDS "${VERSION_HEADER}")
    #
    # 3️⃣  Generate every build (always bump version):
    #     ```
    #     add_custom_target(generate_version ALL
    #         COMMAND "${GEN_VERSION_SCRIPT}"
    #         WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    #         COMMENT "Regenerating SDOM_Version.hpp (runs every build)"
    #         VERBATIM
    #     )
    #     ```
    # ------------------------------------------------------

    message(STATUS "✅ SDOM version will regenerate when source files change")

else()
    message(WARNING "⚠️ Version generator script not found: ${GEN_VERSION_SCRIPT}")
endif()

# ======================================================
#  Build Configuration
# ======================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wno-comment -Wextra -Wno-reorder -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -Wextra -Wno-comment -Wno-reorder -Wno-unused-parameter -DNDEBUG")

# ======================================================
#  Dependencies
# ======================================================
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_library(LUA_LIB lua5.4)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/sol2/include)
include_directories(${PROJECT_SOURCE_DIR}/sol2/include/sol)
include_directories(/usr/include/lua5.4)

# ======================================================
#  Source Files
# ======================================================
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# ======================================================
#  Build Library
# ======================================================
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Ensure the version generator runs before compilation
if(TARGET generate_version)
    add_dependencies(${PROJECT_NAME} generate_version)
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        ${LUA_LIB}
)

# ======================================================
#  Output and Install
# ======================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/SDOM DESTINATION include)
