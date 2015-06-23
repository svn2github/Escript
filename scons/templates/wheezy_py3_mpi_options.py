
##############################################################################
#
# Copyright (c) 2003-2015 by The University of Queensland
# http://www.uq.edu.au
#
# Primary Business: Queensland, Australia
# Licensed under the Open Software License version 3.0
# http://www.opensource.org/licenses/osl-3.0.php
#
# Development until 2012 by Earth Systems Science Computational Center (ESSCC)
# Development 2012-2013 by School of Earth Sciences
# Development from 2014 by Centre for Geoscience Computing (GeoComp)
#
##############################################################################

from .wheezy_options import *

# boost-python library/libraries to link against
boost_libs = ['boost_python-py32']
usepython3=True
pythoncmd='python3'
pythonlibname='python3.2mu'
pythonincpath='/usr/include/python3.2'

mpi='OPENMPI'