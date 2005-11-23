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

/*   Finley: ElementFile */

/*   allocates and deallocates element table                  */

/**************************************************************/

/*  Author: gross@access.edu.au */
/*  Version: $Id$ */

/**************************************************************/

#include "ElementFile.h"

/****************************************************************************/

/*  allocates the element table within an element file to hold numElements: */

void Finley_ElementFile_allocTable(Finley_ElementFile* in,dim_t numElements) {
  index_t *Id2=NULL,*Nodes2=NULL,*Tag2=NULL,*Color2=NULL;
  dim_t numNodes,e,i;
  Finley_resetError();
  
  /*  allocate memory: */
  
  numNodes=(dim_t) in->ReferenceElement->Type->numNodes;
  Id2=MEMALLOC(numElements,dim_t);
  Nodes2=MEMALLOC(numElements*numNodes,dim_t);
  Tag2=MEMALLOC(numElements,dim_t);
  Color2=MEMALLOC(numElements,dim_t);
  
  /*  if fine, deallocate the old table and replace by new: */
  
  if (Finley_checkPtr(Id2) || Finley_checkPtr(Nodes2) || Finley_checkPtr(Tag2) || Finley_checkPtr(Color2)) {
    MEMFREE(Id2);
    MEMFREE(Nodes2);
    MEMFREE(Tag2);
    MEMFREE(Color2);
  } else { 
    Finley_ElementFile_deallocTable(in);
    in->numElements=numElements;
    in->Id=Id2;
    in->Nodes=Nodes2;
    in->Tag=Tag2;
    in->Color=Color2;

    /* this initialization makes sure that data are located on the right processor */

    #pragma omp parallel for private(e,i) schedule(static)
    for (e=0;e<numElements;e++) {
       for (i=0;i<numNodes;i++) in->Nodes[INDEX2(i,e,numNodes)]=-1;
       in->Id[e]=-1;
       in->Tag[e]=-1;
       in->Color[e]=-1;
    }
    in->maxColor=-1;
    in->minColor=0;
  }
  return;
}

/*  deallocates the element table within an element file: */

void Finley_ElementFile_deallocTable(Finley_ElementFile* in) {
  MEMFREE(in->Id);
  MEMFREE(in->Nodes);
  MEMFREE(in->Tag);
  MEMFREE(in->Color);
  in->numElements=0;
  in->maxColor=-1;
  in->minColor=0;
}

/* 
* $Log$
* Revision 1.6  2005/09/15 03:44:22  jgs
* Merge of development branch dev-02 back to main trunk on 2005-09-15
*
* Revision 1.5.2.1  2005/09/07 06:26:18  gross
* the solver from finley are put into the standalone package paso now
*
* Revision 1.5  2005/07/08 04:07:49  jgs
* Merge of development branch back to main trunk on 2005-07-08
*
* Revision 1.4  2004/12/15 07:08:32  jgs
* *** empty log message ***
* Revision 1.1.1.1.2.2  2005/06/29 02:34:49  gross
* some changes towards 64 integers in finley
*
* Revision 1.1.1.1.2.1  2004/11/24 01:37:13  gross
* some changes dealing with the integer overflow in memory allocation. Finley solves 4M unknowns now
*
*
*
*/
