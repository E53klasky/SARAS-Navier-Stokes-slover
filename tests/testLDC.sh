#!/bin/bash

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
 ############################################################################################################################################
 ##
 ##! \file testLDC.sh
 #
 #   \brief Shell script to automatically compile and run tests on SARAS
 #
 #   \author Roshan Samuel
 #   \date Jan 2020
 #   \copyright New BSD License
 #
 ############################################################################################################################################
 ##

# Test for 2D LDC and comparison with Ghia et al's (1982, J. Comput. Phys., 48, 387 - 411) result
PROC=4

# REMOVE PRE-EXISTING EXECUTATBLES
rm -f ldcTest/saras

# If build directory doesn't exist, create it
if [ ! -d build ]; then
    mkdir build
fi

# Switch to build directory
cd build

# Run cmake with necessary flags for 2D LDC test
CC=mpicc CXX=mpicxx cmake ../../ -DPLANAR=ON

# Compile
make -j8

# Move the executable to the directory where the test will be performed
mv ../../saras ../../tests/ldcTest/

# Switch to ldcTest directory
cd ../../tests/ldcTest/

# Run the test case
mpirun -np $PROC ./saras input/parameters.yaml output/

# Run the python script to read the output file and compare with Ghia results
python checkLDC.py
