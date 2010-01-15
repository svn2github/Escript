
/*******************************************************
*
* Copyright (c) 2003-2009 by University of Queensland
* Earth Systems Science Computational Center (ESSCC)
* http://www.uq.edu.au/esscc
*
* Primary Business: Queensland, Australia
* Licensed under the Open Software License version 3.0
* http://www.opensource.org/licenses/osl-3.0.php
*
*******************************************************/

#ifndef __ESCRIPTDATASET_H__
#define __ESCRIPTDATASET_H__

#include <escriptexport/escriptexport.h>
#include <escript/AbstractDomain.h>

class DBfile;

namespace escript {

class Data;

}


namespace escriptexport {

typedef std::vector<escript::Data>  DataVec;
typedef std::vector<DataVar_ptr>    DataBlocks;
typedef std::vector<FinleyMesh_ptr> MeshBlocks;

struct VarInfo {
    std::string varName;
    DataBlocks dataBlocks;
    IntVec sampleDistribution;
    bool valid;
};

typedef std::vector<VarInfo> VarVector;


/// \brief Represents an escript dataset including domain and variables for
/// one timestep.
//
/// This class represents an escript/finley dataset complete with domain and
/// variable data. It can read the dataset from NetCDF files generated by the
/// dump() methods within escript or through an escript domain instance plus
/// a number of escript::Data instances. Subsequently, the dataset can be
/// saved in the Silo file format or accessed through a number of accessor
/// methods.
///
/// If the data is distributed via MPI then all ranks should create an instance
/// of this class and call the respective methods. NetCDF files that stem from
/// a parallel run can be read on one processor or on the same number of MPI
/// processes as they were created with.
class EscriptDataset
{
public:
    /// \brief Default constructor.
    ESCRIPTEXPORT_DLL_API
    EscriptDataset();

#if HAVE_MPI
    /// \brief Constructor with communicator.
    ESCRIPTEXPORT_DLL_API
    EscriptDataset(MPI_Comm comm);
#endif

    /// \brief Destructor.
    ESCRIPTEXPORT_DLL_API
    ~EscriptDataset();

    /// \brief Initialises with instances of escript domain and data.
    ESCRIPTEXPORT_DLL_API
    bool initFromEscript(escript::const_Domain_ptr mesh,
                         DataVec& escriptVars, const StringVec& varNames);
                         
    /// \brief Loads mesh and variables from escript NetCDF files.
    ///
    /// \param meshFilePattern a printf-style pattern for the mesh file names
    ///                        (e.g. "mesh.%02d.nc")
    /// \param varFiles a vector of file name patterns for variables
    /// \param varNames a vector of variable names
    /// \param nBlocks number of blocks/chunks to load
    ///
    /// \note If MPI is enabled nBlocks must be equal to the size of the
    ///       communicator or this method fails.
    ESCRIPTEXPORT_DLL_API
    bool loadNetCDF(const std::string meshFilePattern,
                    const StringVec& varFiles, const StringVec& varNames,
                    int nBlocks);

    /// \brief Loads only variables from escript NetCDF files using given mesh.
    ESCRIPTEXPORT_DLL_API
    bool loadNetCDF(const MeshBlocks& mesh, const StringVec& varFiles,
                    const StringVec& varNames);

    /// \brief Sets the cycle number and time value for this dataset.
    ESCRIPTEXPORT_DLL_API
    void setCycleAndTime(int c, double t) { cycle=c; time=t; }

    /// \brief Saves the dataset in the Silo file format.
    ESCRIPTEXPORT_DLL_API
    bool saveSilo(const std::string fileName, bool useMultiMesh=true);

    /// \brief Saves the dataset in the VTK XML file format.
    /// \note NOT IMPLEMENTED.
    ESCRIPTEXPORT_DLL_API
    bool saveVTK(const std::string fileName);

    /// \brief Returns the dataset's mesh.
    ///
    /// \note The caller is responsible for freeing the memory of the returned
    /// mesh.
    ESCRIPTEXPORT_DLL_API
    MeshBlocks extractMesh() { keepMesh = true; return meshBlocks; }

    /// \brief Returns the dataset's mesh.
    ESCRIPTEXPORT_DLL_API
    const MeshBlocks& getMesh() const { return meshBlocks; }

    /// \brief Returns a vector with the dataset's variables.
    ESCRIPTEXPORT_DLL_API
    const VarVector& getVariables() const { return variables; }

    /// \brief Returns a vector with the mesh variables.
    ESCRIPTEXPORT_DLL_API
    const VarVector& getMeshVariables() const { return meshVariables; }

private:
    bool loadMeshFromNetCDF();
    bool loadVarFromNetCDF(const std::string& fileName,
                           const std::string& varName);
    void convertMeshVariables();
    void updateSampleDistribution(VarInfo& vi);
    void putSiloMultiMesh(DBfile* dbfile, const std::string& meshName);
    void putSiloMultiTensor(DBfile* dbfile, const VarInfo& vi);
    void putSiloMultiVar(DBfile* dbfile, const VarInfo& vi,
                         bool useMeshFile = false);

    int numParts;
    int cycle;
    double time;
    bool keepMesh, externalMesh;
    std::string meshFmt;
    MeshBlocks meshBlocks;
    VarVector variables, meshVariables;
    int mpiRank, mpiSize;
#if HAVE_MPI
    MPI_Comm mpiComm;
#else
    void* mpiComm;
#endif
};

} // namespace escriptexport

#endif // __ESCRIPTDATASET_H__

