
##############################################################################
#
# Copyright (c) 2003-2016 by The University of Queensland
# http://www.uq.edu.au
#
# Primary Business: Queensland, Australia
# Licensed under the Apache License, version 2.0
# http://www.apache.org/licenses/LICENSE-2.0
#
# Development until 2012 by Earth Systems Science Computational Center (ESSCC)
# Development 2012-2013 by School of Earth Sciences
# Development from 2014 by Centre for Geoscience Computing (GeoComp)
#
##############################################################################

from __future__ import print_function, division

__copyright__="""Copyright (c) 2003-2016 by The University of Queensland
http://www.uq.edu.au
Primary Business: Queensland, Australia"""
__license__="""Licensed under the Apache License, version 2.0
http://www.apache.org/licenses/LICENSE-2.0"""
__url__="https://launchpad.net/escript-finley"

"""
test for non-overloaded binary operations

:remark: use see `test_util`
:var __author__: name of author
:var __copyright__: copyrights
:var __license__: licence agreement
:var __url__: url entry point on documentation
:var __version__: version
:var __date__: date of the version
"""

__author__="Joel Fenwick, joelfenwick@uq.edu.au"

import esys.escriptcore.utestselect as unittest
import numpy
from esys.escript import *
from test_util_base import Test_util_values




        
    

class Test_util_binary_new(Test_util_values):
    
   def generate_indices(self, shape):
        res=[0]*len(shape)
        l=len(shape)
        done=False
        while not done:
            yield tuple(res)
            res[0]+=1
            for i in range(l-1):
                if res[i]>=shape[i]:
                    res[i]=0
                    res[i+1]+=1
                else:
                    break
            # now we check the last digit
            if res[l-1]>=shape[l-1]:
                done=True
        

   def subst_outer(self, a, b):
        if isinstance(a,float) or isinstance(a, complex):
            a=(a,)
        if isinstance(b,float) or isinstance(b, complex):
            b=(b,)            
        sa=getShape(a)
        sb=getShape(b)
        a=numpy.array(a)
        b=numpy.array(b)
        targettype=a.dtype if a.dtype.kind=='c' else b.dtype
        if sa==():
            if sb==():
                return a*b
            resshape=sb
            res=numpy.zeros(resshape, dtype=targettype)            
            for xb in self.generate_indices(sb):
                res.itemset(xb,a*b.item(xb))  
            return res
        elif sb==():
            resshape=sa
            res=numpy.zeros(resshape, dtype=targettype)            
            for xa in self.generate_indices(sa):
                res.itemset(xa,a.item(xa)*b)            
            return res
        else:
            resshape=sa+sb
            res=numpy.zeros(resshape, dtype=targettype)            
        for xa in self.generate_indices(sa):
            for xb in self.generate_indices(sb):
                res.itemset(xa+xb,a.item(xa)*b.item(xb))
        return res    
    
    
   #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   def test_inner_combined(self):
       opstring='inner(a,b)'
       misccheck=None   # How to work out what the result of type should be
       oraclecheck="numpy.tensordot(refa, refb, axes=getRank(refa))"
       opname="inner"
       noshapemismatch=True
       permitscalarmismatch=False
       self.generate_binary_operation_test_batch_large(opstring, misccheck, oraclecheck, opname, no_shape_mismatch=noshapemismatch, permit_scalar_mismatch=permitscalarmismatch)
   #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   def test_outer_combined(self):
       opstring='outer(a,b)'
       misccheck=None   # How to work out what the result of type should be
       oraclecheck="self.subst_outer(refa,refb)"
       opname="outer"
       noshapemismatch=True
       permitscalarmismatch=True
       capcombinedrank=True
       self.generate_binary_operation_test_batch_large(opstring, misccheck, oraclecheck, opname, no_shape_mismatch=noshapemismatch, permit_scalar_mismatch=permitscalarmismatch, cap_combined_rank=capcombinedrank)           
   #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   def test_matrix_mult_combined(self):
       opstring='matrix_mult(a,b)'
       misccheck=None   # How to work out what the result of type should be
       oraclecheck="numpy.dot(refa,refb)"
       opname="matrix_mult"
       aranks=(2,)
       self.generate_binary_matrixlike_operation_test_batch_large(opstring, misccheck, oraclecheck, opname, aranks=aranks)
   #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   def test_transpose_matrix_mult_combined(self):
       opstring='transposed_matrix_mult(a,b)'
       misccheck=None   # How to work out what the result of type should be
       oraclecheck="numpy.dot(numpy.transpose(refa),refb)"
       opname="transposed_matrix_mult"
       aranks=(2,)
       self.generate_binary_matrixlike_operation_test_batch_large(opstring, misccheck, oraclecheck, opname, aranks=aranks)
   #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   def test_matrix_transposed_mult_combined(self):
       opstring='matrix_transposed_mult(a,b)'
       misccheck=None   # How to work out what the result of type should be
       oraclecheck="numpy.dot(refa,numpy.transpose(refb))"
       opname="matrix_transposed_mult"
       aranks=(2,)
       self.generate_binary_matrixlike_operation_test_batch_large(opstring, misccheck, oraclecheck, opname, aranks=aranks)       
   #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   def test_tensor_mult_combined(self):
       opstring='tensor_mult(a,b)'
       misccheck=None   # How to work out what the result of type should be
       oraclecheck="numpy.dot(refa,refb) if getRank(refa)==2 else numpy.tensordot(refa,refb)"
       opname="tensor_mult"
       aranks=(2,4)
       self.generate_binary_matrixlike_operation_test_batch_large(opstring, misccheck, oraclecheck, opname, aranks=aranks)
    #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   def test_transposed_tensor_mult_combined(self):
       opstring='transposed_tensor_mult(a,b)'
       misccheck=None   # How to work out what the result of type should be
       oraclecheck="numpy.dot(transpose(refa),refb) if getRank(refa)==2 else numpy.tensordot(transpose(refa),refb)"
       opname="transposed_tensor_mult"
       aranks=(2,4)
       self.generate_binary_matrixlike_operation_test_batch_large(opstring, misccheck, oraclecheck, opname, aranks=aranks) 
    #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   #def test_tensor_transposed_mult_combined(self):
       #opstring='tensor_transposed_mult(a,b)'
       #misccheck=None   # How to work out what the result of type should be
       #oraclecheck="numpy.dot(refa,transpose(refb)) if getRank(refa)==2 else numpy.tensordot(refa,transpose(refb))"
       #opname="tensor_tranposed_mult"
       #aranks=(2,4)
       #self.generate_binary_matrixlike_operation_test_batch_large(opstring, misccheck, oraclecheck, opname, aranks=aranks)         