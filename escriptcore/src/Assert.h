
/*****************************************************************************
*
* Copyright (c) 2003-2016 by The University of Queensland
* http://www.uq.edu.au
*
* Primary Business: Queensland, Australia
* Licensed under the Open Software License version 3.0
* http://www.opensource.org/licenses/osl-3.0.php
*
* Development until 2012 by Earth Systems Science Computational Center (ESSCC)
* Development 2012-2013 by School of Earth Sciences
* Development from 2014 by Centre for Geoscience Computing (GeoComp)
*
*****************************************************************************/

#ifndef __ESCRIPT_ASSERT_H__
#define __ESCRIPT_ASSERT_H__

/**
   \brief
   EsysAssert is a MACRO that will throw an exception if the boolean
   condition specified is false.

   Description:
   EsysAssert is conditionally compiled into code only when DOASSERT is
   defined.  When DOASSERT is not defined, the EsysAssert statement is
   entirely removed from code.
*/

#if DOASSERT

//
// DOASSERT is defined, replace ESYS_ASSERT with Exception throw
//

#include <escript/EsysException.h>
#include <sstream>

// use this if you don't have a JMPI instance handy
#define ESYS_ASSERT(assert_test, assert_msg)\
    do {\
        const bool result = (assert_test);\
        if (!result) {\
            std::ostringstream message;\
            message << assert_msg << "\n\n"\
            << __FILE__ << ":" << __LINE__ << ": " << #assert_test << "\n";\
            throw escript::AssertException(message.str());\
        }\
    } while (0)

// MPI aware version that throws on all ranks
#define ESYS_ASSERT_MPI(assert_test, assert_msg, __mpi__)\
    do {\
	try\
	{\
	    (void)__mpi__;\
	} catch (...)\
	{\
	    ESYS_ASSERT(assert_test, assert_msg);\
	    break;\
	}\
        const int result = (assert_test) ? 0 : 1;\
        int gresult = result;\
        escript::checkResult(result, gresult, __mpi__);\
        if (gresult > 0) {\
            std::ostringstream message;\
            message << assert_msg << "\n\n"\
            << __FILE__ << ":" << __LINE__ << ": " << #assert_test;\
            if (!result)\
                message << " (on other rank(s))";\
            message << "\n";\
            throw escript::AssertException(message.str());\
        }\
    } while (0)

#else // !DOASSERT

//
// DOASSERT is not defined, replace ESYS_ASSERT macros with no-op
//

#define ESYS_ASSERT(a,b)
#define ESYS_ASSERT_MPI(a,b,c)

#endif

#endif // __ESCRIPT_ASSERT_H__

