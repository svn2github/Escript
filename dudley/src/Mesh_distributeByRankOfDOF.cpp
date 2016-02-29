
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

/****************************************************************************/

/* Dudley: Mesh: this will redistribute the Nodes and Elements including overlap */
/* according to the dof_distribution. It will create an element coloring but will not create any mappings. */

/****************************************************************************/

#define ESNEEDPYTHON
#include "esysUtils/first.h"

#include "Mesh.h"

namespace dudley {

void Dudley_Mesh_distributeByRankOfDOF(Dudley_Mesh* self, index_t* dof_distribution)
{
    if (self == NULL)
        return;

    index_t min_dof_id, max_dof_id, *tmp_node_localDOF_map = NULL, *tmp_node_localDOF_mask = NULL;
    index_t k;
    dim_t len, n, numDOFs;

    int* mpiRankOfDOF = new int[self->Nodes->numNodes];

    Dudley_NodeFile_assignMPIRankToDOFs(self->Nodes, mpiRankOfDOF, dof_distribution);

    /* first the elements are redistributed according to mpiRankOfDOF */
    /* at the input the Node tables refering to a the local labeling of the nodes */
    /* while at the output they refer to the global labeling which is rectified in the next step */
    Dudley_ElementFile_distributeByRankOfDOF(self->Elements, mpiRankOfDOF, self->Nodes->Id);
    Dudley_ElementFile_distributeByRankOfDOF(self->FaceElements, mpiRankOfDOF, self->Nodes->Id);
    Dudley_ElementFile_distributeByRankOfDOF(self->Points, mpiRankOfDOF, self->Nodes->Id);

    Dudley_Mesh_resolveNodeIds(self);

    /* create a local labeling of the DOFs */
    Dudley_NodeFile_setDOFRange(&min_dof_id, &max_dof_id, self->Nodes);
    len = max_dof_id - min_dof_id + 1;
    tmp_node_localDOF_mask = new  index_t[len];     /* local mask for used nodes */
    tmp_node_localDOF_map = new  index_t[self->Nodes->numNodes];

#pragma omp parallel for private(n) schedule(static)
    for (n = 0; n < len; n++)
        tmp_node_localDOF_mask[n] = -1;

#pragma omp parallel for private (n) schedule(static)
    for (n = 0; n < self->Nodes->numNodes; n++)
        tmp_node_localDOF_map[n] = -1;

#pragma omp parallel for private(n) schedule(static)
    for (n = 0; n < self->Nodes->numNodes; n++)
    {
#ifdef BOUNDS_CHECK
        if ((self->Nodes->globalDegreesOfFreedom[n] - min_dof_id) >= len
            || (self->Nodes->globalDegreesOfFreedom[n] - min_dof_id) < 0)
        {
            printf("BOUNDS_CHECK %s %d\n", __FILE__, __LINE__);
            exit(1);
        }
#endif
        tmp_node_localDOF_mask[self->Nodes->globalDegreesOfFreedom[n] - min_dof_id] = n;
    }

    numDOFs = 0;
    for (n = 0; n < len; n++)
    {
        k = tmp_node_localDOF_mask[n];
        if (k >= 0)
        {
            tmp_node_localDOF_mask[n] = numDOFs;
            numDOFs++;
        }
    }
#pragma omp parallel for private (n,k)
    for (n = 0; n < self->Nodes->numNodes; n++)
    {
        k = tmp_node_localDOF_mask[self->Nodes->globalDegreesOfFreedom[n] - min_dof_id];
        tmp_node_localDOF_map[n] = k;
    }
    /* create element coloring */
    Dudley_Mesh_createColoring(self, tmp_node_localDOF_map);

    delete[] tmp_node_localDOF_mask;
    delete[] tmp_node_localDOF_map;
    delete[] mpiRankOfDOF;
}

} // namespace dudley

