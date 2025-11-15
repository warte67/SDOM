cmake_minimum_required(VERSION 3.20)

project(prog VERSION 1.0 LANGUAGES CXX)

# ======================================================
# Options
# ======================================================
option(SDOM_ENABLE_WERROR "Treat warnings as errors" OFF)
option(ENABLE_ASAN  "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_TSAN  "Enable ThreadSanitizer" OFF)

# ======================================================
# Paths & Version Header
# ======================================================
set(SDOM_ROOT "${CMAKE_SOURCE_DIR}/../..")
set(VERSION_HEADER "${SDOM_ROOT}/include/SDOM/SDOM_Version.hpp")

if(EXISTS "${VERSION_HEADER}")
    message(STATUS "Using existing SDOM_Version.hpp: ${VERSION_HEADER}")
else()
    message(WARNING "SDOM_Version.hpp not found! Build SDOM first.")
endif()

# ======================================================
# Build setup (C++23)
# ======================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ======================================================
#  Warning set (same as main SDOM)
# ======================================================
add_library(sdom_warnings INTERFACE)
target_compile_options(sdom_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Wall
        -Wextra
        -Wpedantic
        -Wno-comment
        -Wno-reorder
    >
)

if(SDOM_ENABLE_WERROR)
    message(STATUS "SDOM: Warnings treated as errors (-Werror)")
    target_compile_options(sdom_warnings INTERFACE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
    )
endif()

# ======================================================
# Sanitizers (same design as SDOM library)
# ======================================================
if(ENABLE_ASAN AND ENABLE_TSAN)
    message(FATAL_ERROR "ASAN and TSAN cannot be used together")
endif()

add_library(sdom_sanitizers INTERFACE)

if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    target_compile_options(sdom_sanitizers INTERFACE
        -fsanitize=address -fno-omit-frame-pointer -g -O1
    )
    target_link_options(sdom_sanitizers INTERFACE -fsanitize=address)
elseif(ENABLE_UBSAN)
    message(STATUS "UndefinedBehaviorSanitizer enabled")
    target_compile_options(sdom_sanitizers INTERFACE
        -fsanitize=undefined -fno-sanitize-recover=undefined -g -O1
    )
    target_link_options(sdom_sanitizers INTERFACE -fsanitize=undefined)
elseif(ENABLE_TSAN)
    message(STATUS "ThreadSanitizer enabled")
    target_compile_options(sdom_sanitizers INTERFACE
        -fsanitize=thread -g -O1
    )
    target_link_options(sdom_sanitizers INTERFACE -fsanitize=thread)
endif()

# ======================================================
# Dependencies
# ======================================================
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_library(LUA_LIB lua5.4)
find_package(Threads REQUIRED)


# Prefer the repo's canonical SDOM headers before any example-local shims so
# that <SDOM/...> always resolves to the generated/public headers.
set(PROG_INCLUDE_DIRS
    ${SDOM_ROOT}/include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/test_include
    ${SDOM_ROOT}/sol2/include
    ${SDOM_ROOT}/sol2/include/sol
    /usr/include/lua5.4
)

# ======================================================
#  Resolve SDOM library to link to
# ======================================================
set(SDOM_BUILD_LIB "${SDOM_ROOT}/build/libSDOM.a")

if(EXISTS "${SDOM_BUILD_LIB}")
    message(STATUS "Linking against in-tree SDOM: ${SDOM_BUILD_LIB}")
    set(SDOM_LIB_PATH "${SDOM_BUILD_LIB}")
else()
    message(STATUS "Linking against installed SDOM: $ENV{HOME}/.local/lib/libSDOM.a")
    set(SDOM_LIB_PATH "$ENV{HOME}/.local/lib/libSDOM.a")
endif()

# ======================================================
#  Sources
# ======================================================
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/main.cpp")

# ======================================================
#  Test Program
# ======================================================
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROG_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${SDOM_LIB_PATH}
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        ${LUA_LIB}
        sdom_warnings
        sdom_sanitizers
)


# ======================================================
#  Generator for C API (“api_gen”)
# ======================================================
set(GENERATED_DIR "${PROJECT_SOURCE_DIR}/api_gen")
file(MAKE_DIRECTORY "${GENERATED_DIR}")

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
target_link_libraries(dataregistry_generator PRIVATE
    ${LUA_LIB}
    ${CMAKE_DL_LIBS}
    Threads::Threads
    sdom_warnings
    sdom_sanitizers
)

set_target_properties(dataregistry_generator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

# ======================================================
#  Versioned generation step
# ======================================================
set(GENERATED_VERSION_FILE "${GENERATED_DIR}/sdom_capi_objects_generated.version")

add_custom_command(
    OUTPUT "${GENERATED_VERSION_FILE}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${GENERATED_DIR}"
    COMMAND $<TARGET_FILE:dataregistry_generator> "${GENERATED_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${GENERATED_VERSION_FILE}"
    DEPENDS dataregistry_generator
    WORKING_DIRECTORY "${SDOM_ROOT}"
    COMMENT "Generating C API header (examples/test harness)"
)

add_custom_target(run_dataregistry_generator
    DEPENDS "${GENERATED_VERSION_FILE}"
)

# ======================================================
#  Parse version from SDOM_Version.hpp
# ======================================================
set(SDOM_VERSION_STRING "")
if(EXISTS "${VERSION_HEADER}")
    file(READ "${VERSION_HEADER}" _vc)
    string(REGEX MATCHALL "([0-9]+)" _nums "${_vc}")
    if(_nums)
        list(GET _nums 0 SDOM_VERSION_MAJOR)
        list(GET _nums 1 SDOM_VERSION_MINOR)
        list(GET _nums 2 SDOM_VERSION_PATCH)
        set(SDOM_VERSION_STRING "${SDOM_VERSION_MAJOR}.${SDOM_VERSION_MINOR}.${SDOM_VERSION_PATCH}")
    endif()
endif()

set(GENERATED_UP_TO_DATE OFF)
if(EXISTS "${GENERATED_VERSION_FILE}")
    file(READ "${GENERATED_VERSION_FILE}" _gver)
    string(STRIP "${_gver}" _gver)
    if(_gver STREQUAL SDOM_VERSION_STRING)
        set(GENERATED_UP_TO_DATE ON)
        message(STATUS "Generated C API is up-to-date (${_gver})")
    else()
        message(STATUS "C API version mismatch; regeneration required")
    endif()
else()
    message(STATUS "No C API header present; will generate")
endif()

if(NOT GENERATED_UP_TO_DATE)
    add_dependencies(${PROJECT_NAME} run_dataregistry_generator)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${GENERATED_DIR}")

# ======================================================
#  Summary
# ======================================================
message(STATUS "-----------------------------------------")
message(STATUS "SDOM Version        : ${SDOM_VERSION_STRING}")
message(STATUS "Generated C API Path: ${GENERATED_DIR}")
message(STATUS "Version Marker File : ${GENERATED_VERSION_FILE}")
message(STATUS "Regeneration Needed : ${GENERATED_UP_TO_DATE}")
message(STATUS "-----------------------------------------")
