# ------------------------------------
# Multiboard Data Processor CMake List
# ------------------------------------

# Check CMake Version
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Set Project
project(DataProcessor)

# Set Version
set (DataProcessor_VERSION_MAJOR 1)
set (DataProcessor_VERSION_MINOR 0)

# Find Git Version
execute_process(COMMAND bash "-c" "git describe --abbrev=4 --dirty --always --tags" OUTPUT_VARIABLE GIT_TAG)
# Strip newline from output
string(REGEX REPLACE "\n$" "" GIT_TAG "${GIT_TAG}")
# Add quote marks
set(GIT_TAG "\"${GIT_TAG}\"")
# Print tag to cmake output
message(STATUS "Git Tag Detected: ${GIT_TAG}")

# Add Debug definition
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DLOCAL_DEBUG")

# Specify C++ standard required
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure compiler warnings are added
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Werror")

# Set current directory
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Add sources
include( ${PROJECT_SOURCE_DIR}/DataStructures/cmakelist.cmake)
include( ${PROJECT_SOURCE_DIR}/Abstractions/cmakelist.cmake)
include( ${PROJECT_SOURCE_DIR}/Root/cmakelist.cmake)
include( ${PROJECT_SOURCE_DIR}/Core/cmakelist.cmake)

# Find Root
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO XMLIO)
include(${ROOT_USE_FILE})

# Add Executable
add_executable(DataProcessor ${SOURCE_FILES})

# Add git tag definition to executable
target_compile_definitions(DataProcessor PRIVATE GITTAG=${GIT_TAG})

# Link Root Libraries
target_link_libraries(DataProcessor ${ROOT_LIBRARIES})

# Set Install
install(TARGETS DataProcessor RUNTIME DESTINATION bin)
