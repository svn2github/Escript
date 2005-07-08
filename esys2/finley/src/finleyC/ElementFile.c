/* $Id$ */
/**************************************************************/

/*   Finley: ElementFile */

/*   allocates an element file to hold elements of type id and with integration order order. */
/*   use Finley_Mesh_allocElementTable to allocate the element table (Id,Nodes,Tag). */

/**************************************************************/

/*   Copyrights by ACcESS Australia 2003/04 */
/*   Author: gross@access.edu.au */
/*   Version: $Id$ */

/**************************************************************/

#include "Finley.h"
#include "ElementFile.h"

/**************************************************************/

Finley_ElementFile* Finley_ElementFile_alloc(ElementTypeId id,index_t order){
  extern Finley_RefElementInfo Finley_RefElement_InfoList[];
  dim_t NQ;
  Finley_ElementFile *out;
  
  /*   get the number of quadrature nodes needed to achieve integration order order: */
  
  if (order<0) order=2*Finley_RefElement_InfoList[id].numOrder;
  NQ= Finley_RefElement_InfoList[id].getNumQuadNodes(order);
  if (Finley_ErrorCode!=NO_ERROR) return NULL;
  
  /*  allocate the return value */
  
  out=MEMALLOC(1,Finley_ElementFile);
  if (Finley_checkPtr(out)) return NULL;
  out->ReferenceElement=NULL;
  out->LinearReferenceElement=NULL;
  out->numElements=0;
  out->Id=NULL;
  out->Nodes=NULL;
  out->Tag=NULL;
  out->Color=NULL;
  out->minColor=0;
  out->maxColor=-1;
  out->order = order;

  /*  allocate the reference element: */
  
  out->ReferenceElement=Finley_RefElement_alloc(id,NQ);
  if (Finley_ErrorCode!=NO_ERROR) {
     Finley_ElementFile_dealloc(out);
     return NULL;
  }
  out->LinearReferenceElement=Finley_RefElement_alloc(Finley_RefElement_InfoList[id].LinearTypeId,NQ);
  if (Finley_ErrorCode!=NO_ERROR) {
     Finley_ElementFile_dealloc(out);
     return NULL;
  }
  return out;
}

/*  deallocates an element file: */

void Finley_ElementFile_dealloc(Finley_ElementFile* in) {
  if (in!=NULL) {
     #ifdef Finley_TRACE
     if (in->ReferenceElement!=NULL) printf("element file for %s is deallocated.\n",in->ReferenceElement->Type->Name);
     #endif
     Finley_RefElement_dealloc(in->ReferenceElement);
     Finley_RefElement_dealloc(in->LinearReferenceElement);
     Finley_ElementFile_deallocTable(in);   
     MEMFREE(in);      
  }
}
/* 
* $Log$
* Revision 1.5  2005/07/08 04:07:48  jgs
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
