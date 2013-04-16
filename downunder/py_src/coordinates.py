
##############################################################################
#
# Copyright (c) 2003-2013 by University of Queensland
# http://www.uq.edu.au
#
# Primary Business: Queensland, Australia
# Licensed under the Open Software License version 3.0
# http://www.opensource.org/licenses/osl-3.0.php
#
# Development until 2012 by Earth Systems Science Computational Center (ESSCC)
# Development since 2012 by School of Earth Sciences
#
##############################################################################

"""Functions to deal with coordinate systems"""

__copyright__="""Copyright (c) 2003-2013 by University of Queensland
http://www.uq.edu.au
Primary Business: Queensland, Australia"""
__license__="""Licensed under the Open Software License version 3.0
http://www.opensource.org/licenses/osl-3.0.php"""
__url__="https://launchpad.net/escript-finley"

__all__ = [ 'ReferenceSystem', 'CartesianReferenceSystem', 'GeodeticReferenceSystem', 'SphericalReferenceSystem' , 
             'WGS84ReferenceSystem', 'GRS80ReferenceSystem' , 'SpatialCoordinateTransformation', 
             'GeodeticCoordinateTransformation', 'CartesianCoordinateTransformation']

from esys.escript import unitsSI as U  
from esys.escript import *

class ReferenceSystem(object):
    """
    genetic identifier for coordinate systems.  
    """
    def __init__(self,name='none'):
        """
        initialization of reference system 
        :param name: name of the reference system 
        :type name : ``str``
        """
        self.__name=name
        
    def getName(self):
        """
        return name of the reference system
        """
        return self.__name
        
    def __str__(self):
        return ("%s (id %s)"%(self.getName(),id(self)))
        
    def __eq__(self, other):
        return self.isTheSame(other)

    def __ne__(self, other):
        return not self.isTheSame(other)
        
    def isTheSame(self, other):
        """
        test if argument ''other'' defines the same reference system 
        
        :param other: a second reference system
        :type other: `ReferenceSystem`
        :returns: ``True`` if other defines the same reference system 
        :rtype: ``bool``
        :note: needs to be overwritten by a particular reference system 
        """
        raise NotImplementedError()
      
    def createTransformation(self, domain):
	"""
	creates an appropriate coordinate transformation on a given domain 
	
	:param domain: domain of transformation
	:type domain: `esys.escript.AbstractDomain'
	:rtype: `SpatialCoordinateTransformation`
        :note: needs to be overwritten by a particular reference system 
        """
        raise NotImplementedError()        
        
class CartesianReferenceSystem(ReferenceSystem):
      """
      Identifies the Cartesian coordinate system
      """
      def __init__(self, name="CARTESIAN" ):
	"""
	set up Cartesian coordinate system
	"""
	super(CartesianReferenceSystem, self).__init__(name)	   
	   
      def isTheSame(self, other):
        """
        test of argument ''other'' defines the same reference system 
        
        :param other: a second reference system
        :type other: `ReferenceSystem`
        :returns: ``True`` if ``other`` is a `CartesianReferenceSystem` instance. 
        :rtype: ``bool``
        :note: every two `CartesianReferenceSystem` instances considered being the same.
        """
	return isinstance(other, CartesianReferenceSystem)
      
      def createTransformation(self, domain):
	"""
	creates an appropriate coordinate transformation on a given domain 
	
	:param domain: domain of transformation
	:type domain: `esys.escript.AbstractDomain'
	:rtype: `SpatialCoordinateTransformation`
        """
        return SpatialCoordinateTransformation(domain, reference=self)
    
class GeodeticReferenceSystem(ReferenceSystem):
      """
      Identifies a Geodetic coordinate system 
      """
      def __init__(self, a= 6378137.0 *U.m, f=1/298.257223563, angular_unit=1.*U.DEG, name="WGS84"):
	   """
	   initialize  geodetic reference system
	   :param a: semi-major axis in meter
	   :type a: positive ``double``
	   :param f: flattening 
	   :type f: non-negative ``double``, less than one
	   :param name: name of the reference system
	   :type name: ``str``
	   :param angular_unit: factor to scale the unit of latitude and longitude to radiant.
	   :type  angular_unit: positive ``double``
	   """
	   if not a>0:
	       raise ValueError("length of semi-major axis a must be positive.")
	   if not ( f>=0 and f<1 ):
	       raise ValueError("flattening f must be non-negative and less than one.")
	   if not angular_unit>0:
	       raise ValueError("angular_unit  must be positive.")
	   super(GeodeticReferenceSystem, self).__init__(name)	  
	   self.__a=a
	   self.__f=f
	   self.__angular_unit=angular_unit
	   
      def getAngularUnit(self):
	   """
	   returns the angular unit 
	   """
	   return self.__angular_unit
	   
      def getSemiMajorAxis(self):
           """
	   returns the length of semi major axis
	   """
	   return self.__a
	   
      def getSemiMinorAxis(self):
           """
	   returns the length of semi minor axis
	   """
	   a=self.getSemiMajorAxis()
	   f=self.getFlattening()
	   return a*(1-f)
	   
      def getFlattening(self):
	   """
	   returns the flattening
	   """
	   return self.__f	
	   
      def isTheSame(self, other):
        """
        test of the other defines the same reference system 
        
        :param other: a second reference system
        :type other: `ReferenceSystem`
        :returns: ``True`` if other defines then same reference system 
        :rtype: ``bool``
        :note: two `GeodeticReferenceSystem` are considered to be the same
               if the use the same semi major axis, the same flattening
               and the same angular unit.
        """	
	if isinstance(other, GeodeticReferenceSystem):
	       if     self.getSemiMajorAxis() ==  other.getSemiMajorAxis() \
	         and  self.getFlattening() ==  other.getFlattening() \
	         and  self.getAngularUnit() == other.getAngularUnit():
		    return True
	       else:
		    return False
	else:
	     return False

      def createTransformation(self, domain):
	  """
	  creates an appropriate coordinate transformation on a given domain 
	  
	  :param domain: domain of transformation
	  :type domain:`esys.escript.AbstractDomain'
	  :rtype: `SpatialCoordinateTransformation`
	  """
	  return GeodeticCoordinateTransformation(domain, reference=self)
	
def SphericalReferenceSystem(R=6378137.0 *U.m):
     """
     returns the `GeodeticReferenceSystem` of a sphere
     :param R: sphere radius
     :type R: positive ``double``
     """
     return GeodeticReferenceSystem( a= R, f=0, angular_unit=1*U.DEG, name="SPHERE")
     
def WGS84ReferenceSystem():
     """
     returns the `GeodeticReferenceSystem` for the WGS84 Ellipsoid 
     """
     return GeodeticReferenceSystem( a= 6378137.0 *U.m, f=1/298.257223563, angular_unit=1*U.DEG, name="WGS84")

def GRS80ReferenceSystem():
     """
     returns the `GeodeticReferenceSystem` for the GRS80 Ellipsoid 
     """
     return GeodeticReferenceSystem( a= 6378137.0 *U.m, f=1/298.257222101, angular_unit=1*U.DEG, name="GRS80")   
     

class SpatialCoordinateTransformation(object):
     """
     defines an orthogonal coordinate transformation from a domain into the Cartesian domain using a 
     coordinate transformation.
     
     The default implementation is the transformation is the identity transformation (i.e.
     no changes are applied to the domain). Overwrite the appropriate
     methods to define other coordinate systems.
     """
     def __init__(self, domain, reference=CartesianReferenceSystem() ):
         """
         set up the orthogonal coordinate transformation.
         
         :param domain: domain in the domain of the coordinate transformation  
         :type domain: `esys.escript.AbstractDomain'
         :param reference: the reference system
         :type reference: `ReferenceSystem`
         
         """
         self.__domain = domain
         self.__reference_system=reference
         self._volumefactor=Scalar(1., Function(domain))
         self._scaling_factors = [Scalar(1., Function(domain)) ]*domain.getDim()
         self._is_cartesian = isinstance(reference, CartesianReferenceSystem)
        
     def __eq__(self, other):
	  return self.isTheSame(other)

     def __ne__(self, other):
	  return not self.isTheSame(other)
	  
     def isTheSame(self, other):
	  """
	  test of argument ``other`` defines the same coordinate transformation
	  
	  :param other: a second coordinate transformation
	  :type other: `SpatialCoordinateTransformation`
	  :returns: ``True`` if other defines then same coordinate transformation 
	  :rtype: ``bool``
	  """
	  if isinstance(other, SpatialCoordinateTransformation):
		if     self.getDomain() ==  other.getDomain() \
		  and  self.getReferenceSystem() ==  other.getReferenceSystem() :
		      return True
		else:
		      return False
	  else:
	      return False
      
     def isCartesian(self):
         """
         returns ``True`` if the scaling factors (and the volume factor) are equal to 1 
         :rtype: ``bool``
         """
         return self._is_cartesian
         
     def getDomain(self):
         """
         returns the domain of the coordinate transformation.
         :rtype: `esys.escript.AbstractDomain'
         """
         return self.__domain
         
     def getReferenceSystem(self):
         """
         returns the reference system used to to define the coordinate transformation 
         :rtype: `ReferenceSystem`
         """
         return self.__reference_system
     
     def getVolumeFactor(self):
         """
         returns the volume factor for the coordinate transformation
         :rtype:  `esys.escript.Scalar'
         """
         return self._volumefactor
         
     def getScalingFactors(self):
         """
         returns the scaling factors 
         :rtype: ``list`` of `esys.escript.Scalar'
         """
         return self._scaling_factors

def CartesianCoordinateTransformation(domain, reference=CartesianReferenceSystem() ):
     
     return SpatialCoordinateTransformation(domain, reference) 

class GeodeticCoordinateTransformation(SpatialCoordinateTransformation):
     """
     A geodetic coordinate transformation
     """
     def __init__(self, domain, reference=WGS84ReferenceSystem() ):
         """
         set up the orthogonal coordinate transformation.
         
         :param domain: domain in the domain of the coordinate transformation  
         :type domain: `esys.escript.AbstractDomain'
         :param reference: the reference system
         :type reference: `ReferenceSystem`
         
         """
         DIM=domain.getDim()
         super(GeodeticCoordinateTransformation, self).__init__(domain, reference )
         
         a=reference.getSemiMajorAxis()
         f=reference.getFlattening()
         
         x=Function(domain).getX()
         phi=x[0] * reference.getAngularUnit()
         h=x[DIM-1]
         
         e = sqrt(2*f-f**2)
         N = a/sqrt(1 - e**2 * sin(phi) )
         M = ( a*(1-e**2) ) /sqrt(1 - e**2 * sin(phi) )**3
         v_phi = (M + h)
         v_lam = (N + h) * cos(phi) 
         
         if DIM == 2:
             v= v_phi 
             s = [ 1/v_phi, Scalar(1., Function(domain)) ]
	 else:

             v= v_phi * v_lam
             s = [ 1/v_phi, 1/v_lam, Scalar(1., Function(domain)) ]
             
         self._volumefactor=v
         self._scaling_factors = s