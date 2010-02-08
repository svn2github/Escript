
/*******************************************************
*
* Copyright (c) 2003-2010 by University of Queensland
* Earth Systems Science Computational Center (ESSCC)
* http://www.uq.edu.au/esscc
*
* Primary Business: Queensland, Australia
* Licensed under the Open Software License version 3.0
* http://www.opensource.org/licenses/osl-3.0.php
*
*******************************************************/


/**************************************************************/

/* Paso: FluxControl                                          */

/**************************************************************/

/* Author: l.gross@uq.edu.au                                */

/**************************************************************/


#include "Paso.h"
#include "SolverFCT.h"
#include "PasoUtil.h"


/**************************************************************/

/* create the low order transport matrix and stores it negative values 
 * into the iteration_matrix accept the main diagonal which is stored seperately
 * if fc->iteration_matrix==NULL, fc->iteration_matrix is allocated 
 *
 * a=transport_matrix 
 * b= low_order_transport_matrix = - iteration_matrix
 * c=main diagonal low_order_transport_matrix 
 * initialize c[i] mit a[i,i] 
 *    d_ij=max(0,-a[i,j],-a[j,i])  
 *    b[i,j]=-(a[i,j]+d_ij)         
 *    c[i]-=d_ij                  
 */

void Paso_FCTransportProblem_setLowOrderOperator(Paso_FCTransportProblem * fc) {
  dim_t n=Paso_SystemMatrix_getTotalNumRows(fc->transport_matrix),i;
  index_t iptr_ij,j,iptr_ji;
  Paso_SystemMatrixPattern *pattern;
  register double d_ij, sum, rtmp1, rtmp2;

  if (fc==NULL) return;
  if (fc->iteration_matrix==NULL) {
      fc->iteration_matrix=Paso_SystemMatrix_alloc(fc->transport_matrix->type,
                                                   fc->transport_matrix->pattern,
                                                   fc->transport_matrix->row_block_size,
                                                   fc->transport_matrix->col_block_size, TRUE);
  }

  if (Paso_noError()) {
      pattern=fc->iteration_matrix->pattern;
      n=Paso_SystemMatrix_getTotalNumRows(fc->iteration_matrix);
      #pragma omp parallel for private(i,sum,iptr_ij,j,iptr_ji,rtmp1, rtmp2,d_ij)  schedule(static)
      for (i = 0; i < n; ++i) {
          sum=fc->transport_matrix->mainBlock->val[fc->main_iptr[i]];
          /* look at a[i,j] */
          for (iptr_ij=pattern->mainPattern->ptr[i];iptr_ij<pattern->mainPattern->ptr[i+1]; ++iptr_ij) {
              j=pattern->mainPattern->index[iptr_ij];
              if (j!=i) {
                 /* find entry a[j,i] */
                 #pragma ivdep
                 for (iptr_ji=pattern->mainPattern->ptr[j]; iptr_ji<pattern->mainPattern->ptr[j+1]; ++iptr_ji) {
                    if ( pattern->mainPattern->index[iptr_ji] == i) {
                        rtmp1=fc->transport_matrix->mainBlock->val[iptr_ij];
                        rtmp2=fc->transport_matrix->mainBlock->val[iptr_ji];
                        d_ij=-MIN3(0.,rtmp1,rtmp2);
                        fc->iteration_matrix->mainBlock->val[iptr_ij]=-(rtmp1+d_ij);
                        sum-=d_ij;
                        break;
                    }
                 }
             }
          }
          for (iptr_ij=pattern->col_couplePattern->ptr[i];iptr_ij<pattern->col_couplePattern->ptr[i+1]; ++iptr_ij) {
              j=pattern->col_couplePattern->index[iptr_ij];
              /* find entry a[j,i] */
              #pragma ivdep
              for (iptr_ji=pattern->row_couplePattern->ptr[j]; iptr_ji<pattern->row_couplePattern->ptr[j+1]; ++iptr_ji) {
                    if (pattern->row_couplePattern->index[iptr_ji]==i) {
                        rtmp1=fc->transport_matrix->col_coupleBlock->val[iptr_ij];
                        rtmp2=fc->transport_matrix->row_coupleBlock->val[iptr_ji];
                        d_ij=-MIN3(0.,rtmp1,rtmp2);
                        fc->iteration_matrix->col_coupleBlock->val[iptr_ij]=-(rtmp1+d_ij);
                        fc->iteration_matrix->row_coupleBlock->val[iptr_ji]=-(rtmp2+d_ij);
                        sum-=d_ij;
                        break;
                    }
              }
         }
         /* set main diagonal entry */
         fc->main_diagonal_low_order_transport_matrix[i]=sum;
      }
  }
}
/*
 * out_i=m_i u_i + a * \sum_{j <> i} l_{ij} (u_j-u_i) + b * q_i
 *
 */
void Paso_SolverFCT_setMuPaLuPbQ(double* out,
                                 const double* M, 
                                 const Paso_Coupler* u_coupler,
                                 const double a,
                                 const Paso_SystemMatrix *L,
                                 const double b,
                                 const double* Q) 
{
  dim_t n, i, j;
  Paso_SystemMatrixPattern *pattern;
  const double *u=Paso_Coupler_borrowLocalData(u_coupler);
  const double *remote_u=Paso_Coupler_borrowRemoteData(u_coupler);
  register double sum, u_i, l_ij;
  register index_t iptr_ij;
printf("Paso_SolverFCT_setMuPaLuPbQ a,b = %e %e\n",a,b);
  n=Paso_SystemMatrix_getTotalNumRows(L);

  #pragma omp parallel for private(i) schedule(static)
  for (i = 0; i < n; ++i) {
      out[i]=M[i]*u[i]+b*Q[i];
  } 
  if (ABS(a)>0) {
      pattern=L->pattern;
      #pragma omp parallel for schedule(static) private(i, sum, u_i, iptr_ij, j, l_ij)
      for (i = 0; i < n; ++i) {
          sum=0;
          u_i=u[i];
          #pragma ivdep
  	  for (iptr_ij=(pattern->mainPattern->ptr[i]);iptr_ij<pattern->mainPattern->ptr[i+1]; ++iptr_ij) {
               j=pattern->mainPattern->index[iptr_ij];
               l_ij=L->mainBlock->val[iptr_ij];
               sum+=l_ij*(u[j]-u_i);
          }
          #pragma ivdep
  	  for (iptr_ij=(pattern->col_couplePattern->ptr[i]);iptr_ij<pattern->col_couplePattern->ptr[i+1]; ++iptr_ij) {
               j=pattern->col_couplePattern->index[iptr_ij];
               l_ij=L->col_coupleBlock->val[iptr_ij];
               sum+=l_ij*(remote_u[j]-u_i);
          }
          out[i]+=a*sum;
      }
  }
}
/* 
 * calculate QP[i] max_{i in L->pattern[i]} (u[j]-u[i] ) 
 *           QN[i] min_{i in L->pattern[i]} (u[j]-u[i] ) 
 *
*/
void Paso_SolverFCT_setQs(const Paso_Coupler* u_coupler,double* QN, double* QP, const Paso_SystemMatrix *L) 
{
  dim_t n, i, j;
  Paso_SystemMatrixPattern *pattern;
  const double *u=Paso_Coupler_borrowLocalData(u_coupler);
  const double *remote_u=Paso_Coupler_borrowRemoteData(u_coupler);
  register double u_i, u_min_i, u_max_i, u_j;
  register index_t iptr_ij;
  n=Paso_SystemMatrix_getTotalNumRows(L);
  pattern=L->pattern;
  #pragma omp parallel for schedule(static) private(i, u_i, u_min_i, u_max_i, j, u_j, iptr_ij)
  for (i = 0; i < n; ++i) {
     u_i=u[i];
     u_min_i=u_i;
     u_max_i=u_i;
     #pragma ivdep
     for (iptr_ij=(pattern->mainPattern->ptr[i]);iptr_ij<pattern->mainPattern->ptr[i+1]; ++iptr_ij) {
         j=pattern->mainPattern->index[iptr_ij];
         u_j=u[j];
         u_min_i=MIN(u_min_i,u_j);
         u_max_i=MAX(u_max_i,u_j);
     }
     #pragma ivdep
     for (iptr_ij=(pattern->col_couplePattern->ptr[i]);iptr_ij<pattern->col_couplePattern->ptr[i+1]; ++iptr_ij) {
          j=pattern->col_couplePattern->index[iptr_ij];
          u_j=remote_u[j];
          u_min_i=MIN(u_min_i,u_j);
          u_max_i=MAX(u_max_i,u_j);
      }
      QN[i]=u_min_i-u_i;
      QP[i]=u_max_i-u_i;
  }
}

/*
 *
 *  f_{ij} = (m_{ij} - dt (1-theta) d_{ij}) (u_last[j]-u_last[i]) - (m_{ij} + dt theta d_{ij}) (u[j]-u[i])
 *         
 * m=fc->mass matrix
 * d=artifical diffusion matrix = L - K = - fc->iteration matrix - fc->transport matrix (away from main diagonal)
 */
void Paso_FCTransportProblem_setAntiDiffusionFlux(const double dt, const Paso_FCTransportProblem * fc, Paso_SystemMatrix *flux_matrix, const Paso_Coupler* u_coupler)
{
  dim_t n, j, i;
  index_t iptr_ij;
  const double *u=Paso_Coupler_borrowLocalData(u_coupler);
  const double *u_last= Paso_Coupler_borrowLocalData(fc->u_coupler);
  const double *remote_u=Paso_Coupler_borrowRemoteData(u_coupler);
  const double *remote_u_last=Paso_Coupler_borrowRemoteData(fc->u_coupler);
  const double f1=- dt * (1.-fc->theta);
  const double f2=  dt * fc->theta;
  register double m_ij, d_ij, u_i, u_last_i, d_u_last, d_u;
  const Paso_SystemMatrixPattern *pattern=fc->iteration_matrix->pattern;
printf("Paso_FCTransportProblem_setAntiDiffusionFlux f1,f1 = %e %e\n",f1,f2);
  n=Paso_SystemMatrix_getTotalNumRows(fc->iteration_matrix);
  if ( (ABS(f1) >0 ) ) {
     if ( (ABS(f2) >0 ) ) {
        #pragma omp parallel for schedule(static) private(i, u_i, u_last_i, iptr_ij, j,m_ij,d_ij, d_u_last, d_u)
        for (i = 0; i < n; ++i) {
           u_i=u[i];
           u_last_i=u_last[i];
           #pragma ivdep
           for (iptr_ij=(pattern->mainPattern->ptr[i]);iptr_ij<pattern->mainPattern->ptr[i+1]; ++iptr_ij) {
              j=pattern->mainPattern->index[iptr_ij];
              m_ij=fc->mass_matrix->mainBlock->val[iptr_ij];
              d_ij=-(fc->transport_matrix->mainBlock->val[iptr_ij]+
                                                fc->iteration_matrix->mainBlock->val[iptr_ij]);
              d_u=u[j]-u_i;
              d_u_last=u_last[j]-u_last_i;
              flux_matrix->mainBlock->val[iptr_ij]=(m_ij+f1*d_ij)*d_u_last- (m_ij+f2*d_ij)*d_u;
           }
           #pragma ivdep
           for (iptr_ij=(pattern->col_couplePattern->ptr[i]);iptr_ij<pattern->col_couplePattern->ptr[i+1]; ++iptr_ij) {
              j=pattern->col_couplePattern->index[iptr_ij];
              m_ij=fc->mass_matrix->col_coupleBlock->val[iptr_ij];
              d_ij=-(fc->transport_matrix->col_coupleBlock->val[iptr_ij]+
                                             fc->iteration_matrix->col_coupleBlock->val[iptr_ij]);
              d_u=remote_u[j]-u_i;
              d_u_last=remote_u_last[j]-u_last_i;
              flux_matrix->col_coupleBlock->val[iptr_ij]=(m_ij+f1*d_ij)*d_u_last- (m_ij+f2*d_ij)*d_u;
           }
        }
     } else {
        #pragma omp parallel for schedule(static) private(i, u_i, u_last_i, iptr_ij, j,m_ij,d_ij, d_u_last, d_u)
        for (i = 0; i < n; ++i) {
           u_i=u[i];
           u_last_i=u_last[i];
           #pragma ivdep
           for (iptr_ij=(pattern->mainPattern->ptr[i]);iptr_ij<pattern->mainPattern->ptr[i+1]; ++iptr_ij) {
              j=pattern->mainPattern->index[iptr_ij];
              m_ij=fc->mass_matrix->mainBlock->val[iptr_ij];
              d_ij=-(fc->transport_matrix->mainBlock->val[iptr_ij]+
                                                fc->iteration_matrix->mainBlock->val[iptr_ij]);
              d_u=u[j]-u_i;
              d_u_last=u_last[j]-u_last_i;
              flux_matrix->mainBlock->val[iptr_ij]=(m_ij+f1*d_ij)*d_u_last-m_ij*d_u;
           }
           #pragma ivdep
           for (iptr_ij=(pattern->col_couplePattern->ptr[i]);iptr_ij<pattern->col_couplePattern->ptr[i+1]; ++iptr_ij) {
              j=pattern->col_couplePattern->index[iptr_ij];
              m_ij=fc->mass_matrix->col_coupleBlock->val[iptr_ij];
              d_ij=-(fc->transport_matrix->col_coupleBlock->val[iptr_ij]+
                                             fc->iteration_matrix->col_coupleBlock->val[iptr_ij]);
              d_u=remote_u[j]-u_i;
              d_u_last=remote_u_last[j]-u_last_i;
              flux_matrix->col_coupleBlock->val[iptr_ij]=(m_ij+f1*d_ij)*d_u_last-m_ij*d_u;
           }
        }
     }
  } else {
     if ( (ABS(f2) >0 ) ) {
        #pragma omp parallel for schedule(static) private(i, u_i, u_last_i, iptr_ij, j,m_ij,d_ij, d_u_last, d_u)
        for (i = 0; i < n; ++i) {
           u_i=u[i];
           u_last_i=u_last[i];
           #pragma ivdep
           for (iptr_ij=(pattern->mainPattern->ptr[i]);iptr_ij<pattern->mainPattern->ptr[i+1]; ++iptr_ij) {
              j=pattern->mainPattern->index[iptr_ij];
              m_ij=fc->mass_matrix->mainBlock->val[iptr_ij];
              d_ij=-(fc->transport_matrix->mainBlock->val[iptr_ij]+
                                                fc->iteration_matrix->mainBlock->val[iptr_ij]);
              d_u=u[j]-u_i;
              d_u_last=u_last[j]-u_last_i;
              flux_matrix->mainBlock->val[iptr_ij]=m_ij*d_u_last- (m_ij+f2*d_ij)*d_u;
           }
           #pragma ivdep
           for (iptr_ij=(pattern->col_couplePattern->ptr[i]);iptr_ij<pattern->col_couplePattern->ptr[i+1]; ++iptr_ij) {
              j=pattern->col_couplePattern->index[iptr_ij];
              m_ij=fc->mass_matrix->col_coupleBlock->val[iptr_ij];
              d_ij=-(fc->transport_matrix->col_coupleBlock->val[iptr_ij]+
                                             fc->iteration_matrix->col_coupleBlock->val[iptr_ij]);
              d_u=remote_u[j]-u_i;
              d_u_last=remote_u_last[j]-u_last_i;
              flux_matrix->col_coupleBlock->val[iptr_ij]=m_ij*d_u_last- (m_ij+f2*d_ij)*d_u;
           }
        }
     } else {
        #pragma omp parallel for schedule(static) private(i, u_i, u_last_i, iptr_ij, j,m_ij,d_ij, d_u_last, d_u)
        for (i = 0; i < n; ++i) {
           u_i=u[i];
           u_last_i=u_last[i];
           #pragma ivdep
           for (iptr_ij=(pattern->mainPattern->ptr[i]);iptr_ij<pattern->mainPattern->ptr[i+1]; ++iptr_ij) {
              j=pattern->mainPattern->index[iptr_ij];
              m_ij=fc->mass_matrix->mainBlock->val[iptr_ij];
              d_ij=-(fc->transport_matrix->mainBlock->val[iptr_ij]+
                                                fc->iteration_matrix->mainBlock->val[iptr_ij]);
              d_u=u[j]-u_i;
              d_u_last=u_last[j]-u_last_i;
              flux_matrix->mainBlock->val[iptr_ij]=m_ij*(d_u_last-d_u);
           }
           #pragma ivdep
           for (iptr_ij=(pattern->col_couplePattern->ptr[i]);iptr_ij<pattern->col_couplePattern->ptr[i+1]; ++iptr_ij) {
              j=pattern->col_couplePattern->index[iptr_ij];
              m_ij=fc->mass_matrix->col_coupleBlock->val[iptr_ij];
              d_ij=-(fc->transport_matrix->col_coupleBlock->val[iptr_ij]+
                                             fc->iteration_matrix->col_coupleBlock->val[iptr_ij]);
              d_u=remote_u[j]-u_i;
              d_u_last=remote_u_last[j]-u_last_i;
              flux_matrix->col_coupleBlock->val[iptr_ij]=m_ij*(d_u_last-d_u);
           }
        }
      }
  }
}
/*
 *  apply pre flux-correction: f_{ij}:=0 if f_{ij}*(u_[i]-u[j])<=0
 *  
 */
void Paso_FCTransportProblem_applyPreAntiDiffusionCorrection(Paso_SystemMatrix *f,const Paso_Coupler* u_coupler)
{
  dim_t n, i, j;
  Paso_SystemMatrixPattern *pattern;
  const double *u=Paso_Coupler_borrowLocalData(u_coupler);
  const double *remote_u=Paso_Coupler_borrowRemoteData(u_coupler);
  register double u_i, f_ij;
  register index_t iptr_ij;

  n=Paso_SystemMatrix_getTotalNumRows(f);
  pattern=f->pattern;
  #pragma omp parallel for schedule(static) private(i, u_i, iptr_ij, j, f_ij)
  for (i = 0; i < n; ++i) {
      u_i=u[i];
      #pragma ivdep
      for (iptr_ij=(pattern->mainPattern->ptr[i]);iptr_ij<pattern->mainPattern->ptr[i+1]; ++iptr_ij) {
          j=pattern->mainPattern->index[iptr_ij];
          f_ij=f->mainBlock->val[iptr_ij];
          if (f_ij * (u_i-u[j]) <= 0) f->mainBlock->val[iptr_ij]=0;
      }
      #pragma ivdep
      for (iptr_ij=(pattern->col_couplePattern->ptr[i]);iptr_ij<pattern->col_couplePattern->ptr[i+1]; ++iptr_ij) {
          j=pattern->col_couplePattern->index[iptr_ij];
          f_ij=f->col_coupleBlock->val[iptr_ij];
          if (f_ij * (u_i-remote_u[j]) <= 0) f->col_coupleBlock->val[iptr_ij]=0;
      }
  }
}


void Paso_FCTransportProblem_setRs(const Paso_SystemMatrix *f,const double* lumped_mass_matrix,
                                   const Paso_Coupler* QN_coupler,const Paso_Coupler* QP_coupler,double* RN,double* RP)
{
  dim_t n, i, j;
  Paso_SystemMatrixPattern *pattern;
  const double *QN=Paso_Coupler_borrowLocalData(QN_coupler);
  const double *QP=Paso_Coupler_borrowLocalData(QP_coupler);
  register double f_ij, PP_i, PN_i;
  register index_t iptr_ij;

  n=Paso_SystemMatrix_getTotalNumRows(f);
  pattern=f->pattern;
  #pragma omp parallel for schedule(static) private(i, iptr_ij, j, f_ij, PP_i, PN_i)
  for (i = 0; i < n; ++i) {
      PP_i=0.;
      PN_i=0.;
      #pragma ivdep
      for (iptr_ij=(pattern->mainPattern->ptr[i]);iptr_ij<pattern->mainPattern->ptr[i+1]; ++iptr_ij) {
          j=pattern->mainPattern->index[iptr_ij];
          if (i != j ) {
             f_ij=f->mainBlock->val[iptr_ij];
             if (f_ij <=0) {
                PN_i+=f_ij;
             } else {
                PP_i+=f_ij;
             }
          }
      }
      #pragma ivdep
      for (iptr_ij=(pattern->col_couplePattern->ptr[i]);iptr_ij<pattern->col_couplePattern->ptr[i+1]; ++iptr_ij) {
          j=pattern->col_couplePattern->index[iptr_ij];
          f_ij=f->col_coupleBlock->val[iptr_ij];
          if (f_ij <=0  ) {
                PN_i+=f_ij;
          } else {
                PP_i+=f_ij;
          }
      }
      if (PN_i<0) {
         RN[i]=MIN(1,QN[i]*lumped_mass_matrix[i]/PN_i);
      } else {
         RN[i]=1.;
      }
      if (PP_i>0) {
         RP[i]=MIN(1,QP[i]*lumped_mass_matrix[i]/PP_i);
      } else {
         RP[i]=1.;
      }
  }

}

void Paso_FCTransportProblem_addCorrectedFluxes(double* f,const Paso_SystemMatrix *flux_matrix, const Paso_Coupler* RN_coupler, const Paso_Coupler* RP_coupler) 
{
  dim_t n, i, j;
  Paso_SystemMatrixPattern *pattern;
  register double RN_i, RP_i, f_i, f_ij;
  double alpha_i;
  register index_t iptr_ij;
  const double *RN=Paso_Coupler_borrowLocalData(RN_coupler);
  const double *remote_RN=Paso_Coupler_borrowRemoteData(RN_coupler);
  const double *RP=Paso_Coupler_borrowLocalData(RP_coupler);
  const double *remote_RP=Paso_Coupler_borrowRemoteData(RP_coupler);
  n=Paso_SystemMatrix_getTotalNumRows(flux_matrix);

  pattern=flux_matrix->pattern; 
  #pragma omp parallel for schedule(static) private(i, RN_i, RP_i, iptr_ij, j, f_ij, f_i)
  for (i = 0; i < n; ++i) {
    
     alpha_i=0;
     RN_i=RN[i];
     RP_i=RP[i];
     f_i=0;
     #pragma ivdep
     for (iptr_ij=(pattern->mainPattern->ptr[i]);iptr_ij<pattern->mainPattern->ptr[i+1]; ++iptr_ij) {
         j=pattern->mainPattern->index[iptr_ij];
         f_ij=flux_matrix->mainBlock->val[iptr_ij];
         if (f_ij >=0) {
              f_i+=f_ij*MIN(RP_i,RN[j]);
printf("alpha %d %d = %e %e\n",i,j,MIN(RP_i,RN[j]),f_ij);	      
alpha_i=MAX(alpha_i,MIN(RP_i,RN[j]) );
         } else {
              f_i+=f_ij*MIN(RN_i,RP[j]);
printf("alpha %d %d = %e %e\n",i,j,MIN(RN_i,RP[j]),f_ij);	
alpha_i=MAX(alpha_i,MIN(RN_i,RP[j]) );
	 }
     }
     #pragma ivdep
     for (iptr_ij=(pattern->col_couplePattern->ptr[i]);iptr_ij<pattern->col_couplePattern->ptr[i+1]; ++iptr_ij) {
          j=pattern->col_couplePattern->index[iptr_ij];
          f_ij=flux_matrix->col_coupleBlock->val[iptr_ij];
          if (f_ij >=0) {
              f_i+=f_ij*MIN(RP_i,remote_RN[j]);
          }else {
              f_i+=f_ij*MIN(RN_i,remote_RP[j]);
          }
      }
printf("alpha %d = %e -> %e (%e, %e)\n",i,alpha_i, f_i, f[i],f[i]+f_i);
      f[i]+=f_i;
  }
}
