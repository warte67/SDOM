cmake_minimum_required(VERSION 3.20)
project(SDOM VERSION 1.0 LANGUAGES CXX)

# Help CMake find locally installed packages in both user and system prefixes.
# This ensures consistent package discovery for both Debug (local) and
# Release (system) workflows triggered by `./rebuild`.
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local" "/usr/local")

# Option to enable generation of Lua dispatch wrappers in generated C API
option(SDOM_ENABLE_LUA_BINDINGS "Enable generation of Lua dispatch prototypes/wrappers in generated C API" ON)

# Option to treat warnings as errors for SDOM targets
option(SDOM_ENABLE_WERROR "Treat compiler warnings as errors for SDOM targets" OFF)

# ======================================================
#  Version Auto-Generation (SDOM_Version.hpp)
# ======================================================
set(GEN_VERSION_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/gen_version.sh")
set(VERSION_TEMPLATE   "${CMAKE_SOURCE_DIR}/include/SDOM/SDOM_Version.hpp.in")
set(VERSION_HEADER     "${CMAKE_SOURCE_DIR}/include/SDOM/SDOM_Version.hpp")
set(VERSION_TRIGGER    "${CMAKE_BINARY_DIR}/_version_trigger")

# ======================================================
#  Sanitizer Options
# ======================================================
option(ENABLE_ASAN  "Enable AddressSanitizer for builds" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer for builds" OFF)
option(ENABLE_TSAN  "Enable ThreadSanitizer for builds (incompatible with ASAN)" OFF)

if(ENABLE_ASAN AND ENABLE_TSAN)
    message(FATAL_ERROR "ENABLE_TSAN cannot be enabled together with ENABLE_ASAN")
endif()

# Small interface target to carry sanitizer compile/link flags.
add_library(sdom_sanitizers INTERFACE)

if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer: enabled")
    target_compile_options(sdom_sanitizers INTERFACE
        -fsanitize=address
        -fno-omit-frame-pointer
        -g -O1
    )
    target_link_options(sdom_sanitizers INTERFACE
        -fsanitize=address
    )
elseif(ENABLE_UBSAN)
    message(STATUS "UndefinedBehaviorSanitizer: enabled")
    target_compile_options(sdom_sanitizers INTERFACE
        -fsanitize=undefined
        -fno-sanitize-recover=undefined
        -g -O1
    )
    target_link_options(sdom_sanitizers INTERFACE
        -fsanitize=undefined
    )
elseif(ENABLE_TSAN)
    message(STATUS "ThreadSanitizer: enabled")
    target_compile_options(sdom_sanitizers INTERFACE
        -fsanitize=thread
        -g -O1
    )
    target_link_options(sdom_sanitizers INTERFACE
        -fsanitize=thread
    )
endif()

# ======================================================
#  Build Configuration
# ======================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Let CMake set its default Debug/Release flags, then layer on warnings.
# (CMake already adds -g/-O0 for Debug and -O3/-DNDEBUG for Release for GCC/Clang.)

# Common warning policy carried by an INTERFACE target
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
    message(STATUS "SDOM: treating warnings as errors (-Werror)")
    target_compile_options(sdom_warnings INTERFACE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
    )
endif()

# ======================================================
#  Version generator hook (currently disabled block stays as-is)
# ======================================================
if(EXISTS "${GEN_VERSION_SCRIPT}")

    find_program(BASH_EXECUTABLE bash)
    if(NOT BASH_EXECUTABLE)
        message(FATAL_ERROR "Bash not found! Required to run ${GEN_VERSION_SCRIPT}")
    endif()

    add_custom_command(
        OUTPUT "${VERSION_TRIGGER}"
        COMMAND ${CMAKE_COMMAND} -E touch "${VERSION_TRIGGER}"
        COMMENT "⏱ Updating version trigger"
    )

else()
    message(WARNING "⚠️ Version generator script not found: ${GEN_VERSION_SCRIPT}")
endif()

# ======================================================
#  Dependencies
# ======================================================
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
# SDL_mixer is optional for now
find_package(SDL3_mixer QUIET CONFIG)
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

# Propagate Lua bindings option as a compile-time definition for consumers
if(SDOM_ENABLE_LUA_BINDINGS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDOM_ENABLE_LUA_BINDINGS=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDOM_ENABLE_LUA_BINDINGS=0)
endif()

# Ensure the version generator runs before compilation (if you re-enable generate_version)
if(TARGET generate_version)
    add_dependencies(${PROJECT_NAME} generate_version)
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        $<$<BOOL:${SDL3_mixer_FOUND}>:SDL3_mixer::SDL3_mixer>
        ${LUA_LIB}
        sdom_warnings
        sdom_sanitizers
)

# ======================================================
#  Output and Install
# ======================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/SDOM DESTINATION include)

# ======================================================
#  Optional: dataregistry_generator to emit C API headers
#  Run when generated version differs from project version
# ======================================================
set(GENERATED_DIR "${CMAKE_BINARY_DIR}/capi_generated")
file(MAKE_DIRECTORY "${GENERATED_DIR}")

set(GENERATOR_SOURCES
    "${CMAKE_SOURCE_DIR}/tools/run_dataregistry_test.cpp"
    "${CMAKE_SOURCE_DIR}/src/SDOM_DataRegistry.cpp"
    "${CMAKE_SOURCE_DIR}/src/SDOM_CBindingGenerator.cpp"
    "${CMAKE_SOURCE_DIR}/src/SDOM_EventType.cpp"
    "${CMAKE_SOURCE_DIR}/tools/dataregistry_generator_stubs.cpp"
)

add_executable(dataregistry_generator ${GENERATOR_SOURCES})
target_include_directories(dataregistry_generator PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/sol2/include"
    "${CMAKE_SOURCE_DIR}/sol2/include/sol"
)
set_target_properties(dataregistry_generator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

# Give generator the same warnings/sanitizers (it’s your code, so strict is good)
target_link_libraries(dataregistry_generator PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    sdom_warnings
    sdom_sanitizers
)

# The generator may compile parts of the code that reference the Lua C API
if(LUA_LIB)
    find_package(Threads REQUIRED)
    target_link_libraries(dataregistry_generator PRIVATE
        ${LUA_LIB}
        ${CMAKE_DL_LIBS}
        Threads::Threads
    )
endif()

target_include_directories(dataregistry_generator PRIVATE
    /usr/include/lua5.4
)

# Generated version marker file
set(GENERATED_VERSION_FILE "${GENERATED_DIR}/sdom_capi_objects_generated.version")

add_custom_command(
    OUTPUT "${GENERATED_VERSION_FILE}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${GENERATED_DIR}"
    COMMAND $<TARGET_FILE:dataregistry_generator> "${GENERATED_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${GENERATED_VERSION_FILE}"
    DEPENDS dataregistry_generator
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Running dataregistry_generator to produce C API headers and version marker"
)
add_custom_target(run_dataregistry_generator DEPENDS "${GENERATED_VERSION_FILE}")

# Parse project SDOM version from the version header (if available)
set(SDOM_VERSION_STRING "")
if(EXISTS "${VERSION_HEADER}")
    file(READ "${VERSION_HEADER}" _ver_contents)
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

# Ensure the generator runs before building the library when needed.
if(NOT GENERATED_UP_TO_DATE)
    add_dependencies(${PROJECT_NAME} run_dataregistry_generator)
endif()

# Always add the generated dir to include directories so the compiler can
# find the generated header after the generator runs.
target_include_directories(${PROJECT_NAME} PRIVATE "${GENERATED_DIR}")

message(STATUS "SDOM C API generated dir: ${GENERATED_DIR}")

# ------------------------------------------------------------------
# Small regression test: verify generated header is producible and
# compilable (prevents regressions where a repo-level shim hides the
# build-generated header on clean builds).
# ------------------------------------------------------------------
set(GENERATED_HEADER_TEST_SRC "${CMAKE_SOURCE_DIR}/tests/generated_header_check.cpp")
if(EXISTS "${GENERATED_HEADER_TEST_SRC}")
    add_executable(generated_header_check ${GENERATED_HEADER_TEST_SRC})
    target_include_directories(generated_header_check PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${GENERATED_DIR}"
    )
    target_link_libraries(generated_header_check PRIVATE
        sdom_warnings
        sdom_sanitizers
    )
    add_dependencies(generated_header_check run_dataregistry_generator)
    add_test(NAME generated_header_check COMMAND generated_header_check)
endif()
