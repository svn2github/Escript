
########################################################
#
# Copyright (c) 2003-2015 by University of Queensland
# Earth Systems Science Computational Center (ESSCC)
# http://www.uq.edu.au
#
# Primary Business: Queensland, Australia
# Licensed under the Open Software License version 3.0
# http://www.opensource.org/licenses/osl-3.0.php
#
########################################################

__copyright__="""Copyright (c) 2003-2015 by University of Queensland
Earth Systems Science Computational Center (ESSCC)
http://www.uq.edu.au
Primary Business: Queensland, Australia"""
__license__="""Licensed under the Open Software License version 3.0
http://www.opensource.org/licenses/osl-3.0.php"""
__url__="https://launchpad.net/escript-finley"

"""
Test suite for nonlinearPDEs on ripley
"""

import esys.escriptcore.utestselect as unittest
from esys.escriptcore.testing import *
from test_nonLinearPDE import Test_nlpde
from esys.escript import getMPISizeWorld
from esys.ripley import Rectangle,Brick


class Test_RipleyNonLinearPDE2D(Test_nlpde):
   def setUp(self):
        self.domain = Rectangle(l0=1.,l1=1., n0=10, n1=10*getMPISizeWorld()-1, d1=getMPISizeWorld()) 
   def tearDown(self):
        del self.domain

class Test_RipleyNonLinearPDE3D(Test_nlpde):
   def setUp(self):
        self.domain = Brick(l0=1.,l1=1.,l2=1., n0=10, n1=10*getMPISizeWorld()-1, n2=10, d1=getMPISizeWorld()) 
   def tearDown(self):
        del self.domain

if __name__ == '__main__':
   run_tests(__name__, exit_on_failure=True)

