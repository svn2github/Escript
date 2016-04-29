
/*****************************************************************************
*
* Copyright (c) 2003-2016 by The University of Queensland
* http://www.uq.edu.au
*
* Primary Business: Queensland, Australia
* Licensed under the Apache License, version 2.0
* http://www.apache.org/licenses/LICENSE-2.0
*
* Development until 2012 by Earth Systems Science Computational Center (ESSCC)
* Development 2012-2013 by School of Earth Sciences
* Development from 2014 by Centre for Geoscience Computing (GeoComp)
*
*****************************************************************************/


/****************************************************************************

  Finley: Mesh

  removes matching face elements from the mesh

*****************************************************************************/

#include "Mesh.h"

#include <escript/index.h>

namespace finley {

void Mesh::glueFaces(double safety_factor, double tolerance, bool optimize)
{
    if (MPIInfo->size > 1) {
        throw escript::NotImplementedError("Mesh::glueFaces: MPI is not supported yet.");
    }
    if (!FaceElements)
        return;

    const_ReferenceElement_ptr faceRefElement(FaceElements->
                        referenceElementSet->borrowReferenceElement(false));
    const int NNFace = faceRefElement->Type->numNodesOnFace;
    const int NN = FaceElements->numNodes;
    const int numDim = Nodes->numDim;
    const int* faceNodes = faceRefElement->Type->faceNodes;
   
    if (NNFace <= 0) {
        std::stringstream ss;
        ss << "Mesh::glueFaces: glueing faces cannot be applied to face "
            "elements of type " << faceRefElement->Type->Name;
        throw escript::ValueError(ss.str());
    }

    // allocate work arrays
    int* elem1 = new int[FaceElements->numElements];
    int* elem0 = new int[FaceElements->numElements];
    IndexVector elem_mask(FaceElements->numElements, 0);
    int* matching_nodes_in_elem1 = new int[FaceElements->numElements*NN];
    IndexVector new_node_label(Nodes->getNumNodes());
    // find the matching face elements
    int numPairs;
    findMatchingFaces(safety_factor, tolerance, &numPairs, elem0, elem1,
                      matching_nodes_in_elem1);
    for (index_t n = 0; n < Nodes->getNumNodes(); n++)
        new_node_label[n] = n;
    // mark matching face elements to be removed
    for (int e = 0; e < numPairs; e++) {
        elem_mask[elem0[e]] = 1;
        elem_mask[elem1[e]] = 1;
        for (int i = 0; i < NNFace; i++) {
            const int face_node = faceNodes[i];
            new_node_label[matching_nodes_in_elem1[INDEX2(face_node,e,NN)]] =
                    FaceElements->Nodes[INDEX2(face_node,elem0[e],NN)];
        }
    }
    // create an index of face elements
    dim_t new_numFaceElements = 0;
    for (index_t e = 0; e < FaceElements->numElements; e++) {
        if (elem_mask[e] < 1) {
            elem_mask[new_numFaceElements] = e;
            new_numFaceElements++;
        }
    }
    // get the new number of nodes
    IndexVector new_node_mask(Nodes->getNumNodes(), -1);
    IndexVector new_node_list;
    dim_t newNumNodes = 0;
    for (index_t n = 0; n < Nodes->getNumNodes(); n++)
        new_node_mask[new_node_label[n]] = 1;
    for (index_t n = 0; n < Nodes->getNumNodes(); n++) {
        if (new_node_mask[n] > 0) {
            new_node_mask[n] = newNumNodes;
            new_node_list.push_back(n);
            newNumNodes++;
        }
    }
    for (index_t n = 0; n < Nodes->getNumNodes(); n++)
        new_node_label[n] = new_node_mask[new_node_label[n]];
    // allocate new node and element files
    NodeFile* newNodeFile = new NodeFile(numDim, MPIInfo); 
    newNodeFile->allocTable(newNumNodes);
    ElementFile* newFaceElementsFile = new ElementFile(
            FaceElements->referenceElementSet, MPIInfo);
    newFaceElementsFile->allocTable(new_numFaceElements);
    // get the new nodes
    newNodeFile->gather(&new_node_list[0], Nodes);
    // they are the new nodes
    delete Nodes;
    Nodes = newNodeFile;
    // get the face elements which are still in use
    newFaceElementsFile->gather(&elem_mask[0], FaceElements);
    // they are the new face elements
    delete FaceElements;
    FaceElements = newFaceElementsFile;

    // assign new node ids to elements
    relabelElementNodes(new_node_label, 0);
    prepare(optimize);
    delete[] elem1;
    delete[] elem0;
    delete[] matching_nodes_in_elem1;
}

} // namespace finley

