#!/bin/bash

# $Id$
# An explicit building script for esys

#**********************************************************************
# NB: this script is setup to perform default developmental builds
# change any settings you require, but PLEASE do not check any changes
# to this script back into the repository - talk to Jon first
#**********************************************************************

# test commit

# the modules to make
MODULES="tools/CppUnitTest tools/mmio esysUtils escript paso finley bruce modellib"

# the modules to test
TESTS="escript bruce finley"

# the python tests to run
#FINLEY_PYTESTS="ImportTest.py SimpleSolve.py GradTest.py test_linearPDEsOnFinley.py test_generators.py test_visualization_interface.py"
FINLEY_PYTESTS="ImportTest.py SimpleSolve.py GradTest.py"
ESCRIPT_PYTESTS="ImportTest.py BinaryOps.py UnaryOps.py SliceGetting.py SliceSetting.py MiscTests.py newEscriptTests.py DataVariableTests.py test_xml.py"
BRUCE_PYTESTS="ImportTest.py BruceTest.py"

# use this setting for local developmental builds
ENVVARS="DODEBUG=YES"

if [ "$2" == "DEBUG" ]
then
    ENVVARS="DODEBUG=YES"
fi

if [ "$2" == "NODEBUG" ]
then
    ENVVARS="DODEBUG=NO"
fi

# use this setting for production installs on altix
# ** NB: overwrites centrally installed esys libraries on altix ***
#ENVVARS="$ENVVARS L_INSTLIB_DIR=/raid2/tools/esys/lib L_PYTH_DIR=/raid2/tools/esys/esys"

# ensure required symlinks are setup first
(cd esysUtils/inc; if [ ! -h esysUtils ]; then ln -s ../src esysUtils; fi)
(cd escript/inc; if [ ! -h escript ]; then ln -s ../src escript; fi)
(cd finley/inc; if [ ! -h finley ]; then ln -s ../src finley; fi)
(cd paso/inc; if [ ! -h paso ]; then ln -s ../src paso; fi)
(cd bruce/inc; if [ ! -h bruce ]; then ln -s ../src bruce; fi)

(cd tools/mmio; if [ ! -h Makefile ]; then ln -s ./mmio.mk Makefile; fi)
(cd tools/CppUnitTest; if [ ! -h Makefile ]; then ln -s ./CppUnitTest.mk Makefile; fi)
(cd esysUtils; if [ ! -h Makefile ]; then ln -s ./esysUtils.mk Makefile; fi)
(cd escript; if [ ! -h Makefile ]; then ln -s ./escript.mk Makefile; fi)
(cd finley; if [ ! -h Makefile ]; then ln -s ./finley.mk Makefile; fi)
(cd paso; if [ ! -h Makefile ]; then ln -s ./paso.mk Makefile; fi)
(cd bruce; if [ ! -h Makefile ]; then ln -s ./bruce.mk Makefile; fi)
(cd modellib; if [ ! -h Makefile ]; then ln -s ./modellib.mk Makefile; fi)

# clean all modules
if [ "$1" == "clean" ]
then
	for module in $MODULES ; do
		(echo Cleaning: $module; cd $module; make clean)
	done
        exit 0
fi

# uninstall all modules
if [ "$1" == "uninstall" ]
then
	for module in $MODULES ; do
		(echo Cleaning: $module; cd $module; make clean)
	done
	for module in $MODULES ; do
		(echo Uninstalling: $module; cd $module; make uninstall)
	done
        exit 0
fi

# run the unit tests
if [ "$1" == "unit_test" ]
then
    for module in $TESTS
    do
	echo "Running unit tests for: $module"
	cd $ESYS_ROOT/$module/test
	./unit_test
        if [ $? != 0 ]
        then
          echo Unit Testing FAILED for module: $module
          exit 1
        fi
    done
    exit 0
fi

# run the python tests
if [ "$1" == "py_test" ]
then
    for module in $TESTS
    do
	echo "Running python tests for: $module"
	cd $ESYS_ROOT/$module/test/python
	if [ "$module" == "finley" ]
	then
            for pytest in $FINLEY_PYTESTS
            do 
	        echo "Running python test: $pytest"
                python $pytest
                if [ $? != 0 ]
                then
                  echo Python Testing FAILED for $pytest in module $module
                  exit 1
                fi
            done 
	elif [ "$module" == "escript" ]
	then
            for pytest in $ESCRIPT_PYTESTS
            do 
	        echo "Running python test: $pytest"
                python $pytest
                if [ $? != 0 ]
                then
                  echo Python Testing FAILED for $pytest in module $module
                  exit 1
                fi
            done
	elif [ "$module" == "bruce" ]
	then
            for pytest in $BRUCE_PYTESTS
            do 
	        echo "Running python test: $pytest"
                python $pytest
                if [ $? != 0 ]
                then
                  echo Python Testing FAILED for $pytest in module $module
                  exit 1
                fi
            done
	fi
    done
    exit 0
fi

# default case - just build all modules
echo "*************** build ************************************"
for module in $MODULES ; do
	(echo Building: $module; cd $module; env $ENVVARS make -j 10)
        if [ $? != 0 ]
        then
            echo Build FAILED for module: $module
            exit 1
        fi
done

# install all modules
if [ "$1" == "install" ]
then
        echo "***************installation ************************************"
	for module in $MODULES ; do \
                echo ENVVARS = $ENVVARS
		(echo Installing: $module; cd $module; env $ENVVARS make install)
                if [ $? != 0 ]
                then
                  echo Installation FAILED for module: $module
                  exit 1
                fi
	done
        (cd esys; touch __init__.py)
        (cd esys/escript; if [ ! -h escriptcpp.so ]; then ln -s ../../lib/libescriptcpp.so escriptcpp.so; fi)
        (cd esys/finley; if [ ! -h finleycpp.so ]; then ln -s ../../lib/libfinleycpp.so finleycpp.so; fi)
        (cd esys/bruce; if [ ! -h brucecpp.so ]; then ln -s ../../lib/libbrucecpp.so brucecpp.so; fi)
        exit 0
fi
