/* $Id$ */

/**************************************************************/

/*   Some utility routines: */

/**************************************************************/

/*   Copyrights by ACcESS Australia, 2003 */
/*   author: gross@access.edu.au */
/*   Version: $Id$ */

/**************************************************************/

#include "Common.h"
#include "Finley.h"
#include "Util.h"

/**************************************************************/

/*   gathers double values out from in by index: */

/*        out(1:numData,1:len)=in(1:numData,index(1:len)) */

void Finley_Util_Gather_double(int len,maybelong* index,int numData,double* in, double * out){
    int s,i;
    for (s=0;s<len;s++) {
       for (i=0;i<numData;i++) {
          out[INDEX2(i,s,numData)]=in[INDEX2(i,index[s],numData)];
       }
    }
}

/**************************************************************/


/*   gathers maybelong values out from in by index: */

/*        out(1:numData,1:len)=in(1:numData,index(1:len)) */

void Finley_Util_Gather_int(int len,maybelong* index,int numData, maybelong* in, maybelong * out){
    int s,i;
    for (s=0;s<len;s++) {
       for (i=0;i<numData;i++) {
          out[INDEX2(i,s,numData)]=in[INDEX2(i,index[s],numData)];
       }
    }
}

/**************************************************************/

/*   adds a vector in into out using and index. */

/*        out(1:numData,index(1:len))+=in(1:numData,1:len) */

void Finley_Util_AddScatter(int len,maybelong* index,int numData,double* in,double * out){
   int i,s;
   for (s=0;s<len;s++) {
       for(i=0;i<numData;i++) {
          #pragma omp atomic
          out[INDEX2(i,index[s],numData)]+=in[INDEX2(i,s,numData)];
       }
   }
}

/*    multiplies two matrices */

/*          A(1:A1,1:A2)=B(1:A1,1:B2)*C(1:B2,1:A2) */

void Finley_Util_SmallMatMult(int A1,int A2, double* A, int B2, double*B, double* C) {
    int i,j,s;
    for (i=0;i<A1*A2;i++) A[i]=0;
       for (i=0;i<A1;i++) {
          for (j=0;j<A2;j++) {
             for (s=0;s<B2;s++) {
                A[INDEX2(i,j,A1)]+=B[INDEX2(i,s,A1)]*C[INDEX2(s,j,B2)];
             }
          }
       }
}

/*    multiplies a two sets of matries: */

/*        A(1:A1,1:A2,i)=B(1:A1,1:B2,i)*C(1:B2,1:A2,i) i=1,len */

void Finley_Util_SmallMatSetMult(int len,int A1,int A2, double* A, int B2, double*B, double* C) {
    int q,i,j,s;
    for (i=0;i<A1*A2*len;i++) A[i]=0;
    for (q=0;q<len;q++) {
       for (i=0;i<A1;i++) {
          for (j=0;j<A2;j++) {
             for (s=0;s<B2;s++) {
                A[INDEX3(i,j,q,A1,A2)]+=B[INDEX3(i,s,q,A1,B2)]*C[INDEX3(s,j,q,B2,A2)];
             }
          }
       }
    }
}

/*    inverts the set of dim x dim matrices A(:,:,1:len) with dim=1,2,3 */
/*    the determinante is returned. */

void Finley_Util_InvertSmallMat(int len,int dim,double* A,double *invA, double* det){
   int q;
   double D,A11,A12,A13,A21,A22,A23,A31,A32,A33;

   switch(dim) {
      case 1: 
         for (q=0;q<len;q++) {
            D=A[INDEX3(0,0,q,dim,dim)];
            if (D == 0 ){
               Finley_ErrorCode=ZERO_DIVISION_ERROR;
               sprintf(Finley_ErrorMsg,"Non-regular matrix");
               return;
            }
            det[q]=D;
            D=1./D;
            invA[INDEX3(0,0,q,dim,dim)]=D;
         }
         break;

      case 2: 
         for (q=0;q<len;q++) {
            A11=A[INDEX3(0,0,q,dim,dim)];
            A12=A[INDEX3(0,1,q,dim,dim)];
            A21=A[INDEX3(1,0,q,dim,dim)];
            A22=A[INDEX3(1,1,q,dim,dim)];

            D = A11*A22-A12*A21;
            if (D == 0 ){
               Finley_ErrorCode=ZERO_DIVISION_ERROR;
               sprintf(Finley_ErrorMsg,"Non-regular matrix");
               return;
            }
            det[q]=D;
            D=1./D;
            invA[INDEX3(0,0,q,dim,dim)]= A22*D;
            invA[INDEX3(1,0,q,dim,dim)]=-A21*D;
            invA[INDEX3(0,1,q,dim,dim)]=-A12*D;
            invA[INDEX3(1,1,q,dim,dim)]= A11*D;
         }
         break;

      case 3: 
	 for (q=0;q<len;q++) {
            A11=A[INDEX3(0,0,q,dim,dim)];
            A21=A[INDEX3(1,0,q,dim,dim)];
            A31=A[INDEX3(2,0,q,dim,dim)];
            A12=A[INDEX3(0,1,q,dim,dim)];
            A22=A[INDEX3(1,1,q,dim,dim)];
            A32=A[INDEX3(2,1,q,dim,dim)];
            A13=A[INDEX3(0,2,q,dim,dim)];
            A23=A[INDEX3(1,2,q,dim,dim)];
            A33=A[INDEX3(2,2,q,dim,dim)];

            D  =  A11*(A22*A33-A23*A32)+ A12*(A31*A23-A21*A33)+A13*(A21*A32-A31*A22);
            if (D == 0 ){
               Finley_ErrorCode=ZERO_DIVISION_ERROR;
               sprintf(Finley_ErrorMsg,"Non-regular matrix");
               return;
            }
            det[q]    =D;
            D=1./D;
            invA[INDEX3(0,0,q,dim,dim)]=(A22*A33-A23*A32)*D;
            invA[INDEX3(1,0,q,dim,dim)]=(A31*A23-A21*A33)*D;
            invA[INDEX3(2,0,q,dim,dim)]=(A21*A32-A31*A22)*D;
            invA[INDEX3(0,1,q,dim,dim)]=(A13*A32-A12*A33)*D;
            invA[INDEX3(1,1,q,dim,dim)]=(A11*A33-A31*A13)*D;
            invA[INDEX3(2,1,q,dim,dim)]=(A12*A31-A11*A32)*D;
            invA[INDEX3(0,2,q,dim,dim)]=(A12*A23-A13*A22)*D;
            invA[INDEX3(1,2,q,dim,dim)]=(A13*A21-A11*A23)*D;
            invA[INDEX3(2,2,q,dim,dim)]=(A11*A22-A12*A21)*D;
         }
         break;

   }
   return;
}

/*    sets the derterminate of a set of dim x dim matrices A(:,:,1:len) with dim=1,2,3 */

void Finley_Util_DetOfSmallMat(int len,int dim,double* A, double* det){
   int q;
   double A11,A12,A13,A21,A22,A23,A31,A32,A33;

   switch(dim) {
      case 1: 
         for (q=0;q<len;q++) {
            det[q]=A[INDEX3(0,0,q,dim,dim)];
         }
         break;

      case 2: 
         for (q=0;q<len;q++) {
            A11=A[INDEX3(0,0,q,dim,dim)];
            A12=A[INDEX3(0,1,q,dim,dim)];
            A21=A[INDEX3(1,0,q,dim,dim)];
            A22=A[INDEX3(1,1,q,dim,dim)];

            det[q] = A11*A22-A12*A21;
         }
         break;

      case 3: 
	 for (q=0;q<len;q++) {
            A11=A[INDEX3(0,0,q,dim,dim)];
            A21=A[INDEX3(1,0,q,dim,dim)];
            A31=A[INDEX3(2,0,q,dim,dim)];
            A12=A[INDEX3(0,1,q,dim,dim)];
            A22=A[INDEX3(1,1,q,dim,dim)];
            A32=A[INDEX3(2,1,q,dim,dim)];
            A13=A[INDEX3(0,2,q,dim,dim)];
            A23=A[INDEX3(1,2,q,dim,dim)];
            A33=A[INDEX3(2,2,q,dim,dim)];

            det[q]  =  A11*(A22*A33-A23*A32)+ A12*(A31*A23-A21*A33)+A13*(A21*A32-A31*A22);
         }
         break;

   }
   return;
}
/*    returns the normalized vector Normal[dim,len] orthogonal to A(:,0,q) and A(:,1,q) in the case of dim=3  */
/*    or the vector A(:,0,q) in the case of dim=2                                             */

void  Finley_NormalVector(int len, int dim, int dim1, double* A,double* Normal) {
   int q;
   double A11,A12,CO_A13,A21,A22,CO_A23,A31,A32,CO_A33,length,invlength;

   switch(dim) {
      case 1: 
         for (q=0;q<len;q++) Normal[INDEX1(q)]    =1;
         break;
      case 2: 
         for (q=0;q<len;q++) {
            A11=A[INDEX3(0,0,q,dim,dim1)];
            A21=A[INDEX3(1,0,q,dim,dim1)];
            length = sqrt(A11*A11+A21*A21);
            if (! length>0) {
               Finley_ErrorCode=ZERO_DIVISION_ERROR;
               sprintf(Finley_ErrorMsg,"area equals zero.");
               return;
            } else {
               invlength=1./length;
               Normal[INDEX2(0,q,dim)]=A21*invlength;
               Normal[INDEX2(1,q,dim)]=-A11*invlength;
            }
         }
         break;
      case 3: 
         for (q=0;q<len;q++) {
            A11=A[INDEX3(0,0,q,dim,dim1)];
            A21=A[INDEX3(1,0,q,dim,dim1)];
            A31=A[INDEX3(2,0,q,dim,dim1)];
            A12=A[INDEX3(0,1,q,dim,dim1)];
            A22=A[INDEX3(1,1,q,dim,dim1)];
            A32=A[INDEX3(2,1,q,dim,dim1)];
            CO_A13=A21*A32-A31*A22;
            CO_A23=A31*A12-A11*A32;
            CO_A33=A11*A22-A21*A12;
            length=sqrt(CO_A13*CO_A13+CO_A23*CO_A23+CO_A33*CO_A33);
            if (! length>0) {
               Finley_ErrorCode=ZERO_DIVISION_ERROR;
               sprintf(Finley_ErrorMsg,"area equals zero.");
               return;
            } else {
               invlength=1./length;
               Normal[INDEX2(0,q,dim)]=CO_A13*invlength;
               Normal[INDEX2(1,q,dim)]=CO_A23*invlength;
               Normal[INDEX2(2,q,dim)]=CO_A33*invlength;
           }
            
      }
      break;
   
  }
  return;
}

/*    return the length of the vector which is orthogonal to the vectors A(:,0,q) and A(:,1,q) in the case of dim=3 */
/*    or the vector A(:,0,q) in the case of dim=2                                                                   */

void  Finley_LengthOfNormalVector(int len, int dim, int dim1, double* A,double* length) {
   int q;
   double A11,A12,CO_A13,A21,A22,CO_A23,A31,A32,CO_A33;

   switch(dim) {
      case 1: 
         for (q=0;q<len;q++) length[INDEX1(q)]    =1;
         break;
      case 2: 
         for (q=0;q<len;q++) {
            A11=A[INDEX3(0,0,q,dim,dim1)];
            A21=A[INDEX3(1,0,q,dim,dim1)];
            length[q] = sqrt(A11*A11+A21*A21);
         }
         break;
      case 3: 
         for (q=0;q<len;q++) {
            A11=A[INDEX3(0,0,q,dim,dim1)];
            A21=A[INDEX3(1,0,q,dim,dim1)];
            A31=A[INDEX3(2,0,q,dim,dim1)];
            A12=A[INDEX3(0,1,q,dim,dim1)];
            A22=A[INDEX3(1,1,q,dim,dim1)];
            A32=A[INDEX3(2,1,q,dim,dim1)];
            CO_A13=A21*A32-A31*A22;
            CO_A23=A31*A12-A11*A32;
            CO_A33=A11*A22-A21*A12;
            length[q]=sqrt(CO_A13*CO_A13+CO_A23*CO_A23+CO_A33*CO_A33);
      }
      break;
   
  }
  return;
}

/* inverts the map map of length len */
/* there is no range checking! */
/* at output Map[invMap[i]]=i for i=0:lenInvMap */

void Finley_Util_InvertMap(int lenInvMap, maybelong* invMap,int lenMap, maybelong* Map) {
   int i;
   for (i=0;i<lenInvMap;i++) invMap[i]=0;
   for (i=0;i<lenMap;i++) {
      if (Map[i]>=0) invMap[Map[i]]=i;
   }
}

/* orders a Finley_Util_ValueAndIndex array by value */
/* it is assumed that n is large */

int Finley_Util_ValueAndIndex_compar(const void *arg1 , const void *arg2 ) {
   Finley_Util_ValueAndIndex *e1,*e2;
   e1=(Finley_Util_ValueAndIndex*) arg1;
   e2=(Finley_Util_ValueAndIndex*) arg2;
   if (e1->value < e2->value) return -1;
   if (e1->value > e2->value) return  1;
   return 0;
}
void Finley_Util_sortValueAndIndex(int n,Finley_Util_ValueAndIndex* array) {
     /* OMP : needs parallelization !*/
     qsort(array,n,sizeof(Finley_Util_ValueAndIndex),Finley_Util_ValueAndIndex_compar);
}


/**************************************************************/

/* calculates the minimum value from a dim X N integer array */

maybelong Finley_Util_getMinInt(int dim,int N,maybelong* values) {
   maybelong i,j,out;
   out=MAYBELONG_MAX;
   if (values!=NULL && dim*N>0 ) {
     /* OMP */
     out=values[0];
     for (j=0;j<N;j++) {
       for (i=0;i<dim;i++) out=MIN(out,values[INDEX2(i,j,dim)]);
     }
   }
   return out;
}
                                                                                                                                                   
/* calculates the maximum value from a dim X N integer array */

maybelong Finley_Util_getMaxInt(int dim,int N,maybelong* values) {
   maybelong i,j,out;
   out=-MAYBELONG_MAX;
   if (values!=NULL && dim*N>0 ) {
     /* OMP */
     out=values[0];
     for (j=0;j<N;j++) {
       for (i=0;i<dim;i++) out=MAX(out,values[INDEX2(i,j,dim)]);
     }
   }
   return out;
}

/* set the index of the positive entries in mask. The length of index is returned. */

maybelong Finley_Util_packMask(maybelong N,maybelong* mask,maybelong* index) {
      maybelong out,k;
      out=0;
      /*OMP */
      for (k=0;k<N;k++) {
        if (mask[k]>=0) {
          index[out]=k;
          out++;
        }
      }
      return out;
}

/* returns true if array contains value */
int Finley_Util_isAny(maybelong N,maybelong* array,maybelong value) {
   int out=FALSE;
   maybelong i;
   #pragma omp parallel for private(i) schedule(static) reduction(||:out)
   for (i=0;i<N;i++) out=out || (array[i]==value);
   return out;
}

void Finley_copyDouble(int n,double* source, double* target) {
  int i;
  for (i=0;i<n;i++) target[i]=source[i];
}

/*
 * $Log$
 * Revision 1.1  2004/10/26 06:53:57  jgs
 * Initial revision
 *
 * Revision 1.3  2004/08/26 12:03:52  gross
 * Some other bug in Finley_Assemble_gradient fixed.
 *
 * Revision 1.2  2004/07/02 04:21:13  gross
 * Finley C code has been included
 *
 * Revision 1.1.1.1  2004/06/24 04:00:40  johng
 * Initial version of eys using boost-python.
 *
 *
 */
