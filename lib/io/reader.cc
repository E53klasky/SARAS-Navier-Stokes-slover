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
/*! \file reader.cc
 *
 *  \brief Definitions for functions of class reader
 *  \sa reader.h
 *  \author Roshan Samuel
 *  \date Nov 2019
 *  \copyright New BSD License
 *
 ********************************************************************************************************************************************
 */
#include "reader.h"

/**
 ********************************************************************************************************************************************
 * \brief   Constructor of the reader class
 *
 *          The constructor initializes the variables and parameters for parallel file reading through ADIOS2
 *
 * \param   mesh is a const reference to the global data contained in the grid class
 * \param   rField is a vector of fields to be read into
 ********************************************************************************************************************************************
 */
reader::reader(const grid& mesh , std::vector<field>& rFields)
    : mesh(mesh) , rFields(rFields) , timestepCounter(0) , isADIOSInitialized(false)
{
    initLimits();
    // adios2::ADIOS adios(MPI_COMM_WORLD);
    adios = new adios2::ADIOS(MPI_COMM_WORLD);
    bpIO = adios->DeclareIO("ReadBP");
}

/**
 ********************************************************************************************************************************************
 * \brief   Function to initialize the global and local limits for setting file views
 *
 *          All the necessary limits of the local arrays with respect to the global array are appropriately set here.
 ********************************************************************************************************************************************
 */
void reader::initLimits() {
    blitz::TinyVector<int , 3> gloSize , sdStart , locSize;

    for (unsigned int i = 0; i < rFields.size(); i++) {
        gloSize = mesh.globalSize;
        if (not rFields[i].xStag) {
            gloSize(0) -= 1;
        }

#ifndef PLANAR
        if (not rFields[i].yStag) {
            gloSize(1) -= 1;
        }
#else
        gloSize(1) = 1;
#endif

        if (not rFields[i].zStag) {
            gloSize(2) -= 1;
        }

        locSize = mesh.collocCoreSize;
        if (rFields[i].xStag) {
            // Though the last point was excluded in some subdomains in the writer class, 
            // while reading, these overlapping points have to be considered
            locSize(0) = mesh.staggrCoreSize(0);
        }

#ifndef PLANAR
        if (rFields[i].yStag) {
            // All subdomains include both the boundary points
            locSize(1) = mesh.staggrCoreSize(1);
        }
#else
        locSize(1) = 1;
#endif

        if (rFields[i].zStag) {
            locSize(2) = mesh.staggrCoreSize(2);
        }

        // Since only the last rank along X and Y directions include the extra point, 
        // subArrayStarts are same for all ranks
        sdStart = mesh.subarrayStarts;

        localSize.push_back(locSize);
    }
}

/**
 ********************************************************************************************************************************************
 * \brief   Function to read files in ADIOS2 BP format in parallel
 *
 *          It opens a file in the output folder and all the processors read in parallel from the file
 ********************************************************************************************************************************************
 */
real reader::readData() {
    real time = 0.0;

    // Open the BP file for reading
    std::string filename = this->outputDir + "/output.bp";
    adios2::Engine bpReader = bpIO.Open(filename , adios2::Mode::Read);

    // Open the first step 
    bpReader.BeginStep();

    for (unsigned int i = 0; i < rFields.size(); i++) {
        // Resize fieldData based on the local field size
#ifdef PLANAR
        fieldData.resize(blitz::TinyVector<int , 2>(localSize[i](0) , localSize[i](2)));
#else
        fieldData.resize(localSize[i]);
#endif

        // Read the variables based on their names
        if (rFields[i].fieldName == "Vx") {
            auto bpVx = bpIO.InquireVariable<double>("Vx");
            bpReader.Get(bpVx , fieldData.dataFirst());
        }
        else if (rFields[i].fieldName == "Vy") {
            auto bpVy = bpIO.InquireVariable<double>("Vy");
            bpReader.Get(bpVy , fieldData.dataFirst());
        }
        else if (rFields[i].fieldName == "Vz") {
            auto bpVz = bpIO.InquireVariable<double>("Vz");
            bpReader.Get(bpVz , fieldData.dataFirst());
        }
        else if (rFields[i].fieldName == "P") {
            auto bpP = bpIO.InquireVariable<double>("P");
            bpReader.Get(bpP , fieldData.dataFirst());
        }

        // Copy data to the field
        copyData(rFields[i]);
    }

    // Perform the Gets 
    bpReader.PerformGets();

    // End the current step
    bpReader.EndStep();

    // Close the reader
    bpReader.Close();

    return time;
}

/**
 ********************************************************************************************************************************************
 * \brief   Function to copy data to a field without pads
 *
 *          Copies the data from the local blitz array to the actual field
 ********************************************************************************************************************************************
 */
void reader::copyData(field& outField) {
#ifdef PLANAR
    for (int i = 0; i < fieldData.shape()[0]; i++) {
        for (int k = 0; k < fieldData.shape()[1]; k++) {
            outField.F(i , 0 , k) = fieldData(i , k);
        }
    }
#else
    for (int i = 0; i < fieldData.shape()[0]; i++) {
        for (int j = 0; j < fieldData.shape()[1]; j++) {
            for (int k = 0; k < fieldData.shape()[2]; k++) {
                outField.F(i , j , k) = fieldData(i , j , k);
            }
        }
    }
#endif
}

/**
 ********************************************************************************************************************************************
 * \brief   Placeholder for restart check method
 ********************************************************************************************************************************************
 */
// void reader::restartCheck(hid_t fHandle) {
//     // This is intentionally left empty as in the original code
// }

/**
 ********************************************************************************************************************************************
 * \brief   Destructor to clean up ADIOS2 resources
 ********************************************************************************************************************************************
 */
reader::~reader() {
    delete adios;
}