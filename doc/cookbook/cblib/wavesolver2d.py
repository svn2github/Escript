
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

# You can shorten the execution time by reducing variable tend from 60 to 0.5

# Importing all the necessary modules required.
from esys.escript import *
from esys.escript.pdetools import Locator
from esys.escript.linearPDEs import LinearPDE
from esys.finley import Rectangle
from numarray import identity,zeros,ones
import os

def wavesolver2d(domain,h,tend,lam,mu,rho,U0,xc,savepath):
   x=domain.getX()
   # ... open new PDE ...
   mypde=LinearPDE(domain)
   mypde.setSolverMethod(LinearPDE.LUMPING)
   kmat = kronecker(mypde.getDim())

   # define small radius around point xc
   # Lsup(x) returns the maximum value of the argument x
   src_radius = 0.1*Lsup(domain.getSize())
   print "src_radius = ",src_radius

   dunit=numarray.array([1.,0.]) # defines direction of point source

   mypde.setValue(D=kmat*rho)
   # ... set initial values ....
   n=0
   # initial value of displacement at point source is constant (U0=0.01)
   # for first two time steps
   u=U0*whereNegative(length(x-xc)-src_radius)*dunit
   u_m1=U0*whereNegative(length(x-xc)-src_radius)*dunit
   t=0

   # define the location of the point source 
   L=Locator(domain,numarray.array(xc))
   # find potential at point source
   u_pc=L.getValue(u)
   print "u at point charge=",u_pc
  
   u_pc_x = u_pc[0]
   u_pc_y = u_pc[1]

   # open file to save displacement at point source
   u_pc_data=open(os.path.join(savepath,'U_pc.out'),'w')
   u_pc_data.write("%f %f %f\n"%(t,u_pc_x,u_pc_y))
 
   while t<tend:
     # ... get current stress ....
     g=grad(u)
     stress=lam*trace(g)*kmat+mu*(g+transpose(g))
     # ... get new acceleration ....
     mypde.setValue(X=-stress)          
     a=mypde.getSolution()
     # ... get new displacement ...
     u_p1=2*u-u_last+h**2*a
     # ... shift displacements ....
     u_m1=u
     u=u_p1
     t+=h
     n+=1
     print n,"-th time step t ",t
     u_pc=L.getValue(u)
     print "u at point charge=",u_pc
     
     u_pc_x=u_pc[0]
     u_pc_y=u_pc[1]
           
     # save displacements at point source to file for t > 0
     u_pc_data.write("%f %f %f\n"%(t,u_pc_x,u_pc_y))
 
     # ... save current acceleration in units of gravity and displacements 
     if n==1 or n%10==0: saveVTK(os.path.join(savepath,"usoln.%i.vtu"%(n/10)),acceleration=length(a)/9.81,
     displacement = length(u), tensor = stress, Ux = u[0] )

   u_pc_data.close()