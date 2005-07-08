/* $Id$ */

/**************************************************************/

/*    Updates the element matrices: */

/*    assembles the system of numEq components right hand side F */

/*     -div X + Y */

/*      -(X_i)_i + Y */

/*    Shape of the coefficients: */

/*      X = numDim   */
/*      Y = 1 */

/**************************************************************/

/*   Copyrights by ACcESS Australia, 2003 */
/*   author: gross@access.edu.au */
/*   Version: $Id$ */

/**************************************************************/

#include "Common.h"

/**************************************************************/

void Finley_Assemble_RHSMatrix_Single(dim_t NS,dim_t numDim,dim_t numQuad,
                                           double* S,double* DSDX,double* Vol,
                                           dim_t NN, double* EM_F,
                                           double* X, bool_t extendedX,  
                                           double* Y, bool_t extendedY) {
   dim_t s,q,i;
   double rtmp;

   for (s=0;s<NS;s++) {
      /**************************************************************/
      /*   process X: */
      /**************************************************************/
      if (NULL!=X) {
         if (extendedX) {
           for (i=0;i<numDim;i++) {
              for (q=0;q<numQuad;q++) {
                 EM_F[INDEX2(0,s,1)]+=Vol[q]*DSDX[INDEX3(s,i,q,NS,numDim)]*X[INDEX2(i,q,numDim)];
              }
           }
         } else {
           for (i=0;i<numDim;i++) {
               if (X[i]!=0.) {
                   rtmp=0;
                   for (q=0;q<numQuad;q++) rtmp+=Vol[q]*DSDX[INDEX3(s,i,q,NS,numDim)];
                   EM_F[INDEX2(0,s,1)]+=rtmp*X[i];
               }
           }
         }
      }
      /**************************************************************/
      /*   process Y: */
      /**************************************************************/
      if (NULL!=Y) {
        if (extendedY) {
             for (q=0;q<numQuad;q++) EM_F[INDEX2(0,s,1)]+=Vol[q]*S[INDEX2(s,q,NS)]*Y[q];
         } else {
           if (Y[0]!=0.) {
               rtmp=0;
               for (q=0;q<numQuad;q++) rtmp+=Vol[q]*S[INDEX2(s,q,NS)];
               EM_F[INDEX2(0,s,1)]+=rtmp*Y[0];
           }
         }
      }
    }
}

/*
 * $Log$
 * Revision 1.2  2005/07/08 04:07:46  jgs
 * Merge of development branch back to main trunk on 2005-07-08
 *
 * Revision 1.1.1.1.2.1  2005/06/29 02:34:47  gross
 * some changes towards 64 integers in finley
 *
 * Revision 1.1.1.1  2004/10/26 06:53:57  jgs
 * initial import of project esys2
 *
 * Revision 1.2  2004/07/30 04:37:06  gross
 * escript and finley are linking now and RecMeshTest.py has been passed
 *
 * Revision 1.1  2004/07/02 04:21:13  gross
 * Finley C code has been included
 *
 *
 */
