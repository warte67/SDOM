cmake_minimum_required(VERSION 3.20)
project(SDOM VERSION 1.0 LANGUAGES CXX)

# Help CMake find locally installed packages in both user and system prefixes.
# This ensures consistent package discovery for both Debug (local) and
# Release (system) workflows triggered by `./rebuild`.
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local" "/usr/local")

# Option to enable generation of Lua dispatch wrappers in generated C API
option(SDOM_ENABLE_LUA_BINDINGS "Enable generation of Lua dispatch prototypes/wrappers in generated C API" ON)
option(SDOM_ENABLE_RUNTIME_BINDING_EXPORT "Allow Factory to regenerate bindings at runtime" OFF)

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
file(GLOB_RECURSE SDOM_ALL_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

set(SDOM_GENERATED_CAPI_HEADER "${PROJECT_SOURCE_DIR}/include/SDOM/CAPI/SDOM_CAPI_Core.h")
set(SDOM_GENERATED_CAPI_SOURCE "${PROJECT_SOURCE_DIR}/src/CAPI/SDOM_CAPI_Core.cpp")

file(GLOB SDOM_CAPI_SOURCES "${PROJECT_SOURCE_DIR}/src/CAPI/*.cpp")
list(REMOVE_ITEM SDOM_CAPI_SOURCES ${SDOM_GENERATED_CAPI_SOURCE})
# Variant CAPI is implemented manually in src/SDOM_Variant.cpp; exclude generated stub.
list(REMOVE_ITEM SDOM_CAPI_SOURCES "${PROJECT_SOURCE_DIR}/src/CAPI/SDOM_CAPI_Variant.cpp")

set(SDOM_CORE_SOURCES ${SDOM_ALL_SOURCES})
list(REMOVE_ITEM SDOM_CORE_SOURCES ${SDOM_GENERATED_CAPI_SOURCE})
list(REMOVE_ITEM SDOM_CORE_SOURCES "${PROJECT_SOURCE_DIR}/src/CAPI/SDOM_CAPI_Variant.cpp")
foreach(capi_src ${SDOM_CAPI_SOURCES})
    list(REMOVE_ITEM SDOM_CORE_SOURCES ${capi_src})
endforeach()

# ======================================================
# nlohmann::json (header-only)
# ======================================================
add_library(nlohmann_json INTERFACE)

target_include_directories(nlohmann_json INTERFACE
    ${CMAKE_SOURCE_DIR}/include/external
)

# ======================================================
#  Core Object Library (shared between SDOM + generators)
# ======================================================
add_library(sdom_core_obj OBJECT ${SDOM_CORE_SOURCES})

if(SDOM_ENABLE_LUA_BINDINGS)
    target_compile_definitions(sdom_core_obj PRIVATE SDOM_ENABLE_LUA_BINDINGS=1)
else()
    target_compile_definitions(sdom_core_obj PRIVATE SDOM_ENABLE_LUA_BINDINGS=0)
endif()

if(SDOM_ENABLE_RUNTIME_BINDING_EXPORT)
    target_compile_definitions(sdom_core_obj PRIVATE SDOM_ENABLE_RUNTIME_BINDING_EXPORT=1)
endif()

# ======================================================
#  Build Library
# ======================================================
add_library(${PROJECT_NAME} STATIC
    $<TARGET_OBJECTS:sdom_core_obj>
    ${SDOM_GENERATED_CAPI_SOURCE}
    ${SDOM_CAPI_SOURCES}
)

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
#  Binding Generator Utility
# ======================================================
add_executable(sdom_generate_bindings
    tools/sdom_generate_bindings.cpp
    $<TARGET_OBJECTS:sdom_core_obj>
)

target_link_libraries(sdom_generate_bindings
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        $<$<BOOL:${SDL3_mixer_FOUND}>:SDL3_mixer::SDL3_mixer>
        ${LUA_LIB}
        sdom_warnings
        sdom_sanitizers
)

set(SDOM_CAPI_HEADER_DIR "${PROJECT_SOURCE_DIR}/include/SDOM/CAPI")
set(SDOM_CAPI_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/CAPI")

add_custom_target(generate_sdom_capi ALL
    COMMAND sdom_generate_bindings
            --header-dir "${SDOM_CAPI_HEADER_DIR}"
            --source-dir "${SDOM_CAPI_SOURCE_DIR}"
            --verbose
    DEPENDS sdom_generate_bindings
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating SDOM C API bindings"
)

add_dependencies(${PROJECT_NAME} generate_sdom_capi)

# ======================================================
#  Output and Install
# ======================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/SDOM DESTINATION include)

