cmake_minimum_required(VERSION 3.20)

# Project name and version
project(prog VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Debug by default if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Add debug flags and sanitizers
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wno-comment -Wextra -Wno-reorder -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -Wextra -Wno-comment -Wno-reorder -Wno-unused-parameter -DNDEBUG")

# Find SDL3 package
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

# Find Lua (if installed via package manager, usually as 'lua5.3' or 'lua5.4')
find_library(LUA_LIB lua5.3) # or lua5.4, depending on your system


# Use SDOM headers from local or system install
include_directories($ENV{HOME}/.local/include)      # SDL3 Includes (local)
# include_directories(/usr/local/include)           # SDL3 Includes (system)
include_directories(${PROJECT_SOURCE_DIR}/include)  # SDOM Includes

# NOTE: The following paths use '../../' because this test project is in a subdirectory.
# They point to the sol2 headers in the workspace root.
include_directories(${PROJECT_SOURCE_DIR}/../../sol2/include)       # sol2 Includes
include_directories(${PROJECT_SOURCE_DIR}/../../sol2/include/sol)   # sol2 Includes

include_directories(/usr/include/lua5.3)    # Lua Includes


# Source files
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/main.cpp")
# list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/examples/test/main.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

# # Enable unit tests in SDOM library
# target_compile_definitions(prog PRIVATE SDOM_ENABLE_UNIT_TESTS)

# Link to local (user) SDOM static library:
target_link_libraries(prog PRIVATE $ENV{HOME}/.local/lib/libSDOM.a SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf ${LUA_LIB})
# Or for system install, comment above and uncomment below:
# target_link_libraries(prog PRIVATE /usr/local/lib/libSDOM.a SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)

# Output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})