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

# Include directories
include_directories($ENV{HOME}/.local/include)      # SDL3 Includes (local)
include_directories(${PROJECT_SOURCE_DIR}/include)  # SDOM Includes
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

# Link to local (user) SDOM static library
target_link_libraries(prog
    PRIVATE
        $ENV{HOME}/.local/lib/libSDOM.a
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        ${LUA_LIB}
)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
