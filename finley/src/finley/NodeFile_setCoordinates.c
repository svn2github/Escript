/*
 ******************************************************************************
 *                                                                            *
 *       COPYRIGHT  ACcESS 2003,2004,2005 -  All Rights Reserved              *
 *                                                                            *
 * This software is the property of ACcESS. No part of this code              *
 * may be copied in any form or by any means without the expressed written    *
 * consent of ACcESS.  Copying, use or modification of this software          *
 * by any unauthorised person is illegal unless that person has a software    *
 * license agreement with ACcESS.                                             *
 *                                                                            *
 ******************************************************************************
*/

/**************************************************************/

/*   Finley: Mesh: NodeFile */

/* copies the array newX into self->coordinates */

/**************************************************************/

/*   Author: gross@access.edu.au */
/*   Version: $Id$ */

/**************************************************************/

#include "NodeFile.h"
#include "Util.h"

/**************************************************************/


void Finley_NodeFile_setCoordinates(Finley_NodeFile* self,escriptDataC* newX) {
  char error_msg[LenErrorMsg_MAX];
   int n;
   if (getDataPointSize(newX)!=self->numDim)  {
      sprintf(error_msg,"__FILE__: dimension of new coordinates has to be %d.",self->numDim);
      Finley_setError(VALUE_ERROR,error_msg);
   } else if (! numSamplesEqual(newX,1,self->numNodes)) {
         sprintf(error_msg,"__FILE__: number of give nodes must to be %d.",self->numNodes);
         Finley_setError(VALUE_ERROR,error_msg);
   } else {
          #pragma omp parallel for private(n) schedule(static)
          for (n=0;n<self->numNodes;n++)
            Finley_copyDouble(self->numDim,getSampleData(newX,n),&(self->Coordinates[INDEX2(0,n,self->numDim)]));
   }
}
/*
* $Log$
* Revision 1.2  2005/09/15 03:44:23  jgs
* Merge of development branch dev-02 back to main trunk on 2005-09-15
*
* Revision 1.1.1.1.6.1  2005/09/07 06:26:20  gross
* the solver from finley are put into the standalone package paso now
*
* Revision 1.1.1.1  2004/10/26 06:53:57  jgs
* initial import of project esys2
*
* Revision 1.5  2004/08/26 12:03:52  gross
* Some other bug in Finley_Assemble_gradient fixed.
*
* Revision 1.4  2004/07/21 05:00:54  gross
* name changes in DataC
*
* Revision 1.3  2004/07/02 04:21:13  gross
* Finley C code has been included
*
* Revision 1.2  2004/07/01 23:54:32  gross
* used DataC now
*
* Revision 1.1.1.1  2004/06/24 04:00:40  johng
* Initial version of eys using boost-python.
*
*
*/
