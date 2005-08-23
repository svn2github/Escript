/* $Id$ */

/**************************************************************/

/* Finley: SystemMatrix: controls iterative linear system solvers  */ 

/**************************************************************/

/* Copyrights by ACcESS Australia 2003/04 */
/* Author: gross@access.edu.au */

/**************************************************************/

#include "Finley.h"
#include "System.h"
#include "Solver.h"
#include "Common.h"
#if PTR_OFFSET !=0 || INDEX_OFFSET!=0
#error Finley library usage requires PTR_OFFSET=0 and INDEX_OFFSET=0
#endif


/***********************************************************************************/

/*  free space */

void Finley_Solver_free(Finley_SystemMatrix* A) {
    Finley_Preconditioner_free(A->iterative);
    A->iterative=NULL;
}
/*  call the iterative solver: */

void Finley_Solver(Finley_SystemMatrix* A,double* x,double* b,Finley_SolverOptions* options) {
    double norm2_of_b,tol,tolerance,time_iter,time_prec,*r=NULL,norm2_of_residual,last_norm2_of_residual,norm_max_of_b,
           norm2_of_b_local,norm_max_of_b_local,norm2_of_residual_local,norm_max_of_residual_local,norm_max_of_residual,
           last_norm_max_of_residual,*scaling;
    dim_t i,totIter,cntIter,method;
    bool_t finalizeIteration;
    err_t errorCode;
    dim_t n_col = A->num_cols * A-> col_block_size;
    dim_t n_row = A->num_rows * A-> row_block_size;

    tolerance=MAX(options->tolerance,EPSILON);
    Finley_ErrorCode=NO_ERROR;
    /* check matrix type */
    if (A->type!=CSR) {
      Finley_ErrorCode = TYPE_ERROR;
      sprintf(Finley_ErrorMsg, "Iterative solver requires CSR format.");
      return;
    }
    if (A->col_block_size != A->row_block_size) {
       Finley_ErrorCode = TYPE_ERROR;
       sprintf(Finley_ErrorMsg, "preconditioner requires row and column block sizes to be equal.");
       return;
     }
     if (A->num_cols != A->num_rows) {
       Finley_ErrorCode = TYPE_ERROR;
       sprintf(Finley_ErrorMsg, "This method requires a square matrix.");
       return;
     }
     /* select iterative method */
     switch (options->method) {
        default:
            if (options->symmetric) {
               method=ESCRIPT_PCG;
            } else {
               method=ESCRIPT_BICGSTAB;
            }
            break;
        case ESCRIPT_BICGSTAB:
            method=ESCRIPT_BICGSTAB;
            break;
        case ESCRIPT_PCG:
            method=ESCRIPT_PCG;
            break;
        case ESCRIPT_PRES20:
            method=ESCRIPT_PRES20;
            break;
        case ESCRIPT_GMRES:
            method=ESCRIPT_GMRES;
            break;
     }
    /* get normalization */
    scaling=Finley_SystemMatrix_borrowNormalization(A);
    if (Finley_ErrorCode!=NO_ERROR) return;
    /* get the norm of the right hand side */
    norm2_of_b=0.;
    norm_max_of_b=0.;
    #pragma omp parallel private(norm2_of_b_local,norm_max_of_b_local) 
    { 
            norm2_of_b_local=0.;
            norm_max_of_b_local=0.;
            #pragma omp for private(i) schedule(static)
            for (i = 0; i < n_row ; ++i) {
                  norm2_of_b_local += b[i] * b[i];
                  norm_max_of_b_local = MAX(ABS(scaling[i]*b[i]),norm_max_of_b_local);
            }
            #pragma omp critical
            {
               norm2_of_b += norm2_of_b_local;
               norm_max_of_b = MAX(norm_max_of_b_local,norm_max_of_b);
            }
    }
    norm2_of_b=sqrt(norm2_of_b);

    /* if norm2_of_b==0 we are ready: x=0 */
    if (norm2_of_b <=0.) {
        #pragma omp parallel for private(i) schedule(static)
        for (i = 0; i < n_col; i++) x[i]=0.;
        if (options->verbose) printf("right hand side is identical zero.\n");
        return;
    }
    if (options->verbose) {
        printf("l2/lmax-norm of right hand side is  %e/%e.\n",norm2_of_b,norm_max_of_b);
        printf("l2/lmax-stopping criterion is  %e/%e.\n",norm2_of_b*tolerance,norm_max_of_b*tolerance);
        switch (method) {
           case ESCRIPT_BICGSTAB:
               printf("Iterative method is BiCGStab.\n");
               break;
           case ESCRIPT_PCG:
               printf("Iterative method is PCG.\n");
               break;
           case ESCRIPT_PRES20:
               printf("Iterative method is PRES20.\n");
               break;
           case ESCRIPT_GMRES:
               if (options->restart>0) {
                  printf("Iterative method is GMRES(%d,%d)\n",options->truncation,options->restart);
               } else {
                  printf("Iterative method is GMRES(%d)\n",options->truncation);
               }
               break;
        }
    }
    /* construct the preconditioner */
    
    time_prec=Finley_timer();
    Finley_Solver_setPreconditioner(A,options);
    time_prec=Finley_timer()-time_prec;
    if (Finley_ErrorCode!=NO_ERROR) return;

    /* get an initial guess by evaluating the preconditioner */
    time_iter=Finley_timer();
    #pragma omp parallel
    Finley_Solver_solvePreconditioner(A,x,b);
    if (Finley_ErrorCode!=NO_ERROR) return;
    /* start the iteration process :*/
    r=TMPMEMALLOC(n_row,double);
    if (Finley_checkPtr(r)) return;
    totIter = 0;
    finalizeIteration = FALSE;
    last_norm2_of_residual=norm2_of_b;
    last_norm_max_of_residual=norm_max_of_b;
    while (! finalizeIteration) {
       cntIter = options->iter_max - totIter;
       finalizeIteration = TRUE;
       /*     Set initial residual. */
       norm2_of_residual = 0;
       norm_max_of_residual = 0;
       #pragma omp parallel private(norm2_of_residual_local,norm_max_of_residual_local)
       {
          #pragma omp for private(i) schedule(static)
          for (i = 0; i < n_row; i++) r[i]=b[i];

          Finley_RawScaledSystemMatrixVector(DBLE(-1), A, x, DBLE(1), r);

          norm2_of_residual_local = 0;
          norm_max_of_residual_local = 0;
          #pragma omp for private(i) schedule(static)
          for (i = 0; i < n_row; i++) {
                 norm2_of_residual_local+= r[i] * r[i];
                 norm_max_of_residual_local=MAX(ABS(scaling[i]*r[i]),norm_max_of_residual_local);
          }
          #pragma omp critical
          {
             norm2_of_residual += norm2_of_residual_local;
             norm_max_of_residual = MAX(norm_max_of_residual_local,norm_max_of_residual);
          }
       }
       norm2_of_residual =sqrt(norm2_of_residual);

       if (options->verbose) printf("Step %5d: l2/lmax-norm of residual is  %e/%e",totIter,norm2_of_residual,norm_max_of_residual);
       if (totIter>0 && norm2_of_residual>=last_norm2_of_residual &&  norm_max_of_residual>=last_norm_max_of_residual) {
            if (options->verbose) printf(" divergence!\n");
            Finley_ErrorCode=WARNING;
	    sprintf(Finley_ErrorMsg, "No improvement during iteration.\nIterative solver gives up.");
       } else {
          /* */
          if (norm2_of_residual>tolerance*norm2_of_b || norm_max_of_residual>tolerance*norm_max_of_b ) {
              tol=tolerance*MIN(norm2_of_b,0.1*norm2_of_residual/norm_max_of_residual*norm_max_of_b);
             if (options->verbose) printf(" (new tolerance = %e).\n",tol);
             last_norm2_of_residual=norm2_of_residual;
             last_norm_max_of_residual=norm_max_of_residual;
             /* call the solver */
             switch (method) {
                    case ESCRIPT_BICGSTAB:
                        errorCode = Finley_Solver_BiCGStab(A, r, x, &cntIter, &tol); 
                        break;
                    case ESCRIPT_PCG:
                        errorCode = Finley_Solver_PCG(A, r, x, &cntIter, &tol); 
                        break;
                    case ESCRIPT_PRES20:
                        errorCode = Finley_Solver_GMRES(A, r, x, &cntIter, &tol,5,20); 
                        break;
                    case ESCRIPT_GMRES:
                        errorCode = Finley_Solver_GMRES(A, r, x, &cntIter, &tol,options->truncation,options->restart); 
                        break;
              }
              totIter += cntIter;
         
              /* error handling  */
              if (errorCode==NO_ERROR) {
                    finalizeIteration = FALSE;
      	      } else if (errorCode==SOLVER_MAXITER_REACHED) {
                    Finley_ErrorCode=WARNING;
                    sprintf(Finley_ErrorMsg,"maximum number of iteration step reached.\nReturned solution does not fulfil stopping criterion.");
              } else if (errorCode == SOLVER_INPUT_ERROR ) {
                    Finley_ErrorCode=SYSTEM_ERROR;
         	   sprintf(Finley_ErrorMsg,"illegal dimension in iterative solver.");
              } else if ( errorCode == SOLVER_BREAKDOWN ) {
         	  if (cntIter <= 1) {
                       Finley_ErrorCode =ZERO_DIVISION_ERROR;
         	      sprintf(Finley_ErrorMsg, "fatal break down in iterative solver.");
                  } else {
         	      if (options->verbose) printf("Breakdown at iter %d (residual = %e). Restarting ...\n", cntIter+totIter, tol);
                      finalizeIteration = FALSE;
                  }
              } else if (errorCode == SOLVER_MEMORY_ERROR) {
         	      Finley_ErrorCode=MEMORY_ERROR;
         	      sprintf(Finley_ErrorMsg,"memory allocation failed.");
              } else if (errorCode !=SOLVER_NO_ERROR ) {
         	      Finley_ErrorCode=SYSTEM_ERROR;
         	      sprintf(Finley_ErrorMsg,"unidentified error in iterative solver.");
              }
         } else {
            if (options->verbose) printf(". convergence! \n");
         }
      }
    } /* while */
    MEMFREE(r);
    time_iter=Finley_timer()-time_iter;
    if (options->verbose)  {
       printf("timing: solver: %.4e sec\n",time_prec+time_iter);
       printf("timing: preconditioner: %.4e sec\n",time_prec);
       if (totIter>0) printf("timing: per iteration step: %.4e sec\n",time_iter/totIter);
    }
}

/*
* $Log$
* Revision 1.8  2005/08/23 01:24:30  jgs
* Merge of development branch dev-02 back to main trunk on 2005-08-23
*
* Revision 1.7.2.1  2005/08/19 02:44:09  gross
* stopping criterion modified to cope with badly balanced equations
*
* Revision 1.7  2005/07/08 04:08:00  jgs
* Merge of development branch back to main trunk on 2005-07-08
*
* Revision 1.6  2005/05/06 04:26:15  jgs
* Merge of development branch back to main trunk on 2005-05-06
*
* Revision 1.1.1.1.2.7  2005/06/29 02:34:59  gross
* some changes towards 64 integers in finley
*
* Revision 1.1.1.1.2.6  2005/04/14 06:53:11  gross
* a problem with header files fixed: bug appeared  for case -O0 -g in the icc compiler only
*
* Revision 1.1.1.1.2.5  2005/03/02 23:35:07  gross
* reimplementation of the ILU in Finley. block size>1 still needs some testing
*
* Revision 1.1.1.1.2.4  2004/12/07 10:12:06  gross
* GMRES added
*
* Revision 1.1.1.1.2.3  2004/11/24 01:37:17  gross
* some changes dealing with the integer overflow in memory allocation. Finley solves 4M unknowns now
*
* Revision 1.1.1.1.2.2  2004/11/15 06:05:27  gross
* poisson solver added
*
* Revision 1.1.1.1.2.1  2004/11/12 06:58:20  gross
* a lot of changes to get the linearPDE class running: most important change is that there is no matrix format exposed to the user anymore. the format is chosen by the Domain according to the solver and symmetry
*
* Revision 1.1.1.1  2004/10/26 06:53:57  jgs
* initial import of project esys2
*
* Revision 1.1  2004/07/02 04:21:14  gross
* Finley C code has been included
*
*
*/
