
##############################################################################
#
# Copyright (c) 2003-2016 by The University of Queensland
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

from __future__ import print_function, division

__copyright__="""Copyright (c) 2003-2016 by The University of Queensland
http://www.uq.edu.au
Primary Business: Queensland, Australia"""
__license__="""Licensed under the Open Software License version 3.0
http://www.opensource.org/licenses/osl-3.0.php"""
__url__="https://launchpad.net/escript-finley"
__author__="Cihan Altinay"

"""
Test suite for the escript.symbolic module

:var __author__: name of author
:var __copyright__: copyrights
:var __license__: licence agreement
:var __url__: url entry point on documentation
:var __version__: version
:var __date__: date of the version
"""

from esys.escript import *
import esys.escriptcore.utestselect as unittest

class Test_symfuncs(unittest.TestCase):

    #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    def test_div_Symbol(self):
        dim=self.domain.getDim()
        fs1=Function(self.domain)
        fs2=ContinuousFunction(self.domain)
        shape=(dim,)
        x=Symbol('x', shape, dim=dim)
        fs=Symbol('fs')
        y=div(x, fs)
        self.assertTrue(isinstance(y, Symbol), "wrong type of result")
        s=fs2.getX()
        xx=Data(0.,shape,fs2)
        if dim==2:
            xx[(0,)]=+(0.663021)*s[0]**2+(-0.724469)*s[0]+(-0.975927)*s[1]**2+(0.646376)*s[1]
            xx[(1,)]=+(0.333051)*s[0]**2+(0.229280)*s[0]+(-0.951178)*s[1]**2+(-0.911498)*s[1]
            
        else:
            xx[(0,)]=+(-0.478992)*s[0]**2+(0.300973)*s[0]+(-0.309206)*s[1]**2+(0.297671)*s[1]+(0.891206)*s[2]**2+(-0.802093)*s[2]
            xx[(1,)]=+(0.899372)*s[0]**2+(-0.281210)*s[0]+(-0.977563)*s[1]**2+(0.089437)*s[1]+(-0.800010)*s[2]**2+(0.052023)*s[2]
            xx[(2,)]=+(0.181119)*s[0]**2+(-0.461672)*s[0]+(0.595383)*s[1]**2+(-0.090931)*s[1]+(0.125996)*s[2]**2+(-0.332668)*s[2]
            
        ref=div(xx, where=fs1)
        res=Evaluator(y)(x=xx, fs=fs1)
        self.assertAlmostEqual(Lsup(res-ref), 0.0, int(-log10(self.RES_TOL)), "wrong result")
    #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    def test_grad_Symbol_rank0(self):
        dim=self.domain.getDim()
        fs1=Function(self.domain)
        fs2=ContinuousFunction(self.domain)
        shape=()
        x=Symbol('x', shape, dim=dim)
        fs=Symbol('fs')
        y=grad(x, fs)
        self.assertTrue(isinstance(y, Symbol), "wrong type of result")
        s=fs2.getX()
        xx=Data(0.,shape,fs2)
        if dim==2:
            xx=+(0.170566)*s[0]**2+(-0.250728)*s[0]+(-0.705946)*s[1]**2+(0.402498)*s[1]
        else:
            xx=+(-0.398378)*s[0]**2+(-0.717321)*s[0]+(-0.662303)*s[1]**2+(0.408467)*s[1]+(0.142644)*s[2]**2+(-0.814835)*s[2]
        ref=grad(xx, where=fs1)
        res=Evaluator(y)(x=xx, fs=fs1)
        self.assertAlmostEqual(Lsup(res-ref), 0.0, int(-log10(self.RES_TOL)), "wrong result")
    #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    def test_grad_Symbol_rank1(self):
        dim=self.domain.getDim()
        fs1=Function(self.domain)
        fs2=ContinuousFunction(self.domain)
        shape=(2,)
        x=Symbol('x', shape, dim=dim)
        fs=Symbol('fs')
        y=grad(x, fs)
        self.assertTrue(isinstance(y, Symbol), "wrong type of result")
        s=fs2.getX()
        xx=Data(0.,shape,fs2)
        if dim==2:
            xx[(0,)]=+(0.727562)*s[0]**2+(-0.223445)*s[0]+(0.018906)*s[1]**2+(0.689077)*s[1]
            xx[(1,)]=+(-0.464393)*s[0]**2+(-0.047998)*s[0]+(-0.970268)*s[1]**2+(0.483966)*s[1]
            
        else:
            xx[(0,)]=+(0.592999)*s[0]**2+(0.057924)*s[0]+(0.689542)*s[1]**2+(0.884258)*s[1]+(0.226557)*s[2]**2+(0.173093)*s[2]
            xx[(1,)]=+(0.460428)*s[0]**2+(0.305114)*s[0]+(0.527003)*s[1]**2+(-0.642439)*s[1]+(-0.604718)*s[2]**2+(-0.942555)*s[2]
            
        ref=grad(xx, where=fs1)
        res=Evaluator(y)(x=xx, fs=fs1)
        self.assertAlmostEqual(Lsup(res-ref), 0.0, int(-log10(self.RES_TOL)), "wrong result")
    #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    def test_grad_Symbol_rank2(self):
        dim=self.domain.getDim()
        fs1=Function(self.domain)
        fs2=ContinuousFunction(self.domain)
        shape=(2, 6)
        x=Symbol('x', shape, dim=dim)
        fs=Symbol('fs')
        y=grad(x, fs)
        self.assertTrue(isinstance(y, Symbol), "wrong type of result")
        s=fs2.getX()
        xx=Data(0.,shape,fs2)
        if dim==2:
            xx[(0, 0)]=+(0.052440)*s[0]**2+(0.898045)*s[0]+(0.040750)*s[1]**2+(0.419164)*s[1]
            xx[(0, 1)]=+(-0.669825)*s[0]**2+(-0.936252)*s[0]+(0.198975)*s[1]**2+(-0.680455)*s[1]
            xx[(0, 2)]=+(-0.481038)*s[0]**2+(-0.786752)*s[0]+(0.042477)*s[1]**2+(0.230989)*s[1]
            xx[(0, 3)]=+(-0.719574)*s[0]**2+(-0.922763)*s[0]+(0.973905)*s[1]**2+(0.085487)*s[1]
            xx[(0, 4)]=+(0.248226)*s[0]**2+(0.949782)*s[0]+(0.898397)*s[1]**2+(0.062416)*s[1]
            xx[(0, 5)]=+(0.266146)*s[0]**2+(-0.745953)*s[0]+(0.986623)*s[1]**2+(-0.442604)*s[1]
            xx[(1, 0)]=+(0.946222)*s[0]**2+(0.144762)*s[0]+(0.766638)*s[1]**2+(0.485169)*s[1]
            xx[(1, 1)]=+(-0.973735)*s[0]**2+(-0.155894)*s[0]+(-0.883030)*s[1]**2+(-0.720274)*s[1]
            xx[(1, 2)]=+(-0.390356)*s[0]**2+(0.023122)*s[0]+(0.699018)*s[1]**2+(0.536678)*s[1]
            xx[(1, 3)]=+(-0.541868)*s[0]**2+(-0.564254)*s[0]+(-0.516833)*s[1]**2+(-0.894096)*s[1]
            xx[(1, 4)]=+(-0.777034)*s[0]**2+(-0.767801)*s[0]+(-0.176781)*s[1]**2+(0.949135)*s[1]
            xx[(1, 5)]=+(-0.908060)*s[0]**2+(0.506150)*s[0]+(-0.792458)*s[1]**2+(-0.814881)*s[1]
            
        else:
            xx[(0, 0)]=+(0.974761)*s[0]**2+(-0.630540)*s[0]+(-0.442703)*s[1]**2+(-0.630429)*s[1]+(-0.661346)*s[2]**2+(-0.613678)*s[2]
            xx[(0, 1)]=+(-0.473760)*s[0]**2+(-0.753152)*s[0]+(0.168100)*s[1]**2+(0.652806)*s[1]+(-0.039955)*s[2]**2+(-0.595013)*s[2]
            xx[(0, 2)]=+(-0.701450)*s[0]**2+(-0.534639)*s[0]+(-0.637424)*s[1]**2+(-0.376760)*s[1]+(-0.186218)*s[2]**2+(-0.164180)*s[2]
            xx[(0, 3)]=+(0.881699)*s[0]**2+(0.310808)*s[0]+(0.875821)*s[1]**2+(0.569882)*s[1]+(0.885259)*s[2]**2+(0.680284)*s[2]
            xx[(0, 4)]=+(0.187561)*s[0]**2+(0.059442)*s[0]+(-0.728815)*s[1]**2+(0.678741)*s[1]+(-0.305605)*s[2]**2+(0.389898)*s[2]
            xx[(0, 5)]=+(-0.227597)*s[0]**2+(-0.708058)*s[0]+(0.641916)*s[1]**2+(-0.434920)*s[1]+(-0.667407)*s[2]**2+(0.676709)*s[2]
            xx[(1, 0)]=+(-0.937718)*s[0]**2+(0.437670)*s[0]+(-0.640098)*s[1]**2+(0.656226)*s[1]+(-0.018295)*s[2]**2+(-0.983343)*s[2]
            xx[(1, 1)]=+(-0.464532)*s[0]**2+(-0.810388)*s[0]+(-0.271672)*s[1]**2+(0.573887)*s[1]+(0.969285)*s[2]**2+(-0.391581)*s[2]
            xx[(1, 2)]=+(0.075339)*s[0]**2+(0.726071)*s[0]+(-0.297982)*s[1]**2+(-0.735526)*s[1]+(-0.901850)*s[2]**2+(0.812571)*s[2]
            xx[(1, 3)]=+(-0.741720)*s[0]**2+(0.645189)*s[0]+(0.620888)*s[1]**2+(0.830944)*s[1]+(0.333009)*s[2]**2+(-0.722998)*s[2]
            xx[(1, 4)]=+(-0.635480)*s[0]**2+(-0.058960)*s[0]+(-0.989416)*s[1]**2+(-0.359599)*s[1]+(0.715715)*s[2]**2+(-0.343371)*s[2]
            xx[(1, 5)]=+(-0.018615)*s[0]**2+(0.255265)*s[0]+(0.439089)*s[1]**2+(-0.012307)*s[1]+(-0.147746)*s[2]**2+(0.055444)*s[2]
            
        ref=grad(xx, where=fs1)
        res=Evaluator(y)(x=xx, fs=fs1)
        self.assertAlmostEqual(Lsup(res-ref), 0.0, int(-log10(self.RES_TOL)), "wrong result")
    #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    def test_grad_Symbol_rank3(self):
        dim=self.domain.getDim()
        fs1=Function(self.domain)
        fs2=ContinuousFunction(self.domain)
        shape=(3, 3, 6)
        x=Symbol('x', shape, dim=dim)
        fs=Symbol('fs')
        y=grad(x, fs)
        self.assertTrue(isinstance(y, Symbol), "wrong type of result")
        s=fs2.getX()
        xx=Data(0.,shape,fs2)
        if dim==2:
            xx[(0, 0, 0)]=+(-0.131306)*s[0]**2+(0.193509)*s[0]+(-0.715154)*s[1]**2+(-0.468018)*s[1]
            xx[(0, 0, 1)]=+(-0.499801)*s[0]**2+(0.274067)*s[0]+(0.543808)*s[1]**2+(0.309240)*s[1]
            xx[(0, 0, 2)]=+(-0.614983)*s[0]**2+(-0.463479)*s[0]+(0.260369)*s[1]**2+(0.530782)*s[1]
            xx[(0, 0, 3)]=+(-0.780240)*s[0]**2+(-0.914474)*s[0]+(0.948173)*s[1]**2+(0.283119)*s[1]
            xx[(0, 0, 4)]=+(-0.925816)*s[0]**2+(-0.325217)*s[0]+(0.533528)*s[1]**2+(0.150112)*s[1]
            xx[(0, 0, 5)]=+(-0.003243)*s[0]**2+(0.271783)*s[0]+(0.742907)*s[1]**2+(-0.342349)*s[1]
            xx[(0, 1, 0)]=+(-0.581245)*s[0]**2+(0.250662)*s[0]+(-0.066008)*s[1]**2+(0.088922)*s[1]
            xx[(0, 1, 1)]=+(0.469981)*s[0]**2+(-0.713990)*s[0]+(0.329869)*s[1]**2+(-0.623012)*s[1]
            xx[(0, 1, 2)]=+(-0.685071)*s[0]**2+(-0.878608)*s[0]+(-0.229174)*s[1]**2+(-0.492144)*s[1]
            xx[(0, 1, 3)]=+(0.824712)*s[0]**2+(-0.204288)*s[0]+(-0.398843)*s[1]**2+(0.281886)*s[1]
            xx[(0, 1, 4)]=+(0.734346)*s[0]**2+(-0.939761)*s[0]+(-0.257289)*s[1]**2+(-0.510665)*s[1]
            xx[(0, 1, 5)]=+(0.969568)*s[0]**2+(-0.137204)*s[0]+(0.370873)*s[1]**2+(-0.983128)*s[1]
            xx[(0, 2, 0)]=+(-0.607279)*s[0]**2+(0.269975)*s[0]+(-0.869572)*s[1]**2+(-0.053430)*s[1]
            xx[(0, 2, 1)]=+(-0.062744)*s[0]**2+(0.866781)*s[0]+(-0.484676)*s[1]**2+(0.961567)*s[1]
            xx[(0, 2, 2)]=+(0.925242)*s[0]**2+(0.872371)*s[0]+(0.646798)*s[1]**2+(-0.231629)*s[1]
            xx[(0, 2, 3)]=+(0.469564)*s[0]**2+(-0.789278)*s[0]+(-0.723906)*s[1]**2+(-0.422659)*s[1]
            xx[(0, 2, 4)]=+(-0.912357)*s[0]**2+(0.198317)*s[0]+(0.579135)*s[1]**2+(-0.413623)*s[1]
            xx[(0, 2, 5)]=+(-0.232737)*s[0]**2+(0.351387)*s[0]+(0.660669)*s[1]**2+(-0.887246)*s[1]
            xx[(1, 0, 0)]=+(0.847455)*s[0]**2+(-0.925545)*s[0]+(0.472343)*s[1]**2+(-0.458792)*s[1]
            xx[(1, 0, 1)]=+(-0.936636)*s[0]**2+(0.744796)*s[0]+(-0.945311)*s[1]**2+(0.374904)*s[1]
            xx[(1, 0, 2)]=+(-0.901141)*s[0]**2+(0.333934)*s[0]+(0.785233)*s[1]**2+(-0.562526)*s[1]
            xx[(1, 0, 3)]=+(0.328287)*s[0]**2+(0.239731)*s[0]+(-0.970495)*s[1]**2+(-0.265293)*s[1]
            xx[(1, 0, 4)]=+(-0.727271)*s[0]**2+(0.569062)*s[0]+(-0.732065)*s[1]**2+(0.942952)*s[1]
            xx[(1, 0, 5)]=+(-0.060875)*s[0]**2+(-0.155882)*s[0]+(-0.128273)*s[1]**2+(-0.522998)*s[1]
            xx[(1, 1, 0)]=+(-0.740145)*s[0]**2+(-0.111054)*s[0]+(-0.591994)*s[1]**2+(-0.469706)*s[1]
            xx[(1, 1, 1)]=+(0.913894)*s[0]**2+(-0.725244)*s[0]+(0.604411)*s[1]**2+(0.428773)*s[1]
            xx[(1, 1, 2)]=+(-0.793160)*s[0]**2+(-0.442320)*s[0]+(0.368839)*s[1]**2+(0.031337)*s[1]
            xx[(1, 1, 3)]=+(0.013624)*s[0]**2+(0.544540)*s[0]+(-0.087344)*s[1]**2+(0.532561)*s[1]
            xx[(1, 1, 4)]=+(0.575649)*s[0]**2+(-0.868709)*s[0]+(0.321269)*s[1]**2+(-0.832955)*s[1]
            xx[(1, 1, 5)]=+(-0.531271)*s[0]**2+(0.725350)*s[0]+(-0.311404)*s[1]**2+(-0.526259)*s[1]
            xx[(1, 2, 0)]=+(-0.074181)*s[0]**2+(-0.639047)*s[0]+(0.788703)*s[1]**2+(-0.260827)*s[1]
            xx[(1, 2, 1)]=+(0.146911)*s[0]**2+(-0.335965)*s[0]+(-0.739471)*s[1]**2+(0.077494)*s[1]
            xx[(1, 2, 2)]=+(-0.928942)*s[0]**2+(0.106240)*s[0]+(-0.504550)*s[1]**2+(0.242622)*s[1]
            xx[(1, 2, 3)]=+(0.948619)*s[0]**2+(0.862789)*s[0]+(-0.820616)*s[1]**2+(-0.280606)*s[1]
            xx[(1, 2, 4)]=+(-0.926391)*s[0]**2+(-0.834282)*s[0]+(-0.703446)*s[1]**2+(-0.514532)*s[1]
            xx[(1, 2, 5)]=+(0.279064)*s[0]**2+(0.011553)*s[0]+(0.922860)*s[1]**2+(0.228062)*s[1]
            xx[(2, 0, 0)]=+(0.300787)*s[0]**2+(0.231481)*s[0]+(0.848631)*s[1]**2+(0.735588)*s[1]
            xx[(2, 0, 1)]=+(0.768186)*s[0]**2+(0.951823)*s[0]+(-0.418715)*s[1]**2+(-0.624949)*s[1]
            xx[(2, 0, 2)]=+(-0.981363)*s[0]**2+(0.824529)*s[0]+(-0.176847)*s[1]**2+(0.104908)*s[1]
            xx[(2, 0, 3)]=+(-0.618894)*s[0]**2+(-0.518853)*s[0]+(0.703343)*s[1]**2+(0.507626)*s[1]
            xx[(2, 0, 4)]=+(0.160781)*s[0]**2+(-0.296444)*s[0]+(-0.007447)*s[1]**2+(0.392096)*s[1]
            xx[(2, 0, 5)]=+(-0.320045)*s[0]**2+(0.779129)*s[0]+(0.139773)*s[1]**2+(-0.514935)*s[1]
            xx[(2, 1, 0)]=+(-0.761804)*s[0]**2+(0.317824)*s[0]+(-0.216747)*s[1]**2+(-0.331750)*s[1]
            xx[(2, 1, 1)]=+(-0.496301)*s[0]**2+(-0.819377)*s[0]+(-0.259145)*s[1]**2+(0.424337)*s[1]
            xx[(2, 1, 2)]=+(-0.310959)*s[0]**2+(0.075773)*s[0]+(-0.328161)*s[1]**2+(0.486975)*s[1]
            xx[(2, 1, 3)]=+(-0.108863)*s[0]**2+(-0.775165)*s[0]+(0.683862)*s[1]**2+(-0.248680)*s[1]
            xx[(2, 1, 4)]=+(0.242647)*s[0]**2+(-0.812868)*s[0]+(0.488338)*s[1]**2+(0.509810)*s[1]
            xx[(2, 1, 5)]=+(0.947138)*s[0]**2+(0.923807)*s[0]+(0.124437)*s[1]**2+(0.815738)*s[1]
            xx[(2, 2, 0)]=+(-0.720217)*s[0]**2+(-0.410322)*s[0]+(0.382788)*s[1]**2+(-0.430965)*s[1]
            xx[(2, 2, 1)]=+(-0.584795)*s[0]**2+(0.494645)*s[0]+(0.333404)*s[1]**2+(0.883566)*s[1]
            xx[(2, 2, 2)]=+(0.156715)*s[0]**2+(-0.365987)*s[0]+(0.680883)*s[1]**2+(0.831206)*s[1]
            xx[(2, 2, 3)]=+(0.425704)*s[0]**2+(0.566009)*s[0]+(0.901976)*s[1]**2+(0.023032)*s[1]
            xx[(2, 2, 4)]=+(-0.625073)*s[0]**2+(0.681198)*s[0]+(0.983091)*s[1]**2+(0.008421)*s[1]
            xx[(2, 2, 5)]=+(0.595630)*s[0]**2+(-0.819234)*s[0]+(0.773660)*s[1]**2+(-0.602997)*s[1]
            
        else:
            xx[(0, 0, 0)]=+(0.600929)*s[0]**2+(0.596097)*s[0]+(0.294255)*s[1]**2+(0.694586)*s[1]+(-0.061635)*s[2]**2+(-0.076752)*s[2]
            xx[(0, 0, 1)]=+(-0.813016)*s[0]**2+(-0.403450)*s[0]+(-0.868023)*s[1]**2+(0.543489)*s[1]+(-0.256316)*s[2]**2+(-0.239538)*s[2]
            xx[(0, 0, 2)]=+(0.411690)*s[0]**2+(-0.603613)*s[0]+(0.882078)*s[1]**2+(-0.667095)*s[1]+(0.209444)*s[2]**2+(-0.939867)*s[2]
            xx[(0, 0, 3)]=+(-0.423188)*s[0]**2+(0.829195)*s[0]+(-0.793141)*s[1]**2+(-0.307226)*s[1]+(-0.610004)*s[2]**2+(-0.149111)*s[2]
            xx[(0, 0, 4)]=+(-0.222817)*s[0]**2+(0.969934)*s[0]+(-0.998089)*s[1]**2+(-0.602576)*s[1]+(0.479911)*s[2]**2+(-0.755028)*s[2]
            xx[(0, 0, 5)]=+(0.476307)*s[0]**2+(-0.788837)*s[0]+(0.858733)*s[1]**2+(-0.043290)*s[1]+(-0.581684)*s[2]**2+(-0.083946)*s[2]
            xx[(0, 1, 0)]=+(0.803944)*s[0]**2+(-0.633907)*s[0]+(-0.979558)*s[1]**2+(0.964925)*s[1]+(0.102937)*s[2]**2+(0.419015)*s[2]
            xx[(0, 1, 1)]=+(0.545515)*s[0]**2+(-0.638297)*s[0]+(0.683289)*s[1]**2+(-0.692061)*s[1]+(-0.006707)*s[2]**2+(0.524839)*s[2]
            xx[(0, 1, 2)]=+(0.514400)*s[0]**2+(0.750890)*s[0]+(-0.589287)*s[1]**2+(-0.131316)*s[1]+(-0.066467)*s[2]**2+(0.594723)*s[2]
            xx[(0, 1, 3)]=+(-0.992882)*s[0]**2+(0.712724)*s[0]+(-0.999006)*s[1]**2+(0.404830)*s[1]+(0.256593)*s[2]**2+(0.160831)*s[2]
            xx[(0, 1, 4)]=+(-0.268123)*s[0]**2+(0.100398)*s[0]+(-0.697916)*s[1]**2+(-0.653827)*s[1]+(0.534263)*s[2]**2+(0.306629)*s[2]
            xx[(0, 1, 5)]=+(0.442216)*s[0]**2+(0.217248)*s[0]+(-0.938359)*s[1]**2+(-0.712427)*s[1]+(0.243449)*s[2]**2+(-0.410570)*s[2]
            xx[(0, 2, 0)]=+(0.976640)*s[0]**2+(-0.945681)*s[0]+(0.840273)*s[1]**2+(0.104058)*s[1]+(-0.481594)*s[2]**2+(0.651283)*s[2]
            xx[(0, 2, 1)]=+(-0.550890)*s[0]**2+(-0.975264)*s[0]+(0.321281)*s[1]**2+(0.597185)*s[1]+(0.012298)*s[2]**2+(-0.304227)*s[2]
            xx[(0, 2, 2)]=+(0.675807)*s[0]**2+(0.983214)*s[0]+(0.942314)*s[1]**2+(0.131601)*s[1]+(-0.728694)*s[2]**2+(-0.298519)*s[2]
            xx[(0, 2, 3)]=+(0.093818)*s[0]**2+(0.629910)*s[0]+(0.295741)*s[1]**2+(-0.466695)*s[1]+(-0.476851)*s[2]**2+(0.083959)*s[2]
            xx[(0, 2, 4)]=+(0.712992)*s[0]**2+(-0.191711)*s[0]+(-0.776600)*s[1]**2+(0.060347)*s[1]+(0.555511)*s[2]**2+(-0.040710)*s[2]
            xx[(0, 2, 5)]=+(-0.697977)*s[0]**2+(0.202946)*s[0]+(-0.986627)*s[1]**2+(-0.780797)*s[1]+(-0.525267)*s[2]**2+(-0.957787)*s[2]
            xx[(1, 0, 0)]=+(-0.640052)*s[0]**2+(0.748486)*s[0]+(-0.543619)*s[1]**2+(-0.429351)*s[1]+(0.474102)*s[2]**2+(0.194770)*s[2]
            xx[(1, 0, 1)]=+(0.241708)*s[0]**2+(0.162958)*s[0]+(-0.331979)*s[1]**2+(0.650908)*s[1]+(-0.070498)*s[2]**2+(0.511214)*s[2]
            xx[(1, 0, 2)]=+(-0.899001)*s[0]**2+(-0.304301)*s[0]+(-0.080992)*s[1]**2+(-0.921244)*s[1]+(-0.144343)*s[2]**2+(0.408883)*s[2]
            xx[(1, 0, 3)]=+(-0.451742)*s[0]**2+(0.954216)*s[0]+(0.755961)*s[1]**2+(0.890942)*s[1]+(0.458222)*s[2]**2+(-0.493359)*s[2]
            xx[(1, 0, 4)]=+(0.786011)*s[0]**2+(0.524130)*s[0]+(0.441984)*s[1]**2+(-0.781258)*s[1]+(-0.308907)*s[2]**2+(0.968001)*s[2]
            xx[(1, 0, 5)]=+(-0.907555)*s[0]**2+(0.354772)*s[0]+(0.454648)*s[1]**2+(0.931370)*s[1]+(0.919888)*s[2]**2+(-0.655259)*s[2]
            xx[(1, 1, 0)]=+(-0.575377)*s[0]**2+(-0.335159)*s[0]+(-0.513871)*s[1]**2+(0.209510)*s[1]+(-0.434989)*s[2]**2+(-0.654398)*s[2]
            xx[(1, 1, 1)]=+(-0.607627)*s[0]**2+(0.549279)*s[0]+(-0.752153)*s[1]**2+(0.852516)*s[1]+(0.745566)*s[2]**2+(-0.149939)*s[2]
            xx[(1, 1, 2)]=+(0.627115)*s[0]**2+(0.464978)*s[0]+(-0.532965)*s[1]**2+(0.891933)*s[1]+(0.475240)*s[2]**2+(0.624608)*s[2]
            xx[(1, 1, 3)]=+(-0.475015)*s[0]**2+(-0.892129)*s[0]+(-0.856667)*s[1]**2+(-0.024541)*s[1]+(0.228789)*s[2]**2+(-0.638960)*s[2]
            xx[(1, 1, 4)]=+(0.417264)*s[0]**2+(0.157211)*s[0]+(0.646854)*s[1]**2+(-0.864047)*s[1]+(-0.649665)*s[2]**2+(0.293813)*s[2]
            xx[(1, 1, 5)]=+(0.892013)*s[0]**2+(0.316968)*s[0]+(0.570551)*s[1]**2+(-0.110217)*s[1]+(-0.258635)*s[2]**2+(0.277133)*s[2]
            xx[(1, 2, 0)]=+(0.332654)*s[0]**2+(0.194506)*s[0]+(0.844146)*s[1]**2+(0.164964)*s[1]+(0.912835)*s[2]**2+(0.354085)*s[2]
            xx[(1, 2, 1)]=+(0.214030)*s[0]**2+(0.072160)*s[0]+(0.847771)*s[1]**2+(0.053716)*s[1]+(0.783697)*s[2]**2+(0.304542)*s[2]
            xx[(1, 2, 2)]=+(0.384244)*s[0]**2+(-0.617640)*s[0]+(-0.277350)*s[1]**2+(-0.886067)*s[1]+(0.387857)*s[2]**2+(0.798371)*s[2]
            xx[(1, 2, 3)]=+(0.107778)*s[0]**2+(0.836303)*s[0]+(-0.989550)*s[1]**2+(-0.775673)*s[1]+(0.027213)*s[2]**2+(0.037377)*s[2]
            xx[(1, 2, 4)]=+(0.271074)*s[0]**2+(0.037011)*s[0]+(-0.364727)*s[1]**2+(0.359738)*s[1]+(0.783211)*s[2]**2+(-0.923707)*s[2]
            xx[(1, 2, 5)]=+(-0.775950)*s[0]**2+(-0.189329)*s[0]+(-0.152899)*s[1]**2+(-0.689158)*s[1]+(0.317313)*s[2]**2+(-0.430153)*s[2]
            xx[(2, 0, 0)]=+(0.302088)*s[0]**2+(-0.469271)*s[0]+(-0.108453)*s[1]**2+(-0.021785)*s[1]+(0.416413)*s[2]**2+(-0.426322)*s[2]
            xx[(2, 0, 1)]=+(0.853032)*s[0]**2+(-0.913372)*s[0]+(0.041329)*s[1]**2+(-0.447264)*s[1]+(-0.268919)*s[2]**2+(0.021217)*s[2]
            xx[(2, 0, 2)]=+(-0.172605)*s[0]**2+(0.812742)*s[0]+(0.477553)*s[1]**2+(-0.417284)*s[1]+(-0.577712)*s[2]**2+(-0.525548)*s[2]
            xx[(2, 0, 3)]=+(-0.953413)*s[0]**2+(0.109997)*s[0]+(-0.000185)*s[1]**2+(0.856666)*s[1]+(-0.619244)*s[2]**2+(0.789468)*s[2]
            xx[(2, 0, 4)]=+(-0.585182)*s[0]**2+(-0.896520)*s[0]+(0.836469)*s[1]**2+(0.740586)*s[1]+(0.732467)*s[2]**2+(-0.686987)*s[2]
            xx[(2, 0, 5)]=+(0.597808)*s[0]**2+(-0.052074)*s[0]+(0.820301)*s[1]**2+(-0.523770)*s[1]+(-0.853784)*s[2]**2+(-0.035122)*s[2]
            xx[(2, 1, 0)]=+(0.912606)*s[0]**2+(-0.192278)*s[0]+(-0.360151)*s[1]**2+(-0.528818)*s[1]+(0.387275)*s[2]**2+(-0.961334)*s[2]
            xx[(2, 1, 1)]=+(-0.296133)*s[0]**2+(0.738555)*s[0]+(-0.598529)*s[1]**2+(-0.872703)*s[1]+(-0.898730)*s[2]**2+(-0.325618)*s[2]
            xx[(2, 1, 2)]=+(0.498034)*s[0]**2+(-0.977749)*s[0]+(-0.225931)*s[1]**2+(-0.338503)*s[1]+(-0.463655)*s[2]**2+(0.844706)*s[2]
            xx[(2, 1, 3)]=+(-0.898367)*s[0]**2+(0.252574)*s[0]+(0.538103)*s[1]**2+(0.018768)*s[1]+(-0.126026)*s[2]**2+(-0.761573)*s[2]
            xx[(2, 1, 4)]=+(0.794995)*s[0]**2+(-0.516966)*s[0]+(0.941315)*s[1]**2+(-0.582613)*s[1]+(0.189662)*s[2]**2+(-0.832749)*s[2]
            xx[(2, 1, 5)]=+(0.479918)*s[0]**2+(0.440736)*s[0]+(-0.160778)*s[1]**2+(-0.290149)*s[1]+(0.516589)*s[2]**2+(-0.032236)*s[2]
            xx[(2, 2, 0)]=+(0.234409)*s[0]**2+(0.720701)*s[0]+(-0.953224)*s[1]**2+(0.088123)*s[1]+(0.371271)*s[2]**2+(-0.747009)*s[2]
            xx[(2, 2, 1)]=+(0.930631)*s[0]**2+(-0.710470)*s[0]+(0.792530)*s[1]**2+(-0.494428)*s[1]+(-0.237746)*s[2]**2+(-0.172808)*s[2]
            xx[(2, 2, 2)]=+(-0.931212)*s[0]**2+(-0.149843)*s[0]+(0.695545)*s[1]**2+(-0.075549)*s[1]+(0.285642)*s[2]**2+(0.419638)*s[2]
            xx[(2, 2, 3)]=+(0.198343)*s[0]**2+(0.599323)*s[0]+(-0.011312)*s[1]**2+(0.153237)*s[1]+(0.935615)*s[2]**2+(0.229785)*s[2]
            xx[(2, 2, 4)]=+(-0.918994)*s[0]**2+(-0.095060)*s[0]+(-0.299727)*s[1]**2+(0.567973)*s[1]+(0.776279)*s[2]**2+(-0.926348)*s[2]
            xx[(2, 2, 5)]=+(0.736829)*s[0]**2+(-0.527175)*s[0]+(-0.887501)*s[1]**2+(-0.825058)*s[1]+(-0.940681)*s[2]**2+(-0.329166)*s[2]
            
        ref=grad(xx, where=fs1)
        res=Evaluator(y)(x=xx, fs=fs1)
        self.assertAlmostEqual(Lsup(res-ref), 0.0, int(-log10(self.RES_TOL)), "wrong result")
