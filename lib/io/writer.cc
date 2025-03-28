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
/*! \file writer.cc
 *
 *  \brief Definitions for functions of class writer
 *  \sa writer.h
 *  \author Roshan Samuel
 *  \date Nov 2019
 *  \copyright New BSD License
 *
 ********************************************************************************************************************************************
 */

#include "writer.h"

/**
 ********************************************************************************************************************************************
 * \brief   Constructor of the writer class
 *
 *          The constructor initializes the variables and parameters for parallel file writing through HDF5
 *
 * \param   mesh is a const reference to the global data contained in the grid class
 * \param   wField is a vector of sfields to be written
 *
 ********************************************************************************************************************************************
 */
// I need mesh, wFields, outDir, 
writer::writer(const grid& mesh , std::vector<field>& wFields , std::string outDir)
    : mesh(mesh) , wFields(wFields) , outputDir(outDir) , timestepCounter(0) , isADIOSInitialized(false)
{
    initLimits();
    outputCheck();

    adios = new adios2::ADIOS(MPI_COMM_WORLD);
    bpIO = adios->DeclareIO("WriteBP");

    // Define variables with proper offsets based on staggering
    for (unsigned int i = 0; i < wFields.size(); i++) {
        // Use cell-centered dimensions for all variables since we interpolate before writing
        std::vector<std::size_t> shape = {
            static_cast<std::size_t>(mesh.globalSize(0)),
#ifndef PLANAR
            static_cast<std::size_t>(mesh.globalSize(1)),
#endif
            static_cast<std::size_t>(mesh.globalSize(2))
        };

        std::vector<std::size_t> start = {
            static_cast<std::size_t>(mesh.subarrayStarts[0]),
#ifndef PLANAR
            static_cast<std::size_t>(mesh.subarrayStarts[1]),
#endif
            static_cast<std::size_t>(mesh.subarrayStarts[2])
        };

        // Use the same dimensions as HDF5 output
        std::vector<std::size_t> count = {
            static_cast<std::size_t>(localSize[i](0)),
#ifndef PLANAR
            static_cast<std::size_t>(localSize[i](1)),
#endif
            static_cast<std::size_t>(localSize[i](2))
        };

        // need to fix to make sure it is compatiable with 2d and 3d when set to planar it is 2d and 3d when not  
        // test ------------------------------------
        if (wFields[i].fieldName == "Vx") {
            if (mesh.rankData.xRank == mesh.rankData.npX - 1) {
                count[0] += 1;
            }
            // std::cout << mesh.rankData.rank << ": Vx Count: " << count[0] << " " << count[1] << std::endl;
            bpVx = bpIO.DefineVariable<double>("Vx" , shape , start , count);
        }
        else if (wFields[i].fieldName == "Vy") {
            if (mesh.rankData.yRank == mesh.rankData.npY - 1) {
                count[1] += 1;
            }
            // std::cout << mesh.rankData.rank << ": Vy Count: " << count[0] << " " << count[1] << std::endl;
            bpVy = bpIO.DefineVariable<double>("Vy" , shape , start , count);
        }
        else if (wFields[i].fieldName == "Vz") {
            count[1] += 1;
            // std::cout << mesh.rankData.rank << ": Vz Count: " << count[0] << " " << count[1] << std::endl;
            bpVz = bpIO.DefineVariable<double>("Vz" , shape , start , count);
        }
        else if (wFields[i].fieldName == "P") {
            if (mesh.rankData.xRank == mesh.rankData.npX - 1) {
                count[0] += 1;
            }
            bpP = bpIO.DefineVariable<double>("P" , shape , start , count);
        }
    }
}
// Add new method for writing BP files
void writer::writeBP(real time) {
    // used to do this once 
    if (!isADIOSInitialized) {
        std::string filename = this->outputDir + "/output.bp";
        bpWriter = bpIO.Open(filename , adios2::Mode::Write);
        isADIOSInitialized = true;
    }

    bpWriter.BeginStep();

    int pIndex = wFields.size() - 1;

    for (unsigned int i = 0; i < wFields.size(); i++) {
#ifdef PLANAR
        fieldData.resize(blitz::TinyVector<int , 2>(localSize[pIndex](0) , localSize[pIndex](2)));
#else
        fieldData.resize(localSize[pIndex]);
#endif
        // Interpolate data to cell centers
        interpolateData(wFields[i]);

        // // Debug print
        // if (mesh.rankData.rank == 0) {
        //     double* data = fieldData.dataFirst();
        //     std::cout << "Field " << wFields[i].fieldName << " last 5 values: ";
        //     for (int j = 0; j < 5; j++) {
        //         std::cout << data[fieldData.size() - j - 1] << " ";
        //     }
        //     std::cout << std::endl;
        // }

        if (wFields[i].fieldName == "Vx") {
            bpWriter.Put(bpVx , fieldData.dataFirst());
        }
        else if (wFields[i].fieldName == "Vy") {
            bpWriter.Put(bpVy , fieldData.dataFirst());
        }
        else if (wFields[i].fieldName == "Vz") {
            bpWriter.Put(bpVz , fieldData.dataFirst());
        }
        else if (wFields[i].fieldName == "P") {
            bpWriter.Put(bpP , fieldData.dataFirst());
        }
        bpWriter.PerformPuts();
    }

    bpWriter.EndStep();
}


    /**
     ********************************************************************************************************************************************
     * \brief   Function to initialize the global and local limits for setting file views
     *
     *          All the necessary limits of the local arrays with respect to the global array for setting the
     *          dataspace views for HDF5 are appropriately set here.
     ********************************************************************************************************************************************
     */
void writer::initLimits() {
    // hid_t sDSpace;
    // hid_t tDSpace;

    // herr_t status;

    blitz::TinyVector<int , 3> gloSize , sdStart , locSize;

    // this needs to be changed all just var type ----------------------------------------------------------------------
#ifdef PLANAR         /*  change from hsize_t -> size_t  */
    size_t dimsf[2];           /* dataset dimensions */
    size_t offset[2];          /* offset of hyperslab */
#else
    size_t dimsf[3];           /* dataset dimensions */
    size_t offset[3];          /* offset of hyperslab */
#endif
//  --------------------------------------------------------------------------
    for (unsigned int i = 0; i < wFields.size(); i++) {
        gloSize = mesh.globalSize;
        if (not wFields[i].xStag) {
            gloSize(0) -= 1;
        }

#ifndef PLANAR
        if (not wFields[i].yStag) {
            gloSize(1) -= 1;
        }
#else
        gloSize(1) = 1;
#endif

        if (not wFields[i].zStag) {
            gloSize(2) -= 1;
        }

        locSize = mesh.collocCoreSize;
        if (wFields[i].xStag) {
            // All subdomains exclude the last point (which is shared across 2 processors), except those at the last rank along X direction, thereby capturing the full domain
            locSize(0) = mesh.staggrCoreSize(0) - 1;
            if (mesh.rankData.xRank == mesh.rankData.npX - 1) {
                locSize(0) += 1;
            }
        }

#ifndef PLANAR
        if (wFields[i].yStag) {
            // As with X direction, all subdomains exclude the last point (which is shared across 2 processors), except those at the last rank along Y direction
            locSize(1) = mesh.staggrCoreSize(1) - 1;
            if (mesh.rankData.yRank == mesh.rankData.npY - 1) {
                locSize(1) += 1;
            }
        }
#else
        locSize(1) = 1;
#endif

        if (wFields[i].zStag) {
            locSize(2) = mesh.staggrCoreSize(2);
        }

        // Since only the last rank along X and Y directions include the extra point (shared across processors), subArrayStarts are same for all ranks
        sdStart = mesh.subarrayStarts;

        // Create a dataspace representing the full limits of the array - this is the source dataspace
#ifdef PLANAR
        dimsf[0] = locSize(0);
        dimsf[1] = locSize(2);
    //    sDSpace = H5Screate_simple(2 , dimsf , NULL);
#else
        dimsf[0] = locSize(0);
        dimsf[1] = locSize(1);
        dimsf[2] = locSize(2);
     //   sDSpace = H5Screate_simple(3 , dimsf , NULL);
#endif

        // Modify the view of the *source* dataspace by using a hyperslab - *this view will be used to read from memory*

        // ************* NOTE: Check if the hyperslab view must be changed to take care of the common point at the the MPI decomposed sub-domain boundaries ****************//
#ifdef PLANAR
        dimsf[0] = locSize(0);
        dimsf[1] = locSize(2);
        offset[0] = 0;
        offset[1] = 0;
#else
        dimsf[0] = locSize(0);
        dimsf[1] = locSize(1);
        dimsf[2] = locSize(2);
        offset[0] = 0;
        offset[1] = 0;
        offset[2] = 0;
#endif

        // Create a dataspace representing the full limits of the global array - i.e. the dataspace for output file
#ifdef PLANAR
        dimsf[0] = gloSize(0);
        dimsf[1] = gloSize(2);
     //   tDSpace = H5Screate_simple(2 , dimsf , NULL);
#else
        dimsf[0] = gloSize(0);
        dimsf[1] = gloSize(1);
        dimsf[2] = gloSize(2);
     //   tDSpace = H5Screate_simple(3 , dimsf , NULL);
#endif

        // Modify the view of the *target* dataspace by using a hyperslab according to its position in the global file dataspace
#ifdef PLANAR
        // 2d  local sizes and offests 
        dimsf[0] = locSize(0);
        dimsf[1] = locSize(2);
        offset[0] = sdStart[0];
        offset[1] = sdStart[2];
#else
        // 3d local sizes and offests 
        dimsf[0] = locSize(0);
        dimsf[1] = locSize(1);
        dimsf[2] = locSize(2);
        offset[0] = sdStart[0];
        offset[1] = sdStart[1];
        offset[2] = sdStart[2];
#endif
        // std::cout << "Local size: " << locSize << std::endl;    
        localSize.push_back(locSize);
      //  sourceDSpace.push_back(sDSpace);
       // targetDSpace.push_back(tDSpace);
    }
}


/**
 ********************************************************************************************************************************************
 * \brief   Function to create output folder if it does not exist
 *
 *          The function uses methods available in the sys/stat.h header to create an output folder if it does not exist
 *          Without this, the code used to give segmentation fault while attempting to write solution files
 ********************************************************************************************************************************************
 */
void writer::outputCheck() {
    struct stat info;
    int createStatus;

    if (mesh.rankData.rank == 0) {
        // Check if output directory exists
        if (stat("output" , &info) != 0) {
            createStatus = mkdir("output" , S_IRWXU | S_IRWXG);

            // Raise error if the filesystem is read-only or something
            if (createStatus) {
                std::cout << "Error in while attempting to create output directory. Aborting" << std::endl;
                exit(0);
            }
        }
    }
}

// do not do this ---------------------------------------------------------
void writer::writeTarang(real time) {
}

// wrties it out 
void writer::writeSolution(real time) {
    // hid_t plist_id;
    // hid_t fileHandle;
    // hid_t dataSet;

    // herr_t status;


    // The last entry in wFields is P, which is cell centered. Hence the MPI-IO data-structures associated with this index is used for file writing

    // Create a property list for collectively opening a file by all processors
    // plist_id = H5Pcreate(H5P_FILE_ACCESS);
    // H5Pset_fapl_mpio(plist_id , MPI_COMM_WORLD , MPI_INFO_NULL);



 // Generate the filename corresponding to the solution file
//     fileName = new char[100];
//     constFile.str(std::string());

//     constFile << this->outputDir << "/Soln_" << std::fixed << std::setfill('0') << std::setw(9) << std::setprecision(4) << time << ".h5";
//     strcpy(fileName , constFile.str().c_str());

//     constFile << this->outputDir << "/Soln_" << std::fixed << std::setfill('0') << std::setw(9) << std::setprecision(4) << time << ".h5";
//     strcpy(fileName , constFile.str().c_str());


//         // First create a file handle with the path to the output file
//     fileHandle = H5Fcreate(fileName , H5F_ACC_TRUNC , H5P_DEFAULT , plist_id);

//     // Close the property list for later reuse
//     H5Pclose(plist_id);

//     // Create a property list to use collective data write
//     plist_id = H5Pcreate(H5P_DATASET_XFER);
//     H5Pset_dxpl_mpio(plist_id , H5FD_MPIO_COLLECTIVE);

//     for (unsigned int i = 0; i < wFields.size(); i++) {
// #ifdef PLANAR
//         fieldData.resize(blitz::TinyVector<int , 2>(localSize[pIndex](0) , localSize[pIndex](2)));
// #else
//         fieldData.resize(localSize[pIndex]);
// #endif

//         std::cout << "FieldData size: " << fieldData.size() << std::endl;

//         //Write data after first interpolating them to cell centers
//         interpolateData(wFields[i]);

//         // Create the dataset *for the file*, linking it to the file handle.
//         // Correspondingly, it will use the *core* dataspace, as only the core has to be written excluding the pads
//         dataSet = H5Dcreate2(fileHandle , wFields[i].fieldName.c_str() , H5T_NATIVE_REAL , targetDSpace[pIndex] , H5P_DEFAULT , H5P_DEFAULT , H5P_DEFAULT);

        // Write the dataset. Most important thing to note is that the 3rd and 4th arguments represent the *source* and *destination* dataspaces.
        // The source here is the sourceDSpace pointing to the memory buffer. Note that its view has been adjusted using hyperslab.
        // The destination is the targetDSpace. Though the targetDSpace is smaller than the sourceDSpace,
        // only the appropriate hyperslab within the sourceDSpace is transferred to the destination.
  // -----------------------------HERE----------------11111111111111111111---------------------------
  //      status = H5Dwrite(dataSet , H5T_NATIVE_REAL , sourceDSpace[pIndex] , targetDSpace[pIndex] , plist_id , fieldData.dataFirst());
        // if (status) {
        //     if (mesh.rankData.rank == 0) {
        //         std::cout << "Error in writing output to HDF file. Aborting" << std::endl;
        //     }
        //     MPI_Finalize();
        // //     exit(0);
        // }

        // // Close dataset for reuse
        // H5Dclose(dataSet);


    // CLOSE/RELEASE RESOURCES
    // H5Pclose(plist_id);
    // H5Fclose(fileHandle);

    // delete fileName;

    // if (mesh.rankData.rank == 0)
    //     std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!THE TIME STEP IS: " << timestepCounter << std::endl;


  //  if (timestepCounter == 1)
    writeBP(time);
    timestepCounter++;
    // if (mesh.rankData.rank == 0)
    //     std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!THE NEXT TIME STEP IS: " << timestepCounter << std::endl;

}

// DO not do this ---------------------------------------------------------
void writer::writeRestart(real time) {
}

/**
 ********************************************************************************************************************************************
 * \brief   Function to copy data to a blitz array without pads
 *
 *          In order to simplify the file views while writing to disk from memory for the restart file,
 *          the variables are copied into a local blitz array without the pads.
 *
 ********************************************************************************************************************************************
 */
void writer::copyData(field& outField) {
#ifdef PLANAR
    for (int i = 0; i < fieldData.shape()[0]; i++) {
        for (int k = 0; k < fieldData.shape()[1]; k++) {
            fieldData(i , k) = outField.F(i , 0 , k);
        }
    }
#else
    for (int i = 0; i < fieldData.shape()[0]; i++) {
        for (int j = 0; j < fieldData.shape()[1]; j++) {
            for (int k = 0; k < fieldData.shape()[2]; k++) {
                fieldData(i , j , k) = outField.F(i , j , k);
            }
        }
    }
#endif
}

/**
 ********************************************************************************************************************************************
 * \brief   Function to interpolate data to a blitz array without pads
 *
 *          The solution files are written after interpolating the data to cell centers.
 *          This function performs this task depending on the mesh configuration of the variable.
 *
 ********************************************************************************************************************************************
 */
void writer::interpolateData(field& outField) {
    // The below method does not recognize arrays with staggering in more than 1 direction
    // Use with care for exotic arrays

#ifdef PLANAR
    if (mesh.rankData.rank == 0) {
        // std::cout << "Interpolating " << outField.fieldName
        //     << " xStag=" << outField.xStag
        //     << " zStag=" << outField.zStag << std::endl;
    }

    if (not outField.xStag) {
        for (int i = 0; i < fieldData.shape()[0]; i++) {
            for (int k = 0; k < fieldData.shape()[2]; k++) {
                fieldData(i , k) = (outField.F(i - 1 , 0 , k) + outField.F(i , 0 , k)) * 0.5;
            }
        }
    }
    else if (not outField.zStag) {
        for (int i = 0; i < fieldData.shape()[0]; i++) {
            for (int k = 0; k < fieldData.shape()[2]; k++) {
                fieldData(i , k) = (outField.F(i , 0 , k - 1) + outField.F(i , 0 , k)) * 0.5;
            }
        }
    }
    else {
        for (int i = 0; i < fieldData.shape()[0]; i++) {
            for (int k = 0; k < fieldData.shape()[2]; k++) {
                fieldData(i , k) = outField.F(i , 0 , k);
            }
        }
    }
#else
    if (not outField.xStag) {
        for (int i = 0; i < fieldData.shape()[0]; i++) {
            for (int j = 0; j < fieldData.shape()[1]; j++) {
                for (int k = 0; k < fieldData.shape()[2]; k++) {
                    fieldData(i , j , k) = (outField.F(i - 1 , j , k) + outField.F(i , j , k)) * 0.5;
                }
            }
        }
    }
    else if (not outField.yStag) {
        for (int i = 0; i < fieldData.shape()[0]; i++) {
            for (int j = 0; j < fieldData.shape()[1]; j++) {
                for (int k = 0; k < fieldData.shape()[2]; k++) {
                    fieldData(i , j , k) = (outField.F(i , j - 1 , k) + outField.F(i , j , k)) * 0.5;
                }
            }
        }
    }
    else if (not outField.zStag) {
        for (int i = 0; i < fieldData.shape()[0]; i++) {
            for (int j = 0; j < fieldData.shape()[1]; j++) {
                for (int k = 0; k < fieldData.shape()[2]; k++) {
                    fieldData(i , j , k) = (outField.F(i , j , k - 1) + outField.F(i , j , k)) * 0.5;
                }
            }
        }
    }
    else {
        for (int i = 0; i < fieldData.shape()[0]; i++) {
            for (int j = 0; j < fieldData.shape()[1]; j++) {
                for (int k = 0; k < fieldData.shape()[2]; k++) {
                    fieldData(i , j , k) = outField.F(i , j , k);
                }
            }
        }
    }
#endif
}

writer::~writer() {
    if (isADIOSInitialized) {
        bpWriter.Close();
    }
    delete adios;
}
