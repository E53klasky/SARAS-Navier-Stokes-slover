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
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#############################################################################################################################################
##
##! \file CMakeLists.txt
#
#   \brief CMakeLists file to add all the libraries related to I/O.
#
#   \author Roshan Samuel
#   \date Nov 2019
#   \copyright New BSD License
#
#############################################################################################################################################

# Find ADIOS2 package (make sure CMake finds the right ADIOS2 installation)
set(CMAKE_PREFIX_PATH "/home/adios/local/adios-install")
find_package(ADIOS2 REQUIRED)

# Include ADIOS2 directories for header files
include_directories(${ADIOS2_INCLUDE_DIRS})

# Link directories for ADIOS2 libraries
link_directories(${ADIOS2_LIBRARY_DIRS})

# Find MPI if it's required by ADIOS2
find_package(MPI REQUIRED)

# Add libraries
add_library(parser parser.cc)
add_library(probes probes.cc)
add_library(reader reader.cc)
add_library(writer writer.cc)  # Writer library where ADIOS is used
add_library(tseries tseries.cc)

# Link the ADIOS2 library to writer and other necessary targets
target_link_libraries(writer adios2::adios2 MPI::MPI_CXX)

# If additional targets need ADIOS2, link them here
# target_link_libraries(other_target adios2::adios2)
