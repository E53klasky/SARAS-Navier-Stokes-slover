/********************************************************************************************************************************************
 * Saras
 *
 * Copyright (C) 2019, Mahendra K. Verma
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ********************************************************************************************************************************************
 */
/*! \file writer.h
 *
 *  \brief Class declaration of writer
 *
 *  \author Roshan Samuel
 *  \date Nov 2019
 *  \copyright New BSD License
 *
 ********************************************************************************************************************************************
 */

#ifndef WRITER_H
#define WRITER_H

#include <blitz/array.h>
#include <sys/stat.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <iostream>

#include "field.h"
#include "grid.h"
#include "hdf5.h"
#include "adios2.h"

class writer {
<<<<<<< HEAD
public:
        writer(const grid& mesh , std::vector<field>& wFields , std::string outDir);
=======
    public:
        writer(const grid &mesh, std::vector<field> &wFields, std::string outDir);
>>>>>>> origin/main

        // hard coding 2d for now
        void writeBP(real time);
        void writeTarang(real time);
        void writeSolution(real time);
        void writeRestart(real time);

        ~writer();

<<<<<<< HEAD
private:
        int timestepCounter;
        std::string outputDir = "output";
        adios2::Engine bpWriter;
        const grid& mesh;
        bool isADIOSInitialized;
        adios2::ADIOS* adios;
        adios2::IO bpIO;
        adios2::Variable<double> bpVx , bpVy , bpVz , bpP;
        std::vector<field>& wFields;
=======
    private:
        std::string outputDir = "output";
        const grid &mesh;
>>>>>>> origin/main

        std::vector<double> vx_vector;
        std::vector<double> vy_vector;
        std::vector<double> vz_vector;
        std::vector<double> p_vector;

#ifdef PLANAR
        blitz::Array<real , 2> fieldData;
#else
        blitz::Array<real , 3> fieldData;
#endif

        std::vector<hid_t> sourceDSpace , targetDSpace;

        std::vector< blitz::TinyVector<int , 3> > localSize;

        void outputCheck();

        void initLimits();

        void copyData(field& outField);
        void interpolateData(field& outField);
};

/**
 ********************************************************************************************************************************************
 *  \class writer writer.h "lib/io/writer.h"
 *  \brief Class for all the global variables and functions related to writing output data of the solver.
 *
 *  The computational data from the solver is written in HDF5 format in a .h5 file.
 *  The class allows for both collocated and staggered grid data to be written in separate output files.
 ********************************************************************************************************************************************
 */

#endif
