/*****************************************************************************
*
* Copyright (c) 2003-2015 by University of Queensland
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

/*   Dudley: Mesh */

/************************************************************************************/

#include "Mesh.h"

/************************************************************************************/

/*   allocates a Mesh with name name for elements of type id using an integration order. If order is negative, */
/*   the most appropriate order is selected independently. */

Dudley_Mesh *Dudley_Mesh_alloc(char *name, dim_t numDim, esysUtils::JMPI& mpi_info)
{
    Dudley_Mesh *out;

    /*  allocate the return value */

    out = new Dudley_Mesh;
    if (Dudley_checkPtr(out))
	return NULL;
    out->Name = NULL;
    out->Nodes = NULL;
    out->Elements = NULL;
    out->FaceElements = NULL;
    out->Points = NULL;
    out->TagMap = NULL;
    out->reference_counter = 0;
    out->MPIInfo = mpi_info;

    if (!Dudley_noError())
    {
	Dudley_Mesh_free(out);
	return NULL;
    }
    /*   copy name: */

    out->Name = new  char[strlen(name) + 1];
    if (Dudley_checkPtr(out->Name))
    {
	Dudley_Mesh_free(out);
	return NULL;
    }
    strcpy(out->Name, name);

    /*   allocate node table: */
    out->Nodes = Dudley_NodeFile_alloc(numDim, mpi_info);
    if (!Dudley_noError())
    {
	Dudley_Mesh_free(out);
	return NULL;
    }
    out->approximationOrder = -1;
    out->reducedApproximationOrder = -1;
    out->integrationOrder = -1;
    out->reducedIntegrationOrder = -1;

    out->Elements = NULL;
    out->FaceElements = NULL;
    out->Points = NULL;
    out->reference_counter++;
    return out;
}

/* returns a reference to Dudley_Mesh in */

Dudley_Mesh *Dudley_Mesh_reference(Dudley_Mesh * in)
{
    if (in != NULL)
	++(in->reference_counter);
    return in;
}

/*   frees a mesh: */

void Dudley_Mesh_free(Dudley_Mesh * in)
{
    if (in != NULL)
    {
	in->reference_counter--;
	if (in->reference_counter < 1)
	{
	    delete[] in->Name;
	    Dudley_NodeFile_free(in->Nodes);
	    Dudley_ElementFile_free(in->FaceElements);
	    Dudley_ElementFile_free(in->Elements);
	    Dudley_ElementFile_free(in->Points);
	    Dudley_TagMap_free(in->TagMap);
	    delete in;
	}
    }
}

/************************************************************************************/

/*  returns the spatial dimension of the mesh: */

dim_t Dudley_Mesh_getDim(Dudley_Mesh * in)
{
    return in->Nodes->numDim;
}

void Dudley_Mesh_setElements(Dudley_Mesh * self, Dudley_ElementFile * elements)
{
    Dudley_ElementFile_free(self->Elements);
    self->Elements = elements;
}

void Dudley_Mesh_setFaceElements(Dudley_Mesh * self, Dudley_ElementFile * elements)
{
    Dudley_ElementFile_free(self->FaceElements);
    self->FaceElements = elements;
}

void Dudley_Mesh_setPoints(Dudley_Mesh * self, Dudley_ElementFile * elements)
{
    Dudley_ElementFile_free(self->Points);
    self->Points = elements;
}

int Dudley_Mesh_getStatus(Dudley_Mesh * in)
{
    if (in == NULL)
    {
	return -1;
    }
    else if (in->Nodes == NULL)
    {
	return -1;
    }
    else
    {
	return in->Nodes->status;
    }
}

void Dudley_Mesh_setOrders(Dudley_Mesh * in)
{
    in->approximationOrder = 1;	/* order of shapeFunctions is always 1 in Dudley */
    in->reducedApproximationOrder = 1;
    in->integrationOrder = 2;
    in->reducedIntegrationOrder = 0;

}
