cmake_minimum_required(VERSION 3.20)

# Project name and version
project(prog VERSION 1.0 LANGUAGES CXX)

# ======================================================
#  Depend on the generated SDOM_Version.hpp (do NOT regenerate)
# ======================================================
set(SDOM_ROOT "${CMAKE_SOURCE_DIR}/../..")
set(VERSION_HEADER "${SDOM_ROOT}/include/SDOM/SDOM_Version.hpp")

if(EXISTS "${VERSION_HEADER}")
    message(STATUS "Using existing SDOM_Version.hpp at: ${VERSION_HEADER}")
else()
    message(WARNING "SDOM_Version.hpp not found! Build the SDOM library first.")
endif()

# ======================================================
#  Build setup
# ======================================================

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Debug and release flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wno-comment -Wextra -Wno-reorder -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -Wextra -Wno-comment -Wno-reorder -Wno-unused-parameter -DNDEBUG")

# Optional sanitizers for the test app (useful for Release-only crashes)
option(ENABLE_ASAN "Enable AddressSanitizer for test app" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer for test app" OFF)

# Dependencies
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_library(LUA_LIB lua5.4)
find_package(Threads REQUIRED)

# Include directories
include_directories($ENV{HOME}/.local/include)      # SDL3 Includes (local)
include_directories(${SDOM_ROOT}/include)           # In-tree SDOM public headers (C++ + generated C API)
include_directories(${PROJECT_SOURCE_DIR}/include)  # SDOM test harness helpers
include_directories(${PROJECT_SOURCE_DIR}/test_include)  # test-only C API includes
include_directories(${PROJECT_SOURCE_DIR}/../../sol2/include)
include_directories(${PROJECT_SOURCE_DIR}/../../sol2/include/sol)
include_directories(/usr/include/lua5.4)

# Source files
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/main.cpp")

# Build the test executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Depend on the version header so test harness waits for it
if(EXISTS "${VERSION_HEADER}")
    add_custom_target(version_check DEPENDS "${VERSION_HEADER}")
    add_dependencies(${PROJECT_NAME} version_check)
endif()

# Link to SDOM static library: prefer in-tree build output, fallback to user install
set(SDOM_BUILD_LIB "${SDOM_ROOT}/build/libSDOM.a")
if(EXISTS "${SDOM_BUILD_LIB}")
    message(STATUS "Linking against in-tree SDOM: ${SDOM_BUILD_LIB}")
    set(SDOM_LIB_PATH "${SDOM_BUILD_LIB}")
else()
    message(STATUS "Linking against user SDOM install: $ENV{HOME}/.local/lib/libSDOM.a")
    set(SDOM_LIB_PATH "$ENV{HOME}/.local/lib/libSDOM.a")
endif()

target_link_libraries(prog
    PRIVATE
        ${SDOM_LIB_PATH}
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        ${LUA_LIB}
)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# Apply sanitizers to the test executable if requested
if(ENABLE_ASAN)
    message(STATUS "Test app: AddressSanitizer enabled")
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer -g -O1)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif()
if(ENABLE_UBSAN)
    message(STATUS "Test app: UndefinedBehaviorSanitizer enabled")
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined -fno-sanitize-recover=undefined -g -O1)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
endif()


# Report status so users can quickly see version and generated header state
message(STATUS "-----------------------------------------")
message(STATUS "SDOM Version        : ${SDOM_VERSION_STRING}")
message(STATUS "Generated C API Path: ${GENERATED_DIR}")
message(STATUS "Version Marker File : ${GENERATED_VERSION_FILE}")
message(STATUS "Regeneration Needed : ${GENERATED_UP_TO_DATE}")
message(STATUS "-----------------------------------------")
