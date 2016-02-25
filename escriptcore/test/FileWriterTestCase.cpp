
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


#include "FileWriterTestCase.h"
#include "esysUtils/Esys_MPI.h"
#include "escript/FileWriter.h"

#include <cppunit/TestCaller.h>
#include <fstream>
#include <sstream>


using namespace CppUnit;
using namespace std;

using escript::FileWriter;

void FileWriterTestCase::testAll()
{
    const string filename("fwtest_file");
    int mpisize=1, mpirank=0;
    FileWriter* fw;
#ifdef ESYS_MPI
    MPI_Comm_size(MPI_COMM_WORLD, &mpisize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);
    fw = new FileWriter(MPI_COMM_WORLD);
#else
    fw = new FileWriter();
#endif

    const char crank = static_cast<char>(mpirank<128?mpirank:128);
    const char data[] = {crank,crank,crank,crank};
    ostringstream oss;
    oss.write(data, 4);

    cout << endl;
    cout << "\tTest open file." << endl;
    CPPUNIT_ASSERT(fw->openFile(filename) == true);
    cout << "\tTest writeOrdered." << endl;
    CPPUNIT_ASSERT(fw->writeOrdered(oss) == true);
    CPPUNIT_ASSERT(oss.str().length() == 0);
    fw->close();
    CPPUNIT_ASSERT(fileSize(filename) == 4*mpisize);

    cout << "\tTest open file with initial size." << endl;
    CPPUNIT_ASSERT(fw->openFile(filename, 100) == true);
    fw->close();
    CPPUNIT_ASSERT(fileSize(filename) == 100);

    CPPUNIT_ASSERT(fw->openFile(filename) == true);
    oss.write(data, 4);
    cout << "\tTest writeShared." << endl;
    CPPUNIT_ASSERT(fw->writeShared(oss) == true);
    CPPUNIT_ASSERT(oss.str().length() == 0);
    fw->close();
    CPPUNIT_ASSERT(fileSize(filename) == 4*mpisize);

    CPPUNIT_ASSERT(fw->openFile(filename) == true);
    oss.write(data, 4);
    cout << "\tTest writeAt." << endl;
    CPPUNIT_ASSERT(fw->writeAt(oss, 16*(mpirank+1)) == true);
    CPPUNIT_ASSERT(oss.str().length() == 0);
    fw->close();
    CPPUNIT_ASSERT(fileSize(filename) == 16*mpisize+4);
    delete fw;
}

long FileWriterTestCase::fileSize(string filename)
{
    ifstream f(filename.c_str());
    f.seekg(0, f.end);
    long pos = f.tellg();
    f.close();
#ifdef ESYS_MPI
    MPI_Barrier(MPI_COMM_WORLD);
#endif
    return pos;
}

TestSuite* FileWriterTestCase::suite()
{
    TestSuite *testSuite = new TestSuite("FileWriterTestCase");
    testSuite->addTest(new TestCaller<FileWriterTestCase>(
                "testAll",&FileWriterTestCase::testAll));
    return testSuite;
}
