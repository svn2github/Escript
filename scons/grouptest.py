########################################################
#
# Copyright (c) 2003-2008 by University of Queensland
# Earth Systems Science Computational Center (ESSCC)
# http://www.uq.edu.au/esscc
#
# Primary Business: Queensland, Australia
# Licensed under the Open Software License version 3.0
# http://www.opensource.org/licenses/osl-3.0.php
#
########################################################

__copyright__="""Copyright (c) 2003-2008 by University of Queensland
Earth Systems Science Computational Center (ESSCC)
http://www.uq.edu.au/esscc
Primary Business: Queensland, Australia"""
__license__="""Licensed under the Open Software License version 3.0
http://www.opensource.org/licenses/osl-3.0.php"""
__url__="https://launchpad.net/escript-finley"



class GroupTest:
    def __init__(self, exec_cmd, evars, python_dir, working_dir, test_list):
	self.python_dir=python_dir
	self.working_dir=working_dir
	self.test_list=test_list
	self.exec_cmd=exec_cmd
	self.evars=evars
	self.mkdirs=[]
	
    def makeDir(self,dirname):
    	self.mkdirs.append(dirname)

    def makeHeader(build_platform):
	res="#!/bin/bash\n"
	res=res+"\n#############################################\n"
	res=res+"# This file is autogenerated by scons.\n"
	res=res+"# It will be regenerated each time scons is run\n"
	res=res+"#############################################\n\n"
	res=res+"function failed()\n{\n  echo ""Execution failed for $@""\n  exit 1\n}\n"
	res=res+"\nexport LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH\n"
	if build_platform=='darwin':
		res=res+"export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH\n"
	res=res+"\nexport OLD_PYTHON=`pwd`:$PYTHONPATH\nBINRUNNER=$1\nPYTHONRUNNER=\"$1 $2\"\nBATCH_ROOT=`pwd`\n"
	res=res+"BUILD_DIR=$BATCH_ROOT/build/"+build_platform
	res=res+"\nif [ ! -d $BUILD_DIR ]\nthen\n echo Can not find build directory $BUILD_DIR\n exit 2\nfi\n" 
	res=res+"if [ $# -lt 2 ]\nthen\n echo Usage: $0 bin_run_cmd python_run_cmd\n exit 2\nfi\n"
	return res
    makeHeader=staticmethod(makeHeader)

    def makeString(self):
	res=""
	for d in self.mkdirs:
	    res=res+"if [ ! -d "+str(d)+" ]\nthen\n  mkdir "+d+"\nfi\n"
	for v in self.evars:
	    res=res+"\nexport "+str(v[0])+"="+str(v[1])
	res=res+"\nexport PYTHONPATH="+self.python_dir+":$OLD_PYTHON"+"\n"+"cd "+self.working_dir+"\n"
	for t in self.test_list:
	    res=res+"echo Starting "+t+"\n"
	    res=res+self.exec_cmd+' '+t+' || failed '+t+'\n'
	    res=res+"echo Completed "+t+"\n"
	res=res+"\n"
	return res
	
