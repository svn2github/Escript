
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


#ifndef _ESCRIPTDATASETTESTCASE_H_
#define _ESCRIPTDATASETTESTCASE_H_

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

class EscriptDatasetTestCase : public CppUnit::TestFixture
{
public:
    void testAll();

    static CppUnit::TestSuite* suite();

private:
    void checkVTKfile(std::string filename);
    int getDataArrayLength(std::istream& is);
};

#endif

