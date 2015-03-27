
##############################################################################
#
# Copyright (c) 2003-2015 by University of Queensland
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

"""Cost functions for inversions with one or more forward models"""

__copyright__="""Copyright (c) 2003-2015 by University of Queensland
http://www.uq.edu.au
Primary Business: Queensland, Australia"""
__license__="""Licensed under the Open Software License version 3.0
http://www.opensource.org/licenses/osl-3.0.php"""
__url__="https://launchpad.net/escript-finley"

__all__ = [ 'SplitInversionCostFunction']

from .costfunctions import MeteredCostFunction
from .mappings import Mapping
from .forwardmodels import ForwardModel
from esys.escript.pdetools import ArithmeticTuple
from esys.escript import Data, inner, addJobPerWorld, FunctionJob
import numpy as np


class SplitInversionCostFunction(MeteredCostFunction):
    """
    Class to define cost function *J(m)* for inversion with one or more
    forward models based on a multi-valued level set function *m*:

    *J(m) = J_reg(m) + sum_f mu_f * J_f(p)*

    where *J_reg(m)* is the regularization and cross gradient component of the
    cost function applied to a level set function *m*, *J_f(p)* are the data
    defect cost functions involving a physical forward model using the
    physical parameter(s) *p* and *mu_f* is the trade-off factor for model f.

    A forward model depends on a set of physical parameters *p* which are
    constructed from components of the level set function *m* via mappings.

    Example 1 (single forward model):
         m=Mapping()
         f=ForwardModel()
         J=InversionCostFunction(Regularization(), m, f)

    Example 2 (two forward models on a single valued level set)
         m0=Mapping()
         m1=Mapping()
         f0=ForwardModel()
         f1=ForwardModel()

         J=InversionCostFunction(Regularization(), mappings=[m0, m1], forward_models=[(f0, 0), (f1,1)])

    Example 3 (two forward models on 2-valued level set)
         m0=Mapping()
         m1=Mapping()
         f0=ForwardModel()
         f1=ForwardModel()

         J=InversionCostFunction(Regularization(self.numLevelSets=2), mappings=[(m0,0), (m1,0)], forward_models=[(f0, 0), (f1,1)])

    :cvar provides_inverse_Hessian_approximation: if true the class provides an
          approximative inverse of the Hessian operator.
    """
    provides_inverse_Hessian_approximation=True

    # Original params for InversionCostFunction
    # regularization --- need to feed in later
    # mappings       --- could take a domain so need to be created later
    # forward_models --- need to be split (do we have a function which takes a parameter to indicate which model to create?)
    #
    # New constructor
    # num args, who many of each type
    # splitw is the splitworld jobs are running on
    # worldsinit_fn is run on each world at startup
    def __init__(self, numLevelSets, numModels, numMappings, sw, worldsinit_fn):
        """
        fill this in.
        """
        super(SplitInversionCostFunction, self).__init__()
        if numModels<1 or numModels<1 or numMappings<1:
          raise ValueError("The inversion function requires at least one LevelSet, Mapping and Models.")
        self.numModels=numModels
        self.numMappings=numMappings
        self.numLevelSets=numLevelSets
        # sanity check
        addJobPerWorld(sw, FunctionJob, worldsinit_fn)
        sw.runJobs()
        reqd=["models", "regularization", "mu_model","mappings"]
        knownvars=sw.getVarList()
        print(knownvars)
        for n in reqd:
          if [n,True] not in knownvars:
            raise RuntimeError("Required variable "+n+" was not created by the world init function")
        self.configured=False

    # Function to put the (possible list of) forward model(s) into the form expected by the rest of the system
    @staticmethod
    def formatModels(forward_models, numMappings):
        if isinstance(forward_models, ForwardModel):
            forward_models = [ forward_models ]
        result=[]
        for i in range(len(forward_models)):
            print("Doing iteration "+str(i))
            f=forward_models[i]
            if isinstance(f, ForwardModel):
                idx=[0]
                fm=f
            elif len(f) == 1:
                idx=[0]
                fm=f[0]
            else:
                if isinstance(f[1],int):
                    idx=[f[1]]
                else:
                    idx=list(f[1])
                for k in idx:
                    if k<0 or k> numMappings:
                        raise ValueError("mapping index %s in model %s is out of range."%(k,i))
                fm=f[0]
            result.append((fm,idx))
        return result      
      
    def getDomain(self):
        """
        returns the domain of the cost function

        :rtype: `Domain`
        """
        raise RuntimeError("Can't extract domains for SplitInversionCostFunctions")

    def getNumTradeOffFactors(self):
        """
        returns the number of trade-off factors being used including the
        trade-off factors used in the regularization component.

        :rtype: ``int``
        """
        if self.configured:
           return self.__num_tradeoff_factors
        else:
          raise RuntimeError("This inversion function has not been configured yet")

    def getForwardModel(self, idx=None):
        """
        returns the *idx*-th forward model.

        :param idx: model index. If cost function contains one model only `idx`
                    can be omitted.
        :type idx: ``int``
        """
        raise RuntimeError("Can't extract forward models for SplitInversionCostFunctions")

    def getRegularization(self):
        """
        returns the regularization

        :rtype: `Regularization`
        """
        if self.configured:
           return self.regularization
        else:
          raise RuntimeError("This inversion function has not been configured yet")

    def setTradeOffFactorsModels(self, mu=None):
        """
        sets the trade-off factors for the forward model components.

        :param mu: list of the trade-off factors. If not present ones are used.
        :type mu: ``float`` in case of a single model or a ``list`` of
                  ``float`` with the length of the number of models.
        """
        if mu==None:
            self.mu_model=np.ones((self.numModels, ))
        else:
            if self.numModels > 1:
                mu=np.asarray(mu, dtype=float)
                if min(mu) > 0:
                    self.mu_model= mu
                else:
                    raise ValueError("All values for trade-off factor mu must be positive.")
            else:
                mu=float(mu)
                if mu > 0:
                    self.mu_model= [mu, ]
                else:
                    raise ValueError("Trade-off factor must be positive.")
        #Now we need to get these values into the subworlds
        #Getting the mu value in via a closure is safe because it WILL NOT CONTAIN COMPLEX OBJECTS
        extmu=self.mu_model
        def setMu(self, **args):
          chunksize=max(self.worldsize()//len(extmu),1)         #In case we have more worlds than models
          minindex=self.subworldid()*chunksize
          maxindex=(self.subworldid()+1)*chunksize              # yes this could go off the end but I will slice
          mymu=extmu[minindex:maxindex]
          self.exportValue("mu_model", mymu)
        addJobPerWorld(sw, setMu)
        sw.runJobs()
        
    def getTradeOffFactorsModels(self):
        """
        returns the trade-off factors for the forward models

        :rtype: ``float`` or ``list`` of ``float``
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")
        if self.numModels>1:
            return self.mu_model
        else:
            return self.mu_model[0]

    def setTradeOffFactorsRegularization(self, mu=None, mu_c=None):
        """
        sets the trade-off factors for the regularization component of the
        cost function, see `Regularization` for details.

        :param mu: trade-off factors for the level-set variation part
        :param mu_c: trade-off factors for the cross gradient variation part
        """
        self.regularization.setTradeOffFactorsForVariation(mu)
        self.regularization.setTradeOffFactorsForCrossGradient(mu_c)

    def setTradeOffFactors(self, mu=None):
        """
        sets the trade-off factors for the forward model and regularization
        terms.

        :param mu: list of trade-off factors.
        :type mu: ``list`` of ``float``
        """
        if mu is None:
            mu=np.ones((self.__num_tradeoff_factors,))
        self.setTradeOffFactorsModels(mu[:self.numModels])
        self.regularization.setTradeOffFactors(mu[self.numModels:])

    def getTradeOffFactors(self, mu=None):
        """
        returns a list of the trade-off factors.

        :rtype: ``list`` of ``float``
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")        
        mu1=self.getTradeOffFactorsModels(mu[:self.numModels])
        mu2=self.regularization.getTradeOffFactors()
        return [ m for m in mu1] + [ m for m in mu2]

    def createLevelSetFunction(self, *props):
        """
        returns an instance of an object used to represent a level set function
        initialized with zeros. Components can be overwritten by physical
        properties `props`. If present entries must correspond to the
        `mappings` arguments in the constructor. Use ``None`` for properties
        for which no value is given.
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")        
        #Since this involves solving a PDE (and therefore a domain, it must be kept local
        #to each subworld
        raise RuntimeError("This needs to run inside the subworld --- create a function for it")
        m=self.regularization.getPDE().createSolution()
        if len(props) > 0:
            for i in range(self.numMappings):
                if props[i]:
                    mp, idx=self.mappings[i]
                    m2=mp.getInverse(props[i])
                    if idx:
                        if len(idx) == 1:
                            m[idx[0]]=m2
                        else:
                            for k in range(idx): m[idx[k]]=m2[k]
                    else:
                        m=m2
        #return m

    def getProperties(self, m, return_list=False):
        """
        returns a list of the physical properties from a given level set
        function *m* using the mappings of the cost function.

        :param m: level set function
        :type m: `Data`
        :param return_list: if ``True`` a list is returned.
        :type return_list: ``bool``
        :rtype: ``list`` of `Data`
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")        
        #Since this involves solving a PDE (and therefore a domain, it must be kept local
        #to each subworld
        raise RuntimeError("This needs to run inside the subworld --- create a function for it")        
        props=[]
        for i in range(self.numMappings):
            mp, idx=self.mappings[i]
            if idx:
                if len(idx)==1:
                    p=mp.getValue(m[idx[0]])
                else:
                    m2=Data(0.,(len(idx),),m.getFunctionSpace())
                    for k in range(len(idx)): m2[k]=m[idx[k]]
                    p=mp.getValue(m2)
            else:
                p=mp.getValue(m)
            props.append(p)
        #if self.numMappings > 1 or return_list:
            #return props
        #else:
            #return props[0]

    def _getDualProduct(self, x, r):
        """
        Returns the dual product, see `Regularization.getDualProduct`

        :type x: `Data`
        :type r: `ArithmeticTuple`
        :rtype: ``float``
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")        
        #This involves an 'x' which is a Data object.
        #Does this only need to run on one subworld?
        #Then shipped around using getDoubleValues?
        raise RuntimeError("Still need to work this one out")        
        return self.regularization.getDualProduct(x, r)

    
    def setPoint(self):
      self.setPoint()
    
    def _setPoint(self):
      """
      This should take in a value to set the point to, but that can wait
      """
      if not self.configured:
        raise ValueError("This inversion function has not been configured yet")

      def load_initial_guess(self, **args):
          initguess=0
          mods=self.importValue("models")
          reg=self.importValue("regularization")
          reg.setPoint(initguess)
          for m,idx in mods:
            pp=tuple( [props[k] for k in idx] ) # build up collection of properties used by this model
            m.setPoint(*pp)
          # We still need to deal with the props feild and where to get it from
          self.exportValue("props", props)
            
      for i in range(0, self.sw.swcount):      
          self.sw.addJob(FunctionJob, load_initial_guess, imports=["models", "regularization"])
      self.sw.runJobs()
      
    def _getArguments(self, m):
        """
        returns pre-computed values that are shared in the calculation of
        *J(m)* and *grad J(m)*. In this implementation returns a tuple with the
        mapped value of ``m``, the arguments from the forward model and the
        arguments from the regularization.

        :param m: current approximation of the level set function
        :type m: `Data`
        :return: tuple of of values of the parameters, pre-computed values
                 for the forward model and pre-computed values for the
                 regularization
        :rtype: ``tuple``
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")         
        raise RuntimeError("Call to getArguments -- temporary block to see where this is used")
        args_reg=self.regularization.getArguments(m)
        # cache for physical parameters:
        props=self.getProperties(m, return_list=True)
        args_f=[]
        for i in range(self.numModels):
            f, idx=self.forward_models[i]
            pp=tuple( [ props[k] for k in idx] )
            aa=f.getArguments(*pp)
            args_f.append(aa)

        return props, args_f, args_reg

    def calculateValue(self, vnames):
        self.calculate(vnames)
        
    def _calculateValue(self, vnames):
        
       if not self.configured:
          raise ValueError("This inversion function has not been configured yet")
       #The props is already in each world as a variable
       #Each model already has its point set
       #regularization already has its point set
        
       def calculateValueWorker(self, vnames, **args):
          props=self.importValue("props")
          mods=self.importValue("models")
          reg=self.importValue("regularization")
          mu_model=self.importValue("mu_model")
          
          J=None
          for i in range(len(mods)):    # note: iterating over local models not ones on other worlds
            m,idx=mods[i]
            z=m.getDefectAtPoint()
            z*=self.mu_model[i];   
            if J is None:          
              J=z
            else:
              J+=z
            
          if self.worldid==0:    # we only want to add the regularization term once
            J+=reg.getValueAtPoint()    # We actually want to get a value here but
                                        # I want to distiguish it from the other getValue call
          if isinstance(vnames, str):
            self.exportValue(J, vnames)
          else:
            for n in vnames:
              self.exportValue(J, n)
       for i in range(0, self.sw.swcount):      
          self.sw.addJob(FunctionJob, calculateValueWorker, imports=["models", "regularization", "props"])
       self.sw.runJobs()              

    def _getValue(self, m, *args):
        """
        Returns the value *J(m)* of the cost function at *m*.
        If the pre-computed values are not supplied `getArguments()` is called.

        :param m: current approximation of the level set function
        :type m: `Data`
        :param args: tuple of values of the parameters, pre-computed values
                     for the forward model and pre-computed values for the
                     regularization
        :rtype: ``float``
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")         
        raise RuntimeError("Call to getArguments -- temporary block to see where this is used")        
        if len(args)==0:
            args=self.getArguments(m)

        props=args[0]
        args_f=args[1]
        args_reg=args[2]

        J = self.regularization.getValue(m, *args_reg)
        self.logger.debug("J_R  (incl. trade-offs) = %e"%J)

        for i in range(self.numModels):
            f, idx=self.forward_models[i]
            args=tuple( [ props[k] for k in idx]  + list( args_f[i] ) )
            J_f = f.getDefect(*args)
            self.logger.debug("J_f[%d] = %e, mu_model[%d] = %e"%(i, J_f, i, self.mu_model[i]))
            J += self.mu_model[i] * J_f

        return J

    def getComponentValues(self, m, *args):
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")       
        raise RuntimeError("Call to getComponentValues -- temporary block to see where this is used")      
        return self._getComponentValues(m, *args)

    def _getComponentValues(self, m, *args):
        """
        returns the values of the individual cost functions that make up *f(x)*
        using the precalculated values for *x*.

        :param x: a solution approximation
        :type x: x-type
        :rtype: ``list<<float>>``
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")         
        raise RuntimeError("Call to getArguments -- temporary block to see where this is used")        
        if len(args)==0:
            args=self.getArguments(m)

        props=args[0]
        args_f=args[1]
        args_reg=args[2]

        J_reg = self.regularization.getValue(m, *args_reg)
        result = [J_reg]

        for i in range(self.numModels):
            f, idx=self.forward_models[i]
            args=tuple( [ props[k] for k in idx]  + list( args_f[i] ) )
            J_f = f.getValue(*args)
            self.logger.debug("J_f[%d] = %e, mu_model[%d] = %e"%(i, J_f, i, self.mu_model[i]))

            result += [J_f] # self.mu_model[i] * ??

        return result

    def _calculateGradient(self):
       if not self.configured:
          raise ValueError("This inversion function has not been configured yet")

       numLevelSets=self.numLevelSets   # pass in via closure
       def calculateGradientWorker(self, vnames1, vnames2, **args):
          """
          vnames1 gives the names to store the first component of the gradient in
          vnames2 gives the names to store the second component of the gradient in
          """
          props=self.importValue("props")
          mods=self.importValue("models")
          reg=self.importValue("regularization")
          mu_model=self.importValue("mu_model")
          mappings=self.importValue("mappings")
          
          g_J = reg.getGradientAtPoint()
          p_diffs=[]
          # Find the derivative for each mapping
          # If a mapping has a list of components (idx), then make a new Data object with only those
          # components, pass it to the mapping and get the derivative.
          for i in range(len(numMappings)):
              mm, idx=mappings[i]
              if idx and numLevelSets > 1:
                  if len(idx)>1:
                      m2=Data(0,(len(idx),),m.getFunctionSpace())
                      for k in range(len(idx)): m2[k]=m[idx[k]]
                      dpdm = mm.getDerivative(m2)
                  else:
                      dpdm = mm.getDerivative(m[idx[0]])
              else:
                  dpdm = mm.getDerivative(m)
              p_diffs.append(dpdm)
          #Since we are going to be merging Y with other worlds, we need to make sure the the regularization
          #component is only added once.  However most of the ops below are in terms of += so we need to
          #create a zero object to use as a starting point
          if self.subworldid==0:
             Y=g_J[0]    # Because g_J==(Y,X)  Y_k=dKer/dm_k
          else:
             Y=Data(0, g_J[0].getShape(), g_J[0].getForwardModel())
          for i in range(self.numModels):
              mu=self.mu_model[i]
              f, idx_f=mods[i]
              args=tuple( [ props[k] for k in idx_f]  + list( args_f[i] ) )
              Ys = f.getGradientAtPoint() # this d Jf/d props
              # in this case f depends on one parameter props only but this can
              # still depend on several level set components
              if Ys.getRank() == 0:
                  # run through all level sets k prop j is depending on:
                  idx_m=self.mappings[idx_f[0]][1]
                  # tmp[k] = dJ_f/d_prop * d prop/d m[idx_m[k]]
                  tmp=Ys * p_diffs[idx_f[0]] * mu
                  if idx_m:
                      if tmp.getRank()== 0:
                          for k in range(len(idx_m)):
                              Y[idx_m[k]]+=tmp # dJ_f /d m[idx_m[k]] = tmp
                      else:
                          for k in range(len(idx_m)):
                              Y[idx_m[k]]+=tmp[k] # dJ_f /d m[idx_m[k]] = tmp[k]
                  else:
                      Y+=tmp # dJ_f /d m[idx_m[k]] = tmp
              else:
                  s=0
                  # run through all props j forward model f is depending on:
                  for j in range(len(idx_f)):
                      # run through all level sets k prop j is depending on:
                      idx_m=self.mappings[j][1]
                      if p_diffs[idx_f[j]].getRank() == 0 :
                          if idx_m: # this case is not needed (really?)
                              raise RuntimeError("something wrong A")
                              # tmp[k] = dJ_f/d_prop[j] * d prop[j]/d m[idx_m[k]]
                              tmp=Ys[s]*p_diffs[idx_f[j]] * mu
                              for k in range(len(idx_m)):
                                  Y[idx_m[k]]+=tmp[k] # dJ_f /d m[idx_m[k]] = tmp[k]
                          else:
                              Y+=Ys[s]*p_diffs[idx_f[j]] * mu
                          s+=1
                      elif p_diffs[idx_f[j]].getRank() == 1 :
                          l=p_diffs[idx_f[j]].getShape()[0]
                          # tmp[k]=sum_j dJ_f/d_prop[j] * d prop[j]/d m[idx_m[k]]
                          tmp=inner(Ys[s:s+l], p_diffs[idx_f[j]]) * mu
                          if idx_m:
                              for k in range(len(idx_m)):
                                  Y[idx_m[k]]+=tmp # dJ_f /d m[idx_m[k]] = tmp[k]
                          else:
                              Y+=tmp
                          s+=l
                      else: # rank 2 case
                          l=p_diffs[idx_f[j]].getShape()[0]
                          Yss=Ys[s:s+l]
                          if idx_m:
                              for k in range(len(idx_m)):
                                  # dJ_f /d m[idx_m[k]] = tmp[k]
                                  Y[idx_m[k]]+=inner(Yss, p_diffs[idx_f[j]][:,k])
                          else:
                              Y+=inner(Yss, p_diffs[idx_f[j]]) * mu
                          s+=l    
          if isinstance(vnames1, str):
            self.exportValue(Y, vnames1)
          else:
            for n in vnames1:
              self.exportValue(Y, n)
          if isinstance(vnames2, str):          #The second component should be strictly local 
            self.exportValue(g_J[1], vnames2)
          else:
            for n in vnames2:
              self.exportValue(g_J[1], n)
              
       addJobPerWorld(sw, FunctionJob, calculateGradientWorker, vnames, imports=["models", "regularization", "props", "mu_models"])
       self.sw.runJobs()                 
        
    def _getGradient(self, m, *args):
        """
        returns the gradient of the cost function at *m*.
        If the pre-computed values are not supplied `getArguments()` is called.

        :param m: current approximation of the level set function
        :type m: `Data`
        :param args: tuple of values of the parameters, pre-computed values
                     for the forward model and pre-computed values for the
                     regularization

        :rtype: `ArithmeticTuple`

        :note: returns (Y^,X) where Y^ is the gradient from regularization plus
               gradients of fwd models. X is the gradient of the regularization
               w.r.t. gradient of m.
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")         
        raise RuntimeError("Call to getGradient -- temporary block to see where this is used")        
        if len(args)==0:
            args = self.getArguments(m)

        props=args[0]
        args_f=args[1]
        args_reg=args[2]

        g_J = self.regularization.getGradient(m, *args_reg)
        p_diffs=[]
        # Find the derivative for each mapping
        # If a mapping has a list of components (idx), then make a new Data object with only those
        # components, pass it to the mapping and get the derivative.
        for i in range(self.numMappings):
            mm, idx=self.mappings[i]
            if idx and self.numLevelSets > 1:
                if len(idx)>1:
                    m2=Data(0,(len(idx),),m.getFunctionSpace())
                    for k in range(len(idx)): m2[k]=m[idx[k]]
                    dpdm = mm.getDerivative(m2)
                else:
                    dpdm = mm.getDerivative(m[idx[0]])
            else:
                dpdm = mm.getDerivative(m)
            p_diffs.append(dpdm)

        Y=g_J[0] # Because g_J==(Y,X)  Y_k=dKer/dm_k
        for i in range(self.numModels):
            mu=self.mu_model[i]
            f, idx_f=self.forward_models[i]
            args=tuple( [ props[k] for k in idx_f]  + list( args_f[i] ) )
            Ys = f.getGradient(*args) # this d Jf/d props
            # in this case f depends on one parameter props only but this can
            # still depend on several level set components
            if Ys.getRank() == 0:
                # run through all level sets k prop j is depending on:
                idx_m=self.mappings[idx_f[0]][1]
                # tmp[k] = dJ_f/d_prop * d prop/d m[idx_m[k]]
                tmp=Ys * p_diffs[idx_f[0]] * mu
                if idx_m:
                    if tmp.getRank()== 0:
                        for k in range(len(idx_m)):
                            Y[idx_m[k]]+=tmp # dJ_f /d m[idx_m[k]] = tmp
                    else:
                        for k in range(len(idx_m)):
                            Y[idx_m[k]]+=tmp[k] # dJ_f /d m[idx_m[k]] = tmp[k]
                else:
                    Y+=tmp # dJ_f /d m[idx_m[k]] = tmp
            else:
                s=0
                # run through all props j forward model f is depending on:
                for j in range(len(idx_f)):
                    # run through all level sets k prop j is depending on:
                    idx_m=self.mappings[j][1]
                    if p_diffs[idx_f[j]].getRank() == 0 :
                        if idx_m: # this case is not needed (really?)
                            self.logger.error("something wrong A")
                            # tmp[k] = dJ_f/d_prop[j] * d prop[j]/d m[idx_m[k]]
                            tmp=Ys[s]*p_diffs[idx_f[j]] * mu
                            for k in range(len(idx_m)):
                                Y[idx_m[k]]+=tmp[k] # dJ_f /d m[idx_m[k]] = tmp[k]
                        else:
                            Y+=Ys[s]*p_diffs[idx_f[j]] * mu
                        s+=1
                    elif p_diffs[idx_f[j]].getRank() == 1 :
                        l=p_diffs[idx_f[j]].getShape()[0]
                        # tmp[k]=sum_j dJ_f/d_prop[j] * d prop[j]/d m[idx_m[k]]
                        tmp=inner(Ys[s:s+l], p_diffs[idx_f[j]]) * mu
                        if idx_m:
                            for k in range(len(idx_m)):
                                Y[idx_m[k]]+=tmp # dJ_f /d m[idx_m[k]] = tmp[k]
                        else:
                            Y+=tmp
                        s+=l
                    else: # rank 2 case
                        l=p_diffs[idx_f[j]].getShape()[0]
                        Yss=Ys[s:s+l]
                        if idx_m:
                            for k in range(len(idx_m)):
                                # dJ_f /d m[idx_m[k]] = tmp[k]
                                Y[idx_m[k]]+=inner(Yss, p_diffs[idx_f[j]][:,k])
                        else:
                            Y+=inner(Yss, p_diffs[idx_f[j]]) * mu
                        s+=l
        return g_J

    def _getInverseHessianApproximation(self, m, r, *args):
        """
        returns an approximative evaluation *p* of the inverse of the Hessian
        operator of the cost function for a given gradient type *r* at a
        given location *m*: *H(m) p = r*

        :param m: level set approximation where to calculate Hessian inverse
        :type m: `Data`
        :param r: a given gradient
        :type r: `ArithmeticTuple`
        :param args: tuple of values of the parameters, pre-computed values
                     for the forward model and pre-computed values for the
                     regularization
        :rtype: `Data`
        :note: in the current implementation only the regularization term is
               considered in the inverse Hessian approximation.
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")         
        raise RuntimeError("Call to getInverseHessianApproximation -- temporary block to see where this is used")
        
        m=self.regularization.getInverseHessianApproximation(m, r, *args[2])
        return m

    def updateHessian(self):
        """
        notifies the class that the Hessian operator needs to be updated.
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet")         
        self.regularization.updateHessian()

    def _getNorm(self, m):
        """
        returns the norm of `m`

        :param m: level set function
        :type m: `Data`
        :rtype: ``float``
        """
        if not self.configured:
          raise ValueError("This inversion function has not been configured yet") 
        raise RuntimeError("Need to have this in a subworld --- one or all?")
        return self.regularization.getNorm(m)
