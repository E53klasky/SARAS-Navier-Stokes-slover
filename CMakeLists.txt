cmake_minimum_required(VERSION 3.5)
project(saras C CXX)

# Find required packages
find_package(HDF5 REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})
# Prevent in-source builds
# Check if BLITZ_INCLUDE_DIR and BLITZ_LIBRARY are defined
if (NOT DEFINED BLITZ_INCLUDE_DIR)
    set(BLITZ_INCLUDE_DIR "/usr/include/blitz")
endif()

if (NOT DEFINED BLITZ_LIBRARY)
    set(BLITZ_LIBRARY "/usr/lib/x86_64-linux-gnu/libblitz.a")
endif()

# Add this line before the rest of the configuration
find_package(MPI REQUIRED)

# If needed, you can manually specify the paths:
# set(MPI_INCLUDE_DIR "/usr/lib/x86_64-linux-gnu/openmpi/include")
# set(MPI_LIBRARY "/usr/lib/x86_64-linux-gnu/openmpi/lib")


# Include the Blitz++ headers
include_directories(${BLITZ_INCLUDE_DIR})

# Link the Blitz++ library
link_directories(${BLITZ_LIBRARY})

if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    MESSAGE(FATAL_ERROR "\nERROR! ${PROJECT_NAME} does not support in-source builds!\n"
            "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}\n"
            "CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}\n"
            "NEXT STEPS:\n"
            "(1) Delete the CMakeCache.txt file and the CMakeFiles/ directory"
            "    under the source directory for ${PROJECT_NAME}, otherwise you"
            "    will not be able to configure ${PROJECT_NAME} correctly!\n"
            "      * For example, on linux machines do:\n"
            "        $ rm -r CMakeCache.txt CMakeFiles/\n"
            "(2) Create a different directory and configure ${PROJECT_NAME} in that directory.\n"
            "      * For example, on linux machines do:\n"
            "        $ mkdir MY_BUILD\n"
            "        $ cd MY_BUILD\n"
            "        $ cmake [OPTIONS] ..\n")
endif()

# Set directories
set(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(HEADER_DIR ${PARENT_DIR}/lib)

# Output directory information
message(STATUS "The parent directory is ${PARENT_DIR}")
message(STATUS "Install folder is ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "All headers are in ${HEADER_DIR}")
message(STATUS "Using Blitz library: ${BLITZ_LIBRARY}")

# Include directories
include_directories(
    ${HEADER_DIR}/parallel
    ${HEADER_DIR}/boundary
    ${HEADER_DIR}/initial
    ${HEADER_DIR}/poisson
    ${HEADER_DIR}/force
    ${HEADER_DIR}/field
    ${HEADER_DIR}/grid
    ${HEADER_DIR}/io
)

# Find and configure YAML-cpp
find_package(yaml-cpp)
if(yaml-cpp_FOUND)
    if(yaml-cpp_VERSION GREATER 0.3)
        message(STATUS "Found YAML Cpp ${yaml-cpp_VERSION}")
    else()
        message(STATUS "Found YAML Cpp ${yaml-cpp_VERSION}. Using deprecated YAML parsing commands.")
        add_definitions(-DYAML_LEGACY)
    endif()
else()
    find_library(YAML_FOUND yaml-cpp libyaml-cpp)
    if(YAML_FOUND)
        message(STATUS "Found YAML Cpp but could not determine version. You may have to configure with DYAML_LEGACY")
    else()
        message(WARNING "Could not determine YAML-Cpp version. You may have to configure with DYAML_LEGACY")
    endif()
endif()

# Configure YAML legacy support if needed
if(YAML_LEGACY)
    message(STATUS "Compiling Saras for older YAML Cpp library")
    add_definitions(-DYAML_LEGACY)
endif()

# Configure dimensional settings
if(PLANAR)
    message(STATUS "Compiling Saras for 2D simulations")
    add_definitions(-DPLANAR)
else()
    message(STATUS "Compiling Saras for 3D simulations")
endif()

# Configure precision settings
if(REAL_SINGLE)
    message(STATUS "Compiling Saras to solve with single precision calculations")
    add_definitions(-DREAL_SINGLE)
else()
    message(STATUS "Compiling Saras to solve with double precision calculations")
    add_definitions(-DREAL_DOUBLE)
endif()

# Configure test settings
if(TEST_RUN)
    message(STATUS "Compiling Saras for running unit tests")
    add_definitions(-DTEST_RUN)
endif()

if(TIME_RUN)
    message(STATUS "Compiling Saras with additional timing calls")
    add_definitions(-DTIME_RUN)
endif()

if(TEST_POISSON)
    message(STATUS "Compiling Saras to test the Poisson library")
    add_definitions(-DTEST_POISSON)
endif()

# Set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Ofast")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -DBZ_DEBUG")

# Add subdirectories
add_subdirectory(lib)
add_subdirectory(src)

# Clear cache variables
unset(PLANAR CACHE)
unset(TEST_RUN CACHE)
unset(TIME_RUN CACHE)
unset(REAL_DOUBLE CACHE)
unset(REAL_SINGLE CACHE)
unset(TEST_POISSON CACHE)
unset(yaml-cpp_FOUND CACHE)
unset(yaml-cpp_VERSION CACHE)
unset(YAML_LEGACY CACHE)