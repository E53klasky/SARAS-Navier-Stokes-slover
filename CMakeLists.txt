#############################################################################################################################################
 # Saras
 # 
 # Copyright (C) 2019, Mahendra K. Verma
 #
 # All rights reserved.
 # 
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions are met:
 #     1. Redistributions of source code must retain the above copyright
 #        notice, this list of conditions and the following disclaimer.
 #     2. Redistributions in binary form must reproduce the above copyright
 #        notice, this list of conditions and the following disclaimer in the
 #        documentation and/or other materials provided with the distribution.
 #     3. Neither the name of the copyright holder nor the
 #        names of its contributors may be used to endorse or promote products
 #        derived from this software without specific prior written permission.
 # 
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 # ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 # DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 # ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 # (INCLUDING, BUT NOT LIMITED TO, PROCUR-EMENT OF SUBSTITUTE GOODS OR SERVICES;
 # LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 # ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 # (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 # SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 #
 ############################################################################################################################################
 ##
 ##! \file CMakeLists.txt
 #
 #   \brief Root level CMakeLists file.
 #
 #   \author Roshan Samuel
 #   \date Nov 2019
 #   \copyright New BSD License
 #
 ############################################################################################################################################
 ##


# Cmake setup
cmake_minimum_required (VERSION 3.19)
project (saras)

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
endif ()


# Enable C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add ADIOS2 path to CMAKE_PREFIX_PATH
set(CMAKE_PREFIX_PATH "/home/adios/local/adios-install")

# Add this line before the rest of the configuration
find_package(MPI REQUIRED)

# If needed, you can manually specify the paths:
# set(MPI_INCLUDE_DIR "/usr/lib/x86_64-linux-gnu/openmpi/include")
# set(MPI_LIBRARY "/usr/lib/x86_64-linux-gnu/openmpi/lib")


set (PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (HEADER_DIR ${PARENT_DIR}/lib)

# Find required packages
find_package(MPI REQUIRED)
find_package(ADIOS2 REQUIRED)

# Skip MPI C++ bindings
add_definitions(-DOMPI_SKIP_MPICXX -DMPICH_SKIP_MPICXX)

message (STATUS "The parent directory is ${PARENT_DIR}")
message (STATUS "Install folder is ${CMAKE_CURRENT_BINARY_DIR}")
message (STATUS "All headers are in ${HEADER_DIR}")
message (STATUS "ADIOS2 include dirs: ${ADIOS2_INCLUDE_DIRS}")
message (STATUS "MPI include dirs: ${MPI_INCLUDE_PATH}")

# Include MPI directories explicitly
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
include_directories(SYSTEM ${MPI_C_INCLUDE_PATH})
include_directories(SYSTEM ${MPI_CXX_INCLUDE_PATH})

include_directories (${HEADER_DIR}/parallel
                    ${HEADER_DIR}/boundary
                    ${HEADER_DIR}/initial
                    ${HEADER_DIR}/poisson
                    ${HEADER_DIR}/force
                    ${HEADER_DIR}/field
                    ${HEADER_DIR}/grid
                    ${HEADER_DIR}/io
                    ${ADIOS2_INCLUDE_DIRS})

# Search for yaml-cpp library
find_package(yaml-cpp)
if (yaml-cpp_FOUND)
    if (yaml-cpp_VERSION GREATER 0.3)
        message (STATUS "Found YAML Cpp ${yaml-cpp_VERSION}")
    else ()
        message (STATUS "Found YAML Cpp ${yaml-cpp_VERSION}. Using deprecated YAML parsing commands.")
        add_definitions(-DYAML_LEGACY)
    endif ()
else ()
    find_library(YAML_FOUND yaml-cpp libyaml-cpp)
    if (YAML_FOUND)
        message (STATUS "Found YAML Cpp but could not determine version. You may have to configure with DYAML_LEGACY")
    else ()
        message (WARNING "Could not determine YAML-Cpp version. You may have to configure with DYAML_LEGACY")
    endif ()
endif ()

# Compiler flags with MPI flags included
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -Wall -Ofast -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_CXX_LINK_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -DBZ_DEBUG")

# Rest of your configuration flags
if (PLANAR)
    message (STATUS "Compiling Saras for 2D simulations")
    add_definitions(-DPLANAR)
else ()
    message (STATUS "Compiling Saras for 3D simulations")
endif ()

if (REAL_SINGLE)
    message (STATUS "Compiling Saras to solve with single precision calculations")
    add_definitions(-DREAL_SINGLE)
else ()
    message (STATUS "Compiling Saras to solve with double precision calculations")
    add_definitions(-DREAL_DOUBLE)
endif ()

if (TEST_RUN)
    message (STATUS "Compiling Saras for running unit tests")
    add_definitions(-DTEST_RUN)
endif ()

if (TIME_RUN)
    message (STATUS "Compiling Saras with additional timing calls")
    add_definitions(-DTIME_RUN)
endif ()

if (TEST_POISSON)
    message (STATUS "Compiling Saras to test the Poisson library")
    add_definitions(-DTEST_POISSON)
endif ()

add_subdirectory (lib)
add_subdirectory (src)

# Cache cleanup
unset (PLANAR CACHE)
unset (TEST_RUN CACHE)
unset (TIME_RUN CACHE)
unset (REAL_DOUBLE CACHE)
unset (REAL_SINGLE CACHE)
unset (TEST_POISSON CACHE)
unset (yaml-cpp_FOUND CACHE)
unset (yaml-cpp_VERSION CACHE)
unset (YAML_LEGACY CACHE)