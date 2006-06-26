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
                                                                           
#if !defined  escript_FunctionSpaceFactory_20040604_H
#define escript_FunctionSpaceFactory_20040604_H
#include "system_dep.h"

#include "AbstractDomain.h"
#include "FunctionSpace.h"

namespace escript {

  /**
     \brief
     Create function space objects.

     Description:
     Create function space objects.

  */

  /**
     \brief
     Return a continuous FunctionSpace
  */
  ESCRIPT_DLL_API FunctionSpace continuousFunction(const AbstractDomain& domain);

  /**
     \brief
     Return a functon FunctionSpace
  */
  ESCRIPT_DLL_API FunctionSpace function(const AbstractDomain& domain);
  /**
     \brief
     Return a function on boundary FunctionSpace
  */
  ESCRIPT_DLL_API FunctionSpace functionOnBoundary(const AbstractDomain& domain);
  /**
     \brief
     Return a FunctionSpace
  */
  ESCRIPT_DLL_API FunctionSpace functionOnContactZero(const AbstractDomain& domain);
  /**
     \brief
     Return a FunctionSpace
  */
  ESCRIPT_DLL_API FunctionSpace functionOnContactOne(const AbstractDomain& domain);
  /**
     \brief
     Return a FunctionSpace
  */
  ESCRIPT_DLL_API FunctionSpace solution(const AbstractDomain& domain);
  /**
     \brief
     Return a FunctionSpace
  */
  ESCRIPT_DLL_API FunctionSpace reducedSolution(const AbstractDomain& domain);
  /**
     \brief
     Return a FunctionSpace
  */
  ESCRIPT_DLL_API FunctionSpace diracDeltaFunction(const AbstractDomain& domain);

} // end of namespace
#endif
