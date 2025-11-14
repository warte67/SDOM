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

# Dependencies
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_library(LUA_LIB lua5.4)
find_package(Threads REQUIRED)

# Include directories
include_directories($ENV{HOME}/.local/include)      # SDL3 Includes (local)
include_directories(${PROJECT_SOURCE_DIR}/include)  # SDOM Includes
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

# ======================================================
#  Optional: dataregistry_generator to emit C API header
# ======================================================
set(GENERATED_DIR "${PROJECT_SOURCE_DIR}/api_gen")
file(MAKE_DIRECTORY "${GENERATED_DIR}")

# Build a small generator executable from local sources (keeps it separate
# from the main library and avoids circular linking).
set(GENERATOR_SOURCES
    "${SDOM_ROOT}/tools/run_dataregistry_test.cpp"
    "${SDOM_ROOT}/src/SDOM_DataRegistry.cpp"
    "${SDOM_ROOT}/src/SDOM_CBindingGenerator.cpp"
    "${SDOM_ROOT}/src/SDOM_EventType.cpp"
    "${SDOM_ROOT}/tools/dataregistry_generator_stubs.cpp"
)
add_executable(dataregistry_generator ${GENERATOR_SOURCES})
target_include_directories(dataregistry_generator PRIVATE
    "${SDOM_ROOT}/include"
    "${SDOM_ROOT}/sol2/include"
    "${SDOM_ROOT}/sol2/include/sol"
)
set_target_properties(dataregistry_generator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

# Link the generator against Lua and system libs so symbols from sol2
# and the Lua C API are resolved when the generator is built as part
# of the examples/test build flow.
target_link_libraries(dataregistry_generator
    PRIVATE
        ${LUA_LIB}
        ${CMAKE_DL_LIBS}
        Threads::Threads
)

# Custom command: run the generator to produce the version marker
set(GENERATED_VERSION_FILE "${GENERATED_DIR}/sdom_capi_objects_generated.version")
add_custom_command(
    OUTPUT "${GENERATED_VERSION_FILE}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${GENERATED_DIR}"
    COMMAND $<TARGET_FILE:dataregistry_generator> "${GENERATED_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${GENERATED_VERSION_FILE}"
    DEPENDS dataregistry_generator
    WORKING_DIRECTORY "${SDOM_ROOT}"
    COMMENT "Running dataregistry_generator to produce C API header and version marker"
)
add_custom_target(run_dataregistry_generator DEPENDS "${GENERATED_VERSION_FILE}")

# Parse project SDOM version from the version header (if available)
set(SDOM_VERSION_STRING "")
if(EXISTS "${VERSION_HEADER}")
    file(READ "${VERSION_HEADER}" _ver_contents)
    # Extract the first three numeric tokens from the version header; this
    # is a pragmatic way to get MAJOR.MINOR.PATCH without complex regexes.
    string(REGEX MATCHALL "([0-9]+)" _nums "${_ver_contents}")
    if(_nums)
        list(GET _nums 0 SDOM_VERSION_MAJOR)
        list(GET _nums 1 SDOM_VERSION_MINOR)
        list(GET _nums 2 SDOM_VERSION_PATCH)
        set(SDOM_VERSION_STRING "${SDOM_VERSION_MAJOR}.${SDOM_VERSION_MINOR}.${SDOM_VERSION_PATCH}")
    endif()
endif()

# If a generated version file already exists, read it and compare; if it
# matches the project SDOM version we can skip running the generator.
set(GENERATED_UP_TO_DATE OFF)
if(EXISTS "${GENERATED_VERSION_FILE}")
    file(READ "${GENERATED_VERSION_FILE}" _gver)
    string(STRIP "${_gver}" _gver)
    if(_gver STREQUAL SDOM_VERSION_STRING)
        message(STATUS "Generated C API header is up-to-date: ${_gver}")
        set(GENERATED_UP_TO_DATE ON)
    else()
        message(STATUS "Generated C API header version (${_gver}) differs from project version (${SDOM_VERSION_STRING}); generator will run")
    endif()
else()
    message(STATUS "No generated C API header found; generator will run")
endif()

# Ensure the generator runs before building the test binary when needed.
if(NOT GENERATED_UP_TO_DATE)
    add_dependencies(${PROJECT_NAME} run_dataregistry_generator)
endif()

# Always add the generated dir to include directories so the compiler can
# find the generated header after the generator runs.
target_include_directories(${PROJECT_NAME} PRIVATE "${GENERATED_DIR}")

# Report status so users can quickly see version and generated header state
message(STATUS "-----------------------------------------")
message(STATUS "SDOM Version        : ${SDOM_VERSION_STRING}")
message(STATUS "Generated C API Path: ${GENERATED_DIR}")
message(STATUS "Version Marker File : ${GENERATED_VERSION_FILE}")
message(STATUS "Regeneration Needed : ${GENERATED_UP_TO_DATE}")
message(STATUS "-----------------------------------------")
