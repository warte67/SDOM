cmake_minimum_required(VERSION 3.20)
project(SDOM VERSION 1.0 LANGUAGES CXX)

# Option to enable generation of Lua dispatch wrappers in generated C API
option(SDOM_ENABLE_LUA_BINDINGS "Enable generation of Lua dispatch prototypes/wrappers in generated C API" ON)

# ======================================================
#  Version Auto-Generation (SDOM_Version.hpp)
# ======================================================
set(GEN_VERSION_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/gen_version.sh")
set(VERSION_TEMPLATE "${CMAKE_SOURCE_DIR}/include/SDOM/SDOM_Version.hpp.in")
set(VERSION_HEADER "${CMAKE_SOURCE_DIR}/include/SDOM/SDOM_Version.hpp")
set(VERSION_TRIGGER "${CMAKE_BINARY_DIR}/_version_trigger")

option(ENABLE_ASAN "Enable AddressSanitizer for builds" OFF)

if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer: enabled")
    # Add sanitizer flags for compile and link
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer -g -O1)
    # Some toolchains need the link flags explicitly
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
    # Do NOT set CMAKE_STATIC_LINKER_FLAGS: passing linker flags to the
    # static-archive tool (ar) causes those flags to be interpreted as
    # ar options and can break archive creation (see ar: invalid option).
    # Static libraries only need object files compiled with ASAN; the
    # executable link step is where the sanitizer runtime must be linked.
endif()


if(EXISTS "${GEN_VERSION_SCRIPT}")

    # --- Ensure Bash is used instead of /bin/sh (dash) ---
    find_program(BASH_EXECUTABLE bash)
    if(NOT BASH_EXECUTABLE)
        message(FATAL_ERROR "Bash not found! Required to run ${GEN_VERSION_SCRIPT}")
    endif()

    # --- Create trigger to force regeneration when inputs change ---
    add_custom_command(
        OUTPUT "${VERSION_TRIGGER}"
        COMMAND ${CMAKE_COMMAND} -E touch "${VERSION_TRIGGER}"
        COMMENT "⏱ Updating version trigger"
    )
    # # --- Generate version header using Bash explicitly ---
    # get_filename_component(SDOM_ROOT "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
    # set(GEN_VERSION_SCRIPT "${SDOM_ROOT}/scripts/gen_version.sh")
    # add_custom_command(
    #     OUTPUT "${VERSION_HEADER}"
    #     # COMMAND ${BASH_EXECUTABLE} -c "mkdir -p ${CMAKE_BINARY_DIR} && echo Running ${GEN_VERSION_SCRIPT} && ${GEN_VERSION_SCRIPT} > ${CMAKE_BINARY_DIR}/_version_log.txt 2>&1 || echo FAILED: $? "
    #     COMMAND ${BASH_EXECUTABLE} "${GEN_VERSION_SCRIPT}" 2>&1 | tee "${CMAKE_BINARY_DIR}/_version_log.txt"
    #     COMMAND ${CMAKE_COMMAND} -E touch "${VERSION_HEADER}"
    #     WORKING_DIRECTORY "${SDOM_ROOT}"
    #     DEPENDS "${GEN_VERSION_SCRIPT}" "${VERSION_TEMPLATE}" "${VERSION_TRIGGER}"
    #     COMMENT "🔄 Regenerating SDOM_Version.hpp (script/template/source changed)"
    #     VERBATIM
    # )
    # add_custom_target(generate_version DEPENDS "${VERSION_HEADER}")
    # message(STATUS "✅ SDOM version will regenerate when source files change")

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

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wno-comment -Wextra -Wno-reorder -Wno-unused-parameter"
)
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -Wextra -Wno-comment -Wno-reorder -Wno-unused-parameter -DNDEBUG"
)

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

# Propagate Lua bindings option as a compile-time definition for consumers
if(SDOM_ENABLE_LUA_BINDINGS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDOM_ENABLE_LUA_BINDINGS=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDOM_ENABLE_LUA_BINDINGS=0)
endif()

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

# ======================================================
#  Optional: dataregistry_generator to emit C API headers
#  Run when generated version differs from project version
# ======================================================
set(GENERATED_DIR "${CMAKE_BINARY_DIR}/capi_generated")
file(MAKE_DIRECTORY "${GENERATED_DIR}")

# Build a small generator executable from local sources (keeps it separate
# from the main library and avoids circular linking).
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

# The generator may compile parts of the code that reference the Lua C API
# (via sol2). Link the discovered Lua library so the generator can link.
if(LUA_LIB)
    # Link the Lua library and common runtime helpers that Lua may depend on
    # (dl, math, threads). This ensures the generator links cleanly in all
    # build configurations where Lua symbols are referenced.
    find_package(Threads REQUIRED)
    target_link_libraries(dataregistry_generator PRIVATE
        ${LUA_LIB}
        ${CMAKE_DL_LIBS}
        Threads::Threads
    )
endif()

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
