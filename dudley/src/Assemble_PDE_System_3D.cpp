
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

/****************************************************************************

  Assembles the system of numEqu PDEs into the stiffness matrix S and right
  hand side F

      -(A_{k,i,m,j} u_m,j)_i-(B_{k,i,m} u_m)_i+C_{k,m,j} u_m,j-D_{k,m} u_m
  and
      -(X_{k,i})_i + Y_k

  u has p.numComp components in a 3D domain. The shape functions for test and
  solution must be identical and row_NS == row_NN

  Shape of the coefficients:

      A = p.numEqu x 3 x p.numComp x 3
      B = 3 x p.numEqu x p.numComp
      C = p.numEqu x 3 x p.numComp
      D = p.numEqu x p.numComp
      X = p.numEqu x 3
      Y = p.numEqu

*****************************************************************************/

#include "Assemble.h"
#include "Util.h"

namespace dudley {

void Assemble_PDE_System_3D(const Assemble_Parameters& p, const Dudley_ElementFile* elements,
                            escript::ASM_ptr mat, escript::Data& F,
                            const escript::Data& A, const escript::Data& B,
                            const escript::Data& C, const escript::Data& D,
                            const escript::Data& X, const escript::Data& Y)
{
    const int DIM = 3;
    bool expandedA = A.actsExpanded();
    bool expandedB = B.actsExpanded();
    bool expandedC = C.actsExpanded();
    bool expandedD = D.actsExpanded();
    bool expandedX = X.actsExpanded();
    bool expandedY = Y.actsExpanded();
    double* F_p = NULL;
    if (!F.isEmpty()) {
        F.requireWrite();
        F_p = F.getSampleDataRW(0);
    }
    const double* S = p.shapeFns;
    const size_t len_EM_S = p.numShapes * p.numShapes * p.numEqu * p.numComp;
    const size_t len_EM_F = p.numShapes * p.numEqu;

#pragma omp parallel
    {
        std::vector<double> EM_S(len_EM_S);
        std::vector<double> EM_F(len_EM_F);
        std::vector<index_t> row_index(p.numShapes);

        for (int color = elements->minColor; color <= elements->maxColor; color++) {
            // loop over all elements
#pragma omp for
            for (index_t e = 0; e < elements->numElements; e++) {
                if (elements->Color[e] == color) {
                    const double vol = p.row_jac->absD[e] * p.row_jac->quadweight;
                    const double* DSDX = &p.row_jac->DSDX[INDEX5(0, 0, 0, 0, e, p.numShapes, DIM, p.numQuad, 1)];
                    std::fill(EM_S.begin(), EM_S.end(), 0);
                    std::fill(EM_F.begin(), EM_F.end(), 0);
                    bool add_EM_F = false;
                    bool add_EM_S = false;

                    ///////////////
                    // process A //
                    ///////////////
                    if (!A.isEmpty()) {
                        const double* A_p = A.getSampleDataRO(e);
                        add_EM_S = true;
                        if (expandedA) {
                            const double* A_q = &A_p[INDEX6(0, 0, 0, 0, 0, 0, p.numEqu, DIM, p.numComp, DIM, p.numQuad)];
                            for (int s = 0; s < p.numShapes; s++) {
                                for (int r = 0; r < p.numShapes; r++) {
                                    for (int k = 0; k < p.numEqu; k++) {
                                        for (int m = 0; m < p.numComp; m++) {
                                            double f = 0;
                                            for (int q = 0; q < p.numQuad; q++) {
                                                f +=
                                                    vol * (DSDX[INDEX3(s, 0, q, p.numShapes, DIM)] *
                                                           A_q[INDEX5(k, 0, m, 0, q, p.numEqu, DIM, p.numComp, DIM)]
                                                           * DSDX[INDEX3(r, 0, q, p.numShapes, DIM)] +
                                                           DSDX[INDEX3(s, 0, q, p.numShapes, DIM)] *
                                                           A_q[INDEX5(k, 0, m, 1, q, p.numEqu, DIM, p.numComp, DIM)]
                                                           * DSDX[INDEX3(r, 1, q, p.numShapes, DIM)] +
                                                           DSDX[INDEX3(s, 0, q, p.numShapes, DIM)] *
                                                           A_q[INDEX5(k, 0, m, 2, q, p.numEqu, DIM, p.numComp, DIM)]
                                                           * DSDX[INDEX3(r, 2, q, p.numShapes, DIM)] +
                                                           DSDX[INDEX3(s, 1, q, p.numShapes, DIM)] *
                                                           A_q[INDEX5(k, 1, m, 0, q, p.numEqu, DIM, p.numComp, DIM)]
                                                           * DSDX[INDEX3(r, 0, q, p.numShapes, DIM)] +
                                                           DSDX[INDEX3(s, 1, q, p.numShapes, DIM)] *
                                                           A_q[INDEX5(k, 1, m, 1, q, p.numEqu, DIM, p.numComp, DIM)]
                                                           * DSDX[INDEX3(r, 1, q, p.numShapes, DIM)] +
                                                           DSDX[INDEX3(s, 1, q, p.numShapes, DIM)] *
                                                           A_q[INDEX5(k, 1, m, 2, q, p.numEqu, DIM, p.numComp, DIM)]
                                                           * DSDX[INDEX3(r, 2, q, p.numShapes, DIM)] +
                                                           DSDX[INDEX3(s, 2, q, p.numShapes, DIM)] *
                                                           A_q[INDEX5(k, 2, m, 0, q, p.numEqu, DIM, p.numComp, DIM)]
                                                           * DSDX[INDEX3(r, 0, q, p.numShapes, DIM)] +
                                                           DSDX[INDEX3(s, 2, q, p.numShapes, DIM)] *
                                                           A_q[INDEX5(k, 2, m, 1, q, p.numEqu, DIM, p.numComp, DIM)]
                                                           * DSDX[INDEX3(r, 1, q, p.numShapes, DIM)] +
                                                           DSDX[INDEX3(s, 2, q, p.numShapes, DIM)] *
                                                           A_q[INDEX5(k, 2, m, 2, q, p.numEqu, DIM, p.numComp, DIM)]
                                                           * DSDX[INDEX3(r, 2, q, p.numShapes, DIM)]);

                                            }
                                            EM_S[INDEX4(k, m, s, r, p.numEqu, p.numComp, p.numShapes)] += f;
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            for (int s = 0; s < p.numShapes; s++) {
                                for (int r = 0; r < p.numShapes; r++) {
                                    double f00 = 0;
                                    double f01 = 0;
                                    double f02 = 0;
                                    double f10 = 0;
                                    double f11 = 0;
                                    double f12 = 0;
                                    double f20 = 0;
                                    double f21 = 0;
                                    double f22 = 0;
                                    for (int q = 0; q < p.numQuad; q++) {
                                        const double f0 = vol * DSDX[INDEX3(s, 0, q, p.numShapes, DIM)];
                                        f00 += f0 * DSDX[INDEX3(r, 0, q, p.numShapes, DIM)];
                                        f01 += f0 * DSDX[INDEX3(r, 1, q, p.numShapes, DIM)];
                                        f02 += f0 * DSDX[INDEX3(r, 2, q, p.numShapes, DIM)];

                                        const double f1 = vol * DSDX[INDEX3(s, 1, q, p.numShapes, DIM)];
                                        f10 += f1 * DSDX[INDEX3(r, 0, q, p.numShapes, DIM)];
                                        f11 += f1 * DSDX[INDEX3(r, 1, q, p.numShapes, DIM)];
                                        f12 += f1 * DSDX[INDEX3(r, 2, q, p.numShapes, DIM)];

                                        const double f2 = vol * DSDX[INDEX3(s, 2, q, p.numShapes, DIM)];
                                        f20 += f2 * DSDX[INDEX3(r, 0, q, p.numShapes, DIM)];
                                        f21 += f2 * DSDX[INDEX3(r, 1, q, p.numShapes, DIM)];
                                        f22 += f2 * DSDX[INDEX3(r, 2, q, p.numShapes, DIM)];
                                    }
                                    for (int k = 0; k < p.numEqu; k++) {
                                        for (int m = 0; m < p.numComp; m++) {
                                            EM_S[INDEX4(k, m, s, r, p.numEqu, p.numComp, p.numShapes)] +=
                                                f00 * A_p[INDEX4(k, 0, m, 0, p.numEqu, DIM, p.numComp)] +
                                                f01 * A_p[INDEX4(k, 0, m, 1, p.numEqu, DIM, p.numComp)] +
                                                f02 * A_p[INDEX4(k, 0, m, 2, p.numEqu, DIM, p.numComp)] +
                                                f10 * A_p[INDEX4(k, 1, m, 0, p.numEqu, DIM, p.numComp)] +
                                                f11 * A_p[INDEX4(k, 1, m, 1, p.numEqu, DIM, p.numComp)] +
                                                f12 * A_p[INDEX4(k, 1, m, 2, p.numEqu, DIM, p.numComp)] +
                                                f20 * A_p[INDEX4(k, 2, m, 0, p.numEqu, DIM, p.numComp)] +
                                                f21 * A_p[INDEX4(k, 2, m, 1, p.numEqu, DIM, p.numComp)] +
                                                f22 * A_p[INDEX4(k, 2, m, 2, p.numEqu, DIM, p.numComp)];
                                        }
                                    }
                                }
                            }
                        }
                    }
                    ///////////////
                    // process B //
                    ///////////////
                    if (!B.isEmpty()) {
                        const double* B_p = B.getSampleDataRO(e);
                        add_EM_S = true;
                        if (expandedB) {
                            const double* B_q = &B_p[INDEX5(0, 0, 0, 0, 0, p.numEqu, DIM, p.numComp, p.numQuad)];
                            for (int s = 0; s < p.numShapes; s++) {
                                for (int r = 0; r < p.numShapes; r++) {
                                    for (int k = 0; k < p.numEqu; k++) {
                                        for (int m = 0; m < p.numComp; m++) {
                                            double f = 0;
                                            for (int q = 0; q < p.numQuad; q++) {
                                                f +=
                                                    vol * S[INDEX2(r, q, p.numShapes)] *
                                                    (DSDX[INDEX3(s, 0, q, p.numShapes, DIM)] *
                                                     B_q[INDEX4(k, 0, m, q, p.numEqu, DIM, p.numComp)] +
                                                     DSDX[INDEX3(s, 1, q, p.numShapes, DIM)] *
                                                     B_q[INDEX4(k, 1, m, q, p.numEqu, DIM, p.numComp)] +
                                                     DSDX[INDEX3(s, 2, q, p.numShapes, DIM)] *
                                                     B_q[INDEX4(k, 2, m, q, p.numEqu, DIM, p.numComp)]);
                                            }
                                            EM_S[INDEX4(k, m, s, r, p.numEqu, p.numComp, p.numShapes)] += f;
                                        }
                                    }
                                }
                            }
                        } else {
                            for (int s = 0; s < p.numShapes; s++) {
                                for (int r = 0; r < p.numShapes; r++) {
                                    double f0 = 0;
                                    double f1 = 0;
                                    double f2 = 0;
                                    for (int q = 0; q < p.numQuad; q++) {
                                        const double f = vol * S[INDEX2(r, q, p.numShapes)];
                                        f0 += f * DSDX[INDEX3(s, 0, q, p.numShapes, DIM)];
                                        f1 += f * DSDX[INDEX3(s, 1, q, p.numShapes, DIM)];
                                        f2 += f * DSDX[INDEX3(s, 2, q, p.numShapes, DIM)];
                                    }
                                    for (int k = 0; k < p.numEqu; k++) {
                                        for (int m = 0; m < p.numComp; m++) {
                                            EM_S[INDEX4(k, m, s, r, p.numEqu, p.numComp, p.numShapes)] +=
                                                f0 * B_p[INDEX3(k, 0, m, p.numEqu, DIM)] +
                                                f1 * B_p[INDEX3(k, 1, m, p.numEqu, DIM)] +
                                                f2 * B_p[INDEX3(k, 2, m, p.numEqu, DIM)];
                                        }
                                    }
                                }
                            }
                        }
                    }
                    ///////////////
                    // process C //
                    ///////////////
                    if (!C.isEmpty()) {
                        const double* C_p = C.getSampleDataRO(e);
                        add_EM_S = true;
                        if (expandedC) {
                            const double* C_q = &C_p[INDEX5(0, 0, 0, 0, 0, p.numEqu, p.numComp, DIM, p.numQuad)];
                            for (int s = 0; s < p.numShapes; s++) {
                                for (int r = 0; r < p.numShapes; r++) {
                                    for (int k = 0; k < p.numEqu; k++) {
                                        for (int m = 0; m < p.numComp; m++) {
                                            double f = 0;
                                            for (int q = 0; q < p.numQuad; q++) {
                                                f +=
                                                    vol * S[INDEX2(s, q, p.numShapes)] *
                                                    (C_q[INDEX4(k, m, 0, q, p.numEqu, p.numComp, DIM)] *
                                                     DSDX[INDEX3(r, 0, q, p.numShapes, DIM)] +
                                                     C_q[INDEX4(k, m, 1, q, p.numEqu, p.numComp, DIM)] *
                                                     DSDX[INDEX3(r, 1, q, p.numShapes, DIM)] +
                                                     C_q[INDEX4(k, m, 2, q, p.numEqu, p.numComp, DIM)] *
                                                     DSDX[INDEX3(r, 2, q, p.numShapes, DIM)]);
                                            }
                                            EM_S[INDEX4(k, m, s, r, p.numEqu, p.numComp, p.numShapes)] += f;
                                        }
                                    }
                                }
                            }
                        } else {
                            for (int s = 0; s < p.numShapes; s++) {
                                for (int r = 0; r < p.numShapes; r++) {
                                    double f0 = 0;
                                    double f1 = 0;
                                    double f2 = 0;
                                    for (int q = 0; q < p.numQuad; q++) {
                                        const double f = vol * S[INDEX2(s, q, p.numShapes)];
                                        f0 += f * DSDX[INDEX3(r, 0, q, p.numShapes, DIM)];
                                        f1 += f * DSDX[INDEX3(r, 1, q, p.numShapes, DIM)];
                                        f2 += f * DSDX[INDEX3(r, 2, q, p.numShapes, DIM)];
                                    }
                                    for (int k = 0; k < p.numEqu; k++) {
                                        for (int m = 0; m < p.numComp; m++)
                                        {
                                            EM_S[INDEX4(k, m, s, r, p.numEqu, p.numComp, p.numShapes)] +=
                                                f0 * C_p[INDEX3(k, m, 0, p.numEqu, p.numComp)] +
                                                f1 * C_p[INDEX3(k, m, 1, p.numEqu, p.numComp)] +
                                                f2 * C_p[INDEX3(k, m, 2, p.numEqu, p.numComp)];
                                        }
                                    }
                                }
                            }
                        }
                    }
                    ///////////////
                    // process D //
                    ///////////////
                    if (!D.isEmpty()) {
                        const double* D_p = D.getSampleDataRO(e);
                        add_EM_S = true;
                        if (expandedD) {
                            const double* D_q = &D_p[INDEX4(0, 0, 0, 0, p.numEqu, p.numComp, p.numQuad)];
                            for (int s = 0; s < p.numShapes; s++) {
                                for (int r = 0; r < p.numShapes; r++) {
                                    for (int k = 0; k < p.numEqu; k++) {
                                        for (int m = 0; m < p.numComp; m++) {
                                            double f = 0;
                                            for (int q = 0; q < p.numQuad; q++) {
                                                f +=
                                                    vol * S[INDEX2(s, q, p.numShapes)] *
                                                    D_q[INDEX3(k, m, q, p.numEqu, p.numComp)] *
                                                    S[INDEX2(r, q, p.numShapes)];
                                            }
                                            EM_S[INDEX4(k, m, s, r, p.numEqu, p.numComp, p.numShapes)] += f;
                                        }
                                    }
                                }
                            }
                        } else {
                            for (int s = 0; s < p.numShapes; s++) {
                                for (int r = 0; r < p.numShapes; r++) {
                                    double f = 0;
                                    for (int q = 0; q < p.numQuad; q++)
                                        f += vol * S[INDEX2(s, q, p.numShapes)] * S[INDEX2(r, q, p.numShapes)];
                                    for (int k = 0; k < p.numEqu; k++) {
                                        for (int m = 0; m < p.numComp; m++) {
                                            EM_S[INDEX4(k, m, s, r, p.numEqu, p.numComp, p.numShapes)] +=
                                                f * D_p[INDEX2(k, m, p.numEqu)];
                                        }
                                    }
                                }
                            }
                        }
                    }
                    ///////////////
                    // process X //
                    ///////////////
                    if (!X.isEmpty()) {
                        const double* X_p = X.getSampleDataRO(e);
                        add_EM_F = true;
                        if (expandedX) {
                            const double* X_q = &X_p[INDEX4(0, 0, 0, 0, p.numEqu, DIM, p.numQuad)];
                            for (int s = 0; s < p.numShapes; s++) {
                                for (int k = 0; k < p.numEqu; k++) {
                                    double f = 0;
                                    for (int q = 0; q < p.numQuad; q++) {
                                        f +=
                                            vol * (DSDX[INDEX3(s, 0, q, p.numShapes, DIM)] *
                                                   X_q[INDEX3(k, 0, q, p.numEqu, DIM)] +
                                                   DSDX[INDEX3(s, 1, q, p.numShapes, DIM)] *
                                                   X_q[INDEX3(k, 1, q, p.numEqu, DIM)] +
                                                   DSDX[INDEX3(s, 2, q, p.numShapes, DIM)] *
                                                   X_q[INDEX3(k, 2, q, p.numEqu, DIM)]);
                                    }
                                    EM_F[INDEX2(k, s, p.numEqu)] += f;
                                }
                            }
                        } else {
                            for (int s = 0; s < p.numShapes; s++) {
                                double f0 = 0;
                                double f1 = 0;
                                double f2 = 0;
                                for (int q = 0; q < p.numQuad; q++) {
                                    f0 += vol * DSDX[INDEX3(s, 0, q, p.numShapes, DIM)];
                                    f1 += vol * DSDX[INDEX3(s, 1, q, p.numShapes, DIM)];
                                    f2 += vol * DSDX[INDEX3(s, 2, q, p.numShapes, DIM)];
                                }
                                for (int k = 0; k < p.numEqu; k++) {
                                    EM_F[INDEX2(k, s, p.numEqu)] += f0 * X_p[INDEX2(k, 0, p.numEqu)]
                                        + f1 * X_p[INDEX2(k, 1, p.numEqu)] + f2 * X_p[INDEX2(k, 2, p.numEqu)];
                                }
                            }
                        }
                    }
                    ///////////////
                    // process Y //
                    ///////////////
                    if (!Y.isEmpty()) {
                        const double* Y_p = Y.getSampleDataRO(e);
                        add_EM_F = true;
                        if (expandedY) {
                            const double* Y_q = &Y_p[INDEX3(0, 0, 0, p.numEqu, p.numQuad)];
                            for (int s = 0; s < p.numShapes; s++) {
                                for (int k = 0; k < p.numEqu; k++) {
                                    double f = 0.;
                                    for (int q = 0; q < p.numQuad; q++)
                                        f += vol * S[INDEX2(s, q, p.numShapes)] * Y_q[INDEX2(k, q, p.numEqu)];
                                    EM_F[INDEX2(k, s, p.numEqu)] += f;
                                }
                            }
                        } else {
                            for (int s = 0; s < p.numShapes; s++) {
                                double f = 0;
                                for (int q = 0; q < p.numQuad; q++)
                                    f += vol * S[INDEX2(s, q, p.numShapes)];
                                for (int k = 0; k < p.numEqu; k++)
                                    EM_F[INDEX2(k, s, p.numEqu)] += f * Y_p[k];
                            }
                        }
                    }

                    // add the element matrices onto the matrix and right
                    // hand side
                    for (int q = 0; q < p.numShapes; q++)
                        row_index[q] = p.row_DOF[elements->Nodes[INDEX2(q, e, p.NN)]];

                    if (add_EM_F)
                        Dudley_Util_AddScatter(p.numShapes, &row_index[0],
                                p.numEqu, &EM_F[0], F_p, p.row_DOF_UpperBound);
                    if (add_EM_S)
                        Assemble_addToSystemMatrix(mat, p.numShapes,
                                &row_index[0], p.numEqu, p.numShapes,
                                &row_index[0], p.numComp, &EM_S[0]);
                } // end color check
            } // end element loop
        } // end color loop
    } // end parallel region
}

} // namespace dudley
