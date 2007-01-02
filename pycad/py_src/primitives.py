# $Id:$

"""
Geometrical Primitives

the concept is inspired by gmsh and very much focused on the fact that
the classes are used to wrk with gmsh.

@var __author__: name of author
@var __copyright__: copyrights
@var __license__: licence agreement
@var __url__: url entry point on documentation
@var __version__: version
@var __date__: date of the version
"""


__author__="Lutz Gross, l.gross@uq.edu.au"
__copyright__="""  Copyright (c) 2006 by ACcESS MNRF
                    http://www.access.edu.au
                Primary Business: Queensland, Australia"""
__license__="""Licensed under the Open Software License version 3.0
             http://www.opensource.org/licenses/osl-3.0.php"""
__url__="http://www.iservo.edu.au/esys/escript"
__version__="$Revision:$"
__date__="$Date:$"

import numarray
from transformations import _TYPE, Translation, Dilation, Transformation


def resetGlobalPrimitiveIdCounter():
   """
   initializes the global primitive ID counter
   """
   global global_primitive_id_counter
   global_primitive_id_counter=1

def setToleranceForColocation(tol=1.e-11):
   """
   set the global tolerance for colocation checks to tol
   """
   global global_tolerance_for_colocation
   global_tolerance_for_colocation=tol

def getToleranceForColocation():
   """
   returns the global tolerance for colocation checks
   """
   return global_tolerance_for_colocation

resetGlobalPrimitiveIdCounter()
setToleranceForColocation()

class Primitive(object):
    """
    template for elementary geometrical object
    """
    def __init__(self): 
       """
       
       """ 
       global global_primitive_id_counter
       self.__ID=global_primitive_id_counter
       global_primitive_id_counter+=1

    def getID(self):
       return self.__ID

    def __repr__(self):
       return "%s(%s)"%(self.__class__.__name__,self.getID())
    def __cmp__(self,other):
       return cmp(self.getID(),other.getID())

    def getConstructionPoints(self):
        """
        returns the points used to construct the primitive
        """
        out=set()
        for i in self.getPrimitives(): 
           if isinstance(i,Point): out.add(i)
        return list(out)

    def getPrimitives(self):
        """
        returns primitives used to construct the primitive
        """
        return []

    def copy(self):
       """
       returns a deep copy of the object
       """
       return Primitive()


    def modifyBy(self,transformation):
       """
       modifies the coordinates by applying a transformation 
       """
       for p in self.getConstructionPoints(): p.modifyBy(transformation)


    def __add__(self,other):
        """
        returns a new object shifted by other
        """
        return self.apply(Translation(numarray.array(other,_TYPE)))

    def __sub__(self,other):
        """
        returns a new object shifted by other
        """
        return self.apply(Translation(-numarray.array(other,_TYPE)))

    def __iadd__(self,other):
        """
        shifts the point by other
        """
        self.modifyBy(Translation(numarray.array(other,_TYPE)))
        return self

    def __isub__(self,other):
        """
        shifts the point by -other
        """
        self.modifyBy(Translation(-numarray.array(other,_TYPE)))
        return self

    def __imul__(self,other):
        """
        modifies object by applying L{Transformation} other. If other is not a L{Transformation} it will try convert it.
        """
        if isinstance(other,int) or isinstance(other,float):
            trafo=Dilation(other)
        elif isinstance(other,numarray.NumArray):
            trafo=Translation(other)
        elif isinstance(other,Transformation):
            trafo=other
        else:
            raise TypeError, "cannot convert argument to Trnsformation class object."
        self.modifyBy(trafo)
        return self

    def __rmul__(self,other):
        """
        applies L{Transformation} other to object. If other is not a L{Transformation} it will try convert it.
        """
        if isinstance(other,int) or isinstance(other,float):
            trafo=Dilation(other)
        elif isinstance(other,numarray.NumArray):
            trafo=Translation(other)
        elif isinstance(other,Transformation):
            trafo=other
        else:
            raise TypeError, "cannot convert argument to Transformation class object."
        return self.apply(trafo)

    def __neg__(self):
        return ReversedPrimitive(self)

    def setLocalScale(self,factor=1.):
       """
       sets the local refinement factor
       """
       for p in self.getConstructionPoints(): p.setLocalScale(factor)

    def getGmshCommand(self, local_scaling_factor=1.):
        """
        returns the Gmsh command(s) to create the primitive
        """
        raise NotImplementedError("getGmshCommand is not implemented for this class %s."%self.__class__.__name__)

    def apply(self,transformation):
        """
        returns a new object by applying the transformation
        """
        raise NotImplementedError("apply is not implemented for this class %s."%self.__class__.__name__)

    def isColocated(self,primitive):
       """
       returns True is the two primitives are located at the smae position
       """
       raise NotImplementedError("isColocated is not implemented for this class %s."%self.__class__.__name__)

class Point(Primitive):
    """
    a three dimensional point
    """
    def __init__(self,x=0.,y=0.,z=0.,local_scale=1.): 
       """
       creates a point with coorinates x,y,z with the local refinement factor local_scale
       """ 
       super(Point, self).__init__()
       self.setCoordinates(numarray.array([x,y,z],_TYPE))
       self.setLocalScale(local_scale)

    def setLocalScale(self,factor=1.):
       """
       sets the local refinement factor
       """
       if factor<=0.:
          raise ValueError("scaling factor must be positive.")
       self.__local_scale=factor
    def getLocalScale(self):
       """
       returns the local refinement factor
       """
       return self.__local_scale
    def getCoordinates(self):
       """
       returns the coodinates of the point as L{numarray.NumArray} object
       """
       return self._x
    def setCoordinates(self,x):
       """
       returns the coodinates of the point as L{numarray.NumArray} object
       """
       if not isinstance(x, numarray.NumArray):
          self._x=numarray.array(x,_TYPE)
       else:
          self._x=x

    def getPrimitives(self):
       """
       returns primitives used to construct the primitive
       """
       return [self]
 
    def isColocated(self,primitive):
       """
       returns True if L{Point} primitive is colocation (same coordinates) 
       that means if |self-primitive| <= tol * max(|self|,|primitive|)
       """
       if isinstance(primitive,Point):
          primitive=primitive.getCoordinates()
          c=self.getCoordinates()
          d=c-primitive
          return numarray.dot(d,d)<=getToleranceForColocation()**2*max(numarray.dot(c,c),numarray.dot(primitive,primitive))
       else:
          return False

    def copy(self):
       """
       returns a deep copy of the point
       """
       c=self.getCoordinates()
       return Point(c[0],c[1],c[2],local_scale=self.getLocalScale())

    def modifyBy(self,transformation):
        """
        modifies the coordinates by applying a transformation 
        """
        self.setCoordinates(transformation(self.getCoordinates()))

    def apply(self,transformation):
        """
        returns a new L{Point} by applying the transformation
        """
        new_p=self.copy()
        new_p.modifyBy(transformation)
        return new_p

    def getGmshCommand(self, local_scaling_factor=1.):
        """
        returns the Gmsh command(s) to create the primitive
        """
        c=self.getCoordinates()
        return "Point(%s) = {%s , %s, %s , %s };"%(self.getID(),c[0],c[1],c[2], self.getLocalScale()*local_scaling_factor)

class Primitive1D(Primitive):
      """
      general one-dimensional primitive
      """
      def __init__(self,*args):
          """
          create a one-dimensional primitive
          """
          super(Primitive1D, self).__init__()
      
class Curve(Primitive1D):
    """
    a curve defined through a list of control points. 
    """
    def __init__(self,*points):
          """
          defines a curve form control points 
          """
          if len(points)<2:
             raise TypeError("Curve needs at least two points")
          super(Curve, self).__init__()
          i=0
          for p in points:
              i+=1
              if not isinstance(p,Point): raise TypeError("%s-th argument is not a Point object."%i)
          self.__points=points

    def __len__(self):
          """
          returns the number of control points
          """
          return len(self.__points)

    def getStartPoint(self):
         """
         returns start point
         """
         return self.__points[0]

    def getEndPoint(self):
         """
         returns end point
         """
         return self.__points[-1]

    def getControlPoints(self):
         """
         returns a list of the points
         """
         return self.__points

    def getPrimitives(self):
       """
       returns primitives used to construct the Curve
       """
       out=set()
       for p in self.getControlPoints(): out|=set(p.getPrimitives())
       out.add(self)
       return list(out)

    def copy(self):
       """
       returns a deep copy
       """
       new_p=[]
       for p in self.getControlPoints(): new_p.append(p.copy())
       return self.__class__(*tuple(new_p))


    def apply(self,transformation):
        """
        applies transformation
        """
        new_p=[]
        for p in self.getControlPoints(): new_p.append(p.apply(transformation))
        return self.__class__(*tuple(new_p))

    def isColocated(self,primitive):
       """
       returns True curves are on the same position
       """
       if isinstance(primitive,self.__class__):
          if len(primitive) == len(self):
             cp0=self.getControlPoints()
             cp1=primitive.getControlPoints()
             for i in range(len(cp0)):
                if not cp0[i].isColocated(cp1[i]):
                   return False
             return True
          else:
             return False
       else:
          return False

class Spline(Curve):
    """
    a spline curve defined through a list of control points. 
    """
    def getGmshCommand(self):
        """
        returns the Gmsh command(s) to create the Curve
        """
        out=""
        for i in self.getControlPoints():
            if len(out)>0: 
                out+=", %s"%i.getID()
            else:
                out="%s"%i.getID()
        return "Spline(%s) = {%s};"%(self.getID(),out)
    

class BezierCurve(Curve):
    """
    a Bezier curve
    """
    def getGmshCommand(self):
        """
        returns the Gmsh command(s) to create the Curve
        """
        out=""
        for i in self.getControlPoints():
            if len(out)>0: 
                out+=", %s"%i.getID()
            else:
                out="%s"%i.getID()
        return "Bezier(%s) = {%s};"%(self.getID(),out)

class BSpline(Curve):
    """
    a BSpline curve. Control points may be repeated.
    """
    def getGmshCommand(self):
        """
        returns the Gmsh command(s) to create the Curve
        """
        out=""
        for i in self.getControlPoints():
            if len(out)>0: 
                out+=", %s"%i.getID()
            else:
                out="%s"%i.getID()
        return "BSpline(%s) = {%s};"%(self.getID(),out)

class Line(Curve):
    """
    a line is defined by two L{Point}s
    """
    def __init__(self,*points):
        """
        defines a line with start and end point
        """
        if len(points)!=2:
           raise TypeError("Line needs two points")
        super(Line, self).__init__(*points)
    def getGmshCommand(self):
        """
        returns the Gmsh command(s) to create the Curve
        """
        return "Line(%s) = {%s, %s};"%(self.getID(),self.getStartPoint().getID(),self.getEndPoint().getID())


class Arc(Primitive1D):
    """
    defines an arc which is strictly, smaller than Pi
    """
    def __init__(self,center,start,end):
       """
       creates an arc by the start point, end point and center
       """
       if not isinstance(center,Point): raise TypeError("center needs to be a Point object.")
       if not isinstance(end,Point): raise TypeError("end needs to be a Point object.")
       if not isinstance(start,Point): raise TypeError("start needs to be a Point object.")
       # TODO: check length of circle.
       super(Arc, self).__init__()
       self.__center=center
       self.__start=start
       self.__end=end

    def getStartPoint(self):
       """
       returns start point
       """
       return self.__start

    def getEndPoint(self):
       """
       returns end point
       """
       return self.__end

    def getCenterPoint(self):
       """
       returns center
       """
       return self.__center

    def getPrimitives(self):
       """
       returns the primitives used to construct the Curve
       """
       out=set()
       out|=set(self.getStartPoint().getPrimitives())
       out|=set(self.getEndPoint().getPrimitives())
       out|=set(self.getCenterPoint().getPrimitives())
       out.add(self)
       return list(out)

    def getGmshCommand(self):
       """
       returns the Gmsh command(s) to create the primitive
       """
       return "Circle(%s) = {%s, %s, %s};"%(self.getID(),self.getStartPoint().getID(),self.getCenterPoint().getID(),self.getEndPoint().getID())

    def copy(self):
       """
       returns a deep copy
       """
       return Arc(self.getCenterPoint().copy(),self.getStartPoint().copy(),self.getEndPoint().copy())

    def apply(self,transformation):
        """
        applies transformation
        """
        return Arc(self.getCenterPoint().apply(transformation),self.getStartPoint().apply(transformation),self.getEndPoint().apply(transformation))

    def isColocated(self,primitive):
       """
       returns True curves are on the same position
       """
       if isinstance(primitive,Arc):
            if not self.getCenterPoint().isColocated(primitive.getCenterPoint()): return False
            if not self.getEndPoint().isColocated(primitive.getEndPoint()): return False
            if not self.getStartPoint().isColocated(primitive.getStartPoint()): return False
            return True
       else:
          return False

#=================================================================================================================================
class CurveLoop(Primitive):
    """
    An oriented loop of curves. 

    The loop must be closed and the L{Curves}s should be oriented consistently.
    """
    def __init__(self,*curves):
       """
       creates a polygon from a list of line curves. The curves must form a closed loop.
       """
       super(CurveLoop, self).__init__()
       self.__curves=[]
       self.addCurve(*curves)
    def addCurve(self,*curves):
       for i in range(len(curves)):
           if not curves[i].isCurve():
              raise TypeError("%s-th argument is not a Curve object."%i)
       self.__curves+=curves

    def isCurveLoop(self):
        return True
    def getCurves(self):
       return self.__curves
    def __add__(self,other):
       return CurveLoop(*tuple([c+other for c in self.getCurves()[::-1]]))
    def __len__(self):
       return len(self.__curves)
    def getPrimitives(self):
          out=set([self])
          for i in self.getCurves(): out|=i.getPrimitives()
          return out
    def getConstructionPoints(self):
          out=set()
          for i in self.getCurves(): out|=i.getConstructionPoints()
          return out
    def getGmshCommand(self):
        out=""
        for i in self.getCurves():
            if len(out)>0: 
                out+=", %s"%i.getID()
            else:
                out="%s"%i.getID()
        return "Line Loop(%s) = {%s};"%(self.getID(),out)

class Surface(Primitive):
    """
    a surface
    """
    def __init__(self,loop):
       """
       creates a  surface with boundary loop

       @param loop: L{CurveLoop} defining the boundary of the surface
       """
       super(Surface, self).__init__()
       if not loop.isCurveLoop():
           raise TypeError("argument loop needs to be a CurveLoop object.")
       self.__loop=loop
    def isSurface(self):
        return True
    def getBoundaryLoop(self):
       return self.__loop
    def __add__(self,other):
       return Surface(self.getBoundaryLoop()+other)
    def getPrimitives(self):
        out=set([self]) | self.getBoundaryLoop().getPrimitives()
        return out
    def getConstructionPoints(self):
        return self.getBoundaryLoop().getConstructionPoints()
    def getGmshCommand(self):
        return "Ruled Surface(%s) = {%s};"%(self.getID(),self.getBoundaryLoop().getID())

class PlaneSurface(Surface):
    """
    a plane surface with holes
    """
    def __init__(self,loop,holes=[]):
       """
       creates a  plane surface. 

       @param loop: L{CurveLoop} defining the boundary of the surface
       @param holes: list of L{CurveLoop} defining holes in the surface. 
       @note: A CurveLoop defining a hole should not have any lines in common with the exterior CurveLoop.  
              A CurveLoop defining a hole should not have any lines in common with another CurveLoop defining a hole in the same surface.
       """
       super(PlaneSurface, self).__init__(loop)
       for i in range(len(holes)):
            if not holes[i].inCurveLoop():
                 raise TypeError("%i th hole needs to be a CurveLoop object.")
       self.__holes=holes
    def getHoles(self):
       return self.__holes
    def __add__(self,other):
       return PlaneSurface(self.getBoundaryLoop()+other, holes=[h+other for h in self.getHoles()])
    def getPrimitives(self):
        out=set([self]) | self.getBoundaryLoop().getPrimitives()
        for i in self.getHoles(): out|=i.getPrimitives()
        return out
    def getConstructionPoints(self):
        out=self.getBoundaryLoop().getConstructionPoints()
        for i in self.getHoles(): out|=i.getConstructionPoints()
        return out
    def getGmshCommand(self):
        out=""
        for i in self.getHoles():
            if len(out)>0: 
                out+=", %s"%i.getID()
            else:
                out="%s"%i.getID()
        if len(out)>0:
          return "Plane Surface(%s) = {%s, %s};"%(self.getID(),self.getBoundaryLoop().getID(), out)
        else:
          return "Plane Surface(%s) = {%s};"%(self.getID(),self.getBoundaryLoop().getID())

class RuledSurface(Surface):
    """
   A ruled surface, i.e., a surface that can be interpolated using transfinite interpolation
    """
    def __init__(self,loop):
       """
       creates a ruled surface from a 

       @param loop: L{CurveLoop} defining the boundary of the surface. There is a restriction of composed of either three or four L{Curve} objects.
       """
       if not loop.isCurveLoop():
           raise TypeError("argument loop needs to be a CurveLoop object.")
       if len(loop)<3:
           raise TypeError("the loop must contain at least three Curves.")
       super(RuledSurface, self).__init__(loop)
    def __add__(self,other):
       return RuledSurface(self.getBoundaryLoop()+other)
    def getGmshCommand(self):
        return "Ruled Surface(%s) = {%s};"%(self.getID(),self.getBoundaryLoop().getID())

class SurfaceLoop(Primitive):
    """
    a surface loop. It defines the shell of a volume. 

    The loop must represent a closed shell, and the L{Surface}s should be oriented consistently.
    """
    def __init__(self,*surfaces):
       """
       creates a surface loop
       """
       super(SurfaceLoop, self).__init__()
       self.__surfaces=[]
       self.addSurface(*surfaces)
    def addSurface(self,*surfaces):
       for i in range(len(surfaces)):
           if not surfaces[i].isSurface():
              raise TypeError("%s-th argument is not a Surface object."%i)
       self.__surfaces+=surfaces

    def isSurfaceLoop(self):
        return True
    def getSurfaces(self):
       return self.__surfaces
    def __add__(self,other):
       return SurfaceLoop([c+other for c in self.getSurfaces])
    def __len__(self):
       return len(self.__surfaces)
    def getPrimitives(self):
          out=set([self])
          for i in self.getSurfaces(): out|=i.getPrimitives()
          return out
    def getConstructionPoints(self):
          out=set()
          for i in self.getSurfaces(): out|=i.getConstructionPoints()
          return out
    def getGmshCommand(self):
        out=""
        for i in self.getSurfaces():
            if len(out)>0: 
                out+=", %s"%i.getID()
            else:
                out="%s"%i.getID()
        return "Surface Loop(%s) = {%s};"%(self.getID(),out)

class Volume(Primitive):
    """
    a volume with holes.
    """
    def __init__(self,loop,holes=[]):
       """
       creates a volume

       @param loop: L{SurfaceLoop} defining the boundary of the surface
       @param holes: list of L{SurfaceLoop} defining holes in the surface. 
       @note: A SurfaceLoop defining a hole should not have any surfaces in common with the exterior SurfaceLoop.  
              A SurfaceLoop defining a hole should not have any surfaces in common with another SurfaceLoop defining a hole in the same volume.
       """
       super(Volume, self).__init__()
       if not loop.isSurfaceLoop():
           raise TypeError("argument loop needs to be a SurfaceLoop object.")
       for i in range(len(holes)):
            if not holes[i].isSurfaceLoop():
                 raise TypeError("%i th hole needs to be a SurfaceLoop object.")
       self.__loop=loop
       self.__holes=holes
    def getHoles(self):
       return self.__holes
    def getSurfaceLoop(self):
       return self.__loop
    def __add__(self,other):
       return Volume(self.getSurfaceLoop()+other, holes=[h+other for h in self.getHoles()])
    def getPrimitives(self):
        out=set([self]) | self.getSurfaceLoop().getPrimitives()
        for i in self.getHoles(): out|=i.getPrimitives()
        return out
    def getConstructionPoints(self):
        out=self.getSurfaceLoop().getConstructionPoints()
        for i in self.getHoles(): out|=i.Points()
        return out
    def getGmshCommand(self):
        out=""
        for i in self.getHoles():
            if len(out)>0: 
                out+=", %s"%i.getID()
            else:
                out="%s"%i.getID()
        if len(out)>0:
          return "Volume(%s) = {%s, %s};"%(self.getID(),self.getSurfaceLoop().getID(), out)
        else:
          return "Volume(%s) = {%s};"%(self.getID(),self.getSurfaceLoop().getID())

class ReversedPrimitive(object):
    def __init__(self,prim):
       self.__prim=prim
    def __getattr__(self,name):
       if name == "getID":
          return self.getReverseID
       else:
          return getattr(self.__prim,name)
    def getReverseID(self):
        return -self.__prim.getID()

class PropertySet(Primitive):
    """
    defines a group L{Primitive} objects. 
    """
    def __init__(self,tag=None,*items):
       super(PropertySet, self).__init__()
       self.__items=items
       self.__tag=tag
    def getPrimitives(self):
        out=set([self, self.getBoundaryLoop().getPrimitives()])
        for i in self.getHoles(): out|=i.getPrimitives()
        return out

class PrimitiveStack(object):
      def __init__(self,*items):
        self.__prims=set()
        for i in items:
            self.__prims|=i.getPrimitives()
        self.__prims=list(self.__prims)
        self.__prims.sort()

      def getGmshCommands(self):
        out=""
        for i in self.__prims:
           out+=i.getGmshCommand()+"\n"
        return out
