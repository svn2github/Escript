/* $Id$ */
/* 
 ******************************************************************************
 *                                                                            *
 *       COPYRIGHT  ACcESS 2004 -  All Rights Reserved                        *
 *                                                                            *
 * This software is the property of ACcESS. No part of this code              *
 * may be copied in any form or by any means without the expressed written    *
 * consent of ACcESS.  Copying, use or modification of this software          *
 * by any unauthorised person is illegal unless that person has a software    *
 * license agreement with ACcESS.                                             *
 *                                                                            *
 ******************************************************************************
*/
                                                                           
#if !defined  escript_UtilC_20040611_H
#define escript_UtilC_20040611_H

#define ESCRIPT_MAX_DATA_RANK 4
/**
   \brief
   solver options, they have to be consistent with LinearPDE.py settings 

   Description:
   solver options, they have to be consistent with LinearPDE.py settings 

*/
#define ESCRIPT_UNKNOWN -1
#define ESCRIPT_NO_REORDERING 0
#define ESCRIPT_MINIMUM_FILL_IN 1
#define ESCRIPT_NESTED_DISSECTION 2
#define ESCRIPT_DEFAULT_METHOD 0
#define ESCRIPT_DIRECT 1
#define ESCRIPT_CHOLEVSKY 2
#define ESCRIPT_PCG 3
#define ESCRIPT_CR 4
#define ESCRIPT_CGS 5
#define ESCRIPT_BICGSTAB 6
#define ESCRIPT_SSOR 7
#define ESCRIPT_ILU0 8
#define ESCRIPT_ILUT 9
#define ESCRIPT_JACOBI 10
#define ESCRIPT_GMRES 11
#define ESCRIPT_PRES20 12
#define ESCRIPT_LUMPING 13

/* keyword in solver options relevant for lineraPDE class */
#define ESCRIPT_TOLERANCE_KEY "tolerance"
#define ESCRIPT_METHOD_KEY "method"
#define ESCRIPT_SYMMETRY_KEY "symmetric"

#endif
