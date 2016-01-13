
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

/*  set tags to newTag where mask>0 */

/************************************************************************************/

#define ESNEEDPYTHON
#include "esysUtils/first.h"

#include "NodeFile.h"
#include "Util.h"

/************************************************************************************/

void Dudley_NodeFile_setTags(Dudley_NodeFile * self, const int newTag, const escript::Data* mask)
{
    register dim_t n;
    dim_t numNodes;
    register __const double *mask_array;
    Dudley_resetError();

    if (self == NULL)
	return;
    numNodes = self->numNodes;
    if (1 != getDataPointSize(mask))
    {
	Dudley_setError(TYPE_ERROR, "Dudley_NodeFile_setTags: number of components of mask is 1.");
    }
    else if (!numSamplesEqual(mask, 1, numNodes))
    {
	Dudley_setError(TYPE_ERROR, "Dudley_NodeFile_setTags: illegal number of samples of mask Data object");
    }

    /* now we can start */

    if (Dudley_noError())
    {
#pragma omp parallel private(n,mask_array)
	{
#pragma omp for schedule(static)
	    for (n = 0; n < numNodes; n++)
	    {
		mask_array = getSampleDataRO(mask, n);
		if (mask_array[0] > 0)
		    self->Tag[n] = newTag;
	    }
	}
	Dudley_NodeFile_setTagsInUse(self);
    }
}

/*
* $Log$
*
*/
