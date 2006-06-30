/*
 ************************************************************
 *          Copyright 2006 by ACcESS MNRF                   *
 *                                                          *
 *              http://www.access.edu.au                    *
 *       Primary Business: Queensland, Australia            *
 *  Licensed under the Open Software License version 3.0    *
 *     http://www.opensource.org/licenses/osl-3.0.php       *
 *                                                          *
 ************************************************************
*/

/**************************************************************/

/*   Finley: Mesh: ElementFile */

/*  set tags to newTag where mask>0 */

/**************************************************************/

/*   Author: gross@access.edu.au */
/*   Version: $Id$ */

/**************************************************************/

#include "ElementFile.h"
#include "Util.h"

/**************************************************************/


void Finley_ElementFile_setTags(Finley_ElementFile* self,const int newTag, escriptDataC* mask) {
    if (self==NULL) return;
    register dim_t n,q;
    dim_t numElements=self->numElements;
    dim_t numQuad=self->ReferenceElement->numQuadNodes;
    register double *mask_array;
    register bool_t check;
    Finley_resetError();

    if (1!=getDataPointSize(mask)) {
       Finley_setError(TYPE_ERROR,"Finley_ElementFile_setTags: number of components of mask is 1.");
    } else if (!numSamplesEqual(mask,numQuad,numElements)) {
       Finley_setError(TYPE_ERROR,"Finley_ElementFile_setTags: illegal number of samples of mask Data object");
    }

    /* now we can start */

    if (Finley_noError()) {
         if (isExpanded(mask)) {
             #pragma omp parallel for private(n,check,mask_array) schedule(static)
             for (n=0;n<numElements;n++) {
                 mask_array=getSampleData(mask,n);
                 if (mask_array[0]>0) self->Tag[n]=newTag;
             }
         } else {
             #pragma omp parallel for private(q,n,check,mask_array) schedule(static)
             for (n=0;n<numElements;n++) {
                 mask_array=getSampleData(mask,n);
                 check=FALSE;
                 for (q=0;q<numQuad;q++) check=check || mask_array[q];
                 if (check) self->Tag[n]=newTag;
             }
         }
    }
}
/*
* $Log$
*
*/
