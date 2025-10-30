cmake_minimum_required(VERSION 3.20)
project(SDOM VERSION 1.0 LANGUAGES CXX)

# ======================================================
#  Version Auto-Generation (SDOM_Version.hpp)
# ======================================================
set(GEN_VERSION_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/gen_version.sh")
set(VERSION_TEMPLATE "${CMAKE_SOURCE_DIR}/include/SDOM/SDOM_Version.hpp.in")
set(VERSION_HEADER "${CMAKE_SOURCE_DIR}/include/SDOM/SDOM_Version.hpp")
set(VERSION_TRIGGER "${CMAKE_BINARY_DIR}/_version_trigger")

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
