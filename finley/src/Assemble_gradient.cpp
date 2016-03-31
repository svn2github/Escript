
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

  Assemblage of jacobians: calculates the gradient of nodal data at
  quadrature points

*****************************************************************************/

#include "Assemble.h"
#include "Util.h"

namespace finley {

void Assemble_gradient(const NodeFile* nodes, const ElementFile* elements,
                       escript::Data& grad_data, const escript::Data& data)
{
    if (!nodes || !elements)
        return;

    const int numComps=data.getDataPointSize();
    const int NN=elements->numNodes;
    const bool reducedOrder=util::hasReducedIntegrationOrder(grad_data);
    const int data_type=data.getFunctionSpace().getTypeCode();

    bool reducedShapefunction = false;
    int numNodes = 0;
    if (data_type == FINLEY_NODES) {
        numNodes = nodes->getNumNodes();
    } else if (data_type==FINLEY_REDUCED_NODES) { 
        reducedShapefunction = true;
        numNodes = nodes->getNumReducedNodes();
    } else if (data_type==FINLEY_DEGREES_OF_FREEDOM) {
        if (elements->MPIInfo->size > 1) {
            throw escript::ValueError("Assemble_gradient: for more than one processor DEGREES_OF_FREEDOM data are not accepted as input.");
        }
        numNodes = nodes->getNumDegreesOfFreedom();
    } else if (data_type==FINLEY_REDUCED_DEGREES_OF_FREEDOM) {
        if (elements->MPIInfo->size > 1) {
            throw escript::ValueError("Assemble_gradient: for more than one processor REDUCED_DEGREES_OF_FREEDOM data are not accepted as input.");
            return;
        }
        reducedShapefunction = true;
        numNodes = nodes->getNumReducedDegreesOfFreedom();
    } else {
        throw escript::ValueError("Assemble_gradient: Cannot calculate gradient of data because of unsuitable input data representation.");
        return;
    }

    ElementFile_Jacobians *jac = elements->borrowJacobians(nodes,
            reducedShapefunction, reducedOrder);
    const_ReferenceElement_ptr refElement(elements->referenceElementSet->
            borrowReferenceElement(reducedOrder));
    const int numDim=jac->numDim;
    const int numShapes=jac->BasisFunctions->Type->numShapes;
    const int numShapesTotal=jac->numShapesTotal;
    const int numSub=jac->numSub;
    const int numQuad=jac->numQuadTotal/numSub;
    int numShapesTotal2=0;
    int s_offset=0;
    const int *nodes_selector=NULL;
  
    const int grad_data_type=grad_data.getFunctionSpace().getTypeCode();
    if (grad_data_type==FINLEY_CONTACT_ELEMENTS_2 || grad_data_type==FINLEY_REDUCED_CONTACT_ELEMENTS_2)  {
        s_offset=jac->offsets[1];
        s_offset=jac->offsets[1];
    } else {
        s_offset=jac->offsets[0];
        s_offset=jac->offsets[0];
    }
    if (data_type==FINLEY_REDUCED_NODES || data_type==FINLEY_REDUCED_DEGREES_OF_FREEDOM) {
        nodes_selector=refElement->Type->linearNodes;
        numShapesTotal2=refElement->LinearBasisFunctions->Type->numShapes * refElement->Type->numSides;
    } else { 
        nodes_selector=refElement->Type->subElementNodes;
        numShapesTotal2=refElement->BasisFunctions->Type->numShapes * refElement->Type->numSides;
    }

    // check the dimensions of data
    if (!grad_data.numSamplesEqual(numQuad*numSub, elements->numElements)) {
        throw escript::ValueError("Assemble_gradient: illegal number of samples in gradient Data object");
    } else if (!data.numSamplesEqual(1, numNodes)) {
        throw escript::ValueError("Assemble_gradient: illegal number of samples of input Data object");
    } else if (numDim*numComps != grad_data.getDataPointSize()) {
        throw escript::ValueError("Assemble_gradient: illegal number of components in gradient data object.");
    } else if (!grad_data.actsExpanded()) {
        throw escript::ValueError("Assemble_gradient: expanded Data object is expected for output data.");
    } else if (!(s_offset+numShapes <= numShapesTotal)) {
        throw escript::ValueError("Assemble_gradient: nodes per element is inconsistent with number of jacobians.");
    }

    const size_t localGradSize=sizeof(double)*numDim*numQuad*numSub*numComps;
    grad_data.requireWrite();
#pragma omp parallel
    {
        if (data_type==FINLEY_NODES) {
            if (numDim==1) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e, 0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(n);
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            } else if (numDim==2) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e, 0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(n);
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,1,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,1,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            } else if (numDim==3) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e); 
                    memset(grad_data_e,0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(n);
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,1,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,1,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,2,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,2,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            }
        } else if (data_type==FINLEY_REDUCED_NODES) {
            const index_t* target = nodes->borrowTargetReducedNodes();
            if (numDim==1) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e, 0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0;s<numShapes;s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(target[n]);            
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {                              
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            } else if (numDim==2) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e, 0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(target[n]);
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,1,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,1,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            } else if (numDim==3) {
#pragma omp for
                for (index_t e=0;e<elements->numElements;e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e, 0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(target[n]);
                            for (int q=0; q<numQuad; q++) {   
#pragma ivdep
                                for (int l=0;l<numComps;l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,1,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,1,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,2,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,2,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            }
        } else if (data_type==FINLEY_DEGREES_OF_FREEDOM) {
            const index_t* target = nodes->borrowTargetDegreesOfFreedom();
            if (numDim==1) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e, 0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(target[n]);
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            } else if (numDim==2) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e, 0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(target[n]);
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,1,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,1,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            } else if (numDim==3) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e, 0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(target[n]);
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,1,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,1,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,2,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,2,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            }
        } else if (data_type==FINLEY_REDUCED_DEGREES_OF_FREEDOM) {
            const index_t* target = nodes->borrowTargetReducedDegreesOfFreedom();
            if (numDim==1) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e,0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(target[n]);
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            } else if (numDim==2) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e, 0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(target[n]);
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,1,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,1,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }

            } else if (numDim==3) {
#pragma omp for
                for (index_t e=0; e<elements->numElements; e++) {
                    double *grad_data_e=grad_data.getSampleDataRW(e);
                    memset(grad_data_e,0, localGradSize);
                    for (int isub=0; isub<numSub; isub++) {
                        for (int s=0; s<numShapes; s++) {
                            const index_t n=elements->Nodes[INDEX2(nodes_selector[INDEX2(s_offset+s,isub,numShapesTotal2)],e, NN)];
                            const double *data_array=data.getSampleDataRO(target[n]);
                            for (int q=0; q<numQuad; q++) {
#pragma ivdep
                                for (int l=0; l<numComps; l++) {
                                    grad_data_e[INDEX4(l,0,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,0,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,1,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,1,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                    grad_data_e[INDEX4(l,2,q,isub,numComps,numDim,numQuad)]+=data_array[l]*jac->DSDX[INDEX5(s_offset+s,2,q,isub,e,numShapesTotal,numDim,numQuad,numSub)];
                                }
                            }
                        }
                    }
                }
            } // numDim
        } // data_type
    } // end parallel region
}

} // namespace finley

