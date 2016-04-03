
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

  Assembles the system of numEqu PDEs into the stiffness matrix S and right
  hand side F:

      -div(A*grad u)-div(B*u)+C*grad u + D*u = -div X + Y

      -(A_{k,i,m,j} u_m,j)_i-(B_{k,i,m} u_m)_i+C_{k,m,j} u_m,j-D_{k,m} u_m = -(X_{k,i})_i + Y_k

  u has numEqu components.
  Shape of the coefficients:

      A = numEqu x numDim x numEqu x numDim
      B = numDim x numEqu x numEqu
      C = numEqu x numDim x numEqu
      D = numEqu x numEqu
      X = numEqu x numDim
      Y = numEqu

  The coefficients A,B,C,D,X and Y have to be defined on the integration points
  or not present (i.e. empty).

  S and F have to be initialised before the routine is called. S or F can be
  NULL. In this case the left or the right hand side of the PDE is not
  processed.

  The routine does not consider any boundary conditions.

*****************************************************************************/

#include "Assemble.h"
#include "Util.h"

namespace dudley {

inline void setNumSamplesError(const char* c, int n0, int n1)
{
    std::stringstream ss;
    ss << "Assemble_PDE: number of sample points of coefficient " << c
        << " don't match (" << n0 << "," << n1 << ").";
    const std::string errorMsg(ss.str());
    throw DudleyException(errorMsg);
}

inline void setShapeError(const char* c, int num, const int* dims)
{
    std::stringstream ss;
    ss << "Assemble_PDE: shape of coefficient " << c
        << " does not match (" << dims[0] << ",";
    if (num > 1) {
       ss << dims[1];
       if (num > 2) {
           ss << "," << dims[2];
           if (num > 3) {
               ss << "," << dims[3];
           }
       }
    }
    ss << ").";
    const std::string errorMsg(ss.str());
    throw DudleyException(errorMsg);
}

void Assemble_PDE(const NodeFile* nodes, const ElementFile* elements,
                  escript::ASM_ptr S, escript::Data& F,
                  const escript::Data& A, const escript::Data& B,
                  const escript::Data& C, const escript::Data& D,
                  const escript::Data& X, const escript::Data& Y)
{
    if (!nodes || !elements || (S.get()==NULL && F.isEmpty()))
        return;

    if (F.isEmpty() && (!X.isEmpty() || !Y.isEmpty())) {
        throw DudleyException("Assemble_PDE: right hand side coefficients are non-zero but no right hand side vector given.");
    }

    if (S.get()==NULL && !A.isEmpty() && !B.isEmpty() && !C.isEmpty() && !D.isEmpty()) {
        throw DudleyException("Assemble_PDE: coefficients are non-zero but no matrix is given.");
    }

    // get the functionspace for this assemblage call
    int funcspace = -1;
    if (!A.isEmpty()) funcspace=A.getFunctionSpace().getTypeCode();
    if (!B.isEmpty()) funcspace=B.getFunctionSpace().getTypeCode();
    if (!C.isEmpty()) funcspace=C.getFunctionSpace().getTypeCode();
    if (!D.isEmpty()) funcspace=D.getFunctionSpace().getTypeCode();
    if (!X.isEmpty()) funcspace=X.getFunctionSpace().getTypeCode();
    if (!Y.isEmpty()) funcspace=Y.getFunctionSpace().getTypeCode();
    if (funcspace == -1)
        return; // all data are empty

    // check if all function spaces are the same
    if (!A.isEmpty() && A.getFunctionSpace().getTypeCode()!=funcspace) {
        throw DudleyException("Assemble_PDE: unexpected function space type for coefficient A");
    } else if (!B.isEmpty() && B.getFunctionSpace().getTypeCode()!=funcspace) {
        throw DudleyException("Assemble_PDE: unexpected function space type for coefficient B");
    } else if (!C.isEmpty() && C.getFunctionSpace().getTypeCode()!=funcspace) {
        throw DudleyException("Assemble_PDE: unexpected function space type for coefficient C");
    } else if (!D.isEmpty() && D.getFunctionSpace().getTypeCode()!=funcspace) {
        throw DudleyException("Assemble_PDE: unexpected function space type for coefficient D");
    } else if (!X.isEmpty() && X.getFunctionSpace().getTypeCode()!=funcspace) {
        throw DudleyException("Assemble_PDE: unexpected function space type for coefficient X");
    } else if (!Y.isEmpty() && Y.getFunctionSpace().getTypeCode()!=funcspace) {
        throw DudleyException("Assemble_PDE: unexpected function space type for coefficient Y");
    }

    bool reducedIntegrationOrder;
    if (funcspace == DUDLEY_ELEMENTS) {
        reducedIntegrationOrder = false;
    } else if (funcspace == DUDLEY_FACE_ELEMENTS) {
        reducedIntegrationOrder = false;
    } else if (funcspace == DUDLEY_REDUCED_ELEMENTS) {
        reducedIntegrationOrder = true;
    } else if (funcspace == DUDLEY_REDUCED_FACE_ELEMENTS) {
        reducedIntegrationOrder = true;
    } else if (funcspace == DUDLEY_POINTS) {
        reducedIntegrationOrder = true;
    } else {
        throw DudleyException("Assemble_PDE: assemblage failed because of illegal function space.");
    }

    // get assemblage parameters
    AssembleParameters p(nodes, elements, S, F, reducedIntegrationOrder);

    // check if sample numbers are the same
    if (!A.numSamplesEqual(p.numQuad, elements->numElements)) {
        setNumSamplesError("A", p.numQuad, elements->numElements);
    } else if (!B.numSamplesEqual(p.numQuad, elements->numElements)) {
        setNumSamplesError("B", p.numQuad, elements->numElements);
    } else if (!C.numSamplesEqual(p.numQuad, elements->numElements)) {
        setNumSamplesError("C", p.numQuad, elements->numElements);
    } else if (!D.numSamplesEqual(p.numQuad, elements->numElements)) {
        setNumSamplesError("D", p.numQuad, elements->numElements);
    } else if (!X.numSamplesEqual(p.numQuad, elements->numElements)) {
        setNumSamplesError("X", p.numQuad, elements->numElements);
    } else if (!Y.numSamplesEqual(p.numQuad, elements->numElements)) {
        setNumSamplesError("Y", p.numQuad, elements->numElements);
    }

    // check the dimensions
    if (p.numEqu == 1) {
        const int dimensions[2] = { p.numDim, p.numDim };
        if (!A.isDataPointShapeEqual(2, dimensions)) {
            setShapeError("A", 2, dimensions);
        } else if (!B.isDataPointShapeEqual(1, dimensions)) {
            setShapeError("B", 1, dimensions);
        } else if (!C.isDataPointShapeEqual(1, dimensions)) {
            setShapeError("C", 1, dimensions);
        } else if (!D.isDataPointShapeEqual(0, dimensions)) {
            throw DudleyException("Assemble_PDE: coefficient D must be rank 0.");
        } else if (!X.isDataPointShapeEqual(1, dimensions)) {
            setShapeError("X", 1, dimensions);
        } else if (!Y.isDataPointShapeEqual(0, dimensions)) {
            throw DudleyException("Assemble_PDE: coefficient Y must be rank 0.");
        }
    } else {
        const int dimAB[4] = { p.numEqu, p.numDim, p.numEqu, p.numDim };
        const int dimCD[3] = { p.numEqu, p.numEqu, p.numDim };
        if (!A.isDataPointShapeEqual(4, dimAB)) {
            setShapeError("A", 4, dimAB);
        } else if (!B.isDataPointShapeEqual(3, dimAB)) {
            setShapeError("B", 3, dimAB);
        } else if (!C.isDataPointShapeEqual(3, dimCD)) {
            setShapeError("C", 3, dimCD);
        } else if (!D.isDataPointShapeEqual(2, dimCD)) {
            setShapeError("D", 2, dimCD);
        } else if (!X.isDataPointShapeEqual(2, dimAB)) {
            setShapeError("X", 2, dimAB);
        } else if (!Y.isDataPointShapeEqual(1, dimAB)) {
            setShapeError("Y", 1, dimAB);
        }
    }

    if (funcspace==DUDLEY_POINTS) {
        if (!A.isEmpty() || !B.isEmpty() || !C.isEmpty() || !X.isEmpty()) {
            throw DudleyException("Dudley_Assemble_PDE: Point elements require A, B, C and X to be empty.");
        } else {
            Assemble_PDE_Points(p, D, Y);
        }
    } else {
        if (p.numEqu > 1) {
            // system of PDEs
            if (p.numDim == 3) {
                Assemble_PDE_System_3D(p, A, B, C, D, X, Y);
            } else if (p.numDim == 2) {
                Assemble_PDE_System_2D(p, A, B, C, D, X, Y);
            } else {
                throw DudleyException("Assemble_PDE supports spatial dimensions 2 and 3 only.");
            }
        } else {
            // single PDE
            if (p.numDim == 3) {
                Assemble_PDE_Single_3D(p, A, B, C, D, X, Y);
            } else if (p.numDim == 2) {
                Assemble_PDE_Single_2D(p, A, B, C, D, X, Y);
            } else {
                throw DudleyException("Assemble_PDE supports spatial dimensions 2 and 3 only.");
            }
        }
    }
}

} // namespace dudley

