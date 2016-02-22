
/*****************************************************************************
*
* Copyright (c) 2003-2016 by The University of Queensland
* http://www.uq.edu.au
*
* Primary Business: Queensland, Australia
* Licensed under the Open Software License version 3.0
* http://www.opensource.org/licenses/osl-3.0.php
*
* Development until 2012 by Earth Systems Science Computational Center (ESSCC)
* Development 2012-2013 by School of Earth Sciences
* Development from 2014 by Centre for Geoscience Computing (GeoComp)
*
*****************************************************************************/

/************************************************************************************/

/*   Dudley: Mesh: NodeFile */

/* copies the array newX into self->coordinates */

/************************************************************************************/

#define ESNEEDPYTHON
#include "esysUtils/first.h"

#include "NodeFile.h"
#include "Util.h"

/************************************************************************************/

void Dudley_NodeFile_setCoordinates(Dudley_NodeFile* self, const escript::Data* newX)
{
    if (getDataPointSize(newX) != self->numDim)
    {
        std::stringstream ss;
        ss << "Dudley_NodeFile_setCoordinates: number of dimensions of new "
            "coordinates has to be " << self->numDim;
        const std::string errorMsg(ss.str());
        Dudley_setError(VALUE_ERROR, errorMsg.c_str());
    } else if (!numSamplesEqual(newX, 1, self->numNodes)) {
        std::stringstream ss;
        ss << "Dudley_NodeFile_setCoordinates: number of given nodes must be "
            << self->numNodes;
        const std::string errorMsg(ss.str());
        Dudley_setError(VALUE_ERROR, errorMsg.c_str());
    } else {
        const size_t numDim_size = self->numDim * sizeof(double);
        Dudley_increaseStatus(self);
#pragma omp parallel for
        for (index_t n = 0; n < self->numNodes; n++) {
            memcpy(&self->Coordinates[INDEX2(0, n, self->numDim)],
                    newX->getSampleDataRO(n), numDim_size);
        }
    }
}

