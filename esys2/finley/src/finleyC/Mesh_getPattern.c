/* $Id$ */

/**************************************************************/

/* Finley: Mesh */

/**************************************************************/

/* Copyrights by ACcESS Australia 2003, 2004 */
/* Author: gross@access.edu.au */

/**************************************************************/

#include "Finley.h"
#include "IndexList.h"
#include "System.h"
#include "Mesh.h"

/**************************************************************/

/* returns a reference to the matrix pattern                  */

Finley_SystemMatrixPattern* Finley_getPattern(Finley_Mesh *mesh,bool_t reduce_row_order, bool_t reduce_col_order) {
   Finley_SystemMatrixPattern *out=NULL;
   Finley_ErrorCode=NO_ERROR;
   /* make sure that the requested pattern is available */
   if (reduce_row_order) {
      if (reduce_col_order) {
         if (mesh->ReducedReducedPattern==NULL) mesh->ReducedReducedPattern=Finley_makePattern(mesh,reduce_row_order,reduce_col_order);
      } else {
         if (mesh->ReducedFullPattern==NULL) mesh->ReducedFullPattern=Finley_makePattern(mesh,reduce_row_order,reduce_col_order);
      }
   } else {
      if (reduce_col_order) {
         if (mesh->FullReducedPattern==NULL) mesh->FullReducedPattern=Finley_makePattern(mesh,reduce_row_order,reduce_col_order);
      } else {
         if (mesh->FullFullPattern==NULL) mesh->FullFullPattern=Finley_makePattern(mesh,reduce_row_order,reduce_col_order);
      }
   }
   if (Finley_ErrorCode==NO_ERROR) {
      if (reduce_row_order) {
         if (reduce_col_order) {
            out=Finley_SystemMatrixPattern_reference(mesh->ReducedReducedPattern);
         } else {
            out=Finley_SystemMatrixPattern_reference(mesh->ReducedFullPattern);
         }
      } else {
         if (reduce_col_order) {
            out=Finley_SystemMatrixPattern_reference(mesh->FullReducedPattern);
         } else {
            out=Finley_SystemMatrixPattern_reference(mesh->FullFullPattern);
         }
      }
   }  
   return out;
}
Finley_SystemMatrixPattern* Finley_makePattern(Finley_Mesh *mesh,bool_t reduce_row_order, bool_t reduce_col_order) {
  double time0;
  dim_t i,n;
  index_t s,itmp,*rowLabel=NULL,*colLabel=NULL, *ptr=NULL, *index=NULL;
  Finley_IndexList* index_list=NULL;
  Finley_ErrorCode=NO_ERROR;
  
  time0=Finley_timer();
  if (reduce_col_order) {
       n=mesh->Nodes->reducedNumDegreesOfFreedom;
       colLabel=mesh->Nodes->reducedDegreeOfFreedom;
  } else {
       n=mesh->Nodes->numDegreesOfFreedom;
       colLabel=mesh->Nodes->degreeOfFreedom;
  }
     
  if (reduce_row_order) {
      n=mesh->Nodes->reducedNumDegreesOfFreedom;
      rowLabel=mesh->Nodes->reducedDegreeOfFreedom;
  } else {
      n=mesh->Nodes->numDegreesOfFreedom;
      rowLabel=mesh->Nodes->degreeOfFreedom;
  }

  index_list=TMPMEMALLOC(n,Finley_IndexList);
  ptr=MEMALLOC(n+1,index_t);
  if (! (Finley_checkPtr(index_list) || Finley_checkPtr(ptr)) ) {
      #pragma omp parallel private(i,s,itmp)
      {
        #pragma omp for schedule(static)
        for(i=0;i<n;++i) {
             index_list[i].extension=NULL;
             index_list[i].n=0;
        }
        /*  insert contributions from element matrices into colums index index_list: */
        Finley_IndexList_insertElements(index_list,mesh->Elements,
                                        reduce_row_order,rowLabel,reduce_col_order,colLabel);
        Finley_IndexList_insertElements(index_list,mesh->FaceElements,
                                        reduce_row_order,rowLabel,reduce_col_order,colLabel);
        Finley_IndexList_insertElements(index_list,mesh->ContactElements,
                                        reduce_row_order,rowLabel,reduce_col_order,colLabel);
        Finley_IndexList_insertElements(index_list,mesh->Points,
                                        reduce_row_order,rowLabel,reduce_col_order,colLabel);
        /* get the number of connections per row */
        #pragma omp for schedule(static)
        for(i=0;i<n;++i) {
            ptr[i]=Finley_IndexList_count(&index_list[i]);
        }
        /* accumalate ptr */
        /* OMP */
        #pragma omp master 
        {
            s=0;
            for(i=0;i<n;++i) {
                 itmp=ptr[i];
                 ptr[i]=s;
                 s+=itmp;
            }
            ptr[n]=s;
            /* fill index */
            index=MEMALLOC(ptr[n],index_t);
        }
        #pragma omp barrier
        if (! Finley_checkPtr(index)) {
            #pragma omp for schedule(static)
            for(i=0;i<n;++i) Finley_IndexList_toArray(&index_list[i],&index[ptr[i]]);
        }
     }
  }
  /* all done */
  /* clean up */
  if (index_list!=NULL) {
    #pragma omp parallel for private(i) 
    for(i=0;i<n;++i) Finley_IndexList_free(index_list[i].extension);
  }
  TMPMEMFREE(index_list);
  printf("timing: mesh to matrix pattern: %.4e sec\n",Finley_timer()-time0);
  if (Finley_ErrorCode!=NO_ERROR) {
    MEMFREE(ptr);
    MEMFREE(index);
    return NULL;
  } else {
    return Finley_SystemMatrixPattern_alloc(n,ptr,index);
  }
}
/*
 * $Log$
 * Revision 1.3  2005/07/08 04:07:52  jgs
 * Merge of development branch back to main trunk on 2005-07-08
 *
 * Revision 1.2  2004/12/15 07:08:33  jgs
 * *** empty log message ***
 *
 * Revision 1.1.2.5  2005/06/30 01:53:55  gross
 * a bug in coloring fixed
 *
 * Revision 1.1.2.4  2005/06/29 02:34:51  gross
 * some changes towards 64 integers in finley
 *
 * Revision 1.1.2.3  2004/11/24 01:37:14  gross
 * some changes dealing with the integer overflow in memory allocation. Finley solves 4M unknowns now
 *
 * Revision 1.1.2.1  2004/11/15 01:01:09  gross
 * and anotther missing file
 *
 */
