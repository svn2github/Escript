
/*****************************************************************************
*
* Copyright (c) 2003-2013 by University of Queensland
* http://www.uq.edu.au
*
* Primary Business: Queensland, Australia
* Licensed under the Open Software License version 3.0
* http://www.opensource.org/licenses/osl-3.0.php
*
* Development until 2012 by Earth Systems Science Computational Center (ESSCC)
* Development since 2012 by School of Earth Sciences
*
*****************************************************************************/


/****************************************************************************

  Assembles the system of numEq PDEs into the stiffness matrix S and right
  hand side F

      d_dirac_{k,m} u_m and y_dirac_k

  u has p.numComp components in a 3D domain.
  The shape functions for test and solution must be identical and
  row_NS == row_NN.

  Shape of the coefficients:

      d_dirac = p.numEqu x p.numComp
      y_dirac = p.numEqu

*****************************************************************************/

#include "Assemble.h"
#include "Util.h"

namespace finley {

void Assemble_PDE_Points(AssembleParameters p, ElementFile* elements,
                         Paso_SystemMatrix* S, escript::Data& F,
                         escript::Data& d_dirac, escript::Data& y_dirac)
{
    F.requireWrite();
    double *F_p=F.getSampleDataRW(0);

#pragma omp parallel
    {
        for (int color=elements->minColor; color<=elements->maxColor; color++) {
            // loop over all elements
#pragma omp for
            for (int e=0; e<elements->numElements; e++) {
                if (elements->Color[e]==color) {
                    int row_index=p.row_DOF[elements->Nodes[INDEX2(0,e,p.NN)]];
                    if (!y_dirac.isEmpty()) {
                        const double *y_dirac_p=y_dirac.getSampleDataRO(e);
                        util::addScatter(1, &row_index, p.numEqu,
                                         y_dirac_p, F_p, p.row_DOF_UpperBound);
                    }
                    if (!d_dirac.isEmpty()) {
                        const double *d_dirac_p=d_dirac.getSampleDataRO(e);
                        Assemble_addToSystemMatrix(S, 1, &row_index,
                                p.numEqu, 1, &row_index, p.numComp, d_dirac_p);
                    }
                } // end color check
            } // end element loop
        } // end color loop
    } // end parallel section
}

} // namespace finley

